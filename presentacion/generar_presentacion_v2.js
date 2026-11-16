// AutoFinalRobo2 · presentación ampliada y rediseñada (v2)
const pptxgen = require("pptxgenjs");
const fs = require("fs");
const ROOT = "D:/ProyectoRobotica2/presentacion/";
const F = ROOT + "fotos_existentes/", D = ROOT + "diagramas/png/";
const OUT = process.argv[2] || "C:/Users/kobac/OneDrive/Escritorio/PresentacionRobotica2_v2.pptx";

// Paleta
const DARK = "171B26", DARK2 = "232A3A", ACC = "B81D33", ACC2 = "E0485C", INK = "1F2430", MUTE = "5A6275",
  PANEL = "F4F5F8", LINE = "DADDE5", WHITE = "FFFFFF", SOFT = "FBE9EC", BLUE = "2D6CB5", GREEN = "2A8A52", VIOLET = "6A4BB5";
const FONT = "Calibri", HEAD = "Calibri", MATH = "Cambria Math";

const pres = new pptxgen();
pres.layout = "LAYOUT_WIDE";
pres.title = "AutoFinalRobo2";
pres.author = "Koba Chajchir";
const W = 13.333, H = 7.5, M = 0.6;
let num = 0;

function notesFor(n) {
  if (!n) return "";
  const f = fs.readdirSync(ROOT).find(x => x.startsWith("Filmina_" + String(n).padStart(2, "0")));
  const t = fs.readFileSync(ROOT + f, "utf8");
  const sec = h => { const m = t.match(new RegExp("## " + h + "[^\\n]*\\n([\\s\\S]*?)(?=\\n## |$)")); return m ? m[1].trim() : ""; };
  return "GUION\n" + sec("Guion del orador").replace(/[«»]/g, "") + "\n\nFUENTES\n" + sec("Fuentes") + "\n\nADVERTENCIAS\n" + sec("Advertencias");
}

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
function fitImage(s, path, x, y, w, h, align = "center") {
  const [iw, ih] = imgSize(path), k = Math.min(w / iw, h / ih), ww = iw * k, hh = ih * k;
  const xx = align === "left" ? x : x + (w - ww) / 2;
  s.addImage({ path, x: xx, y: y + (h - hh) / 2, w: ww, h: hh });
}
const shadow = () => ({ type: "outer", color: "000000", blur: 8, offset: 2, angle: 90, opacity: 0.12 });

