# ADJUNTO 4 de 4 · contiene FOTOS.md, DIAGRAMAS.md, ANEXO_Formulario.md y ANEXO_Pendientes_y_alertas.md.

===== INICIO DEL ARCHIVO: FOTOS.md =====

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


===== FIN DEL ARCHIVO: FOTOS.md =====

===== INICIO DEL ARCHIVO: DIAGRAMAS.md =====

# Diagramas D-01 … D-11

SVG en [`diagramas/`](diagramas/) (vectoriales, editables) y PNG ×2 en [`diagramas/png/`](diagramas/png/). Se regeneran con
`python diagramas/generar_diagramas.py` (solo escribe en esa carpeta). Leyenda de colores: gris = proceso · amarillo = decisión ·
azul = entrada/dato · violeta = hardware · rojo = fallo/rechazo · verde = correcto/fin · oscuro = inicio/fin · guiones = nota.

| ID | Archivo | Filmina · zona | Qué muestra |
| --- | --- | --- | --- |
| D-01 | `D-01_arquitectura_general.svg` | 1 · B | Operador → Web → ESP → ATmega → actuadores; panel y sensores; Loader por USB |
| D-02 | `D-02_conexionado.svg` | 3 · B | Mega al centro con cada periférico y sus pines |
| D-03a | `D-03a_ciclo_principal_ATmega.svg` | 4 · D | Ciclo principal (`main.c`) en 8 pasos |
| D-03b | `D-03b_servicio_de_control.svg` | 4 (apoyo) / 6 | `Afr2Product_Task`: vencimientos, F1, odometría, PID, F3/F4, `Motor_Write` |
| D-04 | `D-04_flujo_GOTO.svg` | 6 · B | Pedido GOTO → validación → lazo cada 10 ms → llegada |
| D-05 | `D-05_ciclo_mision_recolectora.svg` | 9 · D | Ciclo de la misión con sus cuatro finales |
| D-06 | `D-06_circuito_KEY_RST_NPN.svg` | 12 · A | Esquemático de los dos canales NPN y secuencia de carga |
| D-07 | `D-07_navegacion_OLED_encoder.svg` | 10 · C | Corto/largo, Inicio, Menú, Rápidas, retorno, Sí/No |
| D-08 | `D-08_cadena_cinematica.svg` | 7 · A | Cadena mundo → base → montura → brazo → TCP |
| D-09 | `D-09_flujo_Loader.svg` | 11 · C | Pasos del Loader de la fuente al equipo |
| D-10 | `D-10_pipeline_odometria.svg` | 6 · A | Pulsos Hall → pose (x, y, ψ) |
| D-11 | `D-11_patrones_rueda_mecanum.svg` | 5 · B | Signos de rueda en avance, lateral y giro |

## Notas de uso

- Insertar como imagen y escalar **proporcionalmente** al ancho de la zona. Los diagramas altos (D-03a, D-03b, D-04, D-05, D-10) van en zonas verticales
  (3–4 columnas × 5 filas); si no caben, partirlos en dos columnas copiando el SVG y recortando en el PowerPoint (Formato de imagen → Recortar).
- Para cambiar un texto: abrir el SVG en un navegador o editor vectorial, o editar `generar_diagramas.py` y regenerar.
- Contenido verificado contra: `main.c` (D-03a), `afr2_product.c` (D-03b, D-04, D-10), `ROBOT_RECOLECTOR.md` (D-05), `PLAN_KEY_RST_AUTOMATICO.md` (D-06),
  `UI_SCREEN_FLOW.md` (D-07), `AFR2_Informe_Borrador_D1.md` (D-08, D-11), `Loader/README.md` (D-09), `PINOUT.md` (D-02).
- D-05 y D-06 son **diseño/simulación**, no comportamiento verificado en hardware (el pie de cada diagrama lo dice).


===== FIN DEL ARCHIVO: DIAGRAMAS.md =====

===== INICIO DEL ARCHIVO: ANEXO_Formulario.md =====

# Anexo · Formulario completo con símbolos, unidades y ejemplos

Unidades: ecuaciones en **m, s, rad** (los planos en mm). Marco: X adelante, Y izquierda, Z arriba; ψ positivo antihorario.
Todas las fórmulas están pensadas para pegarse con *Insertar → Ecuación*.

