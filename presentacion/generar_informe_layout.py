#!/usr/bin/env python3
"""Genera INFORME_DIAGRAMACION.html: wireframes 16:9 de las 13 filminas con las imágenes reales
(fotos_existentes/ y diagramas/png/) colocadas en su zona. Solo escribe el HTML junto a este script."""
import html, os
OUT = os.path.dirname(os.path.abspath(__file__))
F = "fotos_existentes/"; D = "diagramas/png/"
# zona: (id, tipo, c0, c1, f0, f1, título, detalle, [imágenes])  tipos: img diag text table formula todo
S = [
 (1, "Introducción y arquitectura", "Filmina_01_Introduccion.md", "60 s", [
  ("A","img",1,4,2,6,"Hero","FOTO-01 robot completo CAD",[F+"FOTO-01_robot_completo_CAD_v17.png"]),
  ("B","diag",5,12,2,4,"Arquitectura","D-01",[D+"D-01_arquitectura_general.png"]),
  ("C","text",5,8,5,5,"Problema y objetivo","3 viñetas + criterio de evidencia",[]),
  ("D","table",9,12,5,5,"Seis módulos","tabla 2 columnas",[]),
  ("E","text",5,12,6,6,"Cifras","chips: 4+6 · 7 servos · 54 cmd · 101 pantallas · 1.27.0",[])]),
 (2, "Modelos 3D", "Filmina_02_Modelos.md", "60 s", [
  ("A","img",1,4,2,5,"Auto base","FOTO-03 (+05/06 mini)",[F+"FOTO-03_auto_base_frente_blender.png"]),
  ("B","img",5,8,2,5,"Brazo v17","FOTO-07 + tabla de cotas",[F+"FOTO-07_brazo_v17_CAD.png"]),
  ("C","img",9,12,2,5,"Conjunto","FOTO-01 + FOTO-08 captura Web",[F+"FOTO-01_robot_completo_CAD_v17.png"]),
  ("D","table",1,12,6,6,"Datos clave","3 columnas alineadas con A·B·C",[])]),
 (3, "Pinout y conexionado", "Filmina_03_Pinout_y_conexionado.md", "60 s", [
  ("A","table",1,6,2,6,"Tabla de pinout","motores · Hall/sensores · enlace y agregados ⭐",[]),
  ("B","diag",7,12,2,4,"Conexionado","D-02",[D+"D-02_conexionado.png"]),
  ("C","img",7,9,5,6,"Shield","FOTO-12 pinout OSOYOO",[F+"FOTO-12_shield_osoyoo_pinout.jpg"]),
  ("D","todo",10,12,5,6,"Cableado real","FOTO-14 / FOTO-15 por tomar",[])]),
 (4, "Firmwares y ciclo", "Filmina_04_Firmwares_y_ciclo.md", "75 s", [
  ("A","text",1,3,2,5,"ATmega2560","qué es · qué hace · seguridad",[]),
  ("B","text",4,6,2,5,"ESP8266","HTTP · WS · broker · LittleFS",[]),
  ("C","text",7,9,2,5,"Web","React · vistas · cliente afr2-ws-1",[]),
  ("D","diag",10,12,2,6,"Ciclo principal","D-03a (vertical)",[D+"D-03a_ciclo_principal_ATmega.png"]),
  ("E","table",1,9,6,6,"Enlace compartido","3 chips: WS · ROBO2/EX01 · Contracts",[])]),
 (5, "Matemática del auto y odometría", "Filmina_05_Matematica_auto_y_odometria.md", "90 s", [
  ("A","formula",1,6,2,5,"Cinemática Mecanum","u=A·S·V · A⁺ · γ · ejemplos",[]),
  ("B","diag",7,12,2,3,"Patrones de rueda","D-11",[D+"D-11_patrones_rueda_mecanum.png"]),
  ("C","formula",7,10,4,6,"Odometría","Δφ · Δs · mezcla inversa · integración",[]),
  ("D","img",11,12,4,6,"Rueda + medidos","FOTO-10 + tabla medido",[F+"FOTO-10_rueda_mecanum.jpg"]),
  ("E","text",1,6,6,6,"Parámetros","r=40 · L=170 · N=370 · D=80",[])]),
 (6, "GOTO y odometría", "Filmina_06_GOTO_y_odometria.md", "90 s", [
  ("A","diag",1,4,2,6,"Pipeline odometría","D-10",[D+"D-10_pipeline_odometria.png"]),
  ("B","diag",5,8,2,6,"Flujo GOTO","D-04",[D+"D-04_flujo_GOTO.png"]),
  ("C","formula",9,12,2,4,"Ley de control","error · marco robot · llegada 10 mm/25 mrad",[]),
  ("D","todo",9,12,5,6,"Web GOTO","FOTO-21 / FOTO-22 capturas",[])]),
 (7, "Matemática del brazo y del conjunto", "Filmina_07_Matematica_brazo_y_conjunto.md", "100 s", [
  ("A","diag",1,12,2,3,"Cadena cinemática","D-08 (banda ancha)",[D+"D-08_cadena_cinematica.png"]),
  ("B","formula",1,4,4,6,"Cinemática directa","T · ρ · z_W · p_TCP · ejemplo",[]),
  ("C","formula",5,8,4,6,"Cinemática inversa","p_W · q1 · q3 · q2 · muñeca",[]),
  ("D","formula",9,12,4,5,"Conjunto + Jacobiano","Q · T_W,TCP · V_TCP",[]),
  ("E","todo",9,12,6,6,"Simulador","FOTO-25 / FOTO-26",[])]),
 (8, "Esfuerzos, tensiones y vuelco", "Filmina_08_Esfuerzos_y_vuelco.md", "100 s", [
  ("A","table",1,4,2,4,"Masas y cargas","total 4,045 kg",[]),
  ("B","table",5,8,2,4,"Pares y márgenes","STS3250 16 kgf·cm",[]),
  ("C","img",9,12,2,4,"Vuelco","FOTO-29 CG 0,85 mm",[F+"FOTO-29_estabilidad_planta_CG.png"]),
  ("D","formula",1,8,5,6,"Método","r_G · FS=M_est/M_vol · a_crit",[]),
  ("E","table",9,12,5,6,"Tabla de vuelco","lateral 3,63 · atrás 1,86 · adelante 1,023",[])]),
 (9, "Simulador, misión y ciclo", "Filmina_09_Simulador_y_mision.md", "90 s", [
  ("A","text",1,4,2,4,"Cómo está definido","escena 3D · convención · alcance",[]),
  ("B","todo",5,12,2,4,"Galería","FOTO-33 · 34 · 35 · 36 capturas",[]),
  ("C","text",1,4,5,6,"Misión","cubos 50 mm · 4 celdas · fallos",[]),
  ("D","diag",5,9,5,6,"Ciclo de misión","D-05",[D+"D-05_ciclo_mision_recolectora.png"]),
  ("E","todo",10,12,5,6,"Secuencia de toma","FOTO-37 · 38 · 39",[])]),
 (10, "Interfaz: pasos y pantallas", "Filmina_10_Interfaz.md", "90 s", [
  ("A","text",1,4,2,4,"Pasos de uso","1 encender → 6 programar",[]),
  ("B","todo",5,12,2,3,"Pantallas OLED","FOTO-48…52 + FOTO-53 silueta",[F+"FOTO-53_autito_silueta_OLED_128x64.png"]),
  ("C","diag",5,8,4,6,"Navegación","D-07",[D+"D-07_navegacion_OLED_encoder.png"]),
  ("E","table",9,12,4,6,"Reglas de entrada","giro · corto · largo · Volver · Sí/No",[]),
  ("D","todo",1,4,5,6,"Web (7 capturas)","FOTO-41…47",[])]),
 (11, "Agregado · AFR2 Loader", "Filmina_11_Agregado_Loader.md", "60 s", [
  ("A","text",1,4,2,5,"Qué resuelve","6 viñetas",[]),
  ("B","img",5,12,2,4,"Capturas","FOTO-54 Operación + FOTO-55 Flasher",[F+"FOTO-54_loader_operacion.png",F+"FOTO-55_loader_flasher.png"]),
  ("C","diag",5,12,5,6,"Flujo","D-09",[D+"D-09_flujo_Loader.png"]),
  ("D","table",1,4,6,6,"Chips","2.4.0 · C# · portátil · manual",[])]),
 (12, "Agregado · NPN KEY / RST", "Filmina_12_Agregado_NPN_KEY_RST.md", "60 s", [
  ("A","diag",1,7,2,5,"Circuito","D-06",[D+"D-06_circuito_KEY_RST_NPN.png"]),
  ("B","img",8,12,2,3,"Shield","FOTO-61 KEY/RST",[F+"FOTO-61_shield_vista_superior_KEY_RST.jpg"]),
  ("C","text",8,12,4,5,"Problema → solución","secuencia de entrada y convivencia",[]),
  ("D","todo",1,12,6,6,"Circuito real","FOTO-62 · 63 · 64",[])]),
 (13, "Agregado · Encoder y OLED", "Filmina_13_Agregado_Encoder_y_OLED.md", "75 s", [
  ("A","todo",1,4,2,5,"Hardware","FOTO-65 panel · FOTO-66 PCB",[]),
  ("B","text",5,8,2,5,"Encoder","ISR · cola 32 B · botón al soltar",[]),
  ("C","text",9,12,2,5,"OLED","SSD1306 · I2C Manager · diferencial",[]),
  ("D","table",1,8,6,6,"Conectores","J1 1×5 · J2 JST 1×2",[]),
  ("E","todo",9,12,6,6,"Soporte / esquema","FOTO-67 · 68",[])]),
]
COL = {"img":"#e4f0ff","diag":"#efe9ff","text":"#f3f5f9","table":"#fff4d6","formula":"#e3f5ea","todo":"#fde7ea"}
TXT = {"img":"imagen existente","diag":"diagrama","text":"texto","table":"tabla","formula":"fórmulas","todo":"foto/captura por hacer"}
css = """body{font-family:Segoe UI,Arial,sans-serif;color:#1f2430;margin:0;background:#fff}
main{max-width:1180px;margin:0 auto;padding:24px}h1{font-size:30px;border-bottom:3px solid #b81d33;padding-bottom:8px}
h2{font-size:22px;margin:40px 0 4px}.meta{color:#4a5163;font-size:13px;margin-bottom:10px}
.slide{position:relative;width:1120px;height:630px;border:1px solid #9aa1b2;background:#fff;box-shadow:0 2px 8px #0002;display:grid;
grid-template-columns:repeat(12,1fr);grid-template-rows:repeat(6,1fr);gap:6px;padding:14px;box-sizing:border-box}
.t{grid-column:1/13;grid-row:1;border-bottom:3px solid #b81d33;font-weight:700;font-size:24px;display:flex;align-items:center}
.z{border:1.5px solid #4a5163;border-radius:6px;overflow:hidden;position:relative;display:flex;flex-direction:column;font-size:12px}
.z b{background:#b81d33;color:#fff;font-size:11px;padding:1px 6px;align-self:flex-start;border-radius:0 0 6px 0}
.z span{padding:2px 6px}.z .imgs{flex:1;display:flex;gap:2px;min-height:0;justify-content:center;align-items:center}
.z img{max-width:100%;max-height:100%;object-fit:contain;min-width:0}.z .tag{font-size:10px;color:#4a5163}
table{border-collapse:collapse;font-size:13px}td,th{border:1px solid #9aa1b2;padding:3px 8px}
.legend span{display:inline-block;padding:3px 10px;margin:2px 6px 2px 0;border:1px solid #4a5163;border-radius:5px;font-size:12px}
.foot{position:absolute;right:14px;bottom:4px;font-size:10px;color:#4a5163}"""
o = ['<!doctype html><html lang="es"><meta charset="utf-8"><title>Informe de diagramación · AutoFinalRobo2</title><style>'+css+'</style><main>',
 '<h1>Informe de diagramación de la presentación · AutoFinalRobo2</h1>',
 '<p class="meta">13 filminas 16:9 sobre grilla 12×6 (título en f1). Cada recuadro es una zona A, B, C… con su contenido; las imágenes y diagramas que ya existen se muestran en su lugar. Fichas completas: <code>Filmina_NN_*.md</code>. Fotos: <code>FOTOS.md</code>. Diagramas: <code>DIAGRAMAS.md</code>. Prompt de armado: <code>PROMPT_ARMADO_PPTX.md</code>.</p>',
 '<div class="legend">'+"".join(f'<span style="background:{COL[k]}">{TXT[k]}</span>' for k in COL)+'</div>',
 '<h2>Paleta y tipografía</h2><table><tr><th>Uso</th><th>Color</th></tr><tr><td>Acento</td><td style="background:#b81d33;color:#fff">#B81D33</td></tr><tr><td>Texto</td><td style="background:#1f2430;color:#fff">#1F2430</td></tr><tr><td>Panel</td><td style="background:#f3f5f9">#F3F5F9</td></tr><tr><td>Hardware</td><td style="background:#efe9ff">#6A4BB5 / #EFE9FF</td></tr><tr><td>Software</td><td style="background:#e4f0ff">#2D6CB5 / #E4F0FF</td></tr><tr><td>Fallo</td><td style="background:#fde7ea">#B81D33 / #FDE7EA</td></tr><tr><td>Correcto</td><td style="background:#e3f5ea">#2A8A52 / #E3F5EA</td></tr></table><p class="meta">Segoe UI · título 30 pt · zona 16 pt carmesí · texto 14 pt (mín. 12) · fórmulas Cambria Math 16 pt · pie 10 pt.</p>']