// ---------- tipos de filmina
function content(section, title, ficha, extraNotes) {
  num++;
  const s = pres.addSlide();
  s.background = { color: WHITE };
  s.addText(section.toUpperCase(), { x: M, y: 0.32, w: 8, h: 0.3, fontFace: FONT, fontSize: 11, bold: true, color: ACC, charSpacing: 2, margin: 0, isTextBox: true });
  s.addText(title, { x: M, y: 0.6, w: W - 2 * M, h: 0.75, fontFace: HEAD, fontSize: 26, bold: true, color: INK, margin: 0, valign: "middle", isTextBox: true });
  s.addText(`${num}`, { x: W - M - 0.6, y: 7.02, w: 0.6, h: 0.3, fontFace: FONT, fontSize: 10, color: MUTE, align: "right", margin: 0, isTextBox: true });
  s.addText("AutoFinalRobo2 · Koba Chajchir", { x: M, y: 7.02, w: 6, h: 0.3, fontFace: FONT, fontSize: 10, color: MUTE, margin: 0, isTextBox: true });
  s.addNotes((extraNotes ? extraNotes + "\n\n" : "") + notesFor(ficha));
  return s;
}
function divider(idx, title, sub, image) {
  num++;
  const s = pres.addSlide();
  s.background = { color: DARK };
  if (image) {
    s.addShape(pres.shapes.RECTANGLE, { x: 7.0, y: 0, w: 6.333, h: H, fill: { color: DARK2 }, line: { color: DARK2 } });
    fitImage(s, image, 7.2, 0.6, 5.9, 6.3);
  }
  s.addText(idx, { x: M + 0.1, y: 1.6, w: 3, h: 1.4, fontFace: HEAD, fontSize: 80, bold: true, color: ACC2, margin: 0, isTextBox: true });
  s.addText(title, { x: M + 0.1, y: 3.1, w: 6.1, h: 1.4, fontFace: HEAD, fontSize: 40, bold: true, color: WHITE, margin: 0, valign: "top", isTextBox: true });
  s.addText(sub, { x: M + 0.1, y: 4.6, w: 6.0, h: 1.6, fontFace: FONT, fontSize: 16, color: "C9CEDA", margin: 0, valign: "top", isTextBox: true });
  return s;
}
// ---------- piezas
function card(s, x, y, w, h, title, body, opt = {}) {
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: 0.08, fill: { color: opt.fill || PANEL }, line: { color: opt.fill || PANEL }, shadow: opt.shadow ? shadow() : undefined });
  let ty = y + 0.18;
  if (opt.badge) {
    s.addShape(pres.shapes.OVAL, { x: x + 0.2, y: y + 0.2, w: 0.46, h: 0.46, fill: { color: opt.badgeColor || ACC }, line: { color: opt.badgeColor || ACC } });
    s.addText(opt.badge, { x: x + 0.2, y: y + 0.2, w: 0.46, h: 0.46, fontFace: HEAD, fontSize: 13, bold: true, color: WHITE, align: "center", valign: "middle", margin: 0, isTextBox: true });
  }
  if (title) {
    s.addText(title, { x: x + (opt.badge ? 0.8 : 0.22), y: y + 0.18, w: w - (opt.badge ? 1.0 : 0.44), h: 0.5, fontFace: HEAD, fontSize: opt.titleSize || 15, bold: true, color: opt.titleColor || ACC, valign: "middle", margin: 0, isTextBox: true });
    ty = y + 0.75;
  }
  if (body) {
    const items = Array.isArray(body) ? body : [body];
    const runs = items.map((t, i) => {
      const last = i === items.length - 1;
      const o = { bullet: opt.noBullet ? false : { indent: 11 }, breakLine: !last, paraSpaceAfter: 4 };
      if (Array.isArray(t)) return [{ text: t[0] + " ", options: { ...o, bold: true, breakLine: false } }, { text: t[1], options: { breakLine: !last } }];
      return { text: t, options: o };
    }).flat();
    s.addText(runs, { x: x + 0.22, y: ty, w: w - 0.44, h: y + h - ty - 0.12, fontFace: FONT, fontSize: opt.size || 13, color: opt.color || INK, valign: "top", margin: 0, isTextBox: true });
  }
}
function formulas(s, x, y, w, h, title, lines, size = 14) {
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: 0.08, fill: { color: WHITE }, line: { color: LINE, width: 1 } });
  s.addText(title, { x: x + 0.22, y: y + 0.15, w: w - 0.44, h: 0.4, fontFace: HEAD, fontSize: 15, bold: true, color: ACC, margin: 0, isTextBox: true });
  const runs = lines.map((t, i) => {
    const note = t.startsWith("//");
    return { text: note ? t.slice(2) : t, options: { fontFace: note ? FONT : MATH, color: note ? MUTE : INK, fontSize: note ? size - 2 : size, breakLine: i < lines.length - 1, paraSpaceAfter: 6 } };
  });
  s.addText(runs, { x: x + 0.22, y: y + 0.62, w: w - 0.44, h: h - 0.74, valign: "top", margin: 0, isTextBox: true });
}
function table(s, x, y, w, rows, colW, size = 12, title, opt = {}) {
  let ty = y;
  if (title) { s.addText(title, { x, y, w, h: 0.38, fontFace: HEAD, fontSize: 15, bold: true, color: ACC, margin: 0, isTextBox: true }); ty += 0.45; }
  const b = { type: "solid", pt: 0.75, color: LINE }, none = { type: "none" };
  const tr = rows.map((r, i) => r.map((c, j) => ({
    text: String(c), options: {
      bold: i === 0 || (opt.boldFirst && j === 0), color: i === 0 ? ACC : INK, fontSize: size, fontFace: FONT,
      fill: { color: opt.hl && opt.hl.includes(i) ? SOFT : WHITE },
      border: [none, none, i === 0 ? { type: "solid", pt: 1.5, color: ACC } : b, none], valign: "middle", margin: [0.03, 0.08, 0.03, 0.08]
    }
  })));
  const sum = colW.reduce((a, c) => a + c, 0);
  s.addTable(tr, { x, y: ty, w, colW: colW.map(c => c / sum * w), rowH: opt.rowH || 0.36 });
}
function pic(s, file, x, y, w, h, caption, opt = {}) {
  const cap = caption ? 0.3 : 0;
  if (opt.bg) s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h: h - cap, rectRadius: 0.08, fill: { color: opt.bg }, line: { color: opt.bg } });
  if (opt.frame) s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h: h - cap, rectRadius: 0.06, fill: { color: WHITE }, line: { color: LINE, width: 1 }, shadow: shadow() });
  const pad = opt.frame || opt.bg ? 0.1 : 0;
  fitImage(s, file, x + pad, y + pad, w - 2 * pad, h - cap - 2 * pad);
  if (caption) s.addText(caption, { x, y: y + h - cap + 0.04, w, h: cap - 0.04, fontFace: FONT, fontSize: 11, italic: true, color: MUTE, margin: 0, isTextBox: true });
}
function stat(s, x, y, w, big, label, color = ACC) {
  s.addText(big, { x, y, w, h: 0.8, fontFace: HEAD, fontSize: 40, bold: true, color, margin: 0, valign: "bottom", isTextBox: true });
  s.addText(label, { x, y: y + 0.82, w, h: 0.6, fontFace: FONT, fontSize: 12, color: MUTE, margin: 0, valign: "top", isTextBox: true });
}
function chip(s, x, y, w, text, color = ACC) {
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h: 0.36, rectRadius: 0.18, fill: { color: WHITE, transparency: 0 }, line: { color, width: 1.25 } });
  s.addText(text, { x, y, w, h: 0.36, fontFace: FONT, fontSize: 11, bold: true, color, align: "center", valign: "middle", margin: 0, isTextBox: true });
}
function note(s, text, y = 6.55) {
  s.addText(text, { x: M, y, w: W - 2 * M, h: 0.4, fontFace: FONT, fontSize: 11, italic: true, color: MUTE, margin: 0, valign: "middle", isTextBox: true });
}
function flow(s, x, y, items, w, h, gap, colors) {
  items.forEach((t, i) => {
    const xi = x + i * (w + gap);
    s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x: xi, y, w, h, rectRadius: 0.08, fill: { color: colors[i] || DARK2 }, line: { color: colors[i] || DARK2 } });
    s.addText(t, { x: xi + 0.08, y, w: w - 0.16, h, fontFace: FONT, fontSize: 12, bold: true, color: WHITE, align: "center", valign: "middle", margin: 0, isTextBox: true });
    if (i < items.length - 1) s.addShape(pres.shapes.RIGHT_ARROW, { x: xi + w + 0.06, y: y + h / 2 - 0.13, w: gap - 0.12, h: 0.26, fill: { color: MUTE }, line: { color: MUTE } });
  });
}

// =============== 1 · PORTADA
num++;
let s = pres.addSlide();
s.background = { color: DARK };
s.addShape(pres.shapes.RECTANGLE, { x: 6.9, y: 0, w: 6.433, h: H, fill: { color: DARK2 }, line: { color: DARK2 } });
fitImage(s, F + "FOTO-01_robot_completo_CAD_v17.png", 7.0, 0.4, 6.2, 6.7);
s.addText("PROYECTO FINAL · ROBÓTICA", { x: M + 0.1, y: 1.0, w: 6, h: 0.35, fontFace: FONT, fontSize: 13, bold: true, color: ACC2, charSpacing: 3, margin: 0, isTextBox: true });
s.addText("AutoFinalRobo2", { x: M + 0.1, y: 1.45, w: 6.2, h: 1.1, fontFace: HEAD, fontSize: 54, bold: true, color: WHITE, margin: 0, isTextBox: true });
s.addText("Manipulador antropomorfo de 6 ejes sobre una plataforma Mecanum 4WD", { x: M + 0.1, y: 2.6, w: 6.0, h: 1.0, fontFace: FONT, fontSize: 20, color: "D7DBE5", margin: 0, valign: "top", isTextBox: true });
[["4", "ruedas Mecanum"], ["6", "ejes + pinza"], ["3", "firmwares propios"]].forEach((t, i) => {
  s.addText(t[0], { x: M + 0.1 + i * 1.95, y: 3.95, w: 1.8, h: 0.8, fontFace: HEAD, fontSize: 40, bold: true, color: ACC2, margin: 0, isTextBox: true });
  s.addText(t[1], { x: M + 0.1 + i * 1.95, y: 4.75, w: 1.8, h: 0.35, fontFace: FONT, fontSize: 12, color: "C9CEDA", margin: 0, isTextBox: true });
});
s.addText("Koba Chajchir", { x: M + 0.1, y: 5.65, w: 6, h: 0.4, fontFace: FONT, fontSize: 16, bold: true, color: WHITE, margin: 0, isTextBox: true });
s.addText("[Institución · asignatura · docentes · integrantes: completar]", { x: M + 0.1, y: 6.05, w: 6, h: 0.35, fontFace: FONT, fontSize: 12, color: ACC2, margin: 0, isTextBox: true });
s.addText("Render CAD v17: geometría de diseño, no foto del prototipo", { x: 7.1, y: 7.05, w: 6, h: 0.3, fontFace: FONT, fontSize: 10, italic: true, color: "8A91A3", margin: 0, isTextBox: true });
s.addNotes(notesFor(1));

