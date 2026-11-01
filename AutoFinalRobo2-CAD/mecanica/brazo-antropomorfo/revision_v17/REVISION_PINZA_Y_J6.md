# Pinza V7 y J6: montajes STS3032 terminados

29/10/2026. Se conservan mordazas, guias, cremalleras, pinon24T y movimientos
V7. Cambian ambos servos aSTS3032 A0 y sus cunas, tapas, horns y fijaciones.
Los maestros v07/v09 no se modifican.

## Correcciones comprobadas

Las cabezas inferiores de los tornillos del horn deJ6 chocaban con su
tapa fija y las puntas entraban en el soporte del625. Se sustituyen por
dosM2x4 avellanados desde la brida de acero a las roscas del horn enY+-6.
Quedan al ras, con paso libre2.3 en acero; no roscar dos piezas a la vez.

El nuevo piso habia cerrado pasos existentes del ejeD y prisionero deJ6,
y parte del alojamiento de las tuercas de la pinza. Se reabren el paso
5.2 y la llaveD con holgura0.1 en la zona superior, el paso nominal del
prisioneroM2.5 y cuatro bolsillos inferiores4.9 para tuercas. Las tapas
usan cuatroM2x35 y tuercas bajasDIN439M2de1.2mm bajo todo el piso, base2.6
y tapa3mm. CentrosX-11/20.5,Y+-8.3. Cada pieza impresa permanece conectada.

## Posiciones y apertura real

`interferencias_mini_corregidas.json` contiene siete poses: extendido,
codo+-90, dos extremos de inclinacion/giro de muneca, cierre y apertura.
Se miden las posiciones reales de los origenes de las mordazas respecto
a la palma, ademas del valor del servo. No se acepta el angulo ordenado
como demostracion de que las mordazas llegaron a esa posicion.

| Posicion | Angulo absoluto J7 | Separacion real |
| --- | ---: | ---: |
| Cerrada | 0 grados | 4.007mm |
| Pieza | 109.8 grados | 50.000mm |
| Cerca del tope | 179.9 grados | 79.363mm |
| Tope exacto | 180 grados | 79.405mm |

El tope exacto se comprueba adicionalmente en
`interferencias_extremo_pinza.json`, aproximando desde179.9. Antes, poner
J7 antes de los otros seis ejes hacia que Fusion devolviera la rama
cerrada. El script ahora aplica J7 al final y avanza en pasos de15grados,
comprueba el angulo alcanzado y compara separacion real con la cinematica.
Los limites mecanicos no fueron recortados.

Las nueve poses conservaron13 contactos brutos por pose, iguales a los
clasificados en REVISION_CONTACTOS_J3.md: roscas/prisioneros simplificados,
correa con discos de referencia y tres solapes internosXL4016 heredados.
Desaparecieron los12 pares nuevos de fijaciones y palma que habia encontrado
la primera revision. Los datos brutos anteriores se conservan como evidencia
de la iteracion. Se restauraron las poses originales despues del analisis.

No es un barrido continuo de todas las combinaciones de articulaciones ni
un ensayo fisico de fuerza de agarre. Tampoco acredita los montajes nuevos
J1/J2/J4/J5 que aun no estan integrados.

## Masas y siguiente comprobacion

Extraccion actual:359 cuerpos solidos. Masa calculada2240.34g sin pieza;
PLA macizo1.24g/cm3, PVC1.45g/cm3 supuesto, metales y masas de catalogo.
No equivale a masa laminada ni pesaje. Hay0errores de historial y4avisos
de cortes sin objetivo: dos del antiguo motorJ4 y dos recesosA11 que no
alcanzan material. No se interpretan como comprobacion resistente.

Con estos dos montajes reales y reservas solo para los restantes,
la preseleccion completa suma2423.94g sin pieza y margen minimo externo
1.274. Ver SELECCION_ACTUADORES.md. No se hicieron calculos nuevos de vuelco,
FEA, tests de software, compilaciones, flasheos ni ensayos fisicos.
