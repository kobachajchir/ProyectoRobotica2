// AutoFinalRobo2 · presentación v4 · tema claro glass (derivada de generar_presentacion_v3.js).
// Uso: node generar_presentacion_v4.js [salida.pptx]   (requiere pptxgenjs; NODE_PATH al node_modules donde esté instalado)
// Los recursos de Fusion/Blender en tema claro llegan a ../assets_claro (ver PROMPT_ESCRITORIO_FUSION_BLENDER.md);
// mientras falten se usa el render de la v3 o un recuadro «pendiente».
// Genera además ../GUION_HABLADO_v4.md con el mismo texto que va en las notas del orador.
const pptxgen = require("pptxgenjs");
const fs = require("fs");
const path = require("path");
const V3 = path.resolve(__dirname, "..") + "/";
const REN = V3 + "renders/", DIA = V3 + "diagramas_claro/", CAP = V3 + "capturas/", FOT = path.resolve(V3, "../fotos_existentes") + "/";
const NEW = V3 + "assets_claro/", FONDO = V3 + "fondos/";
const OUT = process.argv[2] || V3 + "pptx/AutoFinalRobo2_Mecatronica_v4.pptx";

// Paleta clara (tokens de informe-portabilidad-glassmorphism.md) + acentos de rama; *_D = tinta legible sobre claro
const C = { MEC: "3B82F6", ELE: "F59E0B", CTL: "8B5CF6", PRG: "10B981", MEC_D: "1D4ED8", ELE_D: "B45309", CTL_D: "6D28D9", PRG_D: "047857",
  BG: "F6F8FC", TX: "132033", T2: "4F6178", GR: "CBD5E1", PANEL: "FFFFFF", SUB: "EEF3F8", WARN: "C43C3C", WARN_BG: "FFF0F0" };
const FONT = "Inter", HEAD = "Manrope", MONO = "Consolas";
const W = 13.333, H = 7.5, M = 0.7;
const pres = new pptxgen();
pres.layout = "LAYOUT_WIDE";
pres.title = "AutoFinalRobo2 · Diseño, construcción, control y programación de un vehículo robótico";
pres.author = "Koba Chajchir";
const SHADOW = () => ({ type: "outer", color: "0F172A", opacity: 0.10, blur: 14, offset: 4, angle: 90 });
const PESO = (() => { try { return JSON.parse(fs.readFileSync(NEW + "peso_modelo.json", "utf8")); } catch (e) { return null; } })();

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
// Recurso nuevo de la PC de escritorio si existe; si no, el de la v3 (o null)
function A(rel, fallback) { const p = NEW + rel; return fs.existsSync(p) ? p : (fallback && fs.existsSync(fallback) ? fallback : null); }
const isNew = p => !!p && p.startsWith(NEW);
function img(s, p, x, y, w, h, align = "center", label) {
  if (!p || !fs.existsSync(p)) { pending(s, label || ("Render pendiente (PC de escritorio)" + (p ? ": " + path.basename(p) : "")), x, y, w, h); return; }
  const [iw, ih] = imgSize(p), k = Math.min(w / iw, h / ih), ww = iw * k, hh = ih * k;
  const xx = align === "left" ? x : align === "right" ? x + w - ww : x + (w - ww) / 2;
  s.addImage({ path: p, x: xx, y: y + (h - hh) / 2, w: ww, h: hh });
  return { x: xx, y: y + (h - hh) / 2, w: ww, h: hh };
}
// Imagen de la v3 con fondo oscuro: se enmarca en una tarjeta para que no quede como un parche
function framed(s, p, x, y, w, h, label) {
  if (!p) { pending(s, label || "Render pendiente (PC de escritorio)", x, y, w, h); return; }
  if (isNew(p)) return img(s, p, x, y, w, h);
  glass(s, x, y, w, h, { strong: true });
  return img(s, p, x + 0.08, y + 0.08, w - 0.16, h - 0.16);
}
function text(s, t, x, y, w, h, o = {}) {
  s.addText(t, Object.assign({ x, y, w, h, fontFace: FONT, fontSize: 18, color: C.TX, margin: 0, valign: "top", isTextBox: true }, o));
}
function bullets(s, items, x, y, w, h, size = 18, color = C.TX) {
  s.addText(items.map(t => ({ text: t, options: { bullet: { indent: 16 }, paraSpaceAfter: 7 } })),
    { x, y, w, h, fontFace: FONT, fontSize: size, color, margin: 0, valign: "top", isTextBox: true });
}
// Tarjeta glass: blanco translúcido + borde claro + sombra suave; tint la tiñe con el color de la rama
function glass(s, x, y, w, h, o = {}) {
  const r = o.r ?? 0.16, tint = o.tint;
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: r, fill: { color: "FFFFFF", transparency: o.strong ? 10 : 30 },
    line: { color: tint || "FFFFFF", width: tint ? 1.25 : 1.5, transparency: tint ? 40 : 0 }, shadow: SHADOW() });
  if (tint) s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: r, fill: { color: tint, transparency: 91 }, line: { color: tint, width: 0, transparency: 100 } });
}
function panel(s, x, y, w, h, color) { glass(s, x, y, w, h, { tint: color && color !== C.GR ? color : null }); }
function code(s, src, lines, x, y, w, h, size = 13) {
  glass(s, x, y, w, h, { strong: true });
  s.addText(lines.join("\n"), { x: x + 0.18, y: y + 0.14, w: w - 0.36, h: h - 0.52, fontFace: MONO, fontSize: size, color: "1E293B", margin: 0, valign: "top", isTextBox: true });
  s.addText(src, { x: x + 0.18, y: y + h - 0.36, w: w - 0.36, h: 0.28, fontFace: MONO, fontSize: 10, color: C.T2, margin: 0, isTextBox: true });
}
function chip(s, t, x, y, color, w) {
  const ww = w || Math.max(1.1, t.length * 0.105 + 0.3);
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w: ww, h: 0.34, fill: { color, transparency: 85 }, line: { color, width: 1, transparency: 30 }, rectRadius: 0.17 });
  s.addText(t, { x, y, w: ww, h: 0.34, fontFace: FONT, fontSize: 11.5, bold: true, color, align: "center", valign: "middle", margin: 0, isTextBox: true });
  return ww;
}
function pending(s, t, x, y, w, h) {
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h, fill: { color: C.WARN_BG, transparency: 20 }, line: { color: C.WARN, width: 1.25, dashType: "dash" }, rectRadius: 0.14 });
  s.addText(t, { x: x + 0.15, y, w: w - 0.3, h, fontFace: FONT, fontSize: Math.min(14, Math.max(10, h * 9)), color: C.WARN, align: "center", valign: "middle", margin: 0, isTextBox: true });
}
function eq(s, t, x, y, w, h, size = 24, color = C.TX) {
  s.addText(t, { x, y, w, h, fontFace: "Cambria Math", fontSize: size, color, margin: 0, valign: "middle", align: "center", isTextBox: true });
}
function table(s, rows, x, y, w, colW, size = 13) {
  const head = rows[0].map(t => ({ text: t, options: { bold: true, color: C.TX, fill: { color: "E6ECF5" } } }));
  const body = rows.slice(1).map(r => r.map(t => ({ text: String(t), options: { color: C.TX, fill: { color: "FFFFFF" } } })));
  s.addTable([head, ...body], { x, y, w, colW, fontFace: FONT, fontSize: size, border: { type: "solid", color: "D5DCE6", pt: 0.75 }, margin: 0.06, valign: "middle" });
}
// Referencias: catálogo numerado único; en cada filmina se listan con viñetas y al final todas juntas
const REF = [["LP", "Robótica", "K. M. Lynch y F. C. Park, Modern Robotics: Mechanics, Planning, and Control. Cambridge University Press, 2017, cap. 13."],
  ["CR", "Robótica", "J. J. Craig, Introduction to Robotics: Mechanics and Control, 3.ª ed. Pearson, 2005, cap. 2."],
  ["FW", "Proyecto", "AutoFinalRobo2-Atmega/src/services/afr2_product.c: drive_closed_loop() (l. 130) y odometry() (l. 295-304)."],
  ["SG", "Robótica", "R. Siegwart, I. Nourbakhsh y D. Scaramuzza, Introduction to Autonomous Mobile Robots, 2.ª ed. MIT Press, 2011, cap. 3."],
  ["HB", "Mecánica", "R. C. Hibbeler, Ingeniería mecánica: Estática, 14.ª ed. Pearson, 2016, cap. 4 (momento de una fuerza)."],
  ["HBD", "Mecánica", "R. C. Hibbeler, Ingeniería mecánica: Dinámica, 14.ª ed. Pearson, 2016 (centro de masa, momento de inercia, D'Alembert)."],
  ["BJ", "Mecánica", "F. P. Beer, E. R. Johnston, J. T. DeWolf y D. F. Mazurek, Mecánica de materiales, 7.ª ed. McGraw-Hill, 2015."],
  ["BN", "Mecánica", "R. G. Budynas y J. K. Nisbett, Diseño en ingeniería mecánica de Shigley, 10.ª ed. McGraw-Hill, 2015 (engranajes, correas, rodamientos)."],
  ["EST", "Proyecto", "AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/analisis_cargas_200g: resultados_estatica.json, caso CAD macizo 200 g."],
  ["V17", "Proyecto", "AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/README.md (transmisiones, rodamientos y actuadores)."],
  ["OG", "Control", "K. Ogata, Ingeniería de control moderna, 5.ª ed. Pearson, 2010."],
  ["MC", "Electrónica", "Microchip Technology, ATmega640/1280/1281/2560/2561 Datasheet."],
  ["ES", "Electrónica", "Espressif Systems, ESP8266EX Datasheet."],
  ["SEN", "Proyecto", "AutoFinalRobo2-Atmega/src/platform: afr2_board.c (Hall), afr2_sensors.c (ADC, Timer3, Timer5), afr2_twi.c (I²C)."]];
const RN = k => REF.findIndex(r => r[0] === k) + 1;
function refs(s, keys) {
  const items = keys.map(k => ({ text: `[${RN(k)}]  ${REF[RN(k) - 1][2]}`, options: { bullet: { indent: 12 }, paraSpaceAfter: 2 } }));
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x: M, y: 6.27, w: W - 2 * M, h: 0.6, rectRadius: 0.1, fill: { color: "FFFFFF", transparency: 45 }, line: { color: C.GR, width: 0.75, transparency: 30 } });
  s.addText(items, { x: M + 0.15, y: 6.3, w: W - 2 * M - 0.3, h: 0.54, fontFace: FONT, fontSize: 9, color: C.T2, margin: 0, valign: "middle", isTextBox: true, fit: "shrink" });
}
function leyenda(s, t, x, y, w, h, size = 11) { text(s, t, x, y, w, h, { fontSize: size, color: C.T2, italic: false }); }

const BR = { M: ["MECÁNICA", C.MEC, C.MEC_D], E: ["ELECTRÓNICA", C.ELE, C.ELE_D], C: ["CONTROL", C.CTL, C.CTL_D], P: ["PROGRAMACIÓN", C.PRG, C.PRG_D] };
const GUION = [];
let n = 0;
function slide(d) {
  n++;
  const s = pres.addSlide();
  s.background = { path: FONDO + (d.hero ? "bg_hero.png" : "bg_claro.png") };
  const act = d.ramas;
  if (!d.hero) {
    text(s, d.titulo, M, 0.34, 10.0, 0.62, { fontFace: HEAD, fontSize: 28, bold: true });
    if (d.clave) text(s, d.clave, M, 0.98, 8.9, 0.45, { fontSize: 15.5, color: C.T2, fit: "shrink" });
    // minimapa: cuatro ramas en círculos glass, activas con su color
    const R0 = 0.4, X0 = 10.95, GAP = 0.12;
    ["M", "E", "C", "P"].forEach((k, i) => {
      const on = act.includes(k), c = BR[k][1], x = X0 + i * (R0 + GAP);
      s.addShape(pres.shapes.OVAL, { x, y: 0.34, w: R0, h: R0, fill: { color: on ? c : "FFFFFF", transparency: on ? 8 : 35 },
        line: { color: on ? c : C.GR, width: 1.25, transparency: on ? 0 : 20 }, shadow: on ? SHADOW() : undefined });
      s.addText(k, { x, y: 0.34, w: R0, h: R0, fontFace: HEAD, fontSize: 13, bold: true, color: on ? "FFFFFF" : "94A3B8", align: "center", valign: "middle", margin: 0, isTextBox: true });
    });
    text(s, act.map(k => BR[k][0]).join(" + "), 9.6, 0.34 + R0 + 0.16, X0 + 4 * R0 + 3 * GAP - 9.6, 0.25, { fontSize: 8, bold: true, color: C.T2, align: "right", charSpacing: 0.5 });
  }
  // pie curricular
  s.addShape(pres.shapes.LINE, { x: M, y: 6.95, w: W - 2 * M, h: 0, line: { color: C.GR, width: 0.75 } });
  text(s, d.pie, M, 7.0, 10.8, 0.3, { fontSize: 11, color: C.T2 });
  text(s, `${String(n).padStart(2, "0")}${d.id.startsWith("A") ? " · " + d.id : ""}`, W - M - 1.2, 7.0, 1.2, 0.3, { fontSize: 11, color: C.T2, align: "right" });
  d.build(s);
  const notes = `GUION (${d.t} s)\n${d.guion}\n\nFUENTES\n${d.fuentes}${d.pend ? "\n\nPENDIENTE / NO AFIRMAR\n" + d.pend : ""}`;
  s.addNotes(notes);
  GUION.push(d);
}

