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

// ===== Filminas 18–22: matrices y fórmulas armadas (se concatena después de los helpers de build2.js)
num = 17;
const EQ = 18;                      // tamaño de fórmula
const CH = INK;

function rich(str, base) {
  const out = [], re = /_\{([^}]*)\}|_([A-Za-z0-9]+)/g;
  let last = 0, m;
  while ((m = re.exec(str))) {
    if (m.index > last) out.push({ text: str.slice(last, m.index), options: { ...base } });
    out.push({ text: m[1] !== undefined ? m[1] : m[2], options: { ...base, subscript: true } });
    last = re.lastIndex;
  }
  if (last < str.length) out.push({ text: str.slice(last), options: { ...base } });
  return out.length ? out : [{ text: str, options: { ...base } }];
}
function tw(t, size) { return Math.max(0.25, [...t.replace(/_\{([^}]*)\}|_/g, '$1')].length * size / 72 * 0.5 + 0.12); }
function txt(s, t, x, yc, size, color = CH, w) {
  const ww = w || tw(t, size);
  s.addText(rich(t, {}), { x, y: yc - 0.28, w: ww, h: 0.56, fontFace: MATH, fontSize: size, color, align: "center", valign: "middle", margin: 0, isTextBox: true });
  return ww;
}
// Matriz con corchetes. rows: array de filas (strings). Devuelve ancho usado.
function matrix(s, x, yc, rows, o = {}) {
  const size = o.size || 15, cw = o.cw || 0.62, ch = o.ch || 0.42;
  const nr = rows.length, nc = rows[0].length, h = nr * ch, w = nc * cw, top = yc - h / 2;
  const bw = 0.1;
  s.addShape(pres.shapes.LEFT_BRACKET, { x, y: top - 0.04, w: bw, h: h + 0.08, line: { color: CH, width: 1.5 } });
  rows.forEach((r, i) => r.forEach((c, j) => {
    s.addText(rich(String(c), {}), { x: x + bw + 0.04 + j * cw, y: top + i * ch, w: cw, h: ch, fontFace: MATH, fontSize: size, color: o.colors && o.colors[i] ? o.colors[i] : CH, align: "center", valign: "middle", margin: 0, isTextBox: true });
  }));
  const tot = w + 2 * bw + 0.08;
  s.addShape(pres.shapes.RIGHT_BRACKET, { x: x + tot - bw, y: top - 0.04, w: bw, h: h + 0.08, line: { color: CH, width: 1.5 } });
  if (o.label) s.addText(o.label, { x: x - 0.35, y: yc + h / 2 + 0.08, w: tot + 0.7, h: 0.5, fontFace: FONT, fontSize: 10.5, color: o.labelColor || MUTE, align: "center", valign: "top", margin: 0, isTextBox: true });
  return tot;
}
// Fila de ecuación: tokens = string | {m: filas, ...opciones}
function eqRow(s, x, yc, tokens, size = EQ, gap = 0.08) {
  let cx = x;
  tokens.forEach(t => {
    if (typeof t === "string") cx += txt(s, t, cx, yc, size) + gap;
    else cx += matrix(s, cx, yc, t.m, t) + gap;
  });
  return cx;
}
function sub(s, t, x, y, w, h = 0.3) {   // título de bloque
  s.addText(t, { x, y, w, h, fontFace: HEAD, fontSize: 14, bold: true, color: ACC, margin: 0, isTextBox: true });
}
function desc(s, t, x, y, w, h = 0.5, size = 12) {
  s.addText(t, { x, y, w, h, fontFace: FONT, fontSize: size, color: MUTE, margin: 0, valign: "top", isTextBox: true });
}
function panel(s, x, y, w, h, fill = WHITE) {
  s.addShape(pres.shapes.ROUNDED_RECTANGLE, { x, y, w, h, rectRadius: 0.08, fill: { color: fill }, line: { color: fill === WHITE ? LINE : fill, width: 1 } });
}
// Línea "fórmula + descripción"
function eqLine(s, x, y, w, f, d, size = 17) {
  s.addText(rich(f, {}), { x, y, w, h: 0.42, fontFace: MATH, fontSize: size, color: CH, margin: 0, valign: "middle", isTextBox: true });
  if (d) s.addText(d, { x, y: y + 0.42, w, h: 0.3, fontFace: FONT, fontSize: 11.5, color: MUTE, margin: 0, valign: "top", isTextBox: true });
}
function glossary(s, x, y, w, h, title, items) {
  panel(s, x, y, w, h, PANEL);
  sub(s, title, x + 0.22, y + 0.15, w - 0.44);
  const runs = items.map((it, i) => [
    { text: it[0] + "  ", options: { fontFace: MATH, bold: true, color: ACC, breakLine: false } },
    { text: it[1], options: { fontFace: FONT, color: INK, breakLine: i < items.length - 1, paraSpaceAfter: 5 } }]).flat();
  s.addText(runs, { x: x + 0.22, y: y + 0.55, w: w - 0.44, h: h - 0.65, fontSize: 12.5, valign: "top", margin: 0, isTextBox: true });
}

