# AFR2-20261030-19 — táctil disponible tras cargar el modelo

El usuario aclara que el modelo3D sí es necesario para el táctil. Implementado: esperar al asset ready del mismo store de la barra de carga, al callback de la propia vista y a la ausencia de error. Después se utiliza el permiso de motores compartido con Mecanum. Descargar100% no habilita durante preparación; cargar/caché/reconectar no dispara movimiento sin gesto nuevo.

Se conserva ModelAssetProgress y su caché/bytes/etapas reales, sin otra descarga ni porcentaje ficticio. Se muestran espera de carga, preparación de vista y error con Reintentar modelo3D. No listo/contexto perdido cancela entradas; recuperar exige gesto nuevo. Se conserva el reintento tras error transitorio de movimiento corregido en18.

Adaptaciones: ManualDriveOverlay.tsx, ManualDriveModel.tsx y DocsPage.tsx, con autores anteriores y cambios previos conservados. Fuente Web y Contracts; AVR/ESP/Loader/CAD sin cambios. Sin wire/ScreenCodes/EEPROM o recursos nuevos. Esta aclaración sustituye únicamente la política de modelo de la ficha18-manual-touch-permissions, no la ficha18-motor-selector-encoder-dashboard.

[Contrato y pendientes](../contracts/TOUCH_MODEL_READY_19.md). [Procedencia](../provenance/touch-model-ready-20261030-19.json). Sin pruebas, compilación, validadores, generadores, navegador, promoción ni cargas. Requiere build/despliegue posterior autorizado para incorporarse al paquete del ESP.
