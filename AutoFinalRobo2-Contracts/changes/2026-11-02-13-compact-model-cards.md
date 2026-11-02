# AFR2-20261102-13 — Modelos en tarjetas compactas

02/11/2026. Pedido humano: reducir el alto de los banners de Modelos e implementar
la alternativa de tres tarjetas más cuadradas en una sola fila.

`Web/src/styles/models.css`: la portada pasa a tres columnas desde 840 px,
dos entre 600 y 839 px y una en celular. Render encima del texto, con altura
acotada entre 120 y 180 px y `object-fit: contain` para conservar el modelo
completo. Sin relación de aspecto fija para la tarjeta: el texto puede crecer
con el ancho o el zoom. Acción alineada al pie y etiqueta Render CAD arriba.
Conserva tokens, temas, foco y comportamiento de los enlaces existentes.
El banner de Inicio y las escenas de inspección conservan su composición.

`Web/src/features/models/ModelsPage.tsx`: usa el render 640 de formato 16:10
en las tarjetas; deja de elegir la imagen panorámica 1200 para esta portada.
No se generan ni modifican imágenes. Se actualiza el comentario de composición.

Dirección Confirmada por el usuario; disposición responsive Propuesta, usando
las primitivas/tokens vigentes Observados y previamente autorizados. Sustituye
solo la composición horizontal de la portada registrada en VIS-055.

Estado: **atmega / esp sin cambios; web solo fuente**. Sin tests, compilaciones,
validadores, navegador, generación/promoción de paquete, puertos ni flasheos.
Aspecto, temas y responsive no comprobados en ejecución. Sin commit/publicación.