// ======================================================================================
// 01 · Portada
slide({ id: "F01", ramas: ["M", "E", "C", "P"], hero: true, t: 20,
  titulo: "Diseño, construcción, control y programación de un vehículo robótico",
  pie: "Proyecto final de Robótica II · Ingeniería Mecatrónica",
  build: s => {
    const hero = A("renders/F01_auto_hero_v001.png", REN + "F01_A01_RENDER_hero_v001.png");
    if (isNew(hero)) img(s, hero, 5.6, 0.6, 7.6, 6.0);
    else { glass(s, 6.2, 1.0, 6.5, 5.3, { strong: true, r: 0.22 }); img(s, hero, 6.3, 1.1, 6.3, 5.1); }
    text(s, "DE LA TEORÍA A UN SISTEMA MECATRÓNICO REAL", M, 1.35, 5.4, 0.35, { fontFace: HEAD, fontSize: 12, bold: true, color: C.MEC_D, charSpacing: 1.5 });
    text(s, "Diseño, construcción, control y programación de un vehículo robótico", M, 1.8, 5.3, 2.85, { fontFace: HEAD, fontSize: 34, bold: true, color: C.TX, lineSpacingMultiple: 0.95, valign: "middle" });
    ["M", "E", "C", "P"].forEach((k, i) => { const w = k === "P" ? 1.55 : 1.28; chip(s, BR[k][0], M + [0, 1.36, 2.72, 4.08][i], 4.9, BR[k][2], w); });
    text(s, "AutoFinalRobo2 · Koba Chajchir", M, 5.55, 5.4, 0.4, { fontSize: 16, bold: true, color: C.TX });
    text(s, "Proyecto final de Robótica II · Ingeniería Mecatrónica", M, 5.95, 5.4, 0.4, { fontSize: 14, color: C.T2 });
  },
  guion: "No vamos a recorrer la carrera materia por materia. Vamos a recorrer un sistema que necesitó conocimientos de distintas materias para existir. Este auto es el hilo conductor: primero veremos cómo se construye, después cómo mide, cómo controla su comportamiento, cómo se programa y, al final, cómo todo eso se integra.",
  fuentes: "Render F01 desde AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb (v4: assets_claro/renders/F01_auto_hero_v001.png cuando llegue de la PC de escritorio)." });

// 02 · Venn
slide({ id: "F02", ramas: ["M", "E", "C", "P"], t: 40, titulo: "¿Qué es la Mecatrónica?",
  clave: "Cuatro áreas que se superponen: en las uniones están las materias que las conectan.",
  pie: "Las cuatro ramas son la taxonomía didáctica de esta charla · el Plan 2023 se organiza en cuatro bloques formales distintos",
  build: s => img(s, DIA + "F02_A01_VECTOR_venn_ramas_v002.png", 0.4, 1.45, W - 0.8, 5.45),
  guion: "Un sistema mecatrónico no es una suma de partes: es estructura que se mueve, electrónica que mide y actúa, control que regula ese comportamiento y software que procesa y comunica. Cada círculo junta las materias propias de un área; donde dos círculos se tocan están las materias que las unen, como Microcontroladores entre electrónica y programación o Mecánica Racional y Robótica I entre mecánica y control. Y en el centro, donde se cruzan las cuatro, está el proyecto integrador.",
  fuentes: "Res. C.S. 012/23 (Plan 2023): cuatro bloques formales; perfil del egresado. Materias nombradas en el informe maestro y en los pies curriculares de la charla.",
  pend: "La ubicación de cada materia en una zona del Venn es didáctica (criterio del equipo), no del plan. Confirmar los nombres exactos de Máquinas Eléctricas y Sistemas Mecatrónicos I/II contra el plan." });

// 03 · Árbol vertical
slide({ id: "F03", ramas: ["M", "E", "C", "P"], t: 70, titulo: "Árbol de conocimientos y correlatividades",
  clave: "Las materias no son islas: año a año construyen recorridos que convergen.",
  pie: "Correlatividades para cursar según Res. C.D. 726/23 · R regularizada · A aprobada",
  build: s => img(s, DIA + "F03_A01_VECTOR_arbol_vertical_v002.png", 0.4, 1.4, W - 0.8, 5.5),
  guion: "El árbol se lee de arriba hacia abajo, año por año. A la izquierda, la mecánica: Estática y Resistencia de Materiales y Ecuaciones Diferenciales regularizadas, más Ciencias de los Materiales aprobada, llevan a Mecánica Racional, y esa junto con Sistemas de Control lleva a Robótica I y después a Robótica II. A la derecha, la electrónica digital: Electrónica Digital y Laboratorio de Mediciones regularizadas, Electrónica Básica y Computación II aprobadas, habilitan Microcontroladores; desde ahí Sistemas Embebidos y luego Sistemas Operativos y Redes. La letra en cada flecha es la condición oficial: R regularizada, A aprobada.",
  fuentes: "Aristas copiadas del informe maestro (C.D. 726/23): Mecánica Racional, Microcontroladores, Sistemas de Control, Sistemas Embebidos, Robótica I, Sist. Operativos y Redes.",
  pend: "El año de cada materia se asignó respetando el orden de correlatividades, sin el PDF del plan a mano: confirmar los años contra el Plan 2023. Robótica I → Robótica II se dibuja punteada (no está en las aristas citadas). Revisión humana obligatoria contra la C.D. 726/23." });

// 04 · Arquitectura general (nativa, con fotos de componentes)
slide({ id: "F04", ramas: ["M", "E", "C", "P"], t: 50, titulo: "Arquitectura general del robot",
  clave: "Un sistema físico, dos procesadores, sensores, actuadores y una interfaz.",
  pie: "Sistemas Mecatrónicos I/II · Microcontroladores · Sistemas Embebidos · Sistemas Operativos y Redes",
  build: s => {
    const top = [["Navegador", C.PRG, ["React 19 + Vite 7", "Three.js (Modelo GLB)", "Cliente WS afr2-ws-1"]],
      ["ESP8266 · ESP-12", C.PRG, ["Arduino Core · PlatformIO", "AsyncWebServer · LittleFS", "Broker JSON ↔ Binario"]],
      ["ATmega2560", C.ELE, ["C avr-libc, Sin Arduino", "Control PID · Odometría", "UI OLED · Sensores"]]];
    const TW = 3.0, TX0 = M, TG = (W - 2 * M - 3 * TW) / 2;
    top.forEach(([t, c, ls], i) => {
      const x = TX0 + i * (TW + TG);
      glass(s, x, 1.5, TW, 1.75, { tint: c });
      text(s, t, x, 1.62, TW, 0.42, { fontFace: HEAD, fontSize: 18, bold: true, align: "center" });
      text(s, ls.join("\n"), x + 0.1, 2.08, TW - 0.2, 1.05, { fontSize: 13, color: C.T2, align: "center", valign: "middle", lineSpacingMultiple: 1.05 });
    });
    [["Wi-Fi · HTTP\nWebSocket /ws · JSON", C.PRG_D], ["UART 115200\nROBO2 + XOR · USART1", C.ELE_D]].forEach(([t, c], i) => {
      const x1 = TX0 + (i + 1) * TW + i * TG + 0.06, x2 = x1 + TG - 0.12;
      s.addShape(pres.shapes.LINE, { x: x1, y: 2.38, w: x2 - x1, h: 0, line: { color: c, width: 2, beginArrowType: "triangle", endArrowType: "triangle" } });
      text(s, t, x1 - 0.1, 2.48, x2 - x1 + 0.2, 0.6, { fontSize: 9.5, color: c, align: "center", bold: true });
    });
    const hw = [["Drivers OSOYOO\nModel Y V2", "PWM 976 Hz", A("renders/F04_driver_superior_v001.png")],
      ["4 × GM25-370\n+ Hall A/B", "PCINT2 A8–A15", A("renders/F04_motor_gm25_iso_v001.png", REN + "F14_A01_RENDER_motor_gm25_crop_v001.png")],
      ["HC-SR04\nsobre SG90", "Timer3 / Timer5", A("renders/F04_hcsr04_frente_v001.png")],
      ["Línea 5 IR", "A0–A4 Digital", A("renders/F04_sensor_linea_inferior_v001.png")],
      ["OLED 128×64\n+ Encoder KY-040", "I²C 100 kHz · INT4/5", A("renders/F04_oled_encoder_superior_v001.png")],
      ["Batería 2×18650", "Divisor → ADC5", A("renders/F04_bateria_superior_v001.png")]];
    const BW = 1.85, BG_ = (W - 2 * M - 6 * BW) / 5;
    s.addShape(pres.shapes.LINE, { x: TX0 + 2 * (TW + TG) + TW / 2, y: 3.27, w: 0, h: 0.28, line: { color: C.ELE_D, width: 1.5 } });
    s.addShape(pres.shapes.LINE, { x: M + BW / 2, y: 3.55, w: W - 2 * M - BW, h: 0, line: { color: C.ELE_D, width: 1.5 } });
    hw.forEach(([t, sub, p], i) => {
      const x = M + i * (BW + BG_);
      s.addShape(pres.shapes.LINE, { x: x + BW / 2, y: 3.55, w: 0, h: 0.2, line: { color: C.ELE_D, width: 1.5, endArrowType: "triangle" } });
      glass(s, x, 3.8, BW, 3.0, { tint: C.ELE });
      img(s, p, x + 0.12, 3.9, BW - 0.24, 1.55, "center", "Render\npendiente");
      text(s, t, x + 0.06, 5.55, BW - 0.12, 0.62, { fontSize: 12, bold: true, align: "center", valign: "middle" });
      text(s, sub, x + 0.06, 6.2, BW - 0.12, 0.45, { fontSize: 10.5, color: C.T2, align: "center", valign: "middle" });
    });
  },
  guion: "Este es el mapa que vamos a recorrer. En el navegador corre una aplicación React. Se comunica por Wi-Fi con el ESP8266, que sirve los archivos de la aplicación y abre un WebSocket. El ESP traduce esos mensajes JSON a una trama binaria por UART hacia el ATmega2560, que es quien tiene la autoridad sobre los motores: ahí corren el control de velocidad, la odometría, la pantalla OLED y la lectura de sensores. Abajo, cada componente físico con la interfaz por la que el ATmega lo maneja.",
  fuentes: "AutoFinalRobo2-Contracts/contracts/UART_PROFILE_2.md, PROTOCOL.md; AutoFinalRobo2-Atmega/src/services/afr2_product.c (PID y odometría en ATmega); ESP/platformio.ini; Web/package.json. Renders de componentes: assets_claro/renders/F04_* (PC de escritorio).",
  pend: "Faltan los renders de driver, HC-SR04, sensor de línea, OLED/encoder y baterías (prompt de la PC de escritorio)." });

// 05 · ¿Qué es un microcontrolador?
slide({ id: "F05", ramas: ["E", "P"], t: 45, titulo: "¿Qué es un microcontrolador?",
  clave: "Una computadora completa en un chip, dedicada a una tarea y conectada al mundo físico.",
  pie: "ELECTRÓNICA + PROGRAMACIÓN · Electrónica Digital · Microcontroladores · Sistemas Embebidos",
  build: s => {
    // diagrama de bloques nativo
    glass(s, M, 1.55, 6.6, 5.2, { strong: true });
    text(s, "Dentro del chip", M + 0.25, 1.68, 4, 0.35, { fontFace: HEAD, fontSize: 15, bold: true, color: C.T2 });
    const blk = (t, sub, x, y, w, h, c) => { glass(s, x, y, w, h, { tint: c, r: 0.12 }); text(s, t, x, y + 0.1, w, 0.32, { fontSize: 13.5, bold: true, align: "center" }); if (sub) text(s, sub, x + 0.05, y + 0.42, w - 0.1, h - 0.48, { fontSize: 10.5, color: C.T2, align: "center" }); };
    blk("CPU", "ALU + registros\nunidad de control", M + 0.3, 2.15, 2.0, 1.25, C.PRG);
    blk("Flash", "programa", M + 2.5, 2.15, 1.25, 1.25, C.MEC);
    blk("SRAM", "variables", M + 3.9, 2.15, 1.25, 1.25, C.MEC);
    blk("EEPROM", "configuración", M + 5.3, 2.15, 1.05, 1.25, C.MEC);
    s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x: M + 0.3, y: 3.62, w: 6.05, h: 0.34, rectRadius: 0.17, fill: { color: "94A3B8", transparency: 55 }, line: { color: "94A3B8", width: 0, transparency: 100 } });
    text(s, "BUS INTERNO", M + 0.3, 3.62, 6.05, 0.34, { fontSize: 10, bold: true, color: C.TX, align: "center", valign: "middle", charSpacing: 2 });
    [["GPIO", "pines E/S"], ["Timers", "PWM · tiempos"], ["ADC", "analógico → número"], ["USART", "serie"], ["TWI · SPI", "buses"], ["Interrup.", "eventos"]].forEach(([t, sub], i) => {
      blk(t, sub, M + 0.3 + (i % 3) * 2.05, 4.2 + Math.floor(i / 3) * 1.22, 1.95, 1.08, C.ELE);
    });
    // conceptos
    const cs = [["Programa en un lazo", "Inicializa los periféricos y repite un lazo infinito; las interrupciones atienden eventos urgentes."],
      ["Reloj", "Marca el paso: a 16 MHz el AVR ejecuta casi todas sus instrucciones en un ciclo."],
      ["Registros", "Configurar un periférico es escribir bits en un registro (velocidad, modo, interrupción)."],
      ["Cómo se programa", "C o C++ → compilador cruzado (avr-gcc, xtensa-gcc) → binario .hex / .bin → se graba por bootloader serie o por ISP."]];
    cs.forEach(([t, b], i) => {
      const y = 1.55 + i * 1.32;
      glass(s, 7.6, y, 5.05, 1.2, { tint: i === 3 ? C.PRG : C.ELE });
      text(s, t, 7.8, y + 0.1, 4.7, 0.35, { fontFace: HEAD, fontSize: 15, bold: true, color: i === 3 ? C.PRG_D : C.ELE_D });
      text(s, b, 7.8, y + 0.45, 4.7, 0.72, { fontSize: 12, color: C.TX });
    });
  },
  guion: "Antes de mirar nuestras dos placas, ¿qué es un microcontrolador? Es una computadora completa metida en un solo chip: un procesador, una memoria flash donde queda grabado el programa, una RAM para las variables y una EEPROM para guardar configuración aunque se corte la energía. Alrededor tiene periféricos que tocan el mundo físico: pines de entrada y salida, temporizadores que generan PWM y miden tiempos, un conversor analógico digital, puertos serie y buses. El programa inicializa todo eso y después repite un lazo para siempre, y cuando pasa algo urgente, una interrupción lo atiende. Lo escribimos en C o C++, un compilador cruzado lo traduce al código de ese procesador y lo grabamos en la flash.",
  fuentes: "Datasheet Microchip ATmega2560 (arquitectura AVR, ejecución en un ciclo de la mayoría de las instrucciones); Atmega/Makefile (avr-gcc); ESP/platformio.ini (toolchain xtensa)." });

