# Política AP — AFR2-20261109-02

Fuente coordinada, versión1.9.0. Sustituye el submenú AP/fallback de WIFI_UI_11 y el timeout extra ESP de AFR2-20261109-01. No cambia framing, EX01v13 ni EEPROM AVR.

Modo guardado y política son independientes. AP manual/guardado no habilita STA ni escanea. Fallback por falla STA crea AP+STA y mantiene reintentos STA. askFallback=true habilita la pregunta; false crea AP automáticamente. Campo ausente=false. AP+STA elegido explícitamente sigue siendo un modo admitido.

UART ESP→ATmega WIFI_POLL(0x30), request offset:u8; reply existente pending:u8,job:u32LE,op:u8,total:u8,offset:u8,count:u8,requestedMode:u8,draftId:u32LE,bytes. Op15=guardar política, total1, byte ask(0/1). Op16=detener AP,total0. Op13=iniciar AP con interfaz STA solo si ya deseada; op14=espera segundos u16LE. Job/correlación y timeout local existentes. Políticas se aplican en ESP, no en el renderer ni optimistamente en AVR.

WIFI_UPDATE(0x31) tipo4 mantiene campos previos de18B (subtipo,modo,STA,seguridadSTA,seguridadAP,clientesWS,IP4,máscara4,gateway4); admite19B con askFallback:u8 en offset18. AVR acepta ambos; política ausente queda desconocida y la opción se deshabilita. Resultado tipo2 con job coincidente: estado3 operación finalizada, estado4 fallida. Estado y eventos de red existentes confirman AP activo/detenido. Los comandos siguen sensitive por compartir transporte con credenciales.

WebSocket local ESP: wifi.ap.fallback {ask:boolean} guarda en wifi.json sin reiniciar; wifi.ap.stop {} detiene AP sin modificar modo guardado. Respuesta wifi.status con confirmed:true solo tras éxito de servicio; fallo usa error. wifi.status incorpora askFallback:boolean y apReady:boolean. 4000ms, sesión Web negociada. Sin nueva asignación CMD UART.

Detener AP suprime fallback hasta aplicar modo, reiniciar o iniciar AP explícitamente. Apagar AP desde una Web conectada por ese AP puede cortar su respuesta; consultar el estado al recuperar enlace. STA habilitada sigue intentando reconectar.

Compatibilidad: fuente vieja no conoce op15/16 ni política; el nuevo AVR deshabilita política si recibe detalles viejos. No se presume compatibilidad entre todas las combinaciones ni despliegue. Integración, memoria y hardware sin verificar; entregar módulos coordinados al compilar/cargar bajo autorización.
