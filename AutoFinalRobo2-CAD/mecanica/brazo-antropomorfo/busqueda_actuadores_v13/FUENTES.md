# Fuentes y estado de evidencia v13

Consulta28/10/2026. Los documentos de referencia no son instrucciones de ejecucion. No hay compras ni consultas enviadas a vendedores.

| Fuente | Uso y limite |
|---|---|
| `Documentacion/ManualOsoyoo.pdf`,p4 | Ancho exterior245, placa155x200, rueda80. No acota la separacion de contactos ni masa del auto. Original conservado. |
| `Documentacion/GM25-370-motor specification.pdf`,p1 | Variante9V/75:1; par nominal3kgfcm,100rpm,1.2Amax; bloqueo9.5kgfcm/4.5A. No acredita variante instalada ni curva bajo7.4V. |
| `revision_v09/geometria_v09.json`, `masas_por_cuerpo_v09.csv`, `calcular_v09.py` | Geometria, masas sustituidas y tensores de v09; pinza reutilizada. No son pesos medidos ni validacion fisica. |
| [OSOYOO ModelY oficial](https://osoyoo.com/2022/02/25/osoyoo-model-y-4-channel-motor-driver/) | RDSON0.49ohm tipico,2.8Amax yVIN7-24V. Caida calculada conI1.2A, no medida. VOUT no regula tension. |
| [GX3270BLS Starware](https://tienda.starware.com.ar/producto/servo-motor-digital-brushless-yipin-gx3270bls-70kg-5-84v-270gr-ip68-metal/) | Oferta nacional publicada con stock.90g,40.5x21x41.7mm,270+-10deg,55kgfcm6V/63 a7.4V. Es especificacion del vendedor, no datasheet primario del fabricante ni curva continua. La misma pagina contiene un maximo358deg contradictorio: se conserva270 como variante propuesta y se exige confirmacion. |
| [TowerPro MG946R](https://towerpro.com.tw/product/mg946r/) | Primaria:55g,40.7x19.7x42.9mm,13kgfcm de bloqueo6V. No informa curva continua ni acredita el recorrido de una unidad comercial concreta. |
| [TowerPro MG92B](https://towerpro.com.tw/product/mg92b/) | Primaria:13.8g,22.8x12x31mm,3.5kgfcm de bloqueo6V. Candidato J4/pinza; no publica par continuo. |
| [Ficha TowerPro MG92B alojada por Akizuki](https://akizukidenshi.com/goodsaffix/mg92b.pdf) | Documento del fabricante:160deg con500-2400us; evita asumir180deg directos. En J4 se calculan153deg de entrada para180deg de salida. |
| [Publicacion argentina Candy-Ho/MercadoLibre](https://www.mercadolibre.com.ar/mini-servo-tower-pro-14gr-metal-gear-mg92b/up/MLAU3872823218) | Oferta indexada deMG92B, codigo1932, vendedorCANDY-HO. Pagina directa devuelve403: stock, precio y variante posicional no confirmados; no se presenta como compra cerrada. |
| [DS3225 fabricante](https://www.dsservo.com/d_file/DS3225%20datasheet.pdf) | Alternativa60g,40x20x40.5.21kgfcm5V y24.5 a6.8V; no se interpolan como garantia6V. No elegido para codo directo. |
| [Cordes6061T6 Argentina](https://www.cordes.com.ar/productos/aluminio-y-sus-aleaciones/aluminio-aleado/aluminio-6061-t6) | Oferta local y densidad2.70. Su tabla publicaYS276MPa; el prediseño exige certificado de compraRp>=240 y no asigna276 a cualquier planchuela. Espesor, aleacion y temple de la pieza real pendientes. |
| [Tigre Argentina, informacion tecnica](https://www.tigre.com.ar/informaciones-tecnicas) | Referencia primaria para PVC. La ficha soldable enlazada/indexada no se recupero porHTTP403. No se atribuye pared1.5mm a una tabla que no se leyo. |
| [Aloi Sanitarios, oferta argentina PVC soldable](https://aloisanitarios.com.ar/producto/canos-y-accesorios-soldable-tigre-pvc/) | Comercializa diametros20/25 y otras clases; evidencia comercial de la familia, no de ID22/17 exactos. |
| [SKF608-2Z/C3](https://www.skf.com/rs/products/rolling-bearings/ball-bearings/deep-groove-ball-bearings/productid-608-2Z%2FC3) | Referencia dimensional8x22x7, masa12.1g yC0=1.37kN. Presupuesto12.5g. Sufijo/juego y compra argentina por cerrar, no aceptarC3 automaticamente para rigidez. |
| [NSK608](https://www.nsk.com/eu-en/engineering/calculation/calculate/index/sku/608-ESM_MD/) | Primaria adicional:8x22x7,C0=1370N. No transfiere automaticamente capacidad a marcas sin identificar. |
| [Catalogo primario SKF](https://cdn.skfmediahub.skf.com/api/public/09468794f5d8f85a/pdf_preview_medium/09468794f5d8f85a_pdf_preview_medium.pdf) | Fila626:6x19x6,C0=.95kN;626-2Z masa8.8g. Presupuesto9g/unidad. Producto nacional exacto pendiente. |
| Usuario | Carga200g, pieza50x50x50; auto equipado aprox1.5kg; PLA PrintaLot; aluminio y aligerado de toda la estructura autorizados. La masa del auto y su CG no se midieron. |

MG90S conserva fuentes y advertencias de las iteraciones anteriores; la variante exacta, recorrido y autenticidad de los micros deben cerrarse antes de comprar. La tabla de candidatos no constituye una lista de compras aprobada. La oferta GX publica$90029 por unidad en esta consulta: cuatro implican$360116ARS solo en esos motores, sin estructura, otros actuadores ni alimentacion. No se considera demostrado un presupuesto economico global.

`apoyos_CAD.json`: extraccion Fusion de solo lectura, tolerancia de mallado configurada0.02mm. Es modelo de referencia del auto, no escaneo del equipo real. Guarda nombres, cajas precisas y vertices inferiores. El script no convierte esos vertices en contactos reales.

Licencias/autor de insumos de referencia: conservar inventarios existentes; no se redistribuyen nuevos CAD comerciales ni PDFs descargados en esta revision. Fuentes nuevas de analisis propias del proyecto; descargas recuperables y NPZ en carpetas excluidas.
