/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
#include <user_interface.h>
#include "afr2_build_info.h"
// AFR2-20261023-11. New afr2-ws-1 dispatcher. See provenance/esp-firmware.json.
#include "broker.h"
#include <string.h>
Broker::Session *Broker::session(uint32_t id) {
    for (auto &s:sessions_) if (s.used && s.id==id) return &s;
    return nullptr;
}
/* AFR2-20261029-27: keep transport evidence without writing debug to the Mega UART. */
void Broker::closeWs(AsyncWebSocketClient *client,uint16_t code,const char *reason){
    wsCloseCode_=code;strlcpy(wsCloseReason_,reason,sizeof(wsCloseReason_));client->close(code,reason);
}
void Broker::begin() {
    apJoin_=WiFi.onSoftAPModeStationConnected([this](const WiFiEventSoftAPModeStationConnected &e){networkEvent(AFR2_NET_CLIENT_JOIN,e.aid);});
    apLeave_=WiFi.onSoftAPModeStationDisconnected([this](const WiFiEventSoftAPModeStationDisconnected &e){networkEvent(AFR2_NET_CLIENT_LEAVE,e.aid);});
    networkEvent(AFR2_NET_BOOT);
    ws_.addMiddleware([](AsyncWebServerRequest *request, ArMiddlewareNext next) {
        String origin=request->header("Origin");
        if (origin==String("http://")+request->host()) next();
        else request->send(403,"text/plain","WebSocket origin rejected");
    });
    ws_.onEvent([this](AsyncWebSocket *,AsyncWebSocketClient *client,AwsEventType type,void *arg,uint8_t *data,size_t len) {
        event(client,type,arg,data,len);
    });
}
void Broker::event(AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    uint32_t id=client->id();
    if (type==WS_EVT_CONNECT) {
        for (auto &s:sessions_) if (!s.used) {
            ++wsOpened_;s={}; s.used=true; s.id=id; s.inputSeen=s.lastSeen=s.window=millis();networkEvent(AFR2_NET_WS_OPEN);return;
        }
        closeWs(client,1013,"Session limit"); return;
    }
    Session *s=session(id); if (!s) return;
    if (type==WS_EVT_DISCONNECT) {
        ++wsClosed_;networkEvent(AFR2_NET_WS_CLOSE);
        stopWebOperations();
        if(deferredClient_==id){deferredClient_=0;deferredReply_.reset();}
        if(scanEventClient_==id){scanEventClient_=0;scanEvent_.reset();}
        if(delayedErrorClient_==id)delayedErrorPending_=false;
        helloNotices_=0;
        *s=Session{}; return;
    }
    if (type!=WS_EVT_DATA) return;
    auto *info=static_cast<AwsFrameInfo *>(arg);
    // TCP segmentation of one final text frame is supported. RFC continuation frames are rejected.
    if (!info || !info->final || info->opcode!=WS_TEXT || info->len==0 || info->len>1024 ||
        info->index+len>info->len) { closeWs(client,1009,"Text frame limit"); return; }
    if (!info->index) { s->bytes=0; s->expected=(uint16_t)info->len; }
    if (info->index!=s->bytes || s->expected!=info->len) { closeWs(client,1002,"Frame order"); return; }
    memcpy(s->input+s->bytes,data,len); s->bytes+=(uint16_t)len;
    if (s->bytes==s->expected) {
        s->input[s->bytes]=0; ingest(*s);
        memset(s->input,0,sizeof(s->input)); s->bytes=s->expected=0;
    }
}
void Broker::ingest(Session &s) {
    JsonDocument doc;
    BrokerRequest req; req.client=s.id;
    auto failed=deserializeJson(doc,s.input,s.bytes,DeserializationOption::NestingLimit(5));
    if (failed || !doc.is<JsonObject>() || doc["schema"]!="afr2-ws-1" || doc["type"]!="req" ||
        !doc["requestId"].is<const char *>() || !doc["cmd"].is<const char *>() ||
        !doc["payload"].is<JsonObject>() || !doc["ts"].is<double>()) {
        if (auto *client=ws_.client(s.id)) closeWs(client,1008,"Invalid envelope");
        return;
    }
    const char *id=doc["requestId"], *command=doc["cmd"];
    if (!strlen(id) || strlen(id)>64 || !strlen(command) || strlen(command)>32 ||
        doc["requestId"].as<JsonString>().size()!=strlen(id) || doc["cmd"].as<JsonString>().size()!=strlen(command)) {
        if (auto *client=ws_.client(s.id)) closeWs(client,1008,"Identifier limit");
        return;
    }
    strlcpy(req.id,id,sizeof(req.id)); strlcpy(req.command,command,sizeof(req.command));
    if (!s.ready && strcmp(command,"system.hello") && strcmp(command,"robot.stop")) {
        error(req,"BAD_REQUEST",F("Primero enviar system.hello.")); return;
    }
    for (auto &recent:s.recent) if (!strcmp(recent,id)) {
        error(req,"BAD_REQUEST",F("requestId repetido en esta sesion.")); return;
    }
    for (uint8_t i=0;i<count_;++i) {
        const auto &queued=queue_[(tail_+i)%4];
        if (queued.client==req.client && !strcmp(queued.id,id)) {
            error(req,"BAD_REQUEST","requestId en cola."); return;
        }
    }
    if ((waiting_ && pending_.client==req.client && !strcmp(pending_.id,id)) ||
        (stopQueued_ && stop_.client==req.client && !strcmp(stop_.id,id))) {
        error(req,"BAD_REQUEST","requestId en vuelo."); return;
    }
    if(s.controlKind && !strcmp(s.controlId,id)){error(req,"BAD_REQUEST","Control requestId en vuelo.");return;}
    uint32_t now=millis();
    if ((uint32_t)(now-s.window)>=1000u) { s.window=now; s.rate=0; }
    if (++s.rate>40 && strcmp(command,"robot.stop")) {
        error(req,"BUSY_IN_FLIGHT",F("Limite de peticiones por sesion."),true); return;
    }
    JsonObject payload=doc["payload"];
    req.generation=generation_;
    if(!encodeProduct(req,payload))return;
    if(req.wireCommand==AFR2_EXT_DRIVE || req.wireCommand==AFR2_EXT_WHEEL || req.wireCommand==AFR2_EXT_MODE ||
       req.wireCommand==AFR2_EXT_GOTO || req.wireCommand==AFR2_EXT_SEQ_START)pwmStreamPending_=false;
    if(req.wireCommand==AFR2_EXT_DRIVE || req.wireCommand==AFR2_EXT_DRIVE_PWM || req.wireCommand==AFR2_EXT_WHEEL || req.wireCommand==AFR2_EXT_MODE ||
       req.wireCommand==AFR2_EXT_GOTO || req.wireCommand==AFR2_EXT_PREFS || req.wireCommand==AFR2_EXT_LINE_CAL ||
       (req.wireCommand==AFR2_EXT_CONFIG && req.body[4]) ||
       (req.wireCommand>=AFR2_EXT_SEQ_BEGIN && req.wireCommand<=AFR2_EXT_SEQ_START))s.inputSeen=now;
    if (!strcmp(command,"system.hello") &&
        (payload["client"]!="afr2-web" || !payload["clientVersion"].is<const char *>() || strlen(payload["clientVersion"])>15 ||
         payload["clientVersion"].as<JsonString>().size()!=strlen(payload["clientVersion"].as<const char *>()))) {
        error(req,"UNSUPPORTED_PRODUCT","Cliente no soportado."); return;
    }
    if(!strcmp(command,"system.alive")) {
        if(!payload["interactive"].is<bool>()){error(req,"BAD_REQUEST",F("Falta estado de presencia Web."));return;}
        if(payload["interactive"].as<bool>())s.inputSeen=millis();
    }
    if(!strcmp(command,"system.hello"))strlcpy((char *)req.body,payload["clientVersion"].as<const char *>(),16);
    if(!strcmp(command,"wifi.ap.fallback")){
        if(!payload["ask"].is<bool>()){error(req,"BAD_REQUEST","Falta ask booleano.");return;}
        req.body[0]=payload["ask"].as<bool>();req.bodyLength=1;
    }
    if(!strcmp(command,"wifi.mode")){
        if(!payload["mode"].is<const char *>() || payload["mode"].as<JsonString>().size()!=strlen(payload["mode"].as<const char *>())){error(req,"BAD_REQUEST","Modo Wi-Fi invalido.");return;}
        const char *mode=payload["mode"]|"";req.body[0]=!strcmp(mode,"STA")?1:!strcmp(mode,"AP")?2:!strcmp(mode,"AP_STA")?3:0;
        if(!req.body[0]){error(req,"BAD_REQUEST","Modo Wi-Fi invalido.");return;}req.bodyLength=1;
    }
    if(!strcmp(command,"wifi.credentials.get")){
        if(!payload["kind"].is<const char *>() || payload["kind"].as<JsonString>().size()!=strlen(payload["kind"].as<const char *>())){error(req,"BAD_REQUEST",F("Seleccion de credenciales invalida."));return;}
        const char *kind=payload["kind"]|"";
        req.body[0]=!strcmp(kind,"last-sta")?1:!strcmp(kind,"ap")?2:0;
        if(!req.body[0]){error(req,"BAD_REQUEST",F("Seleccionar last-sta o ap."));return;}
        req.bodyLength=1;
    }
    if (!strcmp(command,"wifi.configure") || !strcmp(command,"wifi.draft.submit")) {
        if (!payload["mode"].is<const char *>() || !payload["ssid"].is<const char *>() ||
            !payload["password"].is<const char *>() ||
            payload["mode"].as<JsonString>().size()!=strlen(payload["mode"].as<const char *>()) ||
            payload["ssid"].as<JsonString>().size()!=strlen(payload["ssid"].as<const char *>()) ||
            payload["password"].as<JsonString>().size()!=strlen(payload["password"].as<const char *>()) || strlen(payload["mode"])>=sizeof(req.wifi.mode) ||
            strlen(payload["ssid"])>=sizeof(req.wifi.ssid) || strlen(payload["password"])>=sizeof(req.wifi.password) ||
            (!payload["persist"].isNull() && !payload["persist"].is<bool>())) {
            error(req,"BAD_REQUEST",F("Configuracion Wi-Fi invalida.")); return;
        }
        strlcpy(req.wifi.mode,payload["mode"],sizeof(req.wifi.mode));
        strlcpy(req.wifi.ssid,payload["ssid"],sizeof(req.wifi.ssid));
        strlcpy(req.wifi.password,payload["password"],sizeof(req.wifi.password));
        req.wifi.persist=payload["persist"] | true;
        if(!req.wifi.persist){error(req,"BAD_REQUEST",F("La configuracion Wi-Fi requiere persistencia."));return;}
        if (!WifiService::valid(req.wifi)) { error(req,"BAD_REQUEST",F("SSID o frase WPA fuera de rango.")); return; }
    }
    /* Validated local session control never waits behind UART reads or captures.
     * One slot per session, consumed in loop before product background work. */
    if(!strcmp(command,"system.hello") || !strcmp(command,"system.alive")){
        if(s.controlKind){error(req,"BUSY_IN_FLIGHT",F("Control de sesion pendiente."),true);return;}
        s.controlKind=!strcmp(command,"system.hello")?1:2;
        strlcpy(s.controlId,req.id,sizeof(s.controlId));
        if(s.controlKind==1)strlcpy(s.controlVersion,(const char *)req.body,sizeof(s.controlVersion));
        s.lastSeen=now;strlcpy(s.recent[s.nextRecent],req.id,65);s.nextRecent=(s.nextRecent+1)%4;return;
    }
    if(req.wireCommand==AFR2_EXT_DRIVE_PWM){
        if(!mega_.ready() || !productReady_ || !productStateAt_ || (uint32_t)(now-productStateAt_)>=2000u || !(productFeatures_&AFR2_EXT_F_MANUAL_PWM)){
            error(req,"CAPABILITY_UNAVAILABLE",F("Habilitar motores; revisar fallo, calibracion o tarea local."),true);return;
        }
        memcpy(pwmStream_,req.body,12);pwmStreamPending_=true;pwmStreamAt_=now;
        pwmStreamClient_=s.id;pwmStreamGeneration_=generation_;
        JsonDocument reply;reply["status"]="accepted";reply["payload"]["queued"]=true;
        reply["payload"]["confirmed"]=false;answer(req,reply);
    } else if (!strcmp(command,"robot.stop") || !strcmp(command,"robot.sequence.cancel")) {
        if (stopQueued_ || (waiting_ && !strcmp(pending_.command,"robot.stop"))) {
            error(req,"BUSY_IN_FLIGHT","STOP ya en vuelo.",true); return;
        }
        // Fence every earlier queued action, including movement and sequence.start.
        ++generation_;req.generation=generation_;stop_=req; stopQueued_=true;
    } else {
        if (count_==4) { error(req,"BUSY_IN_FLIGHT","Cola del broker completa.",true); return; }
        queue_[head_]=req; head_=(head_+1)%4; ++count_;
    }
    s.lastSeen=now;
    strlcpy(s.recent[s.nextRecent],req.id,65); s.nextRecent=(s.nextRecent+1)%4;
    req.wifi=WifiConfig{};
}
AsyncWebSocketSharedBuffer Broker::makeFrame(JsonDocument &frame) {
    // ArduinoJson fails allocations without aborting; never send a partial success/status.
    const size_t length=measureJson(frame);
    // ESP8266's ordinary C++ new aborts on OOM. Leave room for the vector/control
    // block, queue nodes and TCP, and check fragmentation as well as total heap.
    // No yield between this check and allocation. These are conservative guards,
    // not a measured guarantee for every Wi-Fi/HTTP workload.
    if(frame.overflowed() || !length || length>8192 ||
       ESP.getFreeHeap()<length+4096u || ESP.getMaxFreeBlockSize()<length+256u){
        ++wsMemoryDrops_;frame.clear();return {};
    }
    auto buffer=std::make_shared<std::vector<uint8_t>>(length);
    const size_t written=serializeJson(frame,reinterpret_cast<char *>(buffer->data()),length);
    // The shared buffer is complete and owns its bytes. Release JSON pools before
    // text(sharedBuffer) starts TCP; that overload does not allocate another copy.
    frame.clear();
    if(written!=length){++wsMemoryDrops_;return {};}
    return buffer;
}
void Broker::answer(const BrokerRequest &req, JsonDocument &frame) {
    frame["schema"]="afr2-ws-1"; frame["requestId"]=req.id; frame["cmd"]=req.command; frame["ts"]=millis();
    if (frame["type"].isNull()) {
        frame["type"]="res"; frame["ok"]=true;
        if (frame["status"].isNull()) frame["status"]="completed";
    }
    if (auto *client=ws_.client(req.client)) {
        if(client->status()!=WS_CONNECTED)return;
        if(measureJson(frame)>8192){frame.clear();closeWs(client,1009,"Reply too large");return;}
        const bool carriesCapabilities=frame["payload"]["capabilities"].is<JsonObject>();
        const auto sentState=capabilityState(frame["payload"]["brokerTs"] | millis());
        auto buffer=makeFrame(frame);
        if(!buffer){closeWs(client,1013,"Reply memory pressure");return;}
        if (client->queueLen()!=0) {
            ++wsBackpressure_;
            if(!deferredClient_){
                deferredReply_=std::move(buffer);deferredClient_=req.client;deferredAt_=millis();
                if(carriesCapabilities)if(auto *s=session(req.client)){s->sentCapabilities=sentState;s->capabilitiesSent=true;}
            }
            else { closeWs(client,1008,"Too many pending replies"); }
            return;
        }
        if(!client->text(std::move(buffer)))closeWs(client,1013,"Reply queue unavailable");
        else if(carriesCapabilities)if(auto *s=session(req.client)){s->sentCapabilities=sentState;s->capabilitiesSent=true;}
    }
}
void Broker::error(const BrokerRequest &req,const char *code,const __FlashStringHelper *message,bool retryable,const char *source) {
    char text[129];strncpy_P(text,reinterpret_cast<PGM_P>(message),sizeof(text)-1);text[sizeof(text)-1]=0;
    error(req,code,text,retryable,source);
}
void Broker::error(const BrokerRequest &req,const char *code,const char *message,bool retryable,const char *source) {
    if(deferredClient_==req.client && req.client){
        auto *peer=ws_.client(req.client);
        if(peer && peer->queueIsFull()){
            if(delayedErrorPending_ && strcmp(delayedErrorId_,req.id)){closeWs(peer,1008,"Concurrent request limit");return;}
            delayedErrorPending_=true;delayedErrorClient_=req.client;delayedErrorRetryable_=retryable;
            strlcpy(delayedErrorId_,req.id,sizeof(delayedErrorId_));strlcpy(delayedErrorCommand_,req.command,sizeof(delayedErrorCommand_));
            strlcpy(delayedErrorCode_,code,sizeof(delayedErrorCode_));strlcpy(delayedErrorMessage_,message,sizeof(delayedErrorMessage_));strlcpy(delayedErrorSource_,source,sizeof(delayedErrorSource_));return;
        }
    }
    JsonDocument frame; frame["type"]="err"; frame["ok"]=false;
    JsonObject e=frame["error"].to<JsonObject>();
    e["code"]=code; e["message"]=message; e["retryable"]=retryable; e["source"]=source;
    answer(req,frame);
}
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
BrokerCapabilityState Broker::capabilityState(uint32_t now) const {
    BrokerCapabilityState state;
    state.ready=mega_.ready();state.fresh=productReady_ && state.ready && (uint32_t)(now-productAt_)<2000u;
    state.features=productFeatures_;state.wireCaps=mega_.capabilities();state.extension=extensionVersion_;
    state.speed=speedLimit_;state.wheel=wheelMm_;state.radius=radiusMm_;state.pwm=pwmLimit_;
    state.manual=productReady_ && state.ready && productStateAt_ && (uint32_t)(now-productStateAt_)<2000u && (productFeatures_&AFR2_EXT_F_MANUAL_PWM);
    return state;
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
void Broker::dispatch(const BrokerRequest &req,uint32_t now) {
    Session *s=session(req.client); if (!s) return;
    JsonDocument frame; JsonObject payload=frame["payload"].to<JsonObject>();
    if (!strcmp(req.command,"system.hello")) {
        s->ready=true;strlcpy(webVersion_,(const char *)req.body,16);wifiVersions_=0;
        payload["product"]="AutoFinalRobo2"; payload["brokerVersion"]=AFR2_FIRMWARE_VERSION;
        status(payload,now,req.client); storage_.health(payload["filesystem"].to<JsonObject>()); answer(req,frame);
        helloNotices_=1;
        return;
    }
    if((rebootAt_ || wifiResetPending_) && strcmp(req.command,"system.info") && strcmp(req.command,"system.alive") && strcmp(req.command,"robot.stop")){error(req,"BUSY_IN_FLIGHT","ESP reiniciando.",true);return;}
    if(req.generation!=generation_ && strcmp(req.command,"system.alive") && strcmp(req.command,"robot.stop") && strcmp(req.command,"robot.sequence.cancel")){error(req,"SUPERSEDED",F("Peticion anterior a STOP/revocacion."));return;}
    if(dispatchSystem(req,now))return;
    if (!strcmp(req.command,"system.alive")) {
        // Presence remains at 500 ms; permissions arrive via hello/status changes.
        payload["megaLink"]=mega_.state();payload["brokerTs"]=now;answer(req,frame);return;
    }
    if(!strcmp(req.command,"wifi.info")){
        wifi_.status(payload);payload["espBootId"]=mega_.ownBootId();payload["wsClients"]=readyWifiClients();auto *peer=ws_.client(req.client);
        bool ap=peer && peer->client() && peer->client()->localIP()==WiFi.softAPIP();
        payload["connectionInterface"]=ap?"AP":"STA";payload["connectionRssiDbm"]=nullptr;
        if(!ap && WiFi.status()==WL_CONNECTED)payload["connectionRssiDbm"]=WiFi.RSSI();
        payload["apClients"]=WiFi.softAPgetStationNum();payload["channel"]=WiFi.channel();payload["rssiSource"]="esp-sta";
        draftStatus(payload["wifiDraft"].to<JsonObject>());answer(req,frame);return;
    }
    if((!strcmp(req.command,"wifi.disconnect") || !strcmp(req.command,"wifi.retry") || !strcmp(req.command,"wifi.mode")) && (wifiPhase_==6 || wifiPhase_==7 || wifiAwaitWeb_ || wifiDraftReady_)){error(req,"BUSY_IN_FLIGHT",F("Hay una operacion Wi-Fi pendiente."),true);return;}
    if(!strcmp(req.command,"wifi.disconnect")){
        if(!wifi_.disconnectStation()){error(req,"BUSY_IN_FLIGHT",F("No se pudo solicitar la desconexion."),true);return;}
        wifi_.status(payload);bool observed=payload["staConnected"].is<bool>() && !payload["staConnected"].as<bool>();frame["status"]=observed?"completed":"accepted";payload["confirmed"]=observed;answer(req,frame);return;
    }
    if(!strcmp(req.command,"wifi.retry")){
        if(!wifi_.retry()){error(req,"BUSY_IN_FLIGHT",F("No se pudo solicitar la reconexion."),true);return;}
        wifi_.status(payload);bool observed=payload["staConnected"]|false;frame["status"]=observed?"completed":"accepted";payload["confirmed"]=observed;answer(req,frame);return;
    }
    if(!strcmp(req.command,"wifi.mode")){
        if(mega_.ready() && (!productReady_ || !productStopped_ || (uint32_t)(now-productStateAt_)>1500u)){error(req,"BUSY_IN_FLIGHT",F("Detener el auto antes de cambiar modo."),true);return;}
        uint8_t mode=req.body[0];if(!wifi_.changeMode(mode)){error(req,"BAD_REQUEST",F("Configurar primero las credenciales del modo elegido."));return;}
        wifi_.status(payload);payload["confirmed"]=false;payload["restartScheduled"]=wifi_.restartPending();payload["persisted"]=true;frame["status"]="accepted";answer(req,frame);return;
    }
    if(!strcmp(req.command,"wifi.ap.fallback") || !strcmp(req.command,"wifi.ap.stop")){
        bool ok=!strcmp(req.command,"wifi.ap.stop")?wifi_.stopAp():wifi_.setAskFallback(req.body[0]!=0);
        if(!ok){error(req,"BUSY_IN_FLIGHT",F("No se pudo aplicar el cambio de AP."),true);return;}
        wifi_.status(payload);payload["confirmed"]=true;answer(req,frame);publishStatus();return;
    }
    if (!strcmp(req.command,"wifi.status")) { wifi_.status(payload);payload["espBootId"]=mega_.ownBootId(); answer(req,frame); return; }
    if(!strcmp(req.command,"wifi.credentials.get")){
        if(!wifi_.credentials(req.body[0],payload)){error(req,"BAD_REQUEST",F("Consulta de credenciales invalida."));return;}
        // Per-request response only: never publishStatus, broadcast, UART or debug logs.
        answer(req,frame);return;
    }
    if(dispatchWifiDraft(req,now))return;
    if (!strcmp(req.command,"wifi.scan")) {
        if(scanEventClient_ || wifiPhase_!=0){error(req,"BUSY_IN_FLIGHT",F("Hay resultados Wi-Fi pendientes."),true);return;}
        if(wifiPhase_==6 || wifiPhase_==7 || wifiAwaitWeb_ || wifiDraftReady_){error(req,"BUSY_IN_FLIGHT",F("Hay una operacion Wi-Fi pendiente."),true);return;}
        if (!wifi_.scan()) { error(req,"BUSY_IN_FLIGHT","Servicio Wi-Fi ocupado.",true); return; }
        frame["status"]="accepted"; payload["status"]="started";payload["scanId"]=wifiScanId_==UINT32_MAX?1:wifiScanId_+1; answer(req,frame); return;
    }
    if (!strcmp(req.command,"wifi.configure")) {
        if (wifi_.busy() || wifiAwaitWeb_ || wifiDraftReady_ || wifiPhase_==6 || wifiPhase_==7) { error(req,"BUSY_IN_FLIGHT","Servicio Wi-Fi ocupado.",true); return; }
        if(mega_.ready() && (!productReady_ || !productStopped_ || (uint32_t)(now-productStateAt_)>1500u)){error(req,"BUSY_IN_FLIGHT",F("Detener el auto antes de configurar Wi-Fi."),true);return;}
        if (!wifi_.stage(req.wifi,now)) {
            error(req,"INTERNAL_ERROR",F("No se pudo guardar la configuracion; no fue aplicada.")); return;
        }
        frame["status"]="accepted"; payload["status"]="accepted"; payload["reconnectExpected"]=true;
        payload["persisted"]=true;payload["restartScheduled"]=true; answer(req,frame); return;
    }
    if(req.generation!=generation_ && strcmp(req.command,"robot.stop")){error(req,"SUPERSEDED",F("Peticion anterior a STOP/revocacion."));return;}
    if(dispatchProduct(req,now))return;
    uint8_t cmd=!strcmp(req.command,"robot.stop")?AFR2_CMD_STOP:
        !strcmp(req.command,"robot.state")?AFR2_CMD_STATE:
        !strcmp(req.command,"robot.sensors")?AFR2_CMD_SENSORS:
        !strcmp(req.command,"robot.encoders")?AFR2_CMD_ENCODERS:0;
    if (!cmd) {
        const char *known[]={"robot.drive","robot.mode","robot.test.wheel","robot.calibrate","robot.goto.start","robot.goto.cancel","robot.screen.get","robot.ui.action"};
        for (const char *name:known) if (!strcmp(req.command,name)) {
            error(req,"CAPABILITY_UNAVAILABLE",F("Servicio Mega aun no integrado; control deshabilitado.")); return;
        }
        error(req,"BAD_REQUEST",F("Comando fuera del catalogo AFR2.")); return;
    }
    if (!mega_.ready()) { error(req,"MEGA_LINK_DOWN",F("Sin negociacion vigente con ATmega."),true); return; }
    bool stop=cmd==AFR2_CMD_STOP;
    if (waiting_ && stop) {
        error(pending_,"SUPERSEDED",F("Peticion reemplazada por STOP.")); waiting_=false;
    }
    if (waiting_ || !mega_.request(cmd,now,stop)) {
        error(req,"BUSY_IN_FLIGHT","UART ocupada; reintentar.",true); return;
    }
    pending_=req; waiting_=true;
}
static void ageField(JsonObject object,const char *key,uint32_t age,bool valid) {
    if (valid && age!=AFR2_AGE_UNKNOWN) object[key]=age; else object[key]=nullptr;
}
void Broker::finish(const MegaResult &r) {
    if (!waiting_) return;
    BrokerRequest req=pending_; waiting_=false;
    if(finishSystem(req,r))return;
    if(req.client==0 && finishNetwork(req,r))return;
    if(req.client==0 && (!strcmp(req.command,"_diag_poll") || !strcmp(req.command,"_diag_result"))){
        if(!r.timeout && !r.transportFault && r.length>=18 && r.payload[8]==AFR2_RESULT_OK && !strcmp(req.command,"_diag_poll") && r.payload[9]){
            uint32_t job=afr2_get_u32(r.payload+10);
            if(job!=diagJob_){diagJob_=job;diagPattern_=afr2_get_u32(r.payload+14);diagReply_=true;}
        }
        return;
    }
    if(req.client==0 && finishWifi(req,r))return;
    if(req.client && (!session(req.client) || (req.generation!=generation_ && strcmp(req.command,"robot.stop") && strcmp(req.command,"robot.sequence.cancel")))) {
        error(req,"SUPERSEDED",F("Respuesta de una operacion revocada."));return;
    }
    if(req.client==0 && !strcmp(req.command,"_web_pulse"))return;
    if(req.client==0 && (!strcmp(req.command,"_ext") || !strcmp(req.command,"_state") || !strcmp(req.command,"_limits"))) {
        if(r.timeout || r.transportFault || r.payload[8]!=AFR2_RESULT_OK){productReady_=false;return;}
        if(r.command==AFR2_EXT_HELLO){extensionVersion_=afr2_get_u16(r.payload+13);productFeatures_=afr2_get_u32(r.payload+15);productReady_=extensionVersion_==AFR2_EXT_VERSION;}
        else if(r.command==AFR2_EXT_LIMITS){speedLimit_=afr2_get_u16(r.payload+9);pwmLimit_=r.payload[11];wheelMm_=afr2_get_u16(r.payload+13);radiusMm_=afr2_get_u16(r.payload+15);limitsAt_=millis();}
        else {productStopped_=r.payload[11]!=0;productStateAt_=millis();productMode_=r.payload[9];productOrigin_=r.payload[10];productFeatures_=afr2_get_u32(r.payload+17);}
        return;
    }
    if (r.transportFault) { error(req,"MEGA_LINK_DOWN",F("Error RX UART; sesion revocada."),true); return; }
    if (r.timeout) { error(req,"TIMEOUT",F("ATmega no respondio; resultado desconocido."),true); return; }
    if (r.payload[8]!=AFR2_RESULT_OK) {
        if(r.payload[8]==AFR2_RESULT_UNAVAILABLE &&
           (req.wireCommand==AFR2_EXT_UI_META || req.wireCommand==AFR2_EXT_UI_ACTION)) {
            error(req,"BUSY_IN_FLIGHT",F("OLED ocupada o captura invalidada por entrada local; actualizar antes de operar."),true,"atmega");return;
        }
        error(req,r.payload[8]==AFR2_RESULT_SESSION?"MEGA_LINK_DOWN":
            r.payload[8]==AFR2_RESULT_BAD_REQUEST?"BAD_REQUEST":r.payload[8]==AFR2_RESULT_INTERNAL?"INTERNAL_ERROR":"CAPABILITY_UNAVAILABLE",
            "ATmega no confirmo la operacion.",false,"atmega"); return;
    }
    if(finishProduct(req,r))return;
    JsonDocument frame; JsonObject payload=frame["payload"].to<JsonObject>();
    const uint8_t *p=r.payload;
    if (r.command==AFR2_CMD_STOP) {
        if (p[9]!=1 || (p[10]!=0 && p[10]!=5)) { error(req,"INTERNAL_ERROR","Respuesta STOP invalida.",false,"atmega"); return; }
        radarRunning_=servoEnabled_=false;sensorKnown_=true;sensorAt_=millis();payload["radarStopped"]=true;payload["servoEnabled"]=false;
        payload["stopped"]=true; payload["mode"]=p[10]==5?"fault":"idle";productOrigin_=0;
        publishStatus();
    } else if (r.command==AFR2_CMD_STATE) {
        const char *modes[]={"idle","manual","line-follow","wheel-test","goto","fault","sequence"};
        payload["mode"]=p[9]<7?modes[p[9]]:"unknown";
        if (p[10]<2) payload["stopped"]=p[10]==1; else payload["stopped"]=nullptr;
         ageField(payload,"ageMs",afr2_get_u32(p+12),p[9]<7);
    } else if (r.command==AFR2_CMD_SENSORS) {
        bool valid=p[9] && afr2_get_u32(p+11)!=AFR2_AGE_UNKNOWN && afr2_get_u32(p+11)<=2000;
        JsonObject line=payload["line"].to<JsonObject>(); line["valid"]=valid;
        if (valid) {
            line["bits"]=p[10]&31; JsonArray ch=line["channels"].to<JsonArray>();
            for (uint8_t i=0;i<5;++i) ch.add((p[10]&(1u<<i))!=0);
        } else { line["bits"]=nullptr; line["channels"]=nullptr; }
        ageField(line,"ageMs",afr2_get_u32(p+11),p[9]!=0);
        JsonObject us=payload["ultrasonic"].to<JsonObject>();
        valid=p[15] && afr2_get_u32(p+18)!=AFR2_AGE_UNKNOWN && afr2_get_u32(p+18)<=2000;
        us["valid"]=valid;
        if (valid) us["distanceMm"]=afr2_get_u16(p+16); else us["distanceMm"]=nullptr;
        ageField(us,"ageMs",afr2_get_u32(p+18),p[15]!=0);
    } else if (r.command==AFR2_CMD_ENCODERS) {
        bool valid=p[9] && p[10] && afr2_get_u32(p+27)!=AFR2_AGE_UNKNOWN && afr2_get_u32(p+27)<=2000;
        payload["wired"]=p[9]!=0; payload["valid"]=valid;
        const char *names[]={"fl","fr","rl","rr"}; JsonObject counts=payload["counts"].to<JsonObject>();
        for (uint8_t i=0;i<4;++i) {
            if (valid) counts[names[i]]=(int32_t)afr2_get_u32(p+11+4*i); else counts[names[i]]=nullptr;
        }
        ageField(payload,"ageMs",afr2_get_u32(p+27),p[10]!=0);
    }
    answer(req,frame);
}
void Broker::broadcast(const char *name,JsonDocument &payload) {
    const bool scan=!strcmp(name,"wifi.scan.result");
    bool wanted=false;
    for(const auto &s:sessions_)if(s.used && s.ready)if(auto *client=ws_.client(s.id)){
        if(client->status()==WS_CONNECTED && (scan || optionalOutput(client))){wanted=true;break;}
    }
    if(!wanted)return;
    JsonDocument frame; frame["schema"]="afr2-ws-1"; frame["type"]="evt"; frame["event"]=name;
    frame["payload"]=payload.as<JsonObject>(); frame["ts"]=millis();
    // Every caller transfers this temporary payload; release the duplicate tree.
    const bool overflow=payload.overflowed();payload.clear();
    if(overflow){++wsMemoryDrops_;return;}
    auto buffer=makeFrame(frame);if(!buffer)return;
    if(scan){
        for(auto &s:sessions_)if(s.used && s.ready){scanEvent_=std::move(buffer);scanEventClient_=s.id;return;}
        return;
    }
    for (auto &s:sessions_) if (s.used && s.ready) if (auto *client=ws_.client(s.id)) {
        if (optionalOutput(client)) client->text(buffer); // Disposable events share the transport buffer.

    }
}
void Broker::tick(uint32_t now) {
    // Preserve both OLED pages during an active transfer, expire inactive results.
    if(wifiScanAt_ && wifiPhase_!=5 && wifiPhase_!=9 && (uint32_t)(now-wifiScanAt_)>=120000u)clearWifiNetworks();
    if(deferredClient_){
        auto *peer=ws_.client(deferredClient_);
        if(!peer || peer->status()!=WS_CONNECTED){deferredClient_=0;deferredReply_.reset();}
        else if(peer->queueLen()==0){
            if(!peer->text(deferredReply_))closeWs(peer,1013,"Reply queue unavailable");
            deferredClient_=0;deferredReply_.reset();
        }
        else if((uint32_t)(now-deferredAt_)>=2000u){closeWs(peer,1013,"Slow consumer timeout");deferredClient_=0;deferredReply_.reset();}
    }
    if(!deferredClient_ && delayedErrorPending_){
        BrokerRequest req;req.client=delayedErrorClient_;strlcpy(req.id,delayedErrorId_,sizeof(req.id));strlcpy(req.command,delayedErrorCommand_,sizeof(req.command));
        delayedErrorPending_=false;error(req,delayedErrorCode_,delayedErrorMessage_,delayedErrorRetryable_,delayedErrorSource_);
    }
    if(scanEventClient_){
        auto *peer=ws_.client(scanEventClient_);
        if(!peer || peer->status()!=WS_CONNECTED){scanEventClient_=0;scanEvent_.reset();}
        else if(optionalOutput(peer) && peer->text(scanEvent_)){scanEventClient_=0;scanEvent_.reset();}
    }
    if(helloNotices_ && !deferredClient_){
        for(auto &s:sessions_)if(s.used && s.ready)if(auto *peer=ws_.client(s.id))if(optionalOutput(peer)){
            if(helloNotices_==1){publishBoot("esp",mega_.ownBootId(),ESP.getResetInfoPtr()?ESP.getResetInfoPtr()->reason:255,"session-snapshot");helloNotices_=infoReady_?2:0;}
            else{publishBoot("atmega",infoBoot_,infoReset_,"identity-snapshot");helloNotices_=0;}
        }
    }
    if(stopQueued_){BrokerRequest req=stop_;stopQueued_=false;dispatch(req,now);}
    for(auto &s:sessions_)if(s.used && s.controlKind && !deferredClient_){
        BrokerRequest req;req.client=s.id;req.generation=generation_;
        strlcpy(req.id,s.controlId,sizeof(req.id));strlcpy(req.command,s.controlKind==1?"system.hello":"system.alive",sizeof(req.command));
        if(s.controlKind==1)strlcpy((char *)req.body,s.controlVersion,16);
        s.controlKind=0;dispatch(req,now);
    }

    if(!mega_.ready() || !productReady_){networkLinked_=false;networkPending_=true;networkPage_=0;wifiResetPending_=false;}
    // AFR2-20261108-06: hay un OLED que puede contestar la pregunta "sin Wi-Fi".
    wifi_.setAskMode(mega_.ready() && productReady_ && extensionVersion_>=5 && (productFeatures_&AFR2_EXT_F_NETWORK_UI),now);
    /* AFR2-20261106-10: collect the finished UART reply and serve the Web queue BEFORE background
     * polls. Previously tickProduct took every free slot first (pulse/state/Wi-Fi/network/head/info),
     * so robot.drive.pwm waited until the Web 600 ms timeout fired STOP. Safety halt still preempts. */
    MegaResult result;
    if (!deferredClient_ && mega_.take(result)) finish(result);
    if (stopQueued_) { BrokerRequest req=stop_; stopQueued_=false; dispatch(req,now); }
    // Session pulses/halt retain priority; manual PWM never awaits AVR feedback.
    if(pwmStreamPending_ && (uint32_t)(now-webPulseAt_)>=500u)tickProduct(now);
    if(pwmStreamPending_){
        auto *owner=session(pwmStreamClient_);
        if(!owner || !owner->ready || pwmStreamGeneration_!=generation_ || safetyRequested_ || stopQueued_ ||
           !mega_.ready() || !productReady_ || !(productFeatures_&AFR2_EXT_F_MANUAL_PWM) || !productStateAt_ || (uint32_t)(now-productStateAt_)>=2000u || (uint32_t)(now-pwmStreamAt_)>=200u)pwmStreamPending_=false;
        else if(!waiting_ && (uint32_t)(now-webPulseAt_)<500u && mega_.streamPwm(pwmStream_,now))pwmStreamPending_=false;
    }
    bool localFront=count_ && (!strncmp(queue_[tail_].command,"wifi.",5) || !strncmp(queue_[tail_].command,"system.",7));
    if (!deferredClient_ && count_ && (localFront || (!waiting_ && !mega_.busy()))) {
        BrokerRequest req=queue_[tail_]; queue_[tail_]=BrokerRequest{};
        tail_=(tail_+1)%4; --count_; dispatch(req,now); req.wifi=WifiConfig{};
    }
    tickProduct(now);tickSystem(now);
    for (auto &s:sessions_) if (s.used && (int32_t)(now-s.lastSeen)>30000) {
        if (auto *client=ws_.client(s.id)) closeWs(client,1001,"Heartbeat expired");
    }
    // Dedicated STOP slot bypasses the ordinary queue, including an in-flight read.
    if (stopQueued_) { BrokerRequest req=stop_; stopQueued_=false; dispatch(req,now); }
    if (!deferredClient_ && mega_.take(result)) finish(result);
    if ((uint32_t)(now-statusAt_)>=1000u) {
        statusAt_=now;publishStatus();
    }
    JsonDocument scan;
    if (wifi_.takeScan(scan.to<JsonObject>())) {scanForOled(scan.as<JsonObjectConst>());scan["scanId"]=wifiScanId_;broadcast("wifi.scan.result",scan);}
}
