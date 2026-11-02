# De funciones del equipo a flujo y pantallas

## Leer una explicación funcional

La persona puede empezar por «tiene dos motores, mide distancia y permite
cambiar la velocidad». Extraer tareas antes de repartir pantallas:

| Pregunta | Resultado de diseño |
| --- | --- |
| ¿Qué necesita saber al encender? | Inicio/dashboard |
| ¿Qué necesita observar con detalle? | Estado/medida/gráfica |
| ¿Qué ajusta y con qué límites? | Menú de ajustes + editor |
| ¿Qué inicia, detiene o calibra? | Acciones de servicio y flujo de operación |
| ¿Qué condición impide ejecutarlo? | Disponibilidad y explicación de requisito |
| ¿Qué puede tardar o fallar? | Espera, progreso, fallo y retorno |

Una función de firmware no implica una pantalla. Un proceso de calibración
puede necesitar varias; varias mediciones pueden compartir un dashboard.
No volcar todos los parámetros internos al menú del usuario.

Si faltan datos, avanzar con el mapa disponible y marcar supuestos. Preguntar
por hardware de entrada, prioridades y efectos de acciones cuando cambien la
navegación o el contenido. No pedir al usuario coordenadas de píxeles.

## Árbol y transiciones son cosas diferentes

Perfil vigente OLED128-3 (hereda navegación2): largo retorna al padre, representado por encoder
mantenido + flecha. Firmware AFR2 conserva el perfil anterior hasta migración.
El árbol organiza destinos: Inicio → principal → subsistemas → detalle.
Las transiciones explican lo que ocurre dentro de cada contexto:

```text
Inicio --corto/OK--> Principal --corto en item--> Menu de subsistema
Pagina --giro--> pagina hermana con padre comun
Menu --giro--> otra opcion
Menu --corto en valor--> Editor --giro--> Borrador
Borrador --corto--> Aplicando --resultado--> Aviso / Error
Borrador --largo--> contexto de origen, sin aplicar
Submenu --largo/flecha--> contexto previo con foco restaurado
Grafica --giro--> Valores --giro--> Grafica
Aviso --corto o timeout permitido--> mismo contexto previo
```

Los nombres `Aplicando` y `Error` representan estados, no una obligación de
crear pantallas separadas si caben en la plantilla. En cada flujo definir qué
eventos son válidos y qué pasa con entradas mientras el servicio está ocupado.
La liberación después de largo no genera corto. No hay un botón Menú extra.

Mantener ID de pantalla, ID de opción, selección, primera fila visible e
historial; al volver restaurar contexto. Si cambia una lista dinámica,
restaurar por ID estable. No ejecutar el elemento que ocupó el índice antiguo.
Cancelar una calibración, volver y detener un motor no son sinónimos: definir
sus efectos mediante el servicio real, no desde el renderer.

## Ficha por pantalla o variante

Usar esta estructura como contenido; adaptar formato al catálogo del proyecto:

```yaml
id: motor.status
role: dashboard
template: ESTADO-3/plain
title: Estado
header_icon: actuator.motor
parent: motor.menu
purpose: Conocer modo, salida y autoridad de control
fields:
  - key: mode
    label: Modo
    source: motor_service.mode
    format: enum_localizado
    unknown: --
  - key: applied_pwm
    label: PWM
    source: motor_service.applied_pwm
    unit: pwm_count
    range: [-255, 255]
    decimals: 0
    validity: reporte vigente del servicio
    unknown: --
inputs:
  rotate: cambiar pagina hermana si existe
  short: confirmar tarea indicada por OK, u omitir si no hay tarea
  long: volver al contexto padre
return:
  restore: opcion y primera fila anteriores
service_effects: ninguno al dibujar ni al volver
states: [valid, unknown, stale, fault]
implementation: propuesta
```

Para editor añadir mínimo/máximo, incremento, unidad, borrador, valor aplicado,
guardar persistente y resultado. Para proceso añadir precondiciones, inicio,
cancelación, finalización, fallos y comportamiento al salir. Para gráfico
añadir ejes, canales, escala y edad de muestra. Para red definir privacidad y
qué datos puede ver la réplica.

## Ejemplo de proyecto nuevo

Explicación: «Un equipo mide distancia; puedo ajustar el límite y accionar un
motor cuando lo indico». No suponer automatismo por distancia si no se pidió.

```text
Inicio [ESTADO-3]
Principal [MENÚ-3]
  Medicion [MENÚ-3]
    Distancia [MEDIDA-1]
    Grafica / Valores [hermanas, si aportan algo a la tarea]
  Motor [MENÚ-3]
    Estado [ESTADO-3]
    Velocidad [EDITOR-1]
    Iniciar [accion del servicio]
    Detener [accion del servicio]
  Ajustes [MENÚ-3]
    Limite [EDITOR-1]
    Guardar [accion persistente si existe]
  # Cada nivel retorna con largo + flecha, sin filas Volver.
```

`Iniciar` puede fallar por requisito, aceptar, quedar pendiente o terminar.
Esos resultados se modelan; no se inventa «motor funcionando» porque el usuario
hizo clic. No incluir una opción Guardar si el producto no tiene persistencia.
Mostrar cinco u ocho sensores simultáneamente con SensorBarGroup; distribuir
otros detalles en páginas por tarea, no dividir por defecto el conjunto IR.

## Implementación y simulación

Separar servicio → snapshot preparado → renderer puro. El wrapper transforma
eventos de entrada en navegación o solicitudes; el renderer no lee periféricos
ni manda comandos, espera o escribe EEPROM. No acoplar un ScreenCode con un
opcode por comodidad. Registrar permisos físicos/remotos de cada acción.

Si se pide simulador, mantener los mismos IDs, recursos, geometría, selección,
ventanas y semántica de gestos. Las respuestas demo son un adaptador de servicios
identificado. Mostrar explícitamente lo que no reproduce: tiempos, física,
protocolos y precondiciones simplificadas. Captura en vivo y simulación tienen
orígenes distintos. No ocultar la falta de una captura con un dibujo local.

## Estado de entrega

Entregar lo que se solicitó: mapa y fichas en fase de diseño; fuentes si se
pidió implementación. Señalar decisiones que necesitan definición funcional,
sin añadir una aprobación genérica antes de tareas ya autorizadas. Registrar
las comprobaciones pendientes sin ejecutarlas por defecto.
