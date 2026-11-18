// AutoFinalRobo2 · presentación v3 (33 filminas + 2 anexos) según el informe maestro de Ingeniería Mecatrónica.
// Uso: node generar_presentacion_v3.js [salida.pptx]   (requiere pptxgenjs; NODE_PATH al node_modules donde esté instalado)
// Genera además ../GUION_HABLADO.md con el mismo texto que va en las notas del orador.
const pptxgen = require("pptxgenjs");
const fs = require("fs");
const path = require("path");
const V3 = path.resolve(__dirname, "..") + "/";
const REN = V3 + "renders/", DIA = V3 + "diagramas/", CAP = V3 + "capturas/", FOT = path.resolve(V3, "../fotos_existentes") + "/";
const OUT = process.argv[2] || V3 + "pptx/AutoFinalRobo2_Mecatronica_v3.pptx";

const C = { MEC: "42B6FF", ELE: "FFB454", CTL: "A78BFA", PRG: "55D6AA", BG: "0B1220", TX: "F4F7FC", T2: "A9B4C4", GR: "2A3448", PANEL: "111A2E", WARN: "FF6B81" };
const FONT = "Segoe UI", MONO = "Consolas";
const W = 13.333, H = 7.5, M = 0.7;
const pres = new pptxgen();
pres.layout = "LAYOUT_WIDE";
pres.title = "AutoFinalRobo2 · De la teoría a un sistema mecatrónico real";
pres.author = "Koba Chajchir";

// ---------- utilidades
function imgSize(p) {
  const b = fs.readFileSync(p);
  if (b[0] === 0x89) return [b.readUInt32BE(16), b.readUInt32BE(20)];
  let i = 2;
  while (i < b.length) {
    if (b[i] !== 0xFF) { i++; continue; }
    const m = b[i + 1], len = b.readUInt16BE(i + 2);
    if (m >= 0xC0 && m <= 0xC3) return [b.readUInt16BE(i + 7), b.readUInt16BE(i + 5)];
    i += 2 + len;
  }
  throw new Error("size " + p);
}
function img(s, p, x, y, w, h, align = "center") {
  if (!fs.existsSync(p)) { pending(s, "FALTA ARCHIVO: " + path.basename(p), x, y, w, h); return; }
  const [iw, ih] = imgSize(p), k = Math.min(w / iw, h / ih), ww = iw * k, hh = ih * k;
  const xx = align === "left" ? x : align === "right" ? x + w - ww : x + (w - ww) / 2;
  s.addImage({ path: p, x: xx, y: y + (h - hh) / 2, w: ww, h: hh });
  return { x: xx, y: y + (h - hh) / 2, w: ww, h: hh };
}
function text(s, t, x, y, w, h, o = {}) {
  s.addText(t, Object.assign({ x, y, w, h, fontFace: FONT, fontSize: 18, color: C.TX, margin: 0, valign: "top", isTextBox: true }, o));
}
function bullets(s, items, x, y, w, h, size = 18, color = C.TX) {
  s.addText(items.map(t => ({ text: t, options: { bullet: { code: "25AA" }, paraSpaceAfter: 6 } })),
    { x, y, w, h, fontFace: FONT, fontSize: size, color, margin: 0, valign: "top", isTextBox: true });
}
function panel(s, x, y, w, h, color = C.GR) {
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h, fill: { color: C.PANEL }, line: { color, width: 1.25 }, rectRadius: 0.08 });
}
function code(s, src, lines, x, y, w, h, size = 13) {
  panel(s, x, y, w, h);
  s.addText(lines.join("\n"), { x: x + 0.15, y: y + 0.12, w: w - 0.3, h: h - 0.5, fontFace: MONO, fontSize: size, color: "DCE6F5", margin: 0, valign: "top", isTextBox: true });
  s.addText(src, { x: x + 0.15, y: y + h - 0.36, w: w - 0.3, h: 0.28, fontFace: MONO, fontSize: 10.5, color: C.T2, margin: 0, isTextBox: true });
}
function chip(s, t, x, y, color, w) {
  const ww = w || Math.max(1.1, t.length * 0.105 + 0.3);
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w: ww, h: 0.34, fill: { color, transparency: 80 }, line: { color, width: 1 }, rectRadius: 0.06 });
  s.addText(t, { x, y, w: ww, h: 0.34, fontFace: FONT, fontSize: 12, bold: true, color, align: "center", valign: "middle", margin: 0, isTextBox: true });
  return ww;
}
function pending(s, t, x, y, w, h) {
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h, fill: { color: C.PANEL }, line: { color: C.WARN, width: 1.25, dashType: "dash" }, rectRadius: 0.08 });
  s.addText(t, { x: x + 0.2, y, w: w - 0.4, h, fontFace: FONT, fontSize: 15, color: C.WARN, align: "center", valign: "middle", margin: 0, isTextBox: true });
}
function eq(s, t, x, y, w, h, size = 24, color = C.TX) {
  s.addText(t, { x, y, w, h, fontFace: "Cambria Math", fontSize: size, color, margin: 0, valign: "middle", align: "center", isTextBox: true });
}
function table(s, rows, x, y, w, colW, size = 13) {
  const head = rows[0].map(t => ({ text: t, options: { bold: true, color: C.TX, fill: { color: "1C2639" } } }));
  const body = rows.slice(1).map(r => r.map(t => ({ text: String(t), options: { color: C.TX, fill: { color: C.PANEL } } })));
  s.addTable([head, ...body], { x, y, w, colW, fontFace: FONT, fontSize: size, border: { type: "solid", color: C.GR, pt: 0.75 }, margin: 0.05, valign: "middle" });
}

const BR = { M: ["MECÁNICA", C.MEC], E: ["ELECTRÓNICA", C.ELE], C: ["CONTROL", C.CTL], P: ["PROGRAMACIÓN", C.PRG] };
const GUION = [];
let n = 0;
function slide(d) {
  n++;
  const s = pres.addSlide();
  s.background = { color: C.BG };
  const act = d.ramas;
  // banda superior: degradé discreto solo si cruzan dos ramas -> dos segmentos de color
  const cols = act.map(k => BR[k][1]);
  cols.forEach((c, i) => s.addShape(pres.shapes.RECTANGLE, { x: i * W / cols.length, y: 0, w: W / cols.length, h: 0.07, fill: { color: c }, line: { color: c, width: 0 } }));
  if (!d.hero) {
    text(s, d.titulo, M, 0.32, 10.2, 0.62, { fontSize: 28, bold: true });
    if (d.clave) text(s, d.clave, M, 0.95, 10.4, 0.5, { fontSize: 18, color: C.T2 });
  }
  // minimapa: cuatro ramas, activas con su color (no en portada/cierre)
  if (!d.hero) ["M", "E", "C", "P"].forEach((k, i) => {
    const on = act.includes(k), c = on ? BR[k][1] : C.GR;
    s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x: 11.35 + i * 0.4, y: 0.36, w: 0.32, h: 0.32, fill: { color: c, transparency: on ? 0 : 0 }, line: { color: c, width: 0 }, rectRadius: 0.05 });
    s.addText(k, { x: 11.35 + i * 0.4, y: 0.36, w: 0.32, h: 0.32, fontFace: FONT, fontSize: 12, bold: true, color: on ? C.BG : "5B6780", align: "center", valign: "middle", margin: 0, isTextBox: true });
  });
  if (!d.hero) text(s, act.map(k => BR[k][0]).join(" + "), 11.0, 0.72, 1.95, 0.25, { fontSize: 9, color: C.T2, align: "right" });
  // pie curricular
  s.addShape(pres.shapes.LINE, { x: M, y: 6.92, w: W - 2 * M, h: 0, line: { color: C.GR, width: 0.75 } });
  text(s, d.pie, M, 6.98, 10.8, 0.3, { fontSize: 12, color: C.T2 });
  text(s, `${String(n).padStart(2, "0")}${d.id.startsWith("A") ? " · " + d.id : ""}`, W - M - 1.2, 6.98, 1.2, 0.3, { fontSize: 12, color: C.T2, align: "right" });
  d.build(s);
  const notes = `GUION (${d.t} s)\n${d.guion}\n\nFUENTES\n${d.fuentes}${d.pend ? "\n\nPENDIENTE / NO AFIRMAR\n" + d.pend : ""}`;
  s.addNotes(notes);
  GUION.push(d);
}

// ======================================================================================
// F01
slide({ id: "F01", ramas: ["M", "E", "C", "P"], hero: true, t: 20,
  titulo: "De la teoría a un sistema mecatrónico real",
  pie: "Introducción a la Ingeniería · Sistemas Mecatrónicos I/II · Proyecto de Ingeniería Mecatrónica",
  build: s => {
    img(s, REN + "F01_A01_RENDER_hero_v001.png", 0, 0.07, W, H - 0.6);
    text(s, "DE LA TEORÍA\nA UN SISTEMA\nMECATRÓNICO REAL", M, 1.7, 4.6, 2.0, { fontSize: 32, bold: true });
    text(s, "Diseño, construcción, control y programación de un vehículo robótico.", M, 3.85, 4.4, 0.9, { fontSize: 19, color: C.T2 });
    text(s, "AutoFinalRobo2 · Koba Chajchir", M, 4.9, 4.8, 0.4, { fontSize: 15, color: C.T2 });
  },
  guion: "No vamos a recorrer la carrera materia por materia. Vamos a recorrer un sistema que necesitó conocimientos de distintas materias para existir. Este auto es el hilo conductor: primero veremos cómo se construye, después cómo mide, cómo controla su comportamiento, cómo se programa y, al final, cómo todo eso se integra.",
  fuentes: "Render F01_A01 desde AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb (sha256 5bd9d51b…), cámara CAM_F01_HERO_V01 (renders/cameras_all.json)." });

