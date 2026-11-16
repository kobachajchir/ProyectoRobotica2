# ADJUNTO 3 de 4 · contiene las fichas de las filminas 9 a 13.

===== INICIO DEL ARCHIVO: Filmina_09_Simulador_y_mision.md =====

# Filmina 9 · Simulador, misión y ciclo de recolección

**Mensaje clave:** antes de mover el robot real, la Web simula auto, brazo y conjunto con la misma matemática; la misión
«recolectar cubos» está definida paso a paso y cada fallo es un resultado posible.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Simulador conjunto y misión del recolector»        │
├──────────────────────┬───────────────────────────────────────────────────┤
│ A  CÓMO ESTÁ DEFINIDO│ B  GALERÍA DEL SIMULADOR  c5–c12 · f2–f4          │
│ el simulador         │  FOTO-33 · FOTO-34 · FOTO-35 · FOTO-36            │
│ c1–c4 · f2–f4        │  (4 capturas, grilla 2×2 o 4×1)                   │
├──────────────────────┼───────────────────────────────────────────────────┤
│ C  MISIÓN (reglas)   │ D  D-05 ciclo de la misión   c5–c9 · f5–f6        │
│ c1–c4 · f5–f6        │ E  FOTO-37/38/39 secuencia toma  c10–c12 · f5–f6  │
└──────────────────────┴───────────────────────────────────────────────────┘
```

## Contenido por zona

### A · Cómo está definido el simulador (`#/simulacion/auto | brazo | conjunta`)
- Escena 3D en el navegador (Three.js sobre el GLB del CAD); estado **local**: no se serializa como telemetría ni mueve el robot.
- **Auto:** ruedas simuladas con la cinemática Mecanum; arrastrar el auto con *Mover auto*.
- **Brazo:** 6 ejes seleccionables, controles a la derecha, pinza independiente, matrices homogéneas en vivo.
- **Conjunto:** `T_mundo,pinza = T_mundo,chasis · T_chasis,montaje · T_J1 … T_J6 · T_TCP`; sensor y depósito son ramas del chasis.
- Convención: X adelante, Y izquierda, Z arriba (Three: (X,Y,Z) → (−Y, Z, −X)); metros y radianes.
- Se detiene al cambiar de ruta, abrir un modal, perder foco o emitir STOP.
- Alcance como **nube de tomas verticales** muestreada cada 25 mm; trayectorias en pasos articulares ≤ 3°.

### B · Galería (imágenes del simulador conjunto, capturas ⌗)
| Foto | Qué mostrar |
| --- | --- |
| **FOTO-33** | Perspectiva: auto + brazo + 6 cubos (semilla 42) + depósito de 4 celdas |
| **FOTO-34** | Vista superior con alcance (nube de tomas) |
| **FOTO-35** | Vista lateral con el brazo en preagarre sobre un cubo |
| **FOTO-36** | Radar ideal: barrido de 37 direcciones y ecos |

Todas con el mismo encuadre, mismo tema (claro) y 1920×1080 recortadas a 16:10 (ver [FOTOS.md](FOTOS.md)).

### C · Misión (3–5 viñetas)
- Cubos de **50 mm, 100 g**, generador LCG con semilla repetible; 1…12 cubos (hasta 24 manuales), separados ≥ 150 mm; semilla inicial 42, seis cubos.
- **Depósito** de 118 × 118 mm con **4 celdas** en una capa (borde a Z = 190 mm).
- Sensor **virtual** a (103, 0, 45) mm (altura ajustable 20–200 mm): *montaje bajo propuesto*, no la ubicación del sensor real.
- Modelo de eco: rayo horizontal ideal contra cajas, sin ruido ni cono acústico; sin eco = nulo. El autónomo **no** recibe IDs ni centros del generador.
- Fallos explícitos: objetivo perdido · toma inalcanzable · trayectoria bloqueada · agarre fallido · depósito lleno.

### D · D-05 ([D-05_ciclo_mision_recolectora.svg](diagramas/D-05_ciclo_mision_recolectora.svg))
Recoger brazo → barrer → candidato → orientar/centrar → eco frontal reciente → aproximar → detener → medir otra vez → preagarre → tomar
→ elevar → llevar sobre celda → soltar → recoger brazo → ¿quedan cubos? La base **permanece detenida** mientras el brazo manipula.

