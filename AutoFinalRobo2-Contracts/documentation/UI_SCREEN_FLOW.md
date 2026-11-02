# Flujo real de pantallas AVR y Simulador UI

AFR2-20261101-02 · 01/11/2026. Revisión de fuente, sin ejecutar firmware,
Web, tests, compilaciones, validadores, generadores ni hardware.

Fuente de navegación: `AutoFinalRobo2-Atmega/src/ui/afr2_screens.c`, funciones
`Afr2Ui_Apply`, `Afr2Ui_Open`, `go_back`, `back`, `dismiss` y `Afr2Ui_Tick`.
Opciones: `afr2_menu_catalog.c` y sus resolvers dinámicos. Servicios y aperturas
por respuesta: `afr2_app.c`/`afr2_app_wire.c`. Entrada física:
`src/input/afr2_encoder_ui.c`. Las rutas AVR son relativas al módulo Atmega.
El árbol objetivo histórico de MENU_PLAN.md no sustituye estas fuentes.

## Reglas de giro, corto y largo

El umbral largo es configurable 800..3000 ms, predeterminado 800 ms. Tanto
corto como largo se ejecutan **al soltar**; mantener no navega y un largo nunca
genera después un corto. El simulador usa el mismo umbral configurado.

| Pantalla o contexto | Giro | Corto | Largo al soltar |
| --- | --- | --- | --- |
| Inicio 010001 | Sin acción | Menú principal 010002 | Controles rápidos 010003 |
| Rápidas 010003 | 7 tarjetas, limitado 0..6 | Alternar servicio; tarjeta 6 abre menú y 7 abre modo | Inicio, con Sí/No si cambió rastro/alcance |
| Menús comunes | Circular; ventana de 3 filas | Abrir opción, editor o servicio | Retorno contextual, con guardas de cambios |
| Vistas comunes con Volver | Selección de su opción | Activar Volver | Mismo retorno contextual |
| Editor numérico | Limitado a min/max/paso | Aplicar, cerrar editor y avisar | Descartar borrador y cerrar editor; no abandonar el menú |
| Editor de motor 030044 | FL/FR/RL/RR circular | Aplicar motor, regresar al menú | Cerrar editor sin aplicar |
| Radar 030031 / Numérica 030017 | Derecha Radar→Numérica; izquierda Numérica→Radar; sin envolver | Opciones radar 03003b | Retorno contextual; Sí/No si cambió alcance/rastro |
| IR gráfica 030016 / Valores 030032 | Derecha gráfica→valores; izquierda valores→gráfica | Sin acción | Retorno contextual |
| Ángulo servo 030033 | 0..180 grados, limitado | Sin acción | Retorno contextual y fin de edición de servo; reanudar radar pausado si procede |
| Hall: elegir rueda 030036, reportada 030044 | FL/FR/RL/RR circular | Elegir motor, comenzar conteo 030037 | Cancelar intento y volver a Hall, conservar borradores terminados |
| Hall: conteo 030037 | No cambia los pulsos | Finalizar vuelta, volver a Hall con borrador | Cancelar intento y volver a Hall, conservar otros borradores |
| Guardar cambios 030038 | Alternar Sí/No | Sí: solicitar guardado y esperar; No: descartar y salir | Sin acción, no evita el diálogo |
| Guardando dentro de 030038 | Sin acción | Sin acción | Sin acción hasta respuesta del servicio |
| Teclado de credenciales | 97 posiciones: ASCII, borrar y OK | Escribir; OK pasa SSID→clave→Configurar | Borrar entrada y cerrar teclado; permanecer en Credenciales |
| Aviso común | Sin acción | Cerrar aviso | Cerrar aviso, conservar contexto; cumplir destino posterior si existe |
| Conexión 03002d | 2 páginas con límites | Volver | Volver |
| Capacidades 030028 | 6 páginas con límites | Volver | Volver |
| Cambiar modo 030043 | STA/AP/AP-STA con límites | Solicitar cambio; esperar respuesta ESP | Volver |
| Datos de red 030041 | Sin acción | Volver | Volver; borrar credenciales locales visibles al salir |
| Prueba encoder 03003e | Elegir Volver | Registrar corto; si se giró, Volver | Primero registrar largo; siguiente largo Volver |
| Control táctil externo 030035 | Ignorado | Ignorado | STOP y cerrar presentación; recuperar contexto previo |

