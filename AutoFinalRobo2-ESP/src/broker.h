/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
// AFR2-20261023-11. Bounded WebSocket ingress, requests dispatched in loop().
#pragma once
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include "mega_link.h"
#include "wifi_service.h"
struct BrokerRequest {
    uint32_t client=0;
    char id[65]={}, command[33]={};
    WifiConfig wifi;
    uint8_t wireCommand=0,bodyLength=0,body[56]={};
    uint32_t generation=0;
    char sequenceId[65]={},executionId[65]={};
};
class Broker {
public:
    Broker(AsyncWebSocket &ws, MegaLink &mega, WifiService &wifi, Storage &storage)
        :ws_(ws),mega_(mega),wifi_(wifi),storage_(storage) {}
    void begin();
    void tick(uint32_t now);
    void networkEvent(uint8_t kind,uint8_t subject=0);
    void networkServerReady();
private:
    struct Session {
        uint32_t id=0, lastSeen=0, window=0, inputSeen=0;
        uint16_t bytes=0, expected=0, rate=0;
        bool used=false, ready=false;
        uint8_t controlKind=0;char controlId[65]={},controlVersion[16]={};
        uint32_t oledRevision=0;
        bool oledCaptured=false;
        bool screenSubscribed=false;
        uint32_t screenSubscriptionAt=0;
        char input[1025]={};
        char recent[4][65]={}; uint8_t nextRecent=0;
    } sessions_[1];
    AsyncWebSocket &ws_;
    MegaLink &mega_;
    WifiService &wifi_;
    Storage &storage_;
    BrokerRequest queue_[4], stop_, pending_;
    uint8_t head_=0, tail_=0, count_=0;
    bool stopQueued_=false, waiting_=false;
    uint32_t statusAt_=0;
    // AFR2-20261106-12: no FIFO or stale replay for manual PWM.
    uint8_t pwmStream_[12]={};bool pwmStreamPending_=false;
    uint32_t pwmStreamAt_=0,pwmStreamClient_=0,pwmStreamGeneration_=0;
    String deferredReply_,scanEvent_;
    bool delayedErrorPending_=false;
    uint32_t delayedErrorClient_=0;
    char delayedErrorId_[65]={},delayedErrorCommand_[33]={},delayedErrorCode_[33]={},delayedErrorMessage_[129]={},delayedErrorSource_[8]={};
    bool delayedErrorRetryable_=false;
    uint32_t scanEventClient_=0;
    uint32_t deferredClient_=0,deferredAt_=0,wsBackpressure_=0;
    uint8_t helloNotices_=0;
    bool optionalOutput(AsyncWebSocketClient *client) const { return !deferredClient_ && client->queueLen()<2; }
    Session *session(uint32_t id);
    void event(AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void ingest(Session &session);
    void closeWs(AsyncWebSocketClient *client,uint16_t code,const char *reason);
    uint32_t wsOpened_=0,wsClosed_=0;
    uint16_t wsCloseCode_=0;char wsCloseReason_[24]={};
    void dispatch(const BrokerRequest &request, uint32_t now);
    void answer(const BrokerRequest &request, JsonDocument &frame);
    void error(const BrokerRequest &request, const char *code, const char *message, bool retryable=false, const char *source="esp");
    void status(JsonObject out, uint32_t now,uint32_t client=0);
    void capabilities(JsonObject out);
    void finish(const MegaResult &result);
    void broadcast(const char *eventName, JsonDocument &payload);
    bool encodeProduct(BrokerRequest &req,JsonObjectConst payload);
    bool dispatchProduct(BrokerRequest req,uint32_t now);
    bool finishProduct(const BrokerRequest &req,const MegaResult &r);
    void tickProduct(uint32_t now);
    bool productRequest(uint8_t cmd,const uint8_t *body,uint8_t size);
    bool encodeSensors(BrokerRequest &req,JsonObjectConst payload);
    bool finishSensors(const BrokerRequest &req,const MegaResult &result);
    void tickSensors(uint32_t now);
    uint32_t sensorAt_=0,screenRevision_=0,screenCode_=0;
    bool sensorKnown_=false,radarRunning_=false,servoEnabled_=false;
    void stopWebOperations();
    void publishStatus();
    bool dispatchWifiDraft(const BrokerRequest &req,uint32_t now);
    bool finishWifi(const BrokerRequest &req,const MegaResult &result);
    void tickWifi(uint32_t now);
    void scanForOled(JsonObjectConst result);
    void draftStatus(JsonObject out);
    void systemInfo(JsonObject out);
    void publishBoot(const char *device,uint32_t id,uint8_t cause,const char *observation);
    bool dispatchSystem(const BrokerRequest &req,uint32_t now);
    bool finishSystem(const BrokerRequest &req,const MegaResult &r);
    void tickSystem(uint32_t now);
    void tickNetwork(uint32_t now);
    bool finishNetwork(const BrokerRequest &req,const MegaResult &result);
    struct NetworkEvent {uint32_t sequence;uint8_t kind,subject;} networkEvents_[12]={};
    WifiDisplayInfo networkInfo_={};
    WiFiEventHandler apJoin_,apLeave_;
    uint32_t networkSequence_=0,networkObserveAt_=0,networkSyncAt_=0,networkTxAt_=0;
    uint16_t networkRevision_=0,networkDropped_=0;
    uint8_t networkHead_=0,networkTail_=0,networkCount_=0,networkPage_=0;
    bool networkLinked_=false,networkPending_=true,networkServer_=false;
    char megaInfo_[5][49]={};
    uint8_t infoPage_=0,infoReset_=0;
    bool infoReady_=false,productStopped_=true;
    uint32_t infoBoot_=0,lastInfoBoot_=0,infoAt_=0,rebootAt_=0,productStateAt_=0;
    uint32_t productFeatures_=0,productAt_=0,webPulseAt_=0,generation_=1;
    uint32_t sequenceClient_=0,sequenceToken_=0,executionToken_=0,oledRevision_=0,oledScreen_=0,oledAt_=0;
    uint16_t extensionVersion_=0,oledOffset_=0;
    uint16_t speedLimit_=0,wheelMm_=0,radiusMm_=0;
    uint8_t pwmLimit_=0;
    uint32_t limitsAt_=0;
    uint8_t productOrigin_=0,productMode_=255,oledSelection_=0,oledFirst_=0,oledCount_=0;
    bool productReady_=false,productAttempted_=false,safetyRequested_=false;
    uint8_t oledData_[1024]={},stateData_[28]={};
    char sequenceId_[65]={},executionId_[65]={};
    WifiConfig oledDraft_;
    uint32_t wifiPollAt_=0,wifiStatusAt_=0,wifiJob_=0,wifiDraftToken_=0,wifiJobAt_=0;
    uint8_t wifiOp_=0,wifiOffset_=0,wifiTotal_=0,wifiRequestedMode_=1,wifiPhase_=0,wifiResult_=0;
    bool wifiDraftReady_=false,wifiAwaitWeb_=false,wifiScanOled_=false;
    bool wifiResetPending_=false;
    uint32_t wifiResetGeneration_=0,wifiResetAt_=0;
    char wifiCredentials_[97]={};
    String wifiNetworks_[16];
    uint32_t wifiScanId_=0;
    uint8_t wifiScanPage_=0;bool wifiScanTruncated_=false;
    uint8_t wifiNetworkCount_=0,wifiNetworkIndex_=0;
    char webVersion_[16]={};
    uint8_t wifiVersions_=0;
    uint8_t wifiSecurity_[16]={},wifiDetails_[19]={};
    bool wifiDetailsPending_=false;
    uint8_t readyWifiClients();
    uint32_t diagPollAt_=0,diagJob_=0,diagPattern_=0;
    bool diagReply_=false;
};
