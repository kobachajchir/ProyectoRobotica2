# Validación y cierre documental del manual 0.1.3

Fecha: 23/10/2026. Pedidos humanos: «Valida el manual, no empieces a implementar, deja todo listo para hacerlo», «`informe-portabilidad-glassmorphism.md` está en `D:\ProyectoRobotica2` ahora. Continua con cerrar el plan visual» e incorporación del activo propio `D:/ProyectoRobotica2/assets/Logo.png` con acento carmesí/rubí sobre vidrio neutro. Alcance autorizado: cotejo íntegro con el informe completo, adopción de marca/paleta roja y comprobaciones sobre el manual. Sin instalación, servidor, build, tests del producto, escáner de Web ni hardware.

## Resultado

Plan visual cerrado documentalmente en versión `0.1.3` (`694` comprobaciones sobre `107` tokens, `0` errores). Resultado reproducible en [validation.json](validation.json); ejecutar desde Contracts: `python docs/design-system/validate-manual.py`. El script solo lee el manual, el informe de origen y el logo, y escribe este informe JSON; no importa ni ejecuta fuentes del producto. Conserva hashes SHA256 de los ocho artefactos, de `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` (`564074c818e14720e62129f70ddf6a96376ebe3a16e993e1096ffdb148a03db0`) y de `D:/ProyectoRobotica2/assets/Logo.png` (`471bcbb75672aa8ec30a32cb74c624975a49b75deda3a531043fce52b3ba3702`).

## Correcciones y reconciliación (`0.1.0` → `0.1.3`)

| Before | After | Why |
|---|---|---|
| Éxito claro `#0f8c53`: mínimo calculado `4,18:1` | `#087a46`, comprobado con el conjunto declarado (`VIS-009`) | Superar objetivo de texto normal `4,5:1` |
| Borde fuerte `.28`/`.20` del informe insuficiente frente a superficies glass | `borderStrong` `rgba(19,32,51,.55)` claro y `rgba(255,255,255,.50)` oscuro (`VIS-009`) | Alcanzar `≥ 3:1` en bordes funcionales de controles |
| Sombras, borde fuerte, hover, foco y geometría solo en prosa | Tokens explícitos y decisiones `VIS-009/010/011` | Evitar decisiones aisladas al implementar |
| `Enter/Space` genéricos en botón/enlace | `Enter` en enlaces; `Enter/Space` en botones (`VIS-011`) | Respetar semántica y scroll nativos |
| Shell Sonner custom con cierre ambiguo e inferior derecho | `Cerrar` explícito con `toast.dismiss(id)`, posición superior (`top-right`/`top-center`), id estable y tema resuelto (`VIS-010`) | Headless requiere contenido accesible propio; no tapar `STOP` |
| Secciones §§6–15 de `F:` ausentes por lectura truncada; `pressMs: 100`, `easeOut: cubic-bezier(.16,1,.3,1)`, `reducedOpacityMs: 80` | Cotejo íntegro con `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` (§§1–18): `pressMs` `150ms`, `motionFast/Standard/Slow` `150/200/300ms`, curvas exactas del informe (`easeOut/easeEnter` `cubic-bezier(.23,1,.32,1)`, `easeStandard` `(.2,.75,.25,1)`, `easeInOut` `(.77,0,.175,1)`, `easeDrawer` `(.32,.72,0,1)`), tiempos de modal `250ms`, drawer `240ms`, segmentado `250ms`, acordeón `200ms`, tooltip `150/280ms`, spinner `800ms`, fade reducido `120ms`, escala `z-index` `0..110`, `surfaceSubtle` y 3 alcances de carga (`VIS-012`) | Cerrar la trazabilidad con el informe completo sin arrastrar el reset `.01ms !important` contradictorio ni entradas editoriales largas |
| Sin logo propio; acción primaria y foco en tinta neutra `#132033` / `#ecf2fb` | Adopción de `D:/ProyectoRobotica2/assets/Logo.png` (`BrandMark` `36×36 px` / `64×64 px` sobre medallón `#000000` + fallback `AFR2`) y acento carmesí/rubí (`action` `#b81d33` / `#ff6b7d`, hover `#961426` / `#ff8594`, `focus` `#b81d33` / `#ff6b7d`, `selection` carmesí translúcido), diferenciando `STOP`/`error` con rojo bermellón (`#c93b1b` / `#ff8a65`), fondo `surfaceSolid`, borde e icono explícito (`VIS-013`) | Dotar de identidad gráfica y cromática roja propia a la app sin teñir el vidrio neutro ni confundir acciones primarias con `STOP` |

## Comprobaciones

JSON parseable; informe completo, `Logo.png` y ocho artefactos presentes; versión `0.1.3` uniforme; metadatos por token (`107` tokens); decisiones `VIS-001`..`VIS-013` aprobadas a nivel documental; enlaces locales resueltos; `scanPaths` vacío; valores de material; cálculo de contraste de texto/ayuda/estados sobre base de tema, sólido y seis materiales; acciones normal/hover (`6,43:1` y `8,71:1` en claro; `6,12:1` y `7,23:1` en oscuro); borde fuerte/foco (`≥ 6,07:1` en claro; `≥ 5,39:1` en oscuro) frente a esas superficies (`128` pares de contraste verificados); diferenciación cromática verificada entre `action` y `error`/`STOP`. HEX canónico con OKLCH derivado para colores opacos.

Objetivos propios: `4,5:1` para texto de los pares definidos; `3:1` para bordes funcionales y foco. Texto sin imagen/GLB bajo el vidrio, de acuerdo con [manual.md](manual.md). El borde sutil es decorativo, no identifica por sí solo un campo. Imágenes, gradientes, antialiasing, cambios de fondo, desempeño de blur, foco real, teclado y móvil deben comprobarse cuando exista implementación y pedido de verificación.

## Límites y estado de entrega

- El informe original en `D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md` (§§1–18) y el activo `D:/ProyectoRobotica2/assets/Logo.png` fueron leídos, cotejados y registrados con sus hashes SHA256.
- El plan visual y las decisiones `VIS-001`..`VIS-013` quedan cerrados como línea base `0.1.3` para implementar. El campo técnico `status` permanece `draft` y `verification` en `No verificada` hasta renderizar y comprobar la interfaz en ejecución.
- Sonner está especificado; no instalado ni integrado. Web/ESP/ATmega conservan sus estados previos y no se cambia wire.
- Sin capturas de la app, render ni prueba física en esta entrega.

## Cinco reglas protectoras de la entrega

1. Vidrio en fondos (`chrome/panel/dense/field/popover/dialog`), texto nítido y fondos admitidos explícitos.
2. Superficies glass neutras en ambos temas con acento carmesí/rubí propio y `BrandMark` (`Logo.png`), diferenciando `STOP`/`error` con bermellón, borde e icono explícito.
3. Datos y resultados autoritativos, sin simulación ni éxito optimista.
4. Interacción cancelable, accesible, con curvas del informe y movimiento reducido de `120ms`.
5. Encoder OLED con giro y SW único: corto confirma, largo `800ms` abre menú, atrás solo por `Volver`.
