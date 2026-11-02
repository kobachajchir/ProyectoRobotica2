# Primera tanda de laminas A3 y despiece

AFR2-20261102-04. Pedido de empezar la documentacion, con referencia al dossierBioLog y a la imagen de composicion aportada el02/11/2026. Se genera documentacion derivada; no cambia la mecanica.

**Revision B, AFR2-20261102-06:** a pedido del usuario, se aumenta la ocupacion visual del3D mediante recorte de margenes vacios, se reemplazan las hileras planas por explosiones documentales isometricas y se agrega `Koba Chajchir` al rotulo. El despiece usa272mm de ancho, mas de media hoja, con unaBOM lateral independiente; no invade las vistas acotadas ni los bloques inferiores.

**Fichas generales revisionC, AFR2-20261102-08:** la hoja general del brazo reemplaza la lista lateral por seis renders de sus subconjuntos. Se agrega una segunda ficha del robot integrado con seis renders funcionales del auto. El libro pasa a14 hojas y tambien se entregan las dos fichas comoPDF independiente. Las seis piezas individuales siguen siendo preliminares: no estan terminados todos los planos del brazo.

**Llamadas locales del despiece, 02/11/2026:** se sustituyeron exclusivamente las paginas5-8 delPDF por los mismos renders con numeracion junto a cada pieza y lideres cortos. Las otras paginas se conservaron desde elPDF anterior y elPDF independiente de fichas no se reescribio. `llamadas_cortas.json` guarda las posiciones; `acortar_llamadas.py` permite recomponer solo esas paginas. La lista lateral conserva las29 referencias. Si se cambia el encuadre o el render, regenerar las posiciones antes de reutilizarlas.

## Entrega

- [PDF A3,14 hojas](AFR2_Laminas_Iniciales_A3.pdf).
- [Dos fichas generales A3](AFR2_Fichas_Brazo_y_Robot_A3.pdf): brazo con subconjuntos y robot integrado con conjuntos del auto.
- [Indice de hojas](INDICE.md).
- [Libro editable JSON](libro.json), [vistas vectoriales](vistas.json), [interfaces nativas](interfaces.json) e [inventario completo](inventario.json).
- [BOM del conjunto J5/J6](BOM_A500.csv), con posiciones y rutas completas.
- [Fuente nativa capturada](AFR2_Fuente_Laminas_20261102.f3d): exportacion de archivo sin mover ni sobrescribir el maestro abierto.
- Skill implementada en [Contracts](../../../../../AutoFinalRobo2-Contracts/.agents/skills/fusion-technical-sheets/SKILL.md), tambien instalada en `C:/Users/kobac/.codex/skills/fusion-technical-sheets` para descubrirla en futuras tareas.

La tanda incluye una hoja general, un plano de conjunto J5/J6, su lista de29 ocurrencias, cuatro tramos de explosion isometrica y seis hojas individuales. Las otras secciones del brazo estan inventariadas pero sus planos detallados y explosiones aun no estan generados. El listado general A100-A600 organiza esa continuacion; no significa que sus hojas ya existan.

Desde revisionC se agrega ademas la ficha del robot integrado. `fichas_conjuntos.json` registra las fuentes y los objetos/raices incluidos en cada render. La asignacion de grupos sirve para identificar la composicion funcional, sin afirmar que cada grupo sea una pieza fabricable o una secuencia de montaje. [Estado detallado de cobertura](ESTADO_PLANOS.md).

## Composicion aplicada

A3 horizontal. Planos a la izquierda;3D a la derecha en las hojas de pieza y plano armado. Las explosiones utilizan una columna deBOM de103mm y un render de272mm de ancho. Se recortan margenes alfa antes de encajar la imagen, manteniendo las coordenadas de globos. Notas en contenedor inferior izquierdo de194x47mm; rotulo en contenedor inferior derecho de200x47mm, separados6mm. No hay notas superpuestas al rotulo. Vistas de pieza alineadas en primer diedro: alzado arriba, planta debajo y perfil izquierdo a la derecha. Las escalas de vistas estan indicadas por hoja; las isometricas son sin escala. Responsable de documentacion confirmado: Koba Chajchir; no constituye firma de aprobacion.

Los renders son del CAD real, en PLA blanco y materiales de referencia del modelo. No se uso generacion de imagen porIA. No se copiaron autor, institucion, normas, dimensiones ni tolerancias deBioLog.

## Geometria y estado documental

