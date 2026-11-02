# QRScreen OLED128-5

Crear un QR de contenido elegido y convertirlo en bitmap con dimensiones y
polaridad explícitas. No generar la imagen del QR por IA ni reconstruirlo a
ojo. Se usa QR Code Model2 con encoder local de Project Nayuki, licencia MIT
conservada en `scripts/vendor/qrcodegen.py`. No envía contenido a ningún sitio.

## Espacio que necesita un QR

El símbolo requiere una zona clara de **cuatro módulos por cada lado**, libre
de texto, marco o iconos. No confundir un módulo con un píxel: a escala2 el
margen es8px, además de cualquier padding exterior.
[Fuente: DENSO WAVE](https://www.qrcode.com/en/howto/code.html).

La versión1 tiene21×21 módulos y cada versión siguiente agrega4 por lado.
La capacidad depende de versión, tipo de datos y corrección de errores.
[Fuente: DENSO WAVE](https://www.qrcode.com/en/about/version.html).

Para una caja62×62, la geometría es:

```text
modulos_simbolo = 17 + 4 * version
modulos_con_margen = modulos_simbolo + 8
escala = floor(62 / modulos_con_margen)
lado = modulos_con_margen * escala
```

| Símbolo | Con margen4 | Escala entera que cabe | Tamaño resultante |
| --- | --- | --- | --- |
| V1,21módulos | 29módulos | 2px/módulo | 58×58 |
| V2,25módulos | 33módulos | 1px/módulo | 33×33 |
| V3,29módulos | 37módulos | 1px/módulo | 37×37 |
| V9,53módulos | 61módulos | 1px/módulo | 61×61 |
| V10,57módulos | 65módulos | no cabe | rechazar |

Perfil inicial: V1 con2px/módulo, campo62×62 incluyendo todos los márgenes.
El QR de58px se centra en ese campo con2px claros extra a cada lado; el campo
se coloca en(1,1), dejando1px exterior arriba/abajo en la OLED. No estirar
58 a62 por interpolación. El bloque completo es62, no el patrón negro.

Para contenido mayor, acortar el texto o usar una URL/identificador corto que
el usuario haya elegido. No inventar un acortador, servidor, dominio operativo
o servicio externo. Una URL codificada no implica que sea accesible desde el
teléfono: red, nombre y disponibilidad se documentan por separado.

La variante de1px/módulo requiere elección explícita (`--min-module-px 1`).
Puede alojar V2..V9 según contenido, pero es más densa y no tiene evidencia de
lectura física. No bajar de2 a1 silenciosamente. La corrección M es el mínimo
inicial; se puede elevar dentro de la misma versión sin aumentar dimensiones.
No reducir ECC para encajar sin que el usuario lo elija con `--ecc`.

## Layout especial

QRScreen ocupa la altura completa: excepción explícita a Header/Footer globales.

```text
+--------------------------------------------------+
|                                                  |
| [ campo QR 62x62 ]    titulo breve                 |
|                      detalle                     |
|                      detalle                     |
|                      [encoder largo] [atras]      |
+--------------------------------------------------+
```

Campo QR x1..62,y1..62. Gap4px y columna derecha x67..126. Título en y3,
dos líneas opcionales en y20/y32; máximo8 caracteres7×10 por línea. Contenido
del QR y rótulos son independientes: no imprimir todo el payload al costado.
Ayuda de retorno a derecha en el pie habitual, íntegra en x94..125. No invadir
el campo QR aunque la cabecera general de la familia sea invertida.

Módulos oscuros = píxeles apagados sobre campo claro encendido. No usar el
QR en negativo como decoración del dashboard. El fondo de la OLED puede ser
negro; el cuadrado claro debe incluir quiet zone completa. No marco interior,
logo, esquinas redondeadas de módulos, recortes o texto sobre el QR.
La paleta del preview QR es blanco/negro, no un tratamiento de neón.

Mantener estático mientras se escanea. Preparar toda la matriz antes de
mostrarla; no cambiar contenido, hacer marquesina o parpadear módulos. La
actualización física por páginas puede ser transitoria; no prometer atomicidad
del display existente. Largo retorna al padre según el perfil nuevo; salir
de la pantalla no ejecuta el contenido QR ni representa una confirmación del
teléfono. El escaneo ocurre en la cámara/app del usuario.

## Generador local reutilizable

Desde la carpeta de la skill:

```powershell
python scripts/generate_qr.py --text 'AFR2:DEMO' --output-dir assets/qr-examples/demo --title 'INFO' --line 'Escanea' --line 'DEMO'
```

Otro contenido, URL o texto en archivo UTF-8:

```powershell
python scripts/generate_qr.py --input-file 'D:/mi-proyecto/contenido-qr.txt' --output-dir 'D:/mi-proyecto/recursos/qr' --title 'WEB' --line 'Escanea'
```

El archivo conserva espacios y saltos de línea; no aplicar `strip`, convertir
a mayúsculas o agregar URL por conveniencia. El modo alfanumérico/números solo
se usa cuando el texto exacto lo permite. Para caracteres noASCII se registra
ECI26 y bytesUTF-8. La fuente de los rótulos OLED sigue siendo ASCII actual:
esto no restringe los caracteres codificados dentro del QR.

Parámetros: `--ecc L|M|Q|H`, `--min-module-px 1|2`, `--name identificador_C`.
`--replace` autoriza reemplazar salidas ya existentes. Sin esa opción, elegir
otra carpeta si hay artefactos del mismo nombre. Si el contenido no cabe, el
generador informa versión/escala y no trunca ni genera un bitmap incompleto.
No propone QR animado/partido, MicroQR o simbología menos compatible como
sustituto automático para la cámara habitual de un teléfono.

## Archivos producidos

| Archivo | Uso y codificación |
| --- | --- |
| `qr.png` | campo nativo62×62, negro sobre blanco |
| `screen.png` | composición nativa128×64 |
| `screen-preview.png` | vista ampliada6× por píxeles enteros |
| `qr-onbits.h` |496bytes: filas, stride8, LSB primero, bit1=OLED encendido |
| `screen-pages.h` |1024bytes: framebuffer vertical por páginas, bit1=encendido |
| `qr-metadata.json` | versión, ECC real, máscara, escala, márgenes y procedencia de estado |

No confundir formatos: en filas el byte es `y*8+x/8`, bit `x%8`; en pantalla
es `x+128*(y/8)`, bit `y%8`. Bits sobrantes de cada fila van en0. Los headers
son independientes, con datos PROGMEM en AVR y constantes en otras plataformas;
no sumar ambos buffers a SRAM. Guardar bitmap en flash no determina memoria
necesaria para generar QR dinámico dentro del MCU.

Para dibujar qr-onbits en un framebuffer existente, leer **todos** los bits y
asignar blanco/negro, incluidos claros y margen. En AVR usar `pgm_read_byte`.
No pasarlo al renderer de iconos, que dibuja solo bits encendidos y reescala:
los ceros del QR deben apagar píxeles y la escala tiene que conservarse.
No copiar screen-pages directamente sobre `Afr2Oled.buffer` sin marcar páginas
dirty a través de la API apropiada o adaptador dueño; no se implementa esa
integración en esta entrega.

La biblioteca genera los módulos, con versiones/ECC y máscara automática;
el adaptador empaqueta los píxeles y compone la pantalla.
[Documentación de Project Nayuki](https://www.nayuki.io/page/qr-code-generator-library).
Copiada desde commit documentado en `scripts/vendor/PROVENANCE.md`; no instala
paquetes ni ejecuta descargas al generar. El contenido no se imprime en logs
o metadatos, aunque evidentemente es recuperable del propio QR.

## Elegir información y límites de entrega

Recibir el texto/URL que el usuario quiera codificar o su archivo de origen.
Si falta, crear solo un ejemplo identificado y dejar la herramienta lista,
sin elegir una URL real por el usuario. WIFI/vCard/texto largo pueden exigir
versiones mayores: informar capacidad, no asegurar que cualquier dato cabe.
No trasladar credenciales a capturas públicas o al visor conectado por defecto.

Esta tarea incorpora skill, generador y ejemplo, no un ScreenCode activo ni
generación dinámica en el ATmega. Si luego se solicita integrar una pantalla
de producto, entregar AVR y réplica Web juntas con identidad compartida y
contrato de datos/privacidad de la skill local.

Estado de escaneo: no se ejecuta un decoder ni se prueba un teléfono/OLED sin
pedido expreso. Respetar geometría y utilizar un encoder real permite crear
el código; no demuestra aún que una cámara concreta lo lea en la placa real.

## Adaptador AFR2 integrado el 02/11/2026

AFR2-20261102-05 incorpora generación C/TS en fuentes de producto, además del generador Python de esta skill. Leer `AutoFinalRobo2-Contracts/contracts/OLED_RUNTIME_QR_20261102.md` desde la raíz del proyecto. QRScreen reutilizable: `Afr2Qr_DrawPayload` y `renderQrPayload`. Usos activos en fuente: AP030045 y Web030046. Conserva geometría, quiet zone y contenido completo; versiones1..9, ECC Low con boost y máscara0. Para la generación MCU usa framebuffer temporal y composición posterior; tablas ECC en PROGMEM, auto/radar detenidos. No trasladar las muestras a datos reales ni asegurar lectura física sin ensayo. Este adaptador reemplaza la limitación histórica de entrega solo-diseño para AFR2; la skill general y sus ejemplos siguen siendo herramientas portables.