Prioridad especial en C: táctil, prueba encoder, diálogo Sí/No, Hall y teclado
se procesan antes que el largo genérico y sus avisos. En Sí/No con aviso solo
corto lo cierra; largo no evita la decisión. En Hall, largo cancela el intento
incluso con aviso. En teclado, largo borra entrada incluso con aviso.

## Árbol de opciones implementadas

Cada línea de opción se activa con **corto**. `[editar]` entra en el editor
descrito arriba; `[acción]` invoca un servicio y puede fallar según requisitos.
Todos los Volver usan contexto/historial; el padre estático solo sirve de
fallback sin historial. Se conservan selección y desplazamiento de la entrada.
Destinos repetidos comparten pantalla, no copias con otro retorno.

```text
Inicio 010001
├─ corto → Menú principal 010002
│  ├─ Estado 030004
│  │  ├─ Resumen robot 030005 → Volver
│  │  ├─ Enlace y control 030006 → Volver
│  │  ├─ Alertas 030007 → Volver
│  │  ├─ Estado OLED/I2C 020002 → Volver
│  │  ├─ Estado encoder 030018 → Volver
│  │  ├─ Control manual 030008
│  │  │  ├─ Detener [acción]
│  │  │  ├─ Límite velocidad mm/s [editar 10..1000, paso 10]
│  │  │  ├─ Potencia manual % [editar 0..100, paso 5; reporta 030040]
│  │  │  ├─ Mecanum 030009
│  │  │  │  ├─ Adelante / Atrás / Izquierda / Derecha [acciones]
│  │  │  │  ├─ Diagonal AD-I / AD-D / AT-I / AT-D [acciones]
│  │  │  │  ├─ Girar CCW / Girar CW / Detener [acciones]
│  │  │  │  └─ Volver
│  │  │  └─ Volver
│  │  ├─ Modos/misiones 03000a
│  │  │  ├─ Reposo/detenido [acción]
│  │  │  ├─ Seguir línea 03000b
│  │  │  │  ├─ Estado/requisitos 03000c → Volver
│  │  │  │  ├─ Velocidad mm/s [editar 10..500, paso 10]
│  │  │  │  ├─ Ganancia [editar 0..1000, paso 10]
│  │  │  │  ├─ Iniciar / Detener [misma fila dinámica]
│  │  │  │  └─ Volver
│  │  │  ├─ GOTO relativo 03000d
│  │  │  │  ├─ Odometría 030010 → Volver
│  │  │  │  ├─ Objetivo X / Y mm [editar -5000..5000, paso 10]
│  │  │  │  ├─ Límite mm/s [editar 10..1000, paso 10]
│  │  │  │  ├─ Revisar objetivo 03000e → Volver
│  │  │  │  ├─ Iniciar / Detener [misma fila dinámica]
│  │  │  │  ├─ Resultado 03000f → Volver
│  │  │  │  └─ Volver
│  │  │  └─ Volver
│  │  └─ Volver
│  ├─ Wi-Fi 03001f
│  │  ├─ Estado de red 030020
│  │  │  ├─ Estado de conexión 03002d [2 páginas]
│  │  │  ├─ Datos de red 030041
│  │  │  ├─ Configurar 030023
│  │  │  │  ├─ Modificar credenciales → Credenciales 030022
│  │  │  │  ├─ Conectar / Desconectar [condicional]
│  │  │  │  └─ Volver
│  │  │  ├─ Conectar / Desconectar [condicional]
│  │  │  ├─ Reintentar [solo desconectado con estado disponible]
│  │  │  └─ Volver
│  │  ├─ Cambiar modo 030043 [STA / AP / AP-STA]
│  │  ├─ Buscar redes 030042
│  │  │  ├─ Redes encontradas 030021
│  │  │  │  ├─ 0..8 redes reales en firmware / ficticias en simulador
│  │  │  │  │  └─ Credenciales 030022
│  │  │  │  │     ├─ Traer credenciales ESP → respuesta → Datos de red 030041
│  │  │  │  │     ├─ Ingresar aquí → teclado SSID/clave → Configurar 030023
│  │  │  │  │     ├─ Ingresar en Web → respuesta → Configurar 030023
│  │  │  │  │     └─ Volver
│  │  │  │  └─ Volver [siempre existe, aun con cero redes]
│  │  │  ├─ Refrescar [acción; aviso Redes listas → listado al cerrar]
│  │  │  └─ Volver
│  │  ├─ AP / fallback 030024
│  │  │  ├─ Modo AP [acción]
│  │  │  ├─ Modo AP-STA [acción]
│  │  │  └─ Volver
│  │  └─ Volver
│  ├─ Sensores 020001
│  │  ├─ Distancia y servo 030030
│  │  │  ├─ Ver distancia → Radar 030031 ↔ Numérica 030017
│  │  │  │  └─ corto → Opciones radar 03003b
│  │  │  │     ├─ Alcance del gráfico 03003c
│  │  │  │     │  └─ 4 m / 2 m / 1 m / 0,5 m [acción; vuelve a Opciones]
│  │  │  │     └─ Volver [recupera Radar o Numérica según entrada]
│  │  │  ├─ Servo 030019
│  │  │  │  ├─ Ángulo 030033 [giro; largo vuelve]
│  │  │  │  ├─ Centrar [acción]
│  │  │  │  ├─ Desactivar [acción]
│  │  │  │  └─ Volver
│  │  │  ├─ Radar automático 030034
│  │  │  │  ├─ Iniciar / Detener [misma fila dinámica]
│  │  │  │  ├─ Desde grados [editar 0..175, paso 5]
│  │  │  │  ├─ Hasta grados [editar 5..180, paso 5]
│  │  │  │  ├─ Velocidad grados/s [editar 1..30, paso 1]
│  │  │  │  ├─ Estabilizar ms [editar 50..500, paso 10]
│  │  │  │  ├─ Intervalo HC ms [editar 70..1000, paso 10]
│  │  │  │  └─ Volver
│  │  │  └─ Volver
│  │  ├─ Línea IR1..IR5 030016 ↔ Valores 030032 [largo vuelve]
│  │  ├─ Hall / velocidad 030015 → Volver
│  │  └─ Volver
│  ├─ Pruebas 030011
│  │  ├─ Motor individual 030012
│  │  │  ├─ Rueda FL/FR/RL/RR [editor 030044]
│  │  │  ├─ PWM con signo [editar -255..255, paso 5]
│  │  │  ├─ Duración ms [editar 100..2000, paso 100]
│  │  │  ├─ Iniciar / Detener [misma fila dinámica]
│  │  │  ├─ Resultado 030013 → Volver
│  │  │  └─ Volver
│  │  ├─ Hall una vuelta 030014
│  │  │  ├─ Rueda FL/FR/RL/RR [editor 030044]
│  │  │  ├─ Estado / cuentas 030015 → Volver
│  │  │  ├─ Iniciar conteo → Elegir rueda 030036, reportada 030044
│  │  │  │  └─ corto → Conteo Hall 030037 → corto finaliza y vuelve a Hall
│  │  │  ├─ Guardar medición [acción asíncrona, cuatro borradores]
│  │  │  ├─ Descartar [descarta mediciones terminadas]
│  │  │  └─ Volver [con borradores → Sí/No 030038]
│  │  ├─ Línea cinco → IR 030016
│  │  ├─ HC-SR04 → Numérica 030017
│  │  ├─ Prueba encoder 03003e
│  │  ├─ Prueba ESP 03003f
│  │  │  ├─ Iniciar prueba [respuesta asíncrona]
│  │  │  ├─ Timeout ms [editar 1000..5000, paso 250]
│  │  │  └─ Volver
│  │  ├─ Servo → 030019
│  │  └─ Volver
│  ├─ Calibración 03001a
│  │  ├─ Línea 03001b
│  │  │  ├─ Tipo 0/1dig/2adc [editar 0..2]
│  │  │  ├─ Invertir bits [editar 0..31]
│  │  │  ├─ Canal IR 1..5 [editar índice 0..4]
│  │  │  ├─ Umbral ADC [editar 0..1023, paso 8]
│  │  │  ├─ Confirmar polaridad [acción]
│  │  │  ├─ Ver lectura → IR 030016
│  │  │  └─ Volver
│  │  ├─ Motores 03001c
│  │  │  ├─ Signos de motor [editar 0..15]
│  │  │  ├─ Límite PWM [editar 1..255, paso 5]
│  │  │  ├─ Rampa por 10 ms [editar 1..32]
│  │  │  ├─ Kp / Ki / Kd Q8 [0..2048/0..1024/0..2048; pasos 16/4/16]
│  │  │  ├─ Habilitar control [editar 0/1]
│  │  │  └─ Volver
│  │  ├─ Hall → 030014
│  │  ├─ Ver odometría → 030010
│  │  ├─ Descartar cambios [solo si hay cambios]
│  │  └─ Volver
│  ├─ Sistema 030026
│  │  ├─ Versiones 030027 → Volver
│  │  ├─ Preferencias 030029
│  │  │  ├─ Encoder 03001e
│  │  │  │  ├─ Transiciones por paso [editar 2/4]
│  │  │  │  ├─ Invertir giro [editar 0/1]
│  │  │  │  ├─ Pulsación larga ms [editar 800..3000, paso 100]
│  │  │  │  └─ Volver
│  │  │  ├─ Invertir servo [editar 0/1; requiere servo desactivado]
│  │  │  ├─ Guardar EEPROM [asíncrono]
│  │  │  ├─ Descartar cambios [solo si hay cambios]
│  │  │  ├─ Estado 03002a → Volver
│  │  │  └─ Volver
│  │  ├─ Batería 030039
│  │  │  ├─ Realizar medición [acción; permanece en Batería]
│  │  │  ├─ Ver medición 03003a → Volver
│  │  │  └─ Volver
│  │  ├─ Reinicio 03002f
│  │  │  ├─ Reiniciar ATMEGA / Reiniciar ESP [acciones]
│  │  │  └─ Volver
│  │  ├─ Diagnóstico 03002b
│  │  │  ├─ Alertas 030007
│  │  │  ├─ Limpiar fallo [acción]
│  │  │  └─ Volver
│  │  ├─ Capacidades 030028 [6 páginas]
│  │  ├─ Pantalla/interfaz 030025
│  │  │  ├─ Contraste [editar 0..255, paso 8]
│  │  │  ├─ Rotación 0/180 [editar 0/1]
│  │  │  └─ Volver
│  │  └─ Volver
│  └─ Volver a Inicio [si se entró desde Rápidas, vuelve a ese contexto]
└─ largo → Controles rápidos 010003
   ├─ Barrido del radar [alternar]
   ├─ Servo [alternar]
   ├─ Motores [habilitar/bloquear]
   ├─ Seguir línea [alternar]
   ├─ Rastro del radar [mostrar/ocultar; cambio pendiente]
   ├─ Menú principal → 010002 [retorno a esta tarjeta]
   └─ Selección de modo 03003d
      ├─ Reposo [acción]
      ├─ Manual [acción]
      └─ Volver [conserva tarjeta]
```