// =============== 2 · AGENDA
s = content("Agenda", "Qué vamos a recorrer", null, "Presentar la estructura en 20 segundos.");
const ag = [["01", "El proyecto", "objetivo, arquitectura y módulos"], ["02", "Mecánica y hardware", "auto base, brazo v17, pinout y conexionado"],
  ["03", "Software", "firmwares, ciclo principal y protocolo"], ["04", "Matemática", "Mecanum, odometría, GOTO, brazo, conjunto y esfuerzos"],
  ["05", "Simulación e interfaz", "simulador, misión, OLED y Web"], ["06", "Nuestros agregados", "Loader, KEY/RST con NPN, encoder y OLED"]];
ag.forEach((a, i) => {
  const col = i % 3, row = Math.floor(i / 3), x = M + col * 4.1, y = 1.75 + row * 2.45;
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w: 3.85, h: 2.1, rectRadius: 0.08, fill: { color: i === 5 ? SOFT : PANEL }, line: { color: i === 5 ? SOFT : PANEL } });
  s.addText(a[0], { x: x + 0.3, y: y + 0.2, w: 2, h: 0.8, fontFace: HEAD, fontSize: 36, bold: true, color: ACC, margin: 0, isTextBox: true });
  s.addText(a[1], { x: x + 0.3, y: y + 1.0, w: 3.3, h: 0.45, fontFace: HEAD, fontSize: 18, bold: true, color: INK, margin: 0, isTextBox: true });
  s.addText(a[2], { x: x + 0.3, y: y + 1.45, w: 3.3, h: 0.55, fontFace: FONT, fontSize: 12, color: MUTE, margin: 0, valign: "top", isTextBox: true });
});

// =============== 01 PROYECTO
divider("01", "El proyecto", "Un auto Mecanum que lleva un brazo de seis ejes, comandado desde el celular o desde un panel propio.", F + "FOTO-01_robot_completo_CAD_v17.png");

s = content("01 · El proyecto", "Mover la base y el brazo como un único robot", 1);
pic(s, F + "FOTO-01_robot_completo_CAD_v17.png", M, 1.6, 5.2, 5.2, "Conjunto CAD v17 (render)", { bg: PANEL });
card(s, 6.1, 1.6, 6.63, 1.45, "Problema", "Convertir una consigna (ir a X,Y · tomar un cubo) en movimiento coordinado de base y brazo, sabiendo en qué referencia y unidad está cada número.", { noBullet: true, size: 13 });
card(s, 6.1, 3.2, 6.63, 1.45, "Objetivo", "Plataforma móvil con manipulación, documentada con modelo matemático, CAD, firmware y una interfaz reproducible.", { noBullet: true, size: 13 });
stat(s, 6.2, 4.85, 1.6, "4+6", "ruedas Mecanum y ejes del brazo");
stat(s, 7.85, 4.85, 1.6, "7", "servos STS (5 × 3250 + 2 × 3032)");
stat(s, 9.5, 4.85, 1.6, "54", "comandos del enlace ESP↔ATmega");
stat(s, 11.15, 4.85, 1.6, "101", "pantallas OLED catalogadas");

s = content("01 · El proyecto", "Arquitectura: del celular a los motores en cuatro saltos", 1);
pic(s, D + "D-01_arquitectura_general.png", M, 1.55, 8.4, 3.6, null, { frame: true });
card(s, 9.25, 1.55, 3.48, 3.6, "La cadena", [["Navegador →", "JSON afr2-ws-1"], ["ESP8266 →", "UART ROBO2 + EX01"], ["ATmega2560 →", "PWM, sentido y sensores"], ["Loader →", "carga por USB"]], { size: 13 });
flow(s, M, 5.5, ["Operador", "Web React", "ESP8266", "ATmega2560", "Motores y sensores"], 2.15, 0.8, 0.4, [BLUE, BLUE, DARK2, DARK2, VIOLET]);
note(s, "Criterio de evidencia en toda la presentación: dato nominal · geometría CAD · implementado en código · medido físicamente.");

s = content("01 · El proyecto", "Seis módulos bajo un único repositorio", 1);
const mods = [["ATmega", "C / avr-libc, sin Arduino: motores, sensores, UI local, odometría y GOTO", DARK2], ["ESP8266", "Arduino ESP8266 propio, sin AT: Wi-Fi, HTTP, WebSocket y LittleFS", DARK2],
  ["Web", "React 19 + Vite + TypeScript: control, simuladores y OLED virtual", BLUE], ["Contracts", "contratos del enlace, pinout, evidencias y versionado", GREEN],
  ["CAD", "Fusion y Blender (auto, brazo v17), KiCad (PCB del panel)", VIOLET], ["Loader", "aplicación Windows en C# que compila y carga el firmware", ACC]];
