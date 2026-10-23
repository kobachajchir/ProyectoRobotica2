# Árbol funcional de menús por implementar

Fecha 2026-10-23. Diseño inicial para contextualizar el trabajo futuro. [Reglas de entrada](NAVIGATION.md). El prototipo fuente sigue teniendo solo Inicio, menú reducido, Sensores y Estado OLED. Esta lista no agrega handlers ni asegura capabilities.

```text
Inicio / dashboard
Menú principal (SW largo)
  Estado
    Resumen del robot
    Enlace Mega-ESP y dueño del mando
    Alertas / datos caducados
    Volver
  Control manual
    Detener
    Tomar / liberar mando
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
