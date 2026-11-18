# Guion hablado · AutoFinalRobo2 (presentación v4, tema claro)

Generado por `tools/generar_presentacion_v4.js` (mismo texto que las notas del orador). Tiempo de contenido: 31:15 + ~1:30 de transiciones ≈ 25 min. Los anexos no cuentan.

## 01 · F01 · Diseño, construcción, control y programación de un vehículo robótico  
*20 s · acumulado 0:20 · ramas M+E+C+P*

> No vamos a recorrer la carrera materia por materia. Vamos a recorrer un sistema que necesitó conocimientos de distintas materias para existir. Este auto es el hilo conductor: primero veremos cómo se construye, después cómo mide, cómo controla su comportamiento, cómo se programa y, al final, cómo todo eso se integra.

**Fuentes:** Render F01 desde AutoFinalRobo2-Web/public/models/AutoFinalRobo2.glb (v4: assets_claro/renders/F01_auto_hero_v001.png cuando llegue de la PC de escritorio).

## 02 · F02 · ¿Qué es la Mecatrónica?  
*40 s · acumulado 1:00 · ramas M+E+C+P*

> Un sistema mecatrónico no es una suma de partes: es estructura que se mueve, electrónica que mide y actúa, control que regula ese comportamiento y software que procesa y comunica. Cada círculo junta las materias propias de un área; donde dos círculos se tocan están las materias que las unen, como Microcontroladores entre electrónica y programación o Mecánica Racional y Robótica I entre mecánica y control. Y en el centro, donde se cruzan las cuatro, está el proyecto integrador.

**Fuentes:** Res. C.S. 012/23 (Plan 2023): cuatro bloques formales; perfil del egresado. Materias nombradas en el informe maestro y en los pies curriculares de la charla.

**Pendiente / no afirmar:** La ubicación de cada materia en una zona del Venn es didáctica (criterio del equipo), no del plan. Confirmar los nombres exactos de Máquinas Eléctricas y Sistemas Mecatrónicos I/II contra el plan.

## 03 · F03 · Árbol de conocimientos y correlatividades  
*70 s · acumulado 2:10 · ramas M+E+C+P*

> El árbol se lee de arriba hacia abajo, año por año. A la izquierda, la mecánica: Estática y Resistencia de Materiales y Ecuaciones Diferenciales regularizadas, más Ciencias de los Materiales aprobada, llevan a Mecánica Racional, y esa junto con Sistemas de Control lleva a Robótica I y después a Robótica II. A la derecha, la electrónica digital: Electrónica Digital y Laboratorio de Mediciones regularizadas, Electrónica Básica y Computación II aprobadas, habilitan Microcontroladores; desde ahí Sistemas Embebidos y luego Sistemas Operativos y Redes. La letra en cada flecha es la condición oficial: R regularizada, A aprobada.

**Fuentes:** Aristas copiadas del informe maestro (C.D. 726/23): Mecánica Racional, Microcontroladores, Sistemas de Control, Sistemas Embebidos, Robótica I, Sist. Operativos y Redes.

**Pendiente / no afirmar:** El año de cada materia se asignó respetando el orden de correlatividades, sin el PDF del plan a mano: confirmar los años contra el Plan 2023. Robótica I → Robótica II se dibuja punteada (no está en las aristas citadas). Revisión humana obligatoria contra la C.D. 726/23.

## 04 · F04 · Arquitectura general del robot  
*50 s · acumulado 3:00 · ramas M+E+C+P*

> Este es el mapa que vamos a recorrer. En el navegador corre una aplicación React. Se comunica por Wi-Fi con el ESP8266, que sirve los archivos de la aplicación y abre un WebSocket. El ESP traduce esos mensajes JSON a una trama binaria por UART hacia el ATmega2560, que es quien tiene la autoridad sobre los motores: ahí corren el control de velocidad, la odometría, la pantalla OLED y la lectura de sensores. Abajo, cada componente físico con la interfaz por la que el ATmega lo maneja.

**Fuentes:** AutoFinalRobo2-Contracts/contracts/UART_PROFILE_2.md, PROTOCOL.md; AutoFinalRobo2-Atmega/src/services/afr2_product.c (PID y odometría en ATmega); ESP/platformio.ini; Web/package.json. Renders de componentes: assets_claro/renders/F04_* (PC de escritorio).

**Pendiente / no afirmar:** Faltan los renders de driver, HC-SR04, sensor de línea, OLED/encoder y baterías (prompt de la PC de escritorio).

## 05 · F05 · ¿Qué es un microcontrolador?  
*45 s · acumulado 3:45 · ramas E+P*

> Antes de mirar nuestras dos placas, ¿qué es un microcontrolador? Es una computadora completa metida en un solo chip: un procesador, una memoria flash donde queda grabado el programa, una RAM para las variables y una EEPROM para guardar configuración aunque se corte la energía. Alrededor tiene periféricos que tocan el mundo físico: pines de entrada y salida, temporizadores que generan PWM y miden tiempos, un conversor analógico digital, puertos serie y buses. El programa inicializa todo eso y después repite un lazo para siempre, y cuando pasa algo urgente, una interrupción lo atiende. Lo escribimos en C o C++, un compilador cruzado lo traduce al código de ese procesador y lo grabamos en la flash.

**Fuentes:** Datasheet Microchip ATmega2560 (arquitectura AVR, ejecución en un ciclo de la mayoría de las instrucciones); Atmega/Makefile (avr-gcc); ESP/platformio.ini (toolchain xtensa).

## 06 · F06 · ATmega2560 · Arduino Mega 2560  
*40 s · acumulado 4:25 · ramas E+P*