// F02
slide({ id: "F02", ramas: ["M", "E", "C", "P"], t: 35, titulo: "¿Qué es la Mecatrónica?",
  clave: "Un sistema mecatrónico integra estructura, electrónica, control y procesamiento.",
  pie: "Las cuatro ramas son la taxonomía didáctica de esta charla · el Plan 2023 se organiza en cuatro bloques formales distintos",
  build: s => img(s, DIA + "F02_A01_VECTOR_cuatro_ramas_v001.png", M, 1.5, W - 2 * M, 5.3),
  guion: "Un sistema mecatrónico no es una suma de partes: es estructura que se mueve, electrónica que mide y actúa, control que regula ese comportamiento y software que procesa y comunica. Estas cuatro ramas son el lenguaje que vamos a usar hoy, con su color en cada filmina; no son los bloques formales del plan, que se organiza en Ciencias Básicas, Tecnologías Básicas, Aplicadas y Complementarias.",
  fuentes: "Res. C.S. 012/23 (Plan 2023): cuatro bloques formales; perfil del egresado (electrónica, mecánica, informática, control)." });

// F03
slide({ id: "F03", ramas: ["M", "E", "C", "P"], t: 70, titulo: "Árbol de conocimientos y correlatividades",
  clave: "Las materias no son islas: construyen recorridos que convergen.",
  pie: "Correlatividades para cursar según Res. C.D. 726/23 · R regularizada · A aprobada",
  build: s => img(s, DIA + "F03_A01_VECTOR_recorridos_curriculares_v001.png", M, 1.45, W - 2 * M, 5.4),
  guion: "Acá hay tres recorridos que el auto usa. Arriba, la mecánica: Estática y Resistencia de Materiales y Ecuaciones Diferenciales regularizadas, más Ciencias de los Materiales aprobada, llevan a Mecánica Racional, y esa junto con Sistemas de Control lleva a Robótica I. Abajo, la electrónica digital: Electrónica Digital y Laboratorio de Mediciones regularizadas, Electrónica Básica y Computación II aprobadas, habilitan Microcontroladores, y desde ahí Sistemas Embebidos y luego Sistemas Operativos y Redes. La letra en cada flecha es la condición oficial: R regularizada, A aprobada. La línea punteada no es una correlatividad: es un vínculo que agregamos nosotros porque en el auto el firmware embebido es el que ejecuta el control.",
  fuentes: "Aristas copiadas del informe maestro (C.D. 726/23): Mecánica Racional, Microcontroladores, Sistemas de Control, Sistemas Embebidos, Robótica I, Sist. Operativos y Redes.",
  pend: "Revisión humana obligatoria contra el PDF de la C.D. 726/23. El grafo completo del plan (todas las materias) no se generó: solo las aristas citadas." });

// F04
slide({ id: "F04", ramas: ["M", "E", "C", "P"], t: 45, titulo: "Arquitectura general del robot",
  clave: "Un sistema físico, dos procesadores, sensores, actuadores y una interfaz.",
  pie: "Sistemas Mecatrónicos I/II · Microcontroladores · Sistemas Embebidos · Sistemas Operativos y Redes",
  build: s => {
    img(s, REN + "F04_A01_RENDER_arch_iso_crop_v001.png", 0.2, 1.6, 3.9, 5.1);
    img(s, DIA + "F04_A02_VECTOR_arquitectura_v001.png", 4.1, 1.45, 9.0, 5.4);
  },
  guion: "Este es el mapa que vamos a recorrer. En el navegador corre una aplicación React. Se comunica por Wi-Fi con el ESP8266, que sirve los archivos de la aplicación y abre un WebSocket. El ESP traduce esos mensajes JSON a una trama binaria por UART hacia el ATmega2560, que es quien tiene la autoridad sobre los motores: ahí corren el control de velocidad, la odometría, la pantalla OLED y la lectura de sensores. Cada flecha de este diagrama tiene un protocolo y un archivo detrás.",
  fuentes: "AutoFinalRobo2-Contracts/contracts/UART_PROFILE_2.md, PROTOCOL.md; AutoFinalRobo2-Atmega/src/services/afr2_product.c (PID y odometría en ATmega); ESP/platformio.ini; Web/package.json." });

// F05
slide({ id: "F05", ramas: ["M"], t: 30, titulo: "Geometría de las ruedas Mecanum",
  clave: "La geometría condiciona el contacto, el montaje y el movimiento.",
  pie: "MECÁNICA · Sistemas de Representación I/II · Mecánica Racional",
  build: s => {
    img(s, REN + "F05_A01_RENDER_wheel_iso_crop_v001.png", 0.4, 1.6, 3.9, 4.0);
    img(s, REN + "F05_A01_RENDER_wheel_side_crop_v001.png", 4.3, 1.6, 3.4, 4.0);
    text(s, "Mano A y mano B, dos de cada una (modelo CAD)", 0.5, 5.75, 7.2, 0.4, { fontSize: 15, color: C.T2, align: "center" });
    panel(s, 8.0, 1.6, 4.6, 4.9, C.MEC);
    text(s, "Cinemática usada por la odometría", 8.2, 1.75, 4.2, 0.4, { fontSize: 16, bold: true, color: C.MEC });
    eq(s, "Δx = (d₀+d₁+d₂+d₃)/4", 8.1, 2.3, 4.4, 0.5, 20);
    eq(s, "Δy = (−d₀+d₁+d₂−d₃)/4", 8.1, 2.85, 4.4, 0.5, 20);
    eq(s, "Δθ = (−d₀+d₁−d₂+d₃) / (4·(a+b))", 8.1, 3.4, 4.4, 0.5, 18);
    eq(s, "dᵢ = ΔNᵢ · π · D / Nrev", 8.1, 4.0, 4.4, 0.5, 18);
    text(s, "a + b = 170 mm (62 + 107,66 nominal)\nD firmware = 80 mm · D en CAD ≈ 109 mm", 8.2, 4.65, 4.3, 0.8, { fontSize: 14, color: C.T2 });
    chip(s, "DISCREPANCIA CAD ↔ FIRMWARE: VERIFICAR D", 8.2, 5.75, C.WARN, 4.2);
  },
  guion: "Las ruedas son Mecanum: los rodillos están inclinados respecto del eje, y por eso combinando el sentido de las cuatro ruedas el auto puede avanzar, ir de costado o girar sobre sí mismo. Esa misma geometría aparece en el firmware: la odometría suma los avances de las cuatro ruedas para obtener el desplazamiento hacia adelante, los combina con signos alternados para el lateral, y divide por la suma de semidistancias, ciento setenta milímetros, para el giro.",
  fuentes: "afr2_product.c odometry() (fórmulas dx, dy, angle); afr2_geometry.h (AFR2_FIXED_WHEEL_MM 80, AFR2_FIXED_RADIUS_SUM_MM 170); render: bbox del objeto AFR2_Rueda80_ManoA:1 = 47 × 109,2 × 109,2 mm.",
  pend: "El diámetro del modelo GLB (≈109 mm) no coincide con la constante del firmware (80 mm). Medir la rueda física y corregir el que esté mal antes de presentar un diámetro. No afirmar ángulo de rodillos (no medido)." });

// F06
slide({ id: "F06", ramas: ["M"], t: 40, titulo: "De un perfil 2D a placas tridimensionales",
  clave: "Perfil 2D → geometría CAD → extrusión → pieza fabricable.",
  pie: "MECÁNICA · Sistemas de Representación II (CAD 2D/3D, planos de conjunto y despiece)",
  build: s => {
    const st = [["Perfil 2D", "contorno de la placa del kit"], ["Sketch CAD", "Fusion: importar / acotar"], ["Extrusión", "espesor de la placa"], ["Pieza", "agujeros y ranuras reales"]];
    st.forEach(([a, b], i) => {
      panel(s, M + i * 1.75, 1.75, 1.6, 1.5, C.MEC);
      text(s, a, M + i * 1.75 + 0.1, 1.9, 1.4, 0.4, { fontSize: 16, bold: true, align: "center" });
      text(s, b, M + i * 1.75 + 0.1, 2.35, 1.4, 0.8, { fontSize: 12, color: C.T2, align: "center" });
    });
    img(s, REN + "F10_A01_RENDER_top_ortho_v001.png", 7.6, 1.5, 5.3, 5.3);
    text(s, "Placas superior e inferior del modelo: los agujeros y ranuras deben coincidir con la pieza real.", M, 3.6, 6.8, 1.2, { fontSize: 17, color: C.T2 });
  },
  guion: "Las placas del chasis vienen del kit. Para modelarlas partimos del contorno en dos dimensiones, lo llevamos a un sketch en Fusion, lo extruimos con el espesor de la placa y verificamos que agujeros y ranuras coincidan con los de la pieza real. Es exactamente lo que se practica en Sistemas de Representación II: pasar de un dibujo plano a un sólido y después a un plano de conjunto.",
  fuentes: "AutoFinalRobo2-CAD/mecanica/auto-completo/fusion_ensamble.py, AutoFinalRobo2.f3d; objetos AFR2_CapaSuperior/AFR2_CapaInferior del GLB.",
  pend: "F06_A01/A02 (captura del SVG fuente y del timeline de Fusion) no se tomaron. Confirmar con qué archivo se generó el perfil antes de nombrarlo SVG. No afirmar material (acrílico/chapa) sin confirmarlo." });

// F07
slide({ id: "F07", ramas: ["M"], t: 40, titulo: "Componentes reutilizados y ensamblaje CAD",
  clave: "Diseñamos lo específico e integramos lo que ya existe.",
  pie: "MECÁNICA · Sistemas de Representación II · Proyecto de Ingeniería Mecatrónica",
  build: s => {
    img(s, REN + "F07_A01_RENDER_exploded_crop_v001.png", 0.2, 1.45, 7.6, 5.4);
    panel(s, 7.9, 1.6, 4.7, 2.2, C.MEC);
    text(s, "Diseño propio", 8.1, 1.7, 4.3, 0.4, { fontSize: 17, bold: true, color: C.MEC });
    bullets(s, ["Soporte y bisel del panel OLED + encoder", "PCB OLED/encoder (KiCad)", "Divisor de batería y su montaje"], 8.1, 2.15, 4.4, 1.6, 15);
    panel(s, 7.9, 4.0, 4.7, 2.6, C.GR);
    text(s, "Catálogo / biblioteca", 8.1, 4.1, 4.3, 0.4, { fontSize: 17, bold: true, color: C.T2 });
    bullets(s, ["Mega 2560 · OSOYOO Motor Driver Model Y V2 · OSOYOO WiFi Shield", "4 × GM25-370 · HC-SR04 · SG90 · línea 5 IR", "Ruedas Mecanum 80 · porta 2 × 18650"], 8.1, 4.55, 4.4, 2.0, 14);
  },
  guion: "El auto parte de un kit comercial. Lo que ya existía, como el Arduino Mega, los shields de motores y Wi-Fi, los motores, el ultrasónico o las ruedas, lo integramos desde modelos de biblioteca. Lo que no existía lo diseñamos: el soporte del panel de pantalla y perilla, su placa en KiCad y el divisor para medir la batería. La vista explotada es solo para mostrar las capas; no representa un orden de montaje.",
  fuentes: "Nombres de componente del GLB (AFR2_Mega2560_CAD, AFR2_OSOYOOMotorDriver_ModelY_V2, AFR2_OSOYOOWifiShield, AFR2_GM25_370 ×4, AFR2_HC_SR04, AFR2_SG90, AFR2_SensorLinea_5IR, AFR2_Rueda80_ManoA/B, AFR2_Celda_18650 ×2); CAD/docs/PANEL_UI_20261025.md.",
  pend: "Nombrar GrabCAD solo si se demuestra la procedencia de cada archivo (ver inventario CAD/reference/supplied). Explosión vertical: desplazamientos ilustrativos." });