// ---------------- 18 · Cinemática Mecanum
let s = content("04 · Matemática del auto", "Cinemática Mecanum: tres velocidades repartidas en cuatro ruedas", 5);
panel(s, M, 1.5, 8.45, 2.75);
sub(s, "Cinemática inversa · del cuerpo a las ruedas", M + 0.22, 1.62, 8);
eqRow(s, M + 0.25, 2.9, [
  { m: [["u_FL"], ["u_FR"], ["u_RL"], ["u_RR"]], cw: 0.75, label: "velocidad periférica\nde cada rueda [m/s]" }, "=",
  { m: [["1", "−1", "−L"], ["1", "1", "L"], ["1", "1", "−L"], ["1", "−1", "L"]], cw: 0.6, label: "A · geometría Mecanum\n(una fila por rueda)" }, "·",
  { m: [["−1", "0", "0"], ["0", "−1", "0"], ["0", "0", "−1"]], cw: 0.55, label: "S · signos del contrato" }, "·",
  { m: [["vₓ"], ["v_y"], ["ω"]], cw: 0.6, label: "V · velocidad del cuerpo\n(avance, lateral, giro)" }]);
pic(s, D + "D-11_patrones_rueda_mecanum.png", 9.2, 1.5, 3.53, 2.75, null, { frame: true });

panel(s, M, 4.4, 8.45, 2.2);
sub(s, "Cinemática directa · de las ruedas al cuerpo", M + 0.22, 4.52, 8);
eqRow(s, M + 0.25, 5.45, [
  { m: [["vₓ"], ["v_y"], ["ω"]], cw: 0.6, label: "lo que se mueve el auto" }, "=", "S ·",
  { m: [["¼", "¼", "¼", "¼"], ["−¼", "¼", "¼", "−¼"], ["−1/4L", "1/4L", "−1/4L", "1/4L"]], cw: 0.82, size: 14, label: "A⁺ · pseudoinversa de A (promedia las 4 ruedas)" }, "·",
  { m: [["u_FL"], ["u_FR"], ["u_RL"], ["u_RR"]], cw: 0.75, ch: 0.3, size: 12, label: "medido por los Hall" }]);
glossary(s, 9.2, 4.4, 3.53, 2.2, "Parámetros", [["r = 0,040 m", "radio de rueda"], ["L = 0,170 m", "semidistancias (62 + 107,66 mm)"], ["ω_rueda = u/r", "velocidad angular de cada rueda"], ["γ", "escala si se pasa u_max (150 mm/s)"]]);
note(s, "Ejemplo: vₓ = 100 mm/s → cada rueda u = 100 mm/s → ω = 2,5 rad/s = 23,9 rpm. S = diag(−1,−1,−1) son los signos del contrato (physicalVerification = false).", 6.62);

