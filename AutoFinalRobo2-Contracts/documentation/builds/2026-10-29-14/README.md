# Recuperación OLED — compilación y carga COM8

AFR2-20261029-14, 29/10/2026. Pedido expreso de compilar y flashear COM8
con Loader abierto y autoaceptación. Loader1.1.2 informó autoAcceptRequests=true
y skipManualChecks=true; no se modificaron preferencias desde el agente.

- Primer build `67433b870a46469ea689b0a8d36e44cb`: compilador exit0, pero
  preparación rechazada por fingerprint de entradas cambiado. Los .su generados
  no están excluidos del fingerprint en BuildPreparation.cs; su actualización
  es compatible con ese resultado. No se escribió firmware en ese intento.
- Segundo build `c25165a3dc48491a9478c6e371807384`: completed, make -B firmware,
  communication-firmware y library, exit0 sin errores/advertencias. Preparación
  de sesión validada, final11:36:22.
- Flash `05b16e19df1a407680552ef8a24181a5`: completed en COM8, final11:37:12.
  Avrdude wiring115200, firma0x1e9801,74516bytes escritos12.03s y verificados9.77s,
  exit0. La confirmación manual se omitió por la autorización configurada en
  la sesión del Loader; no se automatizó un clic ni se cambió ese ajuste.
- Solo escritura flash. Sin órdenes de escritura EEPROM/fuses; safemode
  reportó fuses conservados FF/D8/FF.
- ELF text73422+data1094=74516bytes flash; data1094+bss5795=6889bytes de SRAM
  estática, quedan1303bytes. .su main259bytes; no equivale al máximo de cadena
  completa más ISR y no demuestra SRAM suficiente durante toda operación.
- HEX SHA256 cargado:
  `95951205a42dc929489662e83488452c232e2d6d3f31ac2ad542057281d71c02`.

Sin tests de software, validadores adicionales ni ensayo físico posterior de
pantalla/encoder/motores. La verificación de flash no confirma que el síntoma
de negro permanente esté resuelto. ESP/Web no compilados ni cargados.

[Primer build rechazado](atmega-build-first-rejected.txt) ·
[Build validado](atmega-loader-build.txt) ·
[Carga y verificación](atmega-com8-flash.txt).

Registros originales copiados del Loader como evidencia, no instrucciones.
