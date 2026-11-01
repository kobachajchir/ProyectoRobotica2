/* AFR2-20261025-14. EX01v8 head commands. ESP translates; ATmega owns PWM/sampling. */
#include "broker.h"
#include <string.h>
static bool integer(JsonVariantConst v,int32_t lo,int32_t hi){return v.is<int32_t>() && v.as<int32_t>()>=lo && v.as<int32_t>()<=hi;}
bool Broker::encodeSensors(BrokerRequest &q,JsonObjectConst p){
    const char *c=q.command;auto bad=[&](){error(q,"BAD_REQUEST","Parametros de sensores fuera de rango.");return false;};
    if(!strcmp(c,"robot.ultrasonic.get"))q.wireCommand=AFR2_EXT_US_GET;
    else if(!strcmp(c,"robot.ultrasonic.configure")){if(!integer(p["intervalMs"],70,1000))return bad();q.wireCommand=AFR2_EXT_US_CONFIG;q.bodyLength=2;afr2_put_u16(q.body,p["intervalMs"].as<uint16_t>());}
    else if(!strcmp(c,"robot.ir.get"))q.wireCommand=AFR2_EXT_IR_GET;
    else if(!strcmp(c,"robot.servo.get"))q.wireCommand=AFR2_EXT_SERVO_GET;
    else if(!strcmp(c,"robot.servo.set")){
        const char *op=p["operation"]|"angle";uint8_t operation=!strcmp(op,"off")?0:!strcmp(op,"angle")?1:!strcmp(op,"center")?2:255;
        if(operation==255 || (operation==1 && !integer(p["angleDeg"],0,180)))return bad();
        q.wireCommand=AFR2_EXT_SERVO_SET;q.bodyLength=2;q.body[0]=operation;q.body[1]=operation==1?p["angleDeg"].as<uint8_t>():90;
    }else if(!strcmp(c,"robot.radar.configure")){
        if(!p["write"].is<bool>())return bad();
        q.wireCommand=AFR2_EXT_RADAR_CONFIG;q.bodyLength=9;q.body[0]=p["write"].as<bool>();
        if(q.body[0]){
            if(!integer(p["minAngleDeg"],0,175) || !integer(p["maxAngleDeg"],5,180) || p["minAngleDeg"].as<int>()>=p["maxAngleDeg"].as<int>() ||
               p["minAngleDeg"].as<int>()%5 || p["maxAngleDeg"].as<int>()%5 || !integer(p["speedDegS"],1,30) || !integer(p["settleMs"],50,500) || !integer(p["intervalMs"],70,1000) || !p["inverted"].is<bool>())return bad();
            q.body[1]=p["minAngleDeg"].as<uint8_t>();q.body[2]=p["maxAngleDeg"].as<uint8_t>();q.body[3]=p["speedDegS"].as<uint8_t>();
            afr2_put_u16(q.body+4,p["settleMs"].as<uint16_t>());afr2_put_u16(q.body+6,p["intervalMs"].as<uint16_t>());q.body[8]=p["inverted"].as<bool>();
        }
    }else if(!strcmp(c,"robot.radar.start"))q.wireCommand=AFR2_EXT_RADAR_START;
    else if(!strcmp(c,"robot.radar.stop"))q.wireCommand=AFR2_EXT_RADAR_STOP;
    else if(!strcmp(c,"robot.radar.get")){
        const char *mode=p["mode"]|"delta";bool snapshot=!strcmp(mode,"snapshot");
        if((!snapshot && strcmp(mode,"delta")) || !p["cursor"].is<uint32_t>() || (snapshot && p["cursor"].as<uint32_t>()>37))return bad();
        q.wireCommand=AFR2_EXT_RADAR_GET;q.bodyLength=5;q.body[0]=snapshot?0:1;afr2_put_u32(q.body+1,p["cursor"].as<uint32_t>());
    }else if(!strcmp(c,"robot.screen.status"))q.wireCommand=AFR2_EXT_UI_STATUS;
    else return bad();
    return true;
}
static void rangeSample(JsonObject o,const uint8_t *p,uint32_t now){
    static const char *errors[]={"none","no-rise","no-fall","echo-high","out-of-range","moving","stale","unknown"};
    uint32_t id=afr2_get_u32(p);uint8_t e=p[15];o["sampleId"]=id;o["angleDeg"]=p[14];o["pulseUs"]=afr2_get_u16(p+12);
    o["valid"]=id && e==0;o["error"]=errors[e];o["echoUs"]=(uint32_t)afr2_get_u16(p+10)*25u;
    if(id){o["ageMs"]=(uint32_t)(now-afr2_get_u32(p+4));o["measuredAtMs"]=afr2_get_u32(p+4);}else{o["ageMs"]=nullptr;o["measuredAtMs"]=nullptr;}
    if(id && !e)o["distanceMm"]=afr2_get_u16(p+8);else o["distanceMm"]=nullptr;
}
bool Broker::finishSensors(const BrokerRequest &q,const MegaResult &r){
    if(q.wireCommand<AFR2_EXT_US_GET || q.wireCommand>AFR2_EXT_MOTION_HALT)return false;
    const uint8_t *p=r.payload+9;JsonDocument frame;JsonObject o=frame["payload"].to<JsonObject>();
    switch(q.wireCommand){
    case AFR2_EXT_US_GET:
        o["bootId"]=afr2_get_u32(p);rangeSample(o["sample"].to<JsonObject>(),p+8,afr2_get_u32(p+4));o["intervalMs"]=afr2_get_u16(p+24);o["frontalEligible"]=p[26]!=0;
        o["angleDeg"]=p[27];o["enabled"]=p[28]!=0;o["pulseUs"]=afr2_get_u16(p+29);o["targetUs"]=afr2_get_u16(p+31);o["running"]=p[33]!=0;o["scanRevision"]=p[34];
        sensorKnown_=true;sensorAt_=millis();radarRunning_=p[33];servoEnabled_=p[28];break;
    case AFR2_EXT_US_CONFIG:o["intervalMs"]=afr2_get_u16(p);break;
    case AFR2_EXT_IR_GET:{
        o["bootId"]=afr2_get_u32(p);o["sampleId"]=afr2_get_u32(p+4);if(p[13])o["ageMs"]=afr2_get_u32(p+8);else o["ageMs"]=nullptr;
        o["kind"]=p[12]==1?"digital":p[12]==2?"adc":"off";o["rawValid"]=p[13]!=0;o["calibrated"]=p[14]!=0;o["bits"]=p[15];o["valid"]=p[26]!=0;
        JsonArray raw=o["raw"].to<JsonArray>();for(uint8_t i=0;i<5;i++){if(p[13])raw.add(afr2_get_u16(p+16+2*i));else raw.add(nullptr);}break;}
    case AFR2_EXT_SERVO_GET:case AFR2_EXT_SERVO_SET:
        o["bootId"]=afr2_get_u32(p);o["sequence"]=afr2_get_u32(p+4);o["pulseUs"]=afr2_get_u16(p+8);o["targetUs"]=afr2_get_u16(p+10);
        o["angleDeg"]=p[12];o["enabled"]=p[13]!=0;o["ageMs"]=afr2_get_u32(p+14);o["positionSource"]="applied-pwm";servoEnabled_=p[13];break;
    case AFR2_EXT_RADAR_CONFIG:
        o["minAngleDeg"]=p[0];o["maxAngleDeg"]=p[1];o["speedDegS"]=p[2];o["settleMs"]=afr2_get_u16(p+3);o["intervalMs"]=afr2_get_u16(p+5);o["inverted"]=p[7]!=0;o["stepDeg"]=5;o["persisted"]=false;break;
    case AFR2_EXT_RADAR_START:case AFR2_EXT_RADAR_STOP:o["running"]=p[0]!=0;radarRunning_=p[0];if(!p[0])servoEnabled_=false;publishStatus();break;
    case AFR2_EXT_RADAR_GET:{
        uint32_t now=afr2_get_u32(p+4);o["bootId"]=afr2_get_u32(p);o["nextCursor"]=afr2_get_u32(p+8);o["dropped"]=afr2_get_u16(p+12);
        o["mode"]=q.body[0]?"delta":"snapshot";o["running"]=p[15]!=0;o["angleDeg"]=p[16];o["enabled"]=p[17]!=0;o["pulseUs"]=afr2_get_u16(p+18);o["targetUs"]=afr2_get_u16(p+20);
        o["scanRevision"]=p[22];JsonArray samples=o["samples"].to<JsonArray>();for(uint8_t i=0;i<p[14];i++)rangeSample(samples.add<JsonObject>(),p+23+16*i,now);
        radarRunning_=p[15];servoEnabled_=p[17];sensorKnown_=true;sensorAt_=millis();break;}
    case AFR2_EXT_UI_STATUS:
        o["screenCode"]=afr2_get_u32(p);o["navigationRevision"]=afr2_get_u32(p+4);o["selection"]=p[8];o["firstVisible"]=p[9];o["private"]=p[10]!=0;
        if(screenRevision_!=afr2_get_u32(p+4) || screenCode_!=afr2_get_u32(p)){
            screenRevision_=afr2_get_u32(p+4);screenCode_=afr2_get_u32(p);JsonDocument change;change.set(o);for(const auto &s:sessions_)if(s.used && s.ready && s.screenSubscribed && (uint32_t)(millis()-s.screenSubscriptionAt)<=3500u){JsonDocument f;f["schema"]="afr2-ws-1";f["type"]="evt";f["event"]="robot.screen.changed";f["ts"]=millis();f["payload"].set(change);if(auto *client=ws_.client(s.id)){String data;serializeJson(f,data);if(!client->queueIsFull())client->text(data);}}
        }break;
    case AFR2_EXT_MOTION_HALT:o["stopped"]=true;break;
    }
    answer(q,frame);return true;
}
void Broker::tickSensors(uint32_t now){
    if(!productReady_ || (uint32_t)(now-sensorAt_)<1000u)return;
    if(productRequest(AFR2_EXT_US_GET,nullptr,0)){pending_=BrokerRequest{};pending_.wireCommand=AFR2_EXT_US_GET;strlcpy(pending_.command,"_head",33);waiting_=true;sensorAt_=now;}
}