// ---------------- 19 · Odometría
s = content("04 · Matemática del auto", "Odometría: de pulsos Hall a pose, cada 10 ms", 5);
panel(s, M, 1.5, 4.0, 2.2);
sub(s, "① Pulsos → recorrido de cada rueda", M + 0.22, 1.62, 3.6);
eqLine(s, M + 0.25, 2.05, 3.6, "Δsᵢ = Δnᵢ · π · D / Nᵢ", "Δnᵢ pulsos contados · D = 80 mm · Nᵢ = 370 pulsos/vuelta");
eqLine(s, M + 0.25, 2.85, 3.6, "π·80/370 = 0,679 mm", "recorrido por pulso (1000 mm ≈ 1472 pulsos)", 16);

panel(s, 4.75, 1.5, 7.98, 2.2);
sub(s, "② Mezcla inversa · cuatro ruedas → tres desplazamientos", 4.97, 1.62, 7.5);
eqRow(s, 5.0, 2.7, [
  { m: [["Δx"], ["Δy"], ["Δψ"]], cw: 0.6, ch: 0.34, size: 14, label: "desplazamiento del cuerpo" }, "=", "¼ ·",
  { m: [["1", "1", "1", "1"], ["−1", "1", "1", "−1"], ["−1/L", "1/L", "−1/L", "1/L"]], cw: 0.66, ch: 0.34, size: 14, label: "promedio con signos Mecanum" }, "·",
  { m: [["Δs₁"], ["Δs₂"], ["Δs₃"], ["Δs₄"]], cw: 0.6, ch: 0.26, size: 12, label: "FL, FR, RL, RR" }], 16);

panel(s, M, 3.85, 8.1, 3.05);
sub(s, "③ Integración al mundo con rumbo medio", M + 0.22, 3.97, 7.6);
eqRow(s, M + 0.25, 5.15, [
  { m: [["x"], ["y"], ["ψ"]], cw: 0.5, label: "pose en k+1" }, "=",
  { m: [["x"], ["y"], ["ψ"]], cw: 0.5, label: "pose en k" }, "+",
  { m: [["cos ψₘ", "−sin ψₘ", "0"], ["sin ψₘ", "cos ψₘ", "0"], ["0", "0", "1"]], cw: 1.0, size: 14, label: "rotación del cuerpo al mundo" }, "·",
  { m: [["Δx·kₓ"], ["Δy·k_y"], ["Δψ·k_ψ"]], cw: 0.95, size: 14, label: "desplazamiento corregido" }], 16);
desc(s, "ψₘ = ψₖ + Δψ/2 (rumbo medio: reduce el error de integración) · k = factor direccional medido: avance 1,000, lateral 0,775, giro 1,000 · ψ se normaliza a ±π", M + 0.25, 6.3, 7.6, 0.5, 11.5);

panel(s, 8.85, 3.85, 3.88, 3.05, PANEL);
sub(s, "Medido por nosotros", 9.07, 3.97, 3.5);
stat(s, 9.07, 4.25, 1.7, "370", "pulsos por vuelta");
stat(s, 10.85, 4.25, 1.8, "0,775", "factor lateral");
desc(s, "Avance de 1000 mm = 1000 mm reales. Lateral de 1000 mm por odometría = 775 mm reales (los rodillos patinan). Sin giroscopio, el rumbo deriva con la distancia.", 9.07, 5.75, 3.45, 1.1, 11.5);