// F08
slide({ id: "F08", ramas: ["M", "E"], t: 35, titulo: "Integrar OLED y encoder modifica la mecánica",
  clave: "Una nueva función electrónica también exige una solución mecánica.",
  pie: "MECÁNICA → ELECTRÓNICA · Sistemas de Representación II · Electrónica Digital",
  build: s => {
    img(s, REN + "F08_A01_RENDER_ui_mount_v001.png", 0.2, 1.45, 8.6, 5.4);
    bullets(s, ["Soporte con bisel y torretas M3 sobre la placa superior", "Perilla accesible desde atrás del auto", "PCB 20 × 35 mm ruteada en una cara", "Conectores 1×5 señales + 1×2 alimentación, 2,54 mm"], 9.0, 1.8, 3.7, 4.5, 16);
  },
  guion: "Agregar una pantalla y una perilla parece un cambio electrónico, pero obliga a resolver mecánica: dónde se fija, si se puede girar la perilla sin tocar las ruedas, cómo entra el conector. Diseñamos un soporte con bisel sobre la placa superior, con torretas para tornillos M3, y una placa de veinte por treinta y cinco milímetros.",
  fuentes: "CAD/docs/PANEL_UI_20261025.md (20×35 mm, B.Cu, torretas M3×15); AGENTS.md (conectores 1×5 y 1×2, 2,54 mm); render F08_A01 (objeto AFR2_AFR2_UI_B2_Fondo_OLED15_Auto5).",
  pend: "Montaje físico e interferencias del soporte sin verificar (según la ficha)." });

// F09
slide({ id: "F09", ramas: ["M"], t: 50, titulo: "Fuerzas y momentos en el brazo",
  clave: "La forma define dónde aparecen las solicitaciones.",
  pie: "MECÁNICA · Estática y Resistencia de Materiales · Mecánica Racional",
  build: s => {
    img(s, FOT + "FOTO-01_robot_completo_CAD_v17.png", 0.3, 1.5, 6.1, 4.0);
    img(s, FOT + "FOTO-30_cargas_y_momentos.png", 6.5, 1.5, 6.3, 4.0);
    eq(s, "𝐌 = 𝐫 × 𝐅        |𝐌| = F·d", 0.5, 5.65, 6.0, 0.6, 24);
    text(s, "N axial · V cortante · M flector · T torsor: se distinguen en cada sección.\nCarga terminal de diseño 200 g (análisis v07/v08).", 6.6, 5.6, 6.1, 1.0, { fontSize: 15, color: C.T2 });
  },
  guion: "El brazo es donde la estática se vuelve concreta. Cada peso y la carga en la pinza generan, respecto de cada articulación, un momento igual a la fuerza por el brazo de palanca. Por eso la base y el primer eslabón son los más exigidos: ahí se acumulan todos los momentos. En cada sección hay que separar la fuerza axial, el cortante, el momento flector y el torsor; no son lo mismo y no se dimensionan igual.",
  fuentes: "CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g/INFORME_ITERACION_DISENO.md; revision_v17/README.md; fotos_existentes/FOTO-01, FOTO-30.",
  pend: "El brazo no está aprobado para fabricación ni resistencia (J1 abierto). No mostrar valores numéricos de momento sin la hoja de equilibrio. F09_A01 (lateral ortográfica del brazo sobre el auto) no se renderizó: el GLB del auto no incluye el brazo montado." });

// F10
slide({ id: "F10", ramas: ["M", "C"], t: 50, titulo: "Centro de masa, inercia y vuelco",
  clave: "Mover masa cambia la dinámica y el margen de estabilidad.",
  pie: "MECÁNICA + CONTROL · Mecánica Racional · Robótica I",
  build: s => {
    img(s, REN + "F09_A01_RENDER_side_ortho_v001.png", 0.3, 1.5, 6.2, 3.5);
    img(s, FOT + "FOTO-29_estabilidad_planta_CG.png", 6.6, 1.5, 6.2, 3.5);
    eq(s, "𝐫G = Σ mᵢ 𝐫ᵢ / Σ mᵢ", 0.4, 5.2, 4.0, 0.6, 22);
    eq(s, "J = ∫ r⊥² dm", 4.4, 5.2, 3.2, 0.6, 22);
    eq(s, "𝐅ᵢ = −m 𝐚   (D'Alembert)", 7.6, 5.2, 5.0, 0.6, 22);
    text(s, "Vuelco: comparar momentos estabilizadores y volcantes respecto del borde de apoyo crítico.", 0.5, 6.0, 12.2, 0.5, { fontSize: 15, color: C.T2, align: "center" });
  },
  guion: "Cuando el brazo se estira, el centro de masa del conjunto se desplaza. Mientras su proyección quede dentro del polígono que forman los contactos de las ruedas, el auto no vuelca. Si además frenamos o aceleramos, sumamos una fuerza inercial equivalente, menos masa por aceleración: es la construcción de D'Alembert, no una fuerza física nueva. El criterio no es mirar el dibujo: es comparar los momentos respecto del borde crítico.",
  fuentes: "CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v13/v14 README (estabilidad condicionada a contactos hipotéticos); fotos_existentes/FOTO-29.",
  pend: "Contactos reales y masa medida del auto pendientes: no declarar vuelco ni margen numérico." });

// F11
slide({ id: "F11", ramas: ["E"], t: 40, titulo: "Del datasheet al circuito",
  clave: "El datasheet convierte un componente en restricciones de diseño.",
  pie: "ELECTRÓNICA · Electrónica Básica · Laboratorio de Mediciones · Microcontroladores",
  build: s => {
    img(s, FOT + "FOTO-12_shield_osoyoo_pinout.jpg", 0.3, 1.5, 6.0, 5.3);
    img(s, CAP + "sensorbateria_crop.png", 6.5, 1.5, 6.3, 4.4);
    text(s, "Esquemático real exportado de KiCad (sensorbateria.kicad_sch)", 6.5, 6.0, 6.3, 0.4, { fontSize: 13, color: C.T2, align: "center" });
  },
  guion: "Antes de conectar algo leemos su hoja de datos: qué tensión tolera cada pin, qué corriente entrega, qué nivel lógico espera. Por ejemplo, el ADC del ATmega no acepta más tensión que su referencia de cinco volts, y eso define el circuito que vamos a ver para medir la batería. A la izquierda, el pinout del shield; a la derecha, el esquemático real tal como está en KiCad.",
  fuentes: "fotos_existentes/FOTO-12; AutoFinalRobo2-CAD/kicad/oled-encoder/sensorbateria.kicad_sch exportado con kicad-cli 10.99 (capturas/sensorbateria.png).",
  pend: "F11_A01: captura del datasheet oficial (pinout + límites) no tomada; guardar URL y revisión del documento." });

// F12
slide({ id: "F12", ramas: ["E", "M"], t: 40, titulo: "De KiCad al ensamblaje",
  clave: "Esquemático → PCB → modelo 3D → integración física.",
  pie: "ELECTRÓNICA → MECÁNICA · Electrónica Digital · Sistemas de Representación II",
  build: s => {
    img(s, CAP + "PlacaOLED-Encoder-AutoRobo2_crop.png", 0.3, 1.5, 4.2, 4.6);
    img(s, CAP + "F12_A02_RENDER_kicad_pcb_v001.png", 4.6, 1.5, 4.2, 4.6);
    img(s, path.resolve(V3, "../../AutoFinalRobo2-CAD/exports/oled-encoder/Panel_UI_Fusion_A.png"), 8.9, 1.5, 4.1, 4.6);
    ["Esquemático (KiCad)", "Visor 3D (kicad-cli render)", "Panel en Fusion"].forEach((t, i) => text(s, t, 0.3 + i * 4.3, 6.2, 4.2, 0.4, { fontSize: 14, color: C.T2, align: "center" }));
  },
  guion: "La placa del panel nace en KiCad: primero el esquemático, después el ruteo, y como cada huella tiene su modelo tridimensional, KiCad nos da la placa armada en 3D. Ese modelo se exporta en STEP y entra a Fusion para verificar que encaje en el soporte. La placa se fabricó por transferencia y ácido, con componentes de agujero pasante soldados a mano.",
  fuentes: "AutoFinalRobo2-CAD/kicad/oled-encoder/PlacaOLED-Encoder-AutoRobo2.kicad_sch/.kicad_pcb (kicad-cli 10.99: sch export svg, pcb render); exports/oled-encoder/PCB_UI_Final_20261025.step, Panel_UI_Fusion_A.png.",
  pend: "Fabricación por ácido y soldadura TH: DECLARADO por el equipo (ideas de la charla), sin foto. Sin ERC/DRC registrados." });

