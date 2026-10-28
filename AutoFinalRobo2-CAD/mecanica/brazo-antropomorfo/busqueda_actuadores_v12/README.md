# Busqueda v12:30grados/s, par nominal e inercia interna

AFR2-20261028-08. Objetivo activo. Calculos mecanicos pedidos por el usuario; no nuevo CAD, tests de software, FEA, ensayos ni firmware. Fusion v09 conservado.

## Conclusion de la iteracion

La alternativa STS3215/reductores de v11 no cumple30grados/s en hombro/codo; no se usa como exito del objetivo. Se mantiene la carga200g, los seis ejes a30grados/s y frenada50ms. Tampoco se presenta30grados/s como velocidad maxima de catalogo: es el limite de operacion analizado.

Se encontro una combinacion RS01+STS3215 que supera el filtro nominal de par y el efecto de la inercia publicada de los RS01. Sin embargo, pesa5.45-5.82kg incluyendo disipadores y reservas, exige mas resistencia de la cuna y aun no acredita estabilidad del auto. Por ello NO es una seleccion final ni se justifica modelarla como solucion aprobada.

La iteracion anterior fue progreso: se publicaron nuevas cargas, fuentes y restricciones. Esta iteracion reduce incertidumbre sobre velocidad/par y muestra por que resolver solo el par puede trasladar el problema a la masa y al apoyo.

## STS3250: mejor potencia especifica, pero insuficiente en este presupuesto

Ficha FEETECH de2024-01-16,STS3250:12V,74.5+-1g,par nominal16kgfcm, bloqueo50kgfcm+-10%,450grados/s sin carga+-10%, dimensiones45.22x24.72x35mm. El grafico publicado de velocidad es lineal aproximadamente75rpm a0kgfcm y0rpm a50kgfcm. El STS3215/12V se mantiene en J4/J5/J6; se estudiaron MG92B y STS3215 para pinza.

Se presupuestan80g por cada motor de los seis ejes,125/175/175/50/75/50g para reductores completos,200g de potencia y las piezas nativas v09. Es un presupuesto previo al CAD, no una caja comercial seleccionada. Masa3369g con MG92B o3435g con STS3215 de pinza.

Doce combinaciones: reducciones9/10/11 en J1-J3,4/6/4 en J4-J6,eficiencias0.70/0.85 y dos pinzas. Se calcularon5502 estados fuente de262 trayectorias de frenada50ms. La sensibilidad de par-velocidad usa simultaneamente bloqueo inferior45kgfcm y velocidad inferior405grados/s para3250; no sustituye curva de frenado medida.

| Eta | Pinza | Mejor relacion continua J2 | Mayor margen minimo alcanzable en este modelo |
|---|---|---:|---:|
|0.70|MG92B|10.44|1.462|
|0.70|STS3215|10.46|1.368|
|0.85|MG92B|10.44|1.776|
|0.85|STS3215|10.46|1.661|

Con eta0.85 y pinza ligera, J2 alcanza79.96kgfcm en salida. Aumentar relacion hace que el motor deba girar mas rapido para mantener30grados/s; al optimizar ese compromiso, el margen maximo queda1.776, inferior a2. Esto descarta ESTE presupuesto y modelo de curva, no todos los posibles brazos, masas o transmisiones. Todavia no incluye una inercia interna STS acreditada; sumarla no permite asumir mejor cumplimiento.

### Oferta comercial comprobada

ArmDojo ofrece variantes STS3250-C002 servo solo y C001 con soporte metalico. El endpoint publico de producto informo ambas disponibles; el selector incluye Argentina y la politica declara envios internacionales desdeSingapur. Es una oferta de importacion para Argentina; no stock argentino ni un envio/costo final confirmado. El soporte comercial no se duplica con los housings presupuestados. No se usan180g de empaque como peso neto del motor.

## RS01: usar la ficha vigente, no la tabla indexada anterior

