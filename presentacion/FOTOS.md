# Catálogo de fotos, capturas y dónde va cada una

Numeración **FOTO-01 … FOTO-69**, ordenada por filmina. Estados:

- ✔ **Existe** en el repositorio y está copiada en [`fotos_existentes/`](fotos_existentes/) con su número.
- ✱ **Foto real por tomar** (cámara / celular).
- ⌗ **Captura de pantalla por hacer** (Web, simulador, Loader, KiCad, Fusion, PDF).
- ◐ Existe pero conviene **rehacer o recortar**.

Al tomar una foto nueva, nombrarla `FOTO-NN_descripcion.jpg` (misma numeración) y dejarla en `fotos_nuevas/`.

## Convenciones de toma

| Tipo | Especificación |
| --- | --- |
| Fotos reales | Horizontal 16:10 o 4:3; mínimo 2000 px de ancho; luz difusa; fondo liso claro (cartulina blanca/gris); sin sombras duras; el objeto ocupa ≥ 60 % del cuadro; enfocar el detalle indicado |
| Fotos de OLED | De frente, cámara a 15–25 cm, brillo del OLED al máximo, exposición baja para que no se «queme»; recortar al marco del módulo; sin reflejos |
| Capturas de la Web | Ventana de 1600 × 1000 (o 1920 × 1080), tema **claro**, zoom 100 %, sin barras del navegador, sin datos personales ni claves; guardar PNG |
| Capturas del simulador | Misma ventana y tema; semilla 42, seis cubos; ocultar paneles que no se explican |
| Capturas de CAD/KiCad | Fondo blanco/gris claro, perspectiva isométrica, sin la interfaz del programa si se puede |
| Rótulos | Se agregan en el PowerPoint (etiqueta `FOTO-NN` carmesí + pie), no sobre la imagen original |

## Catálogo

### Filmina 1 · Introducción
| N.º | Estado | Descripción / encuadre | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-01 | ✔ | Render del robot completo (auto + brazo v17), fondo transparente: poner fondo `#F3F5F9` | A (hero) | `FOTO-01_robot_completo_CAD_v17.png` |
| FOTO-02 | ✱ | Foto real del auto armado, vista 3/4 frontal; si el brazo no está, el auto con el panel OLED visible | A (miniatura) | por tomar |
| LOGO | ✔ | Logo del proyecto (y logos UNER/FCAL en `Documentacion/LogoUner.png`) | portada/pie | `LOGO_proyecto.png` |

### Filmina 2 · Modelos
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-03 | ✔ | Auto base, render Blender frente | A | `FOTO-03_auto_base_frente_blender.png` |
| FOTO-04 | ✔ | Auto base, render Blender atrás (opcional) | A | `FOTO-04_auto_base_atras_blender.png` |
| FOTO-05 | ✔ | Auto base, lado derecho (Fusion) | A mini | `FOTO-05_auto_base_lado_derecho_fusion.png` |
| FOTO-06 | ✔ | Auto base, lado izquierdo (Fusion) | A mini | `FOTO-06_auto_base_lado_izquierdo_fusion.png` |
| FOTO-07 | ✔ | Brazo v17 (render CAD) | B | `FOTO-07_brazo_v17_CAD.png` |
| FOTO-08 | ⌗ | Web `#/modelos`: visor 3D con puntos de componentes | C | por capturar |
| FOTO-09 | ⌗ | Web `#/armado`: etapa del ensamblaje por scroll (la 7, «nuestros agregados», sirve también para la filmina 13) | C (opcional) | por capturar |
| FOTO-10 | ✔ | Rueda Mecanum 80 mm (cotas Ø80, 36 mm) | notas / filmina 5 | `FOTO-10_rueda_mecanum.jpg` |
| FOTO-11 | ✔ | Hoja de cotas del chasis OSOYOO 245 × 200 mm | A / notas | `FOTO-11_dimensiones_chasis.png` |

### Filmina 3 · Pinout y conexionado
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-12 | ✔ | Pinout oficial del shield OSOYOO V1.3 | C | `FOTO-12_shield_osoyoo_pinout.jpg` |
| FOTO-13 | ✔ | Mapa de pines Wi-Fi del shield | C alt. | `FOTO-13_shield_mapa_pines_wifi.png` |
| FOTO-14 | ✱ | Mega + shield vistos desde arriba, cables rotulados (etiquetas D18/D19, D32/D33, A5) | D | por tomar |
| FOTO-15 | ✱ | Motores y headers Hall cableados (colores S1 naranja, S2 verde, V/G) | D | por tomar |
| FOTO-16 | ⌗ | Hoja `Documentacion/PINOUT_DEL_AUTO.pdf`, página 1, exportada como imagen | A alt. | PDF existente |