mods.forEach((m, i) => {
  const col = i % 3, row = Math.floor(i / 3), x = M + col * 4.1, y = 1.6 + row * 2.5;
  card(s, x, y, 3.85, 2.2, m[0], m[1], { noBullet: true, badge: String(i + 1), badgeColor: m[2], titleColor: INK, titleSize: 18, size: 13, shadow: true, fill: WHITE });
});
note(s, "Una sola versión de producto para ATmega, ESP y Web: 1.27.0. Cada cambio de comando actualiza wire_catalog.json, header, commands.json y Protocol Studio.");

// =============== 02 MECÁNICA
divider("02", "Mecánica y hardware", "Auto base OSOYOO Mecanum, brazo antropomorfo v17 y todo el cableado documentado pin por pin.", F + "FOTO-03_auto_base_frente_blender.png");

s = content("02 · Mecánica", "Auto base: chasis OSOYOO con cuatro ruedas Mecanum de 80 mm", 2);
pic(s, F + "FOTO-03_auto_base_frente_blender.png", M, 1.55, 6.6, 3.9, "Render Blender del auto base");
pic(s, F + "FOTO-05_auto_base_lado_derecho_fusion.png", M, 5.55, 3.2, 1.4, null);
pic(s, F + "FOTO-06_auto_base_lado_izquierdo_fusion.png", M + 3.4, 5.55, 3.2, 1.4, null);
card(s, 7.45, 1.55, 5.28, 2.9, "Especificación", ["Chasis 245 × 200 mm, rueda Mecanum Ø80 mm", "4 motores GM25-370 con encoder Hall", "ATmega2560 + shield Wi-Fi ESP-12S", "2 puentes H Model Y · batería 2 × 18650", "Movilidad plana: 3 DOF (x, y, ψ)"], { size: 13 });
pic(s, F + "FOTO-11_dimensiones_chasis.png", 7.45, 4.6, 5.28, 2.3, "Cotas del fabricante", { frame: true });

s = content("02 · Mecánica", "Brazo v17: eslabones de 70 y 60 mm sobre tubos de PVC", 2);
pic(s, F + "FOTO-07_brazo_v17_CAD.png", M, 1.5, 5.6, 5.4, "Render CAD del brazo v17", { bg: PANEL });
table(s, 6.5, 1.5, 6.23, [["Tramo", "Valor"], ["Hombro–codo (L1)", "70 mm"], ["Codo–J4", "60 mm"], ["J4–J5 / J5–J6 / J6–TCP", "55 / 65 / 78,5 mm"], ["Tubos PVC", "Ø25 × 1,2 y Ø20 × 1 mm"], ["Actuadores", "5 × STS3250 (J1–J5) + 2 × STS3032"], ["Pinza V7", "cremalleras + piñón Z24 m1 · 4–79 mm"]], [2.3, 3.2], 13, "Cotas y actuadores", { boldFirst: true });
card(s, 6.5, 4.75, 6.23, 2.15, "Transmisiones elegidas por par", [["J2:", "engranaje 18/72, i = 4"], ["J3:", "correa HP3M 20/50, i = 2,5 (motor cerca del hombro)"], ["J4:", "24/24 · J1, J5, J6 directos"]], { size: 13 });

s = content("02 · Mecánica", "Conjunto: el brazo montado sobre la plataforma", 2);
pic(s, F + "FOTO-01_robot_completo_CAD_v17.png", M, 1.5, 6.2, 5.4, null, { bg: PANEL });
pic(s, F + "FOTO-04_auto_base_atras_blender.png", 7.1, 1.5, 5.63, 2.6, "Auto base, vista posterior");
stat(s, 7.2, 4.3, 1.8, "9", "grados de libertad Q = (x, y, ψ, q1…q6)");
stat(s, 9.1, 4.3, 1.8, "4,05", "kg del conjunto (CAD, con 200 g de carga)");
stat(s, 11.0, 4.3, 1.8, "66", "piezas en el libro de planos");
card(s, 7.1, 5.8, 5.63, 1.1, null, "Montaje fijo a (0,0278; 0; 0,079) m con giro −90° sobre Z. Ejes: J1 ±90°, J2 −10…125°, J3–J6 ±90°.", { noBullet: true, size: 12 });

s = content("02 · Hardware", "Pinout: cada señal del auto tiene un pin asignado", 3);
table(s, M, 1.5, 4.0, [["Rueda", "IN1", "IN2", "PWM"], ["DEL DER", "D22", "D24", "D9"], ["DEL IZQ", "D26", "D28", "D10"], ["TRAS DER", "D5", "D6", "D11"], ["TRAS IZQ", "D7", "D8", "D12"]], [1.6, 0.8, 0.8, 0.8], 12, "Motores", { boldFirst: true });
table(s, M, 4.0, 4.0, [["Rueda", "A (S1)", "B (S2)"], ["DEL DER", "A8", "A12"], ["DEL IZQ", "A9", "A13"], ["TRAS DER", "A10", "A14"], ["TRAS IZQ", "A11", "A15"]], [1.6, 1.2, 1.2], 12, "Encoders Hall (x1)", { boldFirst: true });
table(s, 4.9, 1.5, 7.83, [["Elemento", "Pines"], ["Barra IR ×5", "A4 · A3 · A2 · A1 · A0"], ["HC-SR04 TRIG / ECHO", "D30 / D31"], ["Servo del radar", "D13"], ["Batería, divisor 10k/10k ★", "A5"], ["UART al ESP", "D18 TX1 · D19 RX1"], ["OLED I2C ★", "SDA D20 · SCL D21"], ["Encoder de menú ★", "CLK D2 · DT D3 · SW D42"], ["KEY / RST por NPN ★", "D32 / D33"], ["USB (carga y depuración)", "D0 / D1"]], [3.2, 3.6], 12, "Sensores, enlace y agregados propios (★)", { boldFirst: true, hl: [4, 6, 7, 8] });
note(s, "DEL/TRAS e IZQ/DER mirando en el sentido de avance. Hall a +5 V regulados, nunca a la batería. Orden lógico del firmware: FL, FR, RL, RR.");

s = content("02 · Hardware", "Conexionado: el Mega concentra todo y habla con el ESP por D18/D19", 3);
pic(s, D + "D-02_conexionado.png", M, 1.5, 7.8, 5.0, null, { frame: true });
pic(s, F + "FOTO-12_shield_osoyoo_pinout.jpg", 8.65, 1.5, 4.08, 3.2, "Shield OSOYOO V1.3 (fabricante)", { frame: true });
card(s, 8.65, 4.85, 4.08, 1.65, null, ["Potencia: batería → Model Y → POWER del shield", "ESP: usar E_RX/E_TX del selector", "Masa común en todo el sistema"], { size: 12 });

