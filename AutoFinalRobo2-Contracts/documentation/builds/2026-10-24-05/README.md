# Carga del modelo — AFR2-20261024-05

`npm run build:esp`: TypeScript + Vite + empaquetado + promoción, exit0.
Paquete1.450.338 bytes gzip,7 activos. Firmware y modelo sin modificaciones.

- [Diagnóstico HTTP del Vite existente](http-diagnostic.json): GLB200,
  `model/gltf-binary`,827.232 bytes, firma `676c5446` (`glTF`); Draco y WebP200.
- [Compilación](web-build.log), [manifiesto](manifest.json),
  [tamaños y hashes](summary.json).

No se reprodujo la respuesta HTML original. Se corrigió la reutilización de
cargas rechazadas por useLoader y la configuración de directorio público.
Sin suite de tests, renderizado visual, firmware ejecutado, imagen FS o flasheo.
