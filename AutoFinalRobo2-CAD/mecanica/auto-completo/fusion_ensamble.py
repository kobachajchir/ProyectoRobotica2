"""Autoría Fusion: parámetros en mm; sin tests ni análisis de interferencias.
Primitivas simplificadas de referencia, no diseños para fabricar.
"""
import adsk.core
import adsk.fusion

def context():
    a = adsk.core.Application.get()
    d = adsk.fusion.Design.cast(a.activeProduct)
    return a, d, d.rootComponent

def point(x, y, z):
    return adsk.core.Point3D.create(x/10, y/10, z/10)

def matrix(xyz=(0,0,0), rot=(1,0,0,0,1,0,0,0,1)):
    m = adsk.core.Matrix3D.create()
    m.setWithArray([rot[0],rot[1],rot[2],xyz[0]/10,
                    rot[3],rot[4],rot[5],xyz[1]/10,
                    rot[6],rot[7],rot[8],xyz[2]/10,0,0,0,1])
    return m

def find(name):
    return next(o for o in context()[2].occurrences if o.component.name == name)

def place(o, xyz, rot=(1,0,0,0,1,0,0,0,1), role=None):
    o.transform2 = matrix(xyz, rot)
    o.isLightBulbOn = True
    if role:
        o.attributes.add('AFR2','role',role)
    capture()

def capture():
    d = context()[1]
    if d.snapshots.hasPendingSnapshot:
        d.snapshots.add()

def color(name, rgb):
    d = context()[1]
    a = d.appearances.itemByName(name)
    if a:
        return a
    template = next(a for a in d.appearances if a.appearanceProperties.itemById('opaque_albedo'))
    a = d.appearances.addByCopy(template, name)
    p = adsk.core.ColorProperty.cast(a.appearanceProperties.itemById('opaque_albedo'))
    p.value = adsk.core.Color.create(*rgb,255)
    return a

def box(center, size):
    b = adsk.core.OrientedBoundingBox3D.create(point(*center),
        adsk.core.Vector3D.create(1,0,0),adsk.core.Vector3D.create(0,1,0),
        size[0]/10,size[1]/10,size[2]/10)
    return adsk.fusion.TemporaryBRepManager.get().createBox(b)

def cylinder(start, end, radius):
    return adsk.fusion.TemporaryBRepManager.get().createCylinderOrCone(
        point(*start),radius/10,point(*end),radius/10)

def cut(body, tool):
    if not adsk.fusion.TemporaryBRepManager.get().booleanOperation(
        body,tool,adsk.fusion.BooleanTypes.DifferenceBooleanType):
        raise RuntimeError('Recorte no realizado')
    return body

def part(name, pieces, description):
    a,d,r = context()
    if any(o.component.name == name for o in r.occurrences):
        raise RuntimeError('Componente existente: '+name)
    capture()
    o = r.occurrences.addNewComponent(matrix())
    c = o.component
    c.name = name
    c.description = description
    c.attributes.add('AFR2','change_id','AFR2-20261023-07')
    base = c.features.baseFeatures.add()
    base.name = name+'_Geometria'
    base.startEdit()
    for name_body, body, appearance in pieces:
        if not c.bRepBodies.add(body,base):
            raise RuntimeError('Cuerpo no creado: '+name_body)
    base.finishEdit()
    for b,(name_body,body,appearance) in zip(c.bRepBodies,pieces):
        b.name = name_body
        if appearance:
            b.appearance = appearance
    return o

def save(description):
    capture()
    return context()[0].activeDocument.save(description)

def view(front=True):
    a = context()[0]
    cam = a.activeViewport.camera
    cam.eye = point(280,240,-340 if front else 340)
    cam.target = point(0,10,0)
    cam.upVector = adsk.core.Vector3D.create(0,1,0)
    cam.isFitView = True
    a.activeViewport.camera = cam
