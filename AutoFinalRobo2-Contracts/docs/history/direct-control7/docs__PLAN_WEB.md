# Plan Web vigente — AFR2-20261024-15

React/Vite/TypeScript, Router hash y componentes/tokens AFR2 compartidos.
Dependencias y lockfiles propios, referencias inertes fuera de imports.
[Plan anterior conservado](history/alignment15/AutoFinalRobo2-Contracts__docs__PLAN_WEB.md).

| Área | Implementación y conexión | Condición pendiente |
| --- | --- | --- |
|Shell/Inicio|Rutas Inicio/Control/Sensores/Wi-Fi/OLED/Modelo/Docs, desarrollador por triple toque, ajustes globales|Aceptación visual/móvil/temas/foco|
|Conexión|/ws mismo host, reconexión/heartbeat,33comandos/capacidades y enlaces separados|Pruebas de protocolo/desconexión; sin dirección/conectar manuales|
|Movimiento|Manual, ocho direcciones táctiles/mouse/teclas, modelo3D final, velocidad y STOP supervisado|Lease/permisos/validez reales; sin movimiento físico ni gestos probados|
|Pruebas/configuración|Prueba de rueda, CONFIG compartido, EEPROM y línea|Calibración15 solicita control al escribir y confirma salidas. Hall manual desde OLED|
|GOTO|Objetivo real0x16/0x17 y simulador separado con puntos/lista/cola local|Robot necesita odometría válida; lista simulada no es misión enviada|
|Secuencias|Editor v2 reversible, pistas/terminales/tiempo/distancia, simulador y envío EX01v6|Memoria/Hall0,CPR/geometría; no ejecución física habilitada|
|Sensores|Línea5, HC-SR04 y Hall con edad/validez|No fabricar valores; hardware/calibración pendientes|
|Wi-Fi|Estado/scan/configuración real y modal por pedido OLED|Borrador no aplica hasta Conectar físico; probar radio/persistencia|
|OLED|Catálogo59 en árbol, biblioteca copiada AVR y framebuffer1024bytes|Paridad visual/diferencial y encoder físico pendientes|
|Sistema|Hardware anidado, reinicio separado, identidades/boot y FS/paquete15|ACK no es reset completado; versión/hash declarados no garantizan integridad de bytes|
|3D y paquete|GLB final827232bytes, Draco local, WebP, JS/CSS minificados,7activos y copia a ESP|Revisión visual; flash/layout candidato sin identificar físicamente|
|Documentación|Guía separada Web/ESP/ATmega, planes/evidencias vigentes|Actualizar al cambiar producto/contrato, sin confundir fuente y pruebas|

OLED comparte ScreenCodes, fuente/XBM y algoritmos de la biblioteca AVR. La
skill local exige entregar ambas partes en cada pantalla. Corto SW confirma,
largo800ms principal, Volver explícito; vista local sin datos/acciones ficticios.
Visor conectado conserva píxeles recibidos y captura nueva entre pasos laterales.

Calibración: lectura del parámetro sin lease; escritura requiere consulta previa,
adquisición de control y salidas apagadas recién consultadas. STOP, cambio de tarea,
foco o enlace cancelan pasos pendientes; AVR conserva todas sus comprobaciones.
Guardar EEPROM accepted se confirma después en estado, no como escritura ya hecha.

Build autorizado: npm run build:esp genera recursos OLED, TypeScript/Vite,
minifica/empaqueta gzip y copia localmente a ESP/data/dist. No genera imagen
FS ni flashea. build produce perfil full; copy:esp exige perfil/linker/presupuesto
compatibles. package:littlefs puede reempaquetar dist, sin servir credenciales.

Identidad: manual0.1.9 autorizado y tokens/primitivas existentes; sin nueva
paleta/dependencia. Sin MPU/balance, sensores simulados ni orientación física
deducida desde consignas. [Matriz vigente](ALINEACION_MAESTRO.md) y
[evidencia15](../documentation/builds/2026-10-24-15/README.md). No nueva brecha
de código identificada en el alcance; interacción/render/pruebas siguen pendientes.