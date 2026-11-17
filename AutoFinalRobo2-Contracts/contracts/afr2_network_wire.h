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
 /* AFR2-20261116-01: 5 intentos fallidos consecutivos de sincronizacion online. Abre el aviso
  * interactivo con Cerrar (defecto) y Desactivar sincronizacion. */
 AFR2_NET_SYNC_FAIL_5,
 AFR2_NET_EVENT_MAX=AFR2_NET_SYNC_FAIL_5
};
#define AFR2_NOTICE_NETWORK 16u
/* AFR2-20261117-05: codigo de vinculacion de la copia web (Firebase), ESP -> ATmega. Cuerpo de 12 bytes:
 * esp_boot_id u32le, ttl_s u8 (1..60, tiempo en el OLED), codigo ascii:7 ("K7M-4QX", A-Z sin I/L/O/U, 0-9, guion).
 * Sensible: el codigo da acceso de lectura a la copia online durante 5 min. Respuesta sin cuerpo. */
#define AFR2_EXT_SYNC_PAIR 0x57u
#define AFR2_SYNC_PAIR_CODE_LEN 7u
#define AFR2_SYNC_PAIR_BYTES 12u
#endif