### E · FOTO-37 / 38 / 39 (secuencia de toma)
Tres capturas consecutivas: preagarre, elevación/transporte, suelta en la celda. Pie común: *Simulación local: toma por IK vertical
(J4 = 0, herramienta hacia abajo; J6 alinea las mordazas con el cubo).* **FOTO-32** (esquema editable de montaje del recolector,
dibujo nominal) puede ir como apoyo.

## Guion del orador (≈ 90 s)

«El simulador usa el mismo modelo 3D y la misma cinemática que el robot. Podemos mover el auto, mover el brazo articulación por
articulación y ver las matrices; el conjunto compone ambas. Para la misión generamos cubos de 50 milímetros con una semilla
repetible. El auto detenido barre con un ultrasonido ideal, detecta un candidato, se orienta, se aproxima y vuelve a medir; luego el
brazo hace una toma vertical con cinemática inversa, levanta el cubo y lo deja en una de cuatro celdas del depósito. Cualquier falla —
objetivo perdido, inalcanzable, agarre fallido, depósito lleno— detiene la misión con su motivo. Es una simulación: sirve para
definir el ciclo y los criterios antes de probar físicamente.»

## Fuentes

`AutoFinalRobo2-Contracts/docs/ROBOT_RECOLECTOR.md` · `AutoFinalRobo2-Web/src/features/collector/*` (`collectorSimulation.ts`, `armKinematics.ts`) ·
`AutoFinalRobo2-CAD/mecanica/robot-recolector/propuesta_montaje.svg` · `changes/2026-11-11-01-pruebas-minimas-recolector.md`.

## Advertencias

- **Simulación, no misión física verificada.** El sensor real puede no ver un cubo desde su ubicación actual; HC-SR04 y servo siguen por validar en el auto.
- La IK es una rama específica de toma vertical, no un solver general; sin autocolisión BRep ni deformación.
- Valores 130/120 mm del texto del recolector son del v02 histórico; el brazo vigente es v17.


===== FIN DEL ARCHIVO: Filmina_09_Simulador_y_mision.md =====

===== INICIO DEL ARCHIVO: Filmina_10_Interfaz.md =====

# Filmina 10 · Interfaz: pasos y pantallas

