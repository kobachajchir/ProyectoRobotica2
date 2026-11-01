> **AFR2-20261101-02:** [Arbol y gestos del C vigente](UI_SCREEN_FLOW.md), cotejados con el Simulador UI. Esta revision distingue el flujo actual del diseno e hitos historicos inferiores; corto en IR/servo no retorna, largo vuelve por contexto. Solo fuente, sin ejecucion.

> **Árbol fuente07:** Sistema y Estado/Pruebas reorganizados; ajustes de encoder/servo en Preferencias y geometría nominal fija. [Matriz vigente](../changes/2026-10-30-07-ui-integral.md). Árboles inferiores son históricos.

> **AFR2-20261030-05 parcial:** orden vigente de fuente y pendientes en [ficha](../changes/2026-10-30-05-oled-ajustes-parciales.md); el árbol histórico inferior conserva contexto.

> **AFR2-20261029-29:** desde dashboard, corto abre menu completo010002; largo al soltar abre pantallas rapidas010003. Ambos accesos conservan retorno al origen por historial. Sustituye el acceso comun de28. Fuente sin compilar/cargar.

> **AFR2-20261029-28:** clic corto desde dashboard abre Controles rapidos (010003); largo conserva su comportamiento al soltar. Fuente AVR y Web, sin compilar ni cargar. Sustituye la reserva anterior del corto en Inicio.

> **Vigente AFR2-20261029-27:** largo >=800ms ejecutado exclusivamente al soltar: Inicio -> Controles rápidos -> Inicio, resto atrás por contexto/historial; corto reservado en Inicio. Configuración y pruebas conserva el árbol previo bajo la sexta tarjeta. Excepción Sí/No obligatoria y cancelación Hall sin perder drafts. Radar y servo comparten editor; escala/rastro se guardan con EEPROMv5. [Contrato fuente y aceptación pendiente](../contracts/QUICK_RADAR_ACTUATORS_9.md). Las reglas anteriores que describen largo global a menú/Volver obligatorio son históricas. Sin build, validadores ni carga.

> **AFR2-20261029-26:** Batería añadida a Sistema en fuente con Realizar medición / Ver medición / Volver, detalle0x03003A. Web agrega pill e icono por voltaje y confirmación de reinicios. Fuente EX01v9 sin build/generador/pruebas. [Contrato9](../contracts/BATTERY_MEASUREMENT_9.md).

> **AFR2-20261029-20:** Hall una vuelta conserva Iniciar/Guardar/Descartar/Volver, sin Finalizar vuelta separado. Iniciar elige rueda1FL/2FR/3RL/4RR → corto comienza → corto termina. Largo en selector/conteo cancela al menú Hall con aviso, sin borrar drafts previos. Estado muestra4CPR y pendientes. Volver con drafts pregunta Si/No sin Volver; largo no evade esa decisión. [Contrato y excepciones contextuales](../contracts/HALL_CALIBRATION_WIZARD.md). Fuente OLED/Web vinculada, sin prueba física; no flashear.

## Vigente AFR2-20261025-14

Sensores → Distancia y servo (Ver distancia: Radar↔Numérica; Control del servo: Ángulo/Centrar/Desactivar/Volver; Radar automático: Iniciar/Detener/Límites/Velocidad/Estabilización/Intervalo/Inversión/Volver; Volver), Línea IR (Gráfica↔Valores), Ruedas y Volver. Vistas hermanas cortas retornan al listado; largo abre principal. Árbol integrado en fuente C y copia Web, sin ensayo. Ver SENSORS_RADAR_8.

> AFR2-20261025-01: se retiran las opciones de permiso de acceso por pedido explícito. Árbol restante, ScreenCodes y Volver conservados; [control directo7](../contracts/DIRECT_CONTROL_7.md).

# Árbol funcional de menús

Vigente AFR2-20261024-08:48nodos compilados y11avisos de red. Se añaden IP de
conexión/Datos AP y Sistema → Reinicio, preservando el árbol íntegro anterior.
Iconos F4 en flash, texto seleccionado con scroll y Volver en submenús.
[Contrato v5](../contracts/EX01_NETWORK_UI_5.md); sin ensayo físico.

Vigente AFR2-20261024-07: se conserva íntegro el árbol objetivo inferior. Se
agrega vista de ejecución0x03002c, abierta por secuencia y con Volver a Control;
no sustituye ni renumera los nodos previos. Catálogo, productor y consumidor
compilados; secuencias físicas permanecen bloqueadas por memoria y no probadas.
[Estado actual](../docs/ESTADO_IMPLEMENTACION.md).

Estado19/24-10: árbol implementado en fuente ATmega mediante44 nodos, sin compilar/probar.
[Catálogo](screens.json) y [detalle](../docs/ATMEGA_IMPLEMENTACION_19.md). La introducción
inicial siguiente describe el estado anterior. PCB/soporte siguen pendientes.


Fecha 2026-10-23. Diseño inicial para contextualizar el trabajo futuro. [Reglas de entrada](NAVIGATION.md). El prototipo fuente sigue teniendo solo Inicio, menú reducido, Sensores y Estado OLED. Esta lista no agrega handlers ni asegura capabilities.

