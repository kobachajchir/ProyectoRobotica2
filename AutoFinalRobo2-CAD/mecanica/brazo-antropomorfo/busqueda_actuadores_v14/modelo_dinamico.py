"""Ecuaciones de cuerpos rigidos copiadas de v13, commit4b02f65.
No ejecuta perfiles anteriores ni modifica el maestro o los calculos v13.
"""
import math
import numpy as np

def tr(y=0,z=0):
    T=np.eye(4);T[:3,3]=[0,y,z];return T
def rot(axis,a):
    T=np.eye(4);c,s=math.cos(a),math.sin(a)
    T[:3,:3]=[[c,-s,0],[s,c,0],[0,0,1]] if axis=='Z' else [[c,0,s],[0,1,0],[-s,0,c]]
    return T
def frames(q,p):
    a=tr(p.get('axis_y_mm',-14)*.001,p['base_top']*.001)@rot('Z',q[0]);f={'00':np.eye(4),'01':a}
    for j,root,step,axis in [(1,'02',p['tower_h'],'Y'),(2,'03',p['link1'],'Y'),(3,'04',p['link2'],'Z'),(4,'05',p['wrist45'],'Y'),(5,'06',p['wrist56'],'Z')]:
        a=a@tr(z=step*.001)@rot(axis,q[j]);f[root]=a
    f['90']=a@tr(z=p['payload_z']*.001);return f
def aggregate(pp):
    groups={}
    for root in ['00','01','02','03','04','05','06','90']:
        parts=[x for x in pp if x['root']==root];m=sum(x['mass_g'] for x in parts)/1000
        cg=sum((x['mass_g']/1000*np.array(x['cg_local_mm'])*.001 for x in parts),np.zeros(3))/m
        I=np.zeros((3,3))
        for x in parts:
            mass=x['mass_g']/1000;xyz=np.array(x['box_mm'])*.001;r=np.array(x['cg_local_mm'])*.001-cg
            I+=np.array(x['tensor_kg_m2']) if 'tensor_kg_m2' in x else mass*np.diag([xyz[1]**2+xyz[2]**2,xyz[0]**2+xyz[2]**2,xyz[0]**2+xyz[1]**2])/12
            I+=mass*((r@r)*np.eye(3)-np.outer(r,r))
        groups[root]=(m,cg,I)
    return groups
def bodies(q,dq,ddq,p,groups):
    h=1e-4;f=frames(q,p);fp=frames(q+dq*h+.5*ddq*h*h,p);fm=frames(q-dq*h+.5*ddq*h*h,p)
    out={}
    for root,(m,cg,I) in groups.items():
        A,Ap,Am=f[root],fp[root],fm[root];R=A[:3,:3];x=R@cg+A[:3,3]
        acc=((Ap[:3,:3]-2*R+Am[:3,:3])@cg+Ap[:3,3]-2*A[:3,3]+Am[:3,3])/(h*h)
        Rd=(Ap[:3,:3]-Am[:3,:3])/(2*h);Rdd=(Ap[:3,:3]-2*R+Am[:3,:3])/(h*h)
        W=Rd@R.T;Al=Rdd@R.T+Rd@Rd.T
        w=np.array([W[2,1]-W[1,2],W[0,2]-W[2,0],W[1,0]-W[0,1]])/2
        alpha=np.array([Al[2,1]-Al[1,2],Al[0,2]-Al[2,0],Al[1,0]-Al[0,1]])/2
        F=m*(acc+[0,0,9.80665]);Iw=R@I@R.T;M=np.cross(x,F)+Iw@alpha+np.cross(w,Iw@w)
        out[root]=(F,M)
    return f,out