> El cerebro físico del auto es un ATmega2560 sobre una placa Arduino Mega. Es un microcontrolador de ocho bits a dieciséis megahertz, con doscientos cincuenta y seis kilobytes de flash pero solo ocho de RAM. Lo elegimos por la cantidad de pines, temporizadores e interrupciones: cada periférico del auto tiene su temporizador o su interrupción. Lo programamos en C directo sobre los registros, sin la capa de Arduino, para que los tiempos sean predecibles.

**Fuentes:** Datasheet Microchip ATmega2560 (memorias, USART, timers); ficha de la placa Arduino Mega 2560 (54 E/S, 15 PWM, 16 analógicas); Atmega/Makefile (-mmcu=atmega2560, F_CPU 16 MHz); afr2_motor.c, afr2_board.c, afr2_sensors.c, afr2_twi.c, afr2_uart1.c.

**Pendiente / no afirmar:** Renders del Mega solo, vista superior y detalle del chip: PC de escritorio.

## 07 · F07 · ESP8266 · OSOYOO WiFi Shield (ESP-12)  
*40 s · acumulado 5:05 · ramas P+E*

> El segundo procesador es un ESP8266, en el módulo ESP-12 del shield Wi-Fi. Es de treinta y dos bits, más rápido y con más memoria, y sobre todo tiene Wi-Fi. No toca los motores: guarda la aplicación web en su flash, la sirve comprimida, abre el WebSocket y traduce cada mensaje JSON a nuestra trama binaria hacia el ATmega. Así, si la red se cae, el que manda sobre el auto sigue siendo el ATmega.

**Fuentes:** Datasheet Espressif ESP8266EX; ESP/platformio.ini (esp12e, gnu++17, eagle.flash.4m3m.ld «candidate only»); contracts/PROTOCOL.md; UART_PROFILE_2.md.

**Pendiente / no afirmar:** Modelo exacto del módulo ESP y tamaño de su flash no identificados físicamente (platformio.ini lo dice). Renders del shield: PC de escritorio.

## 08 · F08 · Geometría de las ruedas Mecanum  
*35 s · acumulado 5:40 · ramas M*

> Las ruedas son Mecanum: los rodillos están inclinados respecto del eje, y por eso combinando el sentido de las cuatro ruedas el auto puede avanzar, ir de costado o girar sobre sí mismo. Esa misma geometría aparece en el firmware: la odometría suma los avances de las cuatro ruedas para obtener el desplazamiento hacia adelante, los combina con signos alternados para el lateral, y divide por la suma de semidistancias, ciento setenta milímetros, para el giro. Cada avance sale de contar pulsos del encoder: pulsos por el perímetro de la rueda de ochenta milímetros, dividido trescientos setenta pulsos por vuelta.

**Fuentes:** afr2_product.c odometry() (fórmulas dx, dy, angle); afr2_geometry (AFR2_FIXED_WHEEL_MM 80, AFR2_FIXED_RADIUS_SUM_MM 170); Lynch y Park, Modern Robotics (2017), cap. 13.

**Pendiente / no afirmar:** No afirmar ángulo de rodillos (no medido).

## 09 · F09 · Cinemática matricial del chasis Mecanum  
*70 s · acumulado 6:50 · ramas M+C*

> Esto es lo que ampliamos en Robótica II. La cinemática inversa nos dice cuánto tiene que girar cada rueda para que el chasis tenga una velocidad de avance, una lateral y una de giro: es una matriz de cuatro por tres, la misma que usa el firmware. La directa va al revés: con la pseudoinversa, de las cuatro velocidades de rueda sacamos las tres del chasis. Como la matriz tiene rango tres, no hay singularidades mientras la distancia L no sea cero; y como hay cuatro ruedas para tres grados de libertad, queda un modo interno en el que las ruedas se empujan entre sí sin mover el auto. La transformación homogénea ubica el chasis en el mundo, el jacobiano junta todo, y la odometría es esa misma cuenta integrada cada ciclo, girando por el ángulo medio del intervalo.

**Fuentes:** afr2_product.c:130 (v = [vx−vy−t, vx+vy+t, vx+vy−t, vx−vy+t], t = ωz·L), :295-304 (dx, dy, angle; heading = yaw + angle/2); Lynch y Park (2017) §13.2; Craig (2005) cap. 2.

**Pendiente / no afirmar:** Los signos de las matrices son los de la base lógica del chasis; el firmware además multiplica por AFR2_FORWARD/LATERAL/YAW_WHEEL_SIGN y por factores de escala por dirección (odom_scale).

## 10 · F10 · De un perfil 2D a placas tridimensionales  
*40 s · acumulado 7:30 · ramas M*

> Las placas del chasis vienen del kit. Para modelarlas partimos del contorno en dos dimensiones, lo llevamos a un sketch en Fusion, lo extruimos con el espesor de la placa y verificamos que agujeros y ranuras coincidan con los de la pieza real. A la izquierda, el plano en planta con sus cotas; a la derecha, las dos placas solas en isométrica. Es exactamente lo que se practica en Sistemas de Representación II: pasar de un dibujo plano a un sólido y del sólido a un plano.

**Fuentes:** AutoFinalRobo2-CAD/mecanica/auto-completo/fusion_ensamble.py, AutoFinalRobo2.f3d; objetos AFR2_CapaSuperior/AFR2_CapaInferior; plano: assets_claro/planos (Drawing nuevo, PC de escritorio).

**Pendiente / no afirmar:** Confirmar con qué archivo se generó el perfil antes de nombrarlo SVG. No afirmar material (acrílico/chapa) sin confirmarlo. Si el plano solo tiene cotas envolventes, rotularlo preliminar.

## 11 · F11 · Ensamblaje CAD de componentes  
*50 s · acumulado 8:20 · ramas M+E*

