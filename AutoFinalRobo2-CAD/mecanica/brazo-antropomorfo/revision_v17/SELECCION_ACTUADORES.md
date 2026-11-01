# Seleccion por carga propia y pieza de 200 g

> Antecedente congelado el 30/10/2026: cifras anteriores al cierre de los
> montajes J1/J4/J5. El usuario difiere nuevos calculos y comprobaciones
> hasta terminar y revisar el modelo 3D con sus ensambles y limites.

29/10/2026. Alcance corregido por el usuario: **no continuar calculando
vuelco**. Esta seleccion trata el par externo y las masas; no acredita
estabilidad del auto ni resistencia fisica del conjunto.

## Resultado del calculo previo al montaje

Se conserva la arquitectura V7, la pinza de dos cremalleras y los seis
movimientos. El motor de J3 se coloca junto a J2 y mueve el codo por correa.
PVC Tigre25x1.2 y20x1mm, distancias entre ejes70/60mm. Los motores se
seleccionaron antes de adaptar sus soportes, incluyendo su propia masa.

| Eje | Actuador | Reduccion externa | Par externo pico Nm | Par disponible estimado Nm | Margen |
| --- | --- | ---: | ---: | ---: | ---: |
| J1 axial | FEETECH STS3250 | 1 | 1.1130 | 1.4906 | 1.339 |
| J2 hombro | FEETECH STS3250 | 4, engranajes18/72 | 3.7070 | 5.0210 | 1.354 |
| J3 codo | FEETECH STS3250 | 2.5, poleas20/50 | 2.4667 | 3.1381 | 1.272 |
| J4 axial de muneca | FEETECH STS3250 | 1, par24/24 existente | 0.7903 | 1.2553 | 1.588 |
| J5 inclinacion | FEETECH STS3250 | 1 | 0.9825 | 1.4906 | 1.517 |
| J6 herramienta | FEETECH STS3032 | 1 | 0.0113 | 0.1397 | 12.330 |

Fuente numerica: `preseleccion_actuadores_integrados.json`, calculada con
`preseleccionar_actuadores_integrados.py`. **Es una preseleccion, no el
resultado del CAD con todos estos motores ya montados.** La geometria base
y su SHA estan registrados en el resultado. Se actualizo la extraccion con los montajes de J6 y pinza terminados.
Actualizado30/10con J2 nativo: engranajes18/72, apoyos625, carcasa,
brida/eje y herrajes reales. Quedan J1/J4/J5 como propuestas con reservas.
Falta volver a hacerlo al integrar esos tres actuadores y sus montajes.

Se usan16kgfcm nominales del STS3250 a12V y1.5kgfcm nominales del STS3032
a6V. No se utiliza el par de bloqueo como capacidad continua. Eficiencias
hipoteticas:0.95 en acoples directos y0.8 en transmisiones externas.
No sustituyen datos del montaje adquirido.

La masa presupuestada es2495g sin pieza; CAD actual aproximadamente2352g.
Subio unos72g respecto a la preseleccion de2424g al incorporar el montaje
real de J2 y corregir las envolventes comerciales. No se oculta esa masa
adicional atribuyendo ahorros al estudio de aligerado pendiente.
Incluye las masas estructurales
actuales, sustitucion de masas de motores y reservas adicionales para
montajes restantes:40gJ2,25g ruedaJ2,40gJ4,25gJ5 y10g de cableado
en la herramienta. J6 y la pinza ya usan sus montajes CAD y masas actuales,
sin volver a sumar reservas de15g por cuna. No se restan ahorros hipoteticos de optimizacion.
La masa CAD actual es2240g sin pieza; el antecedente anterior a adaptar
los pequenos era2207g. Ninguno de estos valores equivale a un pesaje.

El calculo muestrea1760 estados,30grados/s por articulacion y frenada cubica
finita de50ms, con pico900grados/s2. **No son la velocidad/aceleracion
maximas nominales del fabricante**. No se conoce la inercia interna del
servo ni se ha medido su parada. El minimo1.274 satisface el filtro externo
1.2 pedido por el usuario en esos estados, sin demostrar el maximo global.

