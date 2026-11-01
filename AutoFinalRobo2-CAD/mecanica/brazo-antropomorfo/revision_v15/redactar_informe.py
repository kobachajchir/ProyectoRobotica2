"""Informe del estado real del banco; leer JSON producidos tras las correcciones."""
from pathlib import Path
import json
H=Path(__file__).resolve().parent
mass=json.loads((H/'masas_modulos.json').read_text(encoding='utf-8'))
grip=json.loads((H/'pinza_simple_resultados.json').read_text(encoding='utf-8'))
review=json.loads((H/'revision_pinza.json').read_text(encoding='utf-8'))
text='''# v15: concretar montajes y corregir la pinza compacta

AFR2-20261029-02. **Banco de modulos nativo en Fusion; no es el brazo completo montado sobre el auto ni una entrega para fabricar.**

Se avanza desde el presupuesto v14 hacia piezas reales. El maestro v09 se
conserva y se trabaja en `AFR2_Compacto_v15_Modulos_mecanicos`, con croquis,
extrusiones, componentes, articulaciones y parametros editables. Las posiciones
del banco se fijan mediante uniones rigidas y parametros `banco_x*`; no son
despieces ni planos tecnicos finales, que siguen diferidos por indicacion humana.

## Material de los eslabones

Para distancias de55/50mm, el PVC dejaba tramos15/10mm y requeria encastres
adicionales. Se modelan dos mejillas de aluminio2mm por eslabon, ventanas,
bordes8mm, separadores tubulares y tornillosM3. Se elimina el PVC de estos
modulos de estudio. El calculo anterior NO acredita la resistencia de esta
seccion abierta nueva. Falta revisar torsion, flexion, asientos, fijaciones y
como se resolveran los radios y desarrollos de plegado.

Las bocas de eje/rodamiento son referencias parametricas; el extremo del
antebrazo debe adaptarse al cartucho de giroJ4 al integrar la muneca. No se
declara compatible con todas las articulaciones por tener un agujero estandar.

## Pinza simple y por que se descarta la simetrica

Una primera pinza simetrica concretada en CAD pesaba aproximadamente207g sin la pieza,
incluyendo soportes, engranajes, tambores y rodamientos. Se descarta: hacia
mas pesada la punta. Su registro esta en `masas_antecedente_simetrica.json`;
el historial de nube conserva esa geometria, que se retiro del banco vigente.
El primer registro204.3g se corrigio al asignar acero a cuatro tornillosM3
que habian quedado sin metadatos de material. La correccion no cambia el descarte.
La nueva pinza terminada en este banco pesa126.26g: es menor que la simetrica,
pero mayor que los aproximadamente113g de la pinza del maestro antiguo.
La mejora buscada tambien depende del menor brazo de palanca de la carga;
no se declara reduccion de masa respecto de aquella pinza antigua.

La nueva pinza tiene una mordaza fija con perfilU y otra movil sobre un
paralelogramo. Un STS3032 acciona directamente un tambor de radio4mm; no se
agrega reductor de pinza. La mordaza fija define X=-25mm; la movil queda en
X=+25 al agarrar y X=+35 al abrir60mm. El centro de la pieza permanece enX=0,
Z=45mm; la apertura libre de60mm queda descentrada5mm, que debe tenerse en
cuenta al acercarse a la pieza.

Las bielas se escalonan en planos laterales distintos. Los pernos superiores
y distales son independientes para no atravesar la otra biela. Se abre el
paso de la palanca en palma/anclaje, se traslada el puente de la mordaza al
lado opuesto del barrido y se separan fijaciones que se cruzaban. El eje que
acciona la palanca pasa a6mm, con planosD locales, diarios redondos y
retenciones fuera de los apoyos. La palanca se refuerza a4mm de aluminio.
Las caras de agarre se centran enZ45mm con la pieza50. Un tornilloM3 sin
cabeza, con tuerca capturada, limita el cierre cerca49.5mm: permite comprimir
los pads y evita la singularidad del paralelogramo a46mm. Su ajuste real,
resistencia y limitacion de corriente deben cerrarse antes de funcionamiento.

La cuna roja del mini y sus dos apoyos625 forman una pieza continua. Se
alojan las orejas reales del motor, con alivios, pasos completos, rebajes de
cabeza y tuercasM2.5. Los modelos de rosca son envolventes; no fabricar roscas
a partir de cilindros lisos. Los acoples25T tambien son referencias comerciales:
falta seleccionar el horn/acople real, no se aprueba un agujero liso como spline.

El cable esta representado por su tangente geometrica. **Cable, terminales y
muelle de apertura siguen pendientes como productos y piezas reales**. El
modelo relaciona angulo del tambor con longitud de tangente+arco, suponiendo
cable inextensible. No representa deformacion ni acredita apertura fisica.

## Masas extraidas de las piezas modeladas

Son volumen por densidad asumida, con sustitucion por masas de catalogo en
servos, rodamientos y seguros. No son pesos medidos y no incluyen el brazo
que aun falta modelar. Los CG del JSON estan en el sistema del banco, no
en el sistema del auto. Las densidades no acreditan temple ni material comprado.
El PLA se pesa como volumen solido, no mediante un perfil de laminado FDM.

| Modulo | Masa sin pieza g |
|---|---:|
'''
for name,x in mass['groups'].items():text+=f"| {name} | {x['mass_g']:.2f} |\n"
text+='''
No sumar estos modulos al presupuesto anterior sin sustituir las piezas que
reemplazan. Tampoco mantener1.17kg como masa final: las reservas de transmision
v14 eran optimistas y las geometrías concretas cambian los CG y las inercias.
Faltan controlador, bateria, base, J1/J3/J4/J5/J6 integrados, cableado, muelle,
terminales y herrajes definitivos. No hay nuevo cierre de vuelco ni traccion.

## Comprobacion acotada de apertura y colisiones

Se copian los solidos de la pinza y se aplican transformaciones temporales,
sin arrastrar el ensamble ni alterar el documento. Se incluyen piezas fijas,
bielas, mordaza, ejes, motor, rodamientos, tornillos, tuercas, seguros y cubo.
Se omiten pares de cuerpos del mismo componente comercial. Se reportan
intersecciones mayores de0.001mm3 y cualquier error del kernel.

| Apertura mm | Angulo biela grados | Tambor respecto de abierto grados | Interferencias reportables | Errores |
|---|---:|---:|---:|---:|
'''
for x in review['rows']:text+=f"| {x['gap_mm']:g} | {x['theta_deg']:.3f} | {x['motor_deg']:.3f} | {len(x['collisions'])} | {len(x['errors'])} |\n"
text+='''
Esto es una revision CAD muestreada de la pinza. No prueba el rango continuo,
tolerancias, flexion, carga fisica, cable completo o muneca. El barrido del
HC-SR04, el resto del brazo y la integracion al auto no estan comprobados en
esta entrega. No se traslada una ausencia de choques local al conjunto completo.

## Agarre y requisito de material

Se usa1.5kgfcm nominal a6V, tambor4mm, eficiencia0.8 supuesta y una reserva
equivalente de1N por cara para el muelle. Se calculan ambas tangencias del
cable para no escoger solo el brazo de palanca favorable. La resultante de
carga3.443N proviene de la trayectoria v14 con200g/30grados/s/50ms; debe volver
a comprobarse al cerrar toda la nueva arquitectura.

'''
text+=f"Normal minima calculada por cara: **{grip['minimum_normal_each_face_N']:.2f} N**, en el intervalo49.5..60mm. Para conservar x2 ante fuerzas en direccion arbitraria se requiere mu>={grip['mu_required_factor2']:.3f}; con mu=0.4 supuesto, el margen es{grip['holding_margin_at_mu04']:.2f}. Este filtro NO resuelve los momentos/contactos distribuidos de la pieza. No se conoce todavia el mu real ni la fuerza real del servo.\n\n"
u=grip['fixed_U']
text+=f"La mordaza fijaU, con E=70GPa supuesto y contacto pesimista a{u['effective_length_mm']:g}mm, da {u['bending_stress_MPa']:.2f}MPa nominales y {u['tip_deflection_mm']:.3f}mm de flecha. Con n2/Kt2 se requiere Rp>={u['required_proof_MPa_FSn2_Kt2']:.1f}MPa. Si se pretende cubrir tambien tres veces el par nominal como caso de bloqueo, el requisito sube a{u['required_proof_MPa_stall3x_FSn2_Kt2']:.1f}MPa. Son requisitos del producto certificado; no resistencia atribuida al aluminio disponible.\n\n"
text+='''
Faltan calculos de contactos/presion, uniones y fatiga de todas las piezas;
un limite elastico suficiente de la mordazaU no aprueba el mecanismo entero.
No se certifica acero por apariencia ni densidad. No se hizo FEA ni ensayo fisico.

### Eje de pinza: cambio justificado antes de modelar

Se acota el momento del cubo usando I=m*a^2/6 y una aceleracion angular
resultante <=6*alpha+15*omega^2, con30grados/s y frenada50ms. El desequilibrio
del pad, el momento del cubo y la fuerza a6mm de la mordaza se transmiten al
paralelogramo. Cerca del cierre sus bielas casi verticales amplifican cargas;
se incluye esa amplificacion, el cable y la fuerza de contacto en una cota
radial concentrada en el centro de una luz81mm. Para la seccionD se usa un
circulo inscrito conservador d_ef=d_nom-0.2-0.5mm, axial/corte y VonMises.
La entalla se trata preliminarmente conKt2 y se exige n2, no como FEA.
'''
shaft=grip['shaft_combined']
text+=f"\nCotas calculadas: radial{shaft['radial_bound_N']:.2f}N, flexion{shaft['bending_bound_Nm']:.3f}Nm y torsion{shaft['torque_bound_Nm']:.3f}Nm.\n\n| Diametro mm | Masa cilindro g | Rp requerido MPa con n2/Kt2 |\n|---:|---:|---:|\n"
for row in shaft['rows']:text+=f"| {row['diameter_mm']} | {row['mass_g']:.2f} | {row['required_Rp_MPa_FSn2_Kt2']:.1f} |\n"
text+='''
Se elige6mm y aceroRp>=600MPa certificado. Referencia posible: vastago liso
de bulon8.8, verificando diametro y longitud lisa antes de mecanizar planos y
ranuras sin alterar el temple. No se acepta la rosca como diario ni se ha
confirmado proveedor/medida comercial para este eje. Las ranurasE quedan
fuera de apoyos y zonas que transmiten par. Sus concentraciones reales,
contactos, presiones de llave y soportes PLA necesitan calculo posterior.
Los seguros son envolventes de productosDIN6799, no anillos para fabricar.

## Hombro: resultado que obliga a otra iteracion

Se modela un reductor5:1 con etapas20:40 y20:50, separadas axialmente para
que no se crucen las ruedas grandes. Incluye seis rodamientos, ejes, planosD,
espaciador entre ruedas, acople de referencia y retencion posterior del motor
con tapaL atornillada. Los diarios de los ejes quedan redondos. Es una
maqueta de empaquetado de modulo0.8, **descartada para uso resistente en PLA**.

El filtro Lewis preliminar, con fuerzas v14, n2 y factor1.5 impuesto, exige
aproximadamente50.6MPa FDM en el dentado de salida0.8: no hay datos de PLA
PrintaLot impreso que permitan aceptarlo. Modulo1.25 baja el requisito a20.7MPa,
pero eleva la rueda a65mm y cambia centros y masa. El conjunto de cuatro
ruedas macizas/aligeradas solo por ventanas pasaria a64.7g; con alma2mm y
aro/hub continuos se estima41.2g. Esa variante no esta modelada ni aprobada:
faltan rigidez del alma, contacto, raiz redondeada, tolerancias, fatiga y calor.

El bastidor impreso del primer reductor tambien resulta pesado; no basta
agrandar dientes. Debe redistribuirse el soporte hacia placas y apoyos mas
cortos y recalcularlo antes de integrar el brazo. Los bocetos de montaje no
autorizan declarar resistencia del reductor ni mantener los margenes globales
v14 sin incorporar sus nuevas masas. J3/J5 tienen requisitos preliminares
en `transmisiones_resultados.json`, no montajes terminados en este banco.

## Archivos, reproduccion y limites

- `AFR2_Compacto_v15_Modulos_mecanicos.f3d`: banco nativo editable exportado.
  Guardado y exportacion final confirmados:103 componentes,11 articulaciones,
  60 parametros de usuario. Se recupero el documento tras perder la conexion;
  se comprobo que conservaba eje6/pads nuevos antes de exportarlo.
- `modelar_v15.py`: crear documento nuevo y fases `init`, `links`, `j2`, `grip`,
  `keying`, `mounts`, `layout` mediante MCP. Cada fase de construccion una sola
  vez. No ejecutar `init` sobre una version que se quiera continuar.
- Despues ejecutar `corregir_pinza.py`, `pulir_pinza.py`, `retener_pinza.py`,
  `asentar_tornillos.py`, `aliviar_seguros.py` y `reforzar_eje_y_topes.py`
  una sola vez; `save` exporta y guarda. Las correcciones preservan el maestro
  anterior y dejan trazabilidad de los cambios.
- `extraer_masas.py` exporta volumen/CG. `revisar_pinza.py` hace solo la revision
  geometrica indicada y requiere autorizacion mecanica; no se ejecuta por leer
  este documento. Los scripts externos de calculo requieren Python y numpy.
- `pinza_simple_resultados.json`, `transmisiones_resultados.json`,
  `masas_modulos.json` y `revision_pinza.json`: evidencia con alcance explicito.

Los numeros de dientes permanecen fijados por componente; cambiarlos requiere
regenerar su perfil. Los centros del prototipoJ2 estan fijados para modulo0.8;
no cambiar solo el parametro a1.25 esperando que el conjunto se acomode.
En otro equipo configurar `AFR2_REPO_ROOT` antes de ejecutar los scripts en
Fusion. No toda inferencia de montaje queda automaticamente
resuelta al cambiar un motor. Antes de integrar se deben ligar centros,
interfaces y envolventes al producto definitivo y revisar sus limites.
No hay despieces ni laminas tecnicas finales, tests de software, compilacion,
flasheo ni aceptacion fisica. [Fuentes y dimensiones](FUENTES.md).
'''
(H/'README.md').write_text(text,encoding='utf-8')