// =============== 03 SOFTWARE
divider("03", "Software", "Tres programas propios con responsabilidades separadas y un único contrato compartido.", D + "D-03a_ciclo_principal_ATmega.png");

s = content("03 · Software", "Tres firmwares: el ATmega manda, el ESP transporta, la Web muestra", 4);
card(s, M, 1.55, 3.95, 5.0, "ATmega2560", [["Qué es:", "C / avr-libc, sin Arduino ni DMA, un lazo cooperativo."], ["Hace:", "motores, Hall, odometría, PID, línea, GOTO, radar, batería, OLED + encoder, EEPROM."], ["Seguridad:", "salidas apagadas al arrancar, STOP prioritario, TTL 400 ms, fallos F1–F4."], ["SRAM:", "5.614 B estática."]], { size: 15, shadow: true, fill: WHITE, badge: "1", badgeColor: DARK2, titleColor: INK, titleSize: 18 });
card(s, M + 4.15, 1.55, 3.95, 5.0, "ESP8266", [["Qué es:", "Arduino ESP8266 + LittleFS, sin AT."], ["Hace:", "Wi-Fi AP/STA, HTTP :80 con la Web comprimida, WebSocket /ws, broker de 2 sesiones."], ["Puente:", "UART ROBO2 perfil 2 + EX01 v13 al Mega; no decide movimiento."], ["RAM:", "46.644 B estática."]], { size: 15, shadow: true, fill: WHITE, badge: "2", badgeColor: DARK2, titleColor: INK, titleSize: 18 });
card(s, M + 8.3, 1.55, 3.83, 5.0, "Web", [["Qué es:", "React 19 + Vite + TypeScript, servida por el ESP."], ["Vistas:", "Conducir, Programas, Puesta a punto, Sensores, Modelos, Simulación, Ayuda."], ["Cliente:", "latido 2 s, un slot para drive, STOP prioritario."], ["Contracts:", "54 comandos en wire_catalog.json."]], { size: 15, shadow: true, fill: WHITE, badge: "3", badgeColor: BLUE, titleColor: INK, titleSize: 18 });

s = content("03 · Software", "Ciclo principal del ATmega: ocho pasos que nunca bloquean", 4);
pic(s, D + "D-03a_ciclo_principal_ATmega.png", M, 1.5, 6.0, 5.4, "main.c", { frame: true });
pic(s, D + "D-03b_servicio_de_control.png", 6.75, 1.5, 5.98, 5.4, "Afr2Product_Task: control cada 10 ms", { frame: true });

s = content("03 · Software", "Protocolo: tres capas, un catálogo de comandos", 4);
flow(s, M, 1.7, ["Navegador", "WebSocket\nafr2-ws-1", "ESP8266\nbroker", "UART ROBO2\n+ EX01 v13", "ATmega2560"], 2.15, 1.0, 0.4, [BLUE, BLUE, DARK2, ACC, DARK2]);
card(s, M, 3.1, 3.95, 3.35, "Web ↔ ESP", ["JSON del mismo origen que el HTTP", "hello 5 s, alive 2 s / 6 s", "drive con un único slot reemplazable", "STOP con prioridad máxima"], { size: 13 });
card(s, M + 4.15, 3.1, 3.95, 3.35, "ESP ↔ ATmega", ["Tokens 0x3A / 0xA5, rutas 0x31 / 0x13", "carga útil de 64 B por trama", "EX01: GOTO 0x16, DRIVE 0x13, STATE 0x11…", "D18 / D19 a 115200"], { size: 13 });
card(s, M + 8.3, 3.1, 3.83, 3.35, "Contracts", ["wire_catalog.json: 54 comandos", "lo leen la Web y el intérprete del Loader", "un comando retirado no reutiliza su ID", "sensibles: los que llevan SSID o clave"], { size: 13 });

// =============== 04 MATEMÁTICA
divider("04", "Matemática", "Cinemática Mecanum, odometría, GOTO, cinemática del brazo, el conjunto y la estabilidad.", F + "FOTO-07_brazo_v17_CAD.png");

s = content("04 · Matemática del auto", "Cinemática Mecanum: tres velocidades repartidas en cuatro ruedas", 5);
formulas(s, M, 1.5, 6.3, 5.35, "Directa e inversa (X adelante, Y izquierda, ψ antihorario)", [
  "u = A · S · V ;  ω_rueda = u / r",
  "A = [ 1 −1 −L ; 1 1 L ; 1 1 −L ; 1 −1 L ]",
  "//orden FL, FR, RL, RR · r = 0,040 m · L = 0,170 m",
  "V = S · A⁺ · u",
  "A⁺ = [ ¼ ¼ ¼ ¼ ; −¼ ¼ ¼ −¼ ; −1/4L 1/4L −1/4L 1/4L ]",
  "S = diag(−1, −1, −1)",
  "γ = max(1, max|uᵢ| / u_max) ;  u = u / γ",
  "//Ejemplo: vₓ = 100 mm/s → uᵢ = 100 mm/s → 23,9 rpm por rueda"], 16);
pic(s, D + "D-11_patrones_rueda_mecanum.png", 7.05, 1.5, 5.68, 2.6, null, { frame: true });
card(s, 7.05, 4.3, 5.68, 2.55, "Lectura", ["Avance: las cuatro ruedas giran igual", "Lateral: las diagonales se oponen", "Giro: izquierda contra derecha", "S son los signos del contrato del proyecto (physicalVerification = false)"], { size: 13 });

s = content("04 · Matemática del auto", "Odometría: de pulsos Hall a pose, cada 10 ms", 5);
formulas(s, M, 1.5, 7.0, 3.75, "Cálculo", [
  "Δsᵢ = Δnᵢ · π · D / Nᵢ   //π·80/370 = 0,679 mm por cuenta",
  "Δx = ¼ΣΔsᵢ ;  Δy = ¼(−Δs₁+Δs₂+Δs₃−Δs₄)",
  "Δψ = ¼(−Δs₁+Δs₂−Δs₃+Δs₄) / L",
  "ψₘ = ψ + Δψ/2",
  "x += Δx cosψₘ − Δy sinψₘ ;  y += Δx sinψₘ + Δy cosψₘ"], 16);
