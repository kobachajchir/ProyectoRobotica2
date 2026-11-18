# Presentación v3 · De la teoría a un sistema mecatrónico real

Versión de 33 filminas (+2 anexos, ≈25 min) que ejecuta el *Informe maestro ejecutable para producir la presentación de
Ingeniería Mecatrónica* sobre el repositorio real. Paleta, minimapa de cuatro ramas, pie curricular y estados de verdad
(VERIFICADO / DERIVADO / DECLARADO / BLOQUEADO) del informe. La v2 de 13 fichas (`../Filmina_*.md`) se conserva y sus
guiones de Loader y NPN pasan a los anexos A1/A2.

Producido el 18/11/2026 desde la fuente del repo (producto 1.39.6). No se compiló, flasheó, ni se ejecutó la Web: los
renders salen del GLB que sirve la Web y las capturas KiCad de `kicad-cli`.

## Entregables

| Archivo | Qué es |
| --- | --- |
| [pptx/AutoFinalRobo2_Mecatronica_v3.pptx](pptx/AutoFinalRobo2_Mecatronica_v3.pptx) | La presentación, 16:9, notas del orador con guion + fuentes + pendientes |
| [GUION_HABLADO.md](GUION_HABLADO.md) | Guion completo con tiempos acumulados (mismo texto que las notas) |
| [evidence/verification_matrix.csv](evidence/verification_matrix.csv) | 49 afirmaciones con fuente, línea y estado |
| `renders/` | 14 renders Cycles del GLB (cámaras en `renders/cameras_*.json`) |
| `diagramas/` | 18 diagramas SVG + PNG con la paleta del informe |
| `capturas/` | Esquemáticos y render 3D exportados del proyecto KiCad real |
| `tools/` | Generadores reproducibles (ver abajo) |

## Regenerar

```bash
python -I tools/diagramas_v3.py diagramas
```

```bash
"C:/Program Files/Blender Foundation/Blender 4.5/blender.exe" -b --factory-startup --python tools/render_assets.py -- renders
```

```bash
NODE_PATH=<carpeta con pptxgenjs>/node_modules node tools/generar_presentacion_v3.js
```

El render arranca Blender vacío e importa `AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb`: no abre ni modifica
ningún `.blend` del proyecto. `AFR2_SAMPLES` controla las muestras de Cycles (64 por defecto en esta entrega).

## Hallazgos que conviene resolver antes de exponer

1. **Diámetro de rueda:** firmware `AFR2_FIXED_WHEEL_MM 80`, pero la rueda del GLB mide 109,2 mm. Afecta odometría,
   GOTO y velocidad en mm/s. Medir la rueda física.
2. **Tensión del panel OLED/encoder:** el esquemático lo alimenta a 3V3 y anota «adaptación de niveles I²C con Mega 5 V:
   pendiente externa»; `hardware/PINOUT.md` reporta el panel a 5 V.
3. **Notificaciones:** el informe suponía cola de 8; el código tiene `notification[4]`.
4. **Builds desalineados:** los números de memoria son de los builds del 06/11 y 09/11; la fuente es 1.39.6. Regenerar
   `size`/`.map` de la versión que se vaya a cargar si se quiere mostrar memoria «actual».

## Bloqueados (el asset muestra un recuadro rojo punteado)

Log de respuesta del lazo (F20), ensayo de los 4 motores (F21), latencia extremo a
extremo (F31), captura del datasheet (F11), SVG fuente y timeline de Fusion (F06), brazo montado en lateral ortográfica
(F09; el GLB del auto no incluye el brazo), dirección I²C y tasa de refresco (F17).

## Propuesta de organización

1. **Una sola fuente de verdad por filmina.** El generador ya concentra texto, guion, fuentes y pendientes; editar el
   `.js` y regenerar en vez de tocar el `.pptx` a mano. Si se prefiere editar en PowerPoint, congelar el generador.
2. **Mover lo regenerable fuera de Git.** `renders/`, `diagramas/*.png` y `pptx/` se pueden regenerar; según
   `Contracts/docs/ESTRUCTURA_Y_GIT.md` convendría ignorarlos y versionar solo `tools/`, `evidence/` y los `.md`.
3. **Retirar la v2 a `presentacion/archivo/`** una vez aprobada la v3, dejando en la raíz de `presentacion/` solo
   la versión vigente; `Mustra del trabajo/` (sin trackear) puede pasar a `presentacion/ideas/`.
4. **Ensayo con cronómetro.** El bloque 22–31 es el más largo (6:55). Si se excede: fusionar F22+F23 y F30+F31
   como propone el informe (31 filminas).
5. **Aprobación humana obligatoria** en F03, F09/F10, F13, F20, F27, F30 y F31 antes de exponer.
