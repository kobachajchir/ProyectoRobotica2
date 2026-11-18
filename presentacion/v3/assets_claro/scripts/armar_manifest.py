# Une renders/manifest_renders.json (escrito por tools/render_assets_claro.py) con los assets de Fusion,
# capturas, planos y logos, y escribe assets_claro/MANIFEST.json. Uso: python -I armar_manifest.py
import json, os, hashlib

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def info(rel):
    p = os.path.join(BASE, rel)
    if not os.path.exists(p):
        return {}
    d = {'bytes': os.path.getsize(p), 'sha256': hashlib.sha256(open(p, 'rb').read()).hexdigest()}
    if p.endswith('.png'):
        b = open(p, 'rb').read(24); d['resolucion'] = [int.from_bytes(b[16:20], 'big'), int.from_bytes(b[20:24], 'big')]
    return d


FUSION_DOC = 'Fusion · Admin Project · AutoFinalRobo2 (v5, abierto sin guardar cambios)'
PLACAS_DOC = 'Fusion · Admin Project · AFR2_Placas_Chasis_Plano (v1, documento aparte con copia de los cuerpos CapaSuperior/CapaInferior)'
DRAWING_DOC = 'Fusion · Admin Project · AFR2_Placas_Chasis_Plano Drawing (creado por DrawingManager, modo automático, ISO, mm, A3)'
BLEND = 'AutoFinalRobo2-CAD/mecanica/auto-completo/AutoFinalRobo2-Final-20261025.blend (instancia propia de Blender, sin guardar)'

