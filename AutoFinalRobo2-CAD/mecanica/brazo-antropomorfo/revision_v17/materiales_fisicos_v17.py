"""Sincronizacion nativa de densidades para el calculo de masas solicitado.

Materiales equivalentes de productos comerciales NO sirven para FEA.
El material PLA existente declara E1500 hipotetico, sin resistencia FDM.
"""
import hashlib


def act17_sts_materiales_base():
    data=HERE17/'geometria_v17.json'
    source_hash=hashlib.sha256(data.read_bytes()).hexdigest()
    loads=json.loads((HERE17/'cargas_cad_actuales.json').read_text(encoding='utf-8'))
    if source_hash!=loads['geometry_sha256']:raise RuntimeError('Reextraer y recalcular masas antes de asignar densidades')
    rows=json.loads((HERE17/'masas_cad_actuales.json').read_text(encoding='utf-8'))
    specs={}
    for row in rows:
      if row['volume_cm3']<=0:raise RuntimeError('La masa nativa no debe incluir reservas no modeladas')
      spec=(row['material'],row['mass_g']/row['volume_cm3'])
      old=specs.get(row['component'])
      if old and (old[0]!=spec[0] or abs(old[1]-spec[1])>1e-6):raise RuntimeError('Densidad no uniforme en componente: '+row['component'])
      specs[row['component']]=spec
    steel=D.materials.itemByName('Steel')
    pla=next(m for m in D.materials if m.name.startswith('AFR2 PLA PrintaLot rho1.24'))
    aluminum=adsk.core.Application.get().materialLibraries.itemByName('Fusion Material Library').materials.itemByName('Aluminum 6061')
    if not aluminum:raise RuntimeError('Falta plantilla de aluminio6061 de biblioteca local')
    active={o.component.entityToken:o.component for o in R.allOccurrences if not o.fullPathName.split('+')[0].startswith(('80_','81_','82_'))}
    result=[]
    for index,c in enumerate(active.values()):
      if not c.bRepBodies.count:continue
      if c.name not in specs:raise RuntimeError('Componente sin densidad calculada: '+c.name)
      kind,rho=specs[c.name]
      if kind=='PLA PrintALot':material=pla
      elif kind=='Acero':material=steel
      else:
        if kind=='Aluminio':name='AFR2 Al6061 rho2.70 HIPOTESIS sin certificado';template=aluminum
        elif kind=='Aluminio2024':name='AFR2 Al2024 SOLO DENSIDAD2.78 SIN FEA';template=aluminum
        elif kind=='PVC':name='AFR2 PVC Tigre SOLO DENSIDAD1.45 SIN FEA';template=pla
        elif kind=='Almohadilla':name='AFR2 Almohadilla SOLO DENSIDAD1.10 SIN FEA';template=pla
        elif kind=='Laton':name='AFR2 Laton SOLO DENSIDAD8.50 SIN FEA';template=steel
        else:name='AFR2 EQ '+c.name[:65]+' SOLO MASA NO FEA';template=steel
        material=D.materials.itemByName(name)
        if not material:material=D.materials.addByCopy(template,name)
        properties=[p for p in material.materialProperties if 'density' in p.id.lower()]
        if not properties:raise RuntimeError('Material sin densidad editable '+name)
        for prop in properties:
          if getattr(prop,'units',None)!='KilogramPerCubicMeter':raise RuntimeError('Unidad de densidad desconocida '+prop.id)
          if abs(prop.value-rho*1000)>1e-7:prop.value=rho*1000
      for body in c.bRepBodies:
        if body.material and body.material.name==material.name:continue
        appearance=body.appearance;body.material=material
        if appearance:body.appearance=appearance
      c.attributes.add('AFR2','densidad_nativa_v17',f'{rho:.12g}g/cm3. Masa coherente con calculo externo. Productos comerciales equivalentes y materiales SOLO DENSIDAD excluidos de estudios resistentes.')
      result.append(dict(component=c.name,kind=kind,density_g_cm3=rho,material=material.name))
      if index%10==0:progress('materiales '+str(index)+' '+c.name)
    mass=0
    for o in R.allOccurrences:
      if o.fullPathName.split('+')[0].startswith(('80_','81_','82_')):continue
      for body in o.component.bRepBodies:
        if body.isSolid:mass+=body.getPhysicalProperties(adsk.fusion.CalculationAccuracy.HighCalculationAccuracy).mass*1000
    expected=sum(row['mass_g'] for row in rows)
    if abs(mass-expected)>.02:raise RuntimeError('Masa Fusion distinta del calculo '+str((mass,expected)))
    (HERE17/'materiales_fisicos_asignados.json').write_text(json.dumps(dict(source_geometry_sha256=source_hash,mass_with_payload_g=mass,expected_with_payload_g=expected,components=result,scope='Densidades/massas. NO aprobacion FEA/FDM. Las densidades equivalentes comerciales deben actualizarse si se cambia su envolvente.'),indent=2),encoding='utf-8')
    print(json.dumps(dict(materials_assigned=len(result),native_mass_with_payload_g=mass,expected_with_payload_g=expected)))


def act17_sts_materiales_final():act17_sts_materiales_base()
