"""Asentar cabezas y tuercas contra sus planos de apoyo, sin separaciones."""
from pathlib import Path
_workspace=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))
_source=_workspace/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v15/modelar_v15.py'
exec(compile(_source.read_text(encoding='utf-8'),str(_source),'exec'))
def bolt(parent,name,x,y,head_z,length):
    c=part(parent,name,'Acero')
    ring(c,'Vastago_M3_ENVOLVENTE','Z',f'({head_z})-({length})',x,y,'3 mm',None,length,'steel')
    ring(c,'Cabeza_M3','Z',head_z,x,y,'5.5 mm',None,'2 mm','steel');join_all(c,'Tornillo_una_pieza')
def nut(parent,name,x,y,z):
    c=part(parent,name,'Acero');s=c.sketches.add(datum(c,'Z',z,'Plano_tuerca'))
    rr=.55/math.sqrt(3);xx,yy=val(x),val(y)
    pts=[P(xx+rr*math.cos(k*math.pi/3),yy+rr*math.sin(k*math.pi/3),0) for k in range(6)]
    for k in range(6):s.sketchCurves.sketchLines.addByTwoPoints(pts[k],pts[(k+1)%6])
    f=extrude(c,s,'2.4 mm',OP.NewBodyFeatureOperation,'Tuerca_M3_AF5p5_ENVOLVENTE')
    for b in f.bodies:b.appearance=AP['steel']
    ring(c,'Rosca_M3_ENVOLVENTE','Z',f'({z})-0.1 mm',x,y,'3.2 mm',None,'2.6 mm',cut=True)
def run(context):
    setup()
    if adsk.core.Application.get().activeDocument.name!=MODEL_NAME:raise RuntimeError('Documento incorrecto')
    base=comp('S_Pinza_base');big=comp('J2_Carcasa_fija_5a1')
    for parent,prefix in [(base,('S_Anclaje_nuevo_','S_Housing_nuevo_')),(big,('J2_Fijacion_tapa_',))]:
        for o in list(parent.occurrences):
            if o.component.name.startswith(prefix):o.deleteMe()
    for yy in ['-20 mm','8 mm']:
        bolt(base,'M3x6_anclaje_'+yy,'20.5 mm',yy,'3.5 mm','6 mm')
        nut(base,'M3_anclaje_'+yy,'20.5 mm',yy,'-2.4 mm')
    for xx in ['-4 mm','6 mm']:
        bolt(base,'M3x8_housing_'+xx,xx,'28 mm','1.5 mm','8 mm')
        nut(base,'M3_housing_'+xx,xx,'28 mm','-5.4 mm')
    for xx in ['43.1 mm','81.1 mm']:
        bolt(big,'M3x8_tapa_'+xx,xx,'106.4 mm','-13.265 mm','8 mm')
        nut(big,'M3_tapa_'+xx,xx,'106.4 mm','-19.165 mm')
    # Completar metadatos de fijaciones antiguas hechas por la funcion comun.
    for c in D.allComponents:
        if c.bRepBodies.count and not c.attributes.itemByName('AFR2','material'):
            if all(b.appearance and 'Acero' in b.appearance.name for b in c.bRepBodies):c.attributes.add('AFR2','material','Acero')
    print('Cabezas, tuercas y longitudes comerciales asentadas; densidades de fijaciones explicitas.')
