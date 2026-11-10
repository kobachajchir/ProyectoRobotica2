/* AFR2-20261024-07. Shared product version; toolchain date/compiler supplied
 * by each actual compilation. "workspace" never claims a clean Git commit. */
#ifndef AFR2_BUILD_INFO_H
#define AFR2_BUILD_INFO_H
/* AFR2-20261108-07: version de producto unica para ATmega, ESP y Web (Web/package.json igual).
 * Reglas MAYOR/MENOR/PARCHE en docs/VERSIONADO.md. 1.1.0 = AFR2-20261108-06, 1.2.0 = -07,
 * 1.3.0 = -08 (OLED del flasher ATmega), 1.4.0 = -09 (TAP del log de depuracion), 1.5.0 = -10 (Ultrasonido en rapidas, TAP 115200/250000), 1.6.0 = -12 (orden de rapidas y tarjeta de estados), 1.7.0 = -13 (IR activable sin motores, radar con servo y HC-SR04), 1.7.1 = -14 (modo Wi-Fi desde el OLED sin bloqueo), 1.8.0 = AFR2-20261109-01 (radar en vivo, servo centrado, avisos Wi-Fi, cambio de modo en el ESP). */
#define AFR2_FIRMWARE_VERSION "1.16.0"
#define AFR2_FIRMWARE_MAJOR 1
#define AFR2_FIRMWARE_MINOR 16
#define AFR2_FIRMWARE_PATCH 0
#ifndef AFR2_BUILD_REVISION
#define AFR2_BUILD_REVISION "workspace"
#endif
#endif
