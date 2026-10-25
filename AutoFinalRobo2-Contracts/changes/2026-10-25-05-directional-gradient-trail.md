# AFR2-20261025-05 — Estela direccional con degradé

25/10/2026. Pedido humano: reemplazar el estilo de flecha por degradé, fade
y animación; usar animate. Aplicación visual sobre manual0.1.9 autorizado.

Se retira el icono dentro del círculo/borde/superficie. La dirección activa
usa tres trazos SVG con degradé transparente→acento propio. La cola pierde
intensidad y la punta conserva color. Un ciclo CSS de800ms desplaza cada
trazo−6..+6px y modula opacity; fases ordenadas cola→medio→punta. Se conserva
el frame al desactivar (animation-play-state:paused) y la estela desvanece
en140ms, sin reiniciar bruscamente la animación. Entrada opacity/scale.96→1,
curva ease-out existente. La elección de dirección/texto sigue inmediata.

Se acerca la estela al modelo con encuadre64%/máximo360px. SVG sin filtros,
sombras, superficies, paquetes gráficos o recursos externos. Gradientes con
IDs únicos del useId existente. Solo transform/opacity cambian durante el
ciclo; no se anima layout ni se renderiza React por frame. Teclado inmediato
y sin ciclo; reduced motion conserva degradé estático y dirección legible.

Dos fuentes adaptadas: ManualDriveOverlay.tsx (SVG), manual-drive.css (estela).
Sin cambios de captura, foco, modelo final, simulación sin conexión04 ni
callbacks/órdenes físicas. No cambia protocolo, firmware0.9.0/EX01v7/build01,
30comandos/59ScreenCodes o gates0. ESP recibe solo activos Web05 locales.
No nuevas dependencias ni activos CAD/raster. Autorías previas conservadas.

Web compilada/promovida. Sin navegador, tests/validadores independientes,
teléfono, flasheos o pruebas físicas. Composición VIS-028 Propuesta y visual
No verificada. La compilación no acredita aceptación del degradé en pantalla.

[Comportamiento](../contracts/WEB_TOUCH_SIMULATION.md),
[build05](../documentation/builds/2026-10-25-05/README.md),
[procedencia](../provenance/directional-gradient-trail-20261025.json).
