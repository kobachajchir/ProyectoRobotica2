# AFR2-20261106-02 — Regeneración y verificación autorizadas

Pedido: «Regenerar y hacer comprobaciones». [Evidencia y métricas](../documentation/builds/2026-11-06-02/README.md).

| Módulo | Fuente / compilación | Carga | Prueba física |
| --- | --- | --- | --- |
| ATmega | Adaptada; producto/comunicación/biblioteca PASS; **SRAM/pila FAIL** | No | No |
| ESP | Firmware PASS, LittleFS candidato generado | No | No |
| Web | Recursos regenerados con guardia, build PASS, 17 activos copiados localmente | No | No |
| Loader | Layout lateral previo conservado; ejecutable aislado compilado | No se reemplazó ni abrió instalado | No |
| CAD | Sin cambios | No aplica | No aplica |

Paridad host: **1680/1680** fotogramas C/TS iguales, 70 nodos y snapshots enumerados. No equivale a aceptar todos los flujos. Se conserva respaldo del generado anterior. Harness reproducible y comprobador de integridad gzip/SHA/tamaños añadidos en tools/verification.

Advertencias de estilo corregidas en QR, dashboard y ordenación Wi-Fi; no cambia comportamiento. **No desplegar AVR**: quedan 556 bytes antes de pila, cadena QR estimada 877. HTTP/WS/táctil requieren aceptación en el auto. Sin credenciales en fuentes, fixtures ni informes.

[Procedencia por archivo](../provenance/regeneration-checks-20261106-02.json).
