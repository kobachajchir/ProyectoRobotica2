# Pieza nueva, soporte y cubierta

**Final del usuario:** [Soporte_UI_Final_20261025.f3d](Soporte_UI_Final_20261025.f3d), con sus materiales y ediciones conservados. Sólo fondo con rebaje, sin tapas ni postes impresos. Torretas comerciales de 15 mm para OLED y 5 mm entre soporte y auto; PCB directa al soporte con tornillos/tuercas. [Entrega final](../../docs/MODELO_FINAL_WEB_20261025.md). STEP/STL regenerables en exports/2026-10-25-final. La revisión A inferior es histórica; sus generadores no reconstruyen las ediciones manuales finales.

Revisión A del soporte y bisel OLED/encoder creada en Fusion: `Soporte_UI_A.f3d`, generadores Python `fusion_ui_panel.py`, `fusion_ui_complete.py`, `fusion_ui_cover.py` y parámetros `ui-panel-parameters.json`. [Disposición y montaje](../../docs/PANEL_UI_20261025.md).

Base abierta con paredes principales de 3 mm, bisel desmontable con chaflán de 0,6 mm, dos torretas hexagonales de latón M3×15, agujeros Ø3,2 y cabezas Ø5. El OLED usa el socket y dos fijaciones superiores a tuercas cautivas. El conjunto se colocó en la esquina +X/+Z de la placa superior del auto. La ranura CAD mide ~2,78 mm y su adaptación/medida física para M3 queda pendiente; el chasis se conserva.

STEP/STL en `../../exports/oled-encoder`, regenerables. Los F3D son archivos locales; no se afirma una versión guardada en la nube. Sin análisis de interferencias, impresión ni prueba física. Los generadores se ejecutan por el MCP de Fusion con el documento AutoFinalRobo2 abierto; no volver a crear un conjunto que ya existe.
