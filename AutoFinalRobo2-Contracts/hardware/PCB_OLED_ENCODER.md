> **Vigente AFR2-20261025-12:** [PCB y soporte creados](../changes/2026-10-25-12-oled-encoder-pcb-support.md). J1 hembra 1x05 paso 2,54 mm; J2 JST de **2,00 mm**, +3V3/GND, confirmado por usuario; OLED en socket hembra 1x04 de 2,54 mm con GND/VCC/SCL/SDA de frente. Encoder de contactos a GND, sin VCC directo. Las secciones siguientes son requisitos históricos; tensión, paso de power y tornillería pendientes allí quedan reemplazados por esta revisión. Sin ERC/DRC ni prueba física.

# Placa OLED/encoder y soporte: requisitos para la próxima etapa

**Final AFR2-20261025-13:** [PCB/soporte manuales y modelo Web/ESP](../changes/2026-10-25-13-final-cad-web-model.md). Componentes TH; R2/R3 de 10 kΩ desde CLK/DT a +3V3. Sólo fondo, PCB de 2 mm directa al soporte, encoder directo y torretas comerciales de 15 mm sólo para OLED. Soporte sobre torretas comerciales de 5 mm al auto; tornillos y tuercas inferiores para PCB. Sin ERC/DRC ni prueba física; comprobación CAD solicitada sin penetraciones PCB–fondo. Sustituye los requisitos históricos de montaje que siguen.

Actualización del usuario 2026-10-23. [Pinout](PINOUT.md). Estado: requisitos; no esquemático/PCB/modelo mecánico creados.

El shield Mega ya expone las cabeceras rotuladas SDA y SCL. Los cables I2C se conectarán allí; D20/PD1 y D21/PD0 son las equivalencias lógicas de Mega, no rótulos que deban estar impresos en el shield. El mapa ilustrado del shield corresponde a UNO: no adoptar A4/A5 ni D18/D19 como I2C del Mega. La ruta física de la revisión instalada queda por identificar; conservar A4 para IR1. [Evidencia del shield](WIFI_SHIELD.md).

La PCB nueva monta OLED y encoder de interfaz. Debe exponer DOS conectores externos de paso 2,54 mm:

| Conector | Pin propuesto | Red | Destino externo |
| --- | --- | --- | --- |
| J1 señales 1x5 | 1 | CLK | Mega D2 / PE4 / INT4 |
| J1 señales 1x5 | 2 | DT | Mega D3 / PE5 / INT5 |
| J1 señales 1x5 | 3 | SW | Mega D42 / PL7 |
| J1 señales 1x5 | 4 | SDA | Cabecera SDA del shield (Mega D20 / PD1) |
| J1 señales 1x5 | 5 | SCL | Cabecera SCL del shield (Mega D21 / PD0) |
| J2 alimentación 1x2 | 1 | VCC | Alimentación común de OLED y encoder |
| J2 alimentación 1x2 | 2 | GND | Masa común de OLED, encoder y Mega |

El orden de pines anterior es la propuesta documental basada en el orden pedido; confirmar orientación/pin1 al dibujar huellas. Añadir serigrafía de todas las señales y polaridad. Cableado externo Dupont. VCC común es requisito; su valor numérico queda pendiente de identificar módulos y pull-ups. No asignar 5 V ni 3,3 V sin evidencia y no dividir los conectores de alimentación por iniciativa. Resolver compatibilidad eléctrica antes de cerrar esquema; no se presupone regulador o adaptación de nivel ya existente.

## Montaje mecánico futuro

La cubierta superior la aportará el usuario mediante planos/informe/idea. El objetivo es un conjunto con PCB, soporte impreso 3D y cubierta fijados por DOS tornillos y sus tuercas. No se ha elegido M2/M2.5/M3, longitud, posición de agujeros, altura ni espesor. Resolver la pila de piezas, acceso a tuercas/tornillos, mandos/conectores, visibilidad OLED y espacio bajo la cubierta desde los planos reales.

Cuando el usuario continúe: leer la skill de ruteo que indique y los archivos mecánicos. Preparar esquema/PCB KiCad editables, plano acotado, ensamblaje de referencia y soporte 3D paramétrico editable con exportaciones de fabricación/impresión acordadas. Planear ERC/DRC y comprobación mecánica como actividades separadas que requieren pedido expreso según las reglas actuales; no afirmar manufacturabilidad por tener archivos.

Datos para esa etapa: contorno y cotas/unidades de cubierta, agujeros y zonas disponibles, espesor/material, dimensiones/modelos/pines del OLED y encoder, altura del eje/perilla, tensión VCC, orientación de Dupont, tornillería y espacio inferior. No pedirlos ahora como bloqueo del trabajo documental. Ubicación decidida por el usuario en AFR2-20261023-04: [AutoFinalRobo2-CAD](../../AutoFinalRobo2-CAD/README.md), quinto módulo del único Git en D:/ProyectoRobotica2. KiCad irá en kicad/oled-encoder, la pieza nueva en mecanica/pieza-nueva y el ensamblaje del vehículo en mecanica/auto-completo. Estructura preparada; diseño todavía pendiente.

La regulación/traducción UART declarada en el shield Wi-Fi no fija VCC ni pull-ups de esta placa OLED/encoder. No tomar VIN/VOUT como salida regulada para J2. KEY/RST pertenecen al ESP y no agregan botones a la navegación del usuario.

## Contexto del plan 3D / AFR2-20261023-05

El usuario identifica OLED 0,96 pulgadas y encoder mecánico KY-040 sin placa breakout, con perilla. Esta identificación orienta la búsqueda de modelos, pero no determina controlador, VCC, huellas ni cotas exactas. El [plan de Fusion](../../AutoFinalRobo2-CAD/docs/PLAN_MODELO_3D_FUSION.md) reserva espacio y prepara esas referencias; pieza nueva, soporte, PCB y modificación de cubierta siguen pendientes de planos. El ensamble base se planifica sin cables/mazos/jumpers, conservando conectores físicos.