El archivo conectado se llama `AFR2_Brazo_v17_Base_V7_Correa_J2`, pero conserva el mismo ID de maestro utilizado en la captura anterior `AFR2_Brazo_v17_Base_V7_J3_Remoto`: `urn:adsk.wipprod:dm.lineage:EcPmilmLR5e65I7_wwDoSg`. Se confirma la fuente por identificador y se registra el nombre real; no se renombro el documento.

Captura de425 instancias de solidos y351 definiciones, incluidas las ocultas. Se excluyen auto,pan,envolvente y cubo. Hay297 nombres de componentes con solidos,30 identificados como impresos; nombres distintos de tornillos equivalentes todavia no se normalizaron como una unica referencia de compra.

Las vistas2D son vectores de aristasBRep y siluetas de superficies curvas extraidas de la malla nativa de0,12mm. La visibilidad visible/oculta tambien se aproxima con esa malla; las curvas de aristas se muestrean a0,03mm. Las cotas envolventes usan bounds delBRep extraidos separadamente, no pixeles. Esto no es un Drawing nativo asociativo: ante un cambio de CAD hay que regenerar captura y libro.

**Estado preliminar:** faltan datums, patrones de agujeros, cortes y ajustes funcionales para cerrar planos de fabricacion. La lista de diametros contiene superficies cilindricas, no cantidades de agujeros. Las masas de pieza son volumenCAD por densidad declarada, no masa real impresa ni un porcentaje de infill.

Las explosiones isometricas conservan orientaciones y posiciones relativas del conjunto y aplican traslaciones documentales para separar componentes. No son una secuencia de desmontaje validada. Las traslaciones y globos se conservan en `explosiones_isometricas.json`. Los cuatro tramos y globos comparten posiciones conBOM_A500.csv. Cada rodamiento/servo mantiene juntos los solidos internos de su ocurrencia. La hilera plana de revisionA se conserva solo en las fuentes/imagenes historicas; no es la vista utilizada en elPDF actual.

No se hicieron pruebas de software, builds, nuevos calculos de resistencia, barridos de interferencias ni ensayos fisicos. Los renders/previews forman parte de la produccion de laminas; no certifican aptitud para fabricar.

## Regeneracion

1. Ejecutar `extraer_laminas.py` medianteFusionMCP sobre el ID esperado; escribe la captura de mallas/aristas en cache, sin mover el maestro.
2. Ejecutar `renderizar.py` conBlender en segundo plano para las piezas, el conjunto y la general. Ejecutar `completar_vistas.py` para perfiles del conjunto y despieces por tramos. Ejecutar `corregir_perfiles.py` para regenerar las tres vistas en primer diedro con siluetas curvas, sin renderizar de nuevo. Ejecutar `replantear_explosiones.py` para reemplazar las hileras iniciales por las vistas isometricas de revisionB sin perder las vistas vectoriales ya corregidas.
3. Ejecutar `preparar_libro.py` y luego el compositor de la skill: `componer.py libro.json AFR2_Laminas_Iniciales_A3.pdf`.

Para incluir las fichas nuevas, ejecutar primero `renderizar_fichas.py` enBlender, luego `preparar_libro.py` y `generar_fichas.py`. Este ultimo publica el libro completo y elPDF separado de dos fichas. Las imagenes del auto se derivan de `AutoFinalRobo2-Final-20261025.blend`, coleccion `AFR2_Final_Detallado_20261025`, conservando materiales y transformaciones guardadas; no provienen de un nuevo modelado.

El compositor requierePython conreportlab; Blender4.5.1 usa suPython/numpy/mathutils. La captura de mallas120MB y el.blend derivado son cache regenerable. Las fuentesF3D,JSON de vistas, renders, scripts y PDF conservan la entrega editable/reproducible. Rutas del libro se regeneran en la PC de destino con `preparar_libro.py`.

## Referencias leidas

Dossier `C:/Users/kobac/.codex/visualizations/2026/10/27/01a17b8d-eadd-7e01-91ab-9084cced46eb/biolog_analysis/00_INDICE_DEL_DOSSIER.md`: indice general, indice del atlas,ATLAS_05,ATLAS_15,ANALISIS_03 yANALISIS_20, y paginas/hojas de contacto originales. El anexo de44 laminas se uso como referencia de organizacion y la figura5 como referencia de despieces por articulacion. El requerimiento actual de notas separadas del rotulo prevalece sobre esa referencia.