// 06 · ATmega2560
slide({ id: "F06", ramas: ["E", "P"], t: 40, titulo: "ATmega2560 · Arduino Mega 2560",
  clave: "El microcontrolador con la autoridad sobre motores, sensores y pantalla.",
  pie: "ELECTRÓNICA + PROGRAMACIÓN · Microcontroladores · Sistemas Embebidos",
  build: s => {
    img(s, A("renders/F05_mega2560_iso_v001.png", REN + "F22_A01_RENDER_stack_crop_v001.png"), M, 1.5, 5.3, 3.4);
    img(s, A("renders/F05_mega2560_chip_detalle_v001.png"), M, 5.0, 2.55, 1.75, "center", "Detalle del chip\npendiente");
    img(s, A("renders/F05_mega2560_superior_v001.png"), M + 2.75, 5.0, 2.55, 1.75, "center", "Vista superior\npendiente");
    glass(s, 6.3, 1.5, 6.35, 5.25, { tint: C.ELE });
    text(s, "Características (chip)", 6.55, 1.62, 5.9, 0.4, { fontFace: HEAD, fontSize: 16, bold: true, color: C.ELE_D });
    bullets(s, ["AVR RISC de 8 bits · 16 MHz", "Flash 256 KiB · SRAM 8 KiB · EEPROM 4 KiB", "Placa: 54 E/S digitales (15 con PWM) · 16 entradas ADC de 10 bits",
      "4 USART · TWI (I²C) · SPI · 6 timers (2 de 8 bits, 4 de 16 bits)"], 6.55, 2.05, 5.9, 1.9, 13.5);
    text(s, "Cómo lo programamos", 6.55, 3.95, 5.9, 0.4, { fontFace: HEAD, fontSize: 16, bold: true, color: C.PRG_D });
    bullets(s, ["C con avr-libc, sin Arduino · avr-gcc + Makefile (-mmcu=atmega2560)", "En el auto: PWM de motores (Timer1/2), Hall (PCINT2), ultrasonido (Timer3), servo (Timer5), OLED (TWI), UART1 al ESP"], 6.55, 4.38, 5.9, 2.3, 13.5);
  },
  guion: "El cerebro físico del auto es un ATmega2560 sobre una placa Arduino Mega. Es un microcontrolador de ocho bits a dieciséis megahertz, con doscientos cincuenta y seis kilobytes de flash pero solo ocho de RAM. Lo elegimos por la cantidad de pines, temporizadores e interrupciones: cada periférico del auto tiene su temporizador o su interrupción. Lo programamos en C directo sobre los registros, sin la capa de Arduino, para que los tiempos sean predecibles.",
  fuentes: "Datasheet Microchip ATmega2560 (memorias, USART, timers); ficha de la placa Arduino Mega 2560 (54 E/S, 15 PWM, 16 analógicas); Atmega/Makefile (-mmcu=atmega2560, F_CPU 16 MHz); afr2_motor.c, afr2_board.c, afr2_sensors.c, afr2_twi.c, afr2_uart1.c.",
  pend: "Renders del Mega solo, vista superior y detalle del chip: PC de escritorio." });

// 07 · ESP8266
slide({ id: "F07", ramas: ["P", "E"], t: 40, titulo: "ESP8266 · OSOYOO WiFi Shield (ESP-12)",
  clave: "El procesador de la conectividad: sirve la Web y hace de puente con el ATmega.",
  pie: "PROGRAMACIÓN + ELECTRÓNICA · Sistemas Embebidos · Sistemas Operativos y Redes",
  build: s => {
    img(s, A("renders/F05_wifi_shield_iso_v001.png", REN + "F22_A01_RENDER_stack_crop_v001.png"), M, 1.5, 5.3, 3.4);
    img(s, A("renders/F05_esp12_detalle_v001.png"), M, 5.0, 2.55, 1.75, "center", "Detalle del ESP-12\npendiente");
    img(s, A("renders/F05_wifi_shield_superior_v001.png"), M + 2.75, 5.0, 2.55, 1.75, "center", "Vista superior\npendiente");
    glass(s, 6.3, 1.5, 6.35, 5.25, { tint: C.PRG });
    text(s, "Características", 6.55, 1.62, 5.9, 0.4, { fontFace: HEAD, fontSize: 16, bold: true, color: C.PRG_D });
    bullets(s, ["Tensilica Xtensa L106 de 32 bits · 80/160 MHz", "≈ 80 KiB de RAM de datos · flash SPI externa", "Wi-Fi 802.11 b/g/n 2,4 GHz · punto de acceso y estación"], 6.55, 2.05, 5.9, 1.6, 13.5);
    text(s, "Cómo lo programamos", 6.55, 3.6, 5.9, 0.4, { fontFace: HEAD, fontSize: 16, bold: true, color: C.PRG_D });
    bullets(s, ["C++17 · Arduino Core ESP8266 · PlatformIO", "ESPAsyncWebServer + LittleFS: sirve la Web en gzip", "WebSocket + broker JSON ↔ trama ROBO2 por UART 115200", "Si se cae la red, el ATmega sigue controlando el auto de forma segura"], 6.55, 4.03, 5.9, 2.6, 13.5);
  },
  guion: "El segundo procesador es un ESP8266, en el módulo ESP-12 del shield Wi-Fi. Es de treinta y dos bits, más rápido y con más memoria, y sobre todo tiene Wi-Fi. No toca los motores: guarda la aplicación web en su flash, la sirve comprimida, abre el WebSocket y traduce cada mensaje JSON a nuestra trama binaria hacia el ATmega. Así, si la red se cae, el que manda sobre el auto sigue siendo el ATmega.",
  fuentes: "Datasheet Espressif ESP8266EX; ESP/platformio.ini (esp12e, gnu++17, eagle.flash.4m3m.ld «candidate only»); contracts/PROTOCOL.md; UART_PROFILE_2.md.",
  pend: "Modelo exacto del módulo ESP y tamaño de su flash no identificados físicamente (platformio.ini lo dice). Renders del shield: PC de escritorio." });

// 08 · Ruedas Mecanum
slide({ id: "F08", ramas: ["M"], t: 35, titulo: "Geometría de las ruedas Mecanum",
  clave: "La geometría condiciona el contacto, el montaje y el movimiento.",
  pie: "MECÁNICA · Sistemas de Representación I/II · Mecánica Racional · Robótica II",
  build: s => {
    img(s, A("renders/F06_rueda_iso_v001.png", REN + "F05_A01_RENDER_wheel_iso_crop_v001.png"), 0.4, 1.55, 3.9, 4.2);
    img(s, A("renders/F06_rueda_lateral_v001.png", REN + "F05_A01_RENDER_wheel_side_crop_v001.png"), 4.3, 1.55, 3.4, 4.2);
    text(s, "Mano A y mano B, dos de cada una · D = 80 mm", 0.5, 5.85, 7.2, 0.4, { fontSize: 14, color: C.T2, align: "center" });
    glass(s, 8.0, 1.5, 4.65, 4.65, { tint: C.MEC });
    text(s, "Odometría del chasis  [1][3]", 8.2, 1.6, 4.3, 0.4, { fontFace: HEAD, fontSize: 16, bold: true, color: C.MEC_D });
    eq(s, "Δx = (d₀+d₁+d₂+d₃)/4", 8.05, 2.05, 4.55, 0.45, 19);
    eq(s, "Δy = (−d₀+d₁+d₂−d₃)/4", 8.05, 2.5, 4.55, 0.45, 19);
    eq(s, "Δθ = (−d₀+d₁−d₂+d₃) / (4·(a+b))", 8.05, 2.95, 4.55, 0.45, 17);
    eq(s, "dᵢ = ΔNᵢ · π · D / Nrev", 8.05, 3.4, 4.55, 0.45, 17);
    bullets(s, ["dᵢ: avance de la rueda i (0 FL, 1 FR, 2 RL, 3 RR)", "Δx, Δy, Δθ: avance, corrimiento lateral y giro del chasis", "a + b = 170 mm: suma de las semidistancias entre ruedas",
      "ΔNᵢ: pulsos del encoder · D = 80 mm · Nrev = 370 pulsos/vuelta"], 8.2, 3.95, 4.3, 2.1, 10.5, C.T2);
    refs(s, ["LP", "FW"]);
  },
  guion: "Las ruedas son Mecanum: los rodillos están inclinados respecto del eje, y por eso combinando el sentido de las cuatro ruedas el auto puede avanzar, ir de costado o girar sobre sí mismo. Esa misma geometría aparece en el firmware: la odometría suma los avances de las cuatro ruedas para obtener el desplazamiento hacia adelante, los combina con signos alternados para el lateral, y divide por la suma de semidistancias, ciento setenta milímetros, para el giro. Cada avance sale de contar pulsos del encoder: pulsos por el perímetro de la rueda de ochenta milímetros, dividido trescientos setenta pulsos por vuelta.",
  fuentes: "afr2_product.c odometry() (fórmulas dx, dy, angle); afr2_geometry (AFR2_FIXED_WHEEL_MM 80, AFR2_FIXED_RADIUS_SUM_MM 170); Lynch y Park, Modern Robotics (2017), cap. 13.",
  pend: "No afirmar ángulo de rodillos (no medido)." });

// 09 · Cinemática matricial
slide({ id: "F09", ramas: ["M", "C"], t: 70, titulo: "Cinemática matricial del chasis Mecanum",
  clave: "Matrices que pasan de las ruedas al chasis y del chasis al mundo.",
  pie: "MECÁNICA + CONTROL · Robótica I/II · Mecánica Racional · Álgebra",
  build: s => {
    img(s, DIA + "F06_A01_VECTOR_matrices_cinematica_v002.png", M, 1.45, W - 2 * M, 5.05);
    refs(s, ["LP", "CR", "FW", "SG"]);
  },
  guion: "Esto es lo que ampliamos en Robótica II. La cinemática inversa nos dice cuánto tiene que girar cada rueda para que el chasis tenga una velocidad de avance, una lateral y una de giro: es una matriz de cuatro por tres, la misma que usa el firmware. La directa va al revés: con la pseudoinversa, de las cuatro velocidades de rueda sacamos las tres del chasis. Como la matriz tiene rango tres, no hay singularidades mientras la distancia L no sea cero; y como hay cuatro ruedas para tres grados de libertad, queda un modo interno en el que las ruedas se empujan entre sí sin mover el auto. La transformación homogénea ubica el chasis en el mundo, el jacobiano junta todo, y la odometría es esa misma cuenta integrada cada ciclo, girando por el ángulo medio del intervalo.",
  fuentes: "afr2_product.c:130 (v = [vx−vy−t, vx+vy+t, vx+vy−t, vx−vy+t], t = ωz·L), :295-304 (dx, dy, angle; heading = yaw + angle/2); Lynch y Park (2017) §13.2; Craig (2005) cap. 2.",
  pend: "Los signos de las matrices son los de la base lógica del chasis; el firmware además multiplica por AFR2_FORWARD/LATERAL/YAW_WHEEL_SIGN y por factores de escala por dirección (odom_scale)." });

// 10 · De un perfil 2D a placas
slide({ id: "F10", ramas: ["M"], t: 40, titulo: "De un perfil 2D a placas tridimensionales",
  clave: "Perfil 2D → geometría CAD → extrusión → pieza fabricable.",
  pie: "MECÁNICA · Sistemas de Representación II (CAD 2D/3D, planos de conjunto y despiece)",
  build: s => {
    const st = [["Perfil 2D", "contorno de la placa del kit"], ["Sketch CAD", "Fusion: importar y acotar"], ["Extrusión", "espesor de la placa"], ["Pieza", "agujeros y ranuras reales"]];
    const SW = 2.85, SG = (W - 2 * M - 4 * SW) / 3;
    st.forEach(([a, b], i) => {
      const x = M + i * (SW + SG);
      glass(s, x, 1.5, SW, 0.95, { tint: C.MEC });
      text(s, `${i + 1} · ${a}`, x, 1.58, SW, 0.38, { fontFace: HEAD, fontSize: 15, bold: true, align: "center" });
      text(s, b, x, 1.97, SW, 0.4, { fontSize: 11.5, color: C.T2, align: "center" });
      if (i < 3) text(s, "→", x + SW, 1.68, SG, 0.5, { fontSize: 20, color: C.MEC_D, align: "center" });
    });
    glass(s, M, 2.65, 5.95, 4.15, { strong: true });
    img(s, A("planos/F07_plano_placa_superior_planta_v001.png", A("planos/F07_plano_placas_v001.png")), M + 0.12, 2.75, 5.71, 3.6, "center", "Plano en planta de las placas\npendiente (Fusion Drawing)");
    text(s, "Plano · vista superior", M, 6.38, 5.95, 0.32, { fontSize: 12, color: C.T2, align: "center" });
    img(s, A("renders/F07_placas_iso_separadas_v001.png", A("renders/F07_placas_iso_v001.png")), 6.9, 2.65, 5.75, 3.7, "center", "Render isométrico de las dos placas solas\npendiente (Blender)");
    text(s, "Placa superior e inferior · render isométrico", 6.9, 6.38, 5.75, 0.32, { fontSize: 12, color: C.T2, align: "center" });
  },
  guion: "Las placas del chasis vienen del kit. Para modelarlas partimos del contorno en dos dimensiones, lo llevamos a un sketch en Fusion, lo extruimos con el espesor de la placa y verificamos que agujeros y ranuras coincidan con los de la pieza real. A la izquierda, el plano en planta con sus cotas; a la derecha, las dos placas solas en isométrica. Es exactamente lo que se practica en Sistemas de Representación II: pasar de un dibujo plano a un sólido y del sólido a un plano.",
  fuentes: "AutoFinalRobo2-CAD/mecanica/auto-completo/fusion_ensamble.py, AutoFinalRobo2.f3d; objetos AFR2_CapaSuperior/AFR2_CapaInferior; plano: assets_claro/planos (Drawing nuevo, PC de escritorio).",
  pend: "Confirmar con qué archivo se generó el perfil antes de nombrarlo SVG. No afirmar material (acrílico/chapa) sin confirmarlo. Si el plano solo tiene cotas envolventes, rotularlo preliminar." });