// F13
slide({ id: "F13", ramas: ["E"], t: 40, titulo: "Batería y ADC",
  clave: "Adaptamos una tensión para poder medirla.",
  pie: "ELECTRÓNICA · Electrónica Básica · Laboratorio de Mediciones · Microcontroladores",
  build: s => {
    img(s, DIA + "F13_A01_VECTOR_divisor_bateria_v001.png", 0.2, 1.45, 8.2, 4.7);
    code(s, "Atmega/src/platform/afr2_sensors.c:30", [
      "uint16_t Afr2Battery_Millivolts(const Afr2Sensors *s){",
      "  return ((uint32_t)s->battery_raw*10000u+512u)/1024u;",
      "}", "", "// ADMUX = REFS0 (AVCC) · prescaler 128",
      "// descarta 1.ª conversión, promedia 8 muestras",
      "// raw = 1023 en alguna muestra → fuera de rango"], 8.5, 1.6, 4.4, 3.6, 11.5);
    text(s, "Dos resistencias iguales dividen la tensión por 2: el pack de dos celdas en serie queda dentro de los 5 V del ADC.", 8.5, 5.35, 4.4, 1.3, { fontSize: 14, color: C.T2 });
  },
  guion: "Las dos celdas 18650 en serie dan más de cinco volts, y el ADC del ATmega no puede medir más que su referencia. La solución es un divisor con dos resistencias iguales de diez kilo: en el punto medio tenemos exactamente la mitad, con un capacitor de cien nanofaradios para filtrar. El ADC de diez bits convierte eso en un número de cero a mil veintitrés; como dividimos por dos, el firmware multiplica por diez mil en vez de cinco mil y divide por mil veinticuatro. Dos detalles: es una tensión, no un porcentaje de carga, y todavía no está calibrada.",
  fuentes: "contracts/BATTERY_MEASUREMENT_9.md (10 kΩ+10 kΩ, 100 nF, A5/PF5/ADC5, fórmula, prescaler 128, 8 muestras); afr2_sensors.c:30; kicad sensorbateria.kicad_sch.",
  pend: "Montaje y calibración no comprobados (según contrato). Rango 2×4,2 V es DERIVADO de la química nominal, no medido." });

// F14
slide({ id: "F14", ramas: ["E", "C"], t: 45, titulo: "Encoders Hall y cuadratura",
  clave: "Los pulsos convierten movimiento físico en información medible.",
  pie: "ELECTRÓNICA → CONTROL · Electrónica Digital · Microcontroladores · Sistemas Embebidos",
  build: s => {
    img(s, REN + "F14_A01_RENDER_motor_gm25_crop_v001.png", 0.3, 1.5, 3.8, 2.8);
    img(s, DIA + "F14_A02_VECTOR_cuadratura_hall_v001.png", 4.2, 1.45, 8.7, 3.25);
    code(s, "Atmega/src/platform/afr2_board.c:40 · ISR(PCINT2_vect)", [
      "if(rising&_BV(AFR2_HALL_FL_A_BIT))",
      "  hall_counts[0] += (k&_BV(AFR2_HALL_FL_B_BIT)) ? -1 : +1;"], 0.3, 4.75, 7.6, 1.5, 13);
    eq(s, "ω ≈ 2π·ΔN / (Nrev·Δt)", 8.1, 4.8, 4.8, 0.6, 24);
    text(s, "Nrev = 370 pulsos/vuelta, medido a mano por el equipo, igual en las 4 ruedas", 8.1, 5.45, 4.8, 0.8, { fontSize: 13.5, color: C.T2, align: "center" });
  },
  guion: "Cada motor GM25-370 tiene un encoder Hall con dos canales, A y B, desfasados un cuarto de ciclo. El firmware cuenta solo los flancos de subida de A, con una interrupción por cambio de pin, y mira en ese instante el nivel de B: si está alto resta, si está bajo suma. Así sabemos cuánto giró y hacia dónde. Contando pulsos en un intervalo de tiempo obtenemos la velocidad angular. La resolución de trescientos setenta pulsos por vuelta la medimos girando la rueda a mano.",
  fuentes: "afr2_board.c:40-47 (hall_capture, ISR PCINT2, A8–A15); include/afr2_product.h:17 (AFR2_DEFAULT_CPR 370, medido por el usuario); contracts/hardware/PINOUT.md.",
  pend: "370 es DECLARADO (medición manual del usuario): mostrarlo como tal. El código real usa suma en u32 con UINT32_MAX para restar; el snippet lo simplifica a ±1 (indicarlo si se pregunta)." });

// F15
slide({ id: "F15", ramas: ["E"], t: 45, titulo: "Ultrasonido: tiempo de vuelo",
  clave: "Medimos distancia midiendo el tiempo de ida y vuelta del sonido.",
  pie: "ELECTRÓNICA · Física III (ondas) · Microcontroladores · Laboratorio de Mediciones",
  build: s => {
    img(s, REN + "F15_A01_RENDER_us_tof_v001.png", 0.3, 1.5, 6.0, 3.4);
    img(s, DIA + "F15_A02_VECTOR_tof_tiempos_v001.png", 6.4, 1.45, 6.5, 2.45);
    code(s, "Atmega/src/platform/afr2_sensors.c:136 · ISR(TIMER3_COMPA_vect)", [
      "++echo_ticks;                         // un tick = 25 µs",
      "if(echo_state==2 && (PINC&_BV(PC6))) { echo_rise=echo_ticks; echo_state=3; }",
      "else if(echo_state==3 && !(PINC&_BV(PC6))) {",
      "  echo_width=echo_ticks-echo_rise; echo_done=1; }",
      "if(echo_ticks>=1280u && echo_state)  // 32 ms: timeout sin bloquear",
      "  echo_error = echo_state==3 ? AFR2_ECHO_NO_FALL : AFR2_ECHO_NO_RISE;"], 0.3, 5.0, 12.6, 1.8, 12);
    text(s, "El obstáculo es geometría de escena, no parte del robot.", 6.5, 4.1, 6.3, 0.4, { fontSize: 13, color: C.T2 });
  },
  guion: "El HC-SR04 emite un pulso de ultrasonido y devuelve por el pin de eco un pulso cuyo ancho es el tiempo que tardó el sonido en ir y volver. La distancia es la velocidad del sonido por ese tiempo, dividido dos. En el firmware no esperamos con un bucle: un temporizador interrumpe cada veinticinco microsegundos, detecta el flanco de subida y el de bajada del eco, y si en treinta y dos milisegundos no llegó, informa el error sin trabar el resto del programa. El sensor va sobre un servo, y con eso armamos el radar.",
  fuentes: "afr2_sensors.c:119-148 (Timer3 CTC, OCR3A=49, ÷8 → 25 µs; timeout 1280 ticks; TRIG PC7/D30, ECHO PC6/D31); contracts/SENSORS_RADAR_8.md.",
  pend: "Temperatura/velocidad del sonido asumida en la conversión: verificar en el código de conversión antes de mostrar un valor de c." });

// F16
slide({ id: "F16", ramas: ["E", "C"], t: 45, titulo: "Sensores de línea",
  clave: "La línea se detecta como una diferencia de reflectancia.",
  pie: "ELECTRÓNICA → CONTROL · Física III (óptica) · Sistemas de Control",
  build: s => {
    img(s, REN + "F16_A01_RENDER_line_low_v001.png", 0.3, 1.5, 6.0, 3.4);
    img(s, DIA + "F16_A02_VECTOR_linea_pesos_v001.png", 6.4, 1.45, 6.5, 2.45);
    code(s, "Atmega/src/services/afr2_product.c:383", [
      "int16_t error=0,n=0;",
      "for(uint8_t i=0;i<5;i++) if(p->sensors.line_bits&(1u<<i)){ error+=2-(int16_t)i; n++; }",
      "wz=(int16_t)((int32_t)error*p->config.line_kp/n);   // giro proporcional",
      "drive_closed_loop(p, p->config.line_speed, 0, wz, 500);"], 0.3, 5.0, 12.6, 1.6, 12);
    text(s, "IR1 (bit 0, A4) a la izquierda · línea a la izquierda ⇒ giro antihorario", 6.5, 4.1, 6.3, 0.5, { fontSize: 13, color: C.T2 });
  },
  guion: "Cinco emisores y receptores infrarrojos miran el piso. La cinta oscura refleja menos que el piso claro, y cada sensor entrega un bit. El firmware le asigna a cada sensor un peso, de más dos a menos dos, promedia los que ven cinta y obtiene un error: cuánto está la línea corrida respecto del centro. Ese error multiplicado por una ganancia es el giro que se le pide al auto, mientras avanza a velocidad constante. Si deja de ver la línea, se detiene.",
  fuentes: "afr2_product.c:370-388 (pesos 2−i, line_kp, parada sin línea y por cruce ≥150 ms); line_speed 80 y line_kp 100 por defecto (afr2_product.c:27).",
  pend: "Polaridad analógica (blanco = ADC alto) no verificada: el modo mostrado es digital. F16_A03 sin lecturas reales: no mostrar números." });

// F17
slide({ id: "F17", ramas: ["E", "P"], t: 35, titulo: "I²C y pantalla OLED",
  clave: "Un bus sincroniza datos y reloj para comunicar periféricos.",
  pie: "ELECTRÓNICA → PROGRAMACIÓN · Sistemas Embebidos (USART, I²C, SPI, GPIO, ADC, timers)",
  build: s => {
    img(s, DIA + "F17_A02_VECTOR_i2c_oled_v001.png", 0.2, 1.45, 8.4, 3.2);
    img(s, FOT + "FOTO-53_autito_silueta_OLED_128x64.png", 8.8, 1.6, 4.0, 2.9);
    bullets(s, ["Framebuffer de 1024 bytes (128 × 64 / 8) y una copia para enviar solo lo que cambió", "Encoder KY-040: CLK D2 / DT D3 por interrupción, SW D42 · pull-ups 10 kΩ a 3V3 en la PCB", "Pulsación corta confirma · larga (≥ 800 ms, al soltar) abre controles rápidos"], 0.5, 4.85, 12.3, 1.9, 16);
  },
  guion: "La pantalla habla I²C: dos cables, uno de datos y uno de reloj, que el maestro, el ATmega, genera a cien kilohertz. Como es el único dispositivo del bus, un administrador de I²C es el dueño de las transferencias y nunca se escribe desde una interrupción. La imagen vive en un buffer de mil veinticuatro bytes, uno por cada ocho píxeles, y se manda por bloques. La perilla es un encoder rotativo leído por interrupciones: corto confirma, largo abre los controles rápidos.",
  fuentes: "afr2_twi.c:42 (TWBR=72 → 100 kHz); afr2_oled.c (SSD1306, 128×64 páginas); builds/2026-11-06-04 (buffer 1024 + sombra ACK 1024); AGENTS.md (navegación); afr2_board.c (INT4/INT5).",
  pend: "Dirección I²C y tasa real de actualización (F17_A03) no medidas: no mostrar FPS ni dirección. El esquemático alimenta el panel a 3V3 y anota «adaptación de niveles I²C con Mega 5 V: pendiente externa»; la ficha de pinout reporta el panel a 5 V. Resolver antes de presentar la tensión del bus." });

