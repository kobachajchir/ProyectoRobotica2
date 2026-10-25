# Evidencia de compilación y tamaño — AFR2-20261024-03

Compilaciones autorizadas por el usuario; sin tests, navegador, imagen FS o flasheo.

| Comando en Web | Resultado final | Paquete gzip |
| --- | --- | ---: |
| `npm run build` | TypeScript + Vite + postbuild, exit0 | 1.570.980 bytes,7 activos |
| `npm run build:esp` | TypeScript + Vite + paquete + copia ESP, exit0 | 649.410 bytes,4 activos |

- [Log completo](full-build.log) y [log compacto/promoción](esp-build.log).
- [Antes](before-manifest.json), [perfil completo](full-manifest.json),
  [perfil ESP](esp-manifest.json) y [comparación de tamaños](sizes.json).
- `full-esbuild-*` y `esp-esbuild-manifest.json`: mediciones intermedias antes de
  activar Terser; se conservan para distinguir el ahorro del minificador.
- `full-inline-sizes.json`: CSS/JS inline medidos por separado. No sumarlos como
  tamaño exacto del HTML gzip porque el HTML se comprime como un solo archivo.

El primer intento de TypeScript encontró TS2345 en el listener de OrbitControls;
se corrigió antes de las compilaciones finales cuyos logs se conservan aquí.
No se repitieron las compilaciones de firmware de02: este cambio solo modifica
Web, documentación y activos servidos por el firmware ESP existente.
