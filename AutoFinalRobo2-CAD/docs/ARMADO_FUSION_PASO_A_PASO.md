# Armado en Fusion: avance del 23/10/2026

Continuación 24/10, AFR2-20261024-01: [ruedas y Blender paso a paso](RUEDAS_Y_BLENDER_20261024.md). Fusion v16, cuatro ruedas y casquillos de referencia colocados. Fotos y Blender guardados, GLB 827.232 bytes. Estado sin ruedas de abajo histórico.

AFR2-20261023-07. Documento **AutoFinalRobo2**, Admin Project, versión 15 guardada al terminar. ID: `urn:adsk.wipprod:dm.lineage:U8ld51qMQJi5nVnFmq1OKg`.

[Archivo nativo exportado](../mecanica/auto-completo/AutoFinalRobo2.f3d). Fusion devolvió éxito de exportación. No se reimportó para comprobar recuperación ni referencias externas. La API añadió .f3d al nombre solicitado; se normalizó el nombre final a AutoFinalRobo2.f3d, sin modificar su contenido.

[Driver nativo separado](../mecanica/componentes/OSOYOOMotorDriver_ModelY_V2.f3d), también exportado con éxito. [Poses de continuidad](ensamble-fusion-v15.json): matrices guardadas desde el documento, sin evaluación de interferencias. Vistas regenerables en renders/ensamble-parcial-frontal.png y ensamble-parcial-trasero.png, excluidas de Git.

## Referencias y decisiones

- [Manual principal M2](../reference/supplied/manual-m2/202000600-m2.pdf), copia sin modificación del PDF aportado desde Downloads. El original se conserva.
- [Inventario completo CADS por archivo](../reference/inventario-CADS-2026-10-23.csv). Los nuevos CAD son aportados por el usuario; no se trasladaron ni modificaron. Su README anterior solo describe los nueve insumos históricos.
- [Componentes elegidos, cantidades, fuentes y estados](COMPONENTES_3D.csv). Autoría y licencia remotas de los nuevos CAD aportados siguen sin identificar.
- Fotografías frontal y trasera de la conversación y CADS/MotorDriver/DriverDimensions.png y Driver.png.
- Autorización: ensamblar lo disponible, Model Y esencial al final, guardar por etapas con descripción, saltar dificultades. El usuario confirmó 80 mm y después indicó **no construir ruedas**.

## 1. Conservar chasis y shield

CapaInferior y CapaSuperior mantienen su geometría y posición: Y=0..3 mm e Y=23..26 mm. Se conserva la separación libre de 20 mm del modelo del usuario. Se guardó un punto de partida antes de incorporar los otros CAD.

Se reutiliza **OSOYOOWifiShield**, armado por el usuario, con 103 cuerpos internos. No se reconstruye desde CADS/WifiShield ni se duplica el ESP. El insumo ESP_12_E representa el módulo elegido por el usuario; su nombre no acredita que sea la variante física ESP-12S. Las referencias originales permanecen ocultas en el documento.

## 2. Incorporar los cuatro motores

CADS/GM25-370/gm25-370.ipt se subió a la carpeta del proyecto para traducción de Fusion y se insertó embebido. Una definición GM25_370 se usa cuatro veces, con ejes hacia afuera. Orígenes X=±24, Y=-14, Z=±62 mm: colocaciones inferidas, no medidas físicas. Escuadras esenciales simplificadas bajo el chasis; geometría y fijaciones exactas pendientes.

**Ruedas y acoples pendientes:** Assieme_ruota_mozzo.stp tiene una envolvente de aproximadamente 100 mm, por lo que queda oculto. El archivo histórico de 80 mm es un cilindro de referencia y también queda oculto. No se escaló ni construyó una rueda. Falta montar dos tipos de ruedas de 80 mm y orientar sus rodillos según la página PDF 7.

## 3. Incorporar control y sensores disponibles

Se importan Mega2560 arduinoMegaDetailed.STEP, STEP SG90, HC-SR04 Sensor.stp y 5 Channel IR Tracker.step. El SG90 conserva el cuerpo, brazo simple y tornillos; las crucetas alternativas se ocultan.

Sensor de línea delante y por debajo del chasis, ópticas hacia el suelo. Se representan separadores de 40 mm según páginas PDF 7–8 y 32–33; centros X=±25, Z=-93 mm adoptados para el montaje. SG90 en el recorte delantero y HC-SR04 mirando adelante en pose neutra. El soporte negro reproduce de forma esencial el soporte del kit de las páginas PDF 25–26; no es la futura pieza OLED/encoder. Sin simulación del servo.