// 11 · Ensamblaje CAD de componentes
slide({ id: "F11", ramas: ["M", "E"], t: 50, titulo: "Ensamblaje CAD de componentes",
  clave: "Traemos lo que existe, modelamos lo que faltaba y diseñamos lo propio.",
  pie: "MECÁNICA + ELECTRÓNICA · Sistemas de Representación II · Electrónica Digital · Proyecto de Ingeniería Mecatrónica",
  build: s => {
    const col = (x, w, t, c, cd) => { glass(s, x, 1.5, w, 4.1, { tint: c }); text(s, t, x + 0.18, 1.6, w - 0.36, 0.38, { fontFace: HEAD, fontSize: 15, bold: true, color: cd }); };
    const mini = (p, t, x, y, w, h) => { img(s, p, x, y, w, h - 0.3, "center", "render\npendiente"); text(s, t, x - 0.05, y + h - 0.3, w + 0.1, 0.28, { fontSize: 9.5, bold: true, align: "center" }); };
    // catálogo
    col(M, 4.15, "Catálogos y bibliotecas (GrabCAD)", C.MEC, C.MEC_D);
    [["Rueda Mecanum", A("renders/F06_rueda_iso_v001.png", REN + "F05_A01_RENDER_wheel_iso_crop_v001.png")], ["GM25-370 + Hall", A("renders/F04_motor_gm25_iso_v001.png", REN + "F14_A01_RENDER_motor_gm25_crop_v001.png")],
     ["Servo SG90", A("renders/F04_servo_sg90_iso_v001.png", REN + "F18_A01_RENDER_servo_sg90_crop_v001.png")], ["HC-SR04", A("renders/F04_hcsr04_frente_v001.png")],
     ["Línea 5 IR", A("renders/F04_sensor_linea_inferior_v001.png")], ["Mega 2560", A("renders/F05_mega2560_iso_v001.png")]].forEach(([t, p], i) =>
      mini(p, t, M + 0.2 + (i % 3) * 1.27, 2.05 + Math.floor(i / 3) * 1.45, 1.17, 1.38));
    text(s, "Modelos descargados e importados al ensamble sin rediseñarlos.", M + 0.2, 4.98, 3.8, 0.55, { fontSize: 11, color: C.T2 });
    // modelados por nosotros
    col(5.0, 3.0, "Modelados por nosotros", C.CTL, C.CTL_D);
    mini(A("renders/F04_driver_superior_v001.png"), "OSOYOO Motor Driver Model Y V2", 5.2, 2.05, 2.6, 1.45);
    mini(A("renders/F05_wifi_shield_iso_v001.png"), "OSOYOO WiFi Shield", 5.2, 3.55, 2.6, 1.45);
    text(s, "No había modelo CAD de estas placas: los dibujamos nosotros.", 5.2, 5.0, 2.65, 0.55, { fontSize: 11, color: C.T2 });
    // diseño propio
    col(8.2, 4.45, "Diseño propio", C.ELE, C.ELE_D);
    img(s, A("renders/F09_panel_ui_montaje_v001.png", path.resolve(V3, "../../AutoFinalRobo2-CAD/exports/oled-encoder/Panel_UI_Fusion_A.png")), 8.35, 2.0, 1.9, 1.75);
    bullets(s, ["Soporte del panel OLED + encoder", "PCB OLED/encoder en KiCad", "Divisor de batería y su montaje"], 10.35, 2.1, 2.2, 1.7, 12);
    text(s, "Diseñado sobre la cubierta superior del auto ya modelada: el soporte se monta con torretas M3, casi sin intervenir el chasis.", 8.4, 3.9, 4.1, 1.6, { fontSize: 12, color: C.TX });
    // flujo
    const fl = [["KiCad", "esquemático → PCB → 3D", C.ELE], ["STEP", "modelo de la placa", C.GR], ["Fusion 360", "ensamble con uniones", C.MEC], ["OBJ · USDZ", "exportación", C.GR], ["Blender", "GLB liviano para la Web", C.PRG]];
    const FW = 2.15, FG = (W - 2 * M - 5 * FW) / 4;
    fl.forEach(([a, b, c], i) => {
      const x = M + i * (FW + FG);
      glass(s, x, 5.8, FW, 0.95, { tint: c === C.GR ? null : c, strong: c === C.GR });
      text(s, a, x, 5.86, FW, 0.38, { fontFace: HEAD, fontSize: 14, bold: true, align: "center" });
      text(s, b, x, 6.24, FW, 0.4, { fontSize: 10.5, color: C.T2, align: "center" });
      if (i < 4) s.addShape(pres.shapes.LINE, { x: x + FW + 0.05, y: 6.27, w: FG - 0.1, h: 0, line: { color: C.T2, width: 1.5, endArrowType: "triangle" } });
    });
  },
  guion: "El auto completo vive en un solo ensamble de Fusion. Lo que ya existía lo trajimos de catálogos y bibliotecas como GrabCAD: las ruedas, los motores, el servo, el ultrasónico, el sensor de línea y el Mega. Dos placas no tenían modelo CAD, el driver de motores y el shield Wi-Fi, así que las dibujamos nosotros. Y lo que no existía lo diseñamos: el soporte del panel, su placa en KiCad y el divisor de batería. La placa sale de KiCad como STEP, entra a Fusion, se arma con uniones, y de ahí exportamos a Blender para la Web.",
  fuentes: "Declarado por el equipo: procedencia GrabCAD de ruedas y sensores; Motor Driver y WiFi Shield modelados por el equipo. Nombres de componente del GLB; CAD/reference/inventario-CADS-2026-10-23.csv; exports/2026-10-24/parts (OBJ placas, USDZ resto); CAD/docs/MODELO_FINAL_WEB_20261025.md.",
  pend: "Cruzar la procedencia de cada modelo con el inventario CAD antes de citar autores de GrabCAD." });

// 12 · Panel OLED + encoder con KiCad
slide({ id: "F12", ramas: ["E", "M"], t: 50, titulo: "Diseño propio: panel OLED + encoder",
  clave: "Una función electrónica nueva también exige una solución mecánica.",
  pie: "ELECTRÓNICA → MECÁNICA · Electrónica Digital · Microcontroladores · Sistemas de Representación II",
  build: s => {
    const shots = [["Esquemático (KiCad)", "qué se conecta con qué: símbolos, redes y pull-ups", A("capturas/F12_kicad_esquematico_v001.png", CAP + "PlacaOLED-Encoder-AutoRobo2_crop.png")],
      ["Diseño PCB (KiCad)", "ruteo de cobre en una cara (B.Cu), 20 × 35 mm", A("capturas/F12_kicad_pcb_editor_v001.png", CAP + "F12_A02_RENDER_kicad_pcb_v001.png")],
      ["Modelo 3D en Fusion", "STEP de KiCad → encaje en el soporte", A("renders/F09_panel_ui_montaje_v001.png", path.resolve(V3, "../../AutoFinalRobo2-CAD/exports/oled-encoder/Panel_UI_Fusion_A.png"))]];
    const SW = 3.85, SG = (W - 2 * M - 3 * SW) / 2;
    shots.forEach(([t, sub, p], i) => {
      const x = M + i * (SW + SG);
      glass(s, x, 1.5, SW, 0.72, { tint: [C.ELE, C.ELE, C.MEC][i] });
      text(s, t, x + 0.15, 1.54, SW - 0.3, 0.32, { fontFace: HEAD, fontSize: 13.5, bold: true, color: [C.ELE_D, C.ELE_D, C.MEC_D][i] });
      text(s, sub, x + 0.15, 1.86, SW - 0.3, 0.3, { fontSize: 10.5, color: C.T2 });
      s.addShape(pres.shapes.LINE, { x: x + SW / 2, y: 2.24, w: 0, h: 0.2, line: { color: [C.ELE_D, C.ELE_D, C.MEC_D][i], width: 1.75, endArrowType: "triangle" } });
      glass(s, x, 2.5, SW, 2.35, { strong: true });
      img(s, p, x + 0.08, 2.56, SW - 0.16, 2.23);
      if (i < 2) text(s, "→", x + SW, 3.35, SG, 0.6, { fontFace: HEAD, fontSize: 22, bold: true, color: C.T2, align: "center" });
    });
    glass(s, M, 5.0, 5.9, 1.8, { tint: C.MEC });
    text(s, "Montaje", M + 0.2, 5.07, 5.5, 0.35, { fontFace: HEAD, fontSize: 14, bold: true, color: C.MEC_D });
    bullets(s, ["Soporte diseñado sobre la cubierta superior del auto: se monta casi sin intervención", "Torretas M3 sobre la placa superior (15 mm para el OLED, 5 mm soporte–auto)", "Perilla accesible desde atrás del auto"], M + 0.2, 5.43, 5.55, 1.35, 11.5);
    glass(s, 6.75, 5.0, 5.9, 1.8, { tint: C.ELE });
    text(s, "Conexión al ATmega2560", 6.95, 5.07, 5.5, 0.35, { fontFace: HEAD, fontSize: 14, bold: true, color: C.ELE_D });
    bullets(s, ["I²C (TWI, 100 kHz): SDA D20 · SCL D21", "Encoder KY-040: CLK D2 (INT4) · DT D3 (INT5) · SW D42 · pull-ups 10 kΩ", "Señales por hembra 1×5 de 2,54 mm · alimentación aparte"], 6.95, 5.43, 5.55, 1.35, 11.5);
  },
  guion: "El panel con pantalla y perilla es nuestro diseño de punta a punta. En KiCad primero va el esquemático, que dice qué se conecta con qué; después el diseño de la placa, donde se rutean las pistas de cobre en una sola cara de veinte por treinta y cinco milímetros. Cada huella tiene su modelo 3D, así que KiCad exporta la placa armada en STEP y en Fusion verificamos que encaje. El soporte lo diseñamos sobre la cubierta superior que ya teníamos modelada, con torretas M3, para montarlo casi sin tocar el auto. Eléctricamente, la pantalla va al puerto I²C del ATmega, pines veinte y veintiuno, y la perilla a dos pines con interrupción.",
  fuentes: "CAD/docs/PANEL_UI_20261025.md (20×35 mm, B.Cu, torretas M3); CAD/docs/MODELO_FINAL_WEB_20261025.md (torretas 15 mm y 5 mm, hembra 1×5 2,54 mm, JST 2 mm de alimentación, R 10 kΩ a 3V3); afr2_twi.c (TWBR 72 → 100 kHz); afr2_board.c (INT4/INT5); contracts/hardware/PINOUT.md.",
  pend: "Conector de alimentación: AGENTS.md dice 1×2 de 2,54 mm y el informe final del panel dice JST 2 mm; confirmar el montado. Tensión del panel (3V3 en el esquemático, 5 V en PINOUT.md) sin resolver. Captura del editor PCB de KiCad: PC de escritorio (KiCad 9 de esta máquina no abre la placa)." });

// 12 · Ensamble en Fusion 360
slide({ id: "F12F", ramas: ["M"], t: 45, titulo: "Ensamble en Fusion 360",
  clave: "Las piezas se vinculan con uniones que fijan su posición y sus grados de libertad.",
  pie: "MECÁNICA · Sistemas de Representación II · Proyecto de Ingeniería Mecatrónica",
  build: s => {
    glass(s, M, 1.5, 4.6, 5.25, { tint: C.MEC });
    img(s, A("logos/fusion360.png"), M + 0.25, 1.68, 0.9, 0.9, "center", "logo");
    text(s, "Fusion 360", M + 1.3, 1.8, 3.1, 0.45, { fontFace: HEAD, fontSize: 20, bold: true });
    text(s, "Autodesk · CAD/CAM/CAE en la nube", M + 1.3, 2.22, 3.2, 0.35, { fontSize: 11.5, color: C.T2 });
    bullets(s, ["Diseño paramétrico: bocetos, extrusiones y una línea de tiempo editable", "Ensambles: componentes + uniones (joints) que restringen el movimiento",
      "Importa el STEP de KiCad y los modelos de catálogo", "Genera planos 2D asociados al modelo 3D", "Exporta las piezas para Blender (OBJ y USDZ)"], M + 0.25, 2.85, 4.15, 3.8, 13.5);
    framed(s, A("capturas/F08_fusion_ensamble_v001.png"), 5.55, 1.5, 7.1, 3.55, "Captura de Fusion con el ensamble, el navegador y el timeline\npendiente (PC de escritorio)");
    framed(s, A("capturas/F08_fusion_joints_v001.png"), 5.55, 5.2, 3.45, 1.55, "Detalle de joints\npendiente");
    framed(s, A("renders/F08_explosionado_v001.png", REN + "F07_A01_RENDER_exploded_crop_v001.png"), 9.2, 5.2, 3.45, 1.55, "Explosionado pendiente");
  },
  guion: "Para armar el auto completo usamos Fusion 360, el CAD de Autodesk. Es paramétrico: cada pieza nace de bocetos y operaciones que quedan en una línea de tiempo y se pueden editar. En el ensamble, cada componente se vincula con uniones que le quitan grados de libertad: la rueda queda girando sobre el eje del motor, el motor fijo a su soporte. Ahí entran también la placa que exportamos de KiCad y los modelos de catálogo, y desde el mismo modelo salen los planos y la exportación hacia Blender.",
  fuentes: "AutoFinalRobo2-CAD/mecanica/auto-completo/AutoFinalRobo2-Final-20261025.f3d, fusion_ensamble.py, fusion_exportar_blender.py; docs/ARMADO_FUSION_PASO_A_PASO.md; exports/2026-10-24/parts (OBJ para placas, USDZ para el resto).",
  pend: "Capturas de Fusion y logo: PC de escritorio. La vista explotada es solo visual, no un orden de montaje." });

