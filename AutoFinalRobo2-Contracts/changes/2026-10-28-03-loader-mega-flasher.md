# AFR2-20261028-03 — Flasher ATmega integrado y selector de conexión ESP

28/10/2026. El usuario pide instalar un puente temporal en Mega, probar ESP por
D18/D19 con RESET libre y, si falla, volver al puente físico con adaptación y
RESET-GND. Pide código de servicio integrado al Loader con botón de carga,
plan de prueba y desplegable que abra instrucciones/checklist de cada método.

## Resultado en fuente

Loader0.8 incorpora **Cargar flasher en ATmega**, **Restaurar firmware del auto**
y pestaña Flasher ATmega. Usa la fuente C/avr-libc separada de02, compila solo
su objetivo Make al baud elegido y carga/verifica mediante avrdude. Antes de
reemplazar la aplicación guarda copia única del HEX de producto seleccionado
en PC y su hash. No lee firmware del chip, no cambia EEPROM/fuses/bootloader.
El producto debe existir antes. Restaurar exige esa copia/hash para el mismo
COM, con selección explícita del HEX conocido como alternativa en otra PC/COM.

**Operación → Método ESP** muestra dos opciones sincronizadas con la pestaña:
flasher software D18/D19 RESET libre y puente físico D0/D1 RESET-GND. Elegir
cierra el monitor, abre el cuadro de conexiones del método y solo habilita
Continuar con todas sus casillas marcadas. Cancelar conserva el valor anterior
y no reabre el monitor. No se carga ni cambia cableado por elegir. Los asistentes
de preparación/escritura siguen activos al consultar/cargar, también por MCP.

Perfil schema1 recibe EspTransport (default mega_reset para conservar perfiles
anteriores), BridgeBaud y BridgeStartupSeconds. Instalar con éxito selecciona
mega_bridge y alinea EspBaud. El software no escucha74880 ni sigue cambios
dinámicos de baud; guía KEY/RST manual y exige consulta ROM. El wrapper Python
abre COM con DTR/RTS inactivos, espera tras abrir y conserva el mismo handle
para esptool4/5 según su modelo de apertura. Algoritmos de escritura/verificación
siguen en esptool; no se acredita compatibilidad probada en esta entrega.

Registro local por COM: preparado, escritura iniciada, flasher verificado,
restauración iniciada y producto verificado. Hashes y copias en LOCALAPPDATA;
no es identidad física ni detección continua de firmware. Un estado conocido
incompatible bloquea la consulta por flasher hasta reinstalarlo. Un equipo
sin registro conserva confirmación manual sin llamarla detección automática.

Mensajes finales diferencian servicio instalado, ESP verificado y producto
restaurado, sin recomendar RESET-GND en el modo software. MCP agrega acciones
install_mega_flasher/restore_mega_product; consultas/cargas ESP existentes usan
el transporte del perfil. No se modifica ni registra el cliente Codex global.

[Plan A/B](../hardware/PLAN_PRUEBA_FLASHER_MEGA.md),
[fuente AVR](../../AutoFinalRobo2-Atmega/service/esp_uart_bridge.c),
[manual](../../AutoFinalRobo2-Loader/MANUAL.html#metodo),
[módulo](../../AutoFinalRobo2-Loader/README.md).

## Estado por módulo

| Módulo | Estado de esta entrega |
| --- | --- |
| atmega | Servicio y objetivo Make preparados en02, usados por la integración. Producto sin cambio funcional; sin compilación/carga/prueba. |
| Loader | Fuente0.8, selector/checklists, carga/restauración y wrapper. No compilado/ejecutado/validado. |
| esp | Sin cambios de firmware ni nueva consulta/carga. |
| web | Sin cambios, build ni promoción. |
| Contracts | Ficha compartida y plan con aceptación por paso. |
| CAD | Sin cambios. |

No se ejecutaron tests, validaciones, compilaciones, GUI/MCP, apertura COM ni
flasheos. El plan no se marca aprobado por existir código. No se creó commit
ni se publicó Git.

Limitaciones: integración pendiente de ejercicio físico; el baud fijo tiene
error AVR y el driver puede afectar reset. Copia de PC, no respaldo leído del
chip. Sin restauración automática al cerrar ni identificación de cambio de placa.
La prueba B requiere recableado/adaptación reales; no se activa automáticamente.
