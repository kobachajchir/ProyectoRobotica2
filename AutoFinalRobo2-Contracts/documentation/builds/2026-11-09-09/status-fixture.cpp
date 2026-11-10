
#include <ArduinoJson.h>
#include <cstring>
#include <iostream>
#include <string>
#include <cassert>
#include "afr2_product_wire.h"
#define F(x) x
constexpr int WL_CONNECTED=3;
uint32_t millis(){return 5000;}
const char *modeName(uint8_t mode){return mode==2?"AP":mode==3?"AP_STA":"STA";}
struct WifiMock { int status(){return 3;} } WiFi;
struct EspMock { uint32_t getFreeHeap(){return 20000;} uint32_t getMaxFreeBlockSize(){return 18000;} } ESP;
struct MegaMock {
 bool connected=true; bool ready() const{return connected;}
 uint16_t capabilities() const{return connected?0xffff:0;}
 const char *state()const{return connected?"ready":"disconnected";}
 uint32_t ownBootId()const{return 3653448153u;}
 bool hasReply()const{return connected;}uint32_t age(uint32_t)const{return 200;}
};
struct WifiConfig {char mode[7]={},ssid[33]={},password[65]={};bool persist=true;};
struct BrokerCapabilityState {
    uint32_t features=0;
    uint16_t wireCaps=0,extension=0,speed=0,wheel=0,radius=0;
    uint8_t pwm=0;
    bool ready=false,fresh=false,manual=false;
    bool operator==(const BrokerCapabilityState &b) const {
        return features==b.features && wireCaps==b.wireCaps && extension==b.extension &&
            speed==b.speed && wheel==b.wheel && radius==b.radius && pwm==b.pwm &&
            ready==b.ready && fresh==b.fresh && manual==b.manual;
    }
};

namespace before {
class Broker { public:

