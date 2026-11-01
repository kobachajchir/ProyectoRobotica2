# AFR2-20261029-28 — Clic corto abre menu desde dashboard

Pedido humano expreso: modificar firmware para entrar al menu con clic corto desde dashboard. Se abre el carrusel010003 existente, conserva seleccion y sigue admitiendo largo al soltar. No se toca el resto de retornos ni el dialogo obligatorio Si/No. Footer010001: Clic: menu.

ATmega: afr2_screens.c y destino del item Home en afr2_menu_catalog.c. Web: firmwareRenderer.ts, FirmwareLibraryPreview.tsx y texto de EncoderControl.tsx, con catalogo/recursos y fuentes revisadas vinculados. Documentacion de navegacion e instrucciones locales actualizadas. Autoria previa conservada; adaptacion propia AFR2. Sin nuevo ScreenCode ni wire, sin cambio adicional de versiones0.11.0/Web0.4.0 aun no desplegadas.

Estado: solo fuente y copia de recursos, sin compilar, ejecutar validadores/tests, abrir puertos ni flashear. Paridad fisica pendiente.