```text
Inicio / dashboard
Menú principal (SW largo)
  Estado
    Resumen del robot
    Enlace Mega-ESP y origen de actividad
    Alertas / datos caducados
    Volver
  Control manual
    Detener
    Límite de velocidad
    Movimiento Mecanum
      Adelante / atrás
      Lateral izquierda / derecha
      Diagonales
      Giro izquierda / derecha
      Detener
      Volver
    Volver
  Modos y misiones
    Reposo / detenido
    Seguimiento de línea
      Estado y requisitos
      Velocidad / parámetros permitidos
      Iniciar / detener
      Volver
    GOTO relativo
      Estado de odometría / requisitos
      Objetivo X / Y
      Velocidad / límites
      Revisar objetivo
      Iniciar / cancelar / resultado
      Volver
    Volver
  Pruebas
    Motor individual FL / FR / RL / RR
      Selección / sentido / PWM acotado / duración
      Ejecutar / detener / resultado
      Volver
    Hall de ruedas: cuentas y sentido
    Cinco sensores de línea
    HC-SR04
    OLED e I2C
    Encoder UI: giro, corto y largo
    UART Mega-ESP
    Volver
  Sensores
    Línea IR1..IR5
    Distancia HC-SR04 y validez
    Hall / velocidades de ruedas (si están conectados)
    Volver
  Calibración
    Línea: polaridad y parámetros disponibles
    Motores: canal, signo y perfil
    Hall: cuentas por vuelta y sentido
    Geometría Mecanum / odometría
    Encoder UI: sentido y pasos
    Guardar / descartar cambios
    Volver
  Wi-Fi
    Estado STA / AP / IP
    IP de conexión (SSID y dirección obtenida)
    Datos de red AP (SSID / clave propia / IP, solo OLED local)
    Modo STA / AP / combinado, según soporte
    Buscar redes / seleccionar
    Configurar conexión
    Reintentar conexión
    Punto de acceso / fallback
    Aplicar / resultado
    Volver
  Pantalla e interfaz
    Contraste / orientación
    Estado OLED / I2C
    Parámetros del encoder
    Volver
  Sistema
    Batería
      Realizar medición de batería
      Ver medición (voltios / estado / edad / calibración)
        Volver
      Volver
    Reinicio
      Reiniciar ATMEGA
      Reiniciar ESP
      Volver
    Versiones Mega / ESP / Web
    Capacidades disponibles
    Preferencias y guardado
    Diagnóstico / errores
    Volver
  Volver a Inicio
```

## Reglas de implementación

Giro mueve selección o modifica borrador; corto activa/confirma. Largo SW abre menú principal desde cualquier vista, también desde diagnóstico de encoder, y no genera corto al soltar. NO hay botón menú separado, doble clic atrás ni largo atrás. Todo submenú y detalle de operación debe tener un ítem Volver al padre; si se necesita confirmar un descarte, se abre un diálogo con opciones explícitas.

Menús de más filas que la pantalla desplazan ventana conservando selección visible. Estado de UI incluye selectedIndex y firstVisible; no inferirlos en Web desde bloques de tres. No guardar ni actuar al girar por opciones. Distinguir vista/acción/invocador; un ítem de acción no exige renderer propio.

Valores se editan en borrador. Guardar/aplicar requieren acción explícita; Volver/largo no aplican cambios. Definir conservación/descarte del borrador y mostrarlo antes de persistir. En GOTO preparar X/Y no inicia misión. En control local, usar maniobras acotadas por tiempo/lease: cursor no mantiene motores activos. No existe pulsador separado de parada; mostrar Detener en vistas activas y resolver timeout/supervisión por firmware. Abrir menú/Volver no reanuda una misión ni equivale a STOP: la política al abandonar una operación debe documentarse antes de habilitarla.

No mostrar IMU/MPU/balance de dos ruedas. Funciones pendientes o sin hardware se deshabilitan con motivo, no se simulan. Lecturas ausentes/caducadas son unknown/stale. No derivar distancia desde línea5. Hall UI y Hall de rueda son subsistemas distintos. GOTO depende de odometría calibrada; Wi-Fi depende del servicio ESP real. Edición de credenciales puede usar OSK adaptado o Web, por decidir; nunca mostrarlas en diagnósticos/logs.

Conservar los cuatro IDs locales existentes de screens.json. Asignar IDs nuevos al implementar, con rol/estado/acciones y evidencia. No reutilizar códigos históricos como activos por copiar este árbol. Wire/reportes/actions nuevos requieren ficha AFR2 y adaptación de productor y consumidores.

La evidencia [del shield](../hardware/WIFI_SHIELD.md) no añade botones de navegación: KEY/RST son mantenimiento del ESP. Wi-Fi será gestionado por el firmware Arduino propio; el menú no envía AT, no conmuta jumpers y no inicia flasheo. Estado del enlace y errores de LittleFS deben venir del servicio real cuando se implemente, sin presentar la configuración nominal del módulo como medición.
