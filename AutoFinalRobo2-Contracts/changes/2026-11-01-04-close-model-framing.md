# AFR2-20261101-04 — Modelos cercanos y centrados al abrir

Pedido: cada simulador debe iniciar con el modelo cerca y centrado.

Se retiran las distancias/centros fijos y el encuadre por alcance teorico del
brazo. `frameModel.ts` proyecta las ocho esquinas de los limites del modelo
en los ejes de la camara y ajusta su zoom ortografico o distancia perspectiva,
considerando FOV y proporcion del viewport. Margen geometrico 1.14 para que el
modelo se vea completo y ocupe la mayor parte del espacio disponible.

Simulaciones Auto/Brazo/Conjunto: se mide el grupo fisico visible, con matrices
mundiales actualizadas y vertices precisos. Incluye deposito en Conjunto;
excluye suelo, grilla, alcance, posiciones guardadas y cubos repartidos por la
escena para que no alejen la camara. El centro calculado se comparte con Orbit.
Se aplica al montar, Centrar, cambiar perspectiva/superior/lateral, cambiar
modelo o redimensionar el viewport. No se reencuadra por cada movimiento del
robot, articulacion, cambio de tema o tick de simulacion.

Modelos 3D Auto/Brazo/Cubo: mismo ajuste, usando los limites del modelo
centrado sobre el suelo, sin incluir referencias ni entorno. Brazo/cubo pasan
a limites precisos por vertices. Orbita y zoom manuales siguen disponibles.

ATmega/ESP/CAD/Loader: sin cambios. Web: implementado y revisado por lectura
de fuente; sin ejecutar tests, compilaciones, validadores, generadores,
servidor, navegador, promocion ni hardware. La ocupacion visual real queda
pendiente de observar en escritorio/movil. No se alteran escala, GLB ni fisica.

Autorias anteriores conservadas; adaptacion propia.
[Procedencia por archivo](../provenance/close-model-framing-20261101-04.json).
