/* AFR2-20261024-01. No browser timing, allocation or automatic resumption. */
#include "afr2_product.h"
#include "afr2_product_wire.h"
#include "afr2_sequence_labels.h"
_Static_assert(sizeof(Afr2Segment)==20,"Sequence record must remain 20 bytes");
_Static_assert(sizeof(Afr2WheelStep)==20,"Pulse records reuse the 640-byte buffer");
uint8_t Afr2Sequence_Available(const Afr2Product *p) {
    for(uint8_t w=0;w<4;w++)if(!p->config.counts_per_rev[w])return 0;
    return AFR2_SEQUENCE_MEMORY_APPROVED && p->config.motor_ready && p->pose_valid &&
        p->config.geometry_ready && p->sensors.hall_available && !p->fault;
}
#if AFR2_SEQUENCE_MEMORY_APPROVED
static uint8_t bounded(int16_t v,uint16_t limit) { return (int32_t)v<=limit && (int32_t)v>=-(int32_t)limit; }
static void wheel_enter(Afr2Product *p,uint8_t wheel,uint8_t index) {
    Afr2Sequence *s=&p->sequence;s->cursor[wheel]=index;s->wheel_started[wheel]=p->now;
    s->wheel_previous[wheel]=p->sensors.counts[wheel];s->wheel_pulses[wheel]=0;s->wheel_goal[wheel]=0;
    if(index<s->count && s->steps[index].condition) {
        float goal=(float)s->steps[index].limit*(float)p->config.counts_per_rev[wheel]/((float)p->config.wheel_mm*3.14159265f);
        uint32_t rounded=(uint32_t)goal;if((float)rounded<goal)++rounded;
        s->wheel_goal[wheel]=rounded?rounded:1;
    }
}
static uint8_t pulse_commit(Afr2Sequence *s) {
    uint8_t at[4];uint32_t sum[4]={0},barrier_total=0;
    for(uint8_t w=0;w<4;w++)at[w]=s->first[w];
    for(uint8_t pass=0;pass<s->count;pass++) {
        uint8_t any=0,all=1;uint32_t longest=0;
        for(uint8_t w=0;w<4;w++) {
            if(at[w]>=s->count){all=0;continue;}
            any=1;Afr2WheelStep *b=&s->steps[at[w]];
            if(b->timeout>s->total-sum[w])return 0;
            sum[w]+=b->timeout;if(b->timeout>longest)longest=b->timeout;at[w]=b->next;
        }
        if(!any)return 1;
        if(s->mode==2){if(!all || longest>s->total-barrier_total)return 0;barrier_total+=longest;}
    }
    return 1;
}
#endif
static uint8_t same_session(const Afr2Product *p,uint32_t session) {
    return session && p->session==session;
}
uint8_t Afr2Sequence_Command(Afr2Product *p,uint8_t cmd,const uint8_t *d,uint8_t n,uint8_t *out,uint8_t *length) {
    Afr2Sequence *s=&p->sequence; *length=0;
    if(cmd==AFR2_EXT_SEQ_STATUS) {
        if(n!=8 || !same_session(p,s->source_session) || afr2_get_u32(d)!=s->token || afr2_get_u32(d+4)!=s->execution)return AFR2_RESULT_BAD_REQUEST;
        afr2_put_u32(out,s->token);afr2_put_u32(out+4,s->execution);out[8]=s->state;
        out[9]=s->index;out[10]=s->count;afr2_put_u32(out+11,s->elapsed);afr2_put_u32(out+15,s->total);
        *length=19;return AFR2_RESULT_OK;
    }
    if(n<8 || afr2_get_u32(d)!=0 || !p->session)return AFR2_RESULT_BAD_REQUEST;
    if(p->hall_measuring || (!p->stopped && p->origin==AFR2_SOURCE_LOCAL))return AFR2_RESULT_UNAVAILABLE;
    Afr2Product_WebPulse(p,0);
    uint32_t token=afr2_get_u32(d+4);
    if(!token)return AFR2_RESULT_BAD_REQUEST;
    if(cmd==AFR2_EXT_SEQ_BEGIN) {
        if((n!=13 && n!=14) || (n==14 && (d[13]<1 || d[13]>2)) || !d[8] || d[8]>AFR2_SEQUENCE_MAX || !afr2_get_u32(d+9) ||
           afr2_get_u32(d+9)>AFR2_SEQUENCE_TIME_MAX || afr2_get_u32(d+9)%10)return AFR2_RESULT_BAD_REQUEST;
        if(!Afr2Sequence_Available(p) || !p->stopped || s->state==3 || p->hall_measuring)return AFR2_RESULT_UNAVAILABLE;
#if AFR2_SEQUENCE_MEMORY_APPROVED
        s->token=token;s->execution=0;s->source_session=p->session;s->total=afr2_get_u32(d+9);
        s->sum=s->elapsed=0;s->count=d[8];s->loaded=0;s->index=255;s->state=1;
        s->mode=n==14?d[13]:0;s->completed=s->done=0;
        for(uint8_t w=0;w<4;w++)s->first[w]=s->cursor[w]=255;
        afr2_put_u32(out,token);out[4]=1;out[5]=AFR2_SEQUENCE_MAX;afr2_put_u32(out+6,AFR2_SEQUENCE_TIME_MAX);*length=10;
        return AFR2_RESULT_OK;
#else
        return AFR2_RESULT_UNAVAILABLE;
#endif
    }
    if(token!=s->token || !same_session(p,s->source_session))return AFR2_RESULT_UNAVAILABLE;
    if(cmd==AFR2_EXT_SEQ_CANCEL) {
        if(n!=13 || afr2_get_u32(d+8)!=s->execution)return AFR2_RESULT_BAD_REQUEST;
        Afr2Product_Stop(p);afr2_put_u32(out,token);afr2_put_u32(out+4,s->execution);
        out[8]=s->state;out[9]=p->stopped;*length=10;return AFR2_RESULT_OK;
    }
    if(!Afr2Sequence_Available(p))return AFR2_RESULT_UNAVAILABLE;
#if AFR2_SEQUENCE_MEMORY_APPROVED
    if(cmd==AFR2_EXT_SEQ_APPEND) {
        if(s->mode) {
            if(n!=28 || s->state!=1 || d[8]!=s->loaded || s->loaded>=s->count || d[9]>3 || d[10]>1 || d[11]>=AFR2_SEQ_LABEL_COUNT)return AFR2_RESULT_BAD_REQUEST;
            uint32_t limit=afr2_get_u32(d+12),timeout=afr2_get_u32(d+16),ramp=afr2_get_u32(d+20);
            int16_t from=(int16_t)afr2_get_u16(d+24),to=(int16_t)afr2_get_u16(d+26);
            if(!limit || !timeout || timeout%10 || timeout>s->total || !ramp || ramp%10 || ramp>timeout || !bounded(from,p->config.speed_limit) || !bounded(to,p->config.speed_limit))return AFR2_RESULT_BAD_REQUEST;
            if(d[10]?(limit>10000u || (!from && !to)):(limit!=timeout || limit%10))return AFR2_RESULT_BAD_REQUEST;
            uint8_t index=s->loaded,wheel=d[9];Afr2WheelStep *b=&s->steps[index];
            b->limit=limit;b->timeout=timeout;b->ramp=ramp;b->from=from;b->to=to;b->wheel=wheel;b->condition=d[10];b->next=255;b->kind=d[11];
            if(s->first[wheel]==255)s->first[wheel]=index;
            else {uint8_t previous=s->first[wheel];while(s->steps[previous].next!=255)previous=s->steps[previous].next;s->steps[previous].next=index;}
            ++s->loaded;afr2_put_u32(out,token);out[4]=index;out[5]=1;*length=6;return AFR2_RESULT_OK;
        }
        if(n!=30 || d[29]>=AFR2_SEQ_LABEL_COUNT || s->state!=1 || d[8]!=s->loaded || s->loaded>=s->count)return AFR2_RESULT_BAD_REQUEST;
        uint32_t ms=afr2_get_u32(d+9);
        if(!ms || ms%10 || ms>s->total-s->sum)return AFR2_RESULT_BAD_REQUEST;
        for(uint8_t i=0;i<8;i++)if(!bounded((int16_t)afr2_get_u16(d+13+i*2),p->config.speed_limit))return AFR2_RESULT_BAD_REQUEST;
        Afr2Segment *b=&s->segments[s->loaded];b->ms=ms;
        for(uint8_t i=0;i<4;i++){b->from[i]=(int16_t)afr2_get_u16(d+13+i*2);b->to[i]=(int16_t)afr2_get_u16(d+21+i*2);}
        uint8_t shift=(s->loaded&1u)*4u;
        s->kinds[s->loaded/2]=(s->kinds[s->loaded/2] & (uint8_t)~(15u<<shift)) | (d[29]<<shift);
        s->sum+=ms;s->loaded++;afr2_put_u32(out,token);out[4]=d[8];out[5]=1;*length=6;
    } else if(cmd==AFR2_EXT_SEQ_COMMIT) {
        if(n!=13 || s->state!=1 || d[8]!=s->count || s->loaded!=s->count ||
           afr2_get_u32(d+9)!=s->total || (s->mode?!pulse_commit(s):s->sum!=s->total))return AFR2_RESULT_BAD_REQUEST;
        s->state=2;afr2_put_u32(out,token);out[4]=2;out[5]=s->count;afr2_put_u32(out+6,s->total);*length=10;
    } else if(cmd==AFR2_EXT_SEQ_START) {
        if(n!=12 || !afr2_get_u32(d+8) || s->state!=2 || !p->stopped || Afr2Head_State()->running)return AFR2_RESULT_BAD_REQUEST;
        s->execution=afr2_get_u32(d+8);s->started=p->now;s->elapsed=0;s->index=0;s->state=3;
        p->origin=AFR2_SOURCE_WEB;p->mode=AFR2_SEQUENCE;p->stopped=0;
        if(s->mode){for(uint8_t w=0;w<4;w++)wheel_enter(p,w,s->first[w]);}
        afr2_put_u32(out,token);afr2_put_u32(out+4,s->execution);out[8]=3;out[9]=0;afr2_put_u32(out+10,0);*length=14;
    } else return AFR2_RESULT_UNAVAILABLE;
    return AFR2_RESULT_OK;
#else
    return AFR2_RESULT_UNAVAILABLE;
#endif
}
void Afr2Sequence_Tick(Afr2Product *p) {
#if AFR2_SEQUENCE_MEMORY_APPROVED
    Afr2Sequence *s=&p->sequence;
    if(s->state!=3)return;
    if(!same_session(p,s->source_session) || !Afr2Sequence_Available(p) || p->mode!=AFR2_SEQUENCE){Afr2Product_Stop(p);return;}
    uint32_t elapsed=p->now-s->started;s->elapsed=elapsed>s->total?s->total:elapsed;
    if(s->mode) {
        if(elapsed>=s->total){s->state=6;Afr2Product_Stop(p);return;}
        for(uint8_t w=0;w<4;w++) {
            uint8_t i=s->cursor[w];if(i>=s->count || (s->done&(1u<<w))){p->target[w]=0;continue;}
            Afr2WheelStep *b=&s->steps[i];uint32_t age=p->now-s->wheel_started[w];
            int32_t delta=(int32_t)((uint32_t)p->sensors.counts[w]-(uint32_t)s->wheel_previous[w]);s->wheel_previous[w]=p->sensors.counts[w];
            if(delta>100000L || delta< -100000L){s->state=6;Afr2Product_Stop(p);return;}
            uint32_t pulses=delta<0?(uint32_t)(-delta):(uint32_t)delta;
            if(b->condition && s->wheel_pulses[w]<s->wheel_goal[w])s->wheel_pulses[w]=pulses>=s->wheel_goal[w]-s->wheel_pulses[w]?s->wheel_goal[w]:s->wheel_pulses[w]+pulses;
            if(b->condition?s->wheel_pulses[w]>=s->wheel_goal[w]:age>=b->limit) {
                p->target[w]=0;++s->completed;
                if(s->mode==2)s->done|=1u<<w;else wheel_enter(p,w,b->next);
            } else if(age>=b->timeout){s->state=6;Afr2Product_Stop(p);return;}
            else {uint32_t t=age>b->ramp?b->ramp:age;p->target[w]=b->from+(int32_t)(b->to-b->from)*(int32_t)t/(int32_t)b->ramp;}
        }
        if(s->mode==2 && s->done==15){for(uint8_t w=0;w<4;w++)wheel_enter(p,w,s->steps[s->cursor[w]].next);s->done=0;}
        if(s->completed==s->count){s->state=4;s->index=255;Afr2Product_Stop(p);return;}
        s->index=s->completed;return;
    }
    if(elapsed>=s->total){s->state=4;s->index=255;Afr2Product_Stop(p);return;}
    uint32_t offset=0;
    for(uint8_t i=0;i<s->count;i++) {
        const Afr2Segment *b=&s->segments[i];
        if(elapsed<offset+b->ms){s->index=i;
            for(uint8_t w=0;w<4;w++)p->target[w]=b->from[w]+(int32_t)(b->to[w]-b->from[w])*(int32_t)(elapsed-offset)/(int32_t)b->ms;
            return;
        }
        offset+=b->ms;
    }
    s->state=6;Afr2Product_Stop(p);
#else
    (void)p;
#endif
}
