"""Etapa 2: alimentación y apoyos de referencia. Ejecutar dentro de Fusion.
No cambia las capas del chasis. No genera ruedas. No ejecuta validaciones.
"""
import sys
import os
import adsk.core
import adsk.fusion
sys.path.insert(0, os.path.dirname(__file__))
import fusion_ensamble as h

def run(_context: str):
    a,d,r = h.context()
    black = h.color('AFR2 Negro', (25,27,30))
    brass = h.color('AFR2 Laton', (184,142,57))
    silver = h.color('AFR2 Metal', (175,180,187))
    white = h.color('AFR2 Nylon', (224,224,216))
    mgr = adsk.fusion.TemporaryBRepManager.get()
    if not any(o.component.name == 'Portabaterias_SinCableExterior' for o in r.occurrences):
        source = h.find('Portabaterias_2x18650').component.occurrences.item(0).component.bRepBodies.item(0)
        body = mgr.copy(source)
        envelope = h.box((9.75,0,0),(40.6,20.2,76.2))
        if not mgr.booleanOperation(body,envelope,adsk.fusion.BooleanTypes.IntersectionBooleanType):
            raise RuntimeError('No se pudo recortar el cable exterior del portabaterias')
        holder = h.part('Portabaterias_SinCableExterior', [('Caja_y_contactos',body,black)],
            'Derivado CADS/18650BatteryHolder: recorte a envolvente del cuerpo para quitar cable exterior. Original intacto. Sin ajuste fisico comprobado.')
    else:
        holder = h.find('Portabaterias_SinCableExterior')
    h.find('Portabaterias_2x18650').isLightBulbOn = False
    h.place(holder,(0,36,81.75),(0,0,1,0,1,0,-1,0,0),'Alimentacion trasera')
    cell = h.find('Celda_18650')
    h.place(cell,(-32.55,36.5,81.75),(0,1,0,0,0,1,1,0,0),'18650_A')
    other = next((o for o in r.occurrences if o.attributes.itemByName('AFR2','role') and o.attributes.itemByName('AFR2','role').value=='18650_B'),None)
    if other is None:
        h.capture()
        other = r.occurrences.addExistingComponent(cell.component,h.matrix())
    h.place(other,(32.55,36.5,62.25),(0,-1,0,0,0,1,-1,0,0),'18650_B')
    h.place(h.find('SensorLinea_5IR'),(0,-40,-93),(1,0,0,0,0,1,0,-1,0),'Sensores opticos orientados al suelo')
    pieces=[]
    for x in (-66,66):
        for z in (-88,88):
            p=h.cylinder((x,3,z),(x,23,z),3)
            h.cut(p,h.cylinder((x,2,z),(x,24,z),1.55))
            pieces.append(('Separador_chasis_20mm',p,brass))
    for x in (-25,25):
        p=h.cylinder((x,-40,-93),(x,0,-93),2.5)
        h.cut(p,h.cylinder((x,-41,-93),(x,1,-93),1.55))
        pieces.append(('Separador_IR_M3_40mm',p,brass))
    # Apoyos de placas; posiciones inferidas del montaje, no taladros nuevos.
    for x,z in [(-48,1),(42,1),(-48,49),(42,49)]:
        pieces.append(('Apoyo_Mega_6_5mm',h.cylinder((x,26,z),(x,32.5,z),2.5),white))
    if not any(o.component.name == 'Separadores_Montaje' for o in r.occurrences):
        h.part('Separadores_Montaje',pieces,'Separadores simplificados; cuatro de chasis conservan 20 mm libres entre capas, IR 40 mm segun manual; ubicaciones de referencia inferidas.')
    pieces=[]
    for side in (-1,1):
        for z in (-62,62):
            wall=h.box((side*78,-14,z),(2,28,30))
            h.cut(wall,h.cylinder((side*75,-14,z),(side*81,-14,z),3.6))
            pieces.append(('Escuadra_motor',wall,silver))
            pieces.append(('Ala_fijacion_motor',h.box((side*67,-0.5,z),(22,1,34)),silver))
    if not any(o.component.name == 'Soportes_Motores_Referencia' for o in r.occurrences):
        h.part('Soportes_Motores_Referencia',pieces,'Escuadras simplificadas inferidas de manual; no modelo exacto del soporte ni plano de fabricacion.')
    pieces=[('Base_sobre_servo',h.box((-5.15,42.5,-77),(28,2,16)),black),
            ('Placa_ultrasonido',h.box((-5.15,58.5,-79),(51,30,2)),black)]
    for x in (-25.65,15.35):
        for y in (52.5,69.5):
            h.cut(pieces[1][1],h.cylinder((x,y,-81),(x,y,-77),0.8))
    if not any(o.component.name == 'Soporte_HCSR04_Referencia' for o in r.occurrences):
        h.part('Soporte_HCSR04_Referencia',pieces,'Referencia simplificada del soporte original del kit sobre SG90, derivada de manual pp.25-26. No es la pieza nueva OLED/encoder.')
    visible={'Mega2560_CAD','OSOYOOWifiShield','GM25_370','SensorLinea_5IR','SG90','HC_SR04','Portabaterias_SinCableExterior','Celda_18650','Separadores_Montaje','Soportes_Motores_Referencia','Soporte_HCSR04_Referencia'}
    for o in r.occurrences:
        o.isLightBulbOn=o.component.name in visible
    for b in r.bRepBodies:
        b.isLightBulbOn=True
    h.capture()
    h.view()
    print('Guardado etapa 2:',h.save('Etapa 2: portabaterias derivado sin cable exterior y dos celdas; IR hacia el suelo; apoyos de chasis, motores y ultrasonido simplificados. Ruedas 80 mm pendientes, sin generarlas.'))