**Mensaje clave:** dos interfaces gemelas —el panel OLED + encoder del auto y la Web— con la misma lógica de navegación, y un
recorrido de pasos claro para llevar el robot de «recién encendido» a «en marcha».

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Interfaz: OLED + encoder y Web»                    │
├────────────────────────────────┬─────────────────────────────────────────┤
│ A  PASOS DE USO (1→6)          │ B  PANEL OLED (6 pantallas)             │
│ lista numerada c1–c4 · f2–f4   │ FOTO-48…FOTO-52 + FOTO-53 · c5–c12 · f2–f3│
│                                ├─────────────────────────────────────────┤
├────────────────────────────────┤ C  D-07 navegación c5–c8 · f4–f6        │
│ D  WEB: 7 pantallas (mosaico)  │                                         │
│ FOTO-41…FOTO-47  c1–c4 · f5–f6 │ E  Reglas de entrada c9–c12 · f4–f6     │
└────────────────────────────────┴─────────────────────────────────────────┘
```
Opción de dos filminas de apoyo si hace falta más aire: «10a OLED» y «10b Web»; el contenido ya está separado en las zonas B/C/E y A/D.

## Contenido por zona

### A · Pasos de uso
1. **Encender** (llave del shield) → el dashboard OLED muestra modo, red y batería (010001).
2. **Conectar:** el ESP crea el AP `AutoFinalRobo2-<id>` (abierto si no hay red guardada) o se une a la red configurada (OLED: *Wi-Fi*).
3. **Abrir la Web** (IP en el OLED, o QR de la pantalla de red) → *Inicio* muestra los tres enlaces (WebSocket, Mega, control).
4. **Calibrar** (*Puesta a punto*): Hall por rueda (una vuelta), signos de motor, línea, odometría (prueba de distancia).
5. **Conducir** (*Conducir*): pad Mecanum táctil, prueba por rueda; STOP siempre visible.
6. **Programar** (*Programas*): GOTO, secuencias por rueda, seguidor de línea; **simular** en *Simulación* antes de enviar.

### B · Pantallas del OLED (fotos reales ✱)
| Foto | Pantalla | Código |
| --- | --- | --- |
| **FOTO-48** | Inicio (dashboard) | 010001 |
| **FOTO-49** | Menú principal | 010002 |
| **FOTO-50** | Estado → resumen / fallos | 030004 / 030007 |
| **FOTO-51** | Radar de distancia | 030031 |
| **FOTO-52** | Guardar cambios (Sí / No) | 030038 |
| **FOTO-53** | Silueta del autito (recurso 128×64) | — |

Fotos tomadas de frente, en oscuridad moderada, mismo ángulo; recortar al marco del módulo. Alternativa sin cámara: capturar el
**OLED virtual** de la Web (*Ayuda → OLED*), que renderiza el mismo framebuffer.

### C · D-07 ([D-07_navegacion_OLED_encoder.svg](diagramas/D-07_navegacion_OLED_encoder.svg))
Inicio → (corto) Menú → submenús; Inicio → (largo) Rápidas; largo desde el resto → retorno contextual; cambios pendientes → Guardar Sí/No.

### D · Pantallas de la Web (mosaico de 7 capturas ⌗)
**FOTO-41** Inicio · **FOTO-42** Conducir · **FOTO-43** Programas (GOTO) · **FOTO-44** Puesta a punto · **FOTO-45** Sensores ·
**FOTO-46** Conexión Wi-Fi · **FOTO-47** Ayuda con OLED virtual / árbol de pantallas. (Simulación y Modelos ya se vieron en filminas 2 y 9.)
Mismo ancho, mismo tema, sin datos personales (ocultar SSID/IP si es necesario).

### E · Reglas de entrada (cuadro)
| Gesto del encoder | Efecto |
| --- | --- |
| Giro | mover selección / cambiar valor (inversión configurable) |
| **Corto** (al soltar) | confirmar / alternar; en Inicio abre el Menú |
| **Largo** (≥ 800 ms, al soltar) | en Inicio abre Rápidas; en el resto, vuelve al contexto anterior |
| Volver | solo mediante el ítem *Volver* (**no hay botón menú**) |
| Cambios pendientes | diálogo **Sí/No obligatorio**; el largo no lo evita |

Cifras: 101 pantallas catalogadas; menú con 7 ramas (Estado, Wi-Fi, Sensores, Pruebas, Calibración, Sistema + modos).

## Guion del orador (≈ 90 s)

«La interfaz tiene dos caras con la misma lógica. En el auto, una pantalla OLED y un encoder con pulsador: girar mueve, un clic
corto confirma y un clic largo —al soltar— vuelve o abre las pantallas rápidas. No hay botón de menú, y los cambios sin guardar siempre
piden Sí o No. En el celular, la Web ofrece el mismo árbol, más control táctil Mecanum, programas GOTO, calibración guiada, sensores y
simuladores. El recorrido típico es encender, conectarse al punto de acceso, abrir la Web, calibrar Hall y signos, conducir y recién
entonces programar rutas.»

## Fuentes

`AutoFinalRobo2-Contracts/documentation/UI_SCREEN_FLOW.md`, `MENU_PLAN.md`, `NAVIGATION.md`, `screens.json` · `AutoFinalRobo2-Web/src/app/App.tsx` (rutas) ·
`AutoFinalRobo2-Web/README.md` · `AutoFinalRobo2-Contracts/AGENTS.md` (reglas OLED).

## Advertencias

- Las pantallas físicas se muestran como **fuente/OLED virtual**; la paridad física y la latencia no están acreditadas.
- Los pasos 4–6 requieren calibración real; presentar la secuencia como procedimiento, no como resultados.
- No mostrar claves Wi-Fi: la clave del AP aparece solo en el OLED local.


===== FIN DEL ARCHIVO: Filmina_10_Interfaz.md =====

===== INICIO DEL ARCHIVO: Filmina_11_Agregado_Loader.md =====

# Filmina 11 · Nuestro agregado: AFR2 Loader

**Mensaje clave:** una aplicación de Windows propia que compila, carga y diagnostica ATmega y ESP desde un solo lugar, con
confirmaciones físicas, backups y un intérprete del enlace que lee nuestro catálogo de comandos.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «AFR2 Loader · compilar, cargar y depurar»          │
├──────────────────────┬───────────────────────────────────────────────────┤
│ A  QUÉ RESUELVE      │ B  FOTO-54 (Operación)   c5–c8 · f2–f4            │
│ lista c1–c4 · f2–f5  │    FOTO-55 (Flasher)     c9–c12 · f2–f4           │
│                      ├───────────────────────────────────────────────────┤
│                      │ C  D-09 flujo del Loader  c5–c12 · f5–f6          │
├──────────────────────┤                                                   │
│ D  Datos (chips) f6  │                                                   │
└──────────────────────┴───────────────────────────────────────────────────┘
```
Opcional: FOTO-56 (monitor serie) y FOTO-58 (MCP) como segunda fila reducida, o en notas.