// ---------------- 20 · GOTO
s = content("04 · Matemática del auto", "GOTO: control proporcional sobre la pose odométrica", 6);
pic(s, D + "D-04_flujo_GOTO.png", M, 1.5, 4.1, 5.4, null, { frame: true });
panel(s, 4.85, 1.5, 7.88, 1.75);
sub(s, "① Objetivo en el mundo (X, Y relativos al rumbo al empezar)", 5.07, 1.62, 7.5);
eqRow(s, 5.1, 2.5, [
  { m: [["x_m"], ["y_m"]], cw: 0.6, ch: 0.36 }, "=", { m: [["x"], ["y"]], cw: 0.45, ch: 0.36 }, "+",
  { m: [["cos ψ", "−sin ψ"], ["sin ψ", "cos ψ"]], cw: 0.95, ch: 0.36, size: 14 }, "·", { m: [["X"], ["Y"]], cw: 0.45, ch: 0.36 }, " ;  ψ_m = ψ + θ"], 16);

panel(s, 4.85, 3.4, 7.88, 1.85);
sub(s, "② Error llevado al marco del robot = consigna de velocidad", 5.07, 3.52, 7.5);
eqRow(s, 5.1, 4.45, [
  { m: [["vₓ"], ["v_y"]], cw: 0.5, ch: 0.36 }, "=",
  { m: [["cos ψ", "sin ψ"], ["−sin ψ", "cos ψ"]], cw: 0.95, ch: 0.36, size: 14, label: "Rᵀ(ψ): mundo → robot" }, "·",
  { m: [["x_m − x"], ["y_m − y"]], cw: 1.05, ch: 0.36, size: 14, label: "error eₓ, e_y [mm]" }, " ;  ω = ψ_m − ψ"], 16);

panel(s, 4.85, 5.4, 4.6, 1.5, PANEL);
sub(s, "③ Saturación", 5.07, 5.5, 4.2);
eqLine(s, 5.1, 5.85, 4.2, "|vₓ| + |v_y| + |ω|·L ≤ v", "escala todo junto; mínimo 40 mm/s fuera de tolerancia", 16);
panel(s, 9.6, 5.4, 3.13, 1.5, SOFT);
sub(s, "Llegada", 9.82, 5.5, 2.8);
desc(s, "|eₓ|, |e_y| ≤ 10 mm\n|e_ψ| ≤ 25 mrad (≈ 1,4°)\nmáximo 60 s por tramo", 9.82, 5.85, 2.8, 1.0, 12.5);

// ---------------- 21 · Brazo: transformaciones y directa
s = content("04 · Matemática del brazo", "Brazo: transformaciones homogéneas y cinemática directa", 7);
pic(s, D + "D-08_cadena_cinematica.png", M, 1.45, W - 2 * M, 1.55, null, { frame: true });
panel(s, M, 3.15, 3.95, 2.3);
sub(s, "Transformación homogénea", M + 0.22, 3.25, 3.6);
eqRow(s, M + 0.25, 4.3, ["T =", { m: [["R₃ₓ₃", "t"], ["0  0  0", "1"]], cw: 0.95, ch: 0.42, size: 14 }], 16);
desc(s, "R: rotación (sus columnas son los ejes locales) · t: traslación [m] · se encadenan multiplicando: T(A→C) = T(A→B)·T(B→C)", M + 0.22, 4.9, 3.55, 0.5, 11);
panel(s, 4.7, 3.15, 4.0, 2.3);
sub(s, "Rotación sobre Z (J1, J4, J6)", 4.92, 3.25, 3.6);
eqRow(s, 4.95, 4.4, ["Rz(θ) =", { m: [["cos θ", "−sin θ", "0"], ["sin θ", "cos θ", "0"], ["0", "0", "1"]], cw: 0.72, ch: 0.36, size: 13 }], 15);
panel(s, 8.85, 3.15, 3.88, 2.3);
sub(s, "Rotación sobre Y (J2, J3, J5)", 9.07, 3.25, 3.5);
eqRow(s, 9.1, 4.4, ["Ry(θ) =", { m: [["cos θ", "0", "sin θ"], ["0", "1", "0"], ["−sin θ", "0", "cos θ"]], cw: 0.72, ch: 0.36, size: 13 }], 15);
panel(s, M, 5.6, W - 2 * M, 1.3, PANEL);
eqRow(s, M + 0.25, 6.2, [
  { m: [["x_W"], ["y_W"], ["z_W"]], cw: 0.55, ch: 0.3, size: 13, labelColor: MUTE }, "=",
  { m: [["ρ cos q₁"], ["ρ sin q₁"], ["h + L₁cos q₂ + L₂cos(q₂+q₃)"]], cw: 2.6, ch: 0.3, size: 13 }], 15);