    MegaMock mega_; uint32_t wsBackpressure_=0,deferredClient_=0,wsOpened_=1,wsClosed_=0,wsMemoryDrops_=0;
    uint16_t wsCloseCode_=0;char wsCloseReason_[24]={};
    bool productReady_=true;uint32_t productAt_=4800,productStateAt_=4800,productFeatures_=0xffffffff;
    uint16_t extensionVersion_=AFR2_EXT_VERSION,speedLimit_=500,wheelMm_=80,radiusMm_=150;uint8_t pwmLimit_=255;
    bool sensorKnown_=true,radarRunning_=false,servoEnabled_=true;uint32_t sensorAt_=4800;
    bool wifiAwaitWeb_=false;uint32_t wifiJob_=0,wifiJobAt_=0;uint8_t wifiRequestedMode_=1;WifiConfig oledDraft_;
void capabilities(JsonObject out) ;
void status(JsonObject out,uint32_t now,uint32_t client) ;
void draftStatus(JsonObject out) ;
};
void Broker::capabilities(JsonObject out) {
    const char *names[]={"uart_bridge","mecanum_drive","wheel_odometry","goto_relative","line_array_5ch","ultrasonic_hcsr04","oled_128x64","ui_encoder_sw","sequence_runner","configuration","wheel_test","line_follow"};
    uint16_t caps=mega_.capabilities();
    bool fresh=productReady_ && mega_.ready() && (uint32_t)(millis()-productAt_)<2000;
    uint32_t f=fresh?productFeatures_:0;
    for(uint8_t i=0;i<12;i++) {
        JsonObject c=out[names[i]].to<JsonObject>();
        bool implemented=i==0?mega_.ready():i==1?(f&AFR2_EXT_F_MOTION):i==2?(f&AFR2_EXT_F_POSE):
            i==3?(extensionVersion_>=3 && (f&AFR2_EXT_F_GOTO)):i==4?(f&AFR2_EXT_F_LINE):i==5?(caps&AFR2_CAP_ULTRASONIC):
            (i==6 || i==7)?(f&AFR2_EXT_F_UI):i==8?(extensionVersion_>=4 && (f&AFR2_EXT_F_SEQUENCES)):
            i==9?(f&AFR2_EXT_F_CONFIG):i==10?(f&AFR2_EXT_F_WHEEL):((f&AFR2_EXT_F_LINE) && (f&AFR2_EXT_F_MOTION));
        if((i==1 || i==3 || i==8) && !speedLimit_)implemented=false;
        if(i==10 && !pwmLimit_)implemented=false;
        c["present"]=(i!=2 && i!=3) || implemented;c["implemented"]=implemented;c["calibrated"]=implemented;
        c["valid"]=implemented;c["freshness"]=implemented?"fresh":"unknown";
        c["reason"]=implemented?"Servicio negociado con ATmega; hardware sin certificar.":
            i==6 || i==7?"Esperando extension UI EX01.":i==10?"Habilitar motores; revisar fallo o enlace.":i==8?"Requiere EX01 v4, calibracion y presupuesto SRAM aprobado.":"Requiere productor y calibracion vigentes.";
        if(i==8){JsonObject limits=c["limits"].to<JsonObject>();limits["maxSegments"]=32;limits["maxDurationMs"]=120000;limits["stepMs"]=10;limits["distanceBlocks"]=fresh && extensionVersion_>=6 && (f&AFR2_EXT_F_DISTANCE);}
        if(i==1 || i==3 || i==8 || i==10) {
            JsonObject limits=c["limits"].to<JsonObject>();
            if(speedLimit_)limits["maxSpeedMmS"]=speedLimit_;
            if(pwmLimit_)limits["maxPwmPct"]=(uint16_t)pwmLimit_*100/255;
            if(wheelMm_)limits["wheelDiameterMm"]=wheelMm_;
            if(radiusMm_)limits["radiusSumMm"]=radiusMm_;
        }
    }
}
void Broker::status(JsonObject out,uint32_t now,uint32_t client) {
    out["outputBackpressure"]=wsBackpressure_;out["replyDeferred"]=deferredClient_!=0;
    JsonObject diagnostics=out["connectionDiagnostics"].to<JsonObject>();
    diagnostics["staConnected"]=WiFi.status()==WL_CONNECTED;diagnostics["wifiStatus"]=(int)WiFi.status();
    diagnostics["outputBackpressure"]=wsBackpressure_;diagnostics["replyDeferred"]=deferredClient_!=0;
    diagnostics["wsOpened"]=wsOpened_;diagnostics["wsClosed"]=wsClosed_;
    diagnostics["lastServerCloseCode"]=wsCloseCode_;diagnostics["lastServerCloseReason"]=wsCloseReason_;
    diagnostics["bootId"]=mega_.ownBootId();
    diagnostics["freeHeapBytes"]=ESP.getFreeHeap();diagnostics["maxFreeBlockBytes"]=ESP.getMaxFreeBlockSize();
    diagnostics["wsMemoryDrops"]=wsMemoryDrops_;
    out["megaLink"]=mega_.state(); out["controlMode"]="direct";out["extensionVersion"]=AFR2_EXT_VERSION;
    JsonObject head=out["sensorHead"].to<JsonObject>();head["known"]=sensorKnown_ && mega_.ready();head["running"]=radarRunning_;head["enabled"]=servoEnabled_;head["ageMs"]=(uint32_t)(now-sensorAt_); out["brokerTs"]=now;
    if (mega_.hasReply()) out["megaAgeMs"]=mega_.age(now); else out["megaAgeMs"]=nullptr;
    capabilities(out["capabilities"].to<JsonObject>());
    JsonObject pwm=out["capabilities"]["manual_pwm"].to<JsonObject>();
    bool manual=productReady_ && mega_.ready() && productStateAt_ && (uint32_t)(now-productStateAt_)<2000u && (productFeatures_&AFR2_EXT_F_MANUAL_PWM);
    pwm["present"]=true;pwm["implemented"]=manual;pwm["valid"]=manual;pwm["calibrated"]=false;pwm["freshness"]=manual?"fresh":"unknown";
    pwm["reason"]=manual?"PWM manual habilitado; sin odometria ni HC.":"Habilitar motores; revisar fallo, radar, calibracion, secuencia, tarea local o enlace.";
    pwm["limits"]["maxInputPct"]=100;pwm["limits"]["maxPwmPct"]=(uint16_t)pwmLimit_*100/255;

    const char *headNames[]={"sensor_head","radar_scanner"};
    for(const char *name:headNames){JsonObject capability=out["capabilities"][name].to<JsonObject>();bool available=productReady_ && mega_.ready() && (productFeatures_&(!strcmp(name,"radar_scanner")?AFR2_EXT_F_RADAR:AFR2_EXT_F_SENSOR_HEAD));capability["present"]=true;capability["implemented"]=available;capability["valid"]=available;capability["calibrated"]=false;capability["freshness"]=available?"fresh":"unknown";capability["reason"]="EX01v9; calibracion fisica pendiente.";}
    draftStatus(out["wifiDraft"].to<JsonObject>());
    JsonObject battery=out["capabilities"]["battery_voltage"].to<JsonObject>();
    bool batteryAvailable=productReady_ && mega_.ready() && (productFeatures_&AFR2_EXT_F_BATTERY);
    battery["present"]=true;battery["implemented"]=batteryAvailable;battery["valid"]=batteryAvailable;
    battery["calibrated"]=false;battery["freshness"]=batteryAvailable?"fresh":"unknown";
    battery["reason"]="EX01v9; A5, divisor 10k/10k, AVCC nominal; medicion explicita.";
}
void Broker::draftStatus(JsonObject out) {
    out["pending"]=wifiAwaitWeb_;out["jobId"]=wifiAwaitWeb_?wifiJob_:0;
    if(wifiAwaitWeb_){out["mode"]=modeName(wifiRequestedMode_);out["ssid"]=oledDraft_.ssid;out["expiresInMs"]=(uint32_t)(millis()-wifiJobAt_)<115000u?115000u-(uint32_t)(millis()-wifiJobAt_):0;}
}
}