## Contenido por zona

### A · Qué resuelve (5–6 viñetas)
- **Un flujo, dos chips:** destino ATmega (firmware completo o solo comunicación) o ESP (firmware propio, Web/LittleFS o paquete AT).
- **Compila y prepara** con las herramientas configurables (avr-gcc/make, PlatformIO, scripts de FileSystem); sin rutas fijas: `{App}`/módulos,
  perfiles y registros de cada usuario **fuera del repositorio**.
- **Carga segura:** avrdude al Mega; esptool al ESP **a través del Mega** (flasher temporal en software, USB ↔ USART1 ↔ ESP). Hash verificado en la
  misma sesión; backup de flash opcional antes de sobrescribir; confirmaciones físicas (KEY/RST, D18/D19) siempre visibles.
- **KEY/RST manual o automático** (circuito de la filmina 12).
- **Monitor serie** (ASCII/HEX, TX/RX a archivo) + **intérprete USB**: decodifica el enlace ROBO2/EX01 con `wire_catalog.json`; pestañas
  *Arranques ESP/ATmega* y *Reinicios* (causa de reset, excepción, `epc1`).
- **Servidor MCP** para asistentes de IA: solicitudes que el operador **revisa**; sin herramienta de comandos arbitrarios.

### B · FOTO-54 / FOTO-55
Capturas del Loader (pestañas Operación y Flasher). *Aviso: las existentes son de la versión 1.2.0; si se puede, recapturar con la
versión 2.4.0 (fuente) o la 2.0.0 distribuida.* Pie: *Compilación y carga del robot · pestañas Operación, Flasher, Monitor serie,
Historial, Eventos, Conversor HEX/ASCII, Servidor MCP, Configuración.*

### C · D-09 ([D-09_flujo_Loader.svg](diagramas/D-09_flujo_Loader.svg))
Diez pasos: Operación → Perfil → Compilar/preparar → Confirmaciones + backup → Flasher temporal → avrdude/esptool con hash →
restaura producto → Monitor → Intérprete → MCP.

### D · Chips de datos
| Chip | Dato |
| --- | --- |
| **2.4.0** | versión de fuente del Loader (última ficha 08/11/2026); distribución **2.0.0** (ZIP + SHA-256) |
| **C# / Windows** | aplicación propia; ejecutable `AFR2 Loader.exe` + lanzadores |
| **portátil** | rutas relativas a la aplicación; perfil de ejemplo incluido |
| **MANUAL.html** | manual propio del módulo |

## Guion del orador (≈ 60 s)

«Cargar firmware a un Mega y a un ESP desde cero es propenso a errores, y más cuando el ESP se programa a través del Mega. Hicimos el
Loader: eliges el puerto y el destino, compila, y para el ESP instala un puente en el Mega que reenvía el USB a la UART del ESP. Verifica
el hash en la misma sesión y puede guardar un backup antes. Además trae un monitor serie y un intérprete que entiende nuestros
comandos usando el mismo catálogo que la Web, y un servidor MCP para trabajar con un asistente sin dejarle ejecutar nada sin revisión.»

## Fuentes

`AutoFinalRobo2-Loader/README.md`, `MANUAL.html`, `PORTABILIDAD.md`, `MCP.md` · `AutoFinalRobo2-Contracts/docs/PLAN_LOADER_INTERPRETE_Y_TAP.md` ·
`contracts/LOADER_FLASH_MODES.md` · `hardware/PLAN_PRUEBA_FLASHER_MEGA.md`.

