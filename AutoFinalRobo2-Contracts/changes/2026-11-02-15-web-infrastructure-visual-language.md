# AFR2-20261102-15 — Lenguaje visual de Infraestructura en toda la Web

02/11/2026. Pedido explícito de aplicar solamente la parte visual a toda la interfaz siguiendo Infraestructura y el manual ampliado. Autorización de aplicación del manual **0.1.11 draft**; skill visual-identity-system, modo APLICAR. Dirección Confirmada; composición extendida Propuesta; ejecución No verificada.

## Antes / después

Se añade una capa compartida `styles/visual-language.css`, importada desde index.css, para llevar la composición y los materiales de Infraestructura al resto de las rutas. Las cabeceras PageHeader incorporan base azul noche, grilla, halos estáticos, icono contextual, título display y selector legible sobre el banner en ambos temas. Inicio conserva su render con título degradado; Modelos conserva sus tarjetas compactas y renders completos.

Las tarjetas, superficies, encabezados secundarios, iconos, campos, botones, enlaces, navegación, modales y notificaciones comparten radios, bordes, espaciado y profundidad. Sensores/Conexión reciben familia info, Modelos/Simulación success y Puesta a punto warning como contexto visual; las acciones siguen usando action y las emergencias error. El área se identifica con un atributo de presentación del main derivado de la ruta existente.

Se estilizan además los paneles de simulación, lecturas, calibración, tarjetas de Protocol Studio, estados vacíos y desplegables de contenido. No hay nuevos controles, cambios de orden, listeners, condiciones de permiso, comandos, datos, valores de formulario, rutas ni gestos. No se modifican escenas 3D, modelos, cámaras, raster OLED ni firmware.

Hover fino eleva tarjetas accionables 3 px, desplaza flechas y añade profundidad. Los botones primarios usan los tokens hover/active existentes; emergencia/danger quedan fuera de estas reglas. Se conserva SegmentedControl con copia recortada. La extensión no añade loops decorativos ni esperas editoriales a operación. Reduced motion retira las nuevas elevaciones; contraste/transparencia reducidos simplifican el banner.

## Adaptación por archivo

- Web/src/styles/tokens.css: tokens compartidos de profundidad, espacio, grilla y halos.
- Web/src/styles/visual-language.css: capa visual común y adaptaciones de superficies específicas, responsive y preferencias.
- Web/src/styles/index.css: import global.
- Web/src/components/ui/PageHeader.tsx: icono y halo decorativos, mismo contenido y acciones.
- Web/src/components/ui/Button.tsx: atributos de variante/tamaño para estilos de estado; sin cambio de handlers.
- Web/src/components/ui/Field.tsx: clase de presentación común.
- Web/src/components/navigation/RouteNav.tsx: atributo de área visual del main; navegación conservada.
- Contracts: ficha, decisión VIS-062, alcance y cobertura de fuente.

## Productores / consumidores / wire

Sin cambios de protocolo, comandos, IDs o formato. Consumidores: presentación Web y componentes compartidos. No se recompila ni promueve el paquete del ESP.

## Cobertura y límites

Inicio, Conducir, Programas, Puesta a punto, Sensores, Modelos e inspección, simuladores, Conexión, Ayuda/OLED y Desarrollador/Protocol Studio reciben reglas compartidas o específicas. La cobertura describe alcance de selectores/componentes, no acredita render de cada estado. El raster OLED y los canvas conservan representación propia. El CSS específico cargado por rutas conserva su layout; la capa usa selectores de mayor especificidad para sus propiedades visuales comunes.

| Módulo | Estado |
| --- | --- |
| atmega / esp | Sin cambios de esta entrega. |
| web | Fuente visual modificada; sin compilar ni comprobar en navegador. |
| shared / Contracts | Registro de autorización, decisión y cobertura documental. |
| CAD / Loader | Sin intervención. |

Sin tests, compilaciones, validadores, escáneres, navegador, render, consultas de placa, promoción ni flasheo. Se preservan los cambios concurrentes previos. Sin commit ni publicación solicitados. El aspecto final, desbordes, contraste y convivencia de estilos quedan No verificados.
