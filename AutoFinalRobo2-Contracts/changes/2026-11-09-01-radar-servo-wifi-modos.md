# AFR2-20261109-01 - Radar en vivo, servo centrado, avisos Wi-Fi y cambio de modo en el ESP

Pedido del usuario (09/11/2026):

1. Radar: encenderlo enciende servo y HC-SR04; apagar cualquiera lo apaga.
2. El AP no se levantaba solo cuando correspondía. Mostrar "Iniciando AP" / "Reconectando a" mientras
   ocurre. Cambiar de modo (STA, AP, AP+STA) desde el auto debe hacerse solo, sin pedir detener la red.
3. Servo centrado (90°) al arrancar, como referencia de posición.
4. La pantalla Radar no graficaba ni actualizaba aunque la pantalla Distancia mostraba datos. Mostrar el
   cono del servo, el punto en tiempo real y refrescar el valor.
5. Generar el firmware nuevo.

Versión: 1.8.0 (subversión).

## Causas y cambios

### Radar que no actualizaba (ATmega + Web)
- Desde 1.5.0 el HC-SR04 solo mide con el interruptor Ultrasonido, el radar en marcha o con la pantalla
  Distancia (2/2) abierta. La pantalla **Radar (1/2)** y **Ángulo del servo** no pedían mediciones: sin
  ecos nuevos, sin punto y con el valor congelado.
- Ahora esas dos pantallas también piden mediciones mientras están abiertas (`Afr2Head_SetView`).
- El valor inferior del radar muestra `--` si el último eco tiene más de 1 s.
- Distancia (2/2): la tercera línea muestra hacia dónde apunta el servo (`Servo 090 70ms` / `Servo OFF`).

### Servo centrado y cono visible (ATmega + Web)
- `Afr2App_Init` centra el servo (`Afr2Head_Servo(2,90)`) al arrancar. Con el servo activo, el radar
  dibuja el cono de ±7,5° hacia donde apunta y el punto de cada eco en vivo.
- La tarjeta del dashboard y las rápidas muestran el servo encendido desde el arranque.

### Radar acoplado (ya en 1.7.0, AFR2-20261108-13)
- Encender el radar enciende servo y HC-SR04; apagar el servo o el HC-SR04 detiene el radar. Requiere
  cargar este firmware (o el 1.7.x) para verse.

### Wi-Fi (ESP + ATmega)
- **Cambio de modo en el ESP** (`WifiService::changeMode`): fallaba si quedaba el resultado de una
  búsqueda sin leer o si el modo no tenía red guardada (AP sin red AP propia). Ahora descarta la búsqueda
  terminada y, sin red guardada, usa el AP abierto `AutoFinalRobo2-xxxx` (AP) o la red por defecto (STA,
  AP+STA). El ESP guarda el modo y se reinicia para aplicarlo, como antes. El ATmega ya no exigía detener
  la red desde 1.7.1.
- **AP automático**: si la STA no conecta, el ESP pregunta al OLED («Sin red Wi-Fi»). Si la pregunta no
  tiene respuesta en 20 s (aviso perdido, pantalla ocupada…), ahora levanta el AP igual.
- **Avisos en el OLED** (ATmega): «Cambiando Wi-Fi / a AP, reinicia» al elegir modo; «Wi-Fi / Iniciando
  AP» al elegir Iniciar AP o cuando el ESP informa modo AP sin AP activo; «Reconectando a / <red>»
  cuando informa STA sin conexión. Se detectan con el estado de red que el ESP ya envía (sin trama
  nueva).

## Estado

atmega=compilado sin errores (advertencia previa en `afr2_product.c:214`), flash 150720 B (57,5 %),
SRAM 7017 B (85,7 %); sin cargar. esp=compilado con PlatformIO `esp12s_candidate` sin errores, RAM
62,1 %, flash 43,7 %; sin flashear. web=fuente editada (pantallas de sensores y simulación), sin build.
loader=sin cambios. Sin pruebas en placa.
