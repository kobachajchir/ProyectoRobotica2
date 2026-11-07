> **Actualizacion de fuente:** se implemento AFR2-20261106-01; ver [entrega y pendientes](../changes/2026-11-06-01-coordinated-corrections.md) y [aceptacion no ejecutada](../documentation/acceptance/2026-11-06-01.md). El diagnostico y las observaciones originales que siguen se conservan como antecedente.

# Revisión del auto y plan de correcciones

Resumen de la conversación de pruebas del 5–6 de noviembre de 2026. Las observaciones físicas fueron comunicadas por el usuario; no fueron reproducidas por el agente. Este documento registra síntomas, pedidos y prioridades: no acredita correcciones, ensayos ni despliegue. No contiene credenciales.

## 1. Sensores de línea de cinco canales

El usuario identifica el módulo instalado como OSOYOO Five Channel IR Tracker. El fabricante documenta salidas digitales y un chip 74HC14D; no se verificó visualmente la inscripción de la unidad instalada. El preset ajusta sensibilidad, no proporciona una salida analógica gradual.

- Observado por el usuario: valores próximos a 5–6; sin cambio al ajustar el preset y sin cambio de los LED al pasar entre blanco y negro.
- Código revisado: el perfil predeterminado usa ADC10 en A4..A0, con rango 0..1023.
- Pendiente: alinear adquisición, configuración persistida y representación con el módulo digital. Mostrar claramente el estado de cada canal mediante cuadros activos/inactivos.
- Diagnóstico separado: leer por ADC una salida digital no explica por sí solo que los LED propios del módulo no cambien. Revisar por separado alimentación, conexiones, distancia, superficie y ajuste, sin declarar averiado el módulo por esos valores.

## 2. HC-SR04

El usuario informa que lo puso en prueba y no funciona. No quedó registrado qué resultado mostraba, si se movía el cabezal ni qué lectura entregaba. Causa sin determinar; no atribuir aún a sensor, servo, adquisición o presentación.

## 3. Acceso Web, AP y WebSocket

- Después de reiniciar, el usuario observa modo AP y el navegador agota la espera sin recibir página. No quedó confirmada la dirección exacta utilizada ni el firmware instalado.
- Diseño actual: el servidor HTTP entrega la misma Web por AP o STA. Cada interfaz puede tener una IP diferente; el cliente debe estar en la red correspondiente.
- Código revisado: si el servidor funciona pero falta o se rechaza el paquete Web, la raíz responde con un error HTTP y una página de diagnóstico. Una espera agotada obliga a revisar primero conectividad, dirección, protocolo HTTP y disponibilidad del servidor.
- Riesgo de despliegue conocido: el paquete nuevo tiene 17 activos; el firmware anterior admite 16. La ampliación a 24 está en fuente, sin evidencia de carga de ese firmware. No se confirmó que sea la causa del síntoma observado.
- WS: sigue pendiente el diagnóstico/corrección del bucle inicial. Permanecen cola de cuatro mensajes, cierre por consumidor lento al responder con cola llena, consultas iniciales concurrentes y reinicio temprano del contador de reconexión. Son condiciones compatibles con el fallo, no una captura de su causa real.
- Control táctil: implementado en fuente; funcionamiento físico no confirmado. Requiere enlace Web–ESP–ATmega disponible, permiso de motores y modelo listo.

## 4. Configuración y modo Wi-Fi

- Mostrar el modo actual en el encabezado.
- Evitar acciones ambiguas como «Conectar» sin explicar a qué red o sin borrador válido.
- El usuario informa «No disponible, falta borrador», después aceptación/conectando y finalmente operación fallida. Falta correlacionar cada aviso con su operación y estado final real.
- Pedido: al confirmar un cambio de modo, persistir la configuración y reiniciar el ESP para arrancar en el modo elegido. Distinguir el modo de crear AP del modo de conectarse como estación.
- Permitir configurar la conexión desde la Web servida por el AP.
- Al recibir un evento real de inicio/reinicio del ESP estando en Wi-Fi, salir de ese contexto al menú principal y descartar estados visuales obsoletos.
- Credenciales proporcionadas para futuras pruebas: no copiadas aquí ni a fuentes. No se aplicó configuración de red durante esta revisión.