// F18
slide({ id: "F18", ramas: ["E", "C"], t: 40, titulo: "Motor DC y servo: dos usos de señales temporizadas",
  clave: "Se parecen eléctricamente; no significan lo mismo.",
  pie: "ELECTRÓNICA → CONTROL · Microcontroladores · Sistemas Embebidos · Máquinas eléctricas",
  build: s => {
    img(s, REN + "F14_A01_RENDER_motor_gm25_crop_v001.png", 0.3, 1.55, 2.9, 2.2);
    img(s, REN + "F18_A01_RENDER_servo_sg90_crop_v001.png", 0.3, 3.95, 2.9, 2.6);
    img(s, DIA + "F18_A02_VECTOR_pwm_vs_servo_v001.png", 3.3, 1.5, 9.6, 3.6);
    bullets(s, ["Motor: el duty fija la tensión media → velocidad", "Servo: el ancho del pulso (1–2 ms cada 20 ms) fija el ángulo"], 3.6, 5.3, 9.2, 1.3, 17);
  },
  guion: "El motor y el servo reciben pulsos, pero leen cosas distintas. Al motor le llega un PWM de casi mil hertz: lo que importa es qué fracción del período está encendido, porque eso fija la tensión media y por lo tanto la velocidad. Al servo le llega un pulso cada veinte milisegundos, y lo que importa es el ancho: entre uno y dos milisegundos, que el servo traduce en un ángulo de cero a ciento ochenta grados.",
  fuentes: "afr2_motor.c:21-23 (Fast PWM 8 bits, ÷64 → 976 Hz, Timer1/Timer2; OCR por rueda); afr2_sensors.c:149-170 (Timer5, 0,5 µs, 40000 ticks = 20 ms, 1000–2000 µs, PB7/D13).",
  pend: "Frecuencia PWM 976 Hz es la configurada en el timer (comentario: «candidate»), no medida con osciloscopio." });

// F19
slide({ id: "F19", ramas: ["C"], t: 45, titulo: "Lazo abierto y lazo cerrado",
  clave: "Realimentar es comparar lo pedido con lo medido.",
  pie: "CONTROL · Sistemas y Señales · Sistemas de Control (modelado, controladores, sintonización)",
  build: s => {
    img(s, DIA + "F19_A01_VECTOR_lazo_abierto_cerrado_v001.png", 0.3, 1.45, 9.1, 4.3);
    eq(s, "e(t) = r(t) − y(t)", 9.5, 2.1, 3.4, 0.7, 26, C.CTL);
    text(s, "El auto tiene ambos: PWM manual y prueba de rueda en lazo abierto; Manual, Línea y GOTO en lazo cerrado.", 9.5, 3.0, 3.4, 2.5, { fontSize: 15, color: C.T2 });
  },
  guion: "En lazo abierto mandamos un PWM y confiamos en que el motor gire a la velocidad esperada. Pero cada motor es distinto, la batería baja y el piso cambia. En lazo cerrado medimos la velocidad con el encoder, la comparamos con la pedida y corregimos con esa diferencia, el error. El auto tiene los dos: el modo PWM manual es lazo abierto, y los modos de conducción, línea y GOTO cierran el lazo.",
  fuentes: "afr2_product.c:420-450 (PID solo si modo ≠ WHEEL_TEST y ≠ PWM_MANUAL); afr2_product.c:428-431." });

// F20
slide({ id: "F20", ramas: ["C", "P"], t: 55, titulo: "El controlador se convierte en firmware",
  clave: "El diagrama de bloques termina ejecutándose como instrucciones periódicas.",
  pie: "CONTROL → PROGRAMACIÓN · Sistemas de Control · Sistemas Embebidos · Computación II",
  build: s => {
    code(s, "Atmega/src/services/afr2_product.c:433 · una vez por rueda, cada 10 ms (main.c:119)", [
      "int16_t error = clamp((int32_t)p->target[i] - p->speed[i], 2000);",
      "p->integral[i] += error;                       // I",
      "if(p->integral[i] >  32767) p->integral[i] =  32767;   // anti-windup",
      "if(p->integral[i] < -32767) p->integral[i] = -32767;",
      "int32_t output = (int32_t)p->config.kp*error",
      "               + (int32_t)p->config.ki*p->integral[i]",
      "               + (int32_t)p->config.kd*(error - p->last_error[i]);",
      "int32_t trimmed = ((output/256)*(int32_t)p->config.motor_trim[i] + ...)/1000;",
      "wanted = clamp(trimmed, p->config.pwm_limit);   // saturación",
      "p->pwm[i] = ramp(p->pwm[i], wanted, p->config.ramp_step);  // rampa",
      "Afr2Motor_Write(p->pwm, p->config.motor_signs);"], 0.3, 1.5, 8.4, 4.7, 12.5);
    panel(s, 8.9, 1.5, 4.0, 4.7, C.CTL);
    text(s, "Valores por defecto", 9.1, 1.65, 3.6, 0.4, { fontSize: 16, bold: true, color: C.CTL });
    table(s, [["Parámetro", "Valor"], ["kp / ki / kd", "128 / 8 / 0 (÷256)"], ["Periodo", "10 ms"], ["Rampa", "4 PWM/ciclo"], ["Límite", "pwm_limit 255"], ["Integral", "±32767"]], 9.1, 2.15, 3.6, [1.6, 2.0], 13);
    text(s, "Es un PI entero en el ATmega: sin punto flotante en el lazo.", 9.1, 4.8, 3.6, 1.2, { fontSize: 14, color: C.T2 });
    pending(s, "F20_A03 · gráfico referencia vs. velocidad medida: falta log experimental", 0.3, 6.3, 8.4, 0.5);
  },
  guion: "Este es el controlador que dibujamos en bloques, escrito en C. Cada diez milisegundos, para cada rueda, se calcula el error entre la velocidad pedida y la medida, se acumula la integral con un límite para que no crezca sin fin, se combinan proporcional, integral y derivativo, se aplica una corrección por motor, se satura al PWM máximo y se pasa por una rampa para no dar tirones. Con los valores por defecto la derivativa es cero, así que en la práctica es un PI. Todo con enteros, porque el ATmega no tiene unidad de punto flotante.",
  fuentes: "afr2_product.c:12-27 (defaults kp=128, ki=8, kd=0, ramp_step=4, pwm_limit=255), :243 ramp(), :428-445 lazo; main.c:119 (control_ms ≥ 10).",
  pend: "Sin log experimental: el gráfico de respuesta queda BLOQUEADO. Los valores de ganancias pueden haberse modificado por EEPROM en el auto real." });

// F21
slide({ id: "F21", ramas: ["C"], t: 30, titulo: "Motores diferentes, sistema real",
  clave: "El control corrige diferencias; no elimina límites físicos.",
  pie: "CONTROL · Sistemas de Control · Máquinas eléctricas · Laboratorio de Mediciones",
  build: s => {
    img(s, REN + "F09_A01_RENDER_side_ortho_v001.png", 0.3, 1.5, 6.4, 3.6);
    bullets(s, ["motor_trim por rueda: 500–1500 ‰ corrige diferencias de ganancia", "Factores de escala por dirección en la odometría", "Si una rueda con PWM no cambia su Hall en 500 ms → falla y STOP", "Signo PWM/Hall cruzado = realimentación positiva → STOP"], 6.9, 1.6, 6.0, 3.6, 16);
    pending(s, "F21_A02 · PWM y velocidad de las 4 ruedas con la misma consigna: ensayo pendiente", 0.3, 5.5, 12.6, 0.7);
  },
  guion: "Cuatro motores iguales en el catálogo no son iguales en la realidad. Por eso cada rueda tiene un factor de corrección y la odometría tiene factores por dirección. Pero el control tiene límites: si el PWM llega al máximo, no hay más corrección posible. Y el firmware se protege: si una rueda recibe potencia y su encoder no se mueve, o si el signo del encoder quedó invertido, lo que generaría una realimentación positiva, se detiene todo.",
  fuentes: "afr2_product.c:51 (motor_trim 500–1500), :297 (odom_scale por dirección, AFR2-20261111-06), :446 (fault 3 sin cambio Hall 500 ms), :424 (AFR2_FAULT_SIGN_MISMATCH).",
  pend: "No concluir «motor defectuoso» sin ensayo." });

// F22
slide({ id: "F22", ramas: ["P", "E"], t: 35, titulo: "ATmega2560 y ESP8266",
  clave: "Distribuimos responsabilidades entre procesamiento físico y conectividad.",
  pie: "PROGRAMACIÓN + ELECTRÓNICA · Microcontroladores · Sistemas Embebidos",
  build: s => {
    img(s, REN + "F22_A01_RENDER_stack_crop_v001.png", 0.3, 1.5, 4.6, 3.2);
    table(s, [["", "ATmega2560 (Mega 2560)", "ESP8266 (ESP-12)"],
      ["Arquitectura", "AVR 8 bits · 16 MHz", "Xtensa L106 32 bits"],
      ["Memoria (chip)", "Flash 256 KiB · SRAM 8 KiB", "RAM de datos ≈ 80 KiB · flash externa"],
      ["Programación", "C avr-libc, Makefile, avr-gcc", "C++17, Arduino core, PlatformIO"],
      ["Periféricos usados", "Timers, PWM, ADC, USART1, TWI, PCINT", "Wi-Fi, LittleFS, UART"],
      ["Responsabilidad", "Salidas físicas, control, sensores, OLED", "Web, WebSocket, red, broker"]], 5.1, 1.55, 7.8, [1.7, 3.05, 3.05], 13);
    text(s, "Flash del ESP-12: layout 4 MiB / 3 MiB LittleFS es candidato; el chip físico aún no se identificó.", 0.4, 5.0, 4.5, 1.3, { fontSize: 13, color: C.T2 });
  },
  guion: "Usamos dos procesadores porque cada uno es bueno en algo. El ATmega2560 es un microcontrolador de ocho bits con muchos temporizadores y pines: tiene la autoridad sobre motores, sensores y pantalla, con tiempos predecibles. El ESP8266 tiene Wi-Fi y más memoria: sirve la aplicación web y hace de puente. Si se cae la red, el ATmega sigue controlando el auto de forma segura.",
  fuentes: "Datasheets Microchip ATmega2560 y Espressif ESP8266EX (capacidades del chip); Atmega/Makefile (-mmcu=atmega2560, F_CPU 16 MHz); ESP/platformio.ini (esp12e, eagle.flash.4m3m.ld, «candidate only»).",
  pend: "Modelo exacto del módulo ESP y su flash no identificados físicamente (platformio.ini lo dice)." });