namespace after {
class Broker { public:

    MegaMock mega_; uint32_t wsBackpressure_=0,deferredClient_=0,wsOpened_=1,wsClosed_=0,wsMemoryDrops_=0;
    uint16_t wsCloseCode_=0;char wsCloseReason_[24]={};
    bool productReady_=true;uint32_t productAt_=4800,productStateAt_=4800,productFeatures_=0xffffffff;
    uint16_t extensionVersion_=AFR2_EXT_VERSION,speedLimit_=500,wheelMm_=80,radiusMm_=150;uint8_t pwmLimit_=255;
    bool sensorKnown_=true,radarRunning_=false,servoEnabled_=true;uint32_t sensorAt_=4800;
    bool wifiAwaitWeb_=false;uint32_t wifiJob_=0,wifiJobAt_=0;uint8_t wifiRequestedMode_=1;WifiConfig oledDraft_;
void capabilities(JsonObject out,uint32_t now) ;
void status(JsonObject out,uint32_t now,uint32_t client,bool includeCapabilities) ;
void draftStatus(JsonObject out) ;
BrokerCapabilityState capabilityState(uint32_t now) const ;
void describeCapabilities(JsonObject out,uint32_t now) ;
};
void Broker::capabilities(JsonObject out,uint32_t now) {
    const char *names[]={"uart_bridge","mecanum_drive","wheel_odometry","goto_relative","line_array_5ch","ultrasonic_hcsr04","oled_128x64","ui_encoder_sw","sequence_runner","configuration","wheel_test","line_follow"};
    uint16_t caps=mega_.capabilities();
    bool fresh=productReady_ && mega_.ready() && (uint32_t)(now-productAt_)<2000;
    uint32_t f=fresh?productFeatures_:0;
    for(uint8_t i=0;i<12;i++) {
        JsonObject c=out[names[i]].to<JsonObject>();
        bool implemented=i==0?mega_.ready():i==1?(f&AFR2_EXT_F_MOTION):i==2?(f&AFR2_EXT_F_POSE):
            i==3?(extensionVersion_>=3 && (f&AFR2_EXT_F_GOTO)):i==4?(f&AFR2_EXT_F_LINE):i==5?(caps&AFR2_CAP_ULTRASONIC):
            (i==6 || i==7)?(f&AFR2_EXT_F_UI):i==8?(extensionVersion_>=4 && (f&AFR2_EXT_F_SEQUENCES)):
            i==9?(f&AFR2_EXT_F_CONFIG):i==10?(f&AFR2_EXT_F_WHEEL):((f&AFR2_EXT_F_LINE) && (f&AFR2_EXT_F_MOTION));
        if((i==1 || i==3 || i==8) && !speedLimit_)implemented=false;
        if(i==10 && !pwmLimit_)implemented=false;
        c["present"]=(i!=2 && i!=3) || implemented;c["implemented"]=implemented;c["calibrated"]=implemented;
        c["valid"]=implemented;c["freshness"]=implemented?"fresh":"unknown";
        c["reason"]=implemented?F("Servicio negociado con ATmega; hardware sin certificar."):
            i==6 || i==7?F("Esperando extension UI EX01."):i==10?F("Habilitar motores; revisar fallo o enlace."):i==8?F("Requiere EX01 v4, calibracion y presupuesto SRAM aprobado."):F("Requiere productor y calibracion vigentes.");
        if(i==8){JsonObject limits=c["limits"].to<JsonObject>();limits["maxSegments"]=32;limits["maxDurationMs"]=120000;limits["stepMs"]=10;limits["distanceBlocks"]=fresh && extensionVersion_>=6 && (f&AFR2_EXT_F_DISTANCE);}
        if(i==1 || i==3 || i==8 || i==10) {
            JsonObject limits=c["limits"].to<JsonObject>();
            if(speedLimit_)limits["maxSpeedMmS"]=speedLimit_;
            if(pwmLimit_)limits["maxPwmPct"]=(uint16_t)pwmLimit_*100/255;
            if(wheelMm_)limits["wheelDiameterMm"]=wheelMm_;
            if(radiusMm_)limits["radiusSumMm"]=radiusMm_;
        }
    }
}
void Broker::status(JsonObject out,uint32_t now,uint32_t client,bool includeCapabilities) {
    out["outputBackpressure"]=wsBackpressure_;out["replyDeferred"]=deferredClient_!=0;
    JsonObject diagnostics=out["connectionDiagnostics"].to<JsonObject>();
    diagnostics["staConnected"]=WiFi.status()==WL_CONNECTED;diagnostics["wifiStatus"]=(int)WiFi.status();
    diagnostics["outputBackpressure"]=wsBackpressure_;diagnostics["replyDeferred"]=deferredClient_!=0;
    diagnostics["wsOpened"]=wsOpened_;diagnostics["wsClosed"]=wsClosed_;
    diagnostics["lastServerCloseCode"]=wsCloseCode_;diagnostics["lastServerCloseReason"]=wsCloseReason_;
    diagnostics["bootId"]=mega_.ownBootId();
    diagnostics["freeHeapBytes"]=ESP.getFreeHeap();diagnostics["maxFreeBlockBytes"]=ESP.getMaxFreeBlockSize();
    diagnostics["wsMemoryDrops"]=wsMemoryDrops_;
    out["megaLink"]=mega_.state(); out["controlMode"]="direct";out["extensionVersion"]=AFR2_EXT_VERSION;
    JsonObject head=out["sensorHead"].to<JsonObject>();head["known"]=sensorKnown_ && mega_.ready();head["running"]=radarRunning_;head["enabled"]=servoEnabled_;head["ageMs"]=(uint32_t)(now-sensorAt_); out["brokerTs"]=now;
    if (mega_.hasReply()) out["megaAgeMs"]=mega_.age(now); else out["megaAgeMs"]=nullptr;
    draftStatus(out["wifiDraft"].to<JsonObject>());
    if(includeCapabilities)describeCapabilities(out,now);
}
void Broker::draftStatus(JsonObject out) {
    out["pending"]=wifiAwaitWeb_;out["jobId"]=wifiAwaitWeb_?wifiJob_:0;
    if(wifiAwaitWeb_){out["mode"]=modeName(wifiRequestedMode_);out["ssid"]=oledDraft_.ssid;out["expiresInMs"]=(uint32_t)(millis()-wifiJobAt_)<115000u?115000u-(uint32_t)(millis()-wifiJobAt_):0;}
}
BrokerCapabilityState Broker::capabilityState(uint32_t now) const {
    BrokerCapabilityState state;
    state.ready=mega_.ready();state.fresh=productReady_ && state.ready && (uint32_t)(now-productAt_)<2000u;
    state.features=productFeatures_;state.wireCaps=mega_.capabilities();state.extension=extensionVersion_;
    state.speed=speedLimit_;state.wheel=wheelMm_;state.radius=radiusMm_;state.pwm=pwmLimit_;
    state.manual=productReady_ && state.ready && productStateAt_ && (uint32_t)(now-productStateAt_)<2000u && (productFeatures_&AFR2_EXT_F_MANUAL_PWM);
    return state;
}
void Broker::describeCapabilities(JsonObject out,uint32_t now) {
    capabilities(out["capabilities"].to<JsonObject>(),now);
    JsonObject pwm=out["capabilities"]["manual_pwm"].to<JsonObject>();
    bool manual=productReady_ && mega_.ready() && productStateAt_ && (uint32_t)(now-productStateAt_)<2000u && (productFeatures_&AFR2_EXT_F_MANUAL_PWM);
    pwm["present"]=true;pwm["implemented"]=manual;pwm["valid"]=manual;pwm["calibrated"]=false;pwm["freshness"]=manual?"fresh":"unknown";
    pwm["reason"]=manual?F("PWM manual habilitado; sin odometria ni HC."):F("Habilitar motores; revisar fallo, radar, calibracion, secuencia, tarea local o enlace.");
    pwm["limits"]["maxInputPct"]=100;pwm["limits"]["maxPwmPct"]=(uint16_t)pwmLimit_*100/255;

    const char *headNames[]={"sensor_head","radar_scanner"};
    for(const char *name:headNames){JsonObject capability=out["capabilities"][name].to<JsonObject>();bool available=productReady_ && mega_.ready() && (productFeatures_&(!strcmp(name,"radar_scanner")?AFR2_EXT_F_RADAR:AFR2_EXT_F_SENSOR_HEAD));capability["present"]=true;capability["implemented"]=available;capability["valid"]=available;capability["calibrated"]=false;capability["freshness"]=available?"fresh":"unknown";capability["reason"]=F("EX01v9; calibracion fisica pendiente.");}
    JsonObject battery=out["capabilities"]["battery_voltage"].to<JsonObject>();
    bool batteryAvailable=productReady_ && mega_.ready() && (productFeatures_&AFR2_EXT_F_BATTERY);
    battery["present"]=true;battery["implemented"]=batteryAvailable;battery["valid"]=batteryAvailable;
    battery["calibrated"]=false;battery["freshness"]=batteryAvailable?"fresh":"unknown";
    battery["reason"]=F("EX01v9; A5, divisor 10k/10k, AVCC nominal; medicion explicita.");
}
}
struct BeforeRequest {
    uint32_t client=0;
    char id[65]={}, command[33]={};
    WifiConfig wifi;
    uint8_t wireCommand=0,bodyLength=0,body[56]={};
    uint32_t generation=0;
    char sequenceId[65]={},executionId[65]={};
};
struct AfterRequest {
    uint32_t client=0;
    char id[65]={}, command[33]={};
    struct SequenceIds { char sequenceId[65],executionId[65]; };
    // Credentials and sequence labels never belong to the same command.
    union { WifiConfig wifi{}; SequenceIds sequence; };
    uint8_t wireCommand=0,bodyLength=0,body[56]={};
    uint32_t generation=0;
};