Se guardó la etapa 1 con descripción de importaciones y colocaciones.

## 4. Alimentación y apoyos

Se importa el portabaterías doble aportado. El STEP contiene cable unido al sólido: **Portabaterias_SinCableExterior** es un derivado recortado a la envolvente del cuerpo, manteniendo intactos el archivo y la instancia importada oculta. Sin lazo exterior de cableado en el montaje.

Dos instancias de 18650 Battery.STEP quedan con orientaciones opuestas dentro de la caja trasera. Es un portabaterías abierto; no representa la caja cerrada con tapa/interruptor de algunas fotos.

Cuatro separadores de chasis de 20 mm conservan la distancia existente. Los delanteros de 40 mm proceden del manual. Apoyos de placas de 6,5 mm y ubicaciones de fijación son inferencias. La etapa 2 se guardó en **versión 13**, con descripción.

## 5. Distribución según la página PDF 9

**Mega a la derecha, Model Y a la izquierda y batería atrás**, como muestra el manual. Mega con USB al lateral derecho y OSOYOOWifiShield apilado sobre ella. La alineación del shield es documental y visual; no se comprobó compatibilidad física de pines.

| Componente | Origen X,Y,Z en mm | Orientación |
| --- | --- | --- |
| Mega2560 | 25,5; 32,5; 29 | Giro 180° sobre Y |
| OSOYOOWifiShield | 36,8; 45; 29 | Mismo giro que Mega |
| Portabaterías | 0; 36; 81,75 | Largo transversal |
| Celda A | -32,55; 36,5; 81,75 | Eje hacia +X |
| Celda B | 32,55; 36,5; 62,25 | Eje hacia -X |
| Sensor IR | 0; -40; -93 | Ópticas hacia abajo |
| SG90 | -6,6733; 3,0322; -102,7832 | CAD nativo trasladado |
| HC-SR04 | -5,15; 61; -86 | Frente hacia -Z |
| Model Y | -48; 32,5; 29 | PCB larga longitudinal |
| Voltímetro | -50; 32,5; -32 | Referencia visual |

Se respetan los ejes del chasis existente: **X derecha, Y arriba, frente -Z**. Conversión al convenio inicial del plan: X_vehículo=-Z_archivo; Y_vehículo=-X_archivo; Z_vehículo=Y_archivo. No se rotó el chasis para imponer otro convenio.

## 6. Model Y esencial, construido al final

**OSOYOOMotorDriver_ModelY_V2**: PCB, conectores y pines, dos capacitores, regulador y circuitos principales como cuerpos separados. Cotas de DriverDimensions.png: 62×43 mm, agujeros Ø3,5 mm, centros separados 56×37,4 mm, a 3 mm/2,8 mm de los bordes. Espesor 1,6 mm, alturas y paquetes aproximados de la vista superior. Sin pistas ni diseño eléctrico/fabricable.

El voltímetro no tenía CAD: referencia visual de tres dígitos, PCB adoptada 35×15 mm según foto, sin dimensiones reales confirmadas. Separadores y cabezas de tornillo de placas son simplificados; no se representó toda la tornillería.

Etapa 3 guardada en **versión 14**. Posición del voltímetro y distribución disponible guardadas en **versión 15**, descripción: conservar posición del voltímetro delantero izquierdo y distribución final; ruedas de 80 mm y acoples pendientes.

## Fuentes y continuación

Scripts en mecanica/auto-completo: fusion_ensamble.py, fusion_etapa2.py y fusion_etapa3.py. Parámetros explícitos en Python; cuerpos simplificados en BaseFeatures editables. **No hay parámetros de usuario que conduzcan automáticamente las geometrías**. Los scripts requieren el chasis/importaciones existentes y no reconstruyen desde cero todo el archivo; no ejecutarlos a ciegas sobre el modelo terminado.

Falta recibir ruedas correctas y acoples; afinar soportes/fijaciones/alturas inferidos e identificar variantes físicas motor/Hall, portabaterías y SG90/MG90. OLED/encoder, nueva PCB y pieza de interfaz siguen para otra etapa; sus modelos previos no se insertaron en este ensamble.

Estado: **ensamble CAD parcial guardado y exportado**, con los modelos disponibles incorporados o con excepción explícita, sin ruedas. No es montaje completo ni compatibilidad física comprobada. No se ejecutaron tests, compilaciones, validadores, interferencias, simulaciones, ERC/DRC, flasheos ni pruebas físicas.