// F23
slide({ id: "F23", ramas: ["P"], t: 30, titulo: "C, Arduino Core y abstracción",
  clave: "Abstraer no elimina el hardware; cambia cómo trabajamos con él.",
  pie: "PROGRAMACIÓN · Computación I/II · Microcontroladores · Sistemas Embebidos",
  build: s => {
    img(s, DIA + "F23_A02_VECTOR_capas_abstraccion_v001.png", 0.2, 1.45, 6.6, 2.8);
    code(s, "Atmega/src/platform/afr2_uart1.c:13 · registros", [
      "UCSR1A=_BV(U2X1); UBRR1H=0; UBRR1L=16;   // 115200",
      "UCSR1C=_BV(UCSZ11)|_BV(UCSZ10);          // 8N1",
      "UCSR1B=_BV(RXEN1)|_BV(TXEN1)|_BV(RXCIE1);"], 6.9, 1.5, 6.0, 1.9, 12.5);
    code(s, "ESP/src/wifi_service.cpp:98 · framework", [
      "bool modeOk = WiFi.mode(staWanted_ ? WIFI_AP_STA : WIFI_AP);",
      "apOn_ = modeOk && WiFi.softAP(apSsid_.c_str(), nullptr, 1, false, 4);"], 6.9, 3.55, 6.0, 1.5, 12.5);
    text(s, "La misma idea, «configurar un periférico», en dos niveles: escribir bits de un registro o llamar a una API.", 0.4, 4.6, 6.3, 1.4, { fontSize: 16, color: C.T2 });
  },
  guion: "En el ATmega programamos en C sobre los registros: para abrir la UART escribimos directamente los bits que fijan la velocidad, el formato y las interrupciones. En el ESP usamos el núcleo Arduino: una sola llamada configura el Wi-Fi como punto de acceso o como estación. Abstraer nos hace más rápidos, pero el hardware sigue ahí: alguien tiene que saber qué hace esa llamada por debajo.",
  fuentes: "afr2_uart1.c:13-19; wifi_service.cpp:98-99; AGENTS.md (ATmega sin Arduino ni DMA)." });

// F24
slide({ id: "F24", ramas: ["P"], t: 35, titulo: "Wi-Fi, HTTP y carga de la aplicación",
  clave: "Conectarse a Wi-Fi no es lo mismo que pedir una aplicación web.",
  pie: "PROGRAMACIÓN · Sistemas Operativos y Redes de Comunicación",
  build: s => {
    img(s, DIA + "F24_A01_VECTOR_secuencia_http_v001.png", 0.2, 1.45, 7.0, 3.6);
    code(s, "AutoFinalRobo2-Web/package.json · scripts", [
      "npm run dev       # Vite dev server con HMR",
      "                  # (predev: catálogo 3D + lib. OLED)",
      "npm run build     # tsc -b + vite build + paquete «full»",
      "npm run build:esp # --mode esp + gzip + manifest",
      "                  # + copia a ESP/data/dist",
      "npm run preview   # sirve el build local"], 7.3, 1.5, 5.6, 3.1, 11.5);
    table(s, [["Web", "Versión"], ["React / react-dom", "^19.2.0"], ["Vite", "^7.1.7"], ["TypeScript", "~5.9.3"], ["three / R3F", "^0.180 / ^9.4"], ["Tailwind CSS", "^4.1.14"]], 7.3, 4.75, 2.7, [1.6, 1.1], 11);
    table(s, [["ESP (PlatformIO)", "Versión"], ["espressif8266", "4.2.1"], ["ESPAsyncWebServer", "3.10.3"], ["ArduinoJson", "7.4.2"], ["C++", "gnu++17"], ["Producto", "1.39.6"]], 10.15, 4.75, 2.75, [1.65, 1.1], 11);
    text(s, "Modo AP propio o STA en la red del usuario (AP+STA mientras se conecta).", 0.4, 5.3, 6.8, 0.8, { fontSize: 14, color: C.T2 });
  },
  guion: "Primero el teléfono o la computadora se asocia a la red: o al punto de acceso del propio auto, o a la red de casa donde también está el auto. Recién después el navegador pide la página por HTTP. El ESP responde con los archivos ya comprimidos en gzip desde su memoria, y cuando la aplicación arranca abre un WebSocket para el tiempo real. Esos archivos los generamos con Vite: con npm run dev trabajamos en la computadora con recarga instantánea, y con npm run build:esp se compila, se comprime y se copia a la carpeta que se graba en el ESP.",
  fuentes: "Web/package.json (scripts y versiones declaradas en el manifest); ESP/platformio.ini; contracts/PROTOCOL.md (HTTP/LittleFS: gzip obligatorio, 406, no-store, ETag); wifi_service.cpp:98-109; afr2_build_info.h (1.39.6).",
  pend: "Las versiones Web son rangos del package.json; las instaladas exactas están en package-lock.json. F24_A02 captura de DevTools no tomada." });

// F25
slide({ id: "F25", ramas: ["P"], t: 45, titulo: "El modelo OSI como mapa conceptual",
  clave: "Las capas separan responsabilidades de comunicación.",
  pie: "PROGRAMACIÓN · Sistemas Operativos y Redes de Comunicación (ISO/OSI, TCP/IP, protocolos)",
  build: s => img(s, DIA + "F25_A01_VECTOR_osi_didactico_v001.png", 0.6, 1.45, 12.1, 5.4),
  guion: "El modelo OSI nos sirve como mapa. Arriba, en aplicación, están HTTP y WebSocket y nuestros mensajes. Los datos viajan como JSON, texto. Debajo está TCP, que garantiza que lleguen en orden, después IP y el Wi-Fi. Ojo con dos cosas: es una asociación didáctica, no una implementación capa por capa, y el identificador de pedido que usamos para correlacionar respuestas no es la capa de sesión. A la derecha, el enlace entre el ESP y el ATmega ni siquiera es TCP/IP: es una UART con nuestra propia trama.",
  fuentes: "ESP/platformio.ini (ESPAsyncTCP, ESPAsyncWebServer); contracts/PROTOCOL.md (afr2-ws-1, requestId); UART_PROFILE_2.md." });

// F26
slide({ id: "F26", ramas: ["P", "E"], t: 40, titulo: "React ↔ ESP ↔ ATmega",
  clave: "Una operación atraviesa formatos y enlaces distintos.",
  pie: "PROGRAMACIÓN + ELECTRÓNICA · Sistemas Embebidos · Sistemas Operativos y Redes",
  build: s => {
    img(s, DIA + "F04_A02_VECTOR_arquitectura_v001.png", 0.2, 1.4, 7.0, 3.9);
    code(s, "1 · Navegador → ESP (WebSocket, JSON)", ["{ \"schema\": \"afr2-ws-1\", \"type\": \"req\",", "  \"requestId\": \"req-…\", \"cmd\": \"system.battery.get\",", "  \"payload\": {}, \"ts\": … }"], 7.4, 1.5, 5.5, 1.35, 12);
    code(s, "2 · ESP → ATmega (UART, binario)", ["52 4F 42 4F 32  N  3A A5 01  31  55", "[session u32][transaction u32] 00   XOR"], 7.4, 3.0, 5.5, 1.35, 12);
    code(s, "3 · ATmega → ESP → navegador", ["ruta 0x13 · cuerpo 23 B → JSON:", "{ state, raw, millivolts, referenceMv, … }"], 7.4, 4.5, 5.5, 1.35, 12);
    text(s, "Ejemplo: medir la batería (comando 0x55)", 0.4, 5.6, 6.8, 0.5, { fontSize: 16, color: C.T2 });
  },
  guion: "Sigamos una operación concreta: leer la batería. El navegador manda por WebSocket un mensaje JSON con un identificador de pedido y el nombre del comando. El ESP lo valida y lo traduce a una trama binaria por UART: la cabecera ROBO2, la ruta de ESP a ATmega, el código cero cincuenta y cinco, los números de sesión y transacción, el byte que pide leer y el XOR. El ATmega responde por la ruta inversa con veintitrés bytes, y el ESP los convierte otra vez en JSON para la página. Tres formatos distintos para la misma pregunta.",
  fuentes: "contracts/BATTERY_MEASUREMENT_9.md (0x55, cuerpo 1 B pedido, 23 B respuesta, nombres de campos WS); wire_catalog.json envelope (rutas 0x31/0x13, prefijo session/transaction u32le); Web/src/protocol/afr2Client.ts:567-575 (schema, type 'req', requestId, cmd, payload, ts).",
  pend: "Sobre JSON verificado en afr2Client.ts; falta capturar un mensaje y sus bytes reales (F26_A02) antes de presentarlo como captura." });