J2 requiere120grados/s en motor; J3,75grados/s. Ambos son inferiores a la
velocidad de catalogo del STS3250. La curva de velocidad a par nominal
tambien permite la preseleccion; no demuestra la respuesta transitoria ni
la disipacion termica del montaje final.

## Pinza conservada

Se cambia el MG90S por STS3032, sin cambiar mordazas, cremalleras, guias ni
dentado V7. El horn comercial nuevo es18mm,8M2 enPCD12,25T4.95 y tornillo
centralM2.3. La fijacion anteriorPCD11 debe adaptarse. No es intercambiable
geometricamente solo porque ambos sean servos pequenos.

Con pinon de radio12mm y eficiencia hipotetica0.85, el caso dinamico da
4.475N sobre la pieza. Para margen1.2 de agarre por friccion hace falta
coeficiente efectivo de contacto **al menos0.516**. La almohadilla elegida
y su agarre deben confirmarse fisicamente: no se declara aprobada la
sujecion de200g solo a partir del par del motor.

## Referencias comerciales y montaje

- [FEETECH STS3250 A0](https://www.feetechrc.com/Data/feetechrc/upload/file/20240120/6384135881578380868917773.pdf): cuerpo45.22x24.72x35mm,25T5.9,M3,masa74.5+1g. El dibujo incluye salientes locales hasta36.5mm y estriado3.4mm; conservar las reservas del alojamiento. Se usa75.5g.
- [FEETECH STS3032 A0](https://akizukidenshi.com/goodsaffix/STS3032%20SPEC.pdf): cuerpo23x12x27.5,altura total31.9,masa20.6+1g. Se usa21.6g.
- [SDP-SI engranajes metricos](https://www.sdp-si.com/D815/D815-Metric-Section1-Gears-SDPSI.pdf), paginas impresas1-49/1-50: S10T08M018A0505 yS10T08M072A0508, modulo0.8, aluminio2024, cara5mm, largo12mm, centros36mm. Requieren adaptacion de chaveta y retencion; no confiar todo el par a un prisioneroM2 de catalogo.
- [Oferta STS3250 con destino Argentina seleccionable](https://armdojo.com/products/feetech-sts3250-serial-bus-servo-50kg): importacion admitida; checkout, impuestos y stock definitivo sin confirmar.

Los STS son servos de bus TTL con realimentacion interna. **No se controlan
con PCA9685 como los PWM anteriores.** Hace falta adaptar interfaz serial,
distribucion12V/6V, protecciones y conectores antes de construir. La masa de
la electronica anterior sigue incluida conservadoramente; su dibujo no
representa una conexion electrica ya corregida. No se modifica firmware.

J2 yJ3 requieren recorrido multivuelta del motor. El arranque necesita una
referencia conocida; no se debe asumir que el conteo multivuelta persiste
tras cortar la alimentacion. No se agregan sensores externos al modelo
por esta seleccion, pero el metodo de referenciado sigue pendiente.

## Aligerado en Fusion

Herramienta identificada: Shape Optimization, entorno Simulation. Ver
`PENDIENTE_OPTIMIZACION_MATERIAL.md`. No se ha resuelto ningun estudio ni
se ha supuesto una reduccion de masa para cerrar los margenes anteriores.
Preservar asientos de rodamientos, ranuras del tensor, apoyos y encastres.

La comprobacion de PLA PrintALot debe considerar orientacion y perfil de
impresion aun no definidos. Esta seleccion no es una aprobacion de
resistencia intercapas ni un estudio de tensiones terminado.

## Revision CAD de los dos actuadores pequenos

Ver REVISION_PINZA_Y_J6.md. Nueve poses documentadas, incluyendo apertura
maxima exacta180grados:79.405mm entre mordazas; cierre4.007mm y agarre50mm.
Se corrigieron contactos nuevos de fijaciones y palma; quedan13 contactos
brutos de referencia clasificados. No es barrido continuo del brazo.
La comprobacion no da por integrados los otros cuatro motores propuestos.