Pantallas conservadas fuera del árbol normal: Geometría nominal 03001d
(solo lectura), Datos de red AP 03002e, Potencia manual 030040 (variante de
editor y nodo de inspección). Secuencia 03002c se abre por inicio de ejecución
externa; Táctil 030035 es presentación externa. Guardar cambios 030038 se abre
por guarda; los avisos de red 040001..04000b son eventos, no menús adicionales.
El selector 030044 es variante de Hall/editor de motor, no un nodo 69 inventado.

## Cambios pendientes y respuestas

Los grupos Preferencias, Calibración, Encoder, Pantalla, Motores, Línea y Radar
exigen Sí/No al salir con configuración modificada. Rápidas/Radar/Numérica
preguntan por alcance/rastro; No descarta solo esas preferencias visuales.
Hall pregunta por borradores medidos; cancelar intento no descarta otros motores.
Sí espera la finalización de EEPROM; una solicitud aceptada no es guardado completo.

Wi-Fi depende de estado disponible: conectado sustituye Conectar por Desconectar
y oculta Reintentar; desconocido oculta ambas acciones. Un listado vacío conserva
Volver. Refrescar abre listado al cerrar el aviso Redes listas. Sin cliente Web,
el aviso No hay clientes vuelve al listado. Consulta explícita de credenciales
abre Datos de red; su vista privada se borra al salir o a los 30 s.

