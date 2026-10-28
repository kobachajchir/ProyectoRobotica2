"""Sensibilidad adicional de velocidades/tiempos finitos, sin aprobar servicio."""
import json,itertools,math
import numpy as np
import calcular_v09 as c

parts,_=c.load();out=[]
for deg_s,tb in [(30,.2),(30,.1),(30,.05),(20,.2)]:
    peaks={}
    for signs in itertools.product([-1,1],repeat=6):
        end=np.radians([0,90,0,0,0,0]);speed=np.radians(signs)*deg_s
        for t in np.linspace(0,tb,21):
            s=t/tb;q=end-speed*tb/2+speed*tb*(s-s**3+.5*s**4)
            dq=speed*(1-3*s*s+2*s**3);ddq=speed*(-6*s+6*s*s)/tb
            for j in c.state_at(parts,q,dq,ddq):
                if j['joint'] not in peaks or j['servo_torque_kgfcm']>peaks[j['joint']]['servo_torque_kgfcm']:
                    peaks[j['joint']]=dict(j,signs=list(signs),time_s=float(t),q_deg=np.degrees(q).tolist())
    out.append(dict(speed_deg_s=deg_s,stop_s=tb,peak_deceleration_deg_s2=1.5*deg_s/tb,peaks=peaks))
result=dict(scope='64 signos, seis ejes, 21 instantes por caso, final horizontal J1=0. Sin FEA ni curvas de par o ensayo; no limita firmware.20gr/s/200ms es sensibilidad, no prestación aprobada.',scenarios=out)
(c.HERE/'envolvente_operativa.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps([dict(speed=x['speed_deg_s'],stop_ms=x['stop_s']*1000,peaks={k:round(v['servo_torque_kgfcm'],3) for k,v in x['peaks'].items()}) for x in out]))