La API publica del repositorio oficialRobStride mostro el archivo RS de nombre2026.010.09 y manuales reubicados bajoProduct Literature/CAN2.0. La tabla indexada2026-07-13 daba380g paraRS01; la ficha recuperada especifica395+-3g y equivalente de inercia de baja velocidad0.0042kgm2. Se usa398g y esa inercia, con sensibilidad adicionalx2. No se mezclan los pesos de la ficha vieja con el par de la nueva.

ParaRS01 la ficha da7Nm a100rpm con disipador de aluminio150x150mm y, en la curva de carga en reposo,6Nm como condicionRated. Se limita a6Nm el filtro de este estudio, mas conservador que7Nm. Alimentacion nominal36V, rango15-48V en esta revision. Los puntos de curva36V0.5Nm/305rpm,6Nm/265rpm y17Nm/135rpm quedan muy por encima de los5/30/15rpm exigidos por las reducciones3/6/3 a30grados/s. Eso prueba holgura de velocidad de motorizacion bajo esas condiciones, no frenado regenerativo.

La ficha solo identifica150x150mm en la tabla termica; no se recupero un espesor de ensayo. Se estudian3mm y5mm como HIPOTESIS de masa:182.25g o303.75g por disipador, tres unidades. Un espesor elegido por nosotros no acredita equivalencia termica con el ensayo del fabricante.

### Combinacion de estudio

| Eje | Actuador | Reduccion externa | Presupuesto caja/housing/ejes/rodamientos |
|---|---|---:|---:|
|J1|RS01|3:1|150g|
|J2|RS01|6:1|250g|
|J3|RS01|3:1|250g|
|J4|STS3215/12V|1:1|30g|
|J5|STS3215/12V|4:1|75g|
|J6|STS3215/12V|1:1|30g|

PinzaMG92B posicional; su friccion/vida y par de servicio permanecen abiertos. Potencia/control36V con conversion a12V:reserva650g SIN bateria,BMS,reguladores,clamp regenerativo ni placas finales seleccionados. Los seis movimientos se calculan a30grados/s, conservando finales y recorridos de referencia; no hay nuevos choques/recorridos demostrados sobre cajas que aun no existen.

Se usa masa80g para cadaSTS3215 de los ejes distales. LosRS01 se representan por cajas80x80x45mm para tensor/CG preliminar; estas no son cotas de montaje certificadas. Los disipadores se ubican sobre el soporte padre,40mm lateralmente respecto al CG de motor. Su disposicion fisica y compatibilidad con el auto deben demostrarse despues.

### Inercia propia y reaccion sobre los padres

El inventario v09 aportaba inercias del conjunto exterior. Eso no incluye automaticamente el rotor y engranajes girando internamente. ParaRS01 se agrega un equivalente axial a la salida de0.0042kgm2 o el doble, con velocidad relativa n*dq. Se incorpora su momento angular a los soportes padres y su par de aceleracion al motor. No se divide otra vez por7.75:1, porque la ficha ya refiere la inercia al extremo lento.

Es un modelo equivalente axial, no una descomposicion interna de masas ni una cota de inercia transversal. La distribucion real y los contactos requieren mas datos. LosSTS3215 todavia no tienen inercia equivalente publicada recuperada; esa falta no queda aprobada por pasar el filtro.

| Caso | Masa (g) | Menor capacidad/demanda | Filtro nominal x2 |
|---|---:|---:|---|
|RS01_plate3_eta0.7_I1|5454.8|2.407|Pasa, condicionado|
|RS01_plate3_eta0.7_I2|5454.8|2.033|Pasa, condicionado|
|RS01_plate3_eta0.85_I1|5454.8|2.827|Pasa, condicionado|
|RS01_plate3_eta0.85_I2|5454.8|2.335|Pasa, condicionado|
|RS01_plate5_eta0.7_I1|5819.3|2.364|Pasa, condicionado|
|RS01_plate5_eta0.7_I2|5819.3|2.002|Pasa, condicionado|
|RS01_plate5_eta0.85_I1|5819.3|2.777|Pasa, condicionado|
|RS01_plate5_eta0.85_I2|5819.3|2.301|Pasa, condicionado|

