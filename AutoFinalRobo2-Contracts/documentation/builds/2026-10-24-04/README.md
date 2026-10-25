# Compilación y paquete final — AFR2-20261024-04

| Comando | Resultado | Medida |
| --- | --- | --- |
| Web `npm run build` | exit0, full con modelo final | 1.450.201 bytes gzip |
| Web `npm run build:esp` | exit0, paquete y promoción ESP | 1.450.201 bytes gzip |
| ESP `python -m platformio run -e esp12s_candidate` | SUCCESS | RAM44.396; programa414.281 bytes |

- [Log full](full-build.log), [log Web ESP](esp-web-build.log),
  [log firmware ESP](esp-firmware-build.log).
- [Manifiesto full](full-manifest.json), [manifiesto ESP](esp-manifest.json),
  [tamaños, hashes e inspección de artefactos](sizes.json),
  [CSS/JS por separado](inline-sizes.json), [símbolos del ELF](elf-layout.txt).

Los7 activos gzip del manifiesto están en Web/artifacts/littlefs/dist y
ESP/data/dist. El GLB descomprimido coincide con el exportado CAD final. La
URL legacy no está en el bundle. No hay sourcemaps publicados.

El linker candidato solicita4MiB de flash y3MiB nominales para LittleFS.
La línea HARDWARE del log proviene de la placa candidata, no de una medición.
Sin tests, navegador, imagen FS, puerto serie, flasheo ni pruebas físicas.
Las revisiones automáticas iniciales de compilación agotaron su plazo sin
ejecutar acciones; el reintento permitió terminar los comandos correctamente.
