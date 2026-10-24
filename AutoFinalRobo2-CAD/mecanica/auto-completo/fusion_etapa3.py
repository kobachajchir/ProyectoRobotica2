"""Model Y V2.0 esencial y distribución según PDF M2, página 9.
Cotas de PCB/agujeros de DriverDimensions.png; alturas y componentes inferidos.
No genera ruedas ni modifica el chasis. Sin tests ni análisis automáticos.
"""
import sys
import os
import adsk.core
import adsk.fusion
sys.path.insert(0,os.path.dirname(__file__))
import fusion_ensamble as h

def run(_context: str):
    a,d,r=h.context()
    black=h.color('AFR2 Negro',(25,27,30))
    blue=h.color('AFR2 PCB Azul',(22,77,133))
    white=h.color('AFR2 Nylon',(224,224,216))
    silver=h.color('AFR2 Metal',(175,180,187))
    gold=h.color('AFR2 Laton',(184,142,57))
    red=h.color('AFR2 LED',(205,38,28))
    reverse=(-1,0,0,0,1,0,0,0,-1)
    h.place(h.find('Mega2560_CAD'),(25.5,32.5,29),reverse,'Mega a la derecha; USB al lateral derecho. PDF M2 p9')
    h.place(h.find('OSOYOOWifiShield'),(36.8,45,29),reverse,'Shield del usuario apilado sobre Mega. PDF M2 p10')
    if not any(o.component.name=='OSOYOOMotorDriver_ModelY_V2' for o in r.occurrences):
        board=h.box((0,0.8,0),(62,1.6,43))
        for x in (-28,28):
            for z in (-18.7,18.7):
                h.cut(board,h.cylinder((x,-1,z),(x,3,z),1.75))
        pieces=[('PCB_62x43_agujeros_3_5',board,blue)]
        # XH laterales: cuatro puertos de motores a cada lado.
        for x in (-27,27):
            for z in (-12.7,-4.23,4.23,12.7):
                body=h.box((x,5.1,z),(6.2,7,7.5))
                h.cut(body,h.box((x,7.0,z),(4.1,6,5.2)))
                pieces.append(('Conector_motor_XH',body,white))
                for dz in (-1.27,1.27):
                    pieces.append(('Pin_motor',h.box((x,4.3,z+dz),(0.6,5.4,0.6)),silver))
        for x in (-4.5,4.5):
            body=h.box((x,5.1,-17.2),(8,7,7.5))
            h.cut(body,h.box((x,7,-17.2),(5.8,6,5.2)))
            pieces.append(('Conector_alimentacion',body,white))
        for x in (-21,21):
            pieces.append(('Base_header_control',h.box((x,2.8,-18.5),(15.24,2.4,2.54)),black))
            for i in range(6):
                pieces.append(('Pin_control',h.box((x+(i-2.5)*2.54,5,-18.5),(0.64,8,0.64)),gold))
        for x in (-12,0,12):
            pieces.append(('Base_header_inferior',h.box((x,2.8,18.2),(10.16 if x else 7.62,2.4,2.54)),black))
            for i in range(4 if x else 3):
                pieces.append(('Pin_inferior',h.box((x+(i-(1.5 if x else 1))*2.54,5,18.2),(0.64,8,0.64)),gold))
        for x in (-5.2,5.2):
            pieces.append(('Capacitor',h.cylinder((x,1.6,4),(x,10,4),4),silver))
        for x in (-17,17):
            for z in (-3,8):
                pieces.append(('Circuito_potencia',h.box((x,2.4,z),(6,1.6,5)),black))
        pieces.append(('Regulador_central',h.box((0,3,-8),(8,2.8,8)),black))
        pieces.append(('Aleta_regulador',h.box((0,2.7,-13),(8,1,3)),silver))
        driver=h.part('OSOYOOMotorDriver_ModelY_V2',pieces,
            'Model Y V2.0 simplificado de CADS/MotorDriver/DriverDimensions.png y Driver.png. PCB 62x43 mm; centros 56x37.4 mm; agujeros 3.5 mm. Espesor 1.6 mm, alturas y paquetes aproximados. No esquema electrico ni PCB fabricable.')
    else:
        driver=h.find('OSOYOOMotorDriver_ModelY_V2')
    h.place(driver,(-48,32.5,29),(0,0,1,0,1,0,-1,0,0),'Model Y izquierdo: orientacion PDF M2 p9')
    # Sustituir apoyos preliminares de Mega por los de su posición final.
    supports=h.find('Separadores_Montaje').component
    for b in supports.bRepBodies:
        if b.name.startswith('Apoyo_Mega'): b.isLightBulbOn=False
    pieces=[]
    for x,z in [(-19,5),(-19,53),(71,5),(71,53),(-66.7,1),(-29.3,1),(-66.7,57),(-29.3,57)]:
        p=h.cylinder((x,26,z),(x,32.5,z),2.5)
        h.cut(p,h.cylinder((x,25,z),(x,34,z),1.5))
        pieces.append(('Separador_placas_6_5mm',p,white))
        pieces.append(('Cabeza_tornillo_M3',h.cylinder((x,34.1,z),(x,35.8,z),2.7),white))
    if not any(o.component.name=='Fijaciones_Placas' for o in r.occurrences):
        h.part('Fijaciones_Placas',pieces,'Separadores y cabezas simplificados; alturas inferidas y posiciones de Mega aproximadas. Model Y sigue centros de montaje 56x37.4 mm.')
    # Voltímetro: envolvente inferida de fotografía, no variante identificada.
    if not any(o.component.name=='Voltimetro_Referencia' for o in r.occurrences):
        pcb=h.box((0,0,0),(35,1.6,15))
        for x in (-15,15): h.cut(pcb,h.cylinder((x,-2,0),(x,2,0),1.5))
        pieces=[('PCB_voltimetro',pcb,blue),('Display',h.box((0,3.1,0),(25,4.6,12)),black)]
        for x in (-8,0,8):
            for z in (-3.7,0,3.7): pieces.append(('Segmento_LED',h.box((x,5.5,z),(4.4,0.2,0.7)),red))
            for dx in (-2.4,2.4):
                for z in (-1.9,1.9): pieces.append(('Segmento_LED',h.box((x+dx,5.5,z),(0.7,0.2,2.7)),red))
        vm=h.part('Voltimetro_Referencia',pieces,'Envolvente visual aproximada 35x15 mm inferida de foto del manual M2 p9. No modelo dimensional confirmado del voltimetro real.')
        h.place(vm,(-50,32.5,-32),role='Voltimetro delantero izquierdo, PDF M2 p9')
        h.capture()
    # Los colores se aplican solo a caras grandes del PCB integrado de la Mega.
    mega=h.find('Mega2560_CAD').component.occurrences.item(0).component
    for b in mega.bRepBodies:
        for f in b.faces:
            plane=adsk.core.Plane.cast(f.geometry)
            if plane and abs(plane.normal.y)>0.99 and f.area>12:
                f.appearance=blue
    h.capture()
    h.view()
    print('Guardado etapa 3:',h.save('Etapa 3: Model Y V2.0 esencial desde imagen acotada; Mega y shield a la derecha, driver a la izquierda, voltimetro de referencia. Ruedas 80 mm pendientes; sin cables ni simulacion.'))