// F27
slide({ id: "F27", ramas: ["P"], t: 55, titulo: "Trama serial, parser y tabla de comandos",
  clave: "Los bytes adquieren significado cuando el protocolo define una estructura.",
  pie: "PROGRAMACIÓN · Computación II · Sistemas Embebidos · Sistemas Operativos y Redes",
  build: s => {
    img(s, DIA + "F27_A01_VECTOR_trama_robo2_v001.png", 0.2, 1.4, 7.4, 2.8);
    code(s, "Contracts/contracts/afr2_wire.h:100 · afr2_wire_push()", [
      "case 0: if (b=='R') { p->checksum=b; p->state=1; } return 0;",
      "case 1: if (b!='O') goto bad_header; break;   /* … 'B','O','2' */",
      "case 5: p->length=b; p->drop=b>AFR2_WIRE_PAYLOAD_MAX; break;",
      "case 10: p->command=b; p->checksum^=b; ...",
      "case 11: p->payload[p->position++]=b; p->checksum^=b; ...",
      "case 12: if (b!=p->checksum) { resync(p,b); return 0; }"], 0.3, 4.35, 7.3, 2.45, 11);
    table(s, [["CMD", "Nombre", "WebSocket", "Pide/Resp. B"],
      ["0x06", "STOP", "robot.stop", "0 / 2"], ["0x13", "DRIVE", "robot.drive", "12 / 0"], ["0x15", "MODE", "robot.mode", "5–6 / 0"],
      ["0x16", "GOTO", "robot.goto.start", "12–16 / 0"], ["0x18", "POSE", "—", "0 / 25"], ["0x21", "UI_PAGE", "robot.screen.get", "7 / var."],
      ["0x55", "BATTERY", "system.battery.*", "1 / 23"], ["0x60", "US_GET", "robot.ultrasonic.get", "0 / 35"], ["0x62", "IR_GET", "robot.ir.get", "0 / 27"],
      ["0x64", "SERVO_SET", "robot.servo.set", "2 / 18"]], 7.8, 1.5, 5.1, [0.6, 1.1, 2.1, 1.3], 11);
    text(s, "59 comandos en wire_catalog.json · tamaños sin el prefijo de sesión/transacción", 7.8, 6.2, 5.1, 0.5, { fontSize: 12, color: C.T2 });
  },
  guion: "Una UART solo entrega bytes sueltos; el significado lo pone el protocolo. Nuestra trama empieza con las letras ROBO2, sigue el largo, dos bytes fijos de control, la versión, la ruta, el comando, los datos, y termina con un XOR de todos los bytes anteriores. El receptor es una máquina de estados: avanza un estado por cada byte esperado y, si algo no coincide, se resincroniza buscando otra R. Solo cuando el XOR coincide la trama se entrega al despachador. Y la lista de comandos no está escrita a mano en tres lugares: sale de un único catálogo que leen el firmware, la Web y el Loader.",
  fuentes: "contracts/UART_PROFILE_2.md (tabla de offsets); afr2_wire.h:100-137 (parser y XOR); wire_catalog.json (59 comandos, ws, bytes).",
  pend: "Aprobación humana obligatoria del protocolo (F27)." });

// F28
slide({ id: "F28", ramas: ["P"], t: 40, titulo: "Eventos, estado y notificaciones",
  clave: "Un comando no termina al recibirse: modifica estado y produce efectos.",
  pie: "PROGRAMACIÓN · Computación II (estructuras de datos) · Sistemas Embebidos",
  build: s => {
    img(s, DIA + "F28_A01_VECTOR_evento_oled_v001.png", 0.2, 1.45, 12.9, 4.0);
    code(s, "afr2_uart1.c:11 · afr2_screens.h:65", [
      "static volatile uint8_t rx[128], tx[128], rx_head, rx_tail, ...;  // anillos 1 productor / 1 consumidor",
      "Afr2Notification notification[4];                                // avisos en pantalla"], 0.3, 5.5, 12.6, 1.25, 12);
  },
  guion: "Cuando llega un byte, la interrupción de la UART solo lo guarda en un buffer circular de ciento veintiocho bytes; no interpreta nada. En el lazo principal el parser arma la trama, el despachador llama al manejador del comando, y ese manejador cambia el estado del auto: el modo, una consigna, una falla. Ese cambio de estado es el que después se ve: por ejemplo, como un aviso en la pantalla, que se guarda en un arreglo de cuatro notificaciones y dura tres segundos.",
  fuentes: "afr2_uart1.c:1-40 (ISR RX/UDRE, RING_MASK 127); include/afr2_screens.h:65 (notification[4]); include/afr2_product.h:15 (AFR2_NOTICE_MS 3000).",
  pend: "El informe suponía capacidad 8: el código tiene 4. Política de reemplazo/overflow de las notificaciones: verificar en afr2_screens.c:324 antes de describirla." });

// F29
slide({ id: "F29", ramas: ["P", "M"], t: 40, titulo: "Dashboard y modelo 3D",
  clave: "El sistema físico también tiene una representación digital.",
  pie: "PROGRAMACIÓN → MECÁNICA · Sistemas de Representación II · Computación II",
  build: s => {
    const WEB = CAP + "web/F29_A01_CAPTURE_web_";
    img(s, WEB + "inicio_v001.png", 0.3, 1.5, 6.1, 3.2);
    [["conducir", "Conducir"], ["seguidor", "Seguidor de línea"], ["modelo", "Modelo 3D"], ["sensores", "Sensores / radar"]].forEach(([f, t], i) => {
      const x = 6.6 + (i % 2) * 3.2, y = 1.5 + Math.floor(i / 2) * 1.75;
      img(s, WEB + f + "_v001.png", x, y, 3.05, 1.45);
      text(s, t, x, y + 1.43, 3.05, 0.28, { fontSize: 11, color: C.T2, align: "center" });
    });
    text(s, "Capturas reales (npm run dev), sin auto conectado y sin datos simulados", 0.3, 4.75, 6.1, 0.3, { fontSize: 11, color: C.T2, align: "center" });
    const steps = [["Fusion", "ensamble .f3d"], ["Blender", "materiales, simplificación"], ["GLB + Draco", "880.232 B"], ["gzip", "LittleFS del ESP"], ["Three.js", "React Three Fiber"]];
    steps.forEach(([a, b], i) => {
      panel(s, 0.3 + i * 2.55, 5.15, 2.35, 1.5, i < 2 ? C.MEC : C.PRG);
      text(s, a, 0.4 + i * 2.55, 5.3, 2.15, 0.4, { fontSize: 16, bold: true, align: "center" });
      text(s, b, 0.4 + i * 2.55, 5.75, 2.15, 0.8, { fontSize: 12.5, color: C.T2, align: "center" });
    });
  },
  guion: "El mismo modelo del auto que diseñamos en Fusion es el que ve el usuario en la página. Lo pasamos a Blender para ajustar materiales y aligerarlo, lo exportamos como GLB comprimido con Draco, de unos ochocientos ochenta kilobytes, y el ESP lo sirve comprimido en gzip. En el navegador lo dibuja Three.js dentro de React. La aplicación tiene control táctil, el visor del seguidor de línea, el simulador de GOTO, las secuencias, los sensores y la pantalla del auto en vivo.",
  fuentes: "Web/public/models/catalog.json (AutoFinalRobo2.glb 880.232 B); CAD/mecanica/auto-completo/blender_ligero.py, fusion_exportar_blender.py; contracts/WEB_INFRASTRUCTURE.md.",
  pend: "Capturas tomadas el 18/11/2026 de la fuente 1.39.6 con npm run dev y Chrome headless 1920×1080, sin auto conectado: no muestran telemetría real. Para F29_A03 (indicadores ↔ sensores) falta una captura con el auto en un estado conocido. Uso de Draco: confirmar en el pipeline antes de afirmarlo." });

// F30
slide({ id: "F30", ramas: ["P"], t: 50, titulo: "Flash, LittleFS, RAM y recursos 3D",
  clave: "Almacenar datos y ejecutar software son problemas distintos.",
  pie: "PROGRAMACIÓN · Computación II (memoria) · Sistemas Embebidos",
  build: s => {
    img(s, DIA + "F30_A01_CHART_memoria_v001.png", 0.2, 1.45, 8.3, 4.2);
    bullets(s, ["ATmega: .data 284 B + .bss 5321 B = 5614 B estáticos; quedan 2578 B antes de la pila (no es «memoria libre medida»)", "ESP: optimización −4528 B de RAM estática (51172 → 46644 B)", "Web empaquetada: 17 activos, 2.487.493 B en gzip", "Flash del ESP ≠ RAM del ESP ≠ RAM del navegador que dibuja el 3D"], 8.7, 1.6, 4.3, 5.0, 14);
  },
  guion: "La memoria también es un recurso de ingeniería. En el ATmega tenemos ocho kilobytes de RAM: entre variables inicializadas y no inicializadas usamos unos cinco mil seiscientos bytes, y lo que queda es espacio para la pila, no memoria libre garantizada. En el ESP reducimos más de cuatro kilobytes de RAM estática moviendo textos a flash. Y no hay que confundir: la flash del ESP guarda la aplicación web, la RAM del ESP atiende las conexiones, y el modelo 3D se descomprime y se dibuja en la memoria del navegador, no del auto.",
  fuentes: "documentation/builds/2026-11-06-04/README.md (SRAM 5614, flash 124240, 2578 antes de pila); changes/2026-11-09-09 (ESP 51172→46644 B); build:esp (17 activos 2.487.493 B gzip; presupuesto 2.867.200). Capacidades del chip: datasheets.",
  pend: "Builds de 06/11 y 09/11: la versión actual 1.39.6 puede diferir. Antes de presentar, regenerar size/map de la versión que se cargue (el repo y el auto no están alineados)." });

