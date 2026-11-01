"""Inspeccion nativa solicitada, sin ejecutar computeAll ni cambiar la pose."""
import adsk.core,adsk.fusion,json
from pathlib import Path

def run(context):
    app=adsk.core.Application.get();d=adsk.fusion.Design.cast(app.activeProduct)
    if app.activeDocument.name!='AFR2_Brazo_v17_Base_V7_J3_Remoto':raise RuntimeError('Activar v17')
    out=dict(document=app.activeDocument.name,errors=[],warnings=[],rolled_back=[],suppressed=0,printed_multibody=[],parameters={})
    active={o.component.entityToken:o.component for o in d.rootComponent.allOccurrences}
    for c in active.values():
      for f in c.features:
        if f.isSuppressed:out['suppressed']+=1;continue
        state=f.healthState
        if state==adsk.fusion.FeatureHealthStates.HealthyFeatureHealthState:continue
        row=dict(component=c.name,feature=f.name,message=f.errorOrWarningMessage,state=int(state))
        if state==adsk.fusion.FeatureHealthStates.RolledBackFeatureHealthState:
          out['rolled_back'].append(row);continue
        out['errors' if state==adsk.fusion.FeatureHealthStates.ErrorFeatureHealthState else 'warnings'].append(row)
      a=c.attributes.itemByName('AFR2','material_v17')
      printed=bool((a and a.value.startswith('PLA')) or any(b.appearance and b.appearance.name in ['AFR2_PETG_Rojo','AFR2_PLA_PrintALot_Rojo'] for b in c.bRepBodies))
      if printed and c.bRepBodies.count:
        lumps=sum(b.lumps.count for b in c.bRepBodies if b.isSolid)
        if lumps!=1:out['printed_multibody'].append(dict(component=c.name,bodies=c.bRepBodies.count,lumps=lumps))
    for n in ['link1','link2','tube1_cut','tube2_cut','v17_pvc1_od','v17_pvc2_od','j3_centre','j3_motor_z','j3_belt_length','tower_h','gear_ratio']:
      p=d.userParameters.itemByName(n)
      if p:out['parameters'][n]=dict(expression=p.expression,value_internal=p.value)
    h=Path(globals().get('AFR2_REPO_ROOT',r'D:\ProyectoRobotica2'))/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17'
    (h/'historial_actual.json').write_text(json.dumps(out,indent=2),encoding='utf-8')
    print(json.dumps(dict(errors=len(out['errors']),warnings=len(out['warnings']),printed_multibody=out['printed_multibody'],parameters=out['parameters'])))
