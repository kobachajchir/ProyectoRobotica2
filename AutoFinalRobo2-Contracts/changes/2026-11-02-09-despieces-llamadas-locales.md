# AFR2-20261102-09 — Referencias locales en las hojas de despiece

Pedido humano: modificar solo el despiece para que las flechas no atraviesen el dibujo; numeros junto a las piezas y lista lateral de referencias.

Se reemplazaron exclusivamente las paginas5-8 del [PDF de14 laminas](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/laminas_20261102/AFR2_Laminas_Iniciales_A3.pdf). Las29 referencias se colocan cerca del contorno, con lider corto y punta discreta; laBOM lateral no cambia. Se evitan trayectos sobre la silueta de otras piezas y cruces entre lideres. Renders, geometria, escala visual, rotulos y listado se conservan.

Las otras diez paginas se copiaron directamente delPDF anterior, sin regenerarlas. ElPDF separado de las fichas no se modifico. Fuentes: `acortar_llamadas.py`, `llamadas_cortas.json`, campo opcional `local_balloons` del compositor y persistencia de esas posiciones al preparar el libro.

Solo produccion documental; sin pruebas mecanicas, tests de software, builds o cambios de CAD/firmware/Web/Loader. Las vistas previas corresponden a las cuatro hojas modificadas.