pic(s, D + "D-10_pipeline_odometria.png", 7.85, 1.5, 4.88, 5.35, null, { frame: true });
s.addText("MEDIDO POR NOSOTROS", { x: M, y: 5.45, w: 6, h: 0.3, fontFace: FONT, fontSize: 11, bold: true, color: ACC, charSpacing: 2, margin: 0, isTextBox: true });
stat(s, M, 5.6, 2.2, "370", "pulsos Hall por vuelta");
stat(s, M + 2.35, 5.6, 2.2, "1000", "mm de avance = 1000 mm reales", GREEN);
stat(s, M + 4.7, 5.6, 2.3, "0,775", "factor lateral (1000 → 775 mm)");

s = content("04 · Matemática del auto", "GOTO: control proporcional sobre la pose odométrica", 6);
pic(s, D + "D-04_flujo_GOTO.png", M, 1.5, 5.6, 5.4, null, { frame: true });
formulas(s, 6.4, 1.5, 6.33, 3.6, "Ley de control", [
  "x_m = x + X cosψ − Y sinψ ;  y_m = y + X sinψ + Y cosψ",
  "eₓ = x_m − x ;  e_y = y_m − y ;  e_ψ = ψ_m − ψ",
  "vₓ = eₓ cosψ + e_y sinψ",
  "v_y = −eₓ sinψ + e_y cosψ ;  ω = e_ψ",
  "|vₓ| + |v_y| + |ω|·L ≤ v ;  mínimo 40 mm/s"], 16);
stat(s, 6.5, 5.25, 2.0, "10 mm", "tolerancia de llegada");
stat(s, 8.6, 5.25, 2.0, "25 mrad", "tolerancia de rumbo (≈1,4°)");
stat(s, 10.7, 5.25, 2.0, "60 s", "tiempo máximo por tramo");

s = content("04 · Matemática del brazo", "Brazo: cadena Z–Y–Y–Z–Y–Z con muñeca esférica", 7);
pic(s, D + "D-08_cadena_cinematica.png", M, 1.45, W - 2 * M, 2.5, null, { frame: true });
formulas(s, M, 4.1, 6.0, 2.8, "Directa", [
  "ρ = L₁ sin q₂ + L₂ sin(q₂+q₃)",
  "z_W = h + L₁ cos q₂ + L₂ cos(q₂+q₃)",
  "p_TCP = p_W + d · R · e_z",
  "//q₂ = 90°, resto 0 → TCP = (0,3285 ; 0 ; 0,209) m"], 14);
formulas(s, 6.73, 4.1, 6.0, 2.8, "Inversa", [
  "p_W = p_d − d · R_d · e_z ;  q₁ = atan2(y_W, x_W)",
  "cos q₃ = (ρ² + s² − L₁² − L₂²) / (2 L₁ L₂)",
  "q₂ = atan2(ρ, s) − atan2(L₂ sin q₃, L₁ + L₂ cos q₃)",
  "q₅ = atan2(√(r₁₃² + r₂₃²), r₃₃)"], 14);

s = content("04 · Matemática del conjunto", "Conjunto: nueve grados de libertad en una sola cadena", 7);
formulas(s, M, 1.5, 6.3, 3.0, "Pose y velocidad del TCP", [
  "Q = (x, y, ψ, q₁ … q₆)",
  "T_W,TCP = T_W,B(x,y,ψ) · T_B,M · T_M,TCP(q)",
  "V_TCP = J_base · V_base + J_brazo · q̇",
  "Jᵢ = [ zᵢ × (p_TCP − oᵢ) ; zᵢ ] ;  τ = Jᵀ [F ; M]"], 15);
formulas(s, M, 4.65, 6.3, 2.25, "Transmisión y pinza", [
  "|ω_sal| = |ω_mot| / i ;  |τ_sal| = η · i · |τ_mot|",
  "g(θ) = g₀ + 2 r_p θ ;  F ≈ η τ / (2 r_p)   //r_p = 12 mm"], 14);
pic(s, F + "FOTO-01_robot_completo_CAD_v17.png", 7.1, 1.5, 5.63, 3.6, null, { bg: PANEL });
card(s, 7.1, 5.25, 5.63, 1.65, "Singularidades a evitar", ["sin q₃ = 0: brazo estirado", "ρ = 0: J1 indeterminado · sin q₅ = 0: muñeca alineada"], { size: 13 });

s = content("04 · Esfuerzos", "Los servos tienen margen de par en la pose horizontal", 8);
table(s, M, 1.5, 5.9, [["Elemento", "Masa"], ["PLA (volumen sólido)", "788,97 g"], ["Acero (ejes, herrajes)", "566,78 g"], ["5 STS3250 + 2 STS3032", "420,70 g"], ["Aluminio, rodamientos, electrónica", "549,76 g"], ["Brazo con montura, sin carga", "2.344,99 g"], ["Carga + auto (aprox.)", "200 + 1.500 g"], ["Total del conjunto", "4.044,99 g"]], [3.6, 1.6], 13, "Masas (CAD, PLA macizo 1,24 g/cm³)", { boldFirst: true, hl: [7] });
table(s, 6.85, 1.5, 5.88, [["Eje", "Reducción", "Par en servo", "Margen"], ["J2", "4 : 1", "0,609 N·m", "2,58"], ["J3", "2,5 : 1", "0,596 N·m", "2,63"], ["J4", "1 : 1", "0,079 N·m", "19,9"], ["J5", "1 : 1", "0,447 N·m", "3,51"]], [0.8, 1.2, 1.6, 1.0], 13, "Par: STS3250 nominal 16 kgf·cm = 1,569 N·m", { boldFirst: true });
pic(s, F + "FOTO-30_cargas_y_momentos.png", 6.85, 4.05, 5.88, 2.85, null, { frame: true });
note(s, "Par nominal a 12 V, no el de bloqueo. Esfuerzos de tubos y uniones sin aprobar: no hay FEA ni ensayo de rotura.", 6.55);

