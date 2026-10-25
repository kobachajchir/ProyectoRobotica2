# Web compilada — AFR2-20261024-13

24/10/2026. npm run build:esp, exit0: recursos OLED, TypeScript/Vite,
minificación JS/CSS, gzip y copia local a ESP/data/dist. Sin errores finales.
Se corrigieron el nombre del estado de STOP y la nulabilidad de su timestamp
detectados por TypeScript en la primera compilación.

| Medida | Valor |
| --- | --- |
| Activos | 7 |
| Raw / gzip | 3021347 / 1476266 bytes |
| LittleFS estimado / presupuesto | 1564672 / 2867200 bytes |
| Margen estimado | 1302528 bytes |

[Log](web-build.txt), [manifest](manifest.json), [resumen](summary.json).
Hashes de los7activos coinciden entre paquete y ESP/data/dist; GLB final
827232bytes conservado. No nuevo activo ni dependencia. Promoción local no es flash.

ESP/AVR sin cambios de firmware: conservan evidencia12,0.8.0/EX01v6 y gates0.
No se recompilaron en13. El script4MiB/FS3MiB sigue siendo candidato, sin
identificar flash física. Sin imagen de FS, upload, CAD o publicación.

Solo compilación/lectura de fuente y artefactos. No tests ni validadores
independientes. No se ejecutó navegador, tacto, mouse drag, foco, safe areas,
orientación/reduced motion, WebGL recovery o movimiento físico. El acceso de
navegador previamente denegado no se repite. Es necesaria revisión autorizada
en teléfono antes de afirmar funcionamiento móvil/físico comprobado.