"""Promueve el GLB y descriptor del rig; no compila ni modifica ESP/dist."""
import json,math,shutil,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[5]
HERE=Path(__file__).resolve().parent
WEB=ROOT/'AutoFinalRobo2-Web'
rig=json.loads((HERE/'rig_v17.json').read_text(encoding='utf-8'))
delivery=json.loads((HERE/'entrega_glb.json').read_text(encoding='utf-8'))
source=HERE/'AFR2_Brazo_v17_Blanco.glb'
if source.stat().st_size>600000:raise RuntimeError('No promover: excede600000bytes')
params=rig['parameters']
def clean(value):
 if isinstance(value,float):return round(value,6)
 if isinstance(value,list):return [clean(v) for v in value]
 if isinstance(value,dict):return {k:clean(v) for k,v in value.items()}
 return value
def identity(p=None):return dict(position=p or [0,0,0],quaternion=[0,0,0,1])
joints=[]
for i,(label,limits,radius) in enumerate(zip(['Base','Hombro','Codo','Giro de muneca','Inclinacion','Herramienta'],[[-90,90],[-10,125],[-90,90],[-90,90],[-90,90],[-90,90]],[.018,.0125,.01,.012,.012,.012])):
 joints.append(dict(id='j'+str(i+1),label='J'+str(i+1)+' · '+label,parent='j'+str(i) if i else None,axis=[0,0,1] if i in [0,3,5] else [0,1,0],origin=clean(rig['jointOrigins'][i]),sign=1,zero=0,min=limits[0],max=limits[1],radius=radius,visualNode='j'+str(i+1)))
# CAD car -> arm = (X,-Z-32mm,Y-26mm). The car GLB adds 53.051345mm
# of floor offset, then useRobotModel recentres its bounds, including the OLED.
# Asset-frame evidence: revision_v17/alineacion_web_20261031.json.
car_center_z=.00981888175
deck_height=.026+.05305134505-.00001076514
mount=dict(position=[round(params['v05_axis_y']*.01+.032+car_center_z,6),0,round(deck_height,6)],quaternion=[0,0,-math.sqrt(.5),math.sqrt(.5)])
cad_frame=dict(position=[round(.032+car_center_z,6),0,round(deck_height,6)],quaternion=mount['quaternion'])
model=dict(id='pvc25-v17-blanco',revision='20261031-v17-white',label='PVC25/20 V17 · PLA blanco',units='m',mountFrame='afr2-arm-mount-v17',tcpFrame='afr2-grasp-center-v1',renderBasis=[-.5,.5,.5,.5],cadToRobot=cad_frame,mounts=dict(standalone=identity([0,0,.018]),mounted=mount),joints=joints,tcp=identity([0,0,params['tcp_offset']*.01]),
 gripper=dict(min=params['grip_min']*10,max=params['grip_min']*10+2*params['grip_r']*10*math.pi,open=rig['restOpening'],palmSize=[.118,.06,.016],palmZ=.025,fingerSize=[params['finger_t']*.01,.03,.05],fingerZ=.056,padSize=[params['pad_t']*.01,.032,.024],padZ=.068,travelAxis=[1,0,0],visualNodes=['jaw-left','jaw-right'],metersPerOpeningMm=.0005),
 visual=dict(kind='gltf',url='/models/AFR2_Brazo_v17_Blanco.glb',restToMount=rig['restToMount'],restAngles=rig['restAngles'],restOpening=rig['restOpening'],baseSize=[.172,.063,.028],baseZ=.014,followers=clean(rig['followers'])),
 collision=dict(towerRadius=.035,boxes=[dict(id='chassis',center=[0,0,.052],size=[.278,.232,.08])],allowedPairs=[['j4','j6'],['j5','tcp']]),initial=[0,15,-40,0,25,0],transport=[0,15,-40,0,25,0],seeds=[[0,45,-60,0,90,0],[0,80,30,0,60,0],[60,20,-60,0,40,0],[-60,20,-60,0,40,0]],speedDegS=30,downward='zyyzyz',
 source='Fusion v17 abierto, exportacion por solidos 2026-10-31; rig_v17.json. Solo visual/rig; colision Web aproximada, sin calculos resistentes ni prueba fisica.')
folder=WEB/'src/features/collector/models';folder.mkdir(exist_ok=True)
(folder/'pvc25-v17-blanco.json').write_text(json.dumps(model,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
target=WEB/'public/models/AFR2_Brazo_v17_Blanco.glb';shutil.copy2(source,target)
report=dict(bytes=source.stat().st_size,sha256=hashlib.sha256(source.read_bytes()).hexdigest(),web_path=str(target.relative_to(ROOT)),descriptor=str((folder/'pvc25-v17-blanco.json').relative_to(ROOT)),triangles=delivery['triangles'],materials=delivery['materials'],excluded=delivery['excluded'],build=False,esp_promoted=False)
(HERE/'promocion_web.json').write_text(json.dumps(report,indent=2),encoding='utf-8');print(json.dumps(report))