s = content("04 · Vuelco", "Con el brazo hacia adelante, el conjunto queda al límite del vuelco", 8);
pic(s, F + "FOTO-29_estabilidad_planta_CG.png", M, 1.5, 6.0, 5.4, null, { frame: true });
stat(s, 6.9, 1.5, 2.6, "1,023", "factor de seguridad frontal", ACC);
stat(s, 9.6, 1.5, 2.6, "1,2", "criterio exigido", MUTE);
table(s, 6.9, 3.1, 5.83, [["Dirección del brazo", "Distancia CG–borde", "FS"], ["Lateral", "59,42 mm", "3,63"], ["Atrás", "24,07 mm", "1,86"], ["Adelante", "0,85 mm", "1,023"]], [2.2, 2.2, 1], 13, null, { boldFirst: true, hl: [3] });
formulas(s, 6.9, 4.75, 5.83, 2.15, "Método", ["FS = M_estabilizante / M_volcante", "a_crit = g (b − y_CG) / h_CG ≈ 0,05 m/s²", "//Resultado del 01/11/2026 sobre CAD; 43.798 configuraciones"], 13);

// =============== 05 SIMULACIÓN E INTERFAZ
divider("05", "Simulación e interfaz", "El simulador usa la misma matemática que el robot; la interfaz tiene dos caras con la misma lógica.", D + "D-05_ciclo_mision_recolectora.png");

s = content("05 · Simulador", "Misión del recolector: barrer, aproximar, tomar y depositar", 9);
pic(s, D + "D-05_ciclo_mision_recolectora.png", M, 1.5, 6.4, 5.4, null, { frame: true });
card(s, 7.2, 1.5, 5.53, 2.5, "Cómo está definido", ["Escena 3D sobre el GLB del CAD, estado local", "Auto, Brazo (matrices en vivo) y Conjunto", "Alcance: nube de tomas verticales cada 25 mm"], { size: 13 });
card(s, 7.2, 4.15, 5.53, 2.75, "La misión", ["Cubos de 50 mm y 100 g, semilla 42", "Depósito de 4 celdas, barrido de 37 direcciones", "Fallos: perdido, inalcanzable, bloqueado, agarre, lleno", "Simulación: no es una misión física verificada"], { size: 13 });

s = content("05 · Interfaz", "Panel OLED y encoder: corto confirma, largo vuelve", 10);
pic(s, D + "D-07_navegacion_OLED_encoder.png", M, 1.5, 7.6, 3.6, null, { frame: true });
pic(s, F + "FOTO-53_autito_silueta_OLED_128x64.png", 8.45, 1.5, 4.28, 2.2, "Recurso 128 × 64 del dashboard", { bg: "000000" });
table(s, 8.45, 4.0, 4.28, [["Gesto", "Efecto"], ["Giro", "mover / cambiar valor"], ["Corto", "confirmar; Inicio → Menú"], ["Largo ≥ 800 ms", "Inicio → Rápidas; resto: volver"], ["Cambios", "Sí / No obligatorio"]], [1.4, 2.6], 12, null, { boldFirst: true });
flow(s, M, 5.45, ["Inicio 010001", "Menú 010002", "Submenú", "Guardar Sí/No"], 1.6, 0.75, 0.4, [BLUE, DARK2, DARK2, GREEN]);
note(s, "No hay botón de menú: volver es el ítem Volver. Ambos gestos se ejecutan al soltar.", 6.4);

s = content("05 · Interfaz", "Seis pasos para pasar de encendido a en marcha", 10);
const pasos = [["Encender", "el dashboard OLED muestra modo, red y batería"], ["Conectar", "AP AutoFinalRobo2-<id> o red guardada"], ["Abrir la Web", "IP o QR en el OLED"], ["Calibrar", "Hall, signos, línea y odometría"], ["Conducir", "pad Mecanum táctil, STOP siempre visible"], ["Programar", "GOTO y secuencias, simular antes de enviar"]];
pasos.forEach((p, i) => {
  const x = M + i * 2.04;
  s.addShape(pres.shapes.OVAL, { x: x + 0.6, y: 1.8, w: 0.7, h: 0.7, fill: { color: i < 3 ? BLUE : ACC }, line: { color: WHITE, width: 2 } });
  s.addText(String(i + 1), { x: x + 0.6, y: 1.8, w: 0.7, h: 0.7, fontFace: HEAD, fontSize: 20, bold: true, color: WHITE, align: "center", valign: "middle", margin: 0, isTextBox: true });
  if (i < 5) s.addShape(pres.shapes.LINE, { x: x + 1.35, y: 2.15, w: 1.3, h: 0, line: { color: LINE, width: 2 } });
  s.addText(p[0], { x, y: 2.7, w: 1.9, h: 0.4, fontFace: HEAD, fontSize: 16, bold: true, color: INK, align: "center", margin: 0, isTextBox: true });
  s.addText(p[1], { x, y: 3.1, w: 1.9, h: 1.0, fontFace: FONT, fontSize: 12, color: MUTE, align: "center", valign: "top", margin: 0, isTextBox: true });
});
card(s, M, 4.5, 6.0, 2.35, "Web: 12 vistas", ["Inicio, Conducir, Programas, Puesta a punto, Sensores", "Modelos 3D, Armado, Simulación, Conexión, Ayuda", "Infraestructura y Desarrollador (Protocol Studio)"], { size: 13 });
card(s, 6.73, 4.5, 6.0, 2.35, "Mismo árbol en OLED y Web", ["Ayuda incluye un OLED virtual con el mismo rasterizador", "Paridad comprobada en host: 1680 / 1680 casos", "La clave del AP solo aparece en el OLED local"], { size: 13 });

// =============== 06 AGREGADOS
divider("06", "Nuestros agregados", "Tres desarrollos propios sobre el kit: el cargador AFR2 Loader, el KEY/RST automático y el panel con encoder y OLED.", F + "FOTO-54_loader_operacion.png");

s = content("06 · Agregado · Loader", "AFR2 Loader: compilar, cargar y depurar desde un solo lugar", 11);
pic(s, F + "FOTO-54_loader_operacion.png", M, 1.5, 6.0, 3.6, "Operación (captura v1.2.0)", { frame: true });
pic(s, F + "FOTO-55_loader_flasher.png", 6.73, 1.5, 6.0, 3.6, "Flasher (captura v1.2.0)", { frame: true });
pic(s, D + "D-09_flujo_Loader.png", M, 5.25, W - 2 * M, 1.7, null);

