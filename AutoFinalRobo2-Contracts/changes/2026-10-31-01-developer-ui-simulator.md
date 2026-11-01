# AFR2-20261031-01 - Simulador UI del ATmega en Desarrollador

31/10/2026. Pedido humano: navegar desde la Web las pantallas del ATmega con
todo simulado, reutilizando los tres botones del modulo de controles en vivo.

## Antes y despues

La biblioteca OLED existente permite previsualizar el diseno y parte de los
menus; necesita snapshots conectados para vistas con datos y no ejecuta acciones.
Desarrollador agrega ahora **Simulador UI** dentro de `#/desarrollador`.
Reutiliza `EncoderControl`: Giro izquierdo, SW y Giro derecho. SW corto confirma;
SW largo de 800 ms abre principal y su liberacion no confirma. Volver restaura
historial de seleccion/firstVisible hasta ocho niveles. Radar/Distancia e IR
grafica/Valores alternan por giro y vuelven por corto; Angulo del servo permite
cambiar su valor ficticio por giro y volver por corto.

La maquina local adapta `Afr2Ui_Apply`, `Afr2Ui_Open`, `back` y el teclado de
`Atmega/src/ui/afr2_screens.c`. Usa las tablas de la biblioteca AVR ya copiadas,
`firmwareMenuItem`, los renderers TypeScript y el raster 128x64 existentes.
No agrega ScreenCodes ni recursos; no modifica algoritmos de dibujo C/TypeScript
ni hashes fijados por su port anterior. Catalogo compartido conservado.

Incluye editores con rango/paso y borrador; corto aplica y largo descarta.
Teclado de 97 posiciones con SSID/clave, borrar, OK y limites de longitud.
Avisos locales de tres segundos, cerrables por corto. Red, sensores, EEPROM,
Hall y movimientos usan fixtures explicitamente ficticios. Perfil preparado
para explorar acciones y perfil pendiente para ver falta de datos y bloqueos.
Escenario o Reiniciar restablecen el estado. Datos solo en memoria; no se
guardan credenciales ni ajustes en almacenamiento.

La guia muestra contexto, opcion y siguiente gesto. El selector de inspeccion
deriva todas las identidades de `screens.json`, incluidos avisos de red y una
secuencia de ejemplo. Es un acceso de desarrollo, no una opcion agregada al menu
del ATmega. Reloj pausado con pagina oculta; cancelacion de gestos por el modulo
compartido de encoder. Fuente implementada, sin ensayo de fidelidad.

## Limites del modelo

Navegacion adaptada por fuente; no ejecuta el binario ni emula el procesador.
Servicios inmediatos y simplificados: no reproduce colas ni asincronia UART,
EEPROM y Wi-Fi, todas las condiciones cruzadas de configuracion, adquisicion,
odometria o fisica. Inspeccion muestra los once tipos de aviso sin conexiones.
192.0.2.x y nombres/claves demo son muestras; el perfil preparado no habilita
capacidades reales. Paridad de navegacion y raster no ensayada.

## Productores, consumidores y wire

Productor: `Web/src/features/developer/uiSimulation.ts`, independiente del cliente
de conexion. Consumidor: `UiSimulator.tsx` y renderer OLED existente.
`OledCanvas128x64` agrega solo el origen `local-ui-simulation`; visor conectado
conserva su framebuffer remoto. Sin cambios de protocolo, permisos, EX01 ni
comandos. El simulador no llama a `ui.action`, `robot.*`, serie ni placas.

## Estado por modulo

- atmega: sin cambios; origen de navegacion, identidad y layouts.
- esp: sin cambios; `data/dist` conserva su paquete anterior.
- web: fuente implementada, sin compilar/ejecutar ni promover al ESP.
- shared/Contracts: ficha, procedencia y flujo documental; catalogo conservado.
- CAD/Loader: sin cambios.

Por instruccion humana no se ejecutaron tests, compilaciones, validaciones,
navegador ni flasheos. No se realizo commit ni push.

[Procedencia](../provenance/developer-ui-simulator-20261031.json) ·
[Guia](../../AutoFinalRobo2-Web/docs/modules/ui-simulator.md).