> El auto completo vive en un solo ensamble de Fusion. Lo que ya existía lo trajimos de catálogos y bibliotecas como GrabCAD: las ruedas, los motores, el servo, el ultrasónico, el sensor de línea y el Mega. Dos placas no tenían modelo CAD, el driver de motores y el shield Wi-Fi, así que las dibujamos nosotros. Y lo que no existía lo diseñamos: el soporte del panel, su placa en KiCad y el divisor de batería. La placa sale de KiCad como STEP, entra a Fusion, se arma con uniones, y de ahí exportamos a Blender para la Web.

**Fuentes:** Declarado por el equipo: procedencia GrabCAD de ruedas y sensores; Motor Driver y WiFi Shield modelados por el equipo. Nombres de componente del GLB; CAD/reference/inventario-CADS-2026-10-23.csv; exports/2026-10-24/parts (OBJ placas, USDZ resto); CAD/docs/MODELO_FINAL_WEB_20261025.md.

**Pendiente / no afirmar:** Cruzar la procedencia de cada modelo con el inventario CAD antes de citar autores de GrabCAD.

## 12 · F12 · Diseño propio: panel OLED + encoder  
*50 s · acumulado 9:10 · ramas E+M*

> El panel con pantalla y perilla es nuestro diseño de punta a punta. En KiCad primero va el esquemático, que dice qué se conecta con qué; después el diseño de la placa, donde se rutean las pistas de cobre en una sola cara de veinte por treinta y cinco milímetros. Cada huella tiene su modelo 3D, así que KiCad exporta la placa armada en STEP y en Fusion verificamos que encaje. El soporte lo diseñamos sobre la cubierta superior que ya teníamos modelada, con torretas M3, para montarlo casi sin tocar el auto. Eléctricamente, la pantalla va al puerto I²C del ATmega, pines veinte y veintiuno, y la perilla a dos pines con interrupción.

**Fuentes:** CAD/docs/PANEL_UI_20261025.md (20×35 mm, B.Cu, torretas M3); CAD/docs/MODELO_FINAL_WEB_20261025.md (torretas 15 mm y 5 mm, hembra 1×5 2,54 mm, JST 2 mm de alimentación, R 10 kΩ a 3V3); afr2_twi.c (TWBR 72 → 100 kHz); afr2_board.c (INT4/INT5); contracts/hardware/PINOUT.md.

**Pendiente / no afirmar:** Conector de alimentación: AGENTS.md dice 1×2 de 2,54 mm y el informe final del panel dice JST 2 mm; confirmar el montado. Tensión del panel (3V3 en el esquemático, 5 V en PINOUT.md) sin resolver. Captura del editor PCB de KiCad: PC de escritorio (KiCad 9 de esta máquina no abre la placa).

## 13 · F12F · Ensamble en Fusion 360  
*45 s · acumulado 9:55 · ramas M*

> Para armar el auto completo usamos Fusion 360, el CAD de Autodesk. Es paramétrico: cada pieza nace de bocetos y operaciones que quedan en una línea de tiempo y se pueden editar. En el ensamble, cada componente se vincula con uniones que le quitan grados de libertad: la rueda queda girando sobre el eje del motor, el motor fijo a su soporte. Ahí entran también la placa que exportamos de KiCad y los modelos de catálogo, y desde el mismo modelo salen los planos y la exportación hacia Blender.

**Fuentes:** AutoFinalRobo2-CAD/mecanica/auto-completo/AutoFinalRobo2-Final-20261025.f3d, fusion_ensamble.py, fusion_exportar_blender.py; docs/ARMADO_FUSION_PASO_A_PASO.md; exports/2026-10-24/parts (OBJ para placas, USDZ para el resto).

**Pendiente / no afirmar:** Capturas de Fusion y logo: PC de escritorio. La vista explotada es solo visual, no un orden de montaje.

## 14 · F13 · De Fusion a Blender: un modelo liviano para la Web  
*50 s · acumulado 10:45 · ramas M+P*

> De Fusion exportamos las piezas y las abrimos en Blender, que es un programa libre de modelado, render y animación. Ahí preparamos el modelo para la página: lo renderizamos, recortamos los triángulos que no se ven o que no aportan forma, y unificamos los materiales repetidos sin tocar los colores. El resultado se exporta como GLB comprimido. Pasamos de varios megabytes a menos de uno: es lo que permite que el propio auto sirva su modelo 3D por Wi-Fi.

**Fuentes:** AutoFinalRobo2-CAD/exports/2026-10-24/parts (suma 9.319.792 B, OBJ + USDZ) y exports/2026-10-24/AutoFinalRobo2-ligero.glb (827.232 B); docs/RUEDAS_Y_BLENDER_20261024.md; blender_ligero.py (Decimate); modelo final 25/10: 871.756 B (docs/modelo-final-20261025.json).

**Pendiente / no afirmar:** El par antes/después por defecto es el del 24/10 (misma exportación). Si llega assets_claro/peso_modelo.json de la PC de escritorio con la medición del modelo final, se usa ese.

## 15 · F14 · Fuerzas y momentos modelados  
*55 s · acumulado 11:40 · ramas M*

> El brazo es donde la estática se vuelve concreta. Cada tramo del brazo pesa, y su peso actúa en su centro de masa: son las flechas rojas, más la carga de doscientos gramos en la pinza. Cada fuerza, respecto de una articulación, produce un momento: en forma vectorial es r por F, y en magnitud es la fuerza por la distancia perpendicular, el brazo de palanca. Abajo, el momento flector a lo largo del brazo: crece hacia el hombro porque ahí se suman todos los pesos con sus distancias. Por eso el hombro y el primer eslabón son los más exigidos.

**Fuentes:** analisis_cargas_200g/resultados_estatica.json (caso CAD_macizo_200g: masas por grupo, J2 2,911 N·m); posiciones de los centros de masa leídas de cargas_y_momentos.png; M(x) recalculado por masas concentradas; FOTO-01; Hibbeler, Estática, cap. 4.

