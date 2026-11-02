# AFR2-20261102-15 — Imagen completa de fondo en tarjetas de Modelos

02/11/2026. Pedido humano: conservar las tres tarjetas compactas, quitar las
barras del ajuste de imagen y usar todo el div para el render, con texto
superpuesto y sombra/degradado suave en lugar de un bloque sólido. Quitar el
texto «También: cubo auxiliar de 50 mm».

Web/src/styles/models.css: render absoluto a sangre con `object-fit: cover`,
tarjeta de proporción cuadrada con mínimo de 320 px y crecimiento por contenido;
texto abajo sobre degradado transparente y sombra tipográfica con los tokens
banner existentes. La imagen llena el espacio recortando sus bordes cuando
la proporción lo requiere. Conserva tres/dos/una columnas y enlaces existentes.

Web/src/features/models/ModelsPage.tsx: quita «También» y el enlace al cubo del
pie; conserva los accesos al simulador y a Infraestructura. El modelo auxiliar
continúa disponible en sus rutas y simulación.

Dirección Confirmada; disposición Propuesta. atmega/esp sin cambios; web solo
fuente. Sin tests, compilación, validadores, navegador, nuevos renders,
promoción ni flasheos. Aspecto y responsive no comprobados en ejecución.
