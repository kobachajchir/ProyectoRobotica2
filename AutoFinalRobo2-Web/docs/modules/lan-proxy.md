# Acceso LAN al servidor de desarrollo

AFR2-20261025-12. El proxy propio `tools/lan-proxy.mjs` permite abrir la Web
desde otro dispositivo de la misma red mientras Vite sigue en localhost.
No necesita paquetes nuevos ni recompilar la aplicación.

Desde `D:/ProyectoRobotica2/AutoFinalRobo2-Web`, mantener `npm run dev` abierto
y ejecutar en otra terminal:

```powershell
npm run proxy:lan
```

Abrir en el teléfono una de las direcciones LAN que imprime, con puerto 5180.
El destino es localhost:5173, con selección IPv4/IPv6; escucha en 0.0.0.0:5180.
Si Vite usa otro puerto, definir `AFR2_DEV_PORT` antes de ejecutar el comando.
`AFR2_LAN_PORT` permite cambiar el puerto de entrada. Ambos deben ser distintos.
Ctrl+C detiene el proxy; Vite sigue funcionando.

Reenvía TCP con backpressure, sin alterar HTTP, binarios, rutas ni upgrades
WebSocket. Incluye recursos GLB/Draco y HMR de Vite. El cliente conserva su
conexión `/ws` al origen de la página: este proxy no emula el ESP ni conecta
por su cuenta con el robot. Sin un servidor del robot detrás, se usa simulación.

El firewall debe permitir Node/puerto elegido desde la LAN y los dispositivos
deben compartir una red sin aislamiento entre clientes. No configura router,
túneles públicos ni reglas globales de firewall. Es una herramienta local de
desarrollo; no forma parte del bundle ni del paquete LittleFS.

Estado: implementación propia, sin tests, compilación, navegador ni comprobación
desde otro dispositivo. El inicio del servicio se registra en la ficha12 de
Contracts; no acredita por sí solo acceso desde el teléfono.
