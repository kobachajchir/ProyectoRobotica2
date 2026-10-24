# Candidatos CAD para aprobación

Fecha: 23/10/2026. Registro: AFR2-20261023-06.

El usuario autorizó buscar, descargar y guardar modelos, y revisarlos antes de modelar. Ningún candidato de este lote está aprobado para el ensamble. No se importaron ni adaptaron los archivos descargados en Fusion. No se realizaron pruebas, validadores ni comprobaciones geométricas. Los hashes del manifiesto son trazabilidad de descarga.

**Chasis: se modelará a mano**, por indicación del usuario. Se retira de la búsqueda de CAD. La plataforma acrílica sigue pendiente de decidir/reconstruir según el manual.

## Lote descargado

Los enlaces locales son relativos a este documento. Las descargas permanecen en vendor/, fuera de Git; el manifiesto y el script de recuperación sí se conservan.

| Componente | Archivos para abrir | Procedencia | Qué falta decidir |
| --- | --- | --- | --- |
| Mega2560, 1 unidad | [STEP](../vendor/revision-2026-10-23/arduino-mega.step), [FreeCAD](../vendor/revision-2026-10-23/arduino-mega.fcstd) | [FreeCAD-library](https://github.com/FreeCAD/FreeCAD-library/tree/f5ca01cdcaa2f69b9295a6382719c2d90628780c/Computing/SingleBoardComputers/Arduino); incorporación registrada por Charles Fox | Placa compatible genérica; comparar contorno, agujeros, USB, alimentación y conectores con OSOYOO. El nombre no acredita revisión idéntica. |
| SG90, 1 unidad | [Servo STEP](../vendor/revision-2026-10-23/Servo-sg90.step), [brazo de una salida](../vendor/revision-2026-10-23/SG90-1-arm-horn.step) | [FreeCAD-library](https://github.com/FreeCAD/FreeCAD-library/tree/f5ca01cdcaa2f69b9295a6382719c2d90628780c/Electrical%20Parts/Servos/SG-90); incorporación del servo por Gafeju | Referencia SG90 de montaje; discrepancia SG90/MG90 del manual sin resolver. Revisar brazo y cualquier cable incluido; no adaptar todavía. |
| HC-SR04, 1 unidad | [STEP](../vendor/revision-2026-10-23/HC-SR04.step) | [FreeCAD-library](https://github.com/FreeCAD/FreeCAD-library/tree/f5ca01cdcaa2f69b9295a6382719c2d90628780c/Sensors/Ultrasonic%20Sensors); incorporación registrada por Charles Fox | Comparar revisión de placa, agujeros y orientación del conector. |
| Celda 18650, 2 unidades | [STEP](../vendor/revision-2026-10-23/18650-cell.step) | [Innerscene, envolvente máxima](https://www.innerscene.com/tools/library/3d-parts/18650-cell-maximum-envelope-a6729085), CC0 | Modelo nominal genérico, no identifica marca ni variante de las celdas reales. |
| Portabaterías doble, 1 unidad | [STEP](../vendor/revision-2026-10-23/holder-2x18650.step) | [Innerscene, 2-cell holder](https://www.innerscene.com/tools/library/3d-parts/2-cell-18650-holder-97a0080c), CC0 | Alternativa genérica, envolvente publicada 42 × 76 × 16 mm. No acredita caja OSOYOO, tapa, interruptor ni patrón de fijación correctos. Aprobar o reemplazar. |

La biblioteca FreeCAD declara CC-BY-3.0 para sus piezas. Conservar atribuciones por archivo y los historiales de contribución descargados. Los nombres anteriores identifican la incorporación al repositorio, no prueban por sí solos la autoría original. No se atribuye autoría del proyecto a estos CAD.

## Ya disponibles en Fusion

En Admin Project, abiertos y observados durante esta conversación: KY040RotaryEncoderCAD (encoder sin breakout), KY040RotaryEncoderKnobCAD (perilla) y OLED0.96CAD. Son candidatos aportados por el usuario, pendientes de aprobación para integración. Todavía no se exportaron ni se registraron cotas o licencia.

| Archivo Fusion | ID |
| --- | --- |
| KY040RotaryEncoderCAD | urn:adsk.wipprod:dm.lineage:Jj1zXaHGTyS1uiLp4GPGKg |
| KY040RotaryEncoderKnobCAD | urn:adsk.wipprod:dm.lineage:s04n0JjdR5CFvSnsEkM0eA |
| OLED0.96CAD | urn:adsk.wipprod:dm.lineage:hxP8G0yiTpe-_0zUYUAw3A |

## Faltantes y búsquedas no resueltas

| Componente | Cantidad objetivo | Resultado y siguiente paso |
| --- | --- | --- |
| Ruedas Mecanum 80 mm, izquierda/derecha | 2 + 2 | No se obtuvo nuevo CAD con correspondencia suficiente. El insumo local de 80 mm sigue como referencia sin selección; las ruedas locales de 60 mm no se aceptan como sustitutas. Comparar rodillos, ancho y acople. |
| GM25-370 con Hall | 4 | Variante de la ficha GM25-370-24140-75-14.5D10 como referencia documental; instalado no identificado. [Candidato JGA25-370](https://nxp.gitbook.io/nxp-cup/nxp-cup/jga25-370-files) enlaza GrabCAD, pero es otra variante, 350 rpm declaradas; no descargado ni seleccionado. Motor local de 37 mm no corresponde automáticamente. |
| Soportes de motor y acoples | 4 + 4 | Insumo local Motor Bracket.step sin correspondencia establecida; acoples exactos pendientes. |
| Plataforma acrílica original | 1 | Sin CAD exacto obtenido; reconstrucción documental pendiente de aprobación para modelar. |
| OSOYOO UART Wi-Fi Shield V1.3 con ESP-12S | 1 | Sin CAD exacto obtenido. [Referencia fabricante](https://osoyoo.com/2020/07/31/introduction-of-osoyoo-uart-wifi-shield/). No reemplazar por un shield genérico ni duplicar el ESP. |
| OSOYOO Model Y V2.0 | 1 | Sin CAD exacto obtenido. [Referencia fabricante](https://osoyoo.com/?p=46344). No sustituir por Model X/L298N. |
| Sensor OSOYOO de cinco canales | 1 | [Candidato específico de Marathon OS](https://marathon-os.com/library/osoyoo-5-channel-infrared-tracker-sensor-698053d23cb786ad678602bb). Página publica 81 × 33,28 × 12,19 mm y formato STEP. La descarga gratuita abre inicio de sesión; no descargado. Autor/licencia no establecidos. El enlace aparente de archivo devolvió HTML, conservado como respuesta HTML, no como CAD. |
| Voltímetro OSOYOO de tres dígitos | 1 | Sin CAD correspondiente obtenido; referencias genéricas no seleccionadas por faltar identificación dimensional. |
| Soporte HC-SR04 para servo | 1 | Sin modelo correspondiente obtenido; no confundir con soporte fijo ni diseñar pieza nueva. |
| Tornillería y separadores | Según manual | Pendientes modelos y revisión de material/longitudes. Cantidades documentales abajo. |

## Fijaciones documentales para buscar

Del informe técnico local, tabla 3.2; cantidades documentales, no inventario físico: 16 M3.5×12 + 16 arandelas M3.5 + 16 tuercas M3.5; 8 M3×6; 4 M2.5×10; 2 separadores de cobre M3×40; 12 separadores plásticos M3 + 12 tuercas; 8 tornillos plásticos M3; 4 M3×10 + 4 tuercas; 2 autorroscantes M2.2×8 para servo; 4 M1.4×8 + 4 tuercas para ultrasonido; 1 autorroscante M2×4 para soporte en servo. Los dos tornillos/tuercas de la pieza nueva quedan fuera de esta etapa.

## Continuación

Abrir los STEP descargados para revisión del usuario, registrar aprobación/rechazo por componente y recibir mejores modelos si corresponde. Continuar la búsqueda de faltantes sin iniciar geometría, adaptaciones ni ensamble hasta que el usuario apruebe los modelos. No ejecutar scripts incluidos en descargas.