// 13 · De Fusion a Blender
slide({ id: "F13", ramas: ["M", "P"], t: 50, titulo: "De Fusion a Blender: un modelo liviano para la Web",
  clave: "Mismo auto, mismos colores y materiales, una fracción del peso.",
  pie: "MECÁNICA → PROGRAMACIÓN · Sistemas de Representación II · Computación II",
  build: s => {
    glass(s, M, 1.5, 4.6, 2.95, { tint: C.ELE });
    img(s, A("logos/blender.png"), M + 0.25, 1.68, 0.9, 0.9, "center", "logo");
    text(s, "Blender", M + 1.3, 1.8, 3.1, 0.45, { fontFace: HEAD, fontSize: 20, bold: true });
    text(s, "Software libre de modelado, render y animación", M + 1.3, 2.22, 3.25, 0.4, { fontSize: 11.5, color: C.T2 });
    bullets(s, ["Se importó el ensamble exportado de Fusion", "Se renderizó con Cycles", "Se recortaron triángulos innecesarios (Decimate en la copia visual)", "Se unificaron materiales iguales sin cambiar colores"], M + 0.25, 2.8, 4.15, 1.6, 12.5);
    framed(s, A("capturas/F09_blender_importado_v001.png"), 5.55, 1.5, 7.1, 2.95, "Captura de Blender con el modelo importado\npendiente (PC de escritorio)");
    // peso antes → después
    const before = PESO && PESO.antes_bytes ? PESO.antes_bytes : 9319792, after = PESO && PESO.despues_bytes ? PESO.despues_bytes : 827232;
    const fmt = b => (b / 1e6).toLocaleString("es-AR", { maximumFractionDigits: 2 }) + " MB";
    const pct = Math.round((1 - after / before) * 1000) / 10;
    const lb = PESO && PESO.antes_etiqueta ? PESO.antes_etiqueta : "Exportado de Fusion (OBJ + USDZ, 24/10)";
    const la = PESO && PESO.despues_etiqueta ? PESO.despues_etiqueta : "GLB con Draco (24/10)";
    glass(s, M, 4.65, 4.0, 2.1, { strong: true });
    text(s, "ANTES", M, 4.8, 4.0, 0.3, { fontSize: 11, bold: true, color: C.T2, align: "center", charSpacing: 2 });
    text(s, fmt(before), M, 5.1, 4.0, 0.75, { fontFace: HEAD, fontSize: 36, bold: true, align: "center", valign: "middle" });
    text(s, lb, M + 0.1, 5.9, 3.8, 0.6, { fontSize: 11, color: C.T2, align: "center" });
    text(s, "→", 4.75, 5.05, 1.2, 0.8, { fontFace: HEAD, fontSize: 40, bold: true, color: C.PRG_D, align: "center", valign: "middle" });
    glass(s, 5.95, 4.65, 4.0, 2.1, { tint: C.PRG });
    text(s, "DESPUÉS", 5.95, 4.8, 4.0, 0.3, { fontSize: 11, bold: true, color: C.PRG_D, align: "center", charSpacing: 2 });
    text(s, fmt(after), 5.95, 5.1, 4.0, 0.75, { fontFace: HEAD, fontSize: 36, bold: true, align: "center", valign: "middle" });
    text(s, la, 6.05, 5.9, 3.8, 0.6, { fontSize: 11, color: C.T2, align: "center" });
    glass(s, 10.15, 4.65, 2.5, 2.1, { tint: C.PRG, strong: true });
    text(s, `−${pct.toLocaleString("es-AR")} %`, 10.15, 4.95, 2.5, 0.85, { fontFace: HEAD, fontSize: 30, bold: true, color: C.PRG_D, align: "center", valign: "middle" });
    text(s, "de peso", 10.15, 5.8, 2.5, 0.35, { fontSize: 12, color: C.T2, align: "center" });
  },
  guion: "De Fusion exportamos las piezas y las abrimos en Blender, que es un programa libre de modelado, render y animación. Ahí preparamos el modelo para la página: lo renderizamos, recortamos los triángulos que no se ven o que no aportan forma, y unificamos los materiales repetidos sin tocar los colores. El resultado se exporta como GLB comprimido. Pasamos de varios megabytes a menos de uno: es lo que permite que el propio auto sirva su modelo 3D por Wi-Fi.",
  fuentes: "AutoFinalRobo2-CAD/exports/2026-10-24/parts (suma 9.319.792 B, OBJ + USDZ) y exports/2026-10-24/AutoFinalRobo2-ligero.glb (827.232 B); docs/RUEDAS_Y_BLENDER_20261024.md; blender_ligero.py (Decimate); modelo final 25/10: 871.756 B (docs/modelo-final-20261025.json).",
  pend: "El par antes/después por defecto es el del 24/10 (misma exportación). Si llega assets_claro/peso_modelo.json de la PC de escritorio con la medición del modelo final, se usa ese." });

// 14 · Fuerzas y momentos modelados
slide({ id: "F14", ramas: ["M"], t: 55, titulo: "Fuerzas y momentos modelados",
  clave: "Los esfuerzos definen nuestras solicitaciones físicas.",
  pie: "MECÁNICA · Estática y Resistencia de Materiales · Mecánica Racional · Máquinas y Elementos de Máquinas",
  build: s => {
    img(s, FOT + "FOTO-01_robot_completo_CAD_v17.png", M - 0.1, 1.45, 3.4, 2.8);
    glass(s, 4.1, 1.42, 8.55, 2.85, { strong: true });
    img(s, DIA + "F_brazo_cargas_momento_v001.png", 4.15, 1.45, 8.45, 2.8);
    glass(s, M, 4.35, 5.9, 1.85, { tint: C.MEC });
    img(s, DIA + "EQ_momento_vectorial.png", M + 0.05, 4.37, 5.8, 1.81);
    glass(s, 6.75, 4.35, 5.9, 1.85, { tint: C.MEC });
    img(s, DIA + "EQ_momento_magnitud.png", 6.8, 4.37, 5.8, 1.81);
    refs(s, ["HB", "EST"]);
  },
  guion: "El brazo es donde la estática se vuelve concreta. Cada tramo del brazo pesa, y su peso actúa en su centro de masa: son las flechas rojas, más la carga de doscientos gramos en la pinza. Cada fuerza, respecto de una articulación, produce un momento: en forma vectorial es r por F, y en magnitud es la fuerza por la distancia perpendicular, el brazo de palanca. Abajo, el momento flector a lo largo del brazo: crece hacia el hombro porque ahí se suman todos los pesos con sus distancias. Por eso el hombro y el primer eslabón son los más exigidos.",
  fuentes: "analisis_cargas_200g/resultados_estatica.json (caso CAD_macizo_200g: masas por grupo, J2 2,911 N·m); posiciones de los centros de masa leídas de cargas_y_momentos.png; M(x) recalculado por masas concentradas; FOTO-01; Hibbeler, Estática, cap. 4.",
  pend: "El análisis estático es de la versión v06 del brazo y el render es v17: aclararlo si se pregunta. El brazo no está aprobado para fabricación ni resistencia." });

// 15 · Esfuerzos internos
slide({ id: "F15", ramas: ["M"], t: 45, titulo: "Esfuerzos internos en una sección",
  clave: "Si cortamos el brazo en un punto, aparecen cuatro esfuerzos distintos.",
  pie: "MECÁNICA · Estática y Resistencia de Materiales · Máquinas y Elementos de Máquinas",
  build: s => {
    img(s, DIA + "F_esfuerzos_seccion_v001.png", M - 0.2, 1.4, W - 2 * M + 0.4, 5.05);
    refs(s, ["BJ", "EST"]);
  },
  guion: "Si imaginamos que cortamos el brazo en una sección, para que cada mitad siga en equilibrio aparecen cuatro esfuerzos. La fuerza axial, N, estira o comprime a lo largo del tubo: en la torre es el peso que baja hacia la base. El cortante, V, intenta deslizar una cara sobre la otra. El momento flector, M, dobla el tramo: comprime de un lado y tracciona del otro, y es el que más crece hacia el hombro. Y el torsor, T, lo retuerce. Dos son fuerzas y dos son momentos: no se suman entre sí, cada uno se calcula por separado.",
  fuentes: "analisis_cargas_200g/05_frenada_y_esfuerzos_combinados.md (N, V, T, M y dónde aparecen); Beer y Johnston, Mecánica de materiales." });

// 16 · Elementos de máquinas en el brazo
slide({ id: "F16", ramas: ["M"], t: 50, titulo: "Elementos de máquinas en el brazo",
  clave: "Transmisiones, apoyos y estructura: la mecánica aplicada en una sola pieza.",
  pie: "MECÁNICA · Máquinas y Elementos de Máquinas · Sistemas de Representación II · Estática y Resistencia de Materiales",
  build: s => {
    // imagen y puntos: render lateral de la PC de escritorio (con JSON de puntos) o FOTO-07 con puntos estimados
    const pj = NEW + "renders/F_brazo_lateral_v001.json";
    let p = FOT + "FOTO-07_brazo_v17_CAD.png", pts = { torre: [400, 640], j2: [335, 385], j3: [640, 470], j1: [420, 975], pinza: [1085, 320], servo: [545, 500] };
    if (fs.existsSync(pj) && A("renders/F_brazo_lateral_v001.png")) { p = A("renders/F_brazo_lateral_v001.png"); pts = Object.assign(pts, JSON.parse(fs.readFileSync(pj, "utf8")).points || {}); }
    const r = img(s, p, M - 0.1, 1.45, 6.3, 5.3), [iw] = imgSize(p), k = r.w / iw;
    const at = ([px, py]) => [r.x + px * k, r.y + py * k];
    const call = [["torre", "Torre con arriostramiento en X", "Diseño 3D: transmite los esfuerzos del brazo a J1 → ensayo por elementos finitos", C.MEC],
      ["j2", "Hombro J2 · engranajes", "Rueda dentada 18/72, módulo 0,8 (reducción 4:1) · eje Ø5 en dos rodamientos 625", C.ELE],
      ["j3", "Codo J3 · correa dentada", "Correa HP3M y poleas 20/50 (relación 2,5) desde el motor junto a J2", C.CTL],
      ["j1", "Base J1 · rodamiento", "Cartucho con rodamiento 6000 en la montura baja: soporta axial y flexión", C.PRG],
      ["pinza", "Pinza · piñón", "Un piñón mueve las dos mordazas", C.ELE],
      ["servo", "Actuadores", "Servos STS3250 / STS3032 en cunas impresas con tapas y horns", C.MEC]];
    const LX = 7.05, LW = 5.6, LH = 0.72;
    call.sort((a, b) => pts[a[0]][1] - pts[b[0]][1]);
    call.forEach(([key, t, b, c], i) => {
      const y = 1.45 + i * 0.79;
      glass(s, LX, y, LW, LH, { tint: c });
      text(s, t, LX + 0.15, y + 0.05, LW - 0.3, 0.3, { fontFace: HEAD, fontSize: 12.5, bold: true });
      text(s, b, LX + 0.15, y + 0.32, LW - 0.3, 0.4, { fontSize: 10, color: C.T2, fit: "shrink" });
      const [tx, ty] = at(pts[key]);
      s.addShape(pres.shapes.OVAL, { x: tx - 0.07, y: ty - 0.07, w: 0.14, h: 0.14, fill: { color: c }, line: { color: "FFFFFF", width: 1.5 } });
      const x1 = LX, y1 = y + LH / 2;
      s.addShape(pres.shapes.LINE, { x: Math.min(tx, x1), y: Math.min(ty, y1), w: Math.abs(x1 - tx), h: Math.abs(y1 - ty), flipH: false, flipV: (y1 < ty) !== (x1 < tx),
        line: { color: c, width: 1.5, transparency: 10 } });
    });
    refs(s, ["BN", "V17"]);
  },
  guion: "En el brazo aparece casi todo lo que vemos en Elementos de Máquinas. El hombro mueve el brazo con un par de ruedas dentadas de dieciocho y setenta y dos dientes, una reducción de cuatro a uno, sobre un eje apoyado en dos rodamientos. El codo se mueve con una correa dentada y poleas de veinte y cincuenta, porque su motor está junto al hombro para no cargar peso en la punta. La base gira sobre un cartucho con un rodamiento que aguanta carga axial y flexión. Y la torre con la cruz en el medio es diseño propio: transmite todos los esfuerzos a la base, y por eso es la pieza que llevamos a un ensayo por elementos finitos.",
  fuentes: "revision_v17/README.md (engranajes 18/72 módulo 0,8, eje 5 con dos 625, correa HP3M210x6, poleas 20/50, relación 2,5, cartucho 6000, STS3250/STS3032); FOTO-07; Budynas y Nisbett, Shigley.",
  pend: "Los puntos de las flechas sobre FOTO-07 son estimados a ojo: se reemplazan por los del render lateral con JSON de la PC de escritorio. Las poleas y engranajes del CAD son discos a diámetro primitivo, no dentados fabricables (README v17). Brazo no aprobado para fabricar." });

// 17 · Elementos finitos en la torre
slide({ id: "F17", ramas: ["M"], t: 45, titulo: "Ensayo por elementos finitos: torre J1–J2",
  clave: "El modelo se divide en miles de elementos y se resuelve el equilibrio en cada uno.",
  pie: "MECÁNICA · Estática y Resistencia de Materiales · Máquinas y Elementos de Máquinas",
  build: s => {
    const fea = (() => { try { return JSON.parse(fs.readFileSync(NEW + "renders/F_fea_torre_v001.json", "utf8")); } catch (e) { return null; } })();
    glass(s, M, 1.45, 7.4, 5.3, { strong: true });
    img(s, A("renders/F_fea_torre_von_mises_v001.png"), M + 0.1, 1.55, 7.2, 5.1, "center", "Mapa de tensiones (von Mises) de la torre con las cargas activadas\npendiente (Fusion Simulation, PC de escritorio)");
    img(s, A("renders/F_fea_torre_desplazamiento_v001.png"), 8.35, 1.45, 2.1, 2.1, "center", "Desplaza-\nmiento");
    img(s, A("renders/F_fea_torre_seguridad_v001.png"), 10.55, 1.45, 2.1, 2.1, "center", "Factor de\nseguridad");
    glass(s, 8.35, 3.7, 4.3, 3.05, { tint: C.MEC });
    text(s, "Cómo se lee", 8.55, 3.78, 4, 0.35, { fontFace: HEAD, fontSize: 14, bold: true, color: C.MEC_D });
    const lines = fea ? [`Material: ${fea.material}`, `Cargas: ${fea.cargas}`, `Tensión máx. von Mises: ${fea.von_mises_max_MPa} MPa`, `Factor de seguridad mín.: ${fea.factor_seguridad_min}`]
      : ["von Mises: combina N, V, M y T en una sola tensión equivalente", "Azul = poco exigido · rojo = zona más cargada", "Factor de seguridad = resistencia del material / tensión"];
    bullets(s, lines, 8.55, 4.15, 3.95, 2.5, 11.5);
  },
  guion: "La torre que une la base con el hombro es la pieza por la que pasan todos los esfuerzos del brazo. Para ver cómo se reparten, hicimos un ensayo por elementos finitos: el programa divide la pieza en miles de elementos chicos, aplica las cargas y los apoyos, y resuelve el equilibrio en cada uno. El color muestra la tensión equivalente de von Mises: en azul la pieza casi no trabaja, en rojo está lo más exigido. La cruz en el medio está justamente para repartir esa carga.",
  fuentes: "Estudio de tensión estática de Fusion sobre la torre J1–J2 (PC de escritorio; assets_claro/renders/F_fea_torre_v001.json con material, cargas y resultados); reacciones de analisis_cargas_200g/reacciones_para_FEA.csv.",
  pend: "Resultado ilustrativo: material isotrópico macizo; una pieza impresa en PETG con relleno parcial no queda acreditada por este modelo (informe 05 del análisis). No presentar como aprobación de fabricación." });