## 5. Búsqueda y lista de redes

- Al iniciar la búsqueda debe aparecer «Buscando redes».
- No anunciar «Redes encontradas» si el resultado está vacío o todavía no llegó.
- Mostrar la lista correspondiente al resultado terminado; el usuario observa que inicialmente aparece solo «Volver», mientras que un nuevo refresco sí presenta opciones.
- Al finalizar sin resultados, mostrar el estado vacío y la acción «Refrescar».
- La red esperada no aparece aunque el router está cerca. Proximidad no confirma compatibilidad de banda, visibilidad ni éxito del escaneo; causa pendiente.
- El candado desaparece sobre la selección blanca: invertir su raster junto con el resto del elemento seleccionado para conservar contraste.

## 6. Conteo de pulsos y PWM

Pedidos recogidos durante la conversación:

- Conteo de pulsos: «Rueda» como selección inicial, acciones visibles según el estado y cierre con decisión de guardar o descartar.
- PWM: revisar el sentido observado al aplicar valores positivos y su correspondencia con la referencia del auto. No se identificaron aquí rueda, maniobra ni cableado suficientes para resolverlo con una inversión global.

No se reprodujeron estas observaciones ni se modificaron los mandos durante esta revisión.

## 7. Encoder y acciones rápidas

- Indicador de pulsación larga abajo a la derecha, con el icono existente y un círculo que se llena mientras se mantiene el botón.
- Contorno legible e invertido respecto al fondo, incluso sobre selección.
- Conservar la acción larga al soltar y la confirmación obligatoria de cambios pendientes.
- El usuario informa mucha demora al renderizar acciones rápidas. Revisar adquisición de estado, frecuencia de redibujado, transferencia OLED y renderer antes de atribuir el retraso a una causa concreta.

## Plan priorizado

| Prioridad | Trabajo | Criterio de cierre |
| --- | --- | --- |
| 1 | Identificar firmware/paquete instalados y recuperar HTTP por AP y STA | La IP de cada interfaz responde y entrega la Web o un diagnóstico explícito |
| 1 | Corregir y verificar el arranque WS | Primera conexión y reconexiones sin bucle; distinguir cortes WS de pérdida de UART |
| 1 | Alinear el perfil de línea con el módulo digital | Cinco estados coherentes en firmware, OLED y Web; investigar aparte los LED sin transición |
| 1 | Revisar HC-SR04 y sentido PWM | Resultado de prueba identificable y dirección por rueda conforme a la referencia definida |
| 2 | Unificar cambio de modo, persistencia y reinicio Wi-Fi | Estado final real visible, sin borradores ambiguos; retorno al menú tras reinicio |
| 2 | Corregir búsqueda, lista vacía y refresco | Lista del escaneo terminado y avisos acordes con su resultado |
| 2 | Ajustar conteo y guardar/descartar | Flujo contextual sin pérdida silenciosa de cambios |
| 3 | Corregir candado e indicador de pulsación larga | Iconos legibles y progreso coherente con el umbral configurado |
| 3 | Investigar lentitud de acciones rápidas | Causa identificada y respuesta de navegación comprobada tras el cambio |

Los cambios OLED y su simulador deben realizarse juntos conforme a las skills locales de pantallas. Compilaciones, validadores, pruebas físicas y flasheos requieren pedido expreso; este plan no los inicia. La ampliación ESP/paquete y el firmware ATmega deben desplegarse de forma coherente antes de declarar listo el control táctil.

## Cotejo entre módulos y archivos responsables

Revisión estática sobre la base integrada de `origin/main` en `39df23f`, con cambios locales existentes conservados. No se ejecutaron builds, tests, validadores, consultas al robot ni flasheos. Los enlaces siguientes apuntan a fuentes propias; `reference/` no se considera implementación vigente.

### IR: soporte digital existente y migración que selecciona ADC