extra = [
    {'archivo': 'planos/F07_plano_placas_v001.pdf', 'origen': {'fuente': DRAWING_DOC, 'derivado_de': PLACAS_DOC},
     'fondo': 'blanco', 'estado': 'hecho',
     'nota': 'PDF de 4 hojas A3 horizontales: 1 isométrica + lista de piezas, 2 ambas placas (planta y perfil), 3 CapaSuperior, 4 CapaInferior. '
             'Rótulo: Created by Koba Chajchir, 18/11/2026. Solo cotas envolventes: superior 199,73 × 154,64 mm, inferior 199,92 × 154,98 mm; '
             'espesor 3,0 mm medido en el modelo, sin cota en la hoja. PRELIMINAR: acotado funcional pendiente. '
             'ATENCIÓN: la lista de piezas muestra «Steel», material por defecto de Fusion, NO documentado; no usar como dato. '
             'La cota 339,44 de la hoja 2 abarca las dos placas colocadas una al lado de la otra (30 mm de separación) y no es una medida de pieza. '
             'La escala no figura en el rótulo automático.'},
    {'archivo': 'planos/F07_plano_placas_v001.png', 'origen': {'fuente': 'planos/F07_plano_placas_v001.pdf, hoja 3 (CapaSuperior)'},
     'fondo': 'blanco', 'estado': 'hecho', 'nota': 'Rasterizado a 300 ppp con PyMuPDF 1.24.10. Misma advertencia de material y escala que el PDF.'},
    {'archivo': 'planos/F07_plano_placa_inferior_v001.png', 'origen': {'fuente': 'planos/F07_plano_placas_v001.pdf, hoja 4 (CapaInferior)'},
     'fondo': 'blanco', 'estado': 'hecho', 'nota': 'Rasterizado a 300 ppp. Agregado: no estaba en la lista pedida.'},
    {'archivo': 'planos/F07_plano_placa_superior_planta_v001.png', 'origen': {'fuente': 'planos/F07_plano_placas_v001.pdf, hoja 3, recorte (385,240)-(800,595) pt'},
     'fondo': 'blanco', 'estado': 'hecho', 'nota': 'Solo la vista en planta, con su perfil y cotas envolventes, para poner junto al render F07_placa_superior_planta.'},
    {'archivo': 'capturas/F08_fusion_ensamble_v001.png', 'origen': {'fuente': FUSION_DOC},
     'camara': {'tipo': 'vista Fusion IsoTopRight + ajuste a pantalla (API)'}, 'fondo': 'interfaz de Fusion (tema oscuro)', 'estado': 'hecho',
     'nota': 'Captura pasiva de pantalla 1920×1080 (System.Drawing). El navegador ya estaba desplegado; se ven componentes vinculados (ícono de cadena) y el timeline. '
             'Tema oscuro: el cambio a tema claro requiere Preferencias (interfaz), y el control de la interfaz fue denegado.'},
    {'archivo': 'capturas/F08_fusion_joints_v001.png', 'estado': 'pendiente',
     'motivo': 'El ensamble AutoFinalRobo2 v5 tiene 0 joints y 0 as-built joints (verificado por API: rootComponent.allJoints vacío). '
               'Las piezas se ubican por transformaciones de ocurrencia (scripts fusion_ensamble.py / fusion_ruedas_20261024.py). No hay joint real que capturar.'},
    {'archivo': 'capturas/F08_fusion_exportar_v001.png', 'estado': 'pendiente',
     'motivo': 'Abrir el diálogo de exportación requiere controlar la interfaz de Fusion; el acceso fue denegado. Formato real verificado en exports/2026-10-25-final/parts: '
               '20 .usdz (part_00…part_18 y panel_root) + 2 .obj/.mtl (body_19 y body_20, las placas). La exportación se hizo por script (fusion_exportar_final.py), no por diálogo.'},
    {'archivo': 'capturas/F08_grabcad_modelos_v001.png', 'estado': 'pendiente',
     'motivo': 'Opcional y sin evidencia de GrabCAD: ningún componente del ensamble final está registrado como descargado de GrabCAD (ver procedencia_componentes).'},
    {'archivo': 'capturas/F09_blender_importado_v001.png', 'origen': {'fuente': BLEND, 'coleccion': 'AFR2_Final_Detallado_20261025'},
     'fondo': 'interfaz de Blender', 'estado': 'hecho',
     'nota': 'Overlay Statistics: 33 objetos, 490.526 triángulos. Capturada con bpy.ops.screen.screenshot desde una instancia aparte (scripts/blender_capturas.py); la sesión del usuario no se tocó.'},
    {'archivo': 'capturas/F09_blender_ligero_v001.png', 'origen': {'fuente': BLEND, 'coleccion': 'AFR2_Final_Web_20261025'},
     'fondo': 'interfaz de Blender', 'estado': 'hecho', 'nota': 'Overlay Statistics: 33 objetos, 105.620 triángulos.'},
    {'archivo': 'capturas/F09_blender_decimate_v001.png', 'origen': {'fuente': BLEND, 'objeto': 'FINAL_RAW_SensorLinea_5IR:1'},
     'fondo': 'interfaz de Blender', 'estado': 'hecho',
     'nota': 'El .blend guarda las mallas ya reducidas (sin modificador). El Decimate se RECONSTRUYÓ en memoria con el ratio real del script '
             '(blender_final_20261025.py:90, SensorLinea_5IR 0,12, Collapse + Triangulate) para mostrarlo; no se guardó. Face Count 17.092.'},
    {'archivo': 'logos/fusion360.png', 'estado': 'pendiente',
     'motivo': 'Solo hay .ico locales de hasta 256 px (webdeploy/production/6a0c9611…/Fusion360.ico: 16–256). Falta permiso del usuario para descargar el logo oficial.'},
    {'archivo': 'logos/blender.png', 'estado': 'pendiente',
     'motivo': 'La instalación local no trae PNG/SVG del logo; el ícono del .exe es de 256 px como máximo. Falta permiso del usuario para descargar el logo oficial (blender.org/about/logo).'},
]

