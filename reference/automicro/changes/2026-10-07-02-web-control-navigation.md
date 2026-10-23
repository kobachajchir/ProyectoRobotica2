# AMI-20261007-02 — Control Web: sesión conservada y navegación coherente

- Fecha: 2026-10-07 (America/Buenos_Aires).
- Origen: Web ESP01; rama work/nueva-etapa; commit local5d6754d.
- Estado: implementado en fuente; sin tests, builds ni validaciones.
- Área: navegación UI y flujo de preparación de comandos existentes.
- Relacionado: AMI-20261007-01.

## Contrato anterior y nuevo

El acceso de Control a /control/calibracion usaba window.location.assign. La recarga recreaba la aplicación, UserProvider y su sesión en memoria. La navegación nueva permanece dentro de React Router y conserva los proveedores; no se cambia la autorización, no se evita el PIN ni se persisten tokens.

Rutas existentes /control y /control/calibracion. Nuevos parámetros de presentación: /control?modo=free|goto; /control/calibracion?volver=free|goto. Son estado de interfaz, no modo físico F4 ni permiso. El modo físico sigue cambiando solo mediante el comando existente confirmado por F4.

Selector Libre/GOTO/Calibración adopta el patrón de OLED/Protocol Studio. Preparación arriba; acciones/estado junto al control; GOTO presenta destino y seguimiento; calibración agrupa configuración, preparación y resultado; herramientas de simulación plegadas. Las condiciones incumplidas conservan resolución mediante tarjeta botón y cruz roja; cumplidas check verde informativo.

Borradores sessionStorage: automicro.goto.draft.v1 (x,y); automicro.motor-calibration.draft.v1 (percent,ramp,timeout,notes,curve,levels,repeats,trial). No contienen credenciales ni sustituyen el perfil confirmado de F4. Regreso a la tarea conserva parámetros. La serie permite reiniciar explícitamente.

## Flujo completo

1. Usuario pulsa Calibración; navegación SPA a la ruta protegida, con UserProvider/WebSocket actuales.
2. Selector y regreso usan Router; coordenadas y configuración se conservan por sesión del navegador.
3. Modo se elige en modal con fondo bloqueado y se aplica mediante stm.carMode.set existente; cliente espera confirmación. Los parámetros de navegación no cambian modo.
4. Los modales de requisitos Balance/Objetos incluyen acciones existentes para activar/desactivar/consultar sin quedar bloqueados por el modal.
5. GOTO/calibración conservan comandos 0x23..0x2B, longitudes, status/reason, sesión STM, timeouts y política sin reintento automático de movimientos. No se modificó UserContext.
6. Cambios de layout, foco, teclado y feedback siguen tokens del manual0.3.0 autorizado, Apple Design y Emil Design Engineering. Las páginas Studios se inspeccionaron como referencia para el selector; no se modificaron.

## Impacto en repositorios

| Repositorio | Estado | Acción o motivo |
| --- | --- | --- |
| FirmwareF4 | no_aplica | Sin cambios de código/wire; guarda y ejecuta las operaciones existentes |
| Firmware ESP01 | no_aplica | Sin cambios de código en esta tarea; falta regenerar/copiar el bundle Web |
| Web ESP01 | origen |5d6754d: navegación, selector compartido, toolbar, layout GOTO/calibración y borradores |
| AutoMicroQtDesktop | no_aplica | Pedido y reforma limitados a Web |

## Compatibilidad y orden de integración

No hay IDs, payloads ni permisos nuevos. Solo cambia el flujo local de navegación y resolución de requisitos. La Web reformada sigue requiriendo F4/ESP con AMI-20261007-01. El build/gzip del pedido anterior no incluye este commit; regenerar y copiar cuando el usuario solicite compilar.

## Evidencia y límites

Fuente del problema: ControlSection llamaba window.location.assign y UserContext inicializa user=null en memoria; ProtectedRoute exige user. Se eliminó la recarga mediante navegación interna. No se ejecutaron tests, compilaciones, escáneres ni verificación visual/real de sesión. La cobertura responsive/temas/estados es implementación CSS/React, no una sesión renderizada comprobada.

Informe visual y matriz Before/After en Web docs/design-system/control-navigation-2026-10-07.md. Propuestas de layout aplicadas; estética y comportamiento con dispositivo pendientes. No se tocaron firmware, Qt, dependencias ni artefactos anteriores.

## Actualizaciones posteriores

- 2026-10-07: fuente Web y commit local; central publicada según autorización del skill de integridad.

