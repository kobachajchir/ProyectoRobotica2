# Prompt · Cómo se arma la presentación v4 (renders, diagramas, PowerPoint y referencias)

> Pegar en Claude Code abierto en `D:/ProyectoRobotica2` cuando haya que seguir editando la presentación en cualquier
> computadora. Explica el flujo completo tal como está hecho hoy, para continuarlo sin romperlo.

---

Vas a continuar la presentación **AutoFinalRobo2 v4** (tema claro, estilo glass). Leé primero `AGENTS.md`,
`presentacion/v3/README.md` y este documento. No compiles firmware, no flashees, no corras tests ni simulaciones salvo
pedido expreso del usuario. No toques `Documentacion/` ni los maestros CAD (`*.f3d`, `*.blend`, el GLB de la Web).

## 1 · Regla principal: el PowerPoint se genera, no se edita a mano

El `.pptx` sale **siempre** de un script. Cualquier cambio de texto, posición, color o imagen se hace en el código y se
regenera. Si alguien edita el `.pptx` en PowerPoint, esos cambios se pierden en la próxima generación.

| Pieza | Archivo | Qué hace |
| --- | --- | --- |
| Generador del PPTX | `presentacion/v3/tools/generar_presentacion_v4.js` | Cada filmina es una llamada `slide({...})` con título, frase clave, pie curricular, ramas, `build(s)` con la maquetación, guion, fuentes y pendientes (estos tres van a las notas del orador y a `GUION_HABLADO_v4.md`) |
| Diagramas | `presentacion/v3/tools/diagramas_claro.py` | matplotlib → PNG/SVG transparentes en `presentacion/v3/diagramas_claro/` (Venn, árbol, matrices, cargas del brazo, N/V/M/T, vuelco, fórmulas anotadas, y los diagramas de la v3 en tema claro) |
| Fondos | `presentacion/v3/tools/fondo_claro.py` | PIL → `presentacion/v3/fondos/bg_claro.png` y `bg_hero.png` (manchas difusas de los cuatro colores) |
| Renders 3D | PC de escritorio (Blender/Fusion) | Llegan a `presentacion/v3/assets_claro/` según `PROMPT_ESCRITORIO_FUSION_BLENDER.md` y `PROMPT_ESCRITORIO_2_BRAZO_FEA_DESPIECE.md` |
| Versión anterior | `generar_presentacion_v3.js`, `diagramas_v3.py`, `renders/`, `diagramas/` | Tema oscuro v3: se conserva intacta, no editar |

## 2 · Regenerar todo

Desde `presentacion/v3`:

```bash
python -I tools/fondo_claro.py fondos
```

```bash
python -I tools/diagramas_claro.py diagramas_claro
```

```bash
NODE_PATH=<carpeta>/node_modules node tools/generar_presentacion_v4.js
```

Salida: `pptx/AutoFinalRobo2_Mecatronica_v4.pptx` y `GUION_HABLADO_v4.md`.

**pptxgenjs:** en la notebook está instalado en una carpeta temporal de otra sesión
(`C:/Users/kobac/AppData/Local/Temp/claude/d5a9fdd36aefa519/.../scratchpad/pp/node_modules`), que puede desaparecer.
Si no existe, pedirle permiso al usuario para instalar `pptxgenjs` (npm) en una carpeta fuera del repo o en
`presentacion/v3/tools/node_modules` (ignorada por Git) y usar esa ruta en `NODE_PATH`.

Python: Pillow y matplotlib, con las fuentes **Inter** y **Manrope** instaladas (en la notebook están en la carpeta de
fuentes del usuario). Sin esas fuentes, matplotlib y PowerPoint las reemplazan y cambian los tamaños.

## 3 · Verificar sin abrir PowerPoint a mano

Se usa PowerPoint por COM desde PowerShell para exportar filminas a PNG y la presentación a PDF, y se miran los PNG:

```powershell
$pp = New-Object -ComObject PowerPoint.Application
$pres = $pp.Presentations.Open('D:\ProyectoRobotica2\presentacion\v3\pptx\AutoFinalRobo2_Mecatronica_v4.pptx', $true, $false, $false)
$pres.Slides(15).Export('<carpeta temporal>\s15.png', 'PNG', 1600, 900)
$pres.SaveAs('<carpeta temporal>\AutoFinalRobo2_Mecatronica_v4.pdf', 32)
$pres.Close(); $pp.Quit()
```

