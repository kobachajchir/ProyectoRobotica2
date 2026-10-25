# AFR2-20261025-08 — Difusión por brazo y punta aguda

25/10/2026. Pedido humano: color más suave, degradé a lo largo de las barras
y de su centro hacia bordes, punta más aguda y corregir el fade en ángulo raro.

Se sustituye la difusión global de07 por dos campos por brazo: gradiente
normal simétrico con opacidad0→0,58→0 y máscara longitudinal0→0,95 que sigue
su eje. Los campos inferior/superior son reflejados e iguales en la unión;
la difusión no atraviesa todo el dibujo con una franja horizontal. Transparencia
revela el fondo real en ambos temas, sin copiar un color ni nueva paleta.

Geometría: V trazada por(8,32)→(120,120)→(8,208), stroke88 y unión miter.
Ángulo central aproximado76° por coordenadas. Se elimina estiramiento vertical:
viewBox512×240, misma proporción CSS y preservación de aspecto SVG. Esto evita
aplastar la punta. Gradientes transversales calculados con la normal real del
brazo(112,88); al rotar y animar se desplaza el campo completo con cada cinta.

Tres referencias a la misma V, offsets0/150/300 y fases explícitas data-step.
Componente ManualDirectionTrail memoizado y solo visual, con IDs de useId.
Longitud64% del menor lado local, máximo640px; altura proporcional240/512.
La estela se retrasa22% sobre su eje para nacer bajo el chasis y mantener
la punta cerca del49% del lado local, sin ampliar el modelo ni cambiar cámara52%.
Capas06, fade220ms, flujo1200ms/±12unidadesSVG, pausa, teclado/reduced motion
estáticos y respuesta de gesto inmediata conservados. Sin blur o filtros.

Tres fuentes adaptadas: nuevo ManualDirectionTrail.tsx, integración en
ManualDriveOverlay.tsx y manual-drive.css. Captura/foco, modo local04 y
callbacks físicos no cambian. Firmware0.9.0/EX01v7/build01,30comandos,
59ScreenCodes y gates0 conservados; ESP recibe solo activos Web08 locales.
Sin nuevas dependencias, recursos externos, modificación de GLB/CAD o pruebas
físicas. Autorías previas conservadas; SVG/CSS y geometría propios.

animate/manual0.1.9 autorizado, VIS-031: dirección Confirmada, composición
Propuesta, visual No verificada. Web compilada/promovida; sin navegador/teléfono,
tests/validadores independientes o flasheos. Ángulos/campos seguidos en fuente,
no medidos en un render aceptado.
[Contrato](../contracts/WEB_TOUCH_SIMULATION.md),
[build08](../documentation/builds/2026-10-25-08/README.md),
[procedencia](../provenance/soft-pointed-ribbons-20261025.json).
