# AFR2-20261030-02 — Red e IP con scroll en dashboard

Pedido humano: icono Wi-Fi/AP a la izquierda, SSID arriba e IP local del auto abajo; batería fija a la derecha; texto largo desplazable sin invadir iconos.

Se conserva ScreenCode010001 y el snapshot existente. STA conectado usa network_ssid/network_ip; en ausencia de STA, AP activo usa network_ap_ssid/network_ap_ip. En AP muestra la IP a la cual conectarse al auto, no la del cliente. Ambos activos conservan prioridad STA. Datos desconocidos no se reemplazan por una IP inventada.

La cabecera ya recibía ambos campos en01. Esta revisión concreta scroll independiente de ida/vuelta para las dos líneas, solo si exceden70px, con pausa800ms en cada extremo y paso1px/40ms. Texto recortado a columnas21..90, icono a la izquierda y batería/voltaje a la derecha. El redibujo parcial limpia exclusivamente las bandas de texto y conserva iconos, batería, centro y pie; servicio diferencial/cooperativo I2C sin cambios ni otro framebuffer. Sin estado persistente o RAM estática adicional; helper puro con aritmética acotada a SSID32/IP15 caracteres. Margen de memoria real pendiente de build.

ATmega: src/ui/afr2_screens.c implementado en fuente. Web: src/features/oled/firmwareRenderer.ts portado con iguales coordenadas, clipping y tiempos; recursos copiados --copy-only. ESP: sin cambios de fuentes, wire o paquete. Sin compilaciones, tests, validadores, navegador, puertos, flasheos o pruebas físicas. No cambia navegación corto Menú/largo Rápidas ni medición explícita de batería.

Origen de dashboard/iconos: adaptación01 de kobac AutoMicro; algoritmo de desplazamiento bidireccional y actualización parcial de esta cabecera son AFR2. Fuentes registradas en provenance/dashboard-header-20261030-02.json.