Guardar los PNG y el PDF en una carpeta temporal fuera del repo (los PDF/PNG de revisión son regenerables). Revisar en
cada filmina tocada: texto cortado o superpuesto, imágenes que tapan tarjetas, referencias que pisan contenido, y que
nada pase de `y = 6.9` (ahí empieza el pie). El PDF embebe las fuentes; es lo que se le manda al usuario.

## 4 · Sistema visual (no inventar otro)

Todo está en la cabecera de `generar_presentacion_v4.js`:

- **Tokens** `C`: fondo `F6F8FC`, texto `132033`, secundario `4F6178`, bordes `CBD5E1`; ramas MEC `3B82F6`, ELE `F59E0B`,
  CTL `8B5CF6`, PRG `10B981`, y sus versiones oscuras `*_D` para texto sobre claro. Salen de
  `informe-portabilidad-glassmorphism.md` (raíz del repo) más los colores de rama de la v3.
- **Tipografía:** títulos `Manrope` en negrita, texto `Inter`, código `Consolas`, fórmulas en línea `Cambria Math`.
- **Helpers:** `glass()` (tarjeta blanca translúcida + borde + sombra suave; `tint` la tiñe con el color de la rama),
  `text()`, `bullets()`, `code()`, `chip()`, `table()`, `eq()`, `pending()` (recuadro rojo punteado), `refs()`,
  `leyenda()`, `img()` (encaja manteniendo proporción y devuelve la caja real) y `framed()` (pone una tarjeta detrás de
  los renders viejos de fondo oscuro).
- **Minimapa:** círculos M/E/C/P arriba a la derecha, encendidos según `ramas`; etiqueta abajo.
- Lienzo 13,333 × 7,5 in, margen `M = 0.7`. Título en `y 0.34`, clave en `0.98`, contenido entre `1.45` y `6.2`,
  referencias en `6.27–6.87`, pie en `6.95`.

En los diagramas de `diagramas_claro.py` se usan los mismos colores (`MEC/ELE/CTL/PRG` y `*d`), texto `#132033`,
fondo transparente y fuente Inter. Cada diagrama se guarda con `save(fig, nombre)`.

## 5 · Cómo entran los renders de Fusion/Blender

- La PC de escritorio deja los archivos en `presentacion/v3/assets_claro/{renders,planos,capturas,logos}/` con los nombres
  exactos que piden los dos prompts de escritorio, y un `MANIFEST.json` con cámara, origen y notas.
- En el generador, `A("renders/xxx.png", respaldo)` devuelve el archivo nuevo si existe; si no, el render de la v3
  (`renders/`) o `null`, y `img()` dibuja un recuadro «pendiente». **No hace falta tocar código cuando llegan los
  renders:** basta `git pull` y regenerar.
- Flechas sobre renders: el render viene con un `.json` de puntos en píxeles (`{"w","h","points":{clave:[x,y]}}`). El
  generador convierte píxel → pulgadas con la caja que devuelve `img()` y dibuja las flechas. Lo usan
  «Elementos de máquinas en el brazo» (`F_brazo_lateral_v001.json`) y «Centro de masa, inercia y vuelco»
  (`F18_auto_brazo_lateral/frontal_v001.json`). Sin JSON se usa FOTO-07 con puntos estimados o el esquema didáctico.
- Datos numéricos que llegan como JSON: `assets_claro/peso_modelo.json` (antes/después del modelo 3D en la filmina de
  Blender) y `assets_claro/renders/F_fea_torre_v001.json` (material, cargas y resultados del FEA).
- El script de render de la v3 (`tools/render_assets.py`) arranca Blender vacío e importa el GLB de la Web; la PC de
  escritorio escribe `tools/render_assets_claro.py` con el mismo método y luz clara.

## 6 · Cómo agregar o cambiar una filmina

1. Si necesita un diagrama, agregarlo al final de `diagramas_claro.py` y regenerar diagramas.
2. Agregar o editar el bloque `slide({ id, ramas, t, titulo, clave, pie, build, guion, fuentes, pend })` en el orden en
   que va en la charla. `t` son los segundos del guion; el total se imprime al generar.
