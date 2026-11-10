import numpy as np, json
from numpy import sin,cos,pi
def Rz(t):c,s=cos(t),sin(t);return np.array([[c,-s,0,0],[s,c,0,0],[0,0,1,0],[0,0,0,1.]])
def Ry(t):c,s=cos(t),sin(t);return np.array([[c,0,s,0],[0,1,0,0],[-s,0,c,0],[0,0,0,1.]])
def Rx(t):c,s=cos(t),sin(t);return np.array([[1,0,0,0],[0,c,-s,0],[0,s,c,0],[0,0,0,1.]])
def Tz(d):T=np.eye(4);T[2,3]=d;return T
def Tx(d):T=np.eye(4);T[0,3]=d;return T
h,L1,L2,d=0.209,0.070,0.115,0.1435
def fk_desc(q):
    q1,q2,q3,q4,q5,q6=q
    return Tz(.096)@Rz(q1)@Tz(.113)@Ry(q2)@Tz(.07)@Ry(q3)@Tz(.06)@Rz(q4)@Tz(.055)@Ry(q5)@Tz(.065)@Rz(q6)@Tz(.0785)
def fk_red(q):
    q1,q2,q3,q4,q5,q6=q
    return Tz(h)@Rz(q1)@Ry(q2)@Tz(L1)@Ry(q3)@Tz(L2)@Rz(q4)@Ry(q5)@Rz(q6)@Tz(d)
def A(th,dd,a,al): return Rz(th)@Tz(dd)@Tx(a)@Rx(al)
def fk_dh(q):
    q1,q2,q3,q4,q5,q6=q
    rows=[(q1,h,0,pi/2),(pi/2-q2,0,L1,0),(pi/2-q3,0,0,pi/2),(q4-pi,L2,0,-pi/2),(q5,0,0,pi/2),(q6,d,0,0)]
    T=np.eye(4)
    for r in rows:T=T@A(*r)
    return T
def ik(T,elbow=1):
    R=T[:3,:3];p=T[:3,3];pw=p-d*R[:,2]
    q1=np.arctan2(pw[1],pw[0]);rho=np.hypot(pw[0],pw[1]);s=pw[2]-h
    c3=(rho**2+s**2-L1**2-L2**2)/(2*L1*L2);q3=elbow*np.arccos(np.clip(c3,-1,1))
    q2=np.arctan2(rho,s)-np.arctan2(L2*sin(q3),L1+L2*cos(q3))
    Rw=(Rz(q1)@Ry(q2+q3))[:3,:3].T@R
    q5=np.arctan2(np.hypot(Rw[0,2],Rw[1,2]),Rw[2,2]);q4=np.arctan2(Rw[1,2],Rw[0,2]);q6=np.arctan2(Rw[2,1],-Rw[2,0])
    return np.array([q1,q2,q3,q4,q5,q6])
rng=np.random.default_rng(7)
lim=np.radians([[-90,90],[-10,125],[-90,90],[-90,90],[-90,90],[-90,90]])
e_red=e_dh=e_ik=0;n=0;nik=0;e_ikq=0
for k in range(20000):
    q=rng.uniform(lim[:,0],lim[:,1])
    T=fk_desc(q)
    e_red=max(e_red,np.abs(T-fk_red(q)).max())
    Tdh=fk_dh(q);e_dh=max(e_dh,np.abs(T[:3,3]-Tdh[:3,3]).max())
    # orientation: DH final frame may differ by constant rotation; compare z axis
    if abs(sin(q[4]))>0.05 and q[0]!=0:
        best=1e9
        for el in (1,-1):
            qi=ik(T,el);Ti=fk_desc(qi);err=max(np.abs(Ti-T).max(),0);best=min(best,err)
        e_ik=max(e_ik,best);nik+=1
print('max |FKdesc-FKred|',e_red);print('max pos |FKdesc-FKdh|',e_dh)
# orientation of DH vs desc constant offset?
q=rng.uniform(lim[:,0],lim[:,1]);Rd=fk_desc(q)[:3,:3];Rh=fk_dh(q)[:3,:3];print('Rdesc^T Rdh=\n',np.round(Rd.T@Rh,6))
q=rng.uniform(lim[:,0],lim[:,1]);Rd=fk_desc(q)[:3,:3];Rh=fk_dh(q)[:3,:3];print('Rdesc^T Rdh=\n',np.round(Rd.T@Rh,6))
print('IK roundtrip max err (pose)',e_ik,'over',nik)
# key poses
for name,qd in [('cero',[0,0,0,0,0,0]),('transporte',[0,15,-40,0,25,0]),('horizontal',[0,90,0,0,0,0]),('horiz_codo90',[0,90,-90,0,0,0])]:
    T=fk_desc(np.radians(qd));print(name,np.round(T[:3,3]*1000,2), 'z_tool',np.round(T[:3,2],3))
# Jacobian at transport and singular values
def jac(q):
    q1,q2,q3,q4,q5,q6=q
    Ts=[Tz(.096)@Rz(q1)]
    frames=[];T=Tz(.096);axes=[]
    seq=[('z',q1,.113),('y',q2,.07),('y',q3,.06),('z',q4,.055),('y',q5,.065),('z',q6,.0785)]
    for ax,qq,off in seq:
        R=Rz(qq) if ax=='z' else Ry(qq)
        axv=T[:3,2] if ax=='z' else T[:3,1];o=T[:3,3].copy();axes.append((axv,o))
        T=T@R@Tz(off)
    p=T[:3,3];J=np.zeros((6,6))
    for i,(z,o) in enumerate(axes):J[:3,i]=np.cross(z,p-o);J[3:,i]=z
    return J
for name,qd in [('transporte',[0,15,-40,0,25,0]),('generica',[20,60,-50,30,40,10]),('codo_alineado',[0,60,0,30,40,10]),('muneca_sing',[0,60,-50,30,0,10])]:
    sv=np.linalg.svd(jac(np.radians(qd)),compute_uv=False);print(name,'sv',np.round(sv,4),'cond',np.round(sv[0]/sv[-1],1))
# jacobian numeric check
q=np.radians([20,60,-50,30,40,10]);J=jac(q);eps=1e-7;Jn=np.zeros(6)
for i in range(6):
    dq=np.zeros(6);dq[i]=eps;Jn=(fk_desc(q+dq)[:3,3]-fk_desc(q)[:3,3])/eps;print('col',i,'err',np.abs(Jn-J[:3,i]).max())
# reach
print('alcance max desde J2 [mm]',(L1+L2+d)*1000,' altura TCP en cero',(h+L1+L2+d)*1000)
# mecanum
L=0.170;Am=np.array([[1,-1,-L],[1,1,L],[1,1,-L],[1,-1,L]]);Ap=np.linalg.pinv(Am);print('A+ =\n',np.round(Ap,5));print('A+A',np.round(Ap@Am,10))
r=0.04
for name,V in [('avance 0,15',[0.15,0,0]),('lateral 0,15',[0,0.15,0]),('giro 0,5 rad/s',[0,0,0.5]),('diag',[0.1,0.1,0])]:
    u=-1*(Am@np.array(V));print(name,'u mm/s',np.round(u*1000,1),'w rad/s',np.round(u/r,3),'rpm',np.round(u/r*60/2/pi,1))
print('res lineal mm/cuenta',pi*80/370)
print('vmax ideal rated 100rpm m/s',100*2*pi/60*0.04,'no-load',150*2*pi/60*0.04)
print('ultra c20',331.3+0.606*20,'dmm 25us',(331.3+0.606*20)*25e-6/2*1000)
