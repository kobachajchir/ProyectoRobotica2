# Revision local de J3 despues de corregir holguras

Resultado nativo: `interferencias_J3_correcciones_finales.json`.
Tres poses, pinza ajustada exactamente a pieza50mm,200g como referencia de
carga. Brazo extendido y codo en ambos extremos de90grados. El analisis
restauro las posiciones previas. No se genero un barrido continuo.

En cada pose quedaron13 contactos brutos, frente a17 en la revision
anterior. Desaparecieron los cuatro choques corregidos: dos puntasM4 con
la tapa estacionaria del hombro, ala de tensor con polea50T y tornillo de
abrazadera del codo con plastico que no estaba perforado.

| Contactos restantes por pose | Cantidad | Interpretacion y limite |
| --- | ---: | --- |
| Correa con discos20T/50T | 2 | Contacto esperado de envolventes a diametro primitivo, sin dientes comerciales. No demuestra engrane real. |
| TornillosM2 de tapa625 con piloto impreso | 2 | Envolventes de rosca; ajustar piloto al perfil PLA. |
| Prisionero de collar de codo con eje y collar | 2 | Contactos funcionales de apriete y rosca simplificados. |
| Prisionero de acople de entrada con eje y acople | 2 | Contactos funcionales de apriete y rosca simplificados. |
| Prisionero de collar de entrada con eje y collar | 2 | Contactos funcionales de apriete y rosca simplificados. |
| Disipador con bornera de cada referenciaXL4016 | 3 | Solapes internos de referencias comerciales heredadas,48mm3 por modulo. No son choques del movimiento de J3; corregir o sustituir esas referencias al adaptar la electronica TTL. |

No se ocultan los13 resultados en el archivo bruto. La clasificacion se
aplica a estos pares concretos, no a toda interseccion entre tornillos o
entre correa y cualquier otra pieza. No equivale a aprobar el brazo entero,
ni los montajes de los actuadores que se estan sustituyendo despues de
este analisis. No se calculo vuelco.