3. Toda fórmula lleva debajo qué significa cada símbolo: con `leyenda()`/`bullets()` en la filmina o, si es la
   fórmula protagonista, como imagen anotada con `eq_annot()` (flechas desde cada símbolo a su explicación).
4. Si la filmina usa bibliografía o un archivo del proyecto, llamar `refs(s, ["CLAVE", ...])` (ver sección 7).
5. Si un dato no está verificado, va en `pend` (notas del orador), no en la filmina como afirmación.
6. Regenerar, exportar las filminas tocadas a PNG y revisarlas.

## 7 · De dónde salen las referencias

Hay un catálogo único `REF` en el generador; el número `[n]` es la posición en el catálogo y es el mismo en todas las
filminas y en la filmina final «Referencias», que lo agrupa por área. `refs(s, claves)` imprime las entradas de esa
filmina como viñetas al pie.

Hay dos tipos de referencias, y conviene distinguirlas:

**a) Archivos del proyecto (verificados en el repo).** Todo número que aparece en una filmina (dimensiones, pines,
relaciones de transmisión, masas, momentos, tamaños de archivo) sale de un archivo del repo, citado en el campo
`fuentes` de la filmina y, cuando corresponde, en `REF` con categoría «Proyecto». Ejemplos:

- Cinemática y odometría: `AutoFinalRobo2-Atmega/src/services/afr2_product.c` (`drive_closed_loop()` l. 130,
  `odometry()` l. 295-304). Las matrices de la filmina se derivaron de esas líneas (inversa = mezcla de ruedas; directa
  = pseudoinversa, que coincide con las fórmulas de odometría).
- Cargas del brazo: `AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g/resultados_estatica.json`
  (caso `CAD_macizo_200g`); N/V/M/T de `05_frenada_y_esfuerzos_combinados.md`.
- Transmisiones y rodamientos del brazo: `revision_v17/README.md`.
- Panel OLED: `AutoFinalRobo2-CAD/docs/PANEL_UI_20261025.md` y `MODELO_FINAL_WEB_20261025.md`; pines en
  `contracts/hardware/PINOUT.md`, `afr2_twi.c`, `afr2_board.c`.
- Pesos del modelo 3D: `AutoFinalRobo2-CAD/exports/2026-10-24/parts` y `docs/modelo-final-20261025.json`.
- Procedencia de modelos (GrabCAD, modelados por el equipo): lo declaró el usuario; cruzar con
  `AutoFinalRobo2-CAD/reference/inventario-CADS-2026-10-23.csv` antes de citar autores.

**b) Bibliografía general (elegida por Claude, no está en el repo).** Libros de texto estándar para cada tema, citados
de conocimiento general, **sin haber consultado los ejemplares**: Lynch y Park (Modern Robotics), Craig (Introduction
to Robotics), Siegwart et al. (Autonomous Mobile Robots), Hibbeler (Estática y Dinámica), Beer y Johnston (Mecánica de
materiales), Budynas y Nisbett (Shigley), Ogata (Control moderno) y las hojas de datos de Microchip y Espressif.
Antes de exponer, alguien con los libros (o la cátedra) tiene que confirmar edición, año y capítulo; si la cátedra usa
otra bibliografía, reemplazarla en `REF` y todo se renumera solo.

Las correlatividades del árbol vienen de las aristas del informe maestro (Res. C.D. 726/23). **El año de cada materia
lo asignó Claude respetando ese orden, sin el plan a mano:** está marcado como pendiente en las notas y hay que
confirmarlo contra el Plan 2023.

## 8 · Al terminar

- Regenerar, exportar PDF y mandarlo al usuario.
- Commit solo si el usuario lo pide. Si lo pide: `tools/*.py`, `tools/generar_presentacion_v4.js`, los `.md` de
  `presentacion/v3`, `diagramas_claro/`, `fondos/` y `assets_claro/`; el `.pptx` y los PDF son regenerables (según
  `Contracts/docs/ESTRUCTURA_Y_GIT.md` conviene no versionarlos). La presentación no sube la versión de producto.