tot = 0
for n, ttl, ficha, tiempo, zs in S:
    o.append(f'<h2>Filmina {n} · {html.escape(ttl)}</h2><div class="meta">Ficha: <code>{ficha}</code> · tiempo de exposición ≈ {tiempo}</div>')
    o.append(f'<div class="slide"><div class="t">{n} · {html.escape(ttl)}</div>')
    for zid, tp, c0, c1, f0, f1, name, det, imgs in zs:
        im = "".join(f'<img src="{i}">' for i in imgs)
        o.append(f'<div class="z" style="grid-column:{c0}/{c1+1};grid-row:{f0}/{f1+1};background:{COL[tp]}"><b>{zid} · c{c0}–c{c1} · f{f0}–f{f1}</b><span><strong>{html.escape(name)}</strong> <span class="tag">{TXT[tp]}</span><br>{html.escape(det)}</span><div class="imgs">{im}</div></div>')
    o.append(f'<div class="foot">Filmina {n} · producto 1.27.0</div></div>')
    o.append('<table><tr><th>Zona</th><th>Tipo</th><th>Rango</th><th>Contenido</th></tr>'+"".join(f'<tr><td>{z[0]}</td><td>{TXT[z[1]]}</td><td>c{z[2]}–{z[3]} · f{z[4]}–{z[5]}</td><td>{html.escape(z[6])}: {html.escape(z[7])}</td></tr>' for z in zs)+'</table>')
o.append('</main></html>')
open(os.path.join(OUT, "INFORME_DIAGRAMACION.html"), "w", encoding="utf-8").write("\n".join(o))
print("ok")