- [afr2_sensors.c](../../AutoFinalRobo2-Atmega/src/platform/afr2_sensors.c): `line_kind=1` lee PINF en A4..A0 como 0/1; `line_kind=2` usa ADC. La inversión por canal y el filtro mayoritario de tres muestras ya existen. La validez para seguimiento también exige calibración; la lectura de diagnóstico tiene una condición de validez diferente.
- [afr2_product.c](../../AutoFinalRobo2-Atmega/src/services/afr2_product.c): predeterminado `line_kind=2`, inversión de los cinco canales.
- [afr2_preferences.c](../../AutoFinalRobo2-Atmega/src/platform/afr2_preferences.c): la migración de imágenes anteriores a versión 6 cambia a ADC un perfil que no estuviera ya en 2. Cambiar únicamente el predeterminado no corrige todas las configuraciones guardadas. La futura corrección necesita una política explícita para conservar calibraciones y reconocer este hardware.
- [afr2_app.c](../../AutoFinalRobo2-Atmega/src/services/afr2_app.c): cambiar el tipo invalida calibración. Revisar esa política junto al perfil, sin eliminar los bloqueos de seguimiento por accidente.

Conclusión: no hace falta inventar un driver digital; existe un desajuste entre el módulo identificado y la configuración predeterminada/migrada. No confirma por sí solo el contenido de EEPROM de la placa actual.

### Wi-Fi: cambio temporal de modo y configuración persistida son caminos distintos

- [wifi_service.cpp](../../AutoFinalRobo2-ESP/src/wifi_service.cpp): `changeMode()` cambia el modo activo y el AP en RAM; no escribe configuración ni reinicia. `stage()` sí guarda cuando `persist` está activo y programa la aplicación unos 350 ms después; aplica en marcha, sin reinicio automático.
- [broker.cpp](../../AutoFinalRobo2-ESP/src/broker.cpp): `wifi.mode` confirma el cambio mediante `changeMode()`. `wifi.configure` devuelve aceptación y reconexión esperada, que no equivale a conexión terminada.
- [broker_wifi.cpp](../../AutoFinalRobo2-ESP/src/broker_wifi.cpp): coordina borradores, peticiones OLED, resultados de conexión y reinicio explícito. La fase de conexión puede terminar en fallo por estado de la estación o por plazo de 25 s; falta identificar cuál ocurrió en el ensayo relatado.
- [afr2_app.c](../../AutoFinalRobo2-Atmega/src/services/afr2_app.c): «Conectar» está bloqueado sin borrador y «Reintentar» se declara no disponible en AP puro. Es consistente con «Falta borrador», pero no satisface el flujo contextual pedido.
- [afr2_network.c](../../AutoFinalRobo2-Atmega/src/services/afr2_network.c): el evento de inicio ESP genera una notificación y cancela la captura activa; ese bloque no navega al menú principal. Hay que añadir el retorno contextual solicitado, conservando confirmaciones de cambios pendientes.
- [WifiPage.tsx](../../AutoFinalRobo2-Web/src/features/wifi/WifiPage.tsx) y [ConnectionContext.tsx](../../AutoFinalRobo2-Web/src/features/connection/ConnectionContext.tsx): reflejar el modo, sus transiciones, aceptación y resultado final sin confundirlos con un reinicio ya completado.

### Escaneo: truncamiento y estados vacíos

- `WifiService::takeScan()` conserva como máximo 16 resultados y marca `truncated` cuando el SDK devuelve más. No ordena explícitamente por señal en ese bucle.
- `Broker::scanForOled()` conserva como máximo 8 resultados para el ATmega. Una red presente más allá de esas ocho entradas no aparecería en OLED; es una explicación posible de la ausencia relatada, todavía sin comprobar.
- El ESP transmite las entradas OLED antes del resultado terminal. [afr2_app_wire.c](../../AutoFinalRobo2-Atmega/src/services/afr2_app_wire.c) recibe cada entrada y luego, al éxito, anuncia «Redes listas» y abre la lista. La revisión no demuestra que el éxito se envíe antes de los datos; no atribuirle esa causa sin evidencia.
- Si el escaneo termina correctamente con cero entradas, igualmente se genera el aviso de éxito y la lista tiene solo «Volver». Esto explica un camino real para el estado vacío confuso, aunque no acredita que haya sido el del ensayo.
- [afr2_menu_catalog.c](../../AutoFinalRobo2-Atmega/src/ui/afr2_menu_catalog.c) mantiene «Redes encontradas» y «Refrescar» como opciones estáticas. El pedido exige representar por separado no iniciado, buscando, terminado vacío, terminado con resultados y fallo.