procedencia = [
    {'componente': 'CapaSuperior / CapaInferior', 'fuente': 'Fusion AutoFinalRobo2 (usuario)', 'procedencia': 'propio del usuario; licencia no indicada'},
    {'componente': 'GM25-370 ×4', 'fuente': 'CADS/GM25-370/gm25-370.ipt', 'procedencia': 'aportado por el usuario; autor y licencia remotos sin registrar'},
    {'componente': 'Ruedas Mecanum (Rueda80_ManoA/B)', 'fuente': 'Fusion Wheel80mm / MecanumWheel80mm aportado por el usuario', 'procedencia': 'sin procedencia registrada (autor/licencia pendientes); bbox 109,3 mm'},
    {'componente': 'Mega2560_CAD', 'fuente': 'CADS/MEGA2560/arduinoMegaDetailed.STEP', 'procedencia': 'aportado por el usuario; autor y licencia pendientes'},
    {'componente': 'OSOYOOWifiShield + ESP_12_E', 'fuente': 'Fusion OSOYOOWifiShield armado por el usuario', 'procedencia': 'propio del usuario con piezas Fusion vinculadas (ESP_12_E, headers, LM1117, jst…) sin procedencia registrada'},
    {'componente': 'OSOYOOMotorDriver_ModelY_V2', 'fuente': 'generado desde CADS/MotorDriver/DriverDimensions.png y Driver.png', 'procedencia': 'propio (modelo esencial a partir de imágenes del fabricante)'},
    {'componente': 'Portabaterías 2×18650', 'fuente': 'CADS/18650BatteryHolder/18650 Battery Holder 2x v1.step', 'procedencia': 'aportado por el usuario + recorte propio (sin cable)'},
    {'componente': 'Celda 18650 ×2', 'fuente': 'CADS/18650/18650 Battery.STEP', 'procedencia': 'aportado por el usuario; autor y licencia pendientes'},
    {'componente': 'SensorLinea_5IR', 'fuente': 'CADS/5IRTracker/5 Channel IR Tracker.step', 'procedencia': 'aportado por el usuario; autor y licencia pendientes'},
    {'componente': 'SG90', 'fuente': 'CADS/SG90/SG90 - Micro Servo 9g - Tower Pro.STEP', 'procedencia': 'aportado por el usuario; autor y licencia pendientes'},
    {'componente': 'HC_SR04', 'fuente': 'CADS/HCSR04/HC-SR04 Sensor.stp', 'procedencia': 'aportado por el usuario; autor y licencia pendientes'},
    {'componente': 'Soportes, separadores, fijaciones, voltímetro, casquillos', 'fuente': 'scripts fusion_etapa2/3.py y manual M2', 'procedencia': 'propio (generado en el proyecto)'},
    {'componente': 'Panel UI (PCB + soporte)', 'fuente': 'KiCad PlacaOLED-Encoder-AutoRobo2 + soporte Fusion', 'procedencia': 'propio; OLED0.96CAD y KY040 de Fusion con fuente no identificada'},
]
nota_grabcad = ('No hay evidencia de GrabCAD para ningún componente del ensamble final (COMPONENTES_3D.csv y REVISION_CANDIDATOS_3D.md). '
                'El único enlace a GrabCAD registrado es un candidato JGA25-370 que no se descargó ni se usó. '
                'Los candidatos FreeCAD-library (CC-BY-3.0) e Innerscene (CC0) del 23/10 no son los modelos montados: el ensamble usa los CADS aportados por el usuario.')

r = json.load(open(os.path.join(BASE, 'renders', 'manifest_renders.json'), encoding='utf8'))
allm = r + extra
for m in allm:
    m.update(info(m['archivo']))
out = {'schema': 'afr2-assets-claro-1', 'fecha': '2026-11-18', 'script_renders': 'presentacion/v3/tools/render_assets_claro.py',
       'reglas': 'Sin imágenes generadas por IA; maestros sin guardar; todos los renders PNG transparentes recortados (pedido del usuario: sin fondo, sin sombra de contacto).',
       'assets': allm, 'procedencia_componentes': procedencia, 'grabcad': nota_grabcad, 'peso_modelo': 'peso_modelo.json'}
json.dump(out, open(os.path.join(BASE, 'MANIFEST.json'), 'w', encoding='utf8'), ensure_ascii=False, indent=1)
print('assets', len(allm), 'pendientes', sum(1 for m in allm if m.get('estado') == 'pendiente'))
