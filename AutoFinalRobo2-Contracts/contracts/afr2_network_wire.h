/* AFR2-20261024-08. OLED network state/events. AP passphrase is local-display
 * data only: never include it in HTTP/WS status, logs or compilation info. */
#ifndef AFR2_NETWORK_WIRE_H
#define AFR2_NETWORK_WIRE_H
#define AFR2_EXT_NETWORK_STATE 0x53u
#define AFR2_EXT_NETWORK_EVENT 0x54u
#define AFR2_NETWORK_PAGES 5u
#define AFR2_NETWORK_TEXT_MAX 32u
enum {
 AFR2_NET_BOOT=1, AFR2_NET_WEB_READY, AFR2_NET_GOT_IP, AFR2_NET_AP_READY,
 AFR2_NET_CLIENT_JOIN, AFR2_NET_CLIENT_LEAVE, AFR2_NET_WS_OPEN, AFR2_NET_WS_CLOSE,
 AFR2_NET_WEB_SERVED, AFR2_NET_STA_LOST, AFR2_NET_AP_STOPPED,
 /* AFR2-20261108-06: la red STA elegida no conecto y no hay AP. subject: 1 red no visible,
  * 2 clave incorrecta, 3 no se pudo unir. El ESP espera la decision del OLED (Iniciar AP /
  * Seguir esperando); un ATmega que lo rechaza (BAD_REQUEST) vuelve al AP automatico. */
 AFR2_NET_STA_NOT_FOUND,
 AFR2_NET_EVENT_MAX=AFR2_NET_STA_NOT_FOUND
};
#define AFR2_NOTICE_NETWORK 16u
#endif
