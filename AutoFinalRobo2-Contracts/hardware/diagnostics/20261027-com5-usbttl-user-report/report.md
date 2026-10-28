# ESP8266 en COM5: chip y flash identificados por USB–TTL

AFR2-20261027-25, 27/10/2026. Consultas **ejecutadas por el usuario**, que pegó
las salidas de PowerShell en esta conversación. El agente registra esa evidencia;
no ejecutó de nuevo las consultas para este informe. Hora y exit code numérico
no aportados. Autor de las pruebas: usuario del proyecto; registro asistido por Codex.

## Resultados aportados

| Dato | Resultado |
| --- | --- |
| Herramienta / puerto | esptool.py 4.5.1 / adaptador USB–TTL externo en COM5 |
| Parámetros comunes | 9600 bit/s, `--chip esp8266 --before no_reset --after no_reset --no-stub --connect-attempts 100` |
| Chip | ESP8266EX, ID `0x00c2218e` |
| MAC | `ec:64:c9:c2:21:8e` |
| Cristal informado | 26 MHz; no es la frecuencia SPI de la flash |
| Flash: IDs crudos | Fabricante `0x5e`, dispositivo `0x4016`; marca/modelo no atribuidos |
| Capacidad informada | `4MB`: 4 MiB = 4.194.304 bytes |
| Estado final | `Staying in bootloader`, retorno al prompt en ambas consultas |

## Salida chip_id aportada

```text
esptool.py v4.5.1
Serial port COM5
WARNING: Pre-connection option "no_reset" was selected. Connection may fail if the chip is not in bootloader or flasher stub mode.
Connecting........
Chip is ESP8266EX
Features: WiFi
Crystal is 26MHz
MAC: ec:64:c9:c2:21:8e
Enabling default SPI flash mode...
Chip ID: 0x00c2218e
Staying in bootloader.
```

## Salida flash_id aportada

```text
esptool.py v4.5.1
Serial port COM5
WARNING: Pre-connection option "no_reset" was selected. Connection may fail if the chip is not in bootloader or flasher stub mode.
Connecting.......
Chip is ESP8266EX
Features: WiFi
Crystal is 26MHz
MAC: ec:64:c9:c2:21:8e
Enabling default SPI flash mode...
Manufacturer: 5e
Device: 4016
Detected flash size: 4MB
Staying in bootloader.
```

Las líneas anteriores son transcripciones literales de las salidas. Los comandos
aportados invocan Python de PlatformIO con `-u`, su `esptool.py` y los parámetros
comunes de la tabla, terminando en `chip_id` o `flash_id`, respectivamente.
No aparecen comandos de escritura, borrado o verificación de contenido.

## Montaje y comparación con COM8

Se indicó usar TX/RX/GND del adaptador de lógica 3,3 V hacia ESP RX/ESP TX/GND
de la **cabecera directa ESP UART**, aislando el enlace D0/D1 del selector.
El procedimiento conservaba alimentación del shield desde el Mega, sin VCC del
adaptador. El usuario indicó que ya había usado ese adaptador con un ESP01 y
aportó estas consultas exitosas. Modelo del adaptador, foto del montaje final
y mediciones de niveles/alimentación no comunicados; no se certifican por inferencia.

**Aclaración posterior del usuario en la misma conversación:** utilizó exactamente
los mismos dos pines del shield con el adaptador y con D0/D1 del Mega. Por lo tanto,
no se puede atribuir el éxito COM5 a haber evitado el selector o haber cambiado de
cabecera. La cabecera física concreta sigue pendiente de identificación por foto
o serigrafía inequívoca. El montaje descrito arriba conserva el procedimiento
indicado, no sustituye esta aclaración sobre lo realmente conectado.

El usuario también informó retorno D0–D1 correcto en RealTerm a 9600 con el Mega
aislado. Los intentos anteriores por COM8/Mega/selector fallaron al sincronizar
aun recibiendo `boot mode:(1,6)`. El trabajo MCP
`015877688f7d4523a1d5aa19535e77d9` terminó fallido a las 18:49:18 sin escritura.

La comunicación directa confirma ida/vuelta e identificación en este ejemplar.
La diferencia apunta al puente utilizado, su interacción eléctrica con el
circuito conectado o su manejo de puerto/reset, pero **no localiza un componente
responsable** ni prueba daño del Mega, selector o traductor. La comparación con
los mismos pines no acredita el mismo comportamiento bajo niveles/carga distintos.

## Límites y continuación

La capacidad de 4 MiB pasa de nominal/candidata a identificada por consulta
aportada, solo para este ejemplar. No se verificó contenido de firmware ni
arranque de nuestra aplicación, Wi-Fi, sensores o motores. Layout/offsets de
LittleFS/OTA, modo/frecuencia SPI y alimentación bajo carga siguen pendientes.
`Enabling default SPI flash mode` no es una aprobación de DIO/QIO/DOUT para
una imagen nueva. El perfil 4 MiB / LittleFS nominal 3 MiB conserva sus propias
reglas de linker, imágenes y verificación antes de una futura escritura.

No se modifican perfiles del Loader ni se fijan puertos del producto. COM5 es
un dato de esta sesión. Registrar estos resultados no autoriza nuevas pruebas,
compilaciones, borrados o flasheos.