// 18 · Centro de masa, inercia y vuelco
slide({ id: "F18", ramas: ["M", "C"], t: 60, titulo: "Centro de masa, inercia y vuelco",
  clave: "Mover masa cambia la dinámica y el margen de estabilidad.",
  pie: "MECÁNICA + CONTROL · Mecánica Racional · Robótica I · Estática y Resistencia de Materiales",
  build: s => {
    const L = A("renders/F18_auto_brazo_lateral_v001.png"), Fr = A("renders/F18_auto_brazo_frontal_v001.png");
    const pj = (n) => { try { return JSON.parse(fs.readFileSync(NEW + "renders/" + n + ".json", "utf8")).points; } catch (e) { return null; } };
    const PL = pj("F18_auto_brazo_lateral_v001"), PF = pj("F18_auto_brazo_frontal_v001");
    const arrow = (x1, y1, x2, y2, c) => s.addShape(pres.shapes.LINE, { x: Math.min(x1, x2), y: Math.min(y1, y2), w: Math.max(0.001, Math.abs(x2 - x1)), h: Math.max(0.001, Math.abs(y2 - y1)),
      flipH: x2 < x1, flipV: y2 < y1, line: { color: c, width: 3, endArrowType: "triangle" } });
    const tag = (t, x, y, c) => text(s, t, x, y, 1.6, 0.3, { fontSize: 11, bold: true, color: c });
    if (L && PL && Fr && PF) {
      const r1 = img(s, L, M - 0.1, 1.45, 6.2, 2.95), r2 = img(s, Fr, 6.4, 1.45, 3.2, 2.95);
      const k1 = r1.w / imgSize(L)[0], k2 = r2.w / imgSize(Fr)[0];
      const a1 = ([x, y]) => [r1.x + x * k1, r1.y + y * k1], a2 = ([x, y]) => [r2.x + x * k2, r2.y + y * k2];
      let [x, y] = a1(PL.tcp); arrow(x, y, x, y + 0.8, "DC2626"); tag("P carga", x + 0.08, y + 0.45, "B91C1C");
      [x, y] = a1(PL.torre_top); arrow(x, y - 0.1, x, y + 0.7, C.ELE_D); tag("N axial", x + 0.08, y + 0.1, C.ELE_D);
      [x, y] = a1(PL.cg); arrow(x, y, x, y + 0.7, C.MEC_D); tag("m g", x + 0.08, y + 0.35, C.MEC_D); arrow(x, y, x + 0.9, y, C.CTL_D); tag("m a", x + 0.4, y - 0.32, C.CTL_D);
      [x, y] = a1(PL.borde); s.addShape(pres.shapes.OVAL, { x: x - 0.08, y: y - 0.08, w: 0.16, h: 0.16, fill: { color: "DC2626" }, line: { color: "FFFFFF", width: 1 } }); tag("borde de vuelco", x - 0.6, y + 0.1, "B91C1C");
      [x, y] = a2(PF.tcp); arrow(x, y, x, y + 0.8, "DC2626"); tag("P", x + 0.08, y + 0.4, "B91C1C");
      [x, y] = a2(PF.torre_top); s.addShape(pres.shapes.ARC, { x: x - 0.45, y: y - 0.3, w: 0.9, h: 0.5, angleRange: [200, 340], line: { color: C.PRG_D, width: 3, endArrowType: "triangle" } }); tag("M de giro (J1)", x - 0.7, y - 0.62, C.PRG_D);
    } else {
      glass(s, M, 1.42, 8.9, 3.0, { strong: true });
      img(s, DIA + "F_vuelco_esquema_v001.png", M + 0.05, 1.45, 8.8, 2.95);
    }
    glass(s, 9.75, 1.42, 2.9, 3.0, { tint: C.MEC });
    img(s, FOT + "FOTO-29_estabilidad_planta_CG.png", 9.85, 1.5, 2.7, 2.85);
    const EQ = [["EQ_centro_masa.png", M, 4.0], ["EQ_inercia.png", M + 4.05, 4.0], ["EQ_dalembert.png", M + 8.1, 3.85]];
    EQ.forEach(([f, x, w]) => { glass(s, x, 4.55, w - 0.1, 1.62, { tint: C.MEC }); img(s, DIA + f, x + 0.03, 4.56, w - 0.16, 1.6); });
    refs(s, ["HBD", "EST"]);
  },
  guion: "Cuando el brazo se estira, el centro de masa del conjunto se desplaza. Sobre el auto actúan el peso propio en el centro de masa, la carga en la pinza, la compresión que baja por la torre y, si frenamos, una fuerza de inercia equivalente hacia adelante. Si el brazo gira hacia el costado, aparece además un momento de giro sobre la base. Mientras el momento del peso respecto del borde de apoyo sea mayor que el de la carga y la inercia, el auto no vuelca. Abajo: el centro de masa es el promedio de las posiciones pesado por la masa; el momento de inercia mide cuánto cuesta hacerlo girar; y la fuerza de inercia es menos masa por aceleración.",
  fuentes: "Hibbeler, Dinámica (centro de masa, momento de inercia, D'Alembert); CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v13/v14 README (estabilidad condicionada a contactos hipotéticos); FOTO-29; renders lateral y frontal del auto con brazo (PC de escritorio, con JSON de puntos tcp, torre_top, cg, borde).",
  pend: "Contactos reales y masa medida del auto pendientes: no declarar vuelco ni margen numérico. El esquema es didáctico hasta que lleguen los renders con brazo." });

// 19 · Despiece del auto
function despiece(s, items, cols) {
  const GW = (W - 2 * M - (cols - 1) * 0.15) / cols, rows = Math.ceil(items.length / cols), GH = (5.35 - (rows - 1) * 0.15) / rows;
  items.forEach(([t, p], i) => {
    const x = M + (i % cols) * (GW + 0.15), y = 1.45 + Math.floor(i / cols) * (GH + 0.15);
    glass(s, x, y, GW, GH);
    text(s, t, x + 0.05, y + 0.06, GW - 0.1, 0.42, { fontSize: 10.5, bold: true, align: "center", valign: "middle", fit: "shrink" });
    img(s, p, x + 0.1, y + 0.5, GW - 0.2, GH - 0.6, "center", "render\npendiente");
  });
}
slide({ id: "F19", ramas: ["M"], t: 30, titulo: "Despiece del auto",
  clave: "Cada pieza del ensamble, por separado y sin fondo.",
  pie: "MECÁNICA · Sistemas de Representación II (planos de conjunto y despiece)",
  build: s => despiece(s, [["Placa superior", A("renders/F19_placa_superior_v001.png")], ["Placa inferior", A("renders/F19_placa_inferior_v001.png")], ["Rueda Mecanum A/B", A("renders/F06_rueda_iso_v001.png", REN + "F05_A01_RENDER_wheel_iso_crop_v001.png")],
    ["Motor GM25-370 + soporte", A("renders/F19_motor_soporte_v001.png", REN + "F14_A01_RENDER_motor_gm25_crop_v001.png")], ["Casquillo y prisionero", A("renders/F19_casquillo_v001.png")], ["Portabaterías + 2 × 18650", A("renders/F04_bateria_superior_v001.png")],
    ["Separadores y fijaciones", A("renders/F19_separadores_v001.png")], ["Arduino Mega 2560", A("renders/F05_mega2560_iso_v001.png")], ["Motor Driver Model Y V2", A("renders/F04_driver_superior_v001.png")],
    ["WiFi Shield", A("renders/F05_wifi_shield_iso_v001.png")], ["HC-SR04 + soporte", A("renders/F04_hcsr04_frente_v001.png")], ["Servo SG90", A("renders/F04_servo_sg90_iso_v001.png", REN + "F18_A01_RENDER_servo_sg90_crop_v001.png")],
    ["Sensor de línea 5 IR", A("renders/F04_sensor_linea_inferior_v001.png")], ["Panel OLED + encoder", A("renders/F04_oled_encoder_superior_v001.png")]], 7),
  guion: "Antes de pasar a la electrónica, el auto desarmado: cada pieza del ensamble por separado. Arriba de cada una, su nombre. Es la misma información que tiene el ensamble de Fusion, pero ordenada como un despiece.",
  fuentes: "Renders por objeto del GLB/colección detallada (PC de escritorio, assets_claro/renders/F19_*).",
  pend: "Renders del despiece pendientes en la PC de escritorio." });

// 20 · Despiece del brazo
slide({ id: "F20", ramas: ["M"], t: 25, titulo: "Despiece del brazo",
  clave: "Estructura impresa, transmisiones, apoyos y actuadores.",
  pie: "MECÁNICA · Sistemas de Representación II · Máquinas y Elementos de Máquinas",
  build: s => despiece(s, [["Montura y base J1", A("renders/F20_base_j1_v001.png")], ["Torre J1–J2", A("renders/F20_torre_v001.png")], ["Hombro J2 · engranajes 18/72", A("renders/F20_hombro_j2_v001.png")],
    ["Eslabón 1 (PVC)", A("renders/F20_eslabon1_v001.png")], ["Codo J3 · correa y poleas", A("renders/F20_codo_j3_v001.png")], ["Eslabón 2 (PVC)", A("renders/F20_eslabon2_v001.png")],
    ["Muñeca J4–J5", A("renders/F20_muneca_v001.png")], ["Pinza", A("renders/F20_pinza_v001.png")]], 4),
  guion: "Y el brazo desarmado por conjuntos: la base con el giro, la torre, el hombro con sus engranajes, los dos eslabones de PVC, el codo con la correa, la muñeca y la pinza.",
  fuentes: "AFR2_Brazo_v17_Base_V7_J3_Remoto.f3d (PC de escritorio, assets_claro/renders/F20_*); revision_v17/laminas_20261109 (seis conjuntos).",
  pend: "Renders del despiece del brazo pendientes en la PC de escritorio." });

// F11
slide({ id: "v3-F11", ramas: ["E"], t: 40, titulo: "Del datasheet al circuito",
  clave: "El datasheet convierte un componente en restricciones de diseño.",
  pie: "ELECTRÓNICA · Electrónica Básica · Laboratorio de Mediciones · Microcontroladores",
  build: s => {
    framed(s, FOT + "FOTO-12_shield_osoyoo_pinout.jpg", 0.3, 1.5, 6.0, 5.3);
    img(s, CAP + "sensorbateria_crop.png", 6.5, 1.5, 6.3, 4.4);
    text(s, "Esquemático real exportado de KiCad (sensorbateria.kicad_sch)", 6.5, 6.0, 6.3, 0.4, { fontSize: 13, color: C.T2, align: "center" });
  },
  guion: "Antes de conectar algo leemos su hoja de datos: qué tensión tolera cada pin, qué corriente entrega, qué nivel lógico espera. Por ejemplo, el ADC del ATmega no acepta más tensión que su referencia de cinco volts, y eso define el circuito que vamos a ver para medir la batería. A la izquierda, el pinout del shield; a la derecha, el esquemático real tal como está en KiCad.",
  fuentes: "fotos_existentes/FOTO-12; AutoFinalRobo2-CAD/kicad/oled-encoder/sensorbateria.kicad_sch exportado con kicad-cli 10.99 (capturas/sensorbateria.png).",
  pend: "F11_A01: captura del datasheet oficial (pinout + límites) no tomada; guardar URL y revisión del documento." });

// F13
slide({ id: "v3-F13", ramas: ["E"], t: 40, titulo: "Batería y ADC",
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
    bullets(s, ["V_A5: tensión en el pin del ADC · V_B: tensión de la batería", "R₁ = R₂ = 10 kΩ: dividen por 2 y el pack queda dentro de los 5 V", "raw: lectura del ADC (0 a 1023) · mV: tensión calculada"], 8.5, 5.3, 4.4, 0.95, 10.5, C.T2);
    refs(s, ["MC", "SEN"]);
  },
  guion: "Las dos celdas 18650 en serie dan más de cinco volts, y el ADC del ATmega no puede medir más que su referencia. La solución es un divisor con dos resistencias iguales de diez kilo: en el punto medio tenemos exactamente la mitad, con un capacitor de cien nanofaradios para filtrar. El ADC de diez bits convierte eso en un número de cero a mil veintitrés; como dividimos por dos, el firmware multiplica por diez mil en vez de cinco mil y divide por mil veinticuatro. Dos detalles: es una tensión, no un porcentaje de carga, y todavía no está calibrada.",
  fuentes: "contracts/BATTERY_MEASUREMENT_9.md (10 kΩ+10 kΩ, 100 nF, A5/PF5/ADC5, fórmula, prescaler 128, 8 muestras); afr2_sensors.c:30; kicad sensorbateria.kicad_sch.",
  pend: "Montaje y calibración no comprobados (según contrato). Rango 2×4,2 V es DERIVADO de la química nominal, no medido." });

