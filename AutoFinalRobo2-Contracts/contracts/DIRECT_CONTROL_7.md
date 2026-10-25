# Control Web directo — EX01 v7

AFR2-20261025-01,25/10/2026. Pedido humano explícito: eliminar también el
permiso interno del control Web. Firmware0.9.0, Web0.2.0, ROBO2/perfil2 y
afr2-ws-1. Este documento sustituye las reglas de autorización de EX01v1..v6.
Los formatos de sensores, pose, red, framebuffer y registros de secuencia
conservan las unidades/offsets anteriores salvo lo indicado aquí.

## Acciones directas

Se retiran las tres operaciones control.acquire/control.renew/control.release,
controlLease, los tokens de autorización y la habilitación Web desde la OLED.
El catálogo activo queda en30comandos. El navegador envía directamente movimiento,
modo, prueba de rueda, GOTO, configuración y secuencias cuando existe conexión
y capacidad válida. La configuración sigue exigiendo salidas apagadas y una
consulta reciente; guardar EEPROM accepted no equivale a escritura terminada.

La conexión WS tiene una sola sesión de cliente y conserva hello, correlación,
límites/colas y protección de origen HTTP. Hello no activa motores. Su respuesta
incluye controlMode=direct, requerido por Web0.2.0 para evitar mezclar paquetes
con un ESP anterior. El ESP solo habilita servicios de robot al negociar EX01v7;
una versión anterior no se interpreta como control directo.

## Supervisión automática, sin conceder derechos

- Web envía system.alive cada500ms con clientTs e interactive:boolean. interactive
  es verdadero cuando la página está visible y tiene foco. Esta presencia no
  inicia movimiento, no habilita una bandera ni concede un token.
- ESP registra el último latido interactivo o petición de acción. Estado,
  sensores, capturas y consultas automáticas no renuevan esa presencia.
- ESP retransmite su edad al ATmega cada500ms cuando la UART está disponible.
  0x12 pasa a WEB_PULSE: petición age:u16LE de0..1500ms, respuesta eco de esos
  dos bytes. No tiene operación de adquirir, renovar o liberar.
- ATmega conserva web_ms y web_seen como observación temporal. Una acción
  directa registra actividad. Si una operación Web sigue activa y la última
  presencia alcanza1500ms, apaga salidas y cancela la ejecución; un latido
  posterior nunca la reanuda.
- robot.drive conserva TTL200..1000ms (Web400ms, consignas cada180ms). Pruebas
  de rueda conservan duración100..2000ms. GOTO/secuencias conservan sus watchdogs.
- Desconexión WS programa STOP e invalida operaciones anteriores. STOP sigue
  prioritario y accesible sin preparación. Se preservan watchdog AVR y corte
  por pérdida del transporte. Los tiempos son de diseño, sin medición física.

Origen de actividad (none/local/web) es diagnóstico de una operación, no un
permiso. Una operación local activa puede responder ocupado a una sobrescritura
Web; STOP siempre puede apagar salidas. Acciones físicas de encoder pueden
interrumpir actividad Web antes de iniciar la operación local. No existen
opciones de prioridad/adquisición/liberación ni interruptor de acceso Web.

## Cuerpos UART e identidades

Para limitar el cambio de offsets, los antiguos primeros4bytes de autorización
en DRIVE/WHEEL/MODE/GOTO/CONFIG/PREFS/LINE_CAL y escrituras de secuencia ahora
son reserved:u32LE=0 y se rechazan si no son cero. No representan un token.
SEQ_STATUS conserva sus dos identidades de programa/ejecución, sin ese prefijo.

STATE0x11 conserva16bytes: byte1 es origen de actividad0/1/2; byte4 es reservado
y vale0. JSON robot.state publica origin=none/local/web y elimina lease,
controlLease y remoteEnabled. Estados, validez y edades restantes se conservan.
El endpoint UART Hall0x19 deja de implementarse: medición por vuelta solo OLED.

Las identidades de sesión/transacción UART, programa/ejecución, revisión OLED y
generación de STOP se conservan para correlación y descarte de mensajes viejos.
No conceden autorización. La secuencia se vincula a la sesión de transporte;
al cambiar sesión o desconectarse Web se cancela, sin herencia ni reanudación.

## OLED y biblioteca Web

Se conservan59ScreenCodes,48nodos y11avisos, sus padres y Volver. Se retiran
tres ítems de permiso; Control manual pasa6→4opciones y Sistema8→7. Los IDs de
acción2,3,56 quedan reservados, sin reutilización. Se ajustan offsets del
catálogo y la inserción Reinicio (índice5) tanto en C como en firmwareRenderer.ts.
Tablas, glifos/iconos y recursos se copian del AVR en el build Web. El visor
conectado sigue mostrando el framebuffer original1024bytes, sin datos fabricados.

Hall/calibración, EEPROM ocupada, privacidad de credenciales, revisión de
capturas, límites y memoria aprobada siguen siendo condiciones del dispositivo;
no se convierten en autorización de acceso Web. Los gates de Hall/memoria0 se
conservan. Compilación no certifica funcionalidad física ni paridad de píxeles.
ESP, ATmega y paquete Web deben actualizarse juntos; no se realiza carga en esta tarea.
