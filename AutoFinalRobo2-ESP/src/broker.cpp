#include <user_interface.h>
#include "afr2_build_info.h"
// AFR2-20261023-11. New afr2-ws-1 dispatcher. See provenance/esp-firmware.json.
#include "broker.h"
#include <string.h>
Broker::Session *Broker::session(uint32_t id) {
    for (auto &s:sessions_) if (s.used && s.id==id) return &s;
    return nullptr;
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
            s={}; s.used=true; s.id=id; s.inputSeen=s.lastSeen=s.window=millis();networkEvent(AFR2_NET_WS_OPEN);return;
        }
        client->close(1013,"Session limit"); return;
    }
    Session *s=session(id); if (!s) return;
    if (type==WS_EVT_DISCONNECT) {
        networkEvent(AFR2_NET_WS_CLOSE);
        stopWebOperations();
        *s=Session{}; return;
    }
    if (type!=WS_EVT_DATA) return;
    auto *info=static_cast<AwsFrameInfo *>(arg);
    // TCP segmentation of one final text frame is supported. RFC continuation frames are rejected.
    if (!info || !info->final || info->opcode!=WS_TEXT || info->len==0 || info->len>1024 ||
        info->index+len>info->len) { client->close(1009,"Text frame limit"); return; }
    if (!info->index) { s->bytes=0; s->expected=(uint16_t)info->len; }
    if (info->index!=s->bytes || s->expected!=info->len) { client->close(1002,"Frame order"); return; }
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
        if (auto *client=ws_.client(s.id)) client->close(1008,"Invalid envelope");
        return;
    }
    const char *id=doc["requestId"], *command=doc["cmd"];
    if (!strlen(id) || strlen(id)>64 || !strlen(command) || strlen(command)>32 ||
        doc["requestId"].as<JsonString>().size()!=strlen(id) || doc["cmd"].as<JsonString>().size()!=strlen(command)) {
        if (auto *client=ws_.client(s.id)) client->close(1008,"Identifier limit");
        return;
    }
    strlcpy(req.id,id,sizeof(req.id)); strlcpy(req.command,command,sizeof(req.command));
    if (!s.ready && strcmp(command,"system.hello") && strcmp(command,"robot.stop")) {
        error(req,"BAD_REQUEST","Primero enviar system.hello."); return;
    }
    for (auto &recent:s.recent) if (!strcmp(recent,id)) {
        error(req,"BAD_REQUEST","requestId repetido en esta sesion."); return;
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
    uint32_t now=millis();
    if ((uint32_t)(now-s.window)>=1000u) { s.window=now; s.rate=0; }
    if (++s.rate>40 && strcmp(command,"robot.stop")) {
        error(req,"BUSY_IN_FLIGHT","Limite de peticiones por sesion.",true); return;
    }
    JsonObject payload=doc["payload"];
    req.generation=generation_;
    if(!encodeProduct(req,payload))return;
    if(req.wireCommand==AFR2_EXT_DRIVE || req.wireCommand==AFR2_EXT_WHEEL || req.wireCommand==AFR2_EXT_MODE ||
       req.wireCommand==AFR2_EXT_GOTO || req.wireCommand==AFR2_EXT_PREFS || req.wireCommand==AFR2_EXT_LINE_CAL ||
       (req.wireCommand==AFR2_EXT_CONFIG && req.body[4]) ||
       (req.wireCommand>=AFR2_EXT_SEQ_BEGIN && req.wireCommand<=AFR2_EXT_SEQ_START))s.inputSeen=now;
    if (!strcmp(command,"system.hello") &&
        (payload["client"]!="afr2-web" || !payload["clientVersion"].is<const char *>() || strlen(payload["clientVersion"])>15 ||
         payload["clientVersion"].as<JsonString>().size()!=strlen(payload["clientVersion"].as<const char *>()))) {
        error(req,"UNSUPPORTED_PRODUCT","Cliente no soportado."); return;
    }
    if(!strcmp(command,"system.alive")) {
        if(!payload["interactive"].is<bool>()){error(req,"BAD_REQUEST","Falta estado de presencia Web.");return;}
        if(payload["interactive"].as<bool>())s.inputSeen=millis();
    }
    if(!strcmp(command,"system.hello"))strlcpy((char *)req.body,payload["clientVersion"].as<const char *>(),16);
    if (!strcmp(command,"wifi.configure") || !strcmp(command,"wifi.draft.submit")) {
        if (!payload["mode"].is<const char *>() || !payload["ssid"].is<const char *>() ||
            !payload["password"].is<const char *>() ||
            payload["mode"].as<JsonString>().size()!=strlen(payload["mode"].as<const char *>()) ||
            payload["ssid"].as<JsonString>().size()!=strlen(payload["ssid"].as<const char *>()) ||
            payload["password"].as<JsonString>().size()!=strlen(payload["password"].as<const char *>()) || strlen(payload["mode"])>=sizeof(req.wifi.mode) ||
            strlen(payload["ssid"])>=sizeof(req.wifi.ssid) || strlen(payload["password"])>=sizeof(req.wifi.password) ||
            (!payload["persist"].isNull() && !payload["persist"].is<bool>())) {
            error(req,"BAD_REQUEST","Configuracion Wi-Fi invalida."); return;
        }
        strlcpy(req.wifi.mode,payload["mode"],sizeof(req.wifi.mode));
        strlcpy(req.wifi.ssid,payload["ssid"],sizeof(req.wifi.ssid));
        strlcpy(req.wifi.password,payload["password"],sizeof(req.wifi.password));
        req.wifi.persist=payload["persist"] | true;
        if (!WifiService::valid(req.wifi)) { error(req,"BAD_REQUEST","SSID o frase WPA fuera de rango."); return; }
    }
    if (!strcmp(command,"robot.stop") || !strcmp(command,"robot.sequence.cancel")) {
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
void Broker::answer(const BrokerRequest &req, JsonDocument &frame) {
    frame["schema"]="afr2-ws-1"; frame["requestId"]=req.id; frame["cmd"]=req.command; frame["ts"]=millis();
    if (frame["type"].isNull()) {
        frame["type"]="res"; frame["ok"]=true;
        if (frame["status"].isNull()) frame["status"]="completed";
    }
    if (auto *client=ws_.client(req.client)) {
        if (client->queueIsFull()) { client->close(1013,"Slow consumer"); return; }
        String text; serializeJson(frame,text); client->text(text);
    }
}
void Broker::error(const BrokerRequest &req,const char *code,const char *message,bool retryable,const char *source) {
    JsonDocument frame; frame["type"]="err"; frame["ok"]=false;
    JsonObject e=frame["error"].to<JsonObject>();
    e["code"]=code; e["message"]=message; e["retryable"]=retryable; e["source"]=source;
    answer(req,frame);
}
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
            i==6 || i==7?"Esperando extension UI EX01.":i==8?"Requiere EX01 v4, calibracion y presupuesto SRAM aprobado.":"Requiere productor y calibracion vigentes.";
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
    out["megaLink"]=mega_.state(); out["controlMode"]="direct"; out["brokerTs"]=now;
    if (mega_.hasReply()) out["megaAgeMs"]=mega_.age(now); else out["megaAgeMs"]=nullptr;
    capabilities(out["capabilities"].to<JsonObject>());
    draftStatus(out["wifiDraft"].to<JsonObject>());
}
void Broker::dispatch(const BrokerRequest &req,uint32_t now) {
    Session *s=session(req.client); if (!s) return;
    JsonDocument frame; JsonObject payload=frame["payload"].to<JsonObject>();
    if (!strcmp(req.command,"system.hello")) {
        s->ready=true;strlcpy(webVersion_,(const char *)req.body,16);wifiVersions_=0;
        payload["product"]="AutoFinalRobo2"; payload["brokerVersion"]=AFR2_FIRMWARE_VERSION;
        status(payload,now,req.client); storage_.health(payload["filesystem"].to<JsonObject>()); answer(req,frame);
        publishBoot("esp",mega_.ownBootId(),ESP.getResetInfoPtr()?ESP.getResetInfoPtr()->reason:255,"session-snapshot");
        if(infoReady_)publishBoot("atmega",infoBoot_,infoReset_,"identity-snapshot");
        return;
    }
    if((rebootAt_ || wifiResetPending_) && strcmp(req.command,"system.info") && strcmp(req.command,"system.alive") && strcmp(req.command,"robot.stop")){error(req,"BUSY_IN_FLIGHT","ESP reiniciando.",true);return;}
    if(!strcmp(req.command,"system.reboot") && req.generation!=generation_){error(req,"SUPERSEDED","Reinicio anterior a STOP/revocacion.");return;}
    if(dispatchSystem(req,now))return;
    if (!strcmp(req.command,"system.alive")) { status(payload,now,req.client); answer(req,frame); return; }
    if (!strcmp(req.command,"wifi.status")) { wifi_.status(payload); answer(req,frame); return; }
    if(dispatchWifiDraft(req,now))return;
    if (!strcmp(req.command,"wifi.scan")) {
        if (!wifi_.scan()) { error(req,"BUSY_IN_FLIGHT","Servicio Wi-Fi ocupado.",true); return; }
        frame["status"]="accepted"; payload["status"]="started"; answer(req,frame); return;
    }
    if (!strcmp(req.command,"wifi.configure")) {
        if (wifi_.busy() || wifiAwaitWeb_ || wifiDraftReady_ || wifiPhase_==6) { error(req,"BUSY_IN_FLIGHT","Servicio Wi-Fi ocupado.",true); return; }
        if (!wifi_.stage(req.wifi,now)) {
            error(req,"INTERNAL_ERROR","No se pudo guardar la configuracion; no fue aplicada."); return;
        }
        frame["status"]="accepted"; payload["status"]="accepted"; payload["reconnectExpected"]=true;
        payload["persisted"]=req.wifi.persist; answer(req,frame); return;
    }
    if(req.generation!=generation_ && strcmp(req.command,"robot.stop")){error(req,"SUPERSEDED","Peticion anterior a STOP/revocacion.");return;}
    if(dispatchProduct(req,now))return;
    uint8_t cmd=!strcmp(req.command,"robot.stop")?AFR2_CMD_STOP:
        !strcmp(req.command,"robot.state")?AFR2_CMD_STATE:
        !strcmp(req.command,"robot.sensors")?AFR2_CMD_SENSORS:
        !strcmp(req.command,"robot.encoders")?AFR2_CMD_ENCODERS:0;
    if (!cmd) {
        const char *known[]={"robot.drive","robot.mode","robot.test.wheel","robot.calibrate","robot.goto.start","robot.goto.cancel","robot.screen.get","robot.ui.action"};
        for (const char *name:known) if (!strcmp(req.command,name)) {
            error(req,"CAPABILITY_UNAVAILABLE","Servicio Mega aun no integrado; control deshabilitado."); return;
        }
        error(req,"BAD_REQUEST","Comando fuera del catalogo AFR2."); return;
    }
    if (!mega_.ready()) { error(req,"MEGA_LINK_DOWN","Sin negociacion vigente con ATmega.",true); return; }
    bool stop=cmd==AFR2_CMD_STOP;
    if (waiting_ && stop) {
        error(pending_,"SUPERSEDED","Peticion reemplazada por STOP."); waiting_=false;
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
    if(req.client==0 && finishWifi(req,r))return;
    if(req.client && (!session(req.client) || (req.generation!=generation_ && strcmp(req.command,"robot.stop") && strcmp(req.command,"robot.sequence.cancel")))) {
        error(req,"SUPERSEDED","Respuesta de una operacion revocada.");return;
    }
    if(req.client==0 && !strcmp(req.command,"_web_pulse"))return;
    if(req.client==0 && (!strcmp(req.command,"_ext") || !strcmp(req.command,"_state") || !strcmp(req.command,"_limits"))) {
        if(r.timeout || r.transportFault || r.payload[8]!=AFR2_RESULT_OK){productReady_=false;return;}
        if(r.command==AFR2_EXT_HELLO){extensionVersion_=afr2_get_u16(r.payload+13);productFeatures_=afr2_get_u32(r.payload+15);productReady_=extensionVersion_==7;}
        else if(r.command==AFR2_EXT_LIMITS){speedLimit_=afr2_get_u16(r.payload+9);pwmLimit_=r.payload[11];wheelMm_=afr2_get_u16(r.payload+13);radiusMm_=afr2_get_u16(r.payload+15);limitsAt_=millis();}
        else {productStopped_=r.payload[11]!=0;productStateAt_=millis();productMode_=r.payload[9];productOrigin_=r.payload[10];productFeatures_=afr2_get_u32(r.payload+17);}
        return;
    }
    if (r.transportFault) { error(req,"MEGA_LINK_DOWN","Error RX UART; sesion revocada.",true); return; }
    if (r.timeout) { error(req,"TIMEOUT","ATmega no respondio; resultado desconocido.",true); return; }
    if (r.payload[8]!=AFR2_RESULT_OK) {
        if(r.payload[8]==AFR2_RESULT_UNAVAILABLE &&
           (req.wireCommand==AFR2_EXT_UI_META || req.wireCommand==AFR2_EXT_UI_ACTION)) {
            error(req,"BUSY_IN_FLIGHT","OLED ocupada o captura invalidada por entrada local; actualizar antes de operar.",true,"atmega");return;
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
    JsonDocument frame; frame["schema"]="afr2-ws-1"; frame["type"]="evt"; frame["event"]=name;
    frame["payload"]=payload.as<JsonObject>(); frame["ts"]=millis();
    String text; serializeJson(frame,text);
    for (auto &s:sessions_) if (s.used && s.ready) if (auto *client=ws_.client(s.id)) {
        if (!client->queueIsFull()) client->text(text); else client->close(1013,"Slow consumer");
    }
}
void Broker::tick(uint32_t now) {
    if(!mega_.ready() || !productReady_){networkLinked_=false;networkPending_=true;networkPage_=0;wifiResetPending_=false;}
    tickProduct(now);tickSystem(now);
    for (auto &s:sessions_) if (s.used && (uint32_t)(now-s.lastSeen)>6000u) {
        if (auto *client=ws_.client(s.id)) client->close(1001,"Heartbeat expired");
    }
    // Dedicated STOP slot bypasses the ordinary queue, including an in-flight read.
    if (stopQueued_) { BrokerRequest req=stop_; stopQueued_=false; dispatch(req,now); }
    MegaResult result;
    if (mega_.take(result)) finish(result);
    bool localFront=count_ && (!strncmp(queue_[tail_].command,"wifi.",5) || !strncmp(queue_[tail_].command,"system.",7));
    if (count_ && (localFront || (!waiting_ && !mega_.busy()))) {
        BrokerRequest req=queue_[tail_]; queue_[tail_]=BrokerRequest{};
        tail_=(tail_+1)%4; --count_; dispatch(req,now); req.wifi=WifiConfig{};
    }
    if ((uint32_t)(now-statusAt_)>=1000u) {
        statusAt_=now;publishStatus();
    }
    JsonDocument scan;
    if (wifi_.takeScan(scan.to<JsonObject>())) {scanForOled(scan.as<JsonObjectConst>());broadcast("wifi.scan.result",scan);}
}
