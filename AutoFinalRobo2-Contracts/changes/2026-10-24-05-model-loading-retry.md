# AFR2-20261024-05 — Recuperación de carga del modelo 3D

El usuario reportó `Could not load /models/AutoFinalRobo2.glb: Unexpected token
'<', "<!doctype" ... is not valid JSON`. Ese error indica que GLTFLoader recibió
HTML en vez del GLB. No acredita que el archivo CAD esté dañado.

## Diagnóstico y corrección

El archivo fuente contiene la cabecera `glTF`, versión2 y longitud827.232 bytes,
igual a su longitud declarada. Al consultar el Vite existente en localhost
IPv6:5173, la ruta ya devolvía200 `model/gltf-binary` y el GLB completo. Draco
WASM, su wrapper y WebP también devolvían200 con contenido correcto. Por tanto,
la respuesta HTML original no se reprodujo durante este diagnóstico; no se
afirma haber identificado cuándo o por qué se produjo esa respuesta anterior.

Se encontró un fallo concreto de recuperación: `useLoader` conserva las
promesas rechazadas. El botón Reintentar reiniciaba el ErrorBoundary y el
Canvas, pero reutilizaba el mismo error de carga almacenado.

- `Model3DPage.tsx`: URL compartida para carga y recuperación;
  `useLoader.clear(GLTFLoader, MODEL_URL)` antes de reconstruir el visor.
  Un intento posterior vuelve a solicitar el archivo. Si el error contiene
  la respuesta HTML reportada, se muestra una explicación y la acción disponible.
- `vite.config.ts`: `publicDir` absoluto anclado al archivo de configuración,
  habilitado de forma estable en desarrollo. `build.copyPublicDir=false`
  conserva la selección explícita de activos para el paquete, sin copiar
  maestros ni referencias históricas. Los builds siguen publicando siete activos.
- No se cambia el GLB, Draco, geometría, layout LittleFS ni código de firmware.

## Estado real

| Módulo | Estado de esta ficha |
| --- | --- |
| atmega | Sin cambios; conserva evidencia02 |
| esp | Firmware sin cambios; paquete Web corregido promovido a data/dist |
| web | TypeScript/Vite/paquete/promoción completados con código0; diagnóstico HTTP local registrado |
| CAD | Sin cambios; GLB final exacto conservado |

Paquete: **1.450.338 bytes gzip**,7 activos; estimación con reserva:
**1.540.096 bytes**. Se conserva el presupuesto2800KiB y layout candidato4m3m
solicitado en04. Model SHA-256 sin cambios; manifiestos Web y ESP coinciden.

Se realizaron lecturas HTTP al servidor local ya existente para diagnosticar
el fallo reportado. No se lanzó una suite de tests, validador independiente,
navegador, imagen FS, monitor serie, flasheo ni prueba física. La compilación y
el contenido HTTP no equivalen a una comprobación visual del modelo renderizado.

Para una pestaña que conserve el error anterior: recargar una vez para recibir
el código actualizado o usar Reintentar carga3D con la corrección cargada.

[Evidencia](../documentation/builds/2026-10-24-05/README.md) ·
[Procedencia por archivo](../provenance/model-loading-retry-20261024.json).
