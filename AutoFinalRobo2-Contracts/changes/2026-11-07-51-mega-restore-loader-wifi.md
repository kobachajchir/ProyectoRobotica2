# AFR2-20261107-51 — Firmware del auto recargado con el Loader y red Wi-Fi por defecto local

07/11/2026.

Estado real: atmega = `build/afr2.hex` cargado en el Mega (COM16) y verificado, sin prueba física completa; esp = archivo local de red por defecto creado, sin compilar ni cargar; web = sin cambios.

## Resultado de la prueba OLED/I2C (AFR2-20261107-50)

Con la imagen de servicio, la búsqueda I2C dio `found=0 nack=112 faults=0`, con SDA y SCL en alto y `TWSR=0x20`, y los IR en `00000`. El usuario encontró un error de cableado: el conector estaba flojo. Al ajustarlo, la OLED volvió a funcionar. Es una causa probable de los arranques irregulares que se trataron en AFR2-20261107-46/47.

## Carga del firmware del auto con el Loader

Pedido del usuario: «usa el loader para cargar nuestro codigo de nuevo».

- Se usó el Loader 2.0.0 abierto, a través de su servidor MCP: `loader_request_operation` con `flash_mega`, `COM16` y el job `9d3db620ba5946fbbbbc604a74ed7874`.
- El usuario lo aprobó manualmente en la ventana y confirmó el cableado.
- El Loader liberó su monitor serie y ejecutó avrdude con el perfil (`wiring`, 115200, `-D`): firma esperada, flash escrita y verificada, fusibles L:FF H:D8 E:FF. Terminó a las 16:36:36.
- HEX cargado: `AutoFinalRobo2-Atmega/build/afr2.hex`, 371 457 B, SHA-256 `16826727c1109e86119f98a5bf6fc1b7d3896328703fe6e0ef8d134d7b7ca0a4`, fechado 14:40:16. No hay fuentes del firmware más nuevas que el HEX, así que incluye AFR2-20261107-41, -43, -46, -47 y -48.
- **Corrección de AFR2-20261107-49:** el SHA-256 anotado ahí (`167cc564…eaa346`) no coincide con el archivo actual, que tiene el mismo tamaño y la misma fecha. Vale el hash de arriba, que es el del HEX verificado por avrdude.

## Red Wi-Fi por defecto (ESP)

Pedido del usuario: usar la red «Koba» como red STA por defecto.

- Se creó `AutoFinalRobo2-ESP/src/wifi_defaults.local.h` con `AFR2_DEFAULT_STA_SSID "Koba"` y su clave, según AFR2-20261107-38.
- Git lo ignora (`*.local.*`) y la clave no se registra en Contracts.
- El ESP usa esta red cuando no tiene configuración guardada y como respaldo si falla la red STA guardada.
- Toma efecto recién al compilar y cargar el firmware del ESP.

## Pendiente

- Compilar y cargar el ESP para aplicar la red por defecto. Se pidió `build_esp` al Loader por MCP (job `40a59aed2c2a4904b859b3b2c7acba95`), se rechazó en la ventana y no se ejecutó. Esta vez, el usuario hará la compilación y la carga desde el Loader. Las cargas siguientes se piden al Loader como antes.
- Probar en el auto: arranques en frío de la OLED con el conector ajustado, notificaciones, izquierda/derecha y seguimiento de línea.