**Pendiente / no afirmar:** El análisis estático es de la versión v06 del brazo y el render es v17: aclararlo si se pregunta. El brazo no está aprobado para fabricación ni resistencia.

## 16 · F15 · Esfuerzos internos en una sección  
*45 s · acumulado 12:25 · ramas M*

> Si imaginamos que cortamos el brazo en una sección, para que cada mitad siga en equilibrio aparecen cuatro esfuerzos. La fuerza axial, N, estira o comprime a lo largo del tubo: en la torre es el peso que baja hacia la base. El cortante, V, intenta deslizar una cara sobre la otra. El momento flector, M, dobla el tramo: comprime de un lado y tracciona del otro, y es el que más crece hacia el hombro. Y el torsor, T, lo retuerce. Dos son fuerzas y dos son momentos: no se suman entre sí, cada uno se calcula por separado.

**Fuentes:** analisis_cargas_200g/05_frenada_y_esfuerzos_combinados.md (N, V, T, M y dónde aparecen); Beer y Johnston, Mecánica de materiales.

## 17 · F16 · Elementos de máquinas en el brazo  
*50 s · acumulado 13:15 · ramas M*

> En el brazo aparece casi todo lo que vemos en Elementos de Máquinas. El hombro mueve el brazo con un par de ruedas dentadas de dieciocho y setenta y dos dientes, una reducción de cuatro a uno, sobre un eje apoyado en dos rodamientos. El codo se mueve con una correa dentada y poleas de veinte y cincuenta, porque su motor está junto al hombro para no cargar peso en la punta. La base gira sobre un cartucho con un rodamiento que aguanta carga axial y flexión. Y la torre con la cruz en el medio es diseño propio: transmite todos los esfuerzos a la base, y por eso es la pieza que llevamos a un ensayo por elementos finitos.

**Fuentes:** revision_v17/README.md (engranajes 18/72 módulo 0,8, eje 5 con dos 625, correa HP3M210x6, poleas 20/50, relación 2,5, cartucho 6000, STS3250/STS3032); FOTO-07; Budynas y Nisbett, Shigley.

**Pendiente / no afirmar:** Los puntos de las flechas sobre FOTO-07 son estimados a ojo: se reemplazan por los del render lateral con JSON de la PC de escritorio. Las poleas y engranajes del CAD son discos a diámetro primitivo, no dentados fabricables (README v17). Brazo no aprobado para fabricar.

## 18 · F17 · Ensayo por elementos finitos: torre J1–J2  
*45 s · acumulado 14:00 · ramas M*

> La torre que une la base con el hombro es la pieza por la que pasan todos los esfuerzos del brazo. Para ver cómo se reparten, hicimos un ensayo por elementos finitos: el programa divide la pieza en miles de elementos chicos, aplica las cargas y los apoyos, y resuelve el equilibrio en cada uno. El color muestra la tensión equivalente de von Mises: en azul la pieza casi no trabaja, en rojo está lo más exigido. La cruz en el medio está justamente para repartir esa carga.

**Fuentes:** Estudio de tensión estática de Fusion sobre la torre J1–J2 (PC de escritorio; assets_claro/renders/F_fea_torre_v001.json con material, cargas y resultados); reacciones de analisis_cargas_200g/reacciones_para_FEA.csv.

**Pendiente / no afirmar:** Resultado ilustrativo: material isotrópico macizo; una pieza impresa en PETG con relleno parcial no queda acreditada por este modelo (informe 05 del análisis). No presentar como aprobación de fabricación.

## 19 · F18 · Centro de masa, inercia y vuelco  
*60 s · acumulado 15:00 · ramas M+C*

> Cuando el brazo se estira, el centro de masa del conjunto se desplaza. Sobre el auto actúan el peso propio en el centro de masa, la carga en la pinza, la compresión que baja por la torre y, si frenamos, una fuerza de inercia equivalente hacia adelante. Si el brazo gira hacia el costado, aparece además un momento de giro sobre la base. Mientras el momento del peso respecto del borde de apoyo sea mayor que el de la carga y la inercia, el auto no vuelca. Abajo: el centro de masa es el promedio de las posiciones pesado por la masa; el momento de inercia mide cuánto cuesta hacerlo girar; y la fuerza de inercia es menos masa por aceleración.

