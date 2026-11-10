"""Tessela los STEP de KiCad aportados por el usuario; no ejecuta comprobaciones CAD.
Ejecutar con el Python incluido en FreeCAD para disponer de Part/OpenCascade.
Las coordenadas intermedias están en milímetros, como los STEP de biblioteca.
"""
import json, hashlib
from pathlib import Path
import FreeCAD as App
import Part

ROOT = Path('D:/ProyectoRobotica2')
LIBRARY = Path('C:/Program Files/KiCad/10.99/share/kicad/3dmodels')
OUTPUT = ROOT / 'AutoFinalRobo2-CAD/exports/20261109-sensor-bateria'
OUTPUT.mkdir(parents=True, exist_ok=True)
FILES = {
    'resistor': 'Resistor_THT.3dshapes/R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal.step',
    'capacitor': 'Capacitor_THT.3dshapes/CP_Radial_Tantal_D4.5mm_P2.50mm.step',
    'pin': 'Connector_PinHeader_1.00mm.3dshapes/PinHeader_1x01_P1.00mm_Horizontal.step',
}
result = {'units': 'mm', 'linear_deflection_mm': .18, 'parts': {}}
for key, relative in FILES.items():
    path = LIBRARY / relative
    shape = Part.read(str(path))
    solids = list(shape.Solids) or [shape]
    meshes = []
    for solid in solids:
        vertices, faces = solid.tessellate(.18)
        meshes.append({'vertices': [[v.x, v.y, v.z] for v in vertices], 'faces': faces,
                       'volume_mm3': solid.Volume, 'bounds_mm': [solid.BoundBox.XMin, solid.BoundBox.YMin, solid.BoundBox.ZMin, solid.BoundBox.XMax, solid.BoundBox.YMax, solid.BoundBox.ZMax]})
    result['parts'][key] = {'source': '${KICAD10_3DMODEL_DIR}/' + relative,
                            'resolved_path': str(path), 'sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
                            'meshes': meshes}
(OUTPUT / 'step_meshes.json').write_text(json.dumps(result), encoding='utf-8')
print(json.dumps({key: {'solids': len(part['meshes']), 'triangles': sum(len(m['faces']) for m in part['meshes']), 'bounds': [m['bounds_mm'] for m in part['meshes']]} for key, part in result['parts'].items()}))
