# AFR2-20261024-09 — Biblioteca OLED vinculada ATmega/Web

24/10/2026. El usuario pidió comprobar la alineación y presencia de pantallas/
ScreenCodes en Web, conservar una copia de la biblioteca ATmega y modificar la
skill para que cada pantalla se entregue en ambos módulos. Preguntó además por
la dirección I2C. La autorización previa de compilar/corregir la integración se
aplica a este build; no autoriza tests, validadores independientes ni hardware.

## Brecha y cambio

El visor remoto consumía framebuffer del catálogo ampliado, pero su metadata
seguía limitada a cuatro pantallas. El fallback JavaScript además conservaba
layouts iniciales que ya no correspondían al firmware actual.

Web consume directamente `documentation/screens.json`:59identidades,48nodos y
11avisos, con normalización de códigos de6/8cifras. Se retiran los renderers y
la navegación de cuatro pantallas antiguas. `firmwareRenderer.ts` copia los
layouts C actuales de menús/vistas, avisos genéricos/red, teclado, edición y
secuencias. `oledRasterizer.ts` copia las primitivas actuales de píxeles, XOR,
contorno, relleno, texto/ventanas. Fuente, catálogo,14XBM y recursos de secuencia
se extraen literalmente del AVR activo durante build/inicio, con autorías y
hashes. No se añaden dependencias. Los algoritmos se portan/revisan manualmente;
el build se detiene si cambia un origen revisado o falta un código compartido.

El visor conectado exige framebuffer remoto1024bytes: no reconstruye una
pantalla inventada desde datos parciales ni navega de forma optimista.
La vista local de biblioteca permite consultar diseños de menús estáticos;
no ejecuta comandos ni presenta telemetría/avisos como datos reales. El renderer
completo requiere snapshot preparado, UI/reloj y buffer anterior equivalentes
al AVR. Cadenas de bytes del AVR/ASCII, no Unicode arbitrario. Comparación
diferencial de resultados AVR/JS todavía no ejecutada.

Skill `afr2-screen-authoring` actualizada junto con referencia y plantilla:
cada alta/modificación debe entregar ATmega + contraparte Web + registro
compartido en la misma tarea. Compilar/promover solo con autorización vigente;
copiar a ESP/data/dist no equivale a flashear. No se ejecutó quick_validate de
skill-creator: el proyecto y el usuario restringen validadores independientes.

## Dirección OLED y límites

`AutoFinalRobo2-Atmega/src/main.c:12..14` define `AFR2_OLED_ADDRESS7` como `0x3c`
bajo `#ifndef`; la composición pasa esa dirección de7bits a Afr2Oled_Init.
Se puede sustituir al compilar con `-DAFR2_OLED_ADDRESS7=...`. No es detección de
bus ni confirma dirección/modelo del dispositivo físico. Firmware sin cambios09.

EX01v5, wire y permisos conservados. Captura de AP con clave y teclado protegida;
no se envían secretos a Web. Las páginas físicas de la OLED no se certifican
atómicas por conservar bytes de la captura. Menús completos y Volver/SW intactos.
Gates de memoria/Hall/calibración y CAD sin cambios.

## Estado por módulo

| Módulo | Estado real |
| --- | --- |
| ATmega | Fuente sin cambios09; producto/communication/library compilados08. OLED/dirección física sin comprobar. |
| ESP | Fuente sin cambios09; firmware candidato compilado08. Paquete Web09 copiado localmente a data/dist. |
| Web | Catálogo59, copia de biblioteca y vista local implementados; TypeScript/Vite/build:esp completados, ver evidencia09. |
| Contracts | Catálogo/flujo/consumidor, skill, plantilla, informes, hallazgo y procedencia actualizados. |

[Build/manifest/tamaños](../documentation/builds/2026-10-24-09/README.md) y
[procedencia por archivo](../provenance/linked-oled-library-20261024.json).
Sin tests, validadores independientes, revisión visual, comparación diferencial,
pruebas físicas, reinicios, imagen LittleFS, flasheo, commit ni publicación remota.
