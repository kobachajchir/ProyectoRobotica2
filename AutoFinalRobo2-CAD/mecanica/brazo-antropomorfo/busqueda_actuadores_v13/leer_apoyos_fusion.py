"""Solo lectura: superficies del auto de referencia, sin mover el modelo."""
import adsk.core,adsk.fusion,json

def run(_context: str):
    app=adsk.core.Application.get();des=adsk.fusion.Design.cast(app.activeProduct)
    rows=[];nodes=[];plate=[]
    for occ in des.rootComponent.allOccurrences:
        cn=occ.component.name
        if 'REF_AUTO_CHASIS' in cn:
            for body in occ.bRepBodies:
                bb=body.preciseBoundingBox
                plate.append(dict(body=body.name,bbox_mm=[v*10 for v in list(bb.minPoint.asArray())+list(bb.maxPoint.asArray())]))
        if 'REF_AUTO_Rueda80' not in cn:continue
        for body in occ.bRepBodies:
            calc=body.meshManager.createMeshCalculator();calc.surfaceTolerance=.002
            mesh=calc.calculate();xyz=mesh.nodeCoordinatesAsDouble
            points=[tuple(v*10 for v in xyz[i:i+3]) for i in range(0,len(xyz),3)]
            zmin=min(p[2] for p in points)
            low=[list(p) for p in points if p[2]<=zmin+.25]
            bb=body.preciseBoundingBox
            rows.append(dict(component=cn,body=body.name,nodes=len(points),configured_surface_tolerance_mm=.02,
                precise_bbox_mm=[v*10 for v in list(bb.minPoint.asArray())+list(bb.maxPoint.asArray())],
                mesh_bbox_mm=[min(p[j] for p in points) for j in range(3)]+[max(p[j] for p in points) for j in range(3)],
                bottom_points_mm=low))
    print(json.dumps(dict(document=app.activeDocument.name,isModified=app.activeDocument.isModified,plate=plate,wheels=rows)))
