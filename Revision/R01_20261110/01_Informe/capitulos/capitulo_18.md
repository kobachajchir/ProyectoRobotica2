# 18. Software, comunicaciones y trazabilidad

Contracts es la fuente compartida para unidades, signos, campos y catálogo del enlace. Una modificación de comandos ESP↔ATmega debe mantener wire_catalog.json, header, commands.json y catálogo de Protocol Studio. Un comando retirado conserva su ID y los datos con SSID o claves se identifican como sensibles. El informe no necesita reproducir claves ni configuraciones personales.

La Web utiliza React/Vite y ofrece visualización, control y exploración del sistema. El ESP8266 ejecuta firmware propio con almacenamiento LittleFS. El ATmega se desarrolla en C con avr-libc. El Loader mantiene sus fuentes C#, perfiles y manual por separado, con configuración y registros de usuario fuera del repositorio.

## 18.1 Contratos matemáticos

| Dato | Contrato necesario |
| --- | --- |
| Velocidad lineal | Unidad explícita; conversión mm/s↔m/s en una frontera identificada. |
| Ángulos y velocidad angular | Distinguir grados de visualización de radianes de ecuación y de unidad nativa de servo. |
| Ruedas | Orden FL,FR,RL,RR y signos de motion_reference.json. |
| Tiempo | Período real, edad de muestra y rechazo de datos inválidos. |
| Pose del brazo | Modelo/revisión, ceros, límites y referencia de montaje. |
| Error | No confundir desconectado, dato vencido, fuera de rango y valor cero. |

La versión del producto puede avanzar en otros trabajos mientras este borrador permanece fijo. Por eso los resultados se vinculan a rutas y revisiones de los insumos, no a una afirmación de que el firmware actual haya sido ejecutado. Esta entrega solo modifica documentación y no eleva la versión de ATmega, ESP o Web.

## 18.2 Registro mínimo para futuros ensayos

Cada sesión debería guardar identificación del modelo, versiones, calibraciones, tensión de batería, carga, superficie, consignas, medidas crudas, tiempos y causa de parada. Con esos datos podrán calcularse error de trayectoria, deriva de odometría, repetibilidad del TCP, tiempo de ciclo y tasa de agarre. La ausencia de una muestra no debe rellenarse con un resultado favorable.
