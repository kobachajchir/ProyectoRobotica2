# AFR2-20261024-10 — Simulador GOTO con destinos y cola

24/10/2026. Pedido humano: elegir destinos sobre el plano del mapa, ejecutar
comandos consecutivos como Adelante2m/Izquierda1m, marcar el actual con acento
y seguirlo con scroll/fade usando animate. Aclaración incorporada: mapa ancho
completo con coordenadas en fila, listado debajo y botones al lado.

## Implementación

Nuevo simulador en Control/GOTO. El gesto de selección intersecta el suelo real
Y=0 de Three.js, invierte worldX/Z a Y/X en mm y ajusta a100mm. El fallback SVG
invierte su matriz pantalla/viewBox. Toque/clic exige press/release del mismo
puntero con desplazamiento máximo6px; no convierte un arrastre en destino.
Coordenadas por teclado como alternativa. Selección agrega un comando local;
Simular recorrido inicia la lista, sin adquirir lease ni enviar a una placa.

Plan puro: paso cardinal relativo al extremo anterior o punto absoluto del
plano simulado. Traslación Mecanum ideal a rumbo0; interpolación analítica sin
deriva final, con esquinas conservadas. Velocidad máxima50..500mm/s, tiempo por
tramo redondeado hacia arriba a10ms. 64comandos/600000ms, pasos100..10000mm y
coordenadas±50000mm por eje; rechaza no finitos/destinos coincidentes/límites.
La cola se puede quitar/reordenar/vaciar detenida. Edición reinicia el cursor.

Reloj local publicado a30Hz; pausa/continuación/repetición/reinicio. Comandos y
velocidad pertenecen a ControlPage y sobreviven al cambio de tarea, pero no se
reanuda una ejecución al volver. RAF se cancela al desmontar, perder foco,
ocultar la página o emitir STOP global. Actividad simulation conserva STOP
superior visible mientras reproduce. No se añade otra acción STOP.

Mapa comparte GLB final/precise ground, unidad1m y cuadrícula0,5m. Trazo previsto,
destinos, encuadre de ruta y cámaras; no se modifica la malla ni escala del auto.
Mapa y X/Y/rumbo/tiempo usan todo el ancho. Lista debajo, reproducción lateral;
mobile apila controles. El formulario de objetivo físico se mantiene en un
disclosure propio con gates previos, sin vincular esta cola a robot.goto.start.

## Movimiento y accesibilidad

Skill aplicada: `C:/Users/kobac/.agents/skills/animate/SKILL.md`, receta WAAPI.
Gate: avance automático durante reproducción, propósito State indication y
Spatial consistency. Sin animar navegación/edición por teclado. Acento por
capas opacity,140ms ease (`--duration-fast`); scroll por FLIP transform,220ms
`--ease-in-out` (`--duration-normal`). Sin nueva librería ni transition:all.
Interrupción conserva posición visual; solo se desplaza el contenedor de lista,
sin foco ni scroll de página. Wheel/touch/scroll keys desactivan Seguir activo.
Reduced motion/teclado evita transform; fade suave120ms con reduced motion.
Hover de filas/acciones limitado a hover:hover/pointer:fine.

Lista semántica, aria-current=step, estado escrito además de color, anuncio de
comando actual sin tick de reloj y controles44px. El listado memoizado y los
destinos estables entre tramos evitan reconstruirlos en cada frame.

## Estado y evidencia

| Módulo | Estado |
| --- | --- |
| Web | Implementado y compilado con TypeScript/Vite; paquete ESP promovido localmente. Navegador/gestos/animación sin verificar. |
| ESP | Fuente/firmware sin cambios10, evidencia08 conservada. Solo activos Web actualizados. |
| ATmega | Sin cambios10; fuente/compilación08 y gates de memoria/Hall/calibración conservados. |
| Contracts | Contrato de mapa, flujo local, plan/continuidad/estado, ficha y procedencia actualizados. Sin nueva API UART/WS ni ScreenCodes. |

[Build10](../documentation/builds/2026-10-24-10/README.md) y
[procedencia](../provenance/goto-map-queue-20261024.json). Compilación bajo pedido
previo explícito de compilar/corregir la integración; sin suite de tests,
validadores independientes, navegador (denegado previamente), pruebas físicas,
imagen LittleFS, reinicios, flasheo, CAD, commit ni publicación remota.
Pendiente comprobar tacto del scroll/fade, punteros y temas en un navegador
autorizado. La fuente compilada no acredita navegación física del robot.
