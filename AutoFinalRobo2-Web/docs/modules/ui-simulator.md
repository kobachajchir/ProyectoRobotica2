# Simulador UI - Desarrollador

## Presentacion AFR2-20261031-12

El simulador aparece primero en Desarrollador. Pantalla y encoder comparten
un area de trabajo; la guia muestra pantalla, seleccion, gesto y requisitos.
La opcion seleccionada se mantiene visible dentro de la lista, sin desplazar
la pagina. Equipo de ejemplo y Reiniciar estan en la barra superior.

Catalogo y alcance se despliegan bajo el simulador. La busqueda filtra por
nombre o codigo; el selector abre la pantalla elegida y contempla listas sin
resultados. Las pantallas informativas sin opciones conservan una guia.
En movil los tres controles se mantienen juntos y la guia queda debajo.

| Before | After | Why |
|---|---|---|
| Perfil antes del simulador | Herramienta primero, perfil despues | Acceder antes a la tarea |
| Pantalla, controles y textos con jerarquia similar | Area de dispositivo y guia contextual separadas | Distinguir observar, operar y comprender |
| Escenario y reinicio al fondo | Barra de equipo y reinicio | Acciones de sesion accesibles |
| Catalogo largo sin filtro | Busqueda por nombre/codigo | Encontrar una pantalla concreta |
| Seleccion fuera del tramo visible de la lista | Scroll dentro de la lista | Seguir el encoder sin mover la pagina |

Fuente implementada con tokens y controles existentes. Sin navegador, tests,
compilacion ni validadores; aspecto e interaccion pendientes de comprobar.
No cambia la navegacion ni los renderers originales del ATmega.

Fuente AFR2-20261031-01; sin compilacion ni pruebas.

Tres toques seguidos sobre el robot de Inicio abren Desarrollador. Tambien
se puede abrir `#/desarrollador`. **Simulador UI** inicia en la pantalla
inicial del ATmega y usa exclusivamente un equipo ficticio local.

1. Toca SW sobre Menu o mantenelo 800 ms para abrir el principal.
2. Giro izquierdo/derecho elige; SW corto entra o confirma.
3. En un editor, gira para cambiar y confirma con corto. Largo descarta y
   abre principal.
4. Elige Volver y confirma para regresar. Radar/Distancia, IR grafica/Valores
   y Angulo del servo permiten volver con corto, como el wrapper del firmware.
5. La guia al lado indica la seleccion y el gesto correspondiente.

Escenario y alcance permite elegir equipo preparado o pendiente, reiniciando
la sesion. Inspeccionar abre cualquier identidad del catalogo, incluidos
avisos de red y una secuencia de tres pasos. Reiniciar vuelve a Inicio.
Usar solo SSID/claves de ejemplo. EEPROM es una copia en memoria, descartada
al salir; no hay envio al equipo. Reloj pausado con pestaña oculta.

`UiSimulator.tsx` importa el mismo `EncoderControl` de `LiveOledViewer`.
`uiSimulation.ts` adapta navegacion de `afr2_screens.c` y snapshots de
`afr2_app.c`. Tablas y dibujos vienen de `generated/firmwareLibrary.ts`,
`firmwareRenderer.ts` y `firmwareSensorRenderer.ts`; identidades de
`screenCatalog.ts` y `Contracts/documentation/screens.json`.
Recursos y fuentes existentes, sin regenerar. No se edita firmware.

Servicios simplificados e inmediatos: no emula binario AVR, fisica,
protocolos, todas las condiciones de configuracion ni colas/tiempos de
servicios. **SIMULACION UI · DATOS FICTICIOS** distingue estas vistas de las
capturas OLED. Funcionamiento y fidelidad no probados.