// F14
slide({ id: "v3-F14", ramas: ["E", "C"], t: 45, titulo: "Encoders Hall y cuadratura",
  clave: "Los pulsos convierten movimiento físico en información medible.",
  pie: "ELECTRÓNICA → CONTROL · Electrónica Digital · Microcontroladores · Sistemas Embebidos",
  build: s => {
    img(s, A("renders/F04_motor_gm25_iso_v001.png", REN + "F14_A01_RENDER_motor_gm25_crop_v001.png"), 0.3, 1.5, 3.8, 2.8);
    img(s, DIA + "F14_A02_VECTOR_cuadratura_hall_v001.png", 4.2, 1.45, 8.7, 3.25);
    code(s, "Atmega/src/platform/afr2_board.c:40 · ISR(PCINT2_vect)", [
      "if(rising&_BV(AFR2_HALL_FL_A_BIT))",
      "  hall_counts[0] += (k&_BV(AFR2_HALL_FL_B_BIT)) ? -1 : +1;"], 0.3, 4.75, 7.6, 1.5, 13);
    eq(s, "ω ≈ 2π·ΔN / (Nrev·Δt)", 8.1, 4.8, 4.8, 0.6, 24);
    bullets(s, ["ω: velocidad angular de la rueda (rad/s)", "ΔN: pulsos contados en el intervalo Δt", "Nrev = 370 pulsos/vuelta, medido a mano, igual en las 4 ruedas"], 8.2, 5.4, 4.7, 0.85, 10.5, C.T2);
  },
  guion: "Cada motor GM25-370 tiene un encoder Hall con dos canales, A y B, desfasados un cuarto de ciclo. El firmware cuenta solo los flancos de subida de A, con una interrupción por cambio de pin, y mira en ese instante el nivel de B: si está alto resta, si está bajo suma. Así sabemos cuánto giró y hacia dónde. Contando pulsos en un intervalo de tiempo obtenemos la velocidad angular. La resolución de trescientos setenta pulsos por vuelta la medimos girando la rueda a mano.",
  fuentes: "afr2_board.c:40-47 (hall_capture, ISR PCINT2, A8–A15); include/afr2_product.h:17 (AFR2_DEFAULT_CPR 370, medido por el usuario); contracts/hardware/PINOUT.md.",
  pend: "370 es DECLARADO (medición manual del usuario): mostrarlo como tal. El código real usa suma en u32 con UINT32_MAX para restar; el snippet lo simplifica a ±1 (indicarlo si se pregunta)." });

// F15
slide({ id: "v3-F15", ramas: ["E"], t: 45, titulo: "Ultrasonido: tiempo de vuelo",
  clave: "Medimos distancia midiendo el tiempo de ida y vuelta del sonido.",
  pie: "ELECTRÓNICA · Física III (ondas) · Microcontroladores · Laboratorio de Mediciones",
  build: s => {
    framed(s, A("renders/F_us_tof_v001.png", REN + "F15_A01_RENDER_us_tof_v001.png"), 0.3, 1.5, 6.0, 3.4);
    img(s, DIA + "F15_A02_VECTOR_tof_tiempos_v001.png", 6.4, 1.45, 6.5, 2.45);
    code(s, "Atmega/src/platform/afr2_sensors.c:136 · ISR(TIMER3_COMPA_vect)", [
      "++echo_ticks;                         // un tick = 25 µs",
      "if(echo_state==2 && (PINC&_BV(PC6))) { echo_rise=echo_ticks; echo_state=3; }",
      "else if(echo_state==3 && !(PINC&_BV(PC6))) {",
      "  echo_width=echo_ticks-echo_rise; echo_done=1; }",
      "if(echo_ticks>=1280u && echo_state)  // 32 ms: timeout sin bloquear",
      "  echo_error = echo_state==3 ? AFR2_ECHO_NO_FALL : AFR2_ECHO_NO_RISE;"], 0.3, 5.0, 12.6, 1.8, 12);
    bullets(s, ["d: distancia al obstáculo · c: velocidad del sonido (≈ 343 m/s en aire a 20 °C)", "Δt: ancho del pulso de eco; se divide por 2 porque el sonido va y vuelve", "El obstáculo del render es geometría de escena, no parte del robot"], 6.5, 3.95, 6.3, 1.0, 10.5, C.T2);
  },
  guion: "El HC-SR04 emite un pulso de ultrasonido y devuelve por el pin de eco un pulso cuyo ancho es el tiempo que tardó el sonido en ir y volver. La distancia es la velocidad del sonido por ese tiempo, dividido dos. En el firmware no esperamos con un bucle: un temporizador interrumpe cada veinticinco microsegundos, detecta el flanco de subida y el de bajada del eco, y si en treinta y dos milisegundos no llegó, informa el error sin trabar el resto del programa. El sensor va sobre un servo, y con eso armamos el radar.",
  fuentes: "afr2_sensors.c:119-148 (Timer3 CTC, OCR3A=49, ÷8 → 25 µs; timeout 1280 ticks; TRIG PC7/D30, ECHO PC6/D31); contracts/SENSORS_RADAR_8.md.",
  pend: "Temperatura/velocidad del sonido asumida en la conversión: verificar en el código de conversión antes de mostrar un valor de c." });

// F16
slide({ id: "v3-F16", ramas: ["E", "C"], t: 45, titulo: "Sensores de línea",
  clave: "La línea se detecta como una diferencia de reflectancia.",
  pie: "ELECTRÓNICA → CONTROL · Física III (óptica) · Sistemas de Control",
  build: s => {
    framed(s, A("renders/F_linea_baja_v001.png", REN + "F16_A01_RENDER_line_low_v001.png"), 0.3, 1.5, 6.0, 3.4);
    img(s, DIA + "F16_A02_VECTOR_linea_pesos_v001.png", 6.4, 1.45, 6.5, 2.45);
    code(s, "Atmega/src/services/afr2_product.c:383", [
      "int16_t error=0,n=0;",
      "for(uint8_t i=0;i<5;i++) if(p->sensors.line_bits&(1u<<i)){ error+=2-(int16_t)i; n++; }",
      "wz=(int16_t)((int32_t)error*p->config.line_kp/n);   // giro proporcional",
      "drive_closed_loop(p, p->config.line_speed, 0, wz, 500);"], 0.3, 5.0, 12.6, 1.6, 12);
    bullets(s, ["e: cuánto está corrida la línea respecto del centro · i: sensor 0 a 4, con peso 2 − i", "n: sensores que ven la cinta · Kp,línea: ganancia · ωz: giro pedido al chasis", "IR1 (bit 0, A4) a la izquierda: línea a la izquierda ⇒ giro antihorario"], 6.5, 3.95, 6.3, 1.0, 10.5, C.T2);
  },
  guion: "Cinco emisores y receptores infrarrojos miran el piso. La cinta oscura refleja menos que el piso claro, y cada sensor entrega un bit. El firmware le asigna a cada sensor un peso, de más dos a menos dos, promedia los que ven cinta y obtiene un error: cuánto está la línea corrida respecto del centro. Ese error multiplicado por una ganancia es el giro que se le pide al auto, mientras avanza a velocidad constante. Si deja de ver la línea, se detiene.",
  fuentes: "afr2_product.c:370-388 (pesos 2−i, line_kp, parada sin línea y por cruce ≥150 ms); line_speed 80 y line_kp 100 por defecto (afr2_product.c:27).",
  pend: "Polaridad analógica (blanco = ADC alto) no verificada: el modo mostrado es digital. F16_A03 sin lecturas reales: no mostrar números." });

// F17
slide({ id: "v3-F17", ramas: ["E", "P"], t: 35, titulo: "I²C y pantalla OLED",
  clave: "Un bus sincroniza datos y reloj para comunicar periféricos.",
  pie: "ELECTRÓNICA → PROGRAMACIÓN · Sistemas Embebidos (USART, I²C, SPI, GPIO, ADC, timers)",
  build: s => {
    img(s, DIA + "F17_A02_VECTOR_i2c_oled_v001.png", 0.2, 1.45, 8.4, 3.2);
    framed(s, FOT + "FOTO-53_autito_silueta_OLED_128x64.png", 8.8, 1.6, 4.0, 2.9);
    bullets(s, ["Framebuffer de 1024 bytes (128 × 64 / 8) y una copia para enviar solo lo que cambió", "Encoder KY-040: CLK D2 / DT D3 por interrupción, SW D42 · pull-ups 10 kΩ a 3V3 en la PCB", "Pulsación corta confirma · larga (≥ 800 ms, al soltar) abre controles rápidos"], 0.5, 4.85, 12.3, 1.9, 16);
  },
  guion: "La pantalla habla I²C: dos cables, uno de datos y uno de reloj, que el maestro, el ATmega, genera a cien kilohertz. Como es el único dispositivo del bus, un administrador de I²C es el dueño de las transferencias y nunca se escribe desde una interrupción. La imagen vive en un buffer de mil veinticuatro bytes, uno por cada ocho píxeles, y se manda por bloques. La perilla es un encoder rotativo leído por interrupciones: corto confirma, largo abre los controles rápidos.",
  fuentes: "afr2_twi.c:42 (TWBR=72 → 100 kHz); afr2_oled.c (SSD1306, 128×64 páginas); builds/2026-11-06-04 (buffer 1024 + sombra ACK 1024); AGENTS.md (navegación); afr2_board.c (INT4/INT5).",
  pend: "Dirección I²C y tasa real de actualización (F17_A03) no medidas: no mostrar FPS ni dirección. El esquemático alimenta el panel a 3V3 y anota «adaptación de niveles I²C con Mega 5 V: pendiente externa»; la ficha de pinout reporta el panel a 5 V. Resolver antes de presentar la tensión del bus." });

// F18
slide({ id: "v3-F18", ramas: ["E", "C"], t: 40, titulo: "Motor DC y servo: dos usos de señales temporizadas",
  clave: "Se parecen eléctricamente; no significan lo mismo.",
  pie: "ELECTRÓNICA → CONTROL · Microcontroladores · Sistemas Embebidos · Máquinas eléctricas",
  build: s => {
    img(s, A("renders/F04_motor_gm25_iso_v001.png", REN + "F14_A01_RENDER_motor_gm25_crop_v001.png"), 0.3, 1.55, 2.9, 2.2);
    img(s, A("renders/F04_servo_sg90_iso_v001.png", REN + "F18_A01_RENDER_servo_sg90_crop_v001.png"), 0.3, 3.95, 2.9, 2.6);
    img(s, DIA + "F18_A02_VECTOR_pwm_vs_servo_v001.png", 3.3, 1.5, 9.6, 3.6);
    bullets(s, ["Motor: el duty fija la tensión media → velocidad", "Servo: el ancho del pulso (1–2 ms cada 20 ms) fija el ángulo"], 3.6, 5.3, 9.2, 1.3, 17);
  },
  guion: "El motor y el servo reciben pulsos, pero leen cosas distintas. Al motor le llega un PWM de casi mil hertz: lo que importa es qué fracción del período está encendido, porque eso fija la tensión media y por lo tanto la velocidad. Al servo le llega un pulso cada veinte milisegundos, y lo que importa es el ancho: entre uno y dos milisegundos, que el servo traduce en un ángulo de cero a ciento ochenta grados.",
  fuentes: "afr2_motor.c:21-23 (Fast PWM 8 bits, ÷64 → 976 Hz, Timer1/Timer2; OCR por rueda); afr2_sensors.c:149-170 (Timer5, 0,5 µs, 40000 ticks = 20 ms, 1000–2000 µs, PB7/D13).",
  pend: "Frecuencia PWM 976 Hz es la configurada en el timer (comentario: «candidate»), no medida con osciloscopio." });

// F19
slide({ id: "v3-F19", ramas: ["C"], t: 45, titulo: "Lazo abierto y lazo cerrado",
  clave: "Realimentar es comparar lo pedido con lo medido.",
  pie: "CONTROL · Sistemas y Señales · Sistemas de Control (modelado, controladores, sintonización)",
  build: s => {
    img(s, DIA + "F19_A01_VECTOR_lazo_abierto_cerrado_v001.png", 0.3, 1.45, 9.1, 4.3);
    eq(s, "e(t) = r(t) − y(t)", 9.5, 2.1, 3.4, 0.7, 26, C.CTL);
    bullets(s, ["e(t): error, la diferencia que corrige el controlador", "r(t): referencia, la velocidad pedida", "y(t): salida medida con el encoder"], 9.5, 2.9, 3.4, 1.5, 11.5, C.T2);
    text(s, "El auto tiene ambos: PWM manual y prueba de rueda en lazo abierto; Manual, Línea y GOTO en lazo cerrado.", 9.5, 4.5, 3.4, 1.4, { fontSize: 13, color: C.TX });
    refs(s, ["OG"]);
  },
  guion: "En lazo abierto mandamos un PWM y confiamos en que el motor gire a la velocidad esperada. Pero cada motor es distinto, la batería baja y el piso cambia. En lazo cerrado medimos la velocidad con el encoder, la comparamos con la pedida y corregimos con esa diferencia, el error. El auto tiene los dos: el modo PWM manual es lazo abierto, y los modos de conducción, línea y GOTO cierran el lazo.",
  fuentes: "afr2_product.c:420-450 (PID solo si modo ≠ WHEEL_TEST y ≠ PWM_MANUAL); afr2_product.c:428-431." });

