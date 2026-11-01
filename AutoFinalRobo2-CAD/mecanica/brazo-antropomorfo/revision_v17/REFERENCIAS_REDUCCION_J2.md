# Reduccion comercial del hombro, integrada en CAD30/10

Antes de elegir la reducción definitiva se extraen las masas del CAD
compacto. El presupuesto anterior de1452.5g no representa el modelo actual.

Se identificó una pareja comercial de aluminio2024, módulo0.8, ángulo de
presión20° y cara5mm: SDP/SI **S10T08M018A0505** (18dientes, agujero5H7,
diámetro exterior16mm, cubo10mm) y **S10T08M072A0508** (72dientes,
agujero8H7, exterior59.2mm, cubo12mm). Ambas tienen largo total12mm.
La relación seleccionada es4 y la distancia entre centros36mm. No son piezas
ya montadas ni una confirmación de stock, precio o envío a Argentina.

Fuente: catálogo del fabricante SDP/SI D815, sección1, páginas1-49/1-50,
copyright2019, consultado29/10/2026:
[catálogo métrico](https://www.sdp-si.com/D815/D815-Metric-Section1-Gears-SDPSI.pdf).
Recuperación local excluida: `.downloads/actuadores/SDPSI_Metric_Gears.pdf`.
SHA256: `443c5005829016ac7380441c34aad7b46563504bb1f28ac38b9c92a5eda70f5b`.
Conservar como referencia de un producto comercial, sin redistribuir el PDF
ni atribuirse el diseño. Las adaptaciones de ejes/chaveteros son propias.

Los prisionerosM2 del producto no se toman como interfaz suficiente para
transmitir el par. Harían falta chaveteros y chavetas compatibles, dos625
externos para el eje de entrada y retención axial independiente. Hay que
incluir cubos, soportes y herrajes al comparar la masa de esta alternativa.

El STS3250 especifica16kgf·cm nominales a12V y1.4A nominales, frente a50
de bloqueo. Con reducción4 y eficiencia supuesta0.8, la capacidad de
preselección sería5.021Nm; margen1.2 exige demanda<=4.184Nm. La eficiencia
es una hipotesis. La preseleccion completa da3.707Nm y margen1.354,
incluyendo motores y reservas de montaje; ver SELECCION_ACTUADORES.md.
Pareja integrada30/10enJ2. Ver MONTAJE_J2_STS3250.md y REVISION_MONTAJE_J2.md. Sustituye20/40=2 del modeloV7.

La misma ficha, página5, admite control multivuelta±7vueltas pero pierde
la cuenta de vueltas al apagar. J2 de−10 a125° requeriría540° en el motor
con esta relación. Conservar el recorrido exige prever una referencia al
encender; no afirmar posición absoluta articular solo por enviar comandos.
El motor usa paquetes serie TTL semidúplex, no la salidaPWM delPCA9685.

[FEETECH STS3250 A/0,16/01/2024](https://www.feetechrc.com/Data/feetechrc/upload/file/20240120/6384135881578380868917773.pdf),
páginas3–5. La alimentación12V, la interfaz y el frenado real siguen por
resolver; los reguladoresXL4016 existentes no demuestran una fuente12V
desde una batería de menor tensión. No se cambió firmware ni alimentación.