desc(s, "Centro de muñeca W, con ρ = L₁ sin q₂ + L₂ sin(q₂+q₃)\nTCP: p(TCP) = p(W) + d·R·e(z), R = Rz(q₁)Ry(q₂+q₃)Rz(q₄)Ry(q₅)Rz(q₆)\nh 0,209 · L₁ 0,070 · L₂ 0,115 · d 0,1435 m · ej.: q₂ = 90° → TCP (0,3285; 0; 0,209) m", 5.55, 5.72, 7.0, 1.1, 12);

// ---------------- 22 · Inversa y conjunto
s = content("04 · Matemática del brazo y del conjunto", "Cinemática inversa y modelo del conjunto (9 grados de libertad)", 7);
panel(s, M, 1.5, 6.1, 5.4);
sub(s, "Cinemática inversa · desacople en la muñeca", M + 0.22, 1.62, 5.7);
const inv = [
  ["p_W = p_d − d · R_d · e_z", "① se resta la herramienta: queda el centro de muñeca"],
  ["q₁ = atan2(y_W, x_W) ;  ρ = √(x_W² + y_W²)", "② giro de base y alcance radial"],
  ["cos q₃ = (ρ² + s² − L₁² − L₂²) / (2 L₁ L₂)", "③ teorema del coseno: dos soluciones de codo (±)"],
  ["q₂ = atan2(ρ, s) − atan2(L₂ sin q₃, L₁ + L₂ cos q₃)", "④ hombro, con s = z_W − h"],
  ["q₅ = atan2(√(r₁₃² + r₂₃²), r₃₃)", null],
  ["q₄ = atan2(r₂₃, r₁₃) ;  q₆ = atan2(r₃₂, −r₃₁)", "⑤ muñeca Z–Y–Z desde R_W = [Rz(q₁)·Ry(q₂+q₃)]ᵀ·R_d (sin q₅ ≠ 0)"]];
inv.forEach((e, i) => eqLine(s, M + 0.25, 2.0 + Math.min(i, 4) * 0.86 + (i === 5 ? 0.42 : 0), 5.7, e[0], e[1], 15));

panel(s, 6.85, 1.5, 5.88, 3.25);
sub(s, "Pose de la base Mecanum en el mundo", 7.07, 1.62, 5.5);
eqRow(s, 7.1, 3.05, ["T_{W,B} =", { m: [["cos ψ", "−sin ψ", "0", "x"], ["sin ψ", "cos ψ", "0", "y"], ["0", "0", "1", "0"], ["0", "0", "0", "1"]], cw: 0.78, ch: 0.38, size: 13, label: "rotación ψ y traslación (x, y) del auto" }], 15);
panel(s, 6.85, 4.9, 5.88, 2.0, PANEL);
eqLine(s, 7.07, 5.0, 5.5, "T_{W,TCP}(Q) = T_{W,B}(x,y,ψ) · T_{B,M} · T_{M,TCP}(q)", "Q = (x, y, ψ, q₁…q₆): la pinza no suma grados de pose", 14);
eqLine(s, 7.07, 5.85, 5.5, "V_TCP = J_base·V_base + J_brazo·q̇ ;  Jᵢ = [ zᵢ × (p_TCP − oᵢ) ; zᵢ ]", "base y brazo suman velocidad; singular si sin q₃ = 0, ρ = 0 o sin q₅ = 0", 13);

pres.writeFile({ fileName: OUT }).then(f => console.log("ok", f, num));