s = content("06 · Agregado · Loader", "Qué resuelve el Loader", 11);
card(s, M, 1.5, 5.6, 5.4, null, [["Dos chips, un flujo:", "ATmega (completo o comunicación) y ESP (firmware propio, Web/LittleFS o AT)."], ["Portátil:", "rutas {App} y perfiles fuera del repositorio."], ["Carga segura:", "avrdude al Mega; esptool al ESP a través del Mega, hash verificado y backup opcional."], ["KEY/RST:", "manual o automático con el circuito NPN."], ["Depuración:", "monitor serie e intérprete USB con wire_catalog.json; Arranques y Reinicios."], ["MCP:", "asistentes de IA con revisión manual de cada pedido."]], { size: 13 });
pic(s, F + "FOTO-56_loader_monitor_serie.png", 6.4, 1.5, 3.1, 2.6, "Monitor serie", { frame: true });
pic(s, F + "FOTO-57_loader_configuracion.png", 9.63, 1.5, 3.1, 2.6, "Configuración", { frame: true });
pic(s, F + "FOTO-58_loader_mcp.png", 6.4, 4.3, 3.1, 2.6, "Servidor MCP", { frame: true });
card(s, 9.63, 4.3, 3.1, 2.3, "Versión", ["Fuente 2.4.0", "Distribución 2.0.0", "C# / Windows", "Flasheo con hardware sin comprobar"], { size: 12 });

s = content("06 · Agregado · KEY/RST", "KEY y RST automáticos: dos NPN en paralelo con los pulsadores", 12);
pic(s, D + "D-06_circuito_KEY_RST_NPN.png", M, 1.5, 7.4, 4.2, null, { frame: true });
pic(s, F + "FOTO-61_shield_vista_superior_KEY_RST.jpg", 8.25, 1.5, 4.48, 3.2, "Pulsadores KEY y RST del shield", { frame: true });
card(s, 8.25, 4.85, 4.48, 2.05, "Convivencia", ["D32/D33 arrancan como entradas sin pull-up", "Sin cables de control el shield queda de fábrica", "Modo automático opt-in en el Loader"], { size: 12 });
flow(s, M, 5.95, ["KEY alto", "20 ms", "RST 100 ms", "~200 ms", "esptool"], 1.2, 0.6, 0.35, [ACC, DARK2, ACC, DARK2, BLUE]);
note(s, "Implementado en firmware y Loader; circuito sin armar ni medir (06/11/2026).", 6.65);

s = content("06 · Agregado · Panel", "Encoder y OLED: entrada sin perder pasos, pantalla que no frena el control", 13);
card(s, M, 1.5, 4.05, 4.0, "Encoder KY-040", ["CLK D2 (INT4) · DT D3 (INT5) · SW D42", "La ISR solo guarda AB en una cola de 32 B", "main decodifica 2 o 4 transiciones por paso", "Corto y largo se deciden al soltar"], { size: 16, shadow: true, fill: WHITE });
card(s, M + 4.2, 1.5, 4.05, 4.0, "OLED SSD1306 128 × 64", ["I2C D20/D21, dirección 0x3C, I2C Manager", "Envío diferencial en tramos ≤ 16 columnas", "Redibujo ≥ 40 ms, último paso del ciclo", "Mismo rasterizador en la Web"], { size: 16, shadow: true, fill: WHITE });
pic(s, F + "FOTO-53_autito_silueta_OLED_128x64.png", M + 8.4, 1.5, 3.73, 2.3, null, { bg: "000000" });
table(s, M + 8.4, 4.0, 3.73, [["Conector", "Pines"], ["J1 1×5", "CLK DT SW SDA SCL"], ["J2 JST", "VCC 5 V · GND"], ["PCB", "20 × 35 × 2 mm"]], [1.2, 2.3], 12, null, { boldFirst: true });
note(s, "Montaje a 5 V informado; el KiCad conserva +3V3. PCB y soporte diseñados en KiCad y Fusion, sin ERC/DRC ni fabricación documentada.", 5.75);

// =============== CIERRE
s = content("Cierre", "Qué está medido, qué está diseñado y qué sigue", null, "Cerrar con el estado honesto del proyecto y los próximos pasos.");
card(s, M, 1.5, 3.95, 5.35, "Medido", ["370 pulsos Hall por vuelta", "Avance 1000 mm = 1000 mm", "Lateral 1000 → 775 mm (factor 0,775)", "OLED y encoder funcionando con el diagnóstico (29/10)"], { size: 17, fill: "E3F5EA", titleColor: GREEN });
card(s, M + 4.15, 1.5, 3.95, 5.35, "Diseñado e implementado", ["Firmware ATmega, ESP y Web 1.27.0", "Brazo v17 en CAD y libro de planos", "Simulador y misión", "Loader, NPN KEY/RST, panel OLED"], { size: 17, fill: "E4F0FF", titleColor: BLUE });
card(s, M + 8.3, 1.5, 3.83, 5.35, "Próximos pasos", ["Resolver el vuelco frontal (FS 1,023)", "Fabricar e integrar el brazo y su bus TTL", "Armar y medir el circuito KEY/RST", "Medir deriva, peso real y tiempo de ciclo"], { size: 17, fill: SOFT, titleColor: ACC });

num++;
s = pres.addSlide();
s.background = { color: DARK };
fitImage(s, F + "FOTO-01_robot_completo_CAD_v17.png", 7.4, 0.6, 5.5, 6.3);
s.addText("Gracias", { x: M + 0.1, y: 2.3, w: 6.5, h: 1.3, fontFace: HEAD, fontSize: 60, bold: true, color: WHITE, margin: 0, isTextBox: true });
s.addText("¿Preguntas?", { x: M + 0.1, y: 3.6, w: 6.5, h: 0.8, fontFace: HEAD, fontSize: 30, color: ACC2, margin: 0, isTextBox: true });
s.addText("AutoFinalRobo2 · Koba Chajchir", { x: M + 0.1, y: 5.4, w: 6.5, h: 0.4, fontFace: FONT, fontSize: 14, color: "C9CEDA", margin: 0, isTextBox: true });

pres.writeFile({ fileName: OUT }).then(f => console.log("ok", f, num, "filminas"));
