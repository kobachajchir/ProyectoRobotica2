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
