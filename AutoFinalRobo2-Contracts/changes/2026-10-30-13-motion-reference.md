# AFR2-20261030-13 — adelante/atrás y giro CW/CCW

Pedido: invertir adelante/atrás; el usuario confirma giro también invertido y pide Girar CW / Girar CCW. Se conserva lateral puro. Implementación de fuente ATmega/Web, ESP sin cambios (reenvía comandos existentes), Contracts actualizado. Loader/CAD sin cambios. EX01v11/EEPROMv8 sin modificaciones. Sin ejecutar generadores, compilaciones, tests, validadores, navegador, promoción, COM ni cargas.

- afr2_product.c: referencia fija aplicada en Drive y DrivePwm; odometría X/yaw como transformación inversa. Conserva realimentación PID y datos individuales de ruedas.
- afr2_motion_reference.h y motion_reference.json/control/motionReference.ts: misma política longitudinal=-1,yaw=-1,lateral=1, independiente de trims EEPROM.
- mecanumKinematics.ts: directa/inversa coordinadas; alcanza diagonales, curvas, ramps, pivots, programas por tiempo/distancia y simulación. Línea/GOTO reales consumen Drive y odometría corregidos.
- gotoSimulation.ts y ManualDriveModel.tsx: fórmulas de ruedas/offsets independientes adaptadas también.
- afr2_menu_catalog.c, firmwareRenderer.ts, MecanumDrivePad.tsx, SequenceEditorPanel.tsx, ChassisSpinGraphic.tsx: Girar CW horario y Girar CCW antihorario vistos desde arriba. ScreenCode030009 y argumentos8/9 conservados. Datos/recursos generados no ejecutados por esta ficha.
- sequenceStorage.ts: reconoce solo etiquetas automáticas antiguas exactas de pivots al importar; preserva nombres personalizados, valores y pistas de ruedas. Default/demo cambian sus nombres en mecanumKinematics.ts.
- DocsPage, contratos, pantallas/conexiones e informes: impacto y pendientes registrados. El simulador de recolección hereda funciones comunes, sin editar su lógica.

La corrección se realiza en la referencia chasis↔ruedas. Las direcciones de la interfaz y los mensajes de alto nivel conservan su significado y no se invierten otra vez en el ESP. Las pistas/pruebas de rueda crudas mantienen sus valores; los programas ya calculados no se reinterpretan ni convierten silenciosamente. No hay evidencia física nueva ni afirmación de causa eléctrica.

[Contrato, ecuaciones e impacto detallado](../contracts/MOTION_REFERENCE_13.md). [Procedencia por archivo](../provenance/motion-reference-20261030-13.json). Revisión realizada por lectura y diff, sin ejecución. Regeneración/build, despliegue conjunto AVR/Web y pruebas de direcciones/odometría quedan pendientes de pedido expreso.