## 1. Transformaciones
| Fórmula | Significado |
| --- | --- |
| `p_A = R_AB p_B + t_AB` | coordenadas de B expresadas en A |
| `T_AC = T_AB T_BC` | composición (el orden importa) |
| `T_AB⁻¹ = [R_ABᵀ, −R_ABᵀ t_AB ; 0, 1]` | inversa |
| `Rz(θ) = [cθ −sθ 0 ; sθ cθ 0 ; 0 0 1]` · `Ry(θ) = [cθ 0 sθ ; 0 1 0 ; −sθ 0 cθ]` | rotaciones elementales |
| `A_i = Rz(θᵢ) Tz(dᵢ) Tx(aᵢ) Rx(αᵢ)` | DH estándar (tabla en informe D1, cap. 6.1) |

## 2. Brazo (descriptor v17)
`h = 0,209 · L₁ = 0,070 · L₂ = 0,115 · d = 0,1435` (m)

- Directa: `ρ = L₁ sin q₂ + L₂ sin(q₂+q₃)` ; `z_W = h + L₁ cos q₂ + L₂ cos(q₂+q₃)` ; `p_TCP = p_W + d R e_z`
- Inversa: `q₁ = atan2(y_W, x_W)` ; `cos q₃ = (ρ² + s² − L₁² − L₂²)/(2L₁L₂)` ; `q₂ = atan2(ρ, s) − atan2(L₂ sin q₃, L₁ + L₂ cos q₃)` ;
  `q₅ = atan2(√(r₁₃²+r₂₃²), r₃₃)` ; `q₄ = atan2(r₂₃, r₁₃)` ; `q₆ = atan2(r₃₂, −r₃₁)`
- Jacobiano: columna `[z_i × (p_TCP − o_i) ; z_i]` ; `[v ; ω] = J q̇` ; `τ = Jᵀ [F ; M]` ; `q̇ = Jᵀ (JJᵀ + λ²I)⁻¹ v_d`
- Incertidumbre: `Σ_p ≈ J_v Σ_q J_vᵀ + J_l Σ_l J_lᵀ`
- Quíntica: `q(t) = q₀ + (q_f − q₀)(10σ³ − 15σ⁴ + 6σ⁵)`, `σ = t/T`
- Transmisión: `|ω_sal| = |ω_mot|/i` ; `|τ_sal| = η i |τ_mot|` ; `1 kgf·cm = 0,0980665 N·m`
- Pinza: `g(θ) = g₀ + 2 r_p θ` (r_p = 12 mm) ; `F_mandíbula ≈ η τ_piñón/(2 r_p)` ; apertura ≈ 4,007…79,405 mm

**Ejemplo (q₂ = 90°, resto 0):** ρ = 0,185 m, z = 0,209 m ⇒ TCP = (0,3285 ; 0 ; 0,209) m.
**Ejemplo (pose de transporte [0, 15°, −40°, 0, 25°, 0]):** β = q₂+q₃ = −25° ; ρ = 0,070 sin15° + 0,115 sin(−25°) = −0,0305 m ; z_W = 0,209 + 0,070 cos15° + 0,115 cos25° = 0,3808 m ;
con q₅ = 25°, `Ry(β)·Ry(q₅) = I` y la herramienta apunta hacia arriba: TCP = (−0,0305 ; 0 ; 0,5243) m.

## 3. Base Mecanum
- `u = A S V` ; `ω_rueda = u/r` ; `V = S A⁺ u` ; `A = [1 −1 −L ; 1 1 L ; 1 1 −L ; 1 −1 L]` ; `r = 0,040 m`, `L = 0,170 m`
- `γ = max(1, max|uᵢ|/u_max)` ; `u_ap = u/γ`
- **Ejemplos:** vₓ = 0,1 m/s ⇒ uᵢ = 0,1 m/s ⇒ ω = 2,5 rad/s = 23,9 rpm · ω = 0,5 rad/s ⇒ |uᵢ| = 0,085 m/s.

