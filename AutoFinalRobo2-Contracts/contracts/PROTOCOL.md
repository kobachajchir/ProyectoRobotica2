# Contrato candidato

Estado: diseño sin enlace adaptado end-to-end. [Ficha inicial](../changes/2026-10-23-01-bootstrap.md). No declara compatibilidad AutoMicro.

Framing reutilizable: UNER, N:u8, ':', versión 2, ruta src<<4|dst, comando:u8, N bytes y XOR; total 10+N, N<=255. Enteros multibyte little-endian explícitos; nunca structs/enums/punteros de memoria. Evidencia: referencia F4 uner_core.c/uner_v2.h y manual compartido importado.

Propuesta Mega nodo 1/broker ESP nodo 3; IDs/payloads finales pendientes. Sin rutas Qt/NRF/USB CDC inexistentes. USART1 opera con ESP; UART0 Mega queda para carga. UART0 ESP no mezcla logs con frames.

Web crea requestId. ESP comprueba forma, sesión, permisos/capabilities y encola; aceptación no significa ejecución. UNER v2 carece de secuencia, por eso una transacción correlacionable en vuelo. Mega acepta o rechaza con estado autoritativo; ESP responde con requestId original. Eventos no completan promesas ajenas. Reintentos solo idempotentes.

ACK/NACK heredados 0xE0/0xE1 incluyen comando original/status; adopción/enum pendientes. GOTO distingue aceptación, progreso y fin/cancelación/fallo. Timeout de mando detiene sin esperar al navegador. Resolver respuestas tardías tras timeout mediante cuarentena/resync o versión con secuencia; no confirmar un nuevo pedido con un ACK viejo.

Namespace candidato robot.*, wifi.*, system.* con discriminador de producto AutoFinalRobo2 en handshake. No reutilizar stm.* como implementado. commands.json es diseño, no catálogo activo. Capabilities separan presente/implementado/calibrado/válido/fresco; desconocido no se rellena con cero de sensor. Cinco canales de línea, HC-SR04 con timestamp, cuatro ruedas y encoder UI separado; sin MPU.

Antes de implementar cada comando: ID/versión/propietario, campos y offsets, longitudes, tipos, endian, unidades/rangos, permisos, timeout, respuesta/eventos/idempotencia y errores. Unidades propuestas mm, mm/s, mrad/s, ms, cuentas int32; precisión/saturación por cerrar. STOP/lease y límites son de Mega.

IDs del prototipo OLED son locales (documentation/screens.json), no reporte wire implementado. Réplica Web exige framebuffer o motor/recursos equivalentes con snapshot idéntico; SVG anterior no prueba fidelidad.

Transporte físico documentado en [WIFI_SHIELD](../hardware/WIFI_SHIELD.md): UART0 ESP ↔ USART1 Mega D18/D19 mediante selector E_RX/E_TX aislado de D4/D5 y D0/D1. 115200 8N1 sigue siendo candidato propio, independiente de AT de fábrica/upload. No hay comandos AT en este enlace ni un comando Web que sustituya jumpers/KEY/RST. Identificación de versiones/capabilities posterior al arranque, sin logs intercalados en frames.