// F20
slide({ id: "v3-F20", ramas: ["C", "P"], t: 55, titulo: "El controlador se convierte en firmware",
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
slide({ id: "v3-F21", ramas: ["C"], t: 30, titulo: "Motores diferentes, sistema real",
  clave: "El control corrige diferencias; no elimina límites físicos.",
  pie: "CONTROL · Sistemas de Control · Máquinas eléctricas · Laboratorio de Mediciones",
  build: s => {
    framed(s, A("renders/F_lateral_ortho_v001.png", REN + "F09_A01_RENDER_side_ortho_v001.png"), 0.3, 1.5, 6.4, 3.6);
    bullets(s, ["motor_trim por rueda: 500–1500 ‰ corrige diferencias de ganancia", "Factores de escala por dirección en la odometría", "Si una rueda con PWM no cambia su Hall en 500 ms → falla y STOP", "Signo PWM/Hall cruzado = realimentación positiva → STOP"], 6.9, 1.6, 6.0, 3.6, 16);
    pending(s, "F21_A02 · PWM y velocidad de las 4 ruedas con la misma consigna: ensayo pendiente", 0.3, 5.5, 12.6, 0.7);
  },
  guion: "Cuatro motores iguales en el catálogo no son iguales en la realidad. Por eso cada rueda tiene un factor de corrección y la odometría tiene factores por dirección. Pero el control tiene límites: si el PWM llega al máximo, no hay más corrección posible. Y el firmware se protege: si una rueda recibe potencia y su encoder no se mueve, o si el signo del encoder quedó invertido, lo que generaría una realimentación positiva, se detiene todo.",
  fuentes: "afr2_product.c:51 (motor_trim 500–1500), :297 (odom_scale por dirección, AFR2-20261111-06), :446 (fault 3 sin cambio Hall 500 ms), :424 (AFR2_FAULT_SIGN_MISMATCH).",
  pend: "No concluir «motor defectuoso» sin ensayo." });

// F23
slide({ id: "v3-F23", ramas: ["P"], t: 30, titulo: "C, Arduino Core y abstracción",
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
slide({ id: "v3-F24", ramas: ["P"], t: 35, titulo: "Wi-Fi, HTTP y carga de la aplicación",
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
slide({ id: "v3-F25", ramas: ["P"], t: 45, titulo: "El modelo OSI como mapa conceptual",
  clave: "Las capas separan responsabilidades de comunicación.",
  pie: "PROGRAMACIÓN · Sistemas Operativos y Redes de Comunicación (ISO/OSI, TCP/IP, protocolos)",
  build: s => img(s, DIA + "F25_A01_VECTOR_osi_didactico_v001.png", 0.6, 1.45, 12.1, 5.4),
  guion: "El modelo OSI nos sirve como mapa. Arriba, en aplicación, están HTTP y WebSocket y nuestros mensajes. Los datos viajan como JSON, texto. Debajo está TCP, que garantiza que lleguen en orden, después IP y el Wi-Fi. Ojo con dos cosas: es una asociación didáctica, no una implementación capa por capa, y el identificador de pedido que usamos para correlacionar respuestas no es la capa de sesión. A la derecha, el enlace entre el ESP y el ATmega ni siquiera es TCP/IP: es una UART con nuestra propia trama.",
  fuentes: "ESP/platformio.ini (ESPAsyncTCP, ESPAsyncWebServer); contracts/PROTOCOL.md (afr2-ws-1, requestId); UART_PROFILE_2.md." });

// F26
slide({ id: "v3-F26", ramas: ["P", "E"], t: 40, titulo: "React ↔ ESP ↔ ATmega",
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
slide({ id: "v3-F27", ramas: ["P"], t: 55, titulo: "Trama serial, parser y tabla de comandos",
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
slide({ id: "v3-F28", ramas: ["P"], t: 40, titulo: "Eventos, estado y notificaciones",
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
slide({ id: "v3-F29", ramas: ["P", "M"], t: 40, titulo: "Dashboard y modelo 3D",
  clave: "El sistema físico también tiene una representación digital.",
  pie: "PROGRAMACIÓN → MECÁNICA · Sistemas de Representación II · Computación II",
  build: s => {
    const WEB = CAP + "web/F29_A01_CAPTURE_web_";
    framed(s, WEB + "inicio_v001.png", 0.3, 1.5, 6.1, 3.2);
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
slide({ id: "v3-F30", ramas: ["P"], t: 50, titulo: "Flash, LittleFS, RAM y recursos 3D",
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
slide({ id: "v3-F31", ramas: ["P"], t: 45, titulo: "Sesiones, concurrencia y latencia",
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
slide({ id: "v3-F32", ramas: ["M", "E", "C", "P"], t: 75, titulo: "Workflow organizado e IA agéntica",
  clave: "La IA no hace ingeniería por nosotros: un workflow organizado nos devuelve tiempo para integrar, revisar y validar.",
  pie: "Proyecto de Ingeniería Mecatrónica (planificación, requerimientos, especificación, diseño, testeo y validación)",
  build: s => {
    img(s, DIA + "F32_A01_VECTOR_monorepo_contratos_v001.png", 0.2, 1.75, 6.6, 3.7);
    framed(s, FOT + "FOTO-54_loader_operacion.png", 7.0, 1.8, 5.9, 3.4);
    img(s, DIA + "F32_A02_VECTOR_caso_bateria_v001.png", 0.3, 5.5, 12.7, 0.95);
    text(s, "Estudiar → diseñar → implementar → probar → verificar → documentar", 0.4, 6.5, 12.5, 0.4, { fontSize: 17, bold: true, color: C.T2, align: "center" });
  },
  guion: "Organizar el proyecto no hace el trabajo solo. Lo que hace es partir un sistema complejo en módulos con responsabilidades y contratos claros: firmware del ATmega, firmware del ESP, la Web, el CAD, el Loader para cargar los firmwares, y en el centro Contracts, donde se documenta cada cambio compartido con un identificador y el estado real de cada módulo. Todo vive en un solo repositorio con una única versión de producto. Así, el caso de la batería atraviesa hardware, firmware, protocolo y React, y cada eslabón sabe con qué archivo se conecta. Una herramienta de IA puede ayudarnos a buscar una función, preparar una prueba o implementar un componente; nosotros seguimos definiendo requisitos, arquitectura, seguridad, límites físicos y criterios de aceptación. Esa reducción del trabajo repetitivo nos permite mirar más la vista macro.",
  fuentes: "AGENTS.md raíz (seis módulos, un solo Git, versión única en afr2_build_info.h y package.json); Contracts/docs/ESTRUCTURA_Y_GIT.md, VERSIONADO.md; Loader 2.5.0 (Loader/src/JobState.cs:62); fotos_existentes/FOTO-54.",
  pend: "No atribuir al Loader capacidades MCP/IA no demostradas en la versión instalada (2.5.0 en fuente; distribución en raíz 2.0.0)." });

// Cierre
slide({ id: "F33", ramas: ["M", "E", "C", "P"], hero: true, t: 45, titulo: "Cuatro ramas, un sistema",
  pie: "Proyecto final de Robótica II · Ingeniería Mecatrónica · AutoFinalRobo2",
  build: s => {
    const hero = A("renders/F_cierre_auto_hero_v001.png", REN + "F33_A01_RENDER_hero_final_crop_v001.png");
    if (isNew(hero)) img(s, hero, 3.4, 1.75, 6.5, 4.3);
    else { glass(s, 3.55, 1.85, 6.2, 4.1, { strong: true, r: 0.22 }); img(s, hero, 3.65, 1.95, 6.0, 3.9); }
    text(s, "La mecatrónica no consiste en sumar componentes.", 0.5, 0.45, 12.3, 0.6, { fontFace: HEAD, fontSize: 28, bold: true, align: "center" });
    text(s, "Consiste en integrar conocimientos para resolver problemas reales.", 0.5, 1.05, 12.3, 0.5, { fontSize: 19, color: C.T2, align: "center" });
    [["MECÁNICA", "CAD · ruedas Mecanum · cinemática", C.MEC, C.MEC_D, 0.5, 2.2], ["ELECTRÓNICA", "divisor · Hall · ultrasonido · I²C", C.ELE, C.ELE_D, 0.5, 4.3],
     ["CONTROL", "lazo cerrado PI · seguidor de línea", C.CTL, C.CTL_D, 9.9, 2.2], ["PROGRAMACIÓN", "C · Arduino · React · protocolo", C.PRG, C.PRG_D, 9.9, 4.3]].forEach(([a, b, c, cd, x, y]) => {
      glass(s, x, y, 2.95, 1.35, { tint: c });
      text(s, a, x, y + 0.18, 2.95, 0.4, { fontFace: HEAD, fontSize: 17, bold: true, color: cd, align: "center" });
      text(s, b, x + 0.1, y + 0.65, 2.75, 0.55, { fontSize: 12, color: C.T2, align: "center" });
    });
    text(s, "¡Gracias!", 0.5, 6.25, 12.3, 0.55, { fontFace: HEAD, fontSize: 26, bold: true, align: "center" });
  },
  guion: "Empezamos diciendo que no íbamos a recorrer materias sino un sistema. Vimos que la mecánica define cómo se mueve y cuánto aguanta, la electrónica cómo mide y actúa, el control cómo corrige lo que pasa en la realidad, y la programación cómo todo eso se coordina y se comunica. Ninguna de las cuatro alcanza sola: la mecatrónica no consiste en sumar componentes, sino en integrar conocimientos para resolver problemas reales. Muchas gracias.",
  fuentes: "Render de cierre desde el mismo GLB que la portada.",
  pend: "Agregar QR y link al repositorio si se decide publicarlo (idea del equipo)." });

// ---------- Referencias (todas, agrupadas)
slide({ id: "REF", ramas: ["M", "E", "C", "P"], t: 0, titulo: "Referencias",
  clave: "Bibliografía y archivos del proyecto citados en las filminas, con su número.",
  pie: "Los números [n] son los mismos que aparecen en cada filmina",
  build: s => {
    const cats = ["Robótica", "Mecánica", "Control", "Electrónica", "Proyecto"];
    const colX = [M, 6.75], CW = 5.9;
    let col = 0, y = 1.5;
    cats.forEach(c => {
      const rs = REF.map((r, i) => [i + 1, r]).filter(([, r]) => r[1] === c);
      const h = 0.42 + rs.length * 0.43;
      if (y + h > 6.8) { col = 1; y = 1.5; }
      glass(s, colX[col], y, CW, h, { tint: { Robótica: C.CTL, Mecánica: C.MEC, Control: C.CTL, Electrónica: C.ELE, Proyecto: C.PRG }[c] });
      text(s, c, colX[col] + 0.18, y + 0.06, CW - 0.36, 0.3, { fontFace: HEAD, fontSize: 13, bold: true });
      s.addText(rs.map(([n, r]) => ({ text: `[${n}]  ${r[2]}`, options: { bullet: { indent: 12 }, paraSpaceAfter: 3 } })),
        { x: colX[col] + 0.18, y: y + 0.38, w: CW - 0.36, h: h - 0.44, fontFace: FONT, fontSize: 9.5, color: C.TX, margin: 0, valign: "top", isTextBox: true, fit: "shrink" });
      y += h + 0.15;
    });
  },
  guion: "Respaldo: la bibliografía y los archivos del proyecto que citamos en cada filmina.",
  fuentes: "Catálogo REF del generador." });

// ---------- Anexos (respaldo para preguntas; fuera de los 25 min)
slide({ id: "A1", ramas: ["E", "P"], t: 0, titulo: "Anexo · KEY y RST automáticos con NPN",
  clave: "Dos transistores reemplazan los pulsadores para poner al ESP en modo de carga.",
  pie: "ANEXO · Electrónica Básica (transistor como llave) · Microcontroladores",
  build: s => {
    framed(s, FOT + "FOTO-61_shield_vista_superior_KEY_RST.jpg", 0.3, 1.5, 6.2, 5.2);
    bullets(s, ["NPN en paralelo con cada pulsador del shield, base desde D32/D33 del Mega por 1 kΩ", "10 kΩ base–emisor: apagado aunque el Mega no tenga programa", "Sin los cables de control el shield queda como de fábrica", "Implementado en firmware y Loader; circuito sin medir"], 6.8, 1.6, 6.1, 5.0, 16);
  },
  guion: "Respaldo para preguntas. El ESP entra en modo de carga bajando GPIO0 mientras se reinicia. En lugar de apretar dos botones, dos transistores NPN en paralelo con los pulsadores lo hacen comandados por el Mega.",
  fuentes: "presentacion/Filmina_12_Agregado_NPN_KEY_RST.md; Contracts/hardware/PLAN_KEY_RST_AUTOMATICO.md; FOTO-61.",
  pend: "Circuito en armado, sin mediciones." });
slide({ id: "A2", ramas: ["P"], t: 0, titulo: "Anexo · AFR2 Loader",
  clave: "Compilar, cargar y verificar los dos firmwares sin pasos manuales propensos a error.",
  pie: "ANEXO · Computación II · Sistemas Embebidos · Proyecto",
  build: s => {
    framed(s, FOT + "FOTO-55_loader_flasher.png", 0.3, 1.5, 6.2, 3.6);
    framed(s, FOT + "FOTO-56_loader_monitor_serie.png", 6.7, 1.5, 6.2, 3.6);
    bullets(s, ["Puente temporal en el Mega para programar el ESP por USB", "Verificación por hash en la misma sesión · backup opcional", "Monitor serie con intérprete del catálogo de comandos"], 0.5, 5.3, 12.3, 1.5, 16);
  },
  guion: "Respaldo para preguntas: el Loader elige puerto y destino, compila, instala el puente en el Mega para cargar el ESP, verifica y trae un monitor serie que entiende nuestros comandos.",
  fuentes: "presentacion/Filmina_11_Agregado_Loader.md; AutoFinalRobo2-Loader/README.md, MANUAL.html; FOTO-55, FOTO-56." });

// ---------- salida
fs.mkdirSync(path.dirname(OUT), { recursive: true });
pres.writeFile({ fileName: OUT }).then(f => {
  console.log("PPTX", f);
  const total = GUION.reduce((a, d) => a + d.t, 0);
  let md = `# Guion hablado · AutoFinalRobo2 (presentación v4, tema claro)\n\nGenerado por \`tools/generar_presentacion_v4.js\` (mismo texto que las notas del orador). ` +
    `Tiempo de contenido: ${Math.floor(total / 60)}:${String(total % 60).padStart(2, "0")} + ~1:30 de transiciones ≈ 25 min. Los anexos no cuentan.\n\n`;
  let acc = 0;
  GUION.forEach((d, i) => {
    acc += d.t;
    md += `## ${String(i + 1).padStart(2, "0")} · ${d.id} · ${d.titulo}  \n*${d.t} s · acumulado ${Math.floor(acc / 60)}:${String(acc % 60).padStart(2, "0")} · ramas ${d.ramas.join("+")}*\n\n> ${d.guion}\n\n**Fuentes:** ${d.fuentes}\n\n${d.pend ? "**Pendiente / no afirmar:** " + d.pend + "\n\n" : ""}`;
  });
  fs.writeFileSync(V3 + "GUION_HABLADO_v4.md", md, "utf8");
  console.log("GUION", V3 + "GUION_HABLADO_v4.md", "total s", total);
});
