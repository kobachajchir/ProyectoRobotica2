# AFR2-20261109-13 — Documentación dimensional v17 y primer borrador del informe

Pedido: completar la documentación pendiente de las láminas iniciales con la skill `fusion-technical-sheets` y entregar un primer borrador del informe del proyecto.

## Entrega

- CAD: libro complementario de **224 hojas A3**, separado en **seis tomos**, para **66 definiciones fabricadas/adaptadas**, incluidas las **30 definiciones impresas**, y seis conjuntos del brazo. Vistas ortogonales, cotas exteriores BRep, coordenadas/diámetros de superficies cilíndricas, cortes documentales, BOM y despieces isométricos por tramos.
- Fuentes: extracción BRep de 237 parámetros y superficies, catálogo, vistas/cortes, manifiesto, CSV y scripts. Se conserva el F3D del 02/11 y la entrega anterior sin modificarlos.
- Contracts: primer borrador independiente **D1, 32 páginas**, PDF y DOCX editable con ecuaciones/matrices OMML, fuentes Markdown/JSON y tres PDF de consulta. Incluye matemática del brazo y base Mecanum, modelo integrado, electrónica, sensores, software, resultados históricos, mejoras y pendientes.

Rutas:

- [Documentación CAD](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/laminas_20261109/README.md).
- [Informe D1](../docs/informe_20261109/README.md).

## Procedencia y alcance

Se importó la copia documental `AFR2_Fuente_Laminas_20261102.f3d` en un documento independiente de Fusion para lectura de superficies. Linaje `urn:adsk.wipprod:dm.lineage:EcPmilmLR5e65I7_wwDoSg`; geometría capturada originalmente el 02/11/2026 a las 11:39:50 UTC. La extracción de interfaces del 09/11 no es una revisión de geometría.

Derivación gráfica con sólidos reales, discretización de aristas 0,03 mm y malla 0,12 mm para siluetas/cortes. Proyecciones reproducibles no asociativas; no se crean hojas Drawing nativas. Los desplazamientos de despiece son documentales. No se atribuyen trayectorias de desmontaje comprobadas ni tolerancias de fabricación a la tolerancia de discretización.

Se completa la cobertura nominal del catálogo. **No se liberan planos de taller:** ajustes, tolerancias, referencias funcionales aprobadas, material/proceso, roscas y componentes comerciales reales requieren decisiones adicionales. Los perfiles de dientes representados por discos no se convierten en geometría fabricable. Estado detallado en `CIERRE_FABRICACION.md`.

El informe conserva como históricos los resultados de estabilidad del 01/11, incluyendo el factor frontal 1,023 bajo las hipótesis de ese análisis. No se recalculó estabilidad ni se declara capacidad de carga. La matemática se deriva del descriptor v17, con DH equivalente de exposición académica; no se afirma implementada una IK general de seis ejes. Los signos de Mecanum se toman del contrato vigente y los 370 pulsos/vuelta del dato informado en configuración.

## Estado

- CAD: documentación nominal y renders documentales generados; sin modificación del maestro, fabricación, aprobación de ajustes ni nuevo análisis mecánico.
- Contracts: borrador documental y registro creados.
- ATmega / ESP / Web / Loader: sin cambios por esta entrega; sin pruebas nuevas.
- Informe maestro y Documentacion/: conservados.
- Prompts de envolventes: siguen pendientes del aviso de diseño terminado.

Sin tests, compilaciones, validadores de proyecto, barridos de movimiento, cálculos nuevos de estabilidad, FEA, pruebas físicas, puertos ni flasheos. La producción de PDF/DOCX/renders es documentación, no validación de producto. Sin commit ni publicación solicitados. No cambia la versión de producto por tratarse de documentación/CAD.
