"""Validación documental autorizada. No importa ni ejecuta la Web."""
from pathlib import Path
import json, re, hashlib
base = Path(__file__).resolve().parent
report_src = Path("D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md")
logo_src = Path("D:/ProyectoRobotica2/assets/Logo.png")
errors = []
checks = []
def require(condition, message):
    checks.append({"check": message, "passed": bool(condition)})
    if not condition: errors.append(message)
t = json.loads((base / "tokens.json").read_text(encoding="utf-8"))
require(t["manualVersion"] == "0.1.3", "Versión de tokens 0.1.3")
require(t["status"] == "draft", "Aprobación en ejecución separada de cierre documental")
require(report_src.is_file(), "Informe completo disponible en D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md")
require(logo_src.is_file(), "Logo oficial disponible en D:/ProyectoRobotica2/assets/Logo.png")
for name in ("index.md", "sources.md", "manual.md", "tokens.json", "components.md", "decisions.md", "adoption.md", "audit.config.json"):
    require((base / name).is_file(), "Artefacto: " + name)
decisions = set(re.findall(r"VIS-\d{3}", (base / "decisions.md").read_text(encoding="utf-8")))
count = 0
def walk(value):
    global count
    if not isinstance(value, dict): return
    if "value" in value:
        count += 1
        require(all(k in value for k in ("purpose", "confidence", "approval", "verification", "decisionId", "source")), "Metadatos: " + value.get("purpose", "?"))
        require(value.get("decisionId") in decisions, "Decisión existente: " + value.get("purpose", "?"))
        require(value.get("confidence") in ("Confirmada", "Observada", "Inferida", "Propuesta"), "Evidencia válida: " + value.get("purpose", "?"))
        require(value.get("approval") in ("Pendiente", "Aprobada", "Rechazada", "Sustituida"), "Aprobación válida: " + value.get("purpose", "?"))
        require(value.get("verification") == "No verificada", "No atribuir renderizado: " + value.get("purpose", "?"))
    else:
        for v in value.values(): walk(v)
for section in ("primitives", "semantic", "sections", "components", "motion"): walk(t[section])
config = json.loads((base / "audit.config.json").read_text(encoding="utf-8"))
require(config["scanPaths"] == [], "Sin escaneo implícito de interfaz")
for file in base.glob("*.md"):
    if file.name == "VALIDACION.md": continue
    content = file.read_text(encoding="utf-8")
    for target in re.findall(r"\]\(([^)]+)\)", content):
        if ":" in target or target.startswith("#"): continue
        require((file.parent / target.split("#")[0]).exists(), "Enlace local: " + file.name + " → " + target)
for file in ("index.md", "manual.md", "adoption.md"):
    require("0.1.3" in (base / file).read_text(encoding="utf-8"), "Versión en " + file)
def rgb(h): return [int(h[i:i+2], 16) / 255 for i in (1,3,5)]
def lum(c): return sum(w * (v/12.92 if v <= .04045 else ((v+.055)/1.055)**2.4) for w,v in zip((.2126,.7152,.0722),c))
def ratio(a,b):
    low,high = sorted((lum(a),lum(b)))
    return (high+.05)/(low+.05)
contrasts = []
for theme in ("light", "dark"):
    s = t["semantic"][theme]
    bg = rgb(s["background"]["value"])
    surfaces = {"background": bg, "solid": rgb(s["surfaceSolid"]["value"])}
    for name,m in t["components"].items():
        if not name.startswith("glass"): continue
        v = m["value"]
        a = v["opacityLight" if theme == "light" else "opacityDark"]
        require(0 <= a <= 1 and v["blurPx"] >= 0, "Material válido: " + theme + "/" + name)
        surfaces[name] = [a*c + (1-a)*b for c,b in zip(rgb(s["surfaceSolid"]["value"]),bg)]
    for role in ("text", "muted", "success", "warning", "error"):
        for surface,bgcolor in surfaces.items():
            value = ratio(rgb(s[role]["value"]),bgcolor)
            contrasts.append({"theme":theme,"foreground":role,"background":surface,"ratio":round(value,4),"target":4.5})
            require(value >= 4.5, "Contraste declarado: " + theme + "/" + role + "/" + surface)
    for action in ("action", "actionHover"):
        value = ratio(rgb(s["onAction"]["value"]),rgb(s[action]["value"]))
        contrasts.append({"theme":theme,"foreground":"onAction","background":action,"ratio":round(value,4),"target":4.5})
        require(value >= 4.5, "Contraste acción: " + theme + "/" + action)
    for surface,bgcolor in surfaces.items():
        for role, threshold in (("borderStrong",3),("focus",3)):
            raw=s[role]["value"]
            if raw.startswith("rgba"):
                parts=[float(x) for x in raw[5:-1].split(",")];color=[parts[3]*x/255+(1-parts[3])*b for x,b in zip(parts[:3],bgcolor)]
            else: color=rgb(raw)
            value=ratio(color,bgcolor)
            contrasts.append({"theme":theme,"foreground":role,"background":surface,"ratio":round(value,4),"target":threshold})
            require(value>=threshold,"Límite/foco: "+theme+"/"+role+"/"+surface)
    require(s["action"]["value"].lower() != s["error"]["value"].lower(), "Diferenciación cromática action vs error/STOP: " + theme)
hashes = {name:hashlib.sha256((base/name).read_bytes()).hexdigest() for name in ("index.md","manual.md","tokens.json","components.md","decisions.md","adoption.md","sources.md","audit.config.json")}
if report_src.is_file():
    hashes["D:/ProyectoRobotica2/informe-portabilidad-glassmorphism.md"] = hashlib.sha256(report_src.read_bytes()).hexdigest()
if logo_src.is_file():
    hashes["D:/ProyectoRobotica2/assets/Logo.png"] = hashlib.sha256(logo_src.read_bytes()).hexdigest()
report={"manualVersion":t["manualVersion"],"date":"2026-10-23","scope":"Documentos, cotejo íntegro con informe, logo oficial y cálculo matemático; sin navegador ni producto", "result":"passed-with-limitations" if not errors else "failed", "tokens":count,"checks":len(checks),"errors":errors,"contrastPairs":contrasts,"limitations":["Manual draft: plan visual cerrado documentalmente; verificación visual sujeta a renderizado", "Sin renderizado, comprobación móvil, aplicación Web ni hardware", "Contrastes calculados solo sobre base de tema y materiales declarados; no fondos arbitrarios"],"sourceHashes":hashes,"details":checks}
(base / "validation.json").write_text(json.dumps(report,ensure_ascii=False,indent=2)+"\n",encoding="utf-8")
print(json.dumps({k:report[k] for k in ("result","tokens","checks","errors")},ensure_ascii=True))
raise SystemExit(1 if errors else 0)