## Implementación Web y límites

`uiSimulation.ts` adapta el estado y las transiciones, usando `firmwareNode`,
`firmwareNodeCount` y `firmwareMenuItem`; opciones visibles y dibujo reciben el
mismo snapshot de selección, cambios y red. `uiSimulationFlow.ts` describe los
gestos según prioridad de C. `UiSimulationTree.tsx` ofrece un árbol desplegable
del catálogo compartido, con referencias para destinos repetidos y sus 68 nodos.
No hay una segunda tabla independiente de opciones del simulador.

La guía anterior de corto para volver desde IR/servo y largo global a menú era
histórica: no describe `Afr2Ui_Apply` actual. Esta adaptación sigue el C vigente
y no cambia firmware para imponer esa documentación antigua.

Los servicios siguen siendo ejemplos identificados en memoria. Las respuestas
de guardado/Wi-Fi/ESP se completan en un reloj local de demostración (600 ms),
sin reproducir UART, EEPROM, colas reales, tiempos eléctricos, sensores, física
ni todas las causas de fallo de producto. El conteo Hall ficticio avanza con el
reloj, nunca con giro del encoder. No se altera el visor conectado ni se envían
órdenes al auto. Sin ejecución no se certifica paridad visual ni de hardware.

## AFR2-20261102-05 — Ayuda y QR

Ayuda → OLED abre ahora la simulación interactiva con el reductor existente y los diseños OLED128-5; Biblioteca sin datos permanece como alternativa. Wi-Fi incorpora030045 QR conectar AP y030046 QR abrir Web. Ambas vistas: giro sin acción; corto y largo al soltar vuelven al contexto anterior. Credenciales reales solo en OLED, QR AP protegido de captura/entrada remota. Generación integrada exige auto/radar detenidos. Ver ../contracts/OLED_RUNTIME_QR_20261102.md. Fuentes y copia de recursos, sin tests, build, validadores, navegador ni flasheo.

## AFR2-20261102-14 — Tarjeta de estados en Inicio

010001 agrega modo compacto y tarjeta informativa redondeada. Tras3s sin encoder alterna estados aplicables cada4s; mantener SW pausa y un fallo permanece fijo. Un estado retirado se actualiza inmediatamente. No es una opción de navegación: corto sigue abriendo010002 y largo al soltar010003. Snapshot local real y réplica Web con fixtures. Las guardas Sí/No no cambian. Detalles en ../contracts/OLED_DASHBOARD_STATUS_20261102.md. Sin ejecución de pruebas, build, validadores ni hardware.
