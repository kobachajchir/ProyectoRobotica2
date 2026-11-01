"""Registrar esta entrega en los indices existentes sin alterar sus historicos."""
from pathlib import Path
root=Path(__file__).resolve().parents[4]
entries={
'INFORME_MAESTRO.md':'> **AFR2-20261029-02, banco mecanico v15:** [eslabones de aluminio, pinza y transmisiones](AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/README.md). Fusion independiente del maestro v09; masas desde geometria y revision CAD local muestreada de pinza. J2 modulo0.8 requiere otra iteracion para PLA. Sin brazo completo integrado ni capacidad fisica aprobada; 1.17kg de01 no es peso final actualizado.',
'AutoFinalRobo2-CAD/README.md':'> **AFR2-20261029-02:** [banco mecanico compacto v15](mecanica/brazo-antropomorfo/revision_v15/README.md), con eslabones de aluminio55/50mm, housings, pinza y reductor de estudio. Masas extraidas y revision CAD local de pinza; J2 modulo0.8 no aceptado para PLA, integracion y resistencia global pendientes. Maestro v09 conservado.',
'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/README.md':'> **AFR2-20261029-02:** [modulos mecanicos v15](revision_v15/README.md). Banco Fusion editable independiente: eslabones de aluminio, pinza corregida y transmision J2 de estudio. Masas reales de geometria y revision local muestreada; J2 necesita otra iteracion. No es ensamble final ni fabricacion aprobada; v09 conservado.',
'AutoFinalRobo2-Contracts/README.md':'> **AFR2-20261029-02:** [banco mecanico compacto v15](changes/2026-10-29-02-banco-brazo-compacto-v15.md). CAD nativo separado, eslabones de aluminio y pinza corregida; masas y revision geometrica local. Reductor J2 no aceptado para PLA e integracion global pendiente. atmega/esp/web/Loader sin cambios.',
'AutoFinalRobo2-Contracts/CHANGELOG.md':'## AFR2-20261029-02 - 29/10/2026\n\nBanco Fusion compacto v15: eslabones de aluminio55/50mm, pinza con bielas/pernos escalonados, retenciones y eje6mm justificado por flexion/torsion. Masas desde geometria y revision CAD local solicitada. J2 modulo0.8 requiere rediseño; sin integracion global ni aprobacion fisica. [Ficha](changes/2026-10-29-02-banco-brazo-compacto-v15.md).'
}
for name,prefix in entries.items():
    p=root/name;old=p.read_bytes()
    if b'AFR2-20261029-02' not in old[:1500]:p.write_bytes((prefix+'\n\n').encode('utf-8')+old)
