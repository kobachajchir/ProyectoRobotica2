# Base observada, recursos y adaptación

Lectura de fuente el01/11/2026. No se renderizó, compiló, probó ni validó la
OLED física, la Web o esta skill. Las coordenadas documentadas describen código
o propuestas, no evidencia visual. Esta skill no instala un driver de display.

## Fuentes de AutoFinalRobo2

Raíz de procedencia: `D:/ProyectoRobotica2`, independiente de AutoMicro.
Los destinos relativos siguientes son evidencia de origen, no dependencias
necesarias para usar el sistema en otro equipo:

| Fuente | Qué se extrajo |
| --- | --- |
| `AutoFinalRobo2-Contracts/documentation/screens.json` | 53 identidades de navegación y11 avisos; roles y parentescos |
| `AutoFinalRobo2-Atmega/src/ui/afr2_menu_catalog.c` | tabla de nodos/items, destinos, acciones y rangos de edición |
| `AutoFinalRobo2-Atmega/src/ui/afr2_screens.c` | menú3, vistas de3líneas, editor, teclado, avisos, foco/scroll e historial8 |
| `AutoFinalRobo2-Atmega/src/ui/afr2_icons.c` y `include/afr2_icons.h` | 16 recursos, dimensiones, XBM y elección contextual de iconos |
| `AutoFinalRobo2-Atmega/src/ui/afr2_network_screen.c` | vistas de IP/AP y11 avisos de red |
| `AutoFinalRobo2-Atmega/src/ui/afr2_sensor_screens.c` | radar, lectura numérica, IR5 y ángulo/PWM |
| `AutoFinalRobo2-Atmega/src/ui/afr2_sequence_screen.c` | progreso segmentado, iconos, etapa y tiempo |
| `AutoFinalRobo2-Atmega/src/display/afr2_oled.c` | clipping, fuente7×10, framebuffer vertical por páginas y XOR |
| `AutoFinalRobo2-Atmega/src/display/font7x10.c` | corpus ASCII y acceso de glifos |
| `AutoFinalRobo2-Contracts/documentation/NAVIGATION.md` | corto/largo y excepciones vigentes de vistas hermanas |

Contrapartes Web: `AutoFinalRobo2-Web/src/features/oled/{firmwareRenderer,
firmwareSensorRenderer,oledRasterizer,screenCatalog}.ts`,
`generated/firmwareLibrary.ts` y `EncoderControl.tsx`. El simulador local está
en `src/features/developer/{UiSimulator.tsx,uiSimulation.ts}`; sus fixtures
no son fuente de comportamiento real de los servicios del ATmega.

El corpus contiene menús, estados de robot/enlace/alertas, movimiento, modos,
GOTO, pruebas de motores/Hall, calibración, sensores, Wi-Fi, teclado, interfaz,
preferencias, diagnóstico, secuencia y avisos. El número de identidades no
cuenta editores/teclado como pantallas independientes para cada parámetro.

## Recursos adjuntos

Corrección de pulsación larga,01/11/2026: el usuario identifica el símbolo
correcto por una **L interna**, no subrayado. `assets/encoder-gestures.json`
añade la variante13×13 derivada del bitmap base, sin alterar sus copias de
origen. No se encontró una definición separada conL en las fuentes consultadas:
referencia local, `D:/Microcontroladores/Auto Proyecto/Firmware/Core/Src/fonts.c`
y checkout AutoMicro en OneDrive `FirmwareF4/Core/Src/fonts.c`, `Firmware/Core`,
Web y renderer bitmapF4. Es una adaptación a la indicación humana, no una
afirmación de coincidencia exacta con un original recuperado. No se modificó
AutoMicro. El generador ahora usa el recurso registrado sin dibujar una barrita.

Revisión5 añade encoder QR de Project Nayuki MIT, conservado sin cambios en
`scripts/vendor/qrcodegen.py`, con commit y procedencia en `PROVENANCE.md`.
La guía [QR](qr.md) cita documentación primaria DENSO WAVE para zona clara y
versiones. `scripts/generate_qr.py` es un adaptador nuevo para composición y
exportación, no firmware. Ejemplo `assets/qr-examples/demo/`: contenido elegido
de muestra `AFR2:DEMO`, v1/ECC H,2px/módulo. Generación ejecutada; sin decoder,
teléfono, OLED física, tests, compilación o validaciones. No equivale a prueba
de escaneabilidad física. No hay servicios externos para codificar contenidos.

- `../assets/afr2_icons.c`: copia de la fuente de iconos y lookup originales.
- `../assets/afr2_icons.h`: nombres y orden enum de esa copia.
- `../assets/font7x10.c`: copia del corpus de la fuente y su acceso.

Son **insumos de adaptación**, no fuentes listas para compilar en cualquier
placa. Incluyen nombres/cabeceras y accesos AVR que el proyecto nuevo debe
adaptar. Preservar comentarios de autoría: bitmap/fuente F4 atribuidos a kobac;
adaptaciones PROGMEM/clipping/lookup AFR2; radar/servo originales AFR2.
No actualizar copias ni recursos de proyectos existentes automáticamente.