void checkRequests(){
 AfterRequest wifi; std::strcpy(wifi.wifi.mode,"STA");std::strcpy(wifi.wifi.ssid,"fixture");std::strcpy(wifi.wifi.password,"not-a-real-password");wifi.body[0]=42;wifi.generation=17;
 AfterRequest queued=wifi;wifi=AfterRequest{};assert(std::strcmp(queued.wifi.ssid,"fixture")==0 && queued.body[0]==42 && queued.generation==17);
 AfterRequest seq;seq.sequence=AfterRequest::SequenceIds{};std::memset(seq.sequence.sequenceId,'s',64);std::memset(seq.sequence.executionId,'e',64);seq.body[55]=99;seq.generation=18;
 queued=seq;seq=AfterRequest{};assert(std::strlen(queued.sequence.sequenceId)==64 && std::strlen(queued.sequence.executionId)==64 && queued.body[55]==99 && queued.generation==18);
 queued=AfterRequest{};assert(queued.wifi.ssid[0]==0 && queued.wifi.password[0]==0);
}
int main(){checkRequests();std::cerr<<"Request sizes: "<<sizeof(BeforeRequest)<<" -> "<<sizeof(AfterRequest)<<" bytes\n";
{ before::Broker broker;broker.mega_.connected=true; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="res";frame["ok"]=true;frame["status"]="completed";frame["requestId"]="12345678-1234-1234-1234-123456789abc";frame["cmd"]="system.alive";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"before/ready/alive "<<output<<"\n"; }
{ before::Broker broker;broker.mega_.connected=true; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="evt";frame["event"]="system.status";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"before/ready/status "<<output<<"\n"; }
{ before::Broker broker;broker.mega_.connected=true; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="evt";frame["event"]="system.status";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"before/ready/changed "<<output<<"\n"; }
{ before::Broker broker;broker.mega_.connected=false; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="res";frame["ok"]=true;frame["status"]="completed";frame["requestId"]="12345678-1234-1234-1234-123456789abc";frame["cmd"]="system.alive";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"before/link-down/alive "<<output<<"\n"; }
{ before::Broker broker;broker.mega_.connected=false; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="evt";frame["event"]="system.status";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"before/link-down/status "<<output<<"\n"; }
{ before::Broker broker;broker.mega_.connected=false; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="evt";frame["event"]="system.status";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"before/link-down/changed "<<output<<"\n"; }
{ after::Broker broker;broker.mega_.connected=true; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();

        // Presence remains at 500 ms; permissions arrive via hello/status changes.
        payload["megaLink"]=broker.mega_.state();payload["brokerTs"]=5000;
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="res";frame["ok"]=true;frame["status"]="completed";frame["requestId"]="12345678-1234-1234-1234-123456789abc";frame["cmd"]="system.alive";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"after/ready/alive "<<output<<"\n"; }
{ after::Broker broker;broker.mega_.connected=true; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1,false);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="evt";frame["event"]="system.status";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"after/ready/status "<<output<<"\n"; }
{ after::Broker broker;broker.mega_.connected=true; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1,true);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="evt";frame["event"]="system.status";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"after/ready/changed "<<output<<"\n"; }
{ after::Broker broker;broker.mega_.connected=false; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();

        // Presence remains at 500 ms; permissions arrive via hello/status changes.
        payload["megaLink"]=broker.mega_.state();payload["brokerTs"]=5000;
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="res";frame["ok"]=true;frame["status"]="completed";frame["requestId"]="12345678-1234-1234-1234-123456789abc";frame["cmd"]="system.alive";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"after/link-down/alive "<<output<<"\n"; }
{ after::Broker broker;broker.mega_.connected=false; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1,false);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="evt";frame["event"]="system.status";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"after/link-down/status "<<output<<"\n"; }
{ after::Broker broker;broker.mega_.connected=false; JsonDocument frame; auto payload=frame["payload"].to<JsonObject>();
broker.status(payload,5000,1,true);
frame["schema"]="afr2-ws-1";frame["ts"]=5000;
frame["type"]="evt";frame["event"]="system.status";
assert(!frame.overflowed()); std::string output;serializeJson(frame,output);
std::cout<<"after/link-down/changed "<<output<<"\n"; }
}
