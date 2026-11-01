> **Compilado y cargado:** 29/10/2026 12:02:57, COM8,74796bytes escritos/verificados, exit0. SRAM6923bytes, margen1269bytes; main259, renderer70 y line29bytes según .su individuales. Sin prueba dinámica de pila ni observación física por el agente. [Evidencia](../documentation/builds/2026-10-29-15/README.md). El estado pendiente inferior es histórico. El usuario reporta luego mayor latencia: se atiende en16 junto con la presentación de radar solicitada.

# AFR2-20261029-15 — Reinicio al abrir radar y Hall instalado

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

Pedido: usuario confirma motores/Hall cableados, informa bloqueo de UI por
«Conectar Hall» y reinicio al girar encoder en prueba HC-SR04. Continúa la
corrección de UI con instrucción previa de compilar/cargar COM8 al terminar.
Estado inicial de esta ficha: fuente corregida; compilación y carga pendientes.

## Causa localizada por lectura

El giro en AFR2_N_US_VIEW abre AFR2_N_RADAR_VIEW (0x030031). Su helper line
mezclaba dos variantes de Bresenham: dy era negativo pero las ecuaciones
e=dx-dy, e2>-dy y e2<dx requieren dy positivo. Para el segmento real del primer
arco (72,49)→(72,48), dx=0 y dy=-1: avanza x, no y, y no llega al destino.
El main deja de alimentar el watchdog de500ms y el AVR puede reiniciar. No
se capturó MCUSR de ese incidente, pero sí se identifica un defecto concreto
en la ruta reproducible descrita. No es evidencia de un panic de RTOS.

También radius*trig desborda int de16bits AVR (38*1000=38000). Las coordenadas
de radar se calculan ahora con producto de32bits antes de dividir. El helper
line usa dy positivo y límite max(dx,dy)+1 para impedir trabajo ilimitado.
No se desactiva watchdog ni se lo alimenta dentro de dibujo para ocultar fallos.

## Adaptaciones por archivo

- Atmega/src/ui/afr2_sensor_screens.c: Bresenham corregido y acotado; proyección
  trigonométrica de arcos, radios, cursor y puntos con producto int32_t.
- Web/src/features/oled/firmwareSensorRenderer.ts: mismo límite de segmentos.
  Su dy ya era positivo y JavaScript no tenía el desbordamiento de16bits; se
  conserva su geometría, ahora coincidente con la intención corregida del C.
- Web/src/features/oled/firmwareRenderer.sources.json: fingerprint C revisado
  después de vincular las dos implementaciones; LUT/glifos sin cambios.
- Atmega/include/afr2_product.h, src/services/afr2_product.c y
  src/platform/afr2_preferences.c: journalv3 y Hall conectado en defaults y
  migración de journalv1/v2 válido, preservando el resto de datos. Ver
  [contrato3](../contracts/EEPROM_HALL_MIGRATION_3.md).
- Atmega/src/services/afr2_app.c: motivos específicos de habilitación/medición
  Hall, vista Hall/odometría sin afirmar desconexión y versión EEPROM actual.
  Son textos de snapshot preparado, que la biblioteca Web consume como datos.
- README/informes de módulos y Contracts: estado real; PINOUT registra montaje
  informado sin afirmar calibración física. Autorías originales preservadas.

No cambian IDs, filas, acciones del encoder, layout, wire, I2C Manager, recursos
ni el transporte diferencial14. Se conserva giro para alternar radar/numérica
y retorno contextual existente, sin agregar botón menú.

## Alcance y comprobación

ATmega y contraparte Web adaptados en fuente. No se ejecutan tests ni pruebas
físicas adicionales durante esta corrección. Compilar/cargar AVR queda cubierto
por la instrucción previa del usuario al terminar el trabajo. Web no se compila
ni promociona a ESP; ESP y Loader no modificados. Build/mapa/SRAM y resultado de
flash se registrarán tras finalizar, sin asumir que el compilador prueba placa.

Hall habilitado indica configuración/captura disponible, no demuestra pulsos
correctos. Motors conectados no certifica signos/calibración. Se conservan
motor_ready, requisitos de geometría/CPR y las salidas apagadas al arrancar.
