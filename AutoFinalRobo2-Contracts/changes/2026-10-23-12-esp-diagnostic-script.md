# AFR2-20261023-12 — Script de conexión y lectura flash ESP

23/10/2026. Pedido directo del usuario: preparar una ejecución futura que primero compruebe conexión, después consulte flash/configuración y guarde evidencia para continuar. No es una orden de conectar/flashear hoy.

Archivos propios nuevos: `tools/esp_probe.py` y lanzador `tools/esp-probe.ps1`. Guía `hardware/ESP_DIAGNOSTICO.md`, destino `hardware/diagnostics/` con README/.gitignore. Sin código heredado copiado. Se consultaron documentación primaria Espressif/Arduino y las tablas/API CLI del esptool 4.5.1 local como referencia de sintaxis; se admite también sintaxis 5.x.

El script selecciona un solo puerto explícito o mediante entrada interactiva; `--list-ports` no abre dispositivos. La ejecución completa realiza chip_id, flash_id y lectura de ocho bytes de cabecera, en ese orden y cortando en caso de error. Para leer cabecera usa stub RAM temporal, sin escribir/borrar flash. `--check-only` detiene después de identificar conexión. Cada intento conserva reporte JSON/Markdown y logs con comandos/exit codes, incluyendo fallos. No instala dependencias, cambia firmware, genera imágenes ni modifica AutoMicro.

La conexión ROM no acredita todo el cableado; cabecera configurada no prueba modo SPI ni tamaño físico. Partición LittleFS queda explícitamente desconocida: no hay un descubrimiento universal de offsets del layout Arduino de una imagen desconocida con flash_id. Comparar bundle con flash total no asegura capacidad FS. Se retoma con evidencia medida y linker/mapa del firmware conocido.

Estado: atmega sin cambios, esp sin cambios, web sin cambios; shared con herramienta/guía **en fuente, pendiente de ejecutar**. No se conectó hardware, abrió puerto, ejecutó esptool ni realizó flasheo. No se ejecutaron tests/builds/validadores durante la preparación. No cambia wire ni capabilities. Firmware AFR2-20261023-11 conserva su estado previo.
