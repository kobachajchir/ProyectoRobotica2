# AFR2-20261031-09 — Logo oficial del robot neón

El usuario entrega un nuevo PNG1448x1086 con transparencia y solicita usarlo
como logo de la app. Se conserva el archivo sin modificar en assets/Logo.png
y Web/reference/brand/Logo-neon-robot-source.png. Referencias anteriores intactas.

El componente compartido BrandMark actualiza navegación, Inicio y Desarrollador.
StartupWelcome usa la versión grande y reserva su relación4:3. El navegador
y el icono para inicio de iOS usan versiones específicas. Nuevos nombres evitan
reutilizar el logo anterior desde caché. La lista de recursos Vite incluye los
cuatro derivados, sin incorporar el maestroPNG al paquete.

El conversor Pillow mantiene proporciones y alpha; no reinterpreta el dibujo.
Se retiran los tres WebP antiguos depublic y se conservan sus fuentes históricas.
Original1448x1086, derivadosWebP1024x768 y256x192; favicon64 y touch180 centrados.

VIS-046 y el manual registran el nuevo origen canónico.
[Procedencia](../provenance/logo-robot-20261031.json).
Estados: web fuente/activos actualizados; atmega/esp/Loader sin cambios.
Sin tests, builds, validadores ni navegador. Los paquetes dist/artifacts/ESP
conservan sus activos previos hasta una compilación/promoción solicitada.
