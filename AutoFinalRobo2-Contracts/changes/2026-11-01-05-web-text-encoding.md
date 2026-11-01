# AFR2-20261101-05 — Simbolos de interfaz mal codificados

El usuario aporta captura de Control tactil: rumbo y ayuda con caracteres
espurios en lugar de grados/separadores. Se localizaron literales corruptos
en las fuentes, incluidos casos con varias capas de recodificacion.

Se reparan exclusivamente las secuencias afectadas y se guardan los archivos
como UTF-8 sin BOM. Grados, separadores, puntos suspensivos, flechas,
desigualdades, guiones y la palabra pestaña recuperan su representacion
correcta. No se aplica una conversion global al archivo ni a datos del usuario.

Archivos adaptados, conservando autorias y cambios locales previos:

- Web/src/features/control/manual/ManualDriveOverlay.tsx: rumbo, ayuda tactil,
  teclado, carga y mensajes de gesto/vista.
- Web/src/features/control/ControlPage.tsx: resumen, enlaces, PWM y ayuda.
- Web/src/features/control/CalibrationPanel.tsx: unidades, rangos y mensajes.
- Web/src/features/oled/EncoderControl.tsx: etiquetas, umbral y estado de enlace.
- Web/src/features/oled/FirmwareLibraryPreview.tsx: etiqueta del encoder local.
- Contracts/README.md y CHANGELOG.md: referencia y registro de entrega.

Adaptacion propia de texto; sin cambios en logica, comandos, datos, gestos,
renderers OLED ni recursos del firmware. ATmega/ESP/CAD/Loader sin cambios.
Busqueda textual de las secuencias afectadas y lectura de los literales
resultantes; sin tests, compilaciones, validadores, navegador ni promocion ESP.
El resultado visual en ejecucion no se da por verificado.
