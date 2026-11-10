# Documentación dimensional del brazo v17 · revisión D

09/11/2026. Complemento de la entrega del 02/11. Responsable documental: Koba Chajchir.

## Lectura

- [Libro completo A3](AFR2_Planos_Brazo_V17_Documentacion_Dimensional_A3.pdf).
- [Seis tomos de consulta e índice por tomo](tomos/INDICE.md).
- [Índice de hojas del libro completo](INDICE.md).
- [Primer borrador del informe del proyecto](../../../../../AutoFinalRobo2-Contracts/docs/informe_20261109/README.md).

El libro contiene 224 hojas: documentación de seis conjuntos y de 66 definiciones fabricadas o adaptadas, incluidas las 30 definiciones impresas. Las ocurrencias repetidas comparten plano. Las piezas comerciales no se convierten artificialmente en planos de fabricación. Se mantiene la nomenclatura P501–P506 de la entrega anterior.

Cada pieza tiene vistas ortogonales con cotas envolventes BRep, representación del sólido y corte documental; las hojas -I01, -I02, etc. contienen coordenadas, diámetros y longitudes de superficies cilíndricas agrupadas por eje. Los grupos -LP01, -LP02, etc. listan ocurrencias CAD. Los despieces -E conservan orientaciones y desplazan las piezas para lectura: no demuestran una trayectoria de desmontaje.

## Lo completado y el límite real

Se completó la cobertura nominal de los componentes del catálogo: las 27 definiciones impresas que no tenían hoja individual y los fabricados/adaptados de otros materiales, además de ampliar las seis hojas existentes. Se añadieron documentación de interfaces, cortes y despieces de los seis conjuntos.

**No son planos liberados de taller.** Quedan por decidir ajustes, tolerancias, datums funcionales aprobados, roscas/enganches, acabado, material efectivo, orientación y compensaciones de impresión y proceso de cada pieza. Las cotas de un sólido no permiten inventar esas decisiones. Véase [CIERRE_FABRICACION.md](CIERRE_FABRICACION.md).

Las hojas son vectores y renders reproducibles desde el CAD, **no Drawing nativo asociativo de Fusion**. Cotas exteriores e interfaces: BRep. Aristas curvas discretizadas a 0,03 mm; siluetas y cortes a partir de malla CAD de tolerancia 0,12 mm. Esos valores describen la representación, no una tolerancia admisible de fabricación. Los ejes documentales H/E agrupan superficies coaxiales; no cuentan agujeros por cara. Las longitudes de tramos no implican profundidad de taladro o rosca.

## Fuente y preservación

Se importó en un documento independiente la [copia F3D del 02/11](../laminas_20261102/AFR2_Fuente_Laminas_20261102.f3d), sin modificar el maestro ni los originales. Linaje: `urn:adsk.wipprod:dm.lineage:EcPmilmLR5e65I7_wwDoSg`. La captura geométrica original es del 02/11/2026 11:39:50 UTC. Extraer interfaces el 09/11 no convierte esa geometría en una nueva revisión mecánica.

La extracción nativa conserva 237 parámetros. Las 425 instancias sólidas originales no son 425 piezas fabricables. Las placas comerciales con hijos se agrupan; las rutas completas permanecen en el CSV de conjuntos. Los conjuntos de la plataforma móvil están documentados en las fichas anteriores; esta ampliación individual corresponde al brazo, conforme a los pendientes de ESTADO_PLANOS del 02/11.

## Fuentes editables y regeneración

| Archivo | Función |
|---|---|
| `catalogo_piezas.json` | Definiciones, cantidades, material documental y códigos |
| `interfaces_nativas.json` | Superficies BRep, aristas, límites y parámetros de la copia importada |
| `vistas_completas.json` | Proyecciones, cortes, renders, posiciones y desplazamientos documentales |
| `libro.json` | Manifiesto de hojas y estado nominal |
| `BOM_FABRICADOS.csv` | Definiciones y cantidades de componentes fabricados/adaptados |
| `BOM_CONJUNTOS.csv` | Posiciones y rutas por ocurrencia |
| `INTERFACES.csv` | Coordenadas, diámetros, tramos y vector completo de cada eje |
| `capturar_interfaces.py` | Lectura nativa en Fusion; requiere la copia correcta activa |
| `renderizar_geometria.py` | Deriva vectores, cortes y renders en Blender 4.5 |
| `componer_documentacion.py`, `componer_base.py` | Composición A3 con ReportLab y Pillow |
| `dividir_tomos.py` | Separa el PDF por conjuntos con pypdfium2 |

Orden de producción: captura de interfaces → derivados Blender → composición A3 → división de tomos. La composición usa Arial instalada; los scripts leen rutas relativas a esta carpeta y la captura de geometría conservada en `../laminas_20261102/cache/geometria.json`. Los renders son derivados regenerables. `componer_base.py` es copia de los auxiliares de la skill fusion-technical-sheets utilizada; el compositor nuevo adapta organización, cortes y tablas a esta entrega.

Se generaron documentos y renders documentales. No se ejecutaron tests, compilaciones, validadores de proyecto, barridos nuevos, FEA, simulación de cargas, pruebas físicas ni flasheos. Tampoco se ejecutaron los prompts de envolventes pendientes. Documentación y CAD no modifican la versión de producto.
