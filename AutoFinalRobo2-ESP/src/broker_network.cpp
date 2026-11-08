// AFR2-20261024-08. Bounded, acknowledged OLED state/events. All requests
// share MegaLink and yield to STOP. No JSON/HTTP/WS AP-password transport.
#include "broker.h"
#include <string.h>

void Broker::networkEvent(uint8_t kind,uint8_t subject) {
    if(kind<1 || kind>AFR2_NET_EVENT_MAX)return;
    // Coalesce retained state announcements; keep client join/leave ordering.
    for(uint8_t i=0;i<networkCount_;i++) {
        auto &entry=networkEvents_[(networkTail_+i)%12];
        bool state=kind==AFR2_NET_BOOT || kind==AFR2_NET_WEB_READY || kind==AFR2_NET_GOT_IP || kind==AFR2_NET_AP_READY;
        if(entry.kind==kind && entry.subject==subject && (state || i+1==networkCount_))return;
    }
    if(networkCount_==12 || networkSequence_==UINT32_MAX){if(networkDropped_!=UINT16_MAX)++networkDropped_;return;}
    networkEvents_[networkHead_]={++networkSequence_,kind,subject};
    networkHead_=(networkHead_+1)%12;++networkCount_;
}
void Broker::networkServerReady() {
    if(networkServer_)return;
    networkServer_=true;networkEvent(AFR2_NET_WEB_READY);networkPending_=true;networkPage_=0;
    if(!++networkRevision_)++networkRevision_;
}
bool Broker::finishNetwork(const BrokerRequest &req,const MegaResult &r) {
    if(r.command!=AFR2_EXT_NETWORK_STATE && r.command!=AFR2_EXT_NETWORK_EVENT)return false;
    if(r.timeout || r.transportFault || r.length<9 || r.payload[8]!=AFR2_RESULT_OK) {
        if(r.command==AFR2_EXT_NETWORK_STATE){networkPage_=0;networkPending_=true;}
        // AFR2-20261108-06: un ATmega anterior no conoce AFR2_NET_STA_NOT_FOUND (BAD_REQUEST). Se descarta el
        // evento para no trabar la cola y se vuelve al AP automatico.
        else if(!r.timeout && !r.transportFault && r.length>=9 && r.payload[8]==AFR2_RESULT_BAD_REQUEST && networkCount_ &&
                networkEvents_[networkTail_].kind==AFR2_NET_STA_NOT_FOUND && networkEvents_[networkTail_].sequence==afr2_get_u32(req.body+4)) {
            networkTail_=(networkTail_+1)%12;--networkCount_;wifi_.askUnsupported();
        }
        networkTxAt_=millis();return true;
    }
    if(r.command==AFR2_EXT_NETWORK_STATE) {
        if(afr2_get_u16(req.body+4)!=networkRevision_ || req.body[6]!=networkPage_)return true;
        if(++networkPage_==AFR2_NETWORK_PAGES){networkPending_=false;networkPage_=0;networkSyncAt_=millis();}
    } else if(networkCount_ && networkEvents_[networkTail_].sequence==afr2_get_u32(req.body+4)) {
        networkTail_=(networkTail_+1)%12;--networkCount_;
    }
    return true;
}
void Broker::tickNetwork(uint32_t now) {
    if(!productReady_ || !mega_.ready() || extensionVersion_<5 || !(productFeatures_&AFR2_EXT_F_NETWORK_UI))return;
    if(!networkLinked_) {
        networkLinked_=true;networkPending_=true;networkPage_=0;
        networkEvent(AFR2_NET_BOOT); // Retained startup identity, deduplicated by AVR bootId.
    }
    uint8_t askReason; // AFR2-20261108-06: la STA elegida no conecto; el OLED decide Iniciar AP / Seguir esperando
    if(wifi_.takeAskEvent(askReason))networkEvent(AFR2_NET_STA_NOT_FOUND,askReason);
    // Freeze a started five-page snapshot until its ACKs complete. RSSI and
    // AP client-count jitter must not continually restart that transfer.
    if((!networkPending_ || networkPage_==0) && (!networkObserveAt_ || (uint32_t)(now-networkObserveAt_)>=750u)) {
        networkObserveAt_=now;WifiDisplayInfo current;wifi_.displayInfo(current);
        bool changed=current.mode!=networkInfo_.mode || current.staConnected!=networkInfo_.staConnected || current.apReady!=networkInfo_.apReady ||
            memcmp(current.staIp,networkInfo_.staIp,4) || memcmp(current.apIp,networkInfo_.apIp,4) ||
            strcmp(current.staSsid,networkInfo_.staSsid) || strcmp(current.apSsid,networkInfo_.apSsid) || strcmp(current.apPassword,networkInfo_.apPassword);
        if(changed || !networkRevision_) {
            if(current.staConnected && (!networkInfo_.staConnected || memcmp(current.staIp,networkInfo_.staIp,4)))networkEvent(AFR2_NET_GOT_IP);
            if(!current.staConnected && networkInfo_.staConnected)networkEvent(AFR2_NET_STA_LOST);
            if(current.apReady && (!networkInfo_.apReady || strcmp(current.apSsid,networkInfo_.apSsid) || strcmp(current.apPassword,networkInfo_.apPassword) || memcmp(current.apIp,networkInfo_.apIp,4)))networkEvent(AFR2_NET_AP_READY);
            if(!current.apReady && networkInfo_.apReady)networkEvent(AFR2_NET_AP_STOPPED);
            networkInfo_=current;networkPending_=true;networkPage_=0;
            if(!++networkRevision_)++networkRevision_;
        } else networkInfo_=current; // Volatile counters refresh with the next periodic snapshot.
    }
    if((uint32_t)(now-networkTxAt_)<200u)return;
    if(!networkPending_ && (uint32_t)(now-networkSyncAt_)>=3000u){networkPending_=true;networkPage_=0;}
    uint8_t body[40]={},length=0,command;
    afr2_put_u32(body,mega_.ownBootId());
    if(networkPending_) {
        command=AFR2_EXT_NETWORK_STATE;afr2_put_u16(body+4,networkRevision_);body[6]=networkPage_;
        if(networkPage_==0) {
            uint8_t *p=body+8;p[0]=networkInfo_.mode;p[1]=networkInfo_.staConnected;p[2]=networkInfo_.apReady;
            p[3]=networkInfo_.clients;p[4]=networkServer_;afr2_put_u16(p+5,(uint16_t)networkInfo_.rssi);
            memcpy(p+7,networkInfo_.staIp,4);memcpy(p+11,networkInfo_.apIp,4);body[7]=15;
        } else {
            const char *text=networkPage_==1?networkInfo_.staSsid:networkPage_==2?networkInfo_.apSsid:networkInfo_.apPassword;
            uint8_t offset=networkPage_==4?32:0,size=strlen(text);
            size=size>offset?size-offset:0;if(size>32)size=32;
            body[7]=size;if(size)memcpy(body+8,text+offset,size);
        }
        length=8+body[7];
    } else if(networkCount_) {
        command=AFR2_EXT_NETWORK_EVENT;auto &event=networkEvents_[networkTail_];
        afr2_put_u32(body+4,event.sequence);body[8]=event.kind;body[9]=event.subject;length=10;
    } else return;
    if(productRequest(command,body,length)) {
        pending_=BrokerRequest{};strlcpy(pending_.command,command==AFR2_EXT_NETWORK_STATE?"_network_state":"_network_event",33);
        memcpy(pending_.body,body,length);pending_.bodyLength=length;waiting_=true;networkTxAt_=now;
    }
}
