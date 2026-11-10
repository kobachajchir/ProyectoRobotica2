> Extensión AFR2-20261110-14: [limpieza Web confirmada por AVR y diagnóstico de F1](FAULT_RECOVERY_TIMING_20261110.md). La acción del encoder conserva el mismo servicio; la limpieza ya no es exclusivamente local. Guardas adicionales de PWM cero y ausencia de Hall/secuencia activa. Solo fuente.

# AFR2-20261110-10 · Fallos físicos, radar y batería

Versión: 1.18.0 (subversión). Entrega solo en fuente, seguida por 1.19.0 y 1.20.0 en el mismo workspace.

Estado → Fallos conserva ScreenCode 030007. Cabecera, tres líneas de datos y una acción seleccionable en el pie: girar elige Limpiar fallo / Detener / Volver, corto confirma. El contenido distingue ausencia de fallo, F1 lazo demorado, F2 Hall/odometría, F3 rueda sin pulsos y F4 signo cruzado. Limpiar requiere salidas detenidas y un fallo activo; usa Afr2Product_ClearFault, muestra resultado y no inicia marcha. Largo conserva retorno contextual y los diálogos Sí/No vigentes. La limpieza sigue siendo local: no se amplían permisos remotos ni wire.

Radar 030031 y editor compacto 030033 muestran un cono nominal de 15° centrado en head.angle, también con PWM OFF. El último ángulo comandado no mide la orientación física con el servo sin alimentación. Se conserva OFF/FIJO/AUTO y el punto de eco reciente con su ángulo registrado; no se inventa eco cuando falta. El resto de la rejilla permanece punteado.

La batería ya admitía 8,5–9 V: ADC10 con referencia nominal de 5 V, divisor 10k/10k y escala de 10 V de batería. 9 V corresponden nominalmente a 4,5 V en la entrada analógica A5. La adquisición permanecía desactivada al arrancar hasta pedir una medición manual. Product_Init ahora inicia la adquisición real. El icono sigue desconocido hasta obtener muestras válidas y recientes; no se reemplaza por un voltaje inventado ni se oculta saturación. No se cambian química, calibración ni bandas de barras. Esto corrige el caso de adquisición no iniciada; no acredita la causa física de cualquier lectura desconocida restante.

AVR: afr2_menu_catalog.c, afr2_app.c, afr2_product.c y afr2_sensor_screens.c. Web vinculada: firmwareRenderer.ts, reformCatalogue.ts, firmwareSensorRenderer.ts, uiSimulation.ts y BatteryStatus.tsx. Los datos de la biblioteca local son ficticios y el visor conectado conserva el framebuffer del AVR. Sin nuevos framebuffers ni campos de snapshot; tamaño de flash/pila no medido.

Hashes de catálogo y renderer de sensores registrados después del port C/TS. Fuentes/glifos existentes conservados. Regeneración de firmwareLibrary.ts pendiente de autorización; no se alteró su recurso generado en esta entrega. Los overrides autorales Web cubren nombre, acciones y cantidad de filas. ESP conserva el transporte y solo comparte versión. No se ejecutaron tests, compilaciones, validadores, generadores, navegador, puertos ni flasheos. No hay evidencia de paridad o funcionamiento físico para esta revisión.
