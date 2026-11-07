# Comparación raster C/TypeScript — AFR2-20261106-02



Ejecutar únicamente con comprobaciones autorizadas: `./tools/verification/oled-parity/run.ps1`. Requiere GCC nativo (`-Gcc` permite elegirlo), Node y dependencias Web instaladas. Genera resultados en `.tmp/oled-parity`; no accede a placas, red, puertos ni motores.



Compara los 1024 bytes de 2104 fotogramas: 70 nodos reales × 3 selecciones/progresos × 10 snapshots preparados, más cuatro payloads QR directos. Incluye redes vacías/buscando/fallo, dos páginas de 16 redes con candado, Hall con borradores, IR digital y QR de AP/Web con datos ficticios. La posición es inicial o final y el progreso 0/8/16 sectores. El shim PROGMEM es exclusivamente de host; no se enlaza al firmware.



No prueba todos los estados posibles ni navegación, entrega I2C, memoria AVR, sensores, HTTP o transporte WS. El nodo virtual Web 70 se representa mediante su editor, no es un nodo real adicional del catálogo AVR. Incluye avisos AP/listo, QR de 220 bytes y rechazo de contenido fuera de capacidad; no incluye parciales de scroll, todas las notificaciones ni tiempos reales. Cambiar un fixture exige modificar sus dos representaciones para mantener el mismo snapshot.


AFR2-20261106-04 amplía los fixtures; los originales de 02 permanecen incluidos. El prefijo de aviso 16 reproduce AFR2_NOTICE_NETWORK del header AVR (macro, no enum generado).
