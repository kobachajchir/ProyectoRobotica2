# Web compilada — AFR2-20261024-14

24/10/2026. Pipeline npm run build:esp completado: recursos OLED, TypeScript,
Vite, minificación JS/CSS, gzip y copia local a ESP/data/dist. Sin errores finales.
El log conserva la promoción final; el proceso ya no estaba disponible para
recuperar su código de salida al reanudar la conversación.

| Medida | Valor |
| --- | --- |
| Activos | 7 |
| Raw / gzip | 3033373 / 1479434 bytes |
| LittleFS estimado / presupuesto | 1564672 / 2867200 bytes |
| Margen estimado | 1302528 bytes |

[Log](web-build.txt), [manifest](manifest.json), [resumen](summary.json).
Hashes de los siete activos coinciden entre paquete y ESP/data/dist. No nuevo
activo ni dependencia. Promoción local no es flasheo.

Firmware ESP/AVR sin cambios ni nueva compilación: evidencia12,0.8.0/EX01v6,
gates Hall/memoria0. Recursos y algoritmos OLED no cambiaron. Layout4MiB/FS3MiB
candidato, flash física sin identificar. Sin imagen FS, upload, CAD o publicación.

Revisión de fuentes y artefactos; sin tests/validadores independientes. Árbol,
selección/scroll, espaciado y temas no comprobados visualmente. Comparación
diferencial de píxeles y funcionamiento conectado/físico pendientes. No se
repitió el acceso de navegador anteriormente denegado.