# AFR2-20261102-24 — Descriptores técnicos en Ayuda y página del Loader en Explorar

02/11/2026. Pedido humano:
- hacer más lindo y más técnico el texto descriptor de cada sección de Ayuda;
- agregar en Explorar información sobre el Loader (función, qué hace, por qué es útil), técnica y con el mismo estilo visual que las demás secciones.

Continúa AFR2-20261102-23.

## Ayuda: tarjeta de cada módulo

El párrafo suelto se reemplaza por una tarjeta con la familia clara/oscura `--infra-hero-*` de las cabeceras. Tiene:
- ícono de zona;
- rol: Controlador de tiempo real, Pasarela Wi-Fi y servidor, Consola en el navegador o Interfaz local del auto;
- nombre del componente;
- descripción técnica reescrita;
- cuatro celdas de especificaciones.

| Módulo | Especificaciones |
| --- | --- |
| ATmega2560 | AVR 8 bits a 16 MHz · 256 KiB flash, 8 KiB SRAM, 4 KiB EEPROM · C gnu11 + avr-libc · ROBO2 por USART1 a 115200 |
| ESP8266 · ESP-12S | Xtensa L106 a 80 MHz · 4 MiB de flash con 3 MiB de LittleFS · C++17 con PlatformIO · HTTP y WebSocket /ws |
| Web | 0.4.0 · React 19, TypeScript 5.9 y Vite 7 · Three 0.180, R3F 9 y Draco · afr2-ws-1 |
| OLED + encoder | SSD1306 128×64 por I2C · TWI 100 kHz · KY-040 en INT4/INT5 + D42 · largo ≥ 800 ms al soltar |

En celular el ícono pasa arriba y las celdas quedan en dos columnas. Con transparencia reducida o contraste alto se quitan la grilla y el desenfoque.

## Explorar → Loader (`/loader`)

Nuevo destino en el grupo Explorar, con ícono `arrow-down`; aparece en la barra lateral, en la hoja «Más» y en Inicio.

La página reutiliza el lenguaje de Infraestructura. Las primitivas Reveal, Wire, FlowNode y Stat se movieron sin cambios a `features/infrastructure/infraPrimitives.tsx`.

- **Portada:**
  - título «De la fuente a la placa, con cada byte verificado»;
  - descripción de la función del Loader;
  - chips: C# con WinForms y .NET Framework 4 · avrdude + esptool · versión 1.1.4;
  - mapa de tres carriles: En la computadora (Perfil JSON, Compilar) → AFR2 Loader (Backup, Escribir) → En las placas (ATmega2560, ESP8266);
  - cada nodo abre su etapa en el recorrido.
- **Paso a paso,** con dos recorridos de cinco estaciones y una ficha de etapa (entra, sale, lo que conviene saber y fuente):
  - **Cargar el ATmega:** perfil y puerto → compilar → backup de flash y hfuse con firma 0x1E9801 y doble lectura → avrdude flash:w → verificación por lectura posterior.
  - **Cargar el ESP:** transporte (usb-ttl, mega_reset, mega_bridge) y capacidad con flash_id → preparar firmware y LittleFS (Prepare-Web.ps1 / build:esp) → modo de carga con KEY+RST, escucha a 74 880 baudios y read_mac → esptool write_flash → hash por imagen.
- **En cifras:** versión 1.1.4, 3 transportes ESP, 10 herramientas MCP y 14 operaciones, con su lista y cómo pedirlas por MCP.
- **Qué resuelve frente a cargar a mano:** cargas repetibles, nada a ciegas, guía física y automatizable por MCP stdio.
- **Herramientas incluidas:** monitor serie (64 KiB por envío, 1–1000 repeticiones, pausa de 0 a 60 s), conversor HEX/ASCII, flasher del Mega y manual integrado.

Fuentes de los datos:
- `AutoFinalRobo2-Loader/README.md`, `MCP.md` y `FILESYSTEM_SCRIPTS.md`;
- en `src/`: `McpServer.cs` (herramientas y operaciones), `LoaderVersion` (1.1.4), `Operations.MegaBackup.cs` (flash:r y hfuse:r dobles), `Operations.VerifyMegaFlasher.cs` y `Compilar.ps1` (csc de .NET Framework, winexe).

La página describe la fuente del Loader: no abre puertos ni realiza cargas.

## Adaptación por archivo

Web/src:
- `features/docs/helpContent.ts` y `features/docs/DocsPage.tsx`;
- `features/loader/LoaderPage.tsx` y `features/loader/loaderFlow.ts` (nuevos);
- `features/infrastructure/infraPrimitives.tsx` (nuevo) y `features/infrastructure/InfrastructurePage.tsx` (importa las primitivas);
- `app/App.tsx` (ruta diferida), `app/navigation.ts` (destino) y `styles/workspace.css`.

Contracts: CHANGELOG.md y README.md.

## Estado por repo

- **atmega / esp / loader:** sin cambios. Solo se leyeron sus fuentes.
- **web:** fuente modificada.
  - **Verificado en el navegador local:**
    - tarjeta de ATmega a 1440 px en claro;
    - tarjeta de ESP a 390 px en oscuro;
    - pestaña OLED en celular;
    - Loader completo a 1440 px (portada, cambio de recorrido a ESP, ficha, cifras y tarjetas) y su portada a 390 px sin desplazamiento horizontal;
    - Infraestructura sigue renderizando tras extraer las primitivas;
    - consola sin errores propios.
  - **No verificado:** compilación TypeScript y `build:esp`.
- Sin commit.