## 4. Odometría
- `Δφᵢ = 2π Δnᵢ/Nᵢ` ; `Δsᵢ = Δnᵢ π D/Nᵢ` ; resolución `π D/N = π·80/370 = 0,679 mm/cuenta` ; 1000 mm ⇒ 1472 cuentas
- `Δx = ¼ΣΔsᵢ` ; `Δy = ¼(−Δs₁+Δs₂+Δs₃−Δs₄)` ; `Δψ = ¼(−Δs₁+Δs₂−Δs₃+Δs₄)/L`
- `Δx, Δy, Δψ ·= factor direccional (avance 1,000 · lateral 0,775 · giro 1,000)`
- `ψₘ = ψ + Δψ/2` ; `x += Δx cosψₘ − Δy sinψₘ` ; `y += Δx sinψₘ + Δy cosψₘ` ; `ψ += Δψ`
- `[ẋ ; ẏ ; ψ̇] = [cosψ −sinψ 0 ; sinψ cosψ 0 ; 0 0 1] V`

## 5. Control
- Por rueda: `e = u_ref − u_med` ; `a = sat[(Kp e + Ki ∫e + Kd Δe)/256]` ; Kp 128, Ki 8, Kd 0 ; rampa ±4 por ciclo de 10 ms ; PWM ≤ 255
- GOTO: ver filmina 6 (`eₓ, e_y, e_ψ`; llegada 10 mm / 25 mrad; 60 s)
- Seguidor de línea: `e = Σ wᵢ xᵢ / Σ wᵢ` (en firmware: posiciones 2,1,0,−1,−2 de IR1…IR5), `ω = Kp·e/n`

## 6. Sensores
- Ultrasonido: `ρ = cΔt/2` ; `c ≈ 331,3 + 0,606 T` (m/s, T en °C) ⇒ 343,4 m/s a 20 °C ; cuantización 25 µs ⇒ 4,29 mm
- Punto del eco: `x = ρ cos α` ; `y = ρ sin α` (más la posición real del sensor)
- Batería: `V_bat = ADC/(2ⁿ−1) · V_ref · (R_sup+R_inf)/R_inf` ; con 10k/10k el factor es 2

## 7. Estática, dinámica y vuelco
- `r_G = Σ mᵢ rᵢ/Σ mᵢ` ; `Wᵢ = mᵢ g` ; `g = 9,80665 m/s²`
- `τ_g = Σ J_v,iᵀ mᵢ g` ; `M(q) q̈ + C(q,q̇) q̇ + g(q) + τ_f = τ_act + Jᵀ F_ext`
- `M_est = Σ Wᵢ max(dᵢ,0)` ; `M_vol = Σ Wᵢ max(−dᵢ,0)` ; `FS = M_est/M_vol`
- `y_ZMP = y_CG + a h_CG/g` ; `a_crit = g (b − y_CG)/h_CG`
- Marcha: `F = M [a + g sin α + C_rr g cos α]` ; `τ_rueda ≈ F r/4`
- **Ejemplos del análisis del 01/11:** FS = 1,4855/1,4518 = 1,023 · a_crit = 9,80665·0,00085/0,16194 = 0,0515 m/s² ·
  STS3250: 16 kgf·cm = 1,569 N·m ; J2: 1,569 · 4 · 0,8 = 5,02 N·m (capacidad en articulación).

## 8. Conjunto
- `Q = (x, y, ψ, q₁…q₆)` ; `T_W,TCP = T_W,B(x,y,ψ) T_B,M T_M,TCP(q)`
- Aporte de la base a la velocidad lineal en el punto p: `(vₓ − ω p_y , v_y + ω pₓ , 0)` ; `V_TCP = J_base V_base + J_brazo q̇`

## 9. Conversiones
`1 mm = 10⁻³ m` · `1° = π/180 rad` · `1 rpm = 2π/60 rad/s` · `1 kgf·cm = 0,0980665 N·m`


===== FIN DEL ARCHIVO: ANEXO_Formulario.md =====

===== INICIO DEL ARCHIVO: ANEXO_Pendientes_y_alertas.md =====

# Anexo · Qué se puede afirmar y qué falta

Para exponer con precisión. «Medido» = con instrumento propio; «código» = leído en fuente, no ejecutado en el auto.

## 1. Lo que SÍ se puede afirmar