Los bitmaps de iconos son XBM por filas, stride=ceil(ancho/8), bit LSB primero.
La fuente usa10 filas por glifo,7 bits útiles, con orientación según el accesor
original. No confundirlos con el framebuffer OLED vertical de1024 bytes:
`byte=x+128*(y/8)` y `mask=1<<(y%8)`. Un bitmap fuente no es una página SSD1306.

El registro semántico de la guía es el ID estable de diseño. El enum AFR2 es
solo un mapeo concreto. Un proyecto puede mapearlo a sus propios identificadores
sin cambiar el significado. No redibujar pictogramas existentes por conveniencia.

## Qué está heredado y qué se propone

Revisión4 incorpora la segunda imagen del usuario, una lista dentro de card
con fila invertida. Se preserva como `assets/inspiration/quick-actions-list.png`.
`references/quick-actions.md` define título y contador de card arriba, sin
puntos, y gestos de una secuencia agrupada de acciones. No se copian funciones
de alumnado ni reloj. Sin modificación de renderers activos.

Revisión3 añade la imagen de monitor suministrada por el usuario como
inspiración: cabecera invertida, paneles con énfasis y numerales propios5×7.
`references/dashboard.md` detalla adaptación a128×64; no se copia su densidad
ni sus funciones. `assets/numerals5x7.json` es recurso nuevo, no un port de
fuente firmware ni una extracción de la imagen. Diseño y atlas, sin migración.

Revisión OLED128-2: [componentes](components.md) y `assets/components.json`
son el diseño vigente. El antecedente OLED128-1 y el firmware no se confunden
con ese perfil nuevo. El usuario pide largo=retorno y nomenclatura gráfica;
todavía no se migraron ATmega/Web. No editar firmware al actualizar la skill.

Se leyó el AutoMicro resguardado dentro del proyecto, sin modificarlo:
`Atmega/reference/f4/Core/Src/oled_utils.c`, funciones
`OledUtils_RenderIRGraphScene`, `OledUtils_DrawIRGroupFrame`,
`OledUtils_UpdateIRBars`, `OLED_MiniGlyphRow` y `OLED_MiniTextWidth`;
y `Web/reference/web/src/screens/sensorScreens.ts` con ocho barras. La ruta
histórica D:/AutoProyecto no está disponible en esta sesión. Se utiliza la
referencia local, sin ejecutar el proyecto fuente.

| Regla/patrón | Estado |
| --- | --- |
| OLED128×64 y familia visual consistente | requisito del usuario |
| Referencia al encoder mediante su icono | requisito actual; migración aún pendiente |
| Fuente7×10, menús3×16, marco y scroll | observado en fuente actual |
| Estado3líneas, editor, teclado, red, gráficos y secuencia | observados con variaciones actuales |
| Cabeceras de familia con icono14 | propuesta para uniformar, no aplicada globalmente |
| ESTADO-3/plain e icon-row, alineación de valores y prioridades | especificación propuesta |
| MEDIDA-1 y confirmación como patrones registrados | propuesta; no nueva fuente o renderer implementado |
| Variante pequeña de cada icono complejo | recurso pendiente si se necesita |
| Largo=volver, encoder+OK, encoder mantenido+flecha, gap4px y centro común | pedido humano actual, diseñoOLED128-2 |
| Barra única carga/progreso y sensores con ID arriba/valor abajo | pedido humano actual, componentes diagramados |
| Mini números3×5 y ocho sensores simultáneos | recurso de diseño desde referencia AutoMicro |
| Fidelidad visual/física y legibilidad | no comprobadas |

## Adaptar a un proyecto

Leer su interfaz de entrada real, biblioteca gráfica, catálogo y restricciones.
Mantener este lenguaje visual; mapear tipos/datos/servicios al sistema dueño.
No convertir decisiones de transporte AVR en requisitos del diseño universal:
128×64 no identifica por sí solo driver, dirección I2C, voltaje o MCU.
El framebuffer de1024 bytes corresponde a un bit por píxel; no incluye stack,
recursos, colas ni buffers del bus. Una copia Web no acredita viabilidad SRAM.

Para **AutoFinalRobo2**, aplicar también la skill local
`AutoFinalRobo2-Contracts/.agents/skills/afr2-screen-authoring/SKILL.md`:
pantalla y réplica Web juntas, catálogo compartido, recurso/renderer vinculado,
ficha AFR2 y estados separados atmega/esp/web. Leer navegación vigente en vez de
generalizar apartados históricos de los documentos. No cambiar hashes de port
sin adaptar algoritmos. No regenerar/compilar/promover sin autorización vigente.

Para otro proyecto, esta skill funciona sin esa skill local ni sus rutas.
No crear catálogo paralelo si ya existe uno. No copiar IDs, comandos, hardware,
nombre del robot o funciones que el proyecto nuevo no tiene.
