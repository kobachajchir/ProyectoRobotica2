/* AFR2-20261106-02: host-only prepared framebuffer fixtures; no hardware. */
#include "afr2_screens.h"
#include "afr2_qr_screen.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
static Afr2UiSnapshot s;
static int32_t value(void *ctx,uint8_t action){switch(action){case AFR2_A_WIFI_CONNECTED:return 2;case AFR2_A_WIFI_COUNT:return s.network_total?8:0;case AFR2_A_WIFI_TOTAL:return s.network_total;case AFR2_A_WIFI_PAGE_INDEX:return s.network_page;case AFR2_A_WIFI_SCAN_STATE:return s.network_scan_state;case AFR2_A_HALL_SAVE:return s.hall_pending;default:return 0;}}
int main(int argc,char **argv){
 Afr2Oled oled={0}; Afr2Ui u; 
 if(argc!=2)return 2;
 FILE *out=fopen(argv[1],"wb");if(!out)return 1;
 for(int scenario=0;scenario<10;scenario++)for(int variant=0;variant<3;variant++)for(int node=0;node<70;node++){
  memset(&s,0,sizeof(s));s.sensor_now_ms=1000;s.network_ssid=s.network_ap_ssid=s.network_password=s.network_ip=s.network_ap_ip="";
  if(scenario==1 || scenario==2){s.network_total=16;s.network_scan_state=2;s.network_page=scenario-1;s.network_truncated=1;for(int i=0;i<3;i++){strcpy(s.labels[i],"Red demo");s.row_security[i]=2;}}
  if(scenario==3)s.network_scan_state=1;
  if(scenario==4)s.network_scan_state=2;
  if(scenario==5)s.network_scan_state=3;
  if(scenario==6){s.hall_pending=5;for(int i=0;i<4;i++)s.hall_cpr[i]=100+i;s.ir_kind=1;s.ir_valid=1;s.line_valid=1;s.line_bits=21;for(int i=0;i<5;i++)s.ir_raw[i]=i%2;}
  if(scenario==7){s.network_valid=1;s.network_ap=1;s.network_mode=2;s.network_server=1;s.qr_allowed=1;s.network_ap_ssid="AFR2-demo";s.network_ap_ip="192.0.2.1";}
  Afr2Ui_Init(&u);u.ops.value=value;u.node=node;u.clock_ms=1000;u.redraw=1;u.hold_progress=variant*8;
  if(scenario==8 || scenario==9){Afr2Ui_NotifyFlash(&u,scenario==8?AFR2_NOTICE_NETWORK+AFR2_NET_AP_READY:0,PSTR("Operacion"),PSTR("Aceptada"),3000);u.notice_started_ms=100;}
  Afr2MenuNode menu;Afr2Ui_MenuNode(&u,&menu);u.selection=variant?menu.count-1:0;u.firstVisible=u.selection>2?u.selection-2:0;
  Afr2Oled_Clear(&oled);Afr2Ui_Render(&u,&oled,&s);fwrite(oled.buffer,1,1024,out);
 }
 const char *url="http://192.0.2.1/";uint8_t payload[353];memset(payload,'A',sizeof(payload));
 const uint16_t lengths[]={17,220,300,353};
 for(int i=0;i<4;i++){Afr2Qr_DrawPayload(&oled,i?payload:(const uint8_t *)url,lengths[i]);fwrite(oled.buffer,1,1024,out);}
 return fclose(out);
}
