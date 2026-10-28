# Fuentes y trazabilidad del estudio J1

Consulta 28/10/2026. Datos de catálogo y modelos equivalentes se distinguen de materiales/ensayos del robot real.

| Fuente | Uso | Límite |
| --- | --- | --- |
| Maestro Fusion `AFR2_Brazo_PVC32_AGV_v08_Housing` | 35 secciones, posiciones de apoyos/fijaciones y geometría comercial simplificada | No se modifica; documento sin cambios pendientes durante extracción |
| [Carga y dinámica v08](../revision_v08/README.md) | Estática y 315 instantes de frenadas individuales a 30°/s, con 200 g | Se añaden 63 instantes negativos de J1 y rotaciones de cargas: 379 estados fuente y 7.075 evaluados; sin acreditar prestación nominal bajo carga |
| [SKF608-2Z/C3](https://www.skf.com/rs/products/rolling-bearings/ball-bearings/deep-groove-ball-bearings/productid-608-2Z%2FC3) | Referencia 8×22×7 mm y C₀=1,37 kN | La página se recuperó por su índice; no identifica una unidad comprada ni permite intercambiar su juego con otra variante |
| [NSK, catálogo de rodamientos, parteC](https://www.nsk.com/content/dam/nsk/common/catalogs/ctrgPdf/bearings/e1103c_partc.pdf) | Fórmula de equivalente estático de ranura profunda | Selección estática, no fallo de carcasa PETG ni cálculo de impactos |
| [Prusament PETG TDS1.1](https://prusament.com/wp-content/uploads/2022/10/PETG_Prusament_TDS_2021_10_EN.pdf) | Referencia de módulo y contraste de adhesión entre capas; condiciones de probetas publicadas | Otra marca/proceso. No se asigna su resistencia al material del usuario |
| [NASA RP-1228](https://ntrs.nasa.gov/api/citations/19900009424/downloads/19900009424.pdf) | Grupos de fijación, carga excéntrica y combinación de esfuerzos | No define rigidez, precarga ni resistencia de nuestras piezas |
| [SPIROL, agujeros para insertos](https://www.spirol.com/resources/white-papers/how-to-design-the-proper-hole-for-heat-ultrasonic-inserts/) | Dependencia de resistencia respecto de plástico, agujero e instalación | No se adopta una carga de arrancamiento sin identificar inserto e impresión |
| [Böllhoff ISO10642](https://eshop.boellhoff.de/out/media/pdf/ISO_10642_Stahl_10.9___en.pdf) | Envolvente alternativa M3:Ø6,72/h1,86 máximos para C al ras | No se equipara automáticamente a una DIN7991 distinta; no se prescribe torque de apriete |
| [Bulonera Fastener Argentina](https://fastener.com.ar/productos/tornillos) | Oferta local de la familia Allen avellanada DIN7991 | Modelo/largo/stock y ficha de compra por confirmar; no se usa una lista de precios vieja como precio actual |
| [SKF6000-2Z](https://www.emarketplace.in.skf.com/deep-groove-ball-bearing/6000-2z) y [catálogo SKF](https://cdn.skfmediahub.skf.com/api/public/0901d196809a65c0/pdf_preview_medium/0901d196809a65c0_pdf_preview_medium.pdf) | Candidato de apoyo Ø10/26×8 mm para predimensionado | Todavía no instalado o modelado en J1 |
| [Oferta argentina serie6000 SKF](https://siglo21myh.com.ar/tienda-online/rodamiento-rulman-sellado-10x26x8mm-skf-6000rodamiento-skf-6000/) | Disponibilidad comercial local de esa medida | No prueba stock, cierre o juego de una variante concreta |
| [GX3270BLS de Starware](https://tienda.starware.com.ar/producto/servo-motor-digital-brushless-yipin-gx3270bls-70kg-5-84v-270gr-ip68-metal/) y [manifest v07](../revision_v07/fuentes_actuador.json) | Candidato J1:63 kgf·cm anunciados a 7,4 V, 90 g y hasta 7 A de bloqueo; geometría ya modelada para codo | Sin curva continua/torque-velocidad o frenada real; cotas inferidas del modelo se conservan |

Los cálculos de secciones y energía elástica, modelo de contacto D, reparto de brida y sensibilidades son elaboración propia sobre esas geometrías e hipótesis. No se importó un resultado FEA ajeno ni se asignó un certificado de material.

Los contornos se toman0,0137 mm por encima de cada plano solicitado para evitar caras coplanares/tangentes en la intersección. La posición efectiva se guarda en el JSON. Se aproximan curvas con error de cuerda máximo 0,0025 mm; no se presenta como exactitud de impresión. Los contornos cerrados y sus huecos se integran por fórmulas de polígono.

La resistencia de filamento/perfil sigue pendiente por respuesta explícita del usuario. Los criterios n=2 y Kt=2 son sensibilidades de predimensionado. Kt no proviene de una solución local del detalle y E isotrópico no sustituye ortotropía FDM. Los esfuerzos de tornillos son incrementos de servicio sin pretensión, y la matriz de fijaciones no resuelve apertura de contacto.
