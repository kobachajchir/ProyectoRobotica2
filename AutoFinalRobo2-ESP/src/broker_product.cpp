// AFR2-20261024-01. EX01 adapter. ROBO2/profile2 and Wi-Fi remain independent.
#include "broker.h"
#include "afr2_sequence_labels.h"
#include <string.h>
#include <math.h>
extern "C" {
#include <user_interface.h>
}
static uint32_t token() { uint32_t t=os_random();return t?t:1; }
static bool number(JsonVariantConst v,int32_t lo,int32_t hi) { return v.is<int32_t>() && v.as<int32_t>()>=lo && v.as<int32_t>()<=hi; }
static bool textId(JsonVariantConst v) { return v.is<const char *>() && strlen(v.as<const char *>())>0 && strlen(v.as<const char *>())<=64 && v.as<JsonString>().size()==strlen(v.as<const char *>()); }
bool Broker::encodeProduct(BrokerRequest &q,JsonObjectConst p) {
    const char *c=q.command;
    auto fail=[&](){error(q,"BAD_REQUEST","Argumentos fuera del contrato EX01.");return false;};
    if(!strcmp(c,"system.reboot")) {
        if(!p["confirm"].is<bool>() || !p["confirm"].as<bool>())return fail();
        const char *target=p["target"]|"";
        if(!strcmp(target,"atmega")){q.wireCommand=AFR2_EXT_REBOOT;q.bodyLength=1;q.body[0]=0xa5;}
        else if(strcmp(target,"esp"))return fail();
    } else if(!strcmp(c,"wifi.draft.submit") || !strcmp(c,"wifi.draft.cancel")) {
        if(!p["jobId"].is<uint32_t>() || !p["jobId"].as<uint32_t>())return fail();
        afr2_put_u32(q.body,p["jobId"].as<uint32_t>());
    } else if(!strcmp(c,"robot.drive")) {
        if(!number(p["vxMmS"],-1000,1000) || !number(p["vyMmS"],-1000,1000) ||
           !number(p["wzMradS"],-3140,3140) || !number(p["ttlMs"],200,1000))return fail();
        q.wireCommand=AFR2_EXT_DRIVE;q.bodyLength=12;
        afr2_put_u16(q.body+4,(int16_t)p["vxMmS"].as<int32_t>());afr2_put_u16(q.body+6,(int16_t)p["vyMmS"].as<int32_t>());
        afr2_put_u16(q.body+8,(int16_t)p["wzMradS"].as<int32_t>());afr2_put_u16(q.body+10,p["ttlMs"].as<uint16_t>());
    } else if(!strcmp(c,"robot.test.wheel")) {
        const char *w=p["wheel"]|"",*dir=p["direction"]|"";const char *names[]={"FL","FR","RL","RR"};
        uint8_t i=0;while(i<4 && strcmp(w,names[i]))i++;
        if(i==4 || (strcmp(dir,"forward") && strcmp(dir,"reverse") && strcmp(dir,"stop")) ||
           !number(p["pwmPct"],0,60) || !number(p["durationMs"],100,2000))return fail();
        int16_t pwm=p["pwmPct"].as<int32_t>()*255/100;if(!strcmp(dir,"reverse"))pwm=-pwm;if(!strcmp(dir,"stop"))pwm=0;
        q.wireCommand=AFR2_EXT_WHEEL;q.bodyLength=9;q.body[4]=i;afr2_put_u16(q.body+5,pwm);afr2_put_u16(q.body+7,p["durationMs"].as<uint16_t>());
    } else if(!strcmp(c,"robot.mode")) {
        const char *m=p["mode"]|"";uint8_t value=!strcmp(m,"idle")?0:!strcmp(m,"manual")?1:!strcmp(m,"line-follow")?2:255;
        if(value==255)return fail();
        q.wireCommand=AFR2_EXT_MODE;q.bodyLength=5;q.body[4]=value;
    } else if(!strcmp(c,"robot.goto.start")) {
        if(!number(p["xMm"],-5000,5000) || !number(p["yMm"],-5000,5000) ||
           !number(p["thetaMrad"],-3140,3140) || !number(p["maxSpeedMmS"],10,1000))return fail();
        q.wireCommand=AFR2_EXT_GOTO;q.bodyLength=16;afr2_put_u32(q.body+4,p["xMm"].as<int32_t>());afr2_put_u32(q.body+8,p["yMm"].as<int32_t>());
        afr2_put_u16(q.body+12,(int16_t)p["thetaMrad"].as<int32_t>());afr2_put_u16(q.body+14,p["maxSpeedMmS"].as<uint16_t>());
    } else if(!strcmp(c,"robot.calibrate") || !strcmp(c,"robot.config.get")) {
        if(!number(p["action"],0,56) || (!strcmp(c,"robot.calibrate") && !p["value"].is<int32_t>()))return fail();
        q.wireCommand=AFR2_EXT_CONFIG;q.bodyLength=10;q.body[4]=!strcmp(c,"robot.calibrate");q.body[5]=p["action"].as<uint8_t>();
        afr2_put_u32(q.body+6,p["value"]|0);
    } else if(!strcmp(c,"robot.preferences")) {
        const char *op=p["action"]|"";if(strcmp(op,"save") && strcmp(op,"discard"))return fail();
        q.wireCommand=AFR2_EXT_PREFS;q.bodyLength=5;q.body[4]=!strcmp(op,"discard");
    } else if(!strcmp(c,"robot.line.calibrate")) {
        if(!p["confirm"].is<bool>())return fail();
        q.wireCommand=AFR2_EXT_LINE_CAL;q.bodyLength=5;q.body[4]=p["confirm"].as<bool>()?0:1;
    } else if(!strcmp(c,"robot.goto.cancel")) {q.wireCommand=AFR2_EXT_CANCEL;q.bodyLength=0;
    } else if(!strcmp(c,"robot.state")) {q.wireCommand=AFR2_EXT_STATE;
    } else if(!strcmp(c,"robot.screen.get")) {q.wireCommand=AFR2_EXT_UI_META;
    } else if(!strcmp(c,"robot.ui.action")) {
        const char *a=p["action"]|"";uint8_t action=!strcmp(a,"rotate")?0:!strcmp(a,"select")?1:!strcmp(a,"menu")?2:255;
        if(action==255 || !number(p["delta"],-1,1) || (action==0?p["delta"].as<int>()==0:p["delta"].as<int>()!=0) || !p["revision"].is<uint32_t>())return fail();
        q.wireCommand=AFR2_EXT_UI_ACTION;q.bodyLength=6;afr2_put_u32(q.body,p["revision"].as<uint32_t>());q.body[4]=action;q.body[5]=(int8_t)p["delta"].as<int>();
    } else if(!strncmp(c,"robot.sequence.",15)) {
        if(!textId(p["sequenceId"]))return fail();
        strlcpy(q.sequenceId,p["sequenceId"].as<const char *>(),sizeof(q.sequenceId));
        if(!strcmp(c,"robot.sequence.begin") || !strcmp(c,"robot.sequence.commit")) {
            if(!number(p["segmentCount"],1,32) || !number(p["totalDurationMs"],10,120000) || p["totalDurationMs"].as<uint32_t>()%10)return fail();
            q.wireCommand=!strcmp(c,"robot.sequence.begin")?AFR2_EXT_SEQ_BEGIN:AFR2_EXT_SEQ_COMMIT;q.bodyLength=13;
            q.body[8]=p["segmentCount"].as<uint8_t>();afr2_put_u32(q.body+9,p["totalDurationMs"].as<uint32_t>());
            const char *mode=p["executionMode"]|"timed";
            if(strcmp(mode,"timed")) {
                uint8_t value=!strcmp(mode,"independent-wheels")?1:!strcmp(mode,"barrier-wheels")?2:0;if(!value)return fail();
                if(q.wireCommand==AFR2_EXT_SEQ_BEGIN){q.bodyLength=14;q.body[13]=value;}
            }
        } else if(!strcmp(c,"robot.sequence.append")) {
            if(p["wheel"].is<const char *>()) {
                const char *wheels[]={"fl","fr","rl","rr"};const char *wheel=p["wheel"]|"";uint8_t id=0;while(id<4 && strcmp(wheel,wheels[id]))++id;
                const char *condition=p["completion"]|"";uint8_t distance=!strcmp(condition,"distance");
                if(id==4 || (!distance && strcmp(condition,"time")) || !number(p["index"],0,31) || !number(p["limit"],1,distance?10000:120000) || !number(p["timeoutMs"],10,120000) || !number(p["rampMs"],10,120000) || p["timeoutMs"].as<uint32_t>()%10 || p["rampMs"].as<uint32_t>()%10 || p["rampMs"].as<uint32_t>()>p["timeoutMs"].as<uint32_t>() || !number(p["fromMmS"],-1000,1000) || !number(p["toMmS"],-1000,1000))return fail();
                if(!distance && (p["limit"].as<uint32_t>()!=p["timeoutMs"].as<uint32_t>() || p["limit"].as<uint32_t>()%10))return fail();
                q.wireCommand=AFR2_EXT_SEQ_APPEND;q.bodyLength=28;q.body[8]=p["index"].as<uint8_t>();q.body[9]=id;q.body[10]=distance;q.body[11]=AFR2_SEQ_WHEELS;
                afr2_put_u32(q.body+12,p["limit"].as<uint32_t>());afr2_put_u32(q.body+16,p["timeoutMs"].as<uint32_t>());afr2_put_u32(q.body+20,p["rampMs"].as<uint32_t>());
                afr2_put_u16(q.body+24,(int16_t)p["fromMmS"].as<int32_t>());afr2_put_u16(q.body+26,(int16_t)p["toMmS"].as<int32_t>());return true;
            }
            if(!number(p["index"],0,31) || !number(p["durationMs"],10,120000) || p["durationMs"].as<uint32_t>()%10)return fail();
            const char *w[]={"fl","fr","rl","rr"};q.wireCommand=AFR2_EXT_SEQ_APPEND;q.bodyLength=30;q.body[8]=p["index"].as<uint8_t>();afr2_put_u32(q.body+9,p["durationMs"].as<uint32_t>());
            const char *kinds[]={"constant","ramp","pivot","curve","wait","finish","wheels"};
            const char *kind=p["movementKind"]|"wheels";uint8_t id=0;while(id<7 && strcmp(kind,kinds[id]))++id;
            if(id==7)return fail();
            q.body[29]=id;
            for(uint8_t i=0;i<4;i++) {
                if(!number(p["startWheelMmS"][w[i]],-1000,1000) || !number(p["endWheelMmS"][w[i]],-1000,1000))return fail();
                afr2_put_u16(q.body+13+2*i,(int16_t)p["startWheelMmS"][w[i]].as<int32_t>());afr2_put_u16(q.body+21+2*i,(int16_t)p["endWheelMmS"][w[i]].as<int32_t>());
            }
        } else {
            if(!textId(p["executionId"]))return fail();
            strlcpy(q.executionId,p["executionId"].as<const char *>(),sizeof(q.executionId));
            q.wireCommand=!strcmp(c,"robot.sequence.start")?AFR2_EXT_SEQ_START:!strcmp(c,"robot.sequence.cancel")?AFR2_EXT_SEQ_CANCEL:!strcmp(c,"robot.sequence.status")?AFR2_EXT_SEQ_STATUS:0;
            if(!q.wireCommand)return fail();
            q.bodyLength=q.wireCommand==AFR2_EXT_SEQ_STATUS?8:q.wireCommand==AFR2_EXT_SEQ_CANCEL?13:12;
        }
    }
    return true;
}
bool Broker::productRequest(uint8_t cmd,const uint8_t *body,uint8_t size) {return mega_.requestProduct(cmd,body,size,millis());}
void Broker::stopWebOperations() {++generation_;safetyRequested_=true;sequenceClient_=sequenceToken_=executionToken_=0;sequenceId_[0]=executionId_[0]=0;}
void Broker::publishStatus() {
    for(auto &s:sessions_)if(s.used && s.ready){JsonDocument f;f["schema"]="afr2-ws-1";f["type"]="evt";f["event"]="system.status";f["ts"]=millis();status(f["payload"].to<JsonObject>(),millis(),s.id);
        if(auto *client=ws_.client(s.id)){String data;serializeJson(f,data);if(!client->queueIsFull())client->text(data);}}
}
bool Broker::dispatchProduct(BrokerRequest q,uint32_t now) {
    if(!q.wireCommand)return false;
    if(!mega_.ready() || !productReady_ || extensionVersion_!=7){error(q,"CAPABILITY_UNAVAILABLE","ATmega aun no negocio EX01.");return true;}
    if(q.wireCommand==AFR2_EXT_UI_META || q.wireCommand==AFR2_EXT_UI_ACTION) {
        if(!(productFeatures_&AFR2_EXT_F_UI)){error(q,"CAPABILITY_UNAVAILABLE","Servicio UI remoto no disponible.");return true;}
        if(q.wireCommand==AFR2_EXT_UI_ACTION && (!session(q.client)->oledCaptured || afr2_get_u32(q.body)!=session(q.client)->oledRevision)){error(q,"BAD_REQUEST","La captura ya no corresponde a esta sesion.");return true;}
    }
    if(q.wireCommand>=AFR2_EXT_SEQ_BEGIN && q.wireCommand<=AFR2_EXT_SEQ_STATUS && extensionVersion_<4){error(q,"CAPABILITY_UNAVAILABLE","Secuencias con pantalla requieren EX01 v4.");return true;}
    if(((q.wireCommand==AFR2_EXT_SEQ_BEGIN && q.bodyLength==14) || (q.wireCommand==AFR2_EXT_SEQ_APPEND && q.bodyLength==28)) && (extensionVersion_<6 || !(productFeatures_&AFR2_EXT_F_DISTANCE))){error(q,"CAPABILITY_UNAVAILABLE","Finalizacion por pulsos requiere EX01 v6 y Hall/calibracion/memoria aprobados.");return true;}
    if(q.wireCommand==AFR2_EXT_REBOOT && extensionVersion_<4){error(q,"CAPABILITY_UNAVAILABLE","Reinicio requiere EX01 v4.");return true;}
    if(q.wireCommand==AFR2_EXT_GOTO && (extensionVersion_<3 || !(productFeatures_&AFR2_EXT_F_GOTO))){error(q,"CAPABILITY_UNAVAILABLE","GOTO completo requiere EX01 v3 y odometria valida.");return true;}
    if(q.wireCommand>=AFR2_EXT_SEQ_BEGIN && q.wireCommand<=AFR2_EXT_SEQ_STATUS) {
        if(extensionVersion_<2){error(q,"CAPABILITY_UNAVAILABLE","Secuencias requieren EX01 v2.");return true;}
        if(q.wireCommand==AFR2_EXT_SEQ_BEGIN){sequenceClient_=q.client;sequenceToken_=token();executionToken_=0;strlcpy(sequenceId_,q.sequenceId,65);executionId_[0]=0;afr2_put_u32(q.body+4,sequenceToken_);}
        else {
            if(sequenceClient_!=q.client || strcmp(q.sequenceId,sequenceId_) ||
               (q.executionId[0] && q.wireCommand!=AFR2_EXT_SEQ_START && executionToken_ && strcmp(q.executionId,executionId_))) {
                error(q,"BAD_REQUEST","Identidad de programa/ejecucion distinta.");return true;
            }
            if(q.wireCommand==AFR2_EXT_SEQ_STATUS){afr2_put_u32(q.body,sequenceToken_);afr2_put_u32(q.body+4,executionToken_);}
            else {afr2_put_u32(q.body+4,sequenceToken_);
                if(q.wireCommand==AFR2_EXT_SEQ_START){executionToken_=token();strlcpy(executionId_,q.executionId,65);afr2_put_u32(q.body+8,executionToken_);}
                if(q.wireCommand==AFR2_EXT_SEQ_CANCEL)afr2_put_u32(q.body+8,executionToken_);}
        }
    }
    if(q.wireCommand==AFR2_EXT_SEQ_CANCEL) {
        if(waiting_){error(pending_,"SUPERSEDED","Cancelacion prioritaria.");waiting_=false;}
        if(!mega_.requestProduct(q.wireCommand,q.body,q.bodyLength,now,true)){error(q,"BUSY_IN_FLIGHT","Cancelacion ocupada.",true);return true;}
    } else if(waiting_ || !productRequest(q.wireCommand,q.body,q.bodyLength)){error(q,"BUSY_IN_FLIGHT","UART ocupada.",true);return true;}
    pending_=q;waiting_=true;if(q.wireCommand==AFR2_EXT_UI_META)oledAt_=now;
    return true;
}
bool Broker::finishProduct(const BrokerRequest &q,const MegaResult &r) {
    if(!q.wireCommand)return false;
    const uint8_t *p=r.payload;JsonDocument f;JsonObject out=f["payload"].to<JsonObject>();
    if(r.command==AFR2_EXT_UI_PAGE) {
        if((uint32_t)(millis()-oledAt_)>1800 || afr2_get_u32(p+9)!=oledRevision_ || afr2_get_u16(p+13)!=oledOffset_){error(q,"TIMEOUT","Captura OLED incoherente o vencida.");return true;}
        memcpy(oledData_+oledOffset_,p+16,p[15]);oledOffset_+=p[15];
        uint8_t b[7];afr2_put_u32(b,oledRevision_);
        uint8_t next=oledOffset_<1024?AFR2_EXT_UI_PAGE:AFR2_EXT_UI_RELEASE;
        if(next==AFR2_EXT_UI_PAGE){afr2_put_u16(b+4,oledOffset_);b[6]=(1024-oledOffset_)>40?40:1024-oledOffset_;}
        if(!productRequest(next,b,next==AFR2_EXT_UI_PAGE?7:4)){error(q,"BUSY_IN_FLIGHT","Captura interrumpida.");return true;}
        pending_=q;waiting_=true;return true;
    } else if(r.command==AFR2_EXT_UI_RELEASE) {
        static const char abc[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        String encoded;encoded.reserve(1369);
        for(uint16_t i=0;i<1024;i+=3){uint32_t v=(uint32_t)oledData_[i]<<16;
            if(i+1<1024)v|=(uint32_t)oledData_[i+1]<<8;
            if(i+2<1024)v|=oledData_[i+2];
            encoded+=abc[(v>>18)&63];encoded+=abc[(v>>12)&63];encoded+=i+1<1024?abc[(v>>6)&63]:'=';encoded+=i+2<1024?abc[v&63]:'=';}
        char code[11];snprintf(code,sizeof(code),"0x%06lX",(unsigned long)oledScreen_);
        out["screenCode"]=code;out["selection"]=oledSelection_;out["firstVisible"]=oledFirst_;
        out["revision"]=oledRevision_;out["framebufferBase64"]=encoded;
        if(auto *s=session(q.client)){s->oledRevision=oledRevision_;s->oledCaptured=true;}
    } else if(q.wireCommand==AFR2_EXT_STATE) {
        if(r.command==AFR2_EXT_STATE) {
            memcpy(stateData_,p,25);productMode_=p[9];productOrigin_=p[10];productFeatures_=afr2_get_u32(p+17);productAt_=millis();
            if((productFeatures_&AFR2_EXT_F_POSE) && productRequest(AFR2_EXT_POSE,nullptr,0)){pending_=q;waiting_=true;return true;}
        }
        const char *modes[]={"idle","manual","line-follow","wheel-test","goto","fault","sequence"};
        out["mode"]=modes[stateData_[9]];out["stopped"]=stateData_[11]!=0;out["ageMs"]=(uint32_t)(millis()-productAt_);
        out["origin"]=stateData_[10]==2?"web":stateData_[10]==1?"local":"none";
        const char *missions[]={"idle","running","completed","cancelled","failed"};out["gotoState"]=missions[stateData_[15]];out["preferencesState"]=stateData_[16];out["fault"]=stateData_[12];
        if(r.command==AFR2_EXT_POSE && p[9] && afr2_get_u32(p+22)<=2000){
            JsonObject pose=out["pose"].to<JsonObject>();pose["schema"]="afr2-pose-1";pose["valid"]=true;
            pose["frameId"]=String("mega-")+String(afr2_get_u32(p+26));pose["sequence"]=afr2_get_u32(p+30);
            pose["xMm"]=(int32_t)afr2_get_u32(p+10);pose["yMm"]=(int32_t)afr2_get_u32(p+14);pose["thetaMrad"]=(int32_t)afr2_get_u32(p+18);pose["ageMs"]=afr2_get_u32(p+22);
        }
    } else if(q.wireCommand==AFR2_EXT_CONFIG) {out["action"]=q.body[5];out["value"]=(int32_t)afr2_get_u32(p+9);out["persisted"]=false;limitsAt_=0;
    } else if(q.wireCommand==AFR2_EXT_PREFS) {f["status"]=q.body[4]?"completed":"accepted";out["state"]=q.body[4]?"discarded":"writing";out["persisted"]=false;
    } else if(q.wireCommand==AFR2_EXT_LINE_CAL) {out["calibrated"]=p[9]!=0;
    } else if(q.wireCommand==AFR2_EXT_CANCEL) {out["gotoState"]="cancelled";out["stopped"]=true;productOrigin_=0;publishStatus();
    } else if(q.wireCommand==AFR2_EXT_UI_META) {
        oledRevision_=afr2_get_u32(p+9);oledScreen_=afr2_get_u32(p+13);oledSelection_=p[17];oledFirst_=p[18];oledOffset_=0;
        uint8_t body[7];afr2_put_u32(body,oledRevision_);afr2_put_u16(body+4,0);body[6]=40;
        if(!productRequest(AFR2_EXT_UI_PAGE,body,7)){error(q,"BUSY_IN_FLIGHT","No se pudo leer framebuffer.");return true;}
        pending_=q;waiting_=true;return true;
    } else if(q.wireCommand==AFR2_EXT_UI_ACTION){out["screenCode"]=afr2_get_u32(p+9);out["confirmed"]=true;
    } else if(q.wireCommand>=AFR2_EXT_SEQ_BEGIN && q.wireCommand<=AFR2_EXT_SEQ_STATUS) {
        if(q.wireCommand==AFR2_EXT_SEQ_BEGIN){sequenceClient_=q.client;sequenceToken_=afr2_get_u32(p+9);executionToken_=0;strlcpy(sequenceId_,q.sequenceId,65);executionId_[0]=0;}
        if(q.wireCommand==AFR2_EXT_SEQ_START){executionToken_=afr2_get_u32(p+13);strlcpy(executionId_,q.executionId,65);}
        static const char *states[]={"idle","preparing","ready","running","completed","cancelled","failed"};
        uint8_t state=q.wireCommand==AFR2_EXT_SEQ_STATUS?p[17]:q.wireCommand==AFR2_EXT_SEQ_START?3:q.wireCommand==AFR2_EXT_SEQ_COMMIT?2:q.wireCommand==AFR2_EXT_SEQ_CANCEL?p[17]:1;
        out["sequenceId"]=sequenceId_;out["executionId"]=executionId_;out["state"]=states[state];out["reason"]=nullptr;
        if(q.wireCommand==AFR2_EXT_SEQ_STATUS){if(p[18]==255)out["activeSegmentIndex"]=nullptr;else out["activeSegmentIndex"]=p[18];
            out["segmentCount"]=p[19];out["elapsedMs"]=afr2_get_u32(p+20);out["totalDurationMs"]=afr2_get_u32(p+24);}
        if(q.wireCommand==AFR2_EXT_SEQ_CANCEL){productOrigin_=0;out["stopped"]=true;publishStatus();}
    } else if(q.wireCommand==AFR2_EXT_DRIVE){out["accepted"]=true;
    } else if(q.wireCommand==AFR2_EXT_WHEEL || q.wireCommand==AFR2_EXT_GOTO){f["status"]="accepted";out["accepted"]=true;
    } else if(q.wireCommand==AFR2_EXT_MODE){out["confirmed"]=true;if(q.body[4]==0){productOrigin_=0;publishStatus();}}
    answer(q,f);return true;
}
void Broker::tickProduct(uint32_t now) {
    if(!mega_.ready()){productReady_=false;extensionVersion_=0;productFeatures_=0;productAttempted_=false;speedLimit_=0;pwmLimit_=0;limitsAt_=0;wifiAwaitWeb_=wifiDraftReady_=false;wifiJob_=0;wifiOffset_=0;wifiVersions_=0;oledDraft_=WifiConfig{};}
    if(safetyRequested_ && mega_.ready()) {
        if(waiting_){error(pending_,"SUPERSEDED","Supervision revocada.");waiting_=false;}
        if(mega_.request(AFR2_CMD_STOP,now,true)){pending_=BrokerRequest{};strlcpy(pending_.command,"robot.stop",33);waiting_=true;safetyRequested_=false;}
        return;
    }
    if(waiting_ || mega_.busy() || stopQueued_ || !mega_.ready())return;
    if(productReady_ && extensionVersion_==7 && (uint32_t)(now-webPulseAt_)>=500u) {
        for(const auto &client:sessions_)if(client.used && client.ready) {
            uint32_t age=now-client.inputSeen;uint8_t body[2];
            afr2_put_u16(body,age>1500u?1500u:(uint16_t)age);
            if(productRequest(AFR2_EXT_WEB_PULSE,body,2)) {pending_=BrokerRequest{};strlcpy(pending_.command,"_web_pulse",33);waiting_=true;webPulseAt_=now;}
            return;
        }
    }
    if(productReady_ && extensionVersion_>=3 && (!limitsAt_ || (uint32_t)(now-limitsAt_)>=3000u)) {
        if(productRequest(AFR2_EXT_LIMITS,nullptr,0)){BrokerRequest q;strlcpy(q.command,"_limits",33);pending_=q;waiting_=true;limitsAt_=now;}return;
    }
    tickWifi(now);if(waiting_)return;
    tickNetwork(now);if(waiting_)return;
    if((uint32_t)(now-productAt_)<(productReady_?500u:1000u))return;
    productAt_=now;BrokerRequest q;
    if(!productReady_){
        // Direct-control v7 is required; older extensions do not enable robot services.
        const uint8_t hello[]={'E','X','0','1'};
        if(productRequest(AFR2_EXT_HELLO,hello,4)){strlcpy(q.command,"_ext",33);pending_=q;waiting_=true;productAttempted_=true;}
    } else if(productRequest(AFR2_EXT_STATE,nullptr,0)){strlcpy(q.command,"_state",33);pending_=q;waiting_=true;}
}