| Tema | Afirmación | Nivel |
| --- | --- | --- |
| Arquitectura | 6 módulos, un Git; firmware propio ATmega (C) y ESP (Arduino), Web React, Loader C# | código |
| Enlace | ROBO2 perfil 2 + EX01 v13; 54 comandos catalogados | código |
| Versión | 1.27.0 única para ATmega, ESP y Web | código |
| Odometría | 370 cpr, avance 1000 mm = 1000 mm | **medido** |
| Odometría lateral | 1000 mm contados = 775 mm reales (4 pruebas) → factor 0,775 | **medido** |
| Brazo | v17: 70/60/55/65/78,5 mm, PVC25/20, 5 STS3250 + 2 STS3032 | CAD |
| Masas | 2,345 kg brazo; 4,045 kg conjunto (con 1,5 kg de auto aprox. y 200 g) | CAD + estimación |
| Estabilidad | FS frontal 1,023 < 1,2; lateral 3,63; atrás 1,86 (01/11/2026) | cálculo sobre CAD |
| Simulador y misión | definidos y programados en la Web | código (simulación) |
| OLED + encoder | configuración validada con diagnóstico independiente (29/10/2026, confirmada por el usuario) | **medido/observado** |

## 2. Lo que NO hay que afirmar

- Que el robot «completo» (base + brazo) funciona físicamente. El brazo no está fabricado ni integrado eléctricamente (bus TTL de servos, 12 V/6 V, protecciones sin definir).
- Precisión, repetibilidad, velocidad máxima, carga útil o autonomía: no hay campaña de medición.
- Resistencia estructural: no hay FEA ni ensayos; PLA con E = 1500 MPa **hipotético**.
- Que KEY/RST automáticos funcionan: circuito sin armar/medir (estado 06/11/2026).
- Que el Loader flashea con hardware: no comprobado.
- Que los signos Mecanum/Hall están verificados: vienen de sentidos informados (`physicalVerification: false`).
- Que el GOTO llega a ±10 mm: es la **tolerancia de control**, no un error medido.
- Que el PCB del panel está validado: sin ERC/DRC; KiCad conserva +3V3 mientras el montaje usa 5 V.
- Que la envolvente de trabajo (burbuja) está calculada: pendiente.

## 3. Inconsistencias detectadas en el repositorio (resolver o aclarar al exponer)

1. **L (suma de semidistancias):** 170 mm (firmware/contrato) vs 220 mm (propuesta del simulador). Usar 170 mm.
2. **Eslabones del recolector:** texto con 130/120 mm (v02 histórico) vs v17 con 70/60. Usar v17.
3. **Textos «sin compilar»** de fichas viejas conviven con builds posteriores; el estado vigente está en `INFORME_MAESTRO.md` y los README de cada módulo.
4. **Plan GOTO** (08/11) dice «solo simulado»; desde 1.23.x existe envío por tramos.
5. **Capturas del Loader** son de la v1.2.0; el módulo está en 2.4.0 (fuente).
6. **Pinout PDF del 27/10** es histórico; usar `INFORME_PINOUT_ROBOT_20261106` y `PINOUT.md`.

## 4. Datos que faltan para completar la presentación

- Institución, asignatura, docentes, integrantes y fecha de exposición (portada).
- Foto real del prototipo y del panel (FOTO-02, 65) y de las pantallas OLED (FOTO-48…52).
- Cualquier medición nueva (peso real del auto y del brazo, deriva de rumbo, tiempo de ciclo) para reemplazar hipótesis.
- Confirmación de qué pin (D32/D33) maneja cada cable de color en el circuito NPN.

## 5. Plan de mediciones propuesto (si se quiere cerrar «Resultados» en la exposición)

Pesar auto + brazo + batería · medir CPR y signos por rueda · avance/lateral/giro de 1000 mm / 90° con cinta · deriva en 3 m · tensiones KEY/RST en
reposo y apretado · tiempo de carga del ESP con el circuito. Cada ensayo: hipótesis, instrumento, repeticiones, criterio de aceptación.

## 6. Sugerencia de tiempos (si la exposición dura 20 min)

Intro 1,5 · Modelos 1,5 · Pinout 1,5 · Firmwares 2 · Matemática auto 2 · GOTO 1,5 · Brazo/conjunto 2 · Esfuerzos 2 · Simulador 1,5 · Interfaz 1,5 ·
Loader 1 · NPN 1 · Encoder/OLED 1,5 ≈ 21 min. Para 15 min: filminas 3, 4 y 10 en un minuto cada una.


===== FIN DEL ARCHIVO: ANEXO_Pendientes_y_alertas.md =====