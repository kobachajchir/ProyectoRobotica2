# AFR2-20261107-52 — Hall tolerante a la holgura, prueba de encoder, árbol con zoom y ajustes de la Web

07/11/2026.

Estado real: atmega = fuente editada, sin compilar ni cargar; web = fuente editada y revisada en el servidor de desarrollo (Vite), sin build ni empaquetado; esp = sin cambios.

## Medición Hall de una vuelta (ATmega)

Pedido del usuario: la vuelta da unos 370 pulsos y aparecía «Medición inválida».

La cuenta quedaba invalidada si retrocedía **un solo pulso**. Al girar la rueda a mano, la holgura de la caja hace retroceder el eje del motor en cada reacomodo de la mano. Ahora se guarda el máximo alcanzado (`hall_peak`) y solo se invalida si la cuenta retrocede respecto de ese máximo más de `max(8 pulsos, máximo/32)`, unos 11 pulsos para 370. Una vuelta hecha hacia el lado contrario o un canal B errático siguen invalidando. `src/services/afr2_product.c`, `include/afr2_product.h`.

## Prueba de encoder (ATmega y simulador Web)

- «Clicks: N» cuenta las pulsaciones cortas.
- «Giro: N» cuenta el giro desde 0, hacia arriba o hacia abajo, con un límite de ±9999.
- El largo muestra «Pulse»; después de un largo muestra «Correcto» y la indicación «Salir»; un segundo largo sale.
- La rotación ya no selecciona Volver.
- Archivos: `afr2_screens.c/.h`, más `firmwareRenderer.ts`, `uiSimulation.ts` y `uiSimulationFlow.ts` en la Web.

## Ícono de pulsación larga

Las agujas del reloj estaban corridas abajo a la izquierda del centro: el ángulo quedaba en (5,8) y no en (6,6), con un borde más cerca del aro. Se redibujaron desde el centro exacto, 3 px cada una, con 1 px de borde parejo. Los bytes son idénticos en `afr2_icons.c` y en `encoderLongIcon.ts`. La biblioteca OLED se regeneró y los hashes de `afr2_icons.c`, `afr2_screens.c` y `afr2_screens.h` se actualizaron en `firmwareRenderer.sources.json`.

## Web

- **Árbol de pantallas:**
  - Zoom del 30 % al 160 %, con los botones −/+, el porcentaje (vuelve a 100 %) y «Ajustar».
  - Ctrl + rueda o pellizco del touchpad, y pellizco con dos dedos.
  - El zoom mantiene fijo el punto bajo el cursor.
  - Las páginas que se alternan con el giro del encoder quedan bajo el mismo padre del catálogo: Radar con Distancia numérica bajo «Distancia y servo», y Línea IR1..IR5 con IR valores bajo «Sensores». Desde otras ramas aparecen como referencia.
- **Infraestructura:**
  - Más espacio en los carriles del diagrama, entre los nodos de origen, en la línea de etapas, en los datos y en el glosario.
  - La tarjeta «El paquete que se sirve» separa sus bloques con un gap de 20 px, así que los avisos de consulta (cargando, sin reporte, LittleFS leído del ESP) ya no quedan pegados al bloque siguiente.
- **Modelos y simulación:** en tarjetas angostas, la foto quedaba debajo del título y del texto. Ahora la foto va arriba en su propio bloque (16:10), con un fundido corto, y el texto debajo.
- **Loader:**
  - Captura de la pantalla inicial del Loader 2.0.0, tomada con el programa recién cerrado y vuelto a abrir.
  - Está en `public/brand/loader-inicio-1206.webp` (79 KiB) y en `public/brand/loader-inicio-720.webp` (36 KiB).
  - Va en el banner, debajo de las píldoras «Explorar · AFR2 Loader» y encima del título.
  - Suma 2 archivos al paquete del ESP: quedan 19 de los 24 que admite.

## Pendiente

- Compilar y cargar el ATmega para probar la medición Hall y la prueba de encoder en el auto.
- Build de la Web y empaquetado para el ESP.
