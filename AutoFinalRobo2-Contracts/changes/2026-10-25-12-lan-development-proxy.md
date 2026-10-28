# AFR2-20261025-12 — Proxy de desarrollo para LAN

25/10/2026. Pedido humano: acceder al localhost de la Web desde otro dispositivo
de la LAN. Se agrega un proxy TCP propio Node, sin dependencias adicionales,
en Web/tools/lan-proxy.mjs y el comando npm run proxy:lan.

Entrada 0.0.0.0:5180 → localhost:5173, con selección IPv4/IPv6, streaming con
backpressure y paso transparente de HTTP/WebSocket/HMR y archivos del modelo.
Imprime las IPv4 locales no loopback/autoconfiguradas. Puertos configurables
por AFR2_LAN_PORT/AFR2_DEV_PORT; Ctrl+C cierra las conexiones del proxy.
No reinicia el Vite existente ni cambia protocolos, contratos de comandos,
ScreenCodes o el destino del WebSocket del producto. No es un emulador ESP.

| Módulo | Estado real |
| --- | --- |
| Web | Herramienta de desarrollo y documentación añadidas; bundle11 conservado |
| ESP | Firmware y activos11 sin cambios; sin imagen FS ni flasheo |
| ATmega | Firmware/build01 y gates conservados; sin cambios |
| CAD | Sin cambios |

Servicio iniciado por pedido humano: listener observado en 0.0.0.0:5180,
PID34456, destino Vite localhost:5173. El5174 estaba ocupado por otra aplicación
y se conserva. stdout anuncia la dirección Wi-Fi de esta sesión; stderr vacío.
Las reglas existentes de Windows permiten entrada a Node; no se modificaron.
Acceso desde teléfono y gesto táctil siguen sin comprobar. No tests,
compilaciones, validadores, navegador o pruebas físicas.
No túnel público, cambios del router ni credenciales guardadas.

Autoría/adaptación: herramienta nueva AFR2 con APIs Node integradas. package.json
añade únicamente el comando de arranque; README/informe/módulo describen su uso.
