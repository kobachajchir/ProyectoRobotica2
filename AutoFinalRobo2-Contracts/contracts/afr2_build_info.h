/* AFR2-20261024-07. Shared product version; toolchain date/compiler supplied
 * by each actual compilation. "workspace" never claims a clean Git commit. */
#ifndef AFR2_BUILD_INFO_H
#define AFR2_BUILD_INFO_H
/* AFR2-20261108-07: version de producto unica para ATmega, ESP y Web (Web/package.json igual).
 * Reglas MAYOR/MENOR/PARCHE en docs/VERSIONADO.md. 1.1.0 = AFR2-20261108-06, 1.2.0 = AFR2-20261108-07. */
#define AFR2_FIRMWARE_VERSION "1.2.0"
#define AFR2_FIRMWARE_MAJOR 1
#define AFR2_FIRMWARE_MINOR 2
#define AFR2_FIRMWARE_PATCH 0
#ifndef AFR2_BUILD_REVISION
#define AFR2_BUILD_REVISION "workspace"
#endif
#endif