Caso de mayor masa e inercia y eta0.70:masa5819.3g,CG horizontal[-25.94, 76.32, 134.04]mm. En J2 demanda2.9972Nm frente a6Nm de referencia: margen2.0019. No hay reserva adicional grande: variaciones de caja, temperatura o datos internos pueden cambiar la decision.

| Eje | Demanda motor maxima(Nm) | Capacidad usada(Nm) | Cociente |
|---|---:|---:|---:|
|J1|1.7395|6.0000|3.449|
|J2|2.9972|6.0000|2.002|
|J3|2.5641|6.0000|2.340|
|J4|0.2808|0.9807|3.492|
|J5|0.4005|0.9807|2.449|
|J6|0.0098|0.9807|99.579|

Los presupuestos escalares restantes para inercia propiaSTS, antes de acoplamientos con el padre, son:

| Eje | Presupuesto conservador de inercia equivalente(kgm2) |
|---|---:|
|J4|0.013339|
|J5|0.001430|
|J6|0.030589|

Estos valores son requisitos calculados con la holgura de par, NO propiedades medidas ni garantizadas de un STS. El momento que un actuador debe frenar no es solo el de su carcasa.

## Costo estructural de resolver el par con motores mayores

Sobre J1 v09, en99378 estados expandidos por alternativa: P0=795.5N,C0/P0=2.464. Requisito Rp del husillo con n2/Kt2=527.0MPa. El4140 propuesto deRp>=600MPa solo supera este filtro nominal si se acredita su tratamiento/material; no hay certificado de la pieza. Cuna:resistencia normal FDM requerida22.08MPa. Incremento M3 cartucho285.2N por tornillo y presion de arandela9.37MPa. Son modelos de secciones/barras/placa rigida, no FEA, fatiga ni contacto validado.

El modelo de barras conservaE1500MPa ynu0.38 supuestos; no son propiedades FDM acreditadas de esta bobina. No se asigna53MPa de la ficha de filamento como admisible del PLA impreso. El fabricante no acredita en esa tabla orientacion,perfil,uniones entre capas ni creep de esta pieza. Se necesita un perfil y datos FDM aplicables o un camino de carga metalico que se calcule y modele con su masa. No se diseno un refuerzo a escondidas dentro de la reserva.

## El vuelco es dinamico, no solo una comprobacion del CG estatico

Se sumaron fuerzas y momentos del brazo, base fija y reservas al plano de contacto hipotetico. Para un rectangulo centrado se exige|Mx|<=by*N y|My|<=bx*N en cada instante. Se calcula la masa adicional de auto conCGx=y=0 que haria falta, SIN margen extra. No se midieron la masa,el CG,altura del suelo ni el poligono real del AGV; los rectangulos siguientes NO son medidas certificadas del auto.

| Caso | Semiancho x/y supuesto(mm) | Suelo relativo(mm) | Masa adicional centrada necesaria(kg) |
|---|---|---:|---:|
|v09_actual|100/80|-40|4.87|
|v09_actual|122.5/100|-40|3.25|
|DS51150_reforzado|100/80|-40|5.91|
|DS51150_reforzado|122.5/100|-40|3.81|
|RS01,5mm,Ix2,eta0.70|100/80|-40|7.38|
|RS01,5mm,Ix2,eta0.70|120/100|-40|4.49|

Por ejemplo, el casoRS01 necesita7.38kg adicionales bajo apoyo hipotetico de semiejes100/80mm y sueloz=-40mm. Esto no dice cuanto pesa el auto real ni que esa sea su huella; demuestra que unCG estatico dentro del apoyo no basta para aprobar la frenada. Incluso la v09 ligera exige4.87kg en ese mismo supuesto. El problema no nace solamente del nuevo motor.

