# AFR2-20261117-03 — Pose válida en la sincronización online

Fecha: 17/11/2026. Origen: registro del Loader aportado por el usuario, con error TS2339 en `CloudSyncController.tsx(157,38)` durante `build:esp`.

Versión: **1.33.1 (subversión mínima)**, compartida por ATmega, ESP y Web según `docs/VERSIONADO.md`.

## Antes/después

El controlador leía `robotState.pose.valid`, pero `RobotPose` no contiene esa propiedad. `parseRobotState` exige `valid === true`, esquema, marco, secuencia, edad y coordenadas admitidos antes de crear una pose; si no se cumplen, entrega `pose: null`.

Dentro de la rama donde existe la pose se emite `valid: true` en la capa JSON de sincronización. Sin pose se mantiene `null`. No se modifica el parser ni se añade una propiedad ficticia al modelo interno; tampoco se cambian las coordenadas o se afirma vigencia temporal adicional.

## Estado por módulo

- Web: corrección en fuente de `src/features/sync/CloudSyncController.tsx`, versión en `package.json` y ambas entradas raíz de `package-lock.json`.
- ATmega/ESP: solo versión compartida en `contracts/afr2_build_info.h`.
- Wire/CMD, pantallas, CAD y Loader: sin cambios por esta ficha.
- El registro del usuario informa que el generador OLED completó y que TypeScript detuvo la preparación Web. No acredita FileSystem completado ni carga física.
- El agente no ejecutó tests, compilaciones, validaciones, generadores ni flasheos. Compilación posterior pendiente del usuario.
- Sin commit ni publicación por esta ficha; cambios previos preservados.