// F31
slide({ id: "F31", ramas: ["P"], t: 45, titulo: "Sesiones, concurrencia y latencia",
  clave: "Compartir un sistema físico requiere coordinar clientes y medir tiempos.",
  pie: "PROGRAMACIÓN · Sistemas Operativos y Redes · Sistemas Embebidos",
  build: s => {
    table(s, [["Mecanismo", "Valor", "Para qué"], ["system.alive", "cada 500 ms", "presencia del navegador"], ["Sin presencia", "1500 ms → STOP", "no dejar el auto andando solo"],
      ["DRIVE", "TTL 400 ms, uno en vuelo", "la consigna vence si no se renueva"], ["Timeout UART / WS", "800 ms / 2500 ms", "detectar pedidos perdidos"],
      ["STOP", "prioritario, sin reanudar solo", "seguridad"]], 0.3, 1.55, 8.0, [2.3, 2.6, 3.1], 14);
    text(s, "requestId y session/transaction correlacionan respuestas: no son autenticación.", 0.4, 4.6, 7.9, 0.8, { fontSize: 15, color: C.T2 });
    pending(s, "F31_A02 · latencia extremo a extremo (navegador→ESP→ATmega→retorno): falta instrumentar y medir", 8.5, 1.55, 4.4, 3.3);
  },
  guion: "Un auto que se mueve no puede quedar obedeciendo a un navegador que se colgó. Por eso la página avisa que sigue presente cada medio segundo, y si pasa un segundo y medio sin noticias, el auto se detiene. Cada orden de movimiento vence a los cuatrocientos milisegundos si no se renueva, y la orden de parar tiene prioridad sobre todo. Los identificadores de pedido sirven para saber qué respuesta corresponde a qué pregunta; no son una contraseña. Lo que todavía no medimos es la latencia de punta a punta.",
  fuentes: "contracts/PROTOCOL.md (alive 500 ms, 1500 ms, TTL 400 ms, STOP prioritario); BATTERY_MEASUREMENT_9.md (UART 800 ms, WS 2500 ms); afr2_product.c:322 (web_ms ≥ 1500 → Stop).",
  pend: "Política multicliente: el contrato dice «una sesión»; verificar en broker.cpp cómo se rechaza un segundo cliente antes de dibujar dos clientes." });

// F32
slide({ id: "F32", ramas: ["M", "E", "C", "P"], t: 75, titulo: "Workflow organizado e IA agéntica",
  clave: "La IA no hace ingeniería por nosotros: un workflow organizado nos devuelve tiempo para integrar, revisar y validar.",
  pie: "Proyecto de Ingeniería Mecatrónica (planificación, requerimientos, especificación, diseño, testeo y validación)",
  build: s => {
    img(s, DIA + "F32_A01_VECTOR_monorepo_contratos_v001.png", 0.2, 1.75, 6.6, 3.7);
    img(s, FOT + "FOTO-54_loader_operacion.png", 7.0, 1.8, 5.9, 3.4);
    img(s, DIA + "F32_A02_VECTOR_caso_bateria_v001.png", 0.3, 5.5, 12.7, 0.95);
    text(s, "Estudiar → diseñar → implementar → probar → verificar → documentar", 0.4, 6.5, 12.5, 0.4, { fontSize: 17, bold: true, color: C.T2, align: "center" });
  },
  guion: "Organizar el proyecto no hace el trabajo solo. Lo que hace es partir un sistema complejo en módulos con responsabilidades y contratos claros: firmware del ATmega, firmware del ESP, la Web, el CAD, el Loader para cargar los firmwares, y en el centro Contracts, donde se documenta cada cambio compartido con un identificador y el estado real de cada módulo. Todo vive en un solo repositorio con una única versión de producto. Así, el caso de la batería atraviesa hardware, firmware, protocolo y React, y cada eslabón sabe con qué archivo se conecta. Una herramienta de IA puede ayudarnos a buscar una función, preparar una prueba o implementar un componente; nosotros seguimos definiendo requisitos, arquitectura, seguridad, límites físicos y criterios de aceptación. Esa reducción del trabajo repetitivo nos permite mirar más la vista macro.",
  fuentes: "AGENTS.md raíz (seis módulos, un solo Git, versión única en afr2_build_info.h y package.json); Contracts/docs/ESTRUCTURA_Y_GIT.md, VERSIONADO.md; Loader 2.5.0 (Loader/src/JobState.cs:62); fotos_existentes/FOTO-54.",
  pend: "No atribuir al Loader capacidades MCP/IA no demostradas en la versión instalada (2.5.0 en fuente; distribución en raíz 2.0.0)." });

// F33
slide({ id: "F33", ramas: ["M", "E", "C", "P"], hero: true, t: 45, titulo: "Cuatro ramas, un sistema",
  pie: "Ingeniería Mecatrónica · Plan 2023 · AutoFinalRobo2",
  build: s => {
    img(s, REN + "F33_A01_RENDER_hero_final_v001.png", 0, 0.07, W, H - 0.6);
    text(s, "LA MECATRÓNICA NO CONSISTE EN SUMAR COMPONENTES.", 0.5, 0.45, 12.3, 0.6, { fontSize: 26, bold: true, align: "center" });
    text(s, "CONSISTE EN INTEGRAR CONOCIMIENTOS PARA RESOLVER PROBLEMAS REALES.", 0.5, 1.05, 12.3, 0.5, { fontSize: 20, color: C.T2, align: "center" });
    [["MECÁNICA", "CAD · ruedas Mecanum · estabilidad", C.MEC, 0.5, 2.3], ["ELECTRÓNICA", "divisor · Hall · ultrasonido · I²C", C.ELE, 0.5, 4.5],
     ["CONTROL", "lazo cerrado PI · seguidor de línea", C.CTL, 9.9, 2.3], ["PROGRAMACIÓN", "C · Arduino · React · protocolo", C.PRG, 9.9, 4.5]].forEach(([a, b, c, x, y]) => {
      s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w: 2.95, h: 1.25, fill: { color: C.BG, transparency: 15 }, line: { color: c, width: 1.75 }, rectRadius: 0.08 });
      text(s, a, x + 0.1, y + 0.15, 2.75, 0.4, { fontSize: 17, bold: true, color: c, align: "center" });
      text(s, b, x + 0.1, y + 0.6, 2.75, 0.55, { fontSize: 12.5, color: C.T2, align: "center" });
    });
    text(s, "¡Gracias!", 0.5, 6.25, 12.3, 0.55, { fontSize: 24, bold: true, align: "center" });
  },
  guion: "Empezamos diciendo que no íbamos a recorrer materias sino un sistema. Vimos que la mecánica define cómo se mueve y cuánto aguanta, la electrónica cómo mide y actúa, el control cómo corrige lo que pasa en la realidad, y la programación cómo todo eso se coordina y se comunica. Ninguna de las cuatro alcanza sola: la mecatrónica no consiste en sumar componentes, sino en integrar conocimientos para resolver problemas reales. Muchas gracias.",
  fuentes: "Render F33_A01 (CAM_F33_HERO_FINAL_V01) desde el mismo GLB que F01.",
  pend: "Agregar QR y link al repositorio si se decide publicarlo (idea del equipo)." });

// ---------- Anexos (respaldo para preguntas; fuera de los 25 min)
slide({ id: "A1", ramas: ["E", "P"], t: 0, titulo: "Anexo · KEY y RST automáticos con NPN",
  clave: "Dos transistores reemplazan los pulsadores para poner al ESP en modo de carga.",
  pie: "ANEXO · Electrónica Básica (transistor como llave) · Microcontroladores",
  build: s => {
    img(s, FOT + "FOTO-61_shield_vista_superior_KEY_RST.jpg", 0.3, 1.5, 6.2, 5.2);
    bullets(s, ["NPN en paralelo con cada pulsador del shield, base desde D32/D33 del Mega por 1 kΩ", "10 kΩ base–emisor: apagado aunque el Mega no tenga programa", "Sin los cables de control el shield queda como de fábrica", "Implementado en firmware y Loader; circuito sin medir"], 6.8, 1.6, 6.1, 5.0, 16);
  },
  guion: "Respaldo para preguntas. El ESP entra en modo de carga bajando GPIO0 mientras se reinicia. En lugar de apretar dos botones, dos transistores NPN en paralelo con los pulsadores lo hacen comandados por el Mega.",
  fuentes: "presentacion/Filmina_12_Agregado_NPN_KEY_RST.md; Contracts/hardware/PLAN_KEY_RST_AUTOMATICO.md; FOTO-61.",
  pend: "Circuito en armado, sin mediciones." });
slide({ id: "A2", ramas: ["P"], t: 0, titulo: "Anexo · AFR2 Loader",
  clave: "Compilar, cargar y verificar los dos firmwares sin pasos manuales propensos a error.",
  pie: "ANEXO · Computación II · Sistemas Embebidos · Proyecto",
  build: s => {
    img(s, FOT + "FOTO-55_loader_flasher.png", 0.3, 1.5, 6.2, 3.6);
    img(s, FOT + "FOTO-56_loader_monitor_serie.png", 6.7, 1.5, 6.2, 3.6);
    bullets(s, ["Puente temporal en el Mega para programar el ESP por USB", "Verificación por hash en la misma sesión · backup opcional", "Monitor serie con intérprete del catálogo de comandos"], 0.5, 5.3, 12.3, 1.5, 16);
  },
  guion: "Respaldo para preguntas: el Loader elige puerto y destino, compila, instala el puente en el Mega para cargar el ESP, verifica y trae un monitor serie que entiende nuestros comandos.",
  fuentes: "presentacion/Filmina_11_Agregado_Loader.md; AutoFinalRobo2-Loader/README.md, MANUAL.html; FOTO-55, FOTO-56." });

// ---------- salida
fs.mkdirSync(path.dirname(OUT), { recursive: true });
pres.writeFile({ fileName: OUT }).then(f => {
  console.log("PPTX", f);
  const total = GUION.reduce((a, d) => a + d.t, 0);
  let md = `# Guion hablado · AutoFinalRobo2 (presentación v3)\n\nGenerado por \`tools/generar_presentacion_v3.js\` (mismo texto que las notas del orador). ` +
    `Tiempo de contenido: ${Math.floor(total / 60)}:${String(total % 60).padStart(2, "0")} + ~1:30 de transiciones ≈ 25 min. Los anexos no cuentan.\n\n`;
  let acc = 0;
  GUION.forEach((d, i) => {
    acc += d.t;
    md += `## ${String(i + 1).padStart(2, "0")} · ${d.id} · ${d.titulo}  \n*${d.t} s · acumulado ${Math.floor(acc / 60)}:${String(acc % 60).padStart(2, "0")} · ramas ${d.ramas.join("+")}*\n\n> ${d.guion}\n\n**Fuentes:** ${d.fuentes}\n\n${d.pend ? "**Pendiente / no afirmar:** " + d.pend + "\n\n" : ""}`;
  });
  fs.writeFileSync(V3 + "GUION_HABLADO.md", md, "utf8");
  console.log("GUION", V3 + "GUION_HABLADO.md", "total s", total);
});
