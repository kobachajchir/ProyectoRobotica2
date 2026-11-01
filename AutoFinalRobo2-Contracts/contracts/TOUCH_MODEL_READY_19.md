# AFR2-20261030-19 — habilitar el táctil después de cargar el modelo 3D

Aclaración expresa del usuario: el control táctil sí debe esperar el modelo3D; una vez cargado, debe permitir las maniobras con las mismas condiciones de motores que Mecanum. Se conserva la barra y el componente de progreso existentes. Sustituye la decisión de independencia del modelo descrita en la ficha18-manual-touch-permissions; no cambia las guardas de motores.

## Condición vigente

ControlPage comparte canDrive y driveBlockedReason entre el pad Mecanum y el táctil. El táctil agrega deliberadamente modelLoaded: callback modelReady de su propia vista, fase ready del asset compartido y ausencia de fallo local. En físico: canDrive && modelLoaded; en simulación local también se espera modelLoaded. Abrir, cargar o recuperar el modelo nunca genera una orden de movimiento por sí mismo.

La barra ModelAssetProgress y ManualDriveOverlay se suscriben al mismo store modelAssetLoading para ROBOT_MODEL_URL, usando la misma caché ProgressGLTFLoader/useLoader. No se agrega otro fetch, catálogo, descarga ni porcentaje simulado. La descarga al100% puede estar todavía en fase preparing (decodificación). Incluso con phase=ready del archivo se espera el callback del visor con escena montada y contexto WebGL. La escena se incluye como dependencia de su notificación de disponibilidad para cubrir una recarga del asset.

Durante descarga/preparación se muestran Esperando que termine la carga / Preparando la vista y se bloquean flechas, arrastre y envío físico. Terminado el proceso, si el permiso de motores es válido, se permite un gesto nuevo como en Mecanum. En caché se evita la descarga, pero se sigue esperando a la propia vista. El porcentaje no concede permisos eléctricos ni reemplaza STOP, enlace o habilitación de motores.

## Fallos, recuperación y reintento

Un error del asset o RenderBoundary deja el control sin habilitar y muestra Reintentar modelo3D. El reintento cancela entrada, limpia el estado/caché del modelo y remonta la vista. onReady(false), pérdida del contexto o una fase no lista cancelan el gesto vigente y liberan el movimiento mediante la ruta existente. Las referencias de entrada se deshabilitan antes de esperar el siguiente render al recibir un callback no listo. Recuperar modelo/contexto no repite un gesto previo; se requiere nueva entrada. STOP/cierre/liberación conservan sus rutas, sin esperar que termine la carga.

Se conserva la corrección de18 sobre errores de movimiento históricos: el último error cancela su gesto y se informa; no añade un latch permanente al permiso. Si STOP está pendiente/no confirmado o el firmware no anuncia PWM, sigue bloqueado por ese motivo aunque el modelo esté cargado. Habilitación PWM no exige pose/CPR/HC, y las guardas AVR/ESP existentes no cambian.

## Fuente y pendientes

Web/src/features/control/manual/ManualDriveOverlay.tsx: combina estado compartido de carga con callback local, mensajes de fases/reintento, cancelación y habilitación. ManualDriveModel.tsx: notifica por cambio de escena además del contexto. DocsPage.tsx: ayuda actualizada. ModelAssetProgress y loader de16 se reutilizan sin cambios. Contracts/procedencia/informe19 documentan la aclaración. AVR/ESP/Loader/CAD, wire, EEPROM, ScreenCodes y recursos raster sin cambios.

Solo fuentes, revisión manual por lectura. Sin builds, tests, validadores, navegador, generadores, promoción, conexión física o cargas. Pendiente comprobar descarga lenta,100% con preparación pendiente, caché, error/reintento, contexto perdido/restaurado, cambio de escena, STOP y error transitorio recuperado, input retenido sin autoarranque. No acredita el comportamiento de los paquetes ya compilados o cargados.