## Advertencias

- **El flasheo real con hardware no está comprobado** según el README (2.0.0): presentar el flujo como implementado y compilado.
- Las capturas disponibles son anteriores al intérprete; no mostrarlas como si incluyeran esa función.
- El MCP no demuestra ida y vuelta por sí solo (`boot mode 1` ≠ comunicación completa).


===== FIN DEL ARCHIVO: Filmina_11_Agregado_Loader.md =====

===== INICIO DEL ARCHIVO: Filmina_12_Agregado_NPN_KEY_RST.md =====

# Filmina 12 · Nuestro agregado: KEY y RST automáticos con transistores NPN

**Mensaje clave:** dos NPN en paralelo con los pulsadores del shield permiten que el Mega ponga al ESP en modo carga y lo reinicie
sin tocar nada, sin interferir con otros usos del shield.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «KEY y RST automáticos (NPN)»                       │
├────────────────────────────────────┬─────────────────────────────────────┤
│ A  D-06 CIRCUITO (ambos canales)   │ B  FOTO-61 shield (KEY / RST)       │
│ c1–c7 · f2–f5                      │ c8–c12 · f2–f3                      │
│                                    ├─────────────────────────────────────┤
│                                    │ C  Problema → solución (3 viñetas)  │
│                                    │ c8–c12 · f4–f5                      │
├────────────────────────────────────┴─────────────────────────────────────┤
│ D  FOTO-62 · FOTO-63 · FOTO-64  (circuito real, tester, cables) c1–c12·f6 │
└──────────────────────────────────────────────────────────────────────────┘
```

## Contenido por zona

### A · D-06 ([D-06_circuito_KEY_RST_NPN.svg](diagramas/D-06_circuito_KEY_RST_NPN.svg))
Por canal: **Mega D32/D33 → R 1 kΩ → base**; **R 10 kΩ de base a GND** (lado del transistor); **colector → pata de señal del pulsador**
(KEY = GPIO0, RST = reset); **emisor → GND común**. Tabla de redes (si cabe):

| Red | Une |
| --- | --- |
| `KEY_CTRL` / `RST_CTRL` | D32 / D33 – 1 kΩ |
| `Q1_B` / `Q2_B` | 1 kΩ – base – 10 kΩ |
| `ESP_KEY` / `ESP_RST` | colector – pata de señal del pulsador |
| `GND` | emisores, 10 kΩ, GND del Mega y del shield |

Componentes: 2 NPN (2N3904: E-B-C · BC547: C-B-E; **verificar con la hoja de datos**), 2 × 1 kΩ, 2 × 10 kΩ, cables ≤ 10 cm.

### B · FOTO-61
Vista superior del shield OSOYOO con los pulsadores **KEY** y **RST** a la izquierda. Marcar con flechas carmesí las dos patas donde se
sueldan los colectores. (Imagen del fabricante; no es nuestro montaje.)

### C · Problema → solución
- **Antes:** para cargar el ESP había que apretar KEY y RST a mano (y se usaba el método D0/D1 con RESET del Mega a GND, ya **retirado**).
- **Ahora:** el flasher en software (USART0 ↔ USART1) deja el RESET del Mega libre, así el Mega puede comandar D32/D33.
- **Secuencia de entrada en carga:** KEY alto → 20 ms → RST alto 100 ms → RST bajo → ~200 ms con KEY apretado → puente transparente + esptool →
  se suelta KEY.
- **Convivencia (requisito):** al arrancar D32/D33 son **entradas sin pull-up**; con los cables de control sacados, el circuito queda
  inerte; el pulsador sigue funcionando a mano; el modo automático es *opt-in* en el Loader y primero identifica al flasher AFR2.

### D · Fotos del circuito real (por tomar ✱)
**FOTO-62** circuito armado (vista cenital, componentes rotulados Q1/Q2/R1–R4) · **FOTO-63** medición con tester (reposo ≈ 3,3 V en KEY/RST
y < 0,3 V con el pin en alto) · **FOTO-64** cableado por colores (según el usuario: RST = marrón, KEY = violeta; cadena azul→naranja→marrón y
celeste→amarillo→violeta; **confirmar a qué pin va cada uno antes de rotular**).

## Guion del orador (≈ 60 s)

«El ESP necesita dos botones para entrar en modo de carga: KEY, que baja GPIO0, y RST. Los reemplazamos con dos transistores NPN
puestos en paralelo con los pulsadores del shield, comandados por D32 y D33 del Mega a través de una resistencia de un kilo. Una
resistencia de diez kilos mantiene la base a masa para que el transistor esté apagado aunque el Mega no tenga programa. Así
el Loader puede cargar el ESP sin que nadie toque nada, y si alguien saca los cables de control, el shield se comporta como uno de
fábrica. Hoy está implementado en el firmware y el Loader; el circuito se está armando y falta medirlo.»

## Fuentes

`AutoFinalRobo2-Contracts/hardware/PLAN_KEY_RST_AUTOMATICO.md` · `changes/2026-11-06-11-esp-key-rst-automatico.md` · `hardware/WIFI_SHIELD.md` ·
`hardware/FLASHING.md` · memoria de trabajo (colores de cables, mediciones preliminares: pulsador de 2 patas, una con continuidad a masa y otra 0,125 V en diodo, sin alimentar).

## Advertencias

- **Circuito sin armar ni medido** al 06/11/2026 (estado del plan): no afirmar que funciona. Los criterios de aceptación están definidos, no acreditados.
- Falta medir reposo (~3,3 V) y apretado en KEY y RST antes de soldar; NPN solo sirve si el pulsador lleva la señal a masa (control 2–4 del plan).
- Asignación de cables de colores a D32/D33 y a base/colector: sin confirmar.


===== FIN DEL ARCHIVO: Filmina_12_Agregado_NPN_KEY_RST.md =====

===== INICIO DEL ARCHIVO: Filmina_13_Agregado_Encoder_y_OLED.md =====

# Filmina 13 · Nuestro agregado: encoder y OLED

**Mensaje clave:** un panel local propio (OLED 0,96" + encoder KY-040) con su PCB, soporte y firmware: interrupciones sin pérdida de
pasos, una cola, un renderizado diferencial cooperativo y navegación coherente con la Web.

## Layout (grilla 12 × 6)

```text
┌──────────────────────────────────────────────────────────────────────────┐
│ TÍTULO  c1–c12 · f1   «Panel local: encoder + OLED»                       │
├──────────────────────┬──────────────────────┬────────────────────────────┤
│ A  HARDWARE          │ B  ENCODER           │ C  OLED                    │
│ FOTO-65 panel real   │ cadena de captura    │ cadena de render           │
│ FOTO-66 PCB (render) │ (texto + fórmulas)   │ (texto) + FOTO-69          │
│ c1–c4 · f2–f5        │ c5–c8 · f2–f5        │ c9–c12 · f2–f5             │
├──────────────────────┴──────────────────────┴────────────────────────────┤
│ D  Conectores y pines (tabla ancha) c1–c8 · f6    E  FOTO-67 / 68 c9–c12 │
└──────────────────────────────────────────────────────────────────────────┘
```

## Contenido por zona

### A · Hardware
- **FOTO-65** foto real del panel montado en el auto (✱); **FOTO-66** render de la PCB desde KiCad 3D (✱); **FOTO-67** soporte y PCB en Fusion (✱).
- PCB de **20 × 35 × 2 mm**, componentes pasantes, pistas en B.Cu y plano GND; el encoder va directo y el OLED en socket hembra 1×4.
- Soporte impreso con bisel y torretas M3 × 15 para el OLED; fijación a la cubierta con **dos tornillos y tuercas**; torretas de 5 mm hacia el auto.
- **Sin placa breakout** del KY-040: los comunes (giro y pulsador) van a GND; CLK/DT con R de 10 kΩ de pull-up.

### B · Encoder (captura por interrupción)
- **CLK → D2 (INT4) · DT → D3 (INT5) · SW → D42** (muestreo en `main` cada ≈ 1 ms, sin interrupción).
- Las ISR **solo capturan** el estado AB de ambos pines (una lectura de PINE) en una **cola circular de 32 B / 31 entradas**; nada de decodificar,
  I2C ni PID dentro de la ISR.
- `main` consume hasta **32 entradas por vuelta**, decodifica la cuadratura (2 o 4 transiciones por paso, inversión configurable; por defecto 4 e invertido)
  y ante desborde resincroniza sin inventar pasos.
- **Botón:** corto y largo se deciden **al soltar**; umbral ≥ 800 ms (800–3000 configurable); un largo nunca produce además un corto.
- Tabla de cuadratura (opcional en notas): secuencia 00→01→11→10→00 = un sentido, la inversa = el otro; transiciones inválidas se descartan.

### C · OLED (renderizado cooperativo)
- **SSD1306 128 × 64**, I2C en **D20 (SDA) / D21 (SCL)**, dirección 0x3C; **único dispositivo I2C**, accedido por un **I2C Manager** (`start/poll/abort`).
- Fuente 7 × 10 en PROGMEM; framebuffer en RAM (se conservan los dos buffers para diferencial).
- Envío **diferencial exacto en tramos ≤ 16 columnas**, con una etapa cada 2 ms como mínimo; el redibujo se limita a ≥ 40 ms entre cuadros y no bloquea el lazo
  de control (render es el último paso del ciclo, ver D-03a).
- Recuperación sin apagar ante error aislado; reconfiguración tras fallos persistentes.
- Dashboard con cabecera invertida: modo, SSID/IP desplazables, ícono de batería por bandas de voltaje y tarjeta de estados; **FOTO-69** OLED encendido (o reutilizar FOTO-48).
- La Web contiene el **mismo rasterizador** (paridad comprobada en host: 1680/1680 en la entrega del 06/11/2026) y puede mostrar el framebuffer remoto.

### D · Conectores y pines
| Conector | Pines |
| --- | --- |
| **J1** hembra 1×5 · 2,54 mm | 1 CLK · 2 DT · 3 SW · 4 SDA · 5 SCL |
| **J2** JST 1×2 · 2,00 mm | 1 VCC (5 V en el montaje) · 2 GND |
| OLED (de frente, pines arriba) | GND · VCC · SCL · SDA |

### E · FOTO-67 / FOTO-68
Soporte y PCB en Fusion (render) y captura del esquema KiCad (`PlacaOLED-Encoder-AutoRobo2.kicad_sch`).

## Guion del orador (≈ 75 s)

«El panel local es un OLED y un encoder con pulsador. En el encoder las interrupciones solo guardan el estado de los dos canales en una
cola; el programa principal los decodifica, así no se pierden pasos ni se bloquea nada. El pulsador distingue corto y largo al soltar. En
la pantalla, un único gestor I2C manda solo las diferencias de la imagen en tramos de 16 columnas, para que dibujar nunca retrase al
control de los motores. Diseñamos la placa en KiCad y el soporte en Fusion, y la Web reproduce exactamente las mismas pantallas.»

## Fuentes

`AutoFinalRobo2-Contracts/hardware/INTERRUPCIONES_ENCODER_HALL.md`, `PCB_OLED_ENCODER.md`, `PINOUT.md`, `OLED_I2C_DIAGNOSTICO.md` ·
`AutoFinalRobo2-CAD/kicad/oled-encoder/README.md` · `AutoFinalRobo2-CAD/docs/PANEL_UI_20261025.md` · `AutoFinalRobo2-Atmega/src/main.c`,
`src/input/afr2_encoder_ui.c`, `src/display/afr2_oled.c` · `README.md` del ATmega (fichas 20261029-11/-14).

## Advertencias

- **Tensión:** el montaje se informó a **5 V**, pero el CAD/KiCad conserva +3V3 en J2 y las pull-ups del encoder; no certificar compatibilidad de niveles
  ni rendimiento físico. Decir «a 5 V según el montaje comunicado».
- PCB y soporte: archivos de diseño; **sin ERC/DRC, sin comprobación de interferencias y sin fabricación** documentada. Tornillería y ranura M3 pendientes.
- Configuración validada con el diagnóstico independiente (OLED 0x3C, pulls como Wire, encoder invertido) que el usuario confirmó funcional el 29/10/2026.


===== FIN DEL ARCHIVO: Filmina_13_Agregado_Encoder_y_OLED.md =====