Una caja mas fuerte no cambia por si sola ese equilibrio global. Cambiar solo motores puede cumplir par y seguir incumpliendo apoyo/union. Antes de adoptarRS01 se deben contrastar contactos y masa reales; si no alcanzan, se requieren otra distribucion de masa/actuadores o apoyos calculados, manteniendo el requisito30grados/s50ms. No se declara el objetivo logrado cambiando esos requisitos. El movimiento del AGV sumaria otros esfuerzos que aqui no se agregaron.

## Fuentes y limites de disponibilidad

Consulta2026-10-28. No se compraron productos ni enviaron mensajes a proveedores.

- [FEETECH STS3250,ficha oficial](https://www.feetechrc.com/Data/feetechrc/upload/file/20240120/6384135881578380868917773.pdf) y [wiki ST-3250-C001](https://ftservo.github.io/ftservo-wiki/products/models/st-3250-c001/main/). SHA256c471fe4e3b872423100ec28726800e65bfc5eb7aa14a82e35db28d3c8e89ab28.
- [Oferta STS3250 ArmDojo](https://armdojo.com/products/feetech-sts3250-serial-bus-servo-50kg), [endpoint de variantes](https://armdojo.com/products/feetech-sts3250-serial-bus-servo-50kg.js), [politica de envios](https://armdojo.com/policies/shipping-policy). C001/C002 ofrecidos; importacion, no stock local. Moneda depende de mercado; no usar el numero del endpoint comoUSD sin comprobar moneda.
- [RobStride,repo oficial](https://github.com/RobStride/Product_Information), [directorio vivo](https://api.github.com/repos/RobStride/Product_Information/contents). Ficha RS recuperada de archivo2026.010.09, paginas7-9 paraRS01. [PDF exacto](https://raw.githubusercontent.com/RobStride/Product_Information/main/%E7%81%B5%E8%B6%B3%E6%97%B6%E4%BB%A3RS%E7%B3%BB%E5%88%97%E4%BA%A7%E5%93%81%E8%A7%84%E6%A0%BC%E4%BB%8B%E7%BB%8D%282026.010.09%29.pdf). SHA256727dd15467558c284dbfc9eceab9440d5bda2e70df0d6d5d617ca5b6b55ea4f6. La ruta puede cambiar; verificar hash del archivo recuperado.
- [Mouser Argentina, catalogo Seeed de robotica](https://ar.mouser.com/es/c/industrial-automation/robotics/?b=Seeed+Studio%7C~Seeed+Studio%7C~Seeed+Studio%7C~Seeed+Studio%7C~): ofertaRS01/100029452 en catalogo argentino, indicada sin existencias. No es disponibilidad inmediata ni confirmacion de lote correspondiente a la fichaOct2026.
- [Ficha FEETECH ST-3215-C047](https://files.seeedstudio.com/products/Feetech/108090003_FEETECH_ST-3215-C047-Datasheet.pdf), [oferta Starware3116](https://tienda.starware.com.ar/producto/servo-motor-smart-starware-sts3215-360g-30kg-12v-encoder-magnetico-ttl/). Correspondencia de sufijo y parametros de inercia sigue abierta.

PDF recuperables se guardan enCAD/.downloads/actuadores, excluidos; no hay licencia de redistribucion identificada. Resumenes propios, scripts y fuentes permanecen versionados.

## Reproducir calculos y siguiente decision

```powershell
python calcular.py
python calcular_QDD.py
python uniones_QDD.py
python estabilidad_base.py
python optimizar_relacion.py
python informar.py
```

RequierePython3.12/numpy y estadosv11 regenerados. NPZ nuevos regenerables enCAD/exports/analisis-actuadores-v12 y analisis-actuadores-v12-QDD, excluidos por politica global. No es una bateria de tests ni se modifican puertos o firmware.

Pendientes concretos para cerrar preseleccion: masa/contactos reales del auto; camino de carga y propiedades FDM/metal; cajas con peso/eficiencia/rigidez demostrables; inercia de los servos distales; limites termicos/freno/regeneracion; disponibilidad de la variante exacta. El objetivo sigue activo. No se crean housings antes de esa decision.