### Candado: defecto identificable en el redibujado parcial

En [afr2_screens.c](../../AutoFinalRobo2-Atmega/src/ui/afr2_screens.c), el dibujo completo pinta el candado y después invierte toda la fila seleccionada. El parcial vuelve a pintar el candado con el dibujo normal sobre la fila ya invertida y solo limpia/invierte el área de texto, que excluye el candado. Es un camino concreto compatible con el blanco sobre blanco informado.

La misma secuencia existe en [firmwareRenderer.ts](../../AutoFinalRobo2-Web/src/features/oled/firmwareRenderer.ts). Corregir ambos renderers y revisar el registro de fuentes; no limitarlo a CSS ni actualizar hashes sin cotejo.

### Acciones rápidas: redibujado frecuente, causa de latencia aún no medida

- `Afr2Ui_Tick()` en `afr2_screens.c` solicita redibujado parcial de rápidas cada 40 ms. Las notificaciones también mantienen redibujado periódico; los cambios de firmas pueden solicitar dibujo completo.
- [main.c](../../AutoFinalRobo2-Atmega/src/main.c) ejecuta render y envío OLED en el bucle de servicios. Da oportunidades de envío cada 2 ms aun con actividad UART/encoder; no hay una espera bloqueante explícita de 40 ms en ese camino.
- [afr2_oled.c](../../AutoFinalRobo2-Atmega/src/display/afr2_oled.c) transmite por fragmentos, conserva datos confirmados y reintenta tras fallos. La frecuencia de render, costo de raster y avance real del I2C requieren distinguirse antes de corregir; no se midieron aquí.

### Encoder, Hall, PWM y HC-SR04

- [afr2_encoder_ui.c](../../AutoFinalRobo2-Atmega/src/input/afr2_encoder_ui.c): antirrebote de 20 ms, umbral inicial de 800 ms y clasificación corto/largo al soltar. El indicador circular no debe adelantar la acción. [EncoderControl.tsx](../../AutoFinalRobo2-Web/src/features/oled/EncoderControl.tsx) ya calcula progreso visual de pulsación; eso no implica que el raster OLED tenga el indicador pedido.
- Hall: `afr2_screens.c` separa selección/conteo y vuelve a la pantalla de prueba tras finalizar. `afr2_app.c` conserva borradores por rueda y guarda/calibra mediante el flujo de preferencias. Adaptar selección inicial y acciones sin perder los borradores completados ni saltarse Sí/No.
- [afr2_motor.c](../../AutoFinalRobo2-Atmega/src/platform/afr2_motor.c): el signo final depende del valor solicitado y de una máscara de inversión por rueda. Verificar además referencia de movimiento, perfil guardado y cableado antes de invertir todas las salidas. Ninguna prueba de giro ejecutada.
- HC-SR04: `afr2_sensors.c` distingue captura de eco, timeout y muestra invalidada por movimiento del servo. Revisar habilitación, temporización, error informado y asociación con la vista de prueba antes de concluir fallo del sensor. Sin evidencia física adicional.

### Alcance y situación del Loader

El Loader es relevante para identificar artefactos y desplegar una futura corrección, no como causa demostrada de los síntomas físicos. Su reubicación de comprobaciones está documentada aparte en [AFR2-20261105-01](../changes/2026-11-05-01-loader-checks-sidebar.md), solo en fuente. No se compiló ni reinició la interfaz modificada, ni se iniciaron trabajos MCP o COM durante esta revisión.

CAD no necesita modificaciones para los defectos de software identificados. Si la revisión física descubre distancia/montaje incorrecto de IR o HC-SR04, se incorporará entonces el módulo CAD con medidas verificadas.
