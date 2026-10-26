# AFR2-20261026-04 - Commit global y publicacion a origin

Pedido humano: commit de todos los cambios pendientes y push a origin desde
el unico Git de D:/ProyectoRobotica2. Rama de trabajo: main; destino configurado:
https://github.com/kobachajchir/ProyectoRobotica2.git.

El conjunto incluye los cambios de sensores/radar de ATmega y ESP, pantallas
OLED vinculadas, interfaz Web con radar recompuesto, referencias interactivas
del modelo y control de visibilidad, textos sin tildes, contratos/evidencias,
fuentes CAD e insumos nuevos de CADS, Documentacion y Placa Kicad.
Las exclusiones vigentes de dependencias, caches y artefactos se conservan.

El modelo `AutoFinalRobo2-CAD/mecanica/auto-completo/AutoFinalRobo2-Final-20261025.blend`
se almacena mediante Git LFS por su tamano de 109.611.651 bytes. La regla queda
versionada en `.gitattributes`; el archivo editable permanece completo en el
checkout y en LFS. No se reescribe el historial anterior. Para recuperarlo en
otra PC se necesita Git LFS instalado y `git lfs pull` tras clonar/actualizar.

Estado del producto: ATmega/ESP conservan las fuentes/evidencias14; la Web
posterior contiene cambios en fuente sin nueva compilacion ni empaquetado;
ESP/data/dist conserva el paquete anterior. El commit/push no certifica pruebas,
paridad visual, montaje fisico ni despliegue. En esta operacion no se ejecutan
tests, compilaciones, validaciones de producto, renders ni flasheos. Los hooks
se omiten; los objetos LFS se transfieren expresamente antes del push Git.
