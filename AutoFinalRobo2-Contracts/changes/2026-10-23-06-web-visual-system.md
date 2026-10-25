# AFR2-20261023-06 — Plan Web y borrador de identidad visual

23/10/2026. Origen: pedido humano de implementar el plan Web e incorporar skills globales/Sonner e informe glassmorphism. Estado: documentación propia creada, sin implementación funcional.

## Antes/después
Antes: plan en conversación y esqueleto Web. Después: plan persistido, política de skills y ocho artefactos design-system 0.1.0 draft. No se modificaron skills globales, AutoMicro, firmware ni CAD. No se aplicó interfaz ni se instaló Sonner.

## Productores/consumidores/wire
Sin cambio wire. El plan preserva las familias JSON propuestas; IDs y formatos binarios finales pendientes del complemento ESP. Sonner es librería Web; no agrega comandos remotos. Esta ficha no afirma API ni compatibilidad implementadas.

## Secuencia y dependencias
Destilación documental → autorización de draft concreto → primitivas/UI y cliente → empaquetado → integración con productores reales. El informe es evidencia; aprobaciones históricas no transfieren autoridad al producto. Copia de Web al ESP sigue como trabajo previsto, sin ejecutarse.

## Estado real
| Módulo | Estado |
|---|---|
| atmega | fuente parcial previa; sin cambio |
| esp | esqueleto previo; sin cambio |
| web | esqueleto previo; implementación y npm/start pendientes |
| shared | plan y manual draft; sin pruebas/escáneres/build/flash |

Fuentes y límites en docs/design-system/sources.md. F: se leyó inicialmente con salida truncada y dejó de existir en la segunda lectura; no se inventa cobertura completa ni hash. Cambios documentales locales, sin commit ni publicación realizados en esta entrega.