**Fuentes:** Hibbeler, Dinámica (centro de masa, momento de inercia, D'Alembert); CAD/mecanica/brazo-antropomorfo/busqueda_actuadores_v13/v14 README (estabilidad condicionada a contactos hipotéticos); FOTO-29; renders lateral y frontal del auto con brazo (PC de escritorio, con JSON de puntos tcp, torre_top, cg, borde).

**Pendiente / no afirmar:** Contactos reales y masa medida del auto pendientes: no declarar vuelco ni margen numérico. El esquema es didáctico hasta que lleguen los renders con brazo.

## 20 · F19 · Despiece del auto  
*30 s · acumulado 15:30 · ramas M*

> Antes de pasar a la electrónica, el auto desarmado: cada pieza del ensamble por separado. Arriba de cada una, su nombre. Es la misma información que tiene el ensamble de Fusion, pero ordenada como un despiece.

**Fuentes:** Renders por objeto del GLB/colección detallada (PC de escritorio, assets_claro/renders/F19_*).

**Pendiente / no afirmar:** Renders del despiece pendientes en la PC de escritorio.

## 21 · F20 · Despiece del brazo  
*25 s · acumulado 15:55 · ramas M*

> Y el brazo desarmado por conjuntos: la base con el giro, la torre, el hombro con sus engranajes, los dos eslabones de PVC, el codo con la correa, la muñeca y la pinza.

**Fuentes:** AFR2_Brazo_v17_Base_V7_J3_Remoto.f3d (PC de escritorio, assets_claro/renders/F20_*); revision_v17/laminas_20261109 (seis conjuntos).

**Pendiente / no afirmar:** Renders del despiece del brazo pendientes en la PC de escritorio.

## 22 · v3-F11 · Del datasheet al circuito  
*40 s · acumulado 16:35 · ramas E*

> Antes de conectar algo leemos su hoja de datos: qué tensión tolera cada pin, qué corriente entrega, qué nivel lógico espera. Por ejemplo, el ADC del ATmega no acepta más tensión que su referencia de cinco volts, y eso define el circuito que vamos a ver para medir la batería. A la izquierda, el pinout del shield; a la derecha, el esquemático real tal como está en KiCad.

**Fuentes:** fotos_existentes/FOTO-12; AutoFinalRobo2-CAD/kicad/oled-encoder/sensorbateria.kicad_sch exportado con kicad-cli 10.99 (capturas/sensorbateria.png).

**Pendiente / no afirmar:** F11_A01: captura del datasheet oficial (pinout + límites) no tomada; guardar URL y revisión del documento.

## 23 · v3-F13 · Batería y ADC  
*40 s · acumulado 17:15 · ramas E*

> Las dos celdas 18650 en serie dan más de cinco volts, y el ADC del ATmega no puede medir más que su referencia. La solución es un divisor con dos resistencias iguales de diez kilo: en el punto medio tenemos exactamente la mitad, con un capacitor de cien nanofaradios para filtrar. El ADC de diez bits convierte eso en un número de cero a mil veintitrés; como dividimos por dos, el firmware multiplica por diez mil en vez de cinco mil y divide por mil veinticuatro. Dos detalles: es una tensión, no un porcentaje de carga, y todavía no está calibrada.

**Fuentes:** contracts/BATTERY_MEASUREMENT_9.md (10 kΩ+10 kΩ, 100 nF, A5/PF5/ADC5, fórmula, prescaler 128, 8 muestras); afr2_sensors.c:30; kicad sensorbateria.kicad_sch.

**Pendiente / no afirmar:** Montaje y calibración no comprobados (según contrato). Rango 2×4,2 V es DERIVADO de la química nominal, no medido.

## 24 · v3-F14 · Encoders Hall y cuadratura  
*45 s · acumulado 18:00 · ramas E+C*

> Cada motor GM25-370 tiene un encoder Hall con dos canales, A y B, desfasados un cuarto de ciclo. El firmware cuenta solo los flancos de subida de A, con una interrupción por cambio de pin, y mira en ese instante el nivel de B: si está alto resta, si está bajo suma. Así sabemos cuánto giró y hacia dónde. Contando pulsos en un intervalo de tiempo obtenemos la velocidad angular. La resolución de trescientos setenta pulsos por vuelta la medimos girando la rueda a mano.

**Fuentes:** afr2_board.c:40-47 (hall_capture, ISR PCINT2, A8–A15); include/afr2_product.h:17 (AFR2_DEFAULT_CPR 370, medido por el usuario); contracts/hardware/PINOUT.md.

**Pendiente / no afirmar:** 370 es DECLARADO (medición manual del usuario): mostrarlo como tal. El código real usa suma en u32 con UINT32_MAX para restar; el snippet lo simplifica a ±1 (indicarlo si se pregunta).

## 25 · v3-F15 · Ultrasonido: tiempo de vuelo  
*45 s · acumulado 18:45 · ramas E*

> El HC-SR04 emite un pulso de ultrasonido y devuelve por el pin de eco un pulso cuyo ancho es el tiempo que tardó el sonido en ir y volver. La distancia es la velocidad del sonido por ese tiempo, dividido dos. En el firmware no esperamos con un bucle: un temporizador interrumpe cada veinticinco microsegundos, detecta el flanco de subida y el de bajada del eco, y si en treinta y dos milisegundos no llegó, informa el error sin trabar el resto del programa. El sensor va sobre un servo, y con eso armamos el radar.

**Fuentes:** afr2_sensors.c:119-148 (Timer3 CTC, OCR3A=49, ÷8 → 25 µs; timeout 1280 ticks; TRIG PC7/D30, ECHO PC6/D31); contracts/SENSORS_RADAR_8.md.

**Pendiente / no afirmar:** Temperatura/velocidad del sonido asumida en la conversión: verificar en el código de conversión antes de mostrar un valor de c.

## 26 · v3-F16 · Sensores de línea  
*45 s · acumulado 19:30 · ramas E+C*

> Cinco emisores y receptores infrarrojos miran el piso. La cinta oscura refleja menos que el piso claro, y cada sensor entrega un bit. El firmware le asigna a cada sensor un peso, de más dos a menos dos, promedia los que ven cinta y obtiene un error: cuánto está la línea corrida respecto del centro. Ese error multiplicado por una ganancia es el giro que se le pide al auto, mientras avanza a velocidad constante. Si deja de ver la línea, se detiene.

**Fuentes:** afr2_product.c:370-388 (pesos 2−i, line_kp, parada sin línea y por cruce ≥150 ms); line_speed 80 y line_kp 100 por defecto (afr2_product.c:27).

**Pendiente / no afirmar:** Polaridad analógica (blanco = ADC alto) no verificada: el modo mostrado es digital. F16_A03 sin lecturas reales: no mostrar números.

## 27 · v3-F17 · I²C y pantalla OLED  
*35 s · acumulado 20:05 · ramas E+P*

> La pantalla habla I²C: dos cables, uno de datos y uno de reloj, que el maestro, el ATmega, genera a cien kilohertz. Como es el único dispositivo del bus, un administrador de I²C es el dueño de las transferencias y nunca se escribe desde una interrupción. La imagen vive en un buffer de mil veinticuatro bytes, uno por cada ocho píxeles, y se manda por bloques. La perilla es un encoder rotativo leído por interrupciones: corto confirma, largo abre los controles rápidos.

**Fuentes:** afr2_twi.c:42 (TWBR=72 → 100 kHz); afr2_oled.c (SSD1306, 128×64 páginas); builds/2026-11-06-04 (buffer 1024 + sombra ACK 1024); AGENTS.md (navegación); afr2_board.c (INT4/INT5).

**Pendiente / no afirmar:** Dirección I²C y tasa real de actualización (F17_A03) no medidas: no mostrar FPS ni dirección. El esquemático alimenta el panel a 3V3 y anota «adaptación de niveles I²C con Mega 5 V: pendiente externa»; la ficha de pinout reporta el panel a 5 V. Resolver antes de presentar la tensión del bus.

## 28 · v3-F18 · Motor DC y servo: dos usos de señales temporizadas  
*40 s · acumulado 20:45 · ramas E+C*

> El motor y el servo reciben pulsos, pero leen cosas distintas. Al motor le llega un PWM de casi mil hertz: lo que importa es qué fracción del período está encendido, porque eso fija la tensión media y por lo tanto la velocidad. Al servo le llega un pulso cada veinte milisegundos, y lo que importa es el ancho: entre uno y dos milisegundos, que el servo traduce en un ángulo de cero a ciento ochenta grados.

**Fuentes:** afr2_motor.c:21-23 (Fast PWM 8 bits, ÷64 → 976 Hz, Timer1/Timer2; OCR por rueda); afr2_sensors.c:149-170 (Timer5, 0,5 µs, 40000 ticks = 20 ms, 1000–2000 µs, PB7/D13).

**Pendiente / no afirmar:** Frecuencia PWM 976 Hz es la configurada en el timer (comentario: «candidate»), no medida con osciloscopio.

## 29 · v3-F19 · Lazo abierto y lazo cerrado  
*45 s · acumulado 21:30 · ramas C*

> En lazo abierto mandamos un PWM y confiamos en que el motor gire a la velocidad esperada. Pero cada motor es distinto, la batería baja y el piso cambia. En lazo cerrado medimos la velocidad con el encoder, la comparamos con la pedida y corregimos con esa diferencia, el error. El auto tiene los dos: el modo PWM manual es lazo abierto, y los modos de conducción, línea y GOTO cierran el lazo.

**Fuentes:** afr2_product.c:420-450 (PID solo si modo ≠ WHEEL_TEST y ≠ PWM_MANUAL); afr2_product.c:428-431.

## 30 · v3-F20 · El controlador se convierte en firmware  
*55 s · acumulado 22:25 · ramas C+P*

> Este es el controlador que dibujamos en bloques, escrito en C. Cada diez milisegundos, para cada rueda, se calcula el error entre la velocidad pedida y la medida, se acumula la integral con un límite para que no crezca sin fin, se combinan proporcional, integral y derivativo, se aplica una corrección por motor, se satura al PWM máximo y se pasa por una rampa para no dar tirones. Con los valores por defecto la derivativa es cero, así que en la práctica es un PI. Todo con enteros, porque el ATmega no tiene unidad de punto flotante.

**Fuentes:** afr2_product.c:12-27 (defaults kp=128, ki=8, kd=0, ramp_step=4, pwm_limit=255), :243 ramp(), :428-445 lazo; main.c:119 (control_ms ≥ 10).

**Pendiente / no afirmar:** Sin log experimental: el gráfico de respuesta queda BLOQUEADO. Los valores de ganancias pueden haberse modificado por EEPROM en el auto real.

## 31 · v3-F21 · Motores diferentes, sistema real  
*30 s · acumulado 22:55 · ramas C*

> Cuatro motores iguales en el catálogo no son iguales en la realidad. Por eso cada rueda tiene un factor de corrección y la odometría tiene factores por dirección. Pero el control tiene límites: si el PWM llega al máximo, no hay más corrección posible. Y el firmware se protege: si una rueda recibe potencia y su encoder no se mueve, o si el signo del encoder quedó invertido, lo que generaría una realimentación positiva, se detiene todo.

**Fuentes:** afr2_product.c:51 (motor_trim 500–1500), :297 (odom_scale por dirección, AFR2-20261111-06), :446 (fault 3 sin cambio Hall 500 ms), :424 (AFR2_FAULT_SIGN_MISMATCH).

**Pendiente / no afirmar:** No concluir «motor defectuoso» sin ensayo.

## 32 · v3-F23 · C, Arduino Core y abstracción  
*30 s · acumulado 23:25 · ramas P*

> En el ATmega programamos en C sobre los registros: para abrir la UART escribimos directamente los bits que fijan la velocidad, el formato y las interrupciones. En el ESP usamos el núcleo Arduino: una sola llamada configura el Wi-Fi como punto de acceso o como estación. Abstraer nos hace más rápidos, pero el hardware sigue ahí: alguien tiene que saber qué hace esa llamada por debajo.

**Fuentes:** afr2_uart1.c:13-19; wifi_service.cpp:98-99; AGENTS.md (ATmega sin Arduino ni DMA).

## 33 · v3-F24 · Wi-Fi, HTTP y carga de la aplicación  
*35 s · acumulado 24:00 · ramas P*

> Primero el teléfono o la computadora se asocia a la red: o al punto de acceso del propio auto, o a la red de casa donde también está el auto. Recién después el navegador pide la página por HTTP. El ESP responde con los archivos ya comprimidos en gzip desde su memoria, y cuando la aplicación arranca abre un WebSocket para el tiempo real. Esos archivos los generamos con Vite: con npm run dev trabajamos en la computadora con recarga instantánea, y con npm run build:esp se compila, se comprime y se copia a la carpeta que se graba en el ESP.

**Fuentes:** Web/package.json (scripts y versiones declaradas en el manifest); ESP/platformio.ini; contracts/PROTOCOL.md (HTTP/LittleFS: gzip obligatorio, 406, no-store, ETag); wifi_service.cpp:98-109; afr2_build_info.h (1.39.6).

**Pendiente / no afirmar:** Las versiones Web son rangos del package.json; las instaladas exactas están en package-lock.json. F24_A02 captura de DevTools no tomada.

## 34 · v3-F25 · El modelo OSI como mapa conceptual  
*45 s · acumulado 24:45 · ramas P*

> El modelo OSI nos sirve como mapa. Arriba, en aplicación, están HTTP y WebSocket y nuestros mensajes. Los datos viajan como JSON, texto. Debajo está TCP, que garantiza que lleguen en orden, después IP y el Wi-Fi. Ojo con dos cosas: es una asociación didáctica, no una implementación capa por capa, y el identificador de pedido que usamos para correlacionar respuestas no es la capa de sesión. A la derecha, el enlace entre el ESP y el ATmega ni siquiera es TCP/IP: es una UART con nuestra propia trama.

**Fuentes:** ESP/platformio.ini (ESPAsyncTCP, ESPAsyncWebServer); contracts/PROTOCOL.md (afr2-ws-1, requestId); UART_PROFILE_2.md.

## 35 · v3-F26 · React ↔ ESP ↔ ATmega  
*40 s · acumulado 25:25 · ramas P+E*

> Sigamos una operación concreta: leer la batería. El navegador manda por WebSocket un mensaje JSON con un identificador de pedido y el nombre del comando. El ESP lo valida y lo traduce a una trama binaria por UART: la cabecera ROBO2, la ruta de ESP a ATmega, el código cero cincuenta y cinco, los números de sesión y transacción, el byte que pide leer y el XOR. El ATmega responde por la ruta inversa con veintitrés bytes, y el ESP los convierte otra vez en JSON para la página. Tres formatos distintos para la misma pregunta.

**Fuentes:** contracts/BATTERY_MEASUREMENT_9.md (0x55, cuerpo 1 B pedido, 23 B respuesta, nombres de campos WS); wire_catalog.json envelope (rutas 0x31/0x13, prefijo session/transaction u32le); Web/src/protocol/afr2Client.ts:567-575 (schema, type 'req', requestId, cmd, payload, ts).

**Pendiente / no afirmar:** Sobre JSON verificado en afr2Client.ts; falta capturar un mensaje y sus bytes reales (F26_A02) antes de presentarlo como captura.

## 36 · v3-F27 · Trama serial, parser y tabla de comandos  
*55 s · acumulado 26:20 · ramas P*

> Una UART solo entrega bytes sueltos; el significado lo pone el protocolo. Nuestra trama empieza con las letras ROBO2, sigue el largo, dos bytes fijos de control, la versión, la ruta, el comando, los datos, y termina con un XOR de todos los bytes anteriores. El receptor es una máquina de estados: avanza un estado por cada byte esperado y, si algo no coincide, se resincroniza buscando otra R. Solo cuando el XOR coincide la trama se entrega al despachador. Y la lista de comandos no está escrita a mano en tres lugares: sale de un único catálogo que leen el firmware, la Web y el Loader.

**Fuentes:** contracts/UART_PROFILE_2.md (tabla de offsets); afr2_wire.h:100-137 (parser y XOR); wire_catalog.json (59 comandos, ws, bytes).

**Pendiente / no afirmar:** Aprobación humana obligatoria del protocolo (F27).

## 37 · v3-F28 · Eventos, estado y notificaciones  
*40 s · acumulado 27:00 · ramas P*

> Cuando llega un byte, la interrupción de la UART solo lo guarda en un buffer circular de ciento veintiocho bytes; no interpreta nada. En el lazo principal el parser arma la trama, el despachador llama al manejador del comando, y ese manejador cambia el estado del auto: el modo, una consigna, una falla. Ese cambio de estado es el que después se ve: por ejemplo, como un aviso en la pantalla, que se guarda en un arreglo de cuatro notificaciones y dura tres segundos.

**Fuentes:** afr2_uart1.c:1-40 (ISR RX/UDRE, RING_MASK 127); include/afr2_screens.h:65 (notification[4]); include/afr2_product.h:15 (AFR2_NOTICE_MS 3000).

**Pendiente / no afirmar:** El informe suponía capacidad 8: el código tiene 4. Política de reemplazo/overflow de las notificaciones: verificar en afr2_screens.c:324 antes de describirla.

## 38 · v3-F29 · Dashboard y modelo 3D  
*40 s · acumulado 27:40 · ramas P+M*

> El mismo modelo del auto que diseñamos en Fusion es el que ve el usuario en la página. Lo pasamos a Blender para ajustar materiales y aligerarlo, lo exportamos como GLB comprimido con Draco, de unos ochocientos ochenta kilobytes, y el ESP lo sirve comprimido en gzip. En el navegador lo dibuja Three.js dentro de React. La aplicación tiene control táctil, el visor del seguidor de línea, el simulador de GOTO, las secuencias, los sensores y la pantalla del auto en vivo.

**Fuentes:** Web/public/models/catalog.json (AutoFinalRobo2.glb 880.232 B); CAD/mecanica/auto-completo/blender_ligero.py, fusion_exportar_blender.py; contracts/WEB_INFRASTRUCTURE.md.

**Pendiente / no afirmar:** Capturas tomadas el 18/11/2026 de la fuente 1.39.6 con npm run dev y Chrome headless 1920×1080, sin auto conectado: no muestran telemetría real. Para F29_A03 (indicadores ↔ sensores) falta una captura con el auto en un estado conocido. Uso de Draco: confirmar en el pipeline antes de afirmarlo.

## 39 · v3-F30 · Flash, LittleFS, RAM y recursos 3D  
*50 s · acumulado 28:30 · ramas P*

> La memoria también es un recurso de ingeniería. En el ATmega tenemos ocho kilobytes de RAM: entre variables inicializadas y no inicializadas usamos unos cinco mil seiscientos bytes, y lo que queda es espacio para la pila, no memoria libre garantizada. En el ESP reducimos más de cuatro kilobytes de RAM estática moviendo textos a flash. Y no hay que confundir: la flash del ESP guarda la aplicación web, la RAM del ESP atiende las conexiones, y el modelo 3D se descomprime y se dibuja en la memoria del navegador, no del auto.

**Fuentes:** documentation/builds/2026-11-06-04/README.md (SRAM 5614, flash 124240, 2578 antes de pila); changes/2026-11-09-09 (ESP 51172→46644 B); build:esp (17 activos 2.487.493 B gzip; presupuesto 2.867.200). Capacidades del chip: datasheets.

**Pendiente / no afirmar:** Builds de 06/11 y 09/11: la versión actual 1.39.6 puede diferir. Antes de presentar, regenerar size/map de la versión que se cargue (el repo y el auto no están alineados).

## 40 · v3-F31 · Sesiones, concurrencia y latencia  
*45 s · acumulado 29:15 · ramas P*

> Un auto que se mueve no puede quedar obedeciendo a un navegador que se colgó. Por eso la página avisa que sigue presente cada medio segundo, y si pasa un segundo y medio sin noticias, el auto se detiene. Cada orden de movimiento vence a los cuatrocientos milisegundos si no se renueva, y la orden de parar tiene prioridad sobre todo. Los identificadores de pedido sirven para saber qué respuesta corresponde a qué pregunta; no son una contraseña. Lo que todavía no medimos es la latencia de punta a punta.

**Fuentes:** contracts/PROTOCOL.md (alive 500 ms, 1500 ms, TTL 400 ms, STOP prioritario); BATTERY_MEASUREMENT_9.md (UART 800 ms, WS 2500 ms); afr2_product.c:322 (web_ms ≥ 1500 → Stop).

**Pendiente / no afirmar:** Política multicliente: el contrato dice «una sesión»; verificar en broker.cpp cómo se rechaza un segundo cliente antes de dibujar dos clientes.

## 41 · v3-F32 · Workflow organizado e IA agéntica  
*75 s · acumulado 30:30 · ramas M+E+C+P*

> Organizar el proyecto no hace el trabajo solo. Lo que hace es partir un sistema complejo en módulos con responsabilidades y contratos claros: firmware del ATmega, firmware del ESP, la Web, el CAD, el Loader para cargar los firmwares, y en el centro Contracts, donde se documenta cada cambio compartido con un identificador y el estado real de cada módulo. Todo vive en un solo repositorio con una única versión de producto. Así, el caso de la batería atraviesa hardware, firmware, protocolo y React, y cada eslabón sabe con qué archivo se conecta. Una herramienta de IA puede ayudarnos a buscar una función, preparar una prueba o implementar un componente; nosotros seguimos definiendo requisitos, arquitectura, seguridad, límites físicos y criterios de aceptación. Esa reducción del trabajo repetitivo nos permite mirar más la vista macro.

**Fuentes:** AGENTS.md raíz (seis módulos, un solo Git, versión única en afr2_build_info.h y package.json); Contracts/docs/ESTRUCTURA_Y_GIT.md, VERSIONADO.md; Loader 2.5.0 (Loader/src/JobState.cs:62); fotos_existentes/FOTO-54.

**Pendiente / no afirmar:** No atribuir al Loader capacidades MCP/IA no demostradas en la versión instalada (2.5.0 en fuente; distribución en raíz 2.0.0).

## 42 · F33 · Cuatro ramas, un sistema  
*45 s · acumulado 31:15 · ramas M+E+C+P*

> Empezamos diciendo que no íbamos a recorrer materias sino un sistema. Vimos que la mecánica define cómo se mueve y cuánto aguanta, la electrónica cómo mide y actúa, el control cómo corrige lo que pasa en la realidad, y la programación cómo todo eso se coordina y se comunica. Ninguna de las cuatro alcanza sola: la mecatrónica no consiste en sumar componentes, sino en integrar conocimientos para resolver problemas reales. Muchas gracias.

**Fuentes:** Render de cierre desde el mismo GLB que la portada.

**Pendiente / no afirmar:** Agregar QR y link al repositorio si se decide publicarlo (idea del equipo).

## 43 · REF · Referencias  
*0 s · acumulado 31:15 · ramas M+E+C+P*

> Respaldo: la bibliografía y los archivos del proyecto que citamos en cada filmina.

**Fuentes:** Catálogo REF del generador.

## 44 · A1 · Anexo · KEY y RST automáticos con NPN  
*0 s · acumulado 31:15 · ramas E+P*

> Respaldo para preguntas. El ESP entra en modo de carga bajando GPIO0 mientras se reinicia. En lugar de apretar dos botones, dos transistores NPN en paralelo con los pulsadores lo hacen comandados por el Mega.

**Fuentes:** presentacion/Filmina_12_Agregado_NPN_KEY_RST.md; Contracts/hardware/PLAN_KEY_RST_AUTOMATICO.md; FOTO-61.

**Pendiente / no afirmar:** Circuito en armado, sin mediciones.

## 45 · A2 · Anexo · AFR2 Loader  
*0 s · acumulado 31:15 · ramas P*

> Respaldo para preguntas: el Loader elige puerto y destino, compila, instala el puente en el Mega para cargar el ESP, verifica y trae un monitor serie que entiende nuestros comandos.

**Fuentes:** presentacion/Filmina_11_Agregado_Loader.md; AutoFinalRobo2-Loader/README.md, MANUAL.html; FOTO-55, FOTO-56.