### Filmina 4 · Firmwares
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-17 | ⌗ | Web `#/infraestructura`: recorrido React/Vite → gzip → LittleFS → ESP → navegador (opcional) | C | por capturar |
| FOTO-18 | ⌗ | Web `#/desarrollador/protocol-studio` con el catálogo de comandos (opcional) | E | por capturar |

### Filmina 5 · Matemática del auto
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-19 | ⌗ | Web `#/simulacion/auto`: matriz Mecanum lateral desplegada, ruedas con vectores | B alt. | por capturar |
| FOTO-20 | ✱ | Auto sobre el piso junto a la marca de 1000 mm (cinta métrica / regla larga), vista cenital | D | por tomar (prueba de odometría) |

### Filmina 6 · GOTO y odometría
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-21 | ⌗ | Web `#/programas`: lista de comandos GOTO + mapa con ruta (con arco/curva) | D | por capturar |
| FOTO-22 | ⌗ | Web `#/puesta-a-punto` → Odometría: prueba de distancia | D | por capturar |
| FOTO-23 | ✱ | OLED: *Estado → Modos/misiones → GOTO relativo* (03000d) | D | por tomar |
| FOTO-24 | ✱/⌗ | Gráfico o foto «ruta pedida vs recorrida» si se mide (si no hay datos, omitir) | D opcional | por generar |

### Filmina 7 · Brazo y conjunto
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-25 | ⌗ | Web `#/simulacion/brazo`: panel de articulaciones y matrices homogéneas | E | por capturar |
| FOTO-26 | ⌗ | Web `#/simulacion/conjunta`: auto + brazo | E | por capturar |
| FOTO-27 | ⌗ | Una lámina A3 del libro de planos (`Revision/R01_20261110/02_Planos/…Documentacion_Dimensional_A3.pdf`) | E opcional | PDF existente |
| FOTO-28 | ⌗ | Fusion: brazo con ejes J1–J6 rotulados | E opcional | por capturar |

### Filmina 8 · Esfuerzos y vuelco
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-29 | ✔ | Proyección del centro de masa sobre el apoyo (CG frontal 0,85 mm del borde) | C | `FOTO-29_estabilidad_planta_CG.png` |
| FOTO-30 | ✔ | Cargas y momentos en la pose horizontal | C alt. | `FOTO-30_cargas_y_momentos.png` |
| FOTO-31 | ⌗ | Render/Fusion del brazo extendido hacia adelante (pose crítica) | C opcional | por capturar |

### Filmina 9 · Simulador y misión
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-32 | ✔ | Esquema editable de montaje del recolector (SVG, dibujo nominal) | apoyo | `FOTO-32_propuesta_montaje_recolector.svg` |
| FOTO-33 | ⌗ | Conjunto en perspectiva con cubos y depósito | B | por capturar |
| FOTO-34 | ⌗ | Vista superior con nube de alcance | B | por capturar |
| FOTO-35 | ⌗ | Vista lateral en preagarre | B | por capturar |
| FOTO-36 | ⌗ | Radar ideal con ecos y barrido | B | por capturar |
| FOTO-37 | ⌗ | Secuencia 1/3: preagarre sobre cubo | E | por capturar |
| FOTO-38 | ⌗ | Secuencia 2/3: elevación y transporte | E | por capturar |
| FOTO-39 | ⌗ | Secuencia 3/3: suelta en celda del depósito | E | por capturar |
| FOTO-40 | ⌗ | Panel de resultado de misión (motivo de fin) | opcional | por capturar |

