# AFR2-20261108-12 - Nuevo orden de Controles rápidos y tarjeta de estados del dashboard

Pedido del usuario (08/11/2026): reordenar las tarjetas rápidas (Modo, Motores, Ultrasonido, Radar,
Servo, Línea, Rastro, Ajustes) y que la tarjetita del dashboard, que mostraba solo "OFF" con el motor,
vaya mostrando de a poco el estado de todos esos sistemas, con símbolo y estado.

Versión: 1.6.0 (subversión).

## Qué cambió (ATmega y Web, mismas coordenadas y textos)

### Controles rápidos (0x010003)
- Orden: 0 **Modo** (entra a Elegir modo), 1 **Motores**, 2 **Ultrasonido**, 3 **Radar**, 4 **Servo**,
  5 **Línea**, 6 **Rastro**, 7 **Ajustes** (entra al menú principal). El carrusel abre en Modo.
- Ítems por tabla `{43,33,48,31,32,34,35,36}` sobre `extra[]` (`afr2_menu_catalog.c`); ningún recurso
  cambia de índice.
- `quick_flags`: el bit coincide con la página (bit1 motores … bit6 rastro). Lo usan el carrusel, el
  dashboard y la simulación Web.

### Tarjeta de estado del dashboard (0x010001)
- Antes solo mostraba lo encendido (o "OFF" con motores bloqueados) y solo rotaba con dos o más cosas
  activas.
- Ahora recorre siempre los seis sistemas en el orden de las rápidas, con ícono y estado:

| Sistema | Ícono | Texto |
|---|---|---|
| Motores | motor | OFF / ON / RUN (habilitados y en movimiento) |
| Ultrasonido | sensores | US OFF / US ON |
| Radar | radar | OFF / ON |
| Servo | servo | OFF / ON |
| Línea | sensores | IR OFF / IR ON |
| Rastro | radar | RS OFF / RS ON |

- La tarjeta mide 64 px: entran ícono + 6 caracteres. Ultrasonido/Línea y Radar/Rastro comparten ícono,
  por eso llevan sigla (US, IR, RS).
- Ritmo de AFR2-20261102-14 sin cambios: empieza a rotar a los 3 s sin tocar el encoder, una página cada
  4 s, se pausa al girar o apretar. Si un estado cambia, la tarjeta salta a ese sistema enseguida y sigue
  desde ahí. Un fallo tiene prioridad (FALLO con ícono de alerta). Sin datos del robot muestra "--".
- Se redibuja también cuando cambia el estado del sistema mostrado, no solo al pasar de página.

### Web
- `dashboardRenderer.ts`, `firmwareRenderer.ts` (carrusel e ítems), `uiSimulation.ts` (orden, clics y
  `quickFlags`). `screens.json` 010001 y 010003, y hashes de fuentes revisadas actualizados.

## Estado

atmega=fuente editada, sin compilar ni cargar; web=fuente editada, sin build ni navegador; esp=solo
versión compartida; loader=sin cambios.