### Filmina 10 · Interfaz
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-41 | ⌗ | Web `#/` Inicio (tres enlaces, capacidades) | D | por capturar |
| FOTO-42 | ⌗ | Web `#/conducir` (pad táctil, STOP) | D | por capturar |
| FOTO-43 | ⌗ | Web `#/programas` (GOTO/secuencias) — puede reutilizar FOTO-21 | D | por capturar |
| FOTO-44 | ⌗ | Web `#/puesta-a-punto` (calibración con ayuda lateral) | D | por capturar |
| FOTO-45 | ⌗ | Web `#/sensores` (IR, HC-SR04, Hall con antigüedad) | D | por capturar |
| FOTO-46 | ⌗ | Web `#/conexion` (Wi-Fi; ocultar SSID/clave) | D | por capturar |
| FOTO-47 | ⌗ | Web `#/ayuda?seccion=oled` (OLED virtual + árbol de pantallas) | D | por capturar |
| FOTO-48 | ✱ | OLED: Inicio 010001 (dashboard) | B | por tomar |
| FOTO-49 | ✱ | OLED: Menú principal 010002 | B | por tomar |
| FOTO-50 | ✱ | OLED: Estado 030004 (o Fallos 030007) | B | por tomar |
| FOTO-51 | ✱ | OLED: Radar 030031 | B | por tomar |
| FOTO-52 | ✱ | OLED: Guardar cambios Sí/No 030038 | B | por tomar |
| FOTO-53 | ✔ | Silueta del autito 128 × 64 (recurso de la pantalla) | B | `FOTO-53_autito_silueta_OLED_128x64.png` |

### Filmina 11 · Loader
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-54 | ◐ | Loader, pestaña Operación (captura de **v1.2.0**; rehacer con 2.x) | B | `FOTO-54_loader_operacion.png` |
| FOTO-55 | ◐ | Loader, pestaña Flasher | B | `FOTO-55_loader_flasher.png` |
| FOTO-56 | ◐ | Loader, Monitor serie | opcional | `FOTO-56_loader_monitor_serie.png` |
| FOTO-57 | ◐ | Loader, Configuración | opcional | `FOTO-57_loader_configuracion.png` |
| FOTO-58 | ◐ | Loader, Servidor MCP | opcional | `FOTO-58_loader_mcp.png` |
| FOTO-59 | ✱ | Montaje de carga: cable USB al Mega, shield con ESP, mano en KEY/RST (o NPN) | opcional | por tomar |
| FOTO-60 | ⌗ | Loader 2.x: Monitor serie con el **intérprete** y pestaña *Arranques* | B alt. | por capturar |

### Filmina 12 · NPN KEY / RST
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-61 | ✔ | Vista superior del shield con KEY y RST | B | `FOTO-61_shield_vista_superior_KEY_RST.jpg` |
| FOTO-62 | ✱ | Circuito armado (cenital, Q1/Q2, R1–R4 rotulables) | D | por tomar |
| FOTO-63 | ✱ | Medición con tester en KEY/RST (pantalla del multímetro legible) | D | por tomar |
| FOTO-64 | ✱ | Cables de colores desde Mega a transistores | D | por tomar |

### Filmina 13 · Encoder y OLED
| N.º | Estado | Descripción | Zona | Archivo o fuente |
| --- | --- | --- | --- | --- |
| FOTO-65 | ✱ | Panel OLED + encoder montado en el auto (lateral y frontal) | A | por tomar |
| FOTO-66 | ⌗ | PCB en el visor 3D de KiCad (`PlacaOLED-Encoder-AutoRobo2.kicad_pcb`) | A | por capturar |
| FOTO-67 | ⌗ | PCB + soporte con torretas en Fusion | E | por capturar |
| FOTO-68 | ⌗ | Esquema KiCad (`.kicad_sch`) | E | por capturar |
| FOTO-69 | ✱ | OLED encendido con el dashboard (o reutilizar FOTO-48) | C | por tomar |

## Recuento

- Existentes ✔/◐: 20 imágenes + logo (FOTO-01, 03–07, 10–13, 29, 30, 32, 53, 54–58 del Loader, 61).
- Fotos reales por tomar ✱: FOTO-02, 14, 15, 20, 23, 48–52, 59, 62–65, 69 (≈ 15).
- Capturas por hacer ⌗: FOTO-08, 09, 16–19, 21, 22, 25–28, 31, 33–47, 60, 66–68 (≈ 30). Casi todas se hacen en una sola sesión con la Web local y el simulador.

> Orden práctico de trabajo: (1) sesión de capturas de la Web y del simulador (≈ 1 h); (2) sesión de fotos de OLED (cinco pantallas); (3) fotos de armado
> y NPN; (4) capturas de KiCad y Fusion.
