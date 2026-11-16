"""Adaptación documental D1 a D2. Autoría base: Koba Chajchir.
No importa ni ejecuta código de producto o generadores heredados.
"""
from pathlib import Path
import json, copy, shutil

HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[2]
BASE=HERE.parent/'informe_20261109'
raw=json.loads((BASE/'contenido_informe.json').read_text(encoding='utf-8'))['blocks']
groups=[]
for b in raw:
    if b['kind']=='chapter': groups.append([])
    groups[-1].append(copy.deepcopy(b))
def p(t): return {'kind':'paragraph','text':t}
def h(t): return {'kind':'heading','text':t}
def e(t): return {'kind':'equation','text':t}
def m(label,rows): return {'kind':'matrix','label':label,'rows':rows}
def table(head,rows): return {'kind':'table','head':head,'rows':rows,'widths':None}
def note(t): return {'kind':'callout','text':t}
def fig(path,caption): return {'kind':'image','path':path,'caption':caption}
def add(i,*blocks): groups[i].extend(blocks)
def replace_text(old,new):
    for g in groups:
        for b in g:
            if 'text' in b: b['text']=b['text'].replace(old,new)

# El D1 se conserva intacto. La portada se compone en el generador.
groups[0]=[{'kind':'chapter','text':'AutoFinalRobo2'},
p('Informe de presentación y desarrollo del proyecto de robótica'),
p('Plataforma móvil Mecanum y manipulador antropomorfo de seis ejes'),
p('Autoría documental de base: Koba Chajchir. Revisión D2 · 12 de noviembre de 2026.'),
fig('figuras/robot_ensamblado.png','Robot completo representado con geometría CAD v17. Render de diseño, no fotografía de un montaje físico.')]
replace_text('Este borrador','Este informe')
replace_text('este borrador','este informe')
replace_text('del borrador','del informe')
replace_text('derivaciones del borrador','derivaciones del modelo')
replace_text('Este borrador y el nuevo libro amplían esa cobertura.','El informe y el libro complementario amplían esa cobertura.')
replace_text('del primer borrador','del proyecto')
replace_text('Conclusiones del primer borrador','Conclusiones del proyecto')
replace_text('El nuevo libro complementario','El libro complementario del 09/11')
replace_text('El nuevo libro extiende','El libro complementario extiende')

# Suprimir texto de trabajo editorial que ya se resuelve en esta edición.
for g in groups[1:]:
    g[:]=[b for b in g if not (b.get('kind')=='paragraph' and any(s in b.get('text','') for s in [
        'Este texto es un plan; no implica que se haya ejecutado.',
        'El informe final deberá',
        'Hasta entonces, esta revisión debe mantenerse identificada como borrador D1.',
        'Todavía deben materializarse y medirse',
        'Se proponen láminas de marcos',
        'BioLog se toma como referencia de organización',
        'El informe se entrega en PDF para lectura',
        'El cierre del informe requerirá',
    ]))]

# Fuentes y realidad experimental actualizadas al corte documental.
groups[19][-1]=p('El proyecto cuenta con mediciones de base aportadas por el equipo el 12/11/2026, resumidas en este capítulo. Esos datos no se extienden al manipulador, a otra superficie, a una carga distinta ni a una versión de firmware posterior. No se atribuyen al conjunto completo precisión, carga útil, autonomía o tasa de agarre que no hayan sido medidas.')
for g in groups:
    for b in g:
        if b.get('kind')=='table':
            b['rows']=[[str(v).replace('No se dispone aquí de','No se dispone de') for v in row] for row in b['rows']]
replace_text('La integración física del bus de servos','La integración física del bus de servos')
replace_text('no presenta como terminadas las piezas que carecen de tolerancias de fabricación ni la integración física del bus de servos','mantiene pendientes la liberación de tolerancias de fabricación y la integración física del bus de servos')
replace_text('La masa del auto1.5kg','La masa del auto 1,5 kg')
replace_text('Una comprobación posterior deberá comparar poses conocidas entre la ecuación, el descriptor Web y mediciones del mecanismo. No se presentan errores RMS ni precisiones del TCP porque no se realizaron esas mediciones.',
             'El contraste de poses conocidas debe vincular la ecuación, el descriptor Web y una medición externa del mecanismo. La precisión y repetibilidad del TCP permanecen sin caracterización experimental.')

# Resumen académico y lectura del informe.
groups[1][1:1]=[
h('Resumen ejecutivo'),
p('AutoFinalRobo2 integra una plataforma de cuatro ruedas Mecanum con el diseño de un manipulador de seis articulaciones y una pinza. El desarrollo combina mecánica CAD, control embebido, sensado, conectividad Wi-Fi y una interfaz Web. Su aporte central es relacionar cada consigna con una cadena explícita de transformaciones, conversiones de unidades, medición y actuación.'),
p('La base dispone de mediciones de avance, desplazamiento lateral y giro aportadas por el equipo. El brazo cuenta con geometría v17 y un modelo analítico de pose, mientras la fabricación, potencia y operación física de la manipulación siguen pendientes de cierre. El análisis histórico de estabilidad identifica una limitación frontal: un factor estático 1,023 frente al criterio 1,2 adoptado. Por ello se diferencia el espacio geométrico de una envolvente operativa segura.'),
note('Resultado principal: una arquitectura documentada y un modelo matemático integrado de movilidad y manipulación. Las prestaciones físicas se declaran únicamente para las condiciones medidas.'),
p('Palabras clave: robótica móvil, ruedas Mecanum, manipulador 6R, cinemática, Jacobiano, odometría, control embebido, estabilidad.')]
add(1,h('1.3 Metodología de desarrollo'),
p('El trabajo se organiza como un proceso de ingeniería iterativo. Primero se identifican las restricciones del kit y los objetivos de movilidad y manipulación. Luego se fijan ejes, unidades y responsabilidades de cada módulo. La geometría CAD determina los parámetros del modelo cinemático; el firmware convierte las consignas en actuación y registra mediciones; los resultados de la base retroalimentan la calibración de odometría y las tolerancias de llegada.'),
table(['Etapa','Actividad','Salida y criterio de cierre'],[
['Definición','Problema, alcance y restricciones.','Requisitos vinculados a evidencia y responsables.'],
['Diseño','CAD, interfaces, transmisiones y arquitectura.','Geometría identificada por revisión; interfaces documentadas.'],
['Modelado','Transformaciones, matrices y leyes de control.','Símbolos, unidades, hipótesis y límites explícitos.'],
['Implementación','Fuentes ATmega, ESP, Web y herramientas.','Versiones, catálogo compartido y procedencia por archivo.'],
['Caracterización','Medidas externas y datos crudos del equipo.','Comparación consigna–odometría–medida con condiciones registradas.'],
['Iteración','Corrección de escala y llegada.','Nueva versión separada de la evidencia anterior.']]),
p('La revisión D2 amplía el D1 del 09/11 y conserva su autoría. Los desarrollos de modelo describen relaciones analíticas; los resultados históricos mantienen fecha y condiciones. Los campos académicos de portada pueden completarse sin alterar el cuerpo técnico.'))

add(2,h('2.2 Matriz de trazabilidad de requisitos'),
table(['ID y necesidad','Modelo o solución','Evidencia y aceptación'],[
['R01 Movilidad planar','Matriz 4×3 y pseudoinversa; capítulos 11 y 12.','Medidas de base del 12/11; caracterización ampliada pendiente.'],
['R02 Posicionar herramienta','Cadena 6R y TCP; capítulos 6 a 9.','Descriptor v17; contraste físico del TCP pendiente.'],
['R03 Sujetar pieza','Pinza de dos cremalleras; capítulo 10.','Apertura CAD; fuerza real y retención pendientes.'],
['R04 Operación local','OLED y encoder; capítulos 13 y 18.','Interacción documentada; prueba por versión de producto.'],
['R05 Operación remota','Web, WS, ESP y UART; capítulo 18.','Estados y comandos trazables; ACK separado de resultado.'],
['R06 Estabilidad','Centro de masa y momentos; capítulo 15.','Histórico frontal no cumple FS 1,2.'],
['R07 Reproducibilidad','Fuentes, BOM y planos; capítulos 4 y 23.','Revisiones y origen conservados; fabricación pendiente.']]),
p('El sistema de producto excluye MPU, balance de dos ruedas y firmware AT en el ESP. El ATmega usa C con avr-libc, sin Arduino ni DMA. El ESP usa Arduino ESP8266 y LittleFS. La OLED es el único dispositivo I2C y su acceso se concentra en el I2C Manager.'),
fig('figuras/arquitectura.png','Arquitectura funcional de plataforma, interfaces y herramientas. El bus serie y la potencia del manipulador constituyen una integración separada, todavía por cerrar.'))

add(3,h('3.1 Decisiones mecánicas y justificación'),
p('La revisión v17 acorta las palancas principales y reubica el motor de J3 cerca del hombro. Reducir la distancia de una masa al eje disminuye su contribución al par gravitatorio. Para un mismo motor, la comparación aislada τ = m g ℓ muestra el efecto de esa decisión; el balance completo debe agregar poleas, correa, ejes, soportes e inercias.'),
p('Los tubos de PVC aportan estructura tubular con poca masa nominal. Las uniones, rodamientos y soportes impresos resuelven interfaces entre ejes y actuadores. La rigidez real depende de longitud libre, orientación de impresión, ajustes y fijaciones; la elección de material no constituye por sí sola una verificación resistente.'),
table(['Decisión','Motivo técnico','Compromiso'],[
['Base Mecanum de cuatro ruedas','Avance, lateral y giro sin orientar previamente el chasis.','Mayor sensibilidad al piso y al deslizamiento lateral.'],
['Manipulador de seis ejes','Separar posición y orientación de la herramienta.','Masa, holguras y calibración de varios ejes.'],
['J3 remoto con correa','Acercar masa de accionamiento al hombro.','Tensión, elasticidad y transmisión que caracterizar.'],
['Reducción 4:1 en J2','Aumentar par disponible a la salida.','Menor velocidad y necesidad de evaluar diente y fijación.'],
['Pinza simétrica','Mantener centro de agarre entre mandíbulas.','Fricción y contacto real determinan retención.']]))

add(4,h('4.2 Gestión de configuración y fabricación'),
p('Una definición de pieza identifica geometría; una ocurrencia identifica una instancia montada. La BOM debe conservar ambas cantidades para evitar subestimar masa y compras. Una hoja nominal comunica forma y dimensiones, mientras un plano de fabricación agrega material, proceso, tolerancias, referencias de inspección y acabado.'),
p('La documentación complementaria del 09/11/2026 contiene 224 hojas A3 organizadas en seis tomos, para 66 definiciones fabricadas o adaptadas y seis conjuntos. Se debe consultar junto con los archivos editables y los registros de interfaces. La revisión del plano debe permanecer vinculada a la geometría que la originó. [S11]'))

add(5,h('5.2 Coordenadas homogéneas y rotaciones'),
p('Al ampliar un punto tridimensional a cuatro componentes, la traslación se incorpora a un producto matricial. La última componente vale 1 para puntos y 0 para vectores libres. Una fuerza o una velocidad angular no reciben la traslación aplicada a un punto.'),
e('p̃_A = T_AB p̃_B ;   p̃ = [x, y, z, 1]ᵀ'),
e('RᵀR = I₃ ;   det(R) = +1'),
m('Rotación elemental alrededor de X',[['1','0','0'],['0','cos α','−sin α'],['0','sin α','cos α']]),
p('Las condiciones de ortogonalidad y determinante distinguen una rotación propia de una matriz que incluye escala o reflexión. Los ángulos se convierten a radianes antes de evaluar seno y coseno. Las transformaciones se aplican de derecha a izquierda sobre un vector columna.'),
note('Convención del informe: T_AB lleva coordenadas del marco B al marco A. El subíndice inicial identifica el marco de expresión final.'))

add(6,h('6.2 Matrices elementales del modelo'),
m('Rotación homogénea alrededor de Z',[['cos q','−sin q','0','0'],['sin q','cos q','0','0'],['0','0','1','0'],['0','0','0','1']]),
m('Rotación homogénea alrededor de Y',[['cos q','0','sin q','0'],['0','1','0','0'],['−sin q','0','cos q','0'],['0','0','0','1']]),
m('Traslación homogénea sobre Z',[['1','0','0','0'],['0','1','0','0'],['0','0','1','ℓ'],['0','0','0','1']]),
p('La cadena multiplica estas matrices en el orden declarado. Las rotaciones Y inclinan eslabones inicialmente orientados sobre Z. Esa elección explica que el alcance horizontal aparezca con senos y la altura con cosenos. No se usa la lectura del servo como q sin aplicar antes transmisión, cero y signo.'),
fig('figuras/cadena.png','Cadena esquemática de marcos y transformaciones del manipulador móvil. Los desplazamientos y ángulos provienen del descriptor v17; el dibujo expresa relaciones y no constituye un plano a escala.'))

add(7,h('7.2 Orientación de muñeca desarrollada'),
p('Para escribir explícitamente la orientación distal se define cᵢ = cos(qᵢ) y sᵢ = sin(qᵢ), con i = 4, 5, 6. La matriz siguiente corresponde a Rz(q4) Ry(q5) Rz(q6); se premultiplica por Rz(q1) Ry(q2+q3) para obtener la orientación completa.'),
m('Rotación Z Y Z de la muñeca',[
['c4 c5 c6 − s4 s6','−c4 c5 s6 − s4 c6','c4 s5'],
['s4 c5 c6 + c4 s6','−s4 c5 s6 + c4 c6','s4 s5'],
['−s5 c6','s5 s6','c5']]),
h('7.3 Ejemplo analítico de lectura de la cadena'),
p('Con q1…q6 = 0, los eslabones están alineados con Z en el modelo matemático: ρ = 0, R = I y z_TCP = h + L1 + L2 + d = 0,5375 m, medido desde la montura. Es una configuración ideal singular; el resultado no acredita acceso físico sin colisión ni una altura medida del robot.'),
p('La pose de transporte [0°, 15°, −40°, 0°, 25°, 0°] produce β = −25° y una orientación ideal R = I, porque β + q5 = 0. Sustituir esos ángulos en las ecuaciones permite interpretar cómo dos articulaciones compensan la inclinación de la herramienta. La postura utilizada en un montaje debe conservar además margen de estabilidad y de interferencia.'))

add(8,h('8.2 Procedimiento completo de selección'),
table(['Paso','Operación','Rechazo o decisión'],[
['1','Llevar la pose objetivo al marco de la montura.','No mezclar objetivos de mundo con longitudes locales.'],
['2','Restar d R_d e_z para hallar el centro de muñeca.','Identificar correctamente TCP y orientación.'],
['3','Evaluar c3 y las ramas de q1, q2 y q3.','Rechazar objetivos fuera de alcance o límites.'],
['4','Extraer q4, q5, q6 de R_W y su rama equivalente.','Resolver singularidad por continuidad.'],
['5','Comparar pose reconstruida, límites y colisiones.','Mantener separadas coherencia algebraica y viabilidad física.'],
['6','Elegir candidato cercano a la postura actual.','Priorizar estabilidad y márgenes operativos.']]),
e('C(q) = Σᵢ wᵢ wrap(qᵢ − qᵢ,actual)²'),
p('La función C es un criterio posible de continuidad entre candidatos factibles; los pesos dependen de velocidades, inercias y margen a límites. No compensa una colisión ni autoriza una postura inestable. La solución analítica general descrita aquí amplía la explicación académica del modelo; la función Web disponible sigue siendo una rama de agarre descendente restringida.'))

add(9,h('9.3 Jacobiano del centro de muñeca'),
p('Al derivar p_W respecto de q1, q2 y q3 se obtiene una matriz 3×3 que deja visibles las singularidades de posición. Se define a = L1 cos q2 + L2 cos β y b = L2 cos β, con β = q2 + q3. El Jacobiano de TCP completo agrega la contribución de la orientación al desplazamiento d.'),
m('Derivada de la posición del centro de muñeca',[
['−ρ sin q1','a cos q1','b cos q1'],
['ρ cos q1','a sin q1','b sin q1'],
['0','−ρ','−L2 sin β']]),
e('|det(J_W)| = |ρ L1 L2 sin q3|'),
p('La expresión muestra dos causas de pérdida de rango: ρ = 0 y sin q3 = 0. La singularidad de muñeca se agrega cuando sin q5 = 0. El número de condición de un Jacobiano completo requiere una escala que haga comparables velocidades lineales y angulares; no se deben mezclar metros y radianes sin declarar ese criterio.'))

add(10,h('10.2 Conversión entre articulación y accionamiento'),
e('qᵢ = σᵢ (θ_motor,i − θ0,i) / iᵢ ;   σᵢ ∈ {−1,+1}'),
p('θ0 es el cero de accionamiento y σ incorpora el sentido de montaje. La unidad nativa del servo debe convertirse a radianes usando la variante y el modo realmente configurados. Una resolución del sensor interno no asegura que la salida tenga esa exactitud debido a holgura, elasticidad y transmisión.'),
h('10.3 Retención estática del objeto'),
e('2 μ F_mandíbula ≥ m_obj g ;   FS_agarre = 2 μ F_mandíbula / (m_obj g)'),
p('La condición corresponde a dos contactos simétricos sobre caras verticales, sin aceleración y con coeficiente de fricción μ. Una carga de 0,2 kg tiene un peso aproximado de 1,961 N; para asignar una fuerza de pinza y un factor de seguridad falta medir fricción, rigidez, límites del servo y presión admisible sobre la pieza. La relación no es una capacidad útil garantizada.'))

add(11,h('11.2 De la velocidad planar a cada rueda'),
p('Para la rueda i situada en (xᵢ,yᵢ), el movimiento rígido induce vᵢ = (v_x − ω yᵢ, v_y + ω xᵢ). La restricción de rodadura asociada a la inclinación de los rodillos proyecta esa velocidad sobre la dirección accionada. Con rodillos a 45° y la disposición adoptada aparecen los signos alternados y el término L = l_x + l_y de la matriz A.'),
e('Aᵀ A = diag(4, 4, 4L²) ;   A⁺ = (Aᵀ A)⁻¹ Aᵀ'),
p('Cuatro mediciones de rueda reconstruyen tres velocidades de cuerpo. La redundancia permite formar un residuo ε = u − A S V_est. Un residuo grande señala incompatibilidad con el modelo ideal, pero no identifica por sí solo qué rueda patina ni detecta todos los deslizamientos comunes.'),
h('11.3 Ejemplo de consigna de avance'),
p('Para V = (0,100 m/s, 0, 0), con S = −I, se obtiene u = (−0,100; −0,100; −0,100; −0,100) m/s. Con r = 0,040 m, cada velocidad angular vale −2,5 rad/s, aproximadamente −23,87 rpm. El signo negativo corresponde al contrato lógico vigente y debe convertirse al sentido eléctrico de cada puente H.'),
note('El límite de 150 mm/s es un parámetro nominal de software. La relación entre PWM, velocidad, tensión y carga debe caracterizarse en el montaje.'))

# Actualizar odometría: el punto medio ya está en la fuente.
replace_text('12.1 Integración temporal propuesta','12.1 Integración temporal implementada en fuente')
replace_text('Se propone la orientación de punto medio para reducir el error de integración respecto de usar únicamente el ángulo inicial.',
             'La fuente ATmega utiliza la orientación de punto medio al integrar incrementos de rueda, antes de publicar x, y y rumbo.')
add(12,h('12.3 Escala de odometría por dirección'),
p('La fuente vigente aplica seis factores separados a los incrementos medidos del cuerpo antes de rotarlos al marco global. Los factores se eligen por el signo de cada componente y no multiplican PWM, velocidad de rueda ni la meta de distancia. Los valores de fábrica consultados son 1,000 para avance y retroceso, 0,775 para laterales y 1,028 para ambos giros. Una configuración guardada puede contener otros valores. [S12, S13]'),
m('Corrección por tramo con signos fijos',[['Δx_B,c'],['Δy_B,c'],['Δψ_c']]),
e('[Δx_B,c ; Δy_B,c ; Δψ_c] = diag(k_x, k_y, k_ψ) [Δx_B ; Δy_B ; Δψ]'),
e('k_nuevo = k_actual × distancia_real / distancia_objetivo'),
p('La regla anterior corresponde a una misión detenida cuando la odometría alcanza su objetivo: si se recorren 775 mm reales al contar 1000 mm, el factor pasa de 1 a 0,775. Para calibrar con un tramo físico fijado externamente se compara, en cambio, distancia real con odometría del mismo tramo. No deben intercambiarse ambos cocientes.'),
p('Ejemplo documental: contar 1000 mm laterales sin corrección equivalía a 775 mm reales en las mediciones de referencia. Con factor 0,775, esos mismos incrementos se publican como 775 mm y el control necesita más cuentas para completar una meta de 1000 mm. El factor representa una calibración empírica bajo ciertas condiciones; no elimina la dependencia con piso, carga y aceleración.'),
p('Los factores se guardan en milésimas, rango 0,000 a 2,000. Cero impide iniciar la componente correspondiente en control cerrado. La revisión 1.28.1 introduce EEPROM v13, con payload de 78 bytes, y migra en RAM factores de giro antiguos iguales a 1,000 al valor 1,028; no guarda automáticamente. Aplicar escala reinicia el marco de pose para evitar mezclar trayectorias con calibraciones distintas.'),
fig('figuras/odometria.png','Flujo conceptual de medición e integración. Los factores direccionales corrigen componentes de cuerpo antes de la rotación global; el resultado conserva deriva acumulativa.'))

# Sustituir el texto del control genérico por el control real, sin atribuir antiwindup no existente.
start=next(i for i,b in enumerate(groups[13]) if b.get('kind')=='heading')
groups[13][1:start]=[
p('El control de velocidad por rueda disponible en la fuente compara consigna y velocidad estimada por Hall. El firmware comparte ganancias, pero mantiene acumulador e historial de error por rueda. PWM manual y prueba PWM evitan este lazo. Por ello una prueba de potencia y una misión cerrada no tienen las mismas condiciones de funcionamiento. [S4]'),
e('eᵢ[k] = sat₂₀₀₀(u_ref,i[k] − u_med,i[k])'),
e('Eᵢ[k] = sat₃₂₇₆₇(Eᵢ[k−1] + eᵢ[k])'),
e('PWM_obj,i[k] = sat_límite((Kp eᵢ[k] + Ki Eᵢ[k] + Kd(eᵢ[k]−eᵢ[k−1])) / 256)'),
p('Las ganancias se almacenan en Q8: el divisor 256 fija su escala. Los valores iniciales consultados son Kp = 128, Ki = 8 y Kd = 0. El acumulador suma error por ciclo sin Δt explícito; la ley depende del período de servicio. Para expresarla en forma continua equivalente deben incluirse el tiempo de muestreo y las unidades de mm/s usadas por el firmware.'),
p('La salida atraviesa un límite de PWM y una rampa antes de escribirse al puente H. El acumulador tiene recorte numérico; ese recorte no equivale a una compensación completa de saturación. Una mejora posible es condicionar la integración cuando la acción satura y el error empuja en el mismo sentido. Esa mejora requiere diseño y ajuste, y no se presenta como implementada.'),
note('PWM representa una acción eléctrica. La velocidad se estima por Hall; su correspondencia con avance real depende también de la adherencia y de los rodillos Mecanum.')]
add(13,h('13.3 Planificación de duración y límites'),
e('max |dqᵢ/dt| = (15/8) |qf,i−q0,i| / T'),
e('max |d²qᵢ/dt²| = (10/√3) |qf,i−q0,i| / T²'),
e('T ≥ maxᵢ {15 |Δqᵢ|/(8 v_max,i), √[10 |Δqᵢ|/(√3 a_max,i)]}'),
p('Estas cotas se deducen de las derivadas del perfil quíntico del apartado 13.1. Se aplican a una transición articular aislada con condiciones iniciales y finales de reposo. El límite efectivo también debe contemplar par, carga, calor, velocidad del servo, transmisión y estabilidad del conjunto.'),
h('13.4 GOTO y errores en el marco del cuerpo'),
m('Conversión del error mundial al cuerpo',[['cos ψ','sin ψ'],['−sin ψ','cos ψ']]),
e('[e_x,B ; e_y,B] = Rz(ψ)ᵀ₂×₂ [x_d−x ; y_d−y]'),
e('e_ψ = wrap(ψ_d−ψ) ;   [v_x ; v_y] ≈ K_xy [e_x,B ; e_y,B]'),
p('En la fuente consultada K_xy equivale nominalmente a 1/s antes de límites. La corrección de rumbo usa ganancia 3/s durante traslación y 1/s en giro puro. Se mantienen mínimos para superar zona muerta y se normaliza la mezcla con el límite de misión. El GOTO depende de pose válida, calibración y vigencia de la misión; no es una localización absoluta.'),
p('La revisión 1.28.1 define llegada por |e_x| y |e_y| ≤ 4 mm y |e_ψ| ≤ 10 mrad, aproximadamente 0,57°. Estas son bandas de decisión de software, no una precisión física demostrada. Los resultados del 12/11 corresponden a 1.28.0 con bandas anteriores de 10 mm y 25 mrad. La respuesta de la nueva revisión queda pendiente. [S13]'))

add(14,h('14.2 Jacobiano integrado explícito'),
p('Si p = (p_x,p_y,p_z) es la ubicación del TCP respecto del origen de la base, las tres columnas siguientes relacionan velocidades de cuerpo (v_x,v_y,ω) con la velocidad de TCP expresada en ejes de la base. El Jacobiano del brazo debe trasladarse y rotarse a ese mismo marco antes de concatenarlo.'),
m('Contribución cinemática de la base al TCP',[['1','0','−p_y'],['0','1','p_x'],['0','0','0'],['0','0','0'],['0','0','0'],['0','0','1']]),
e('J_total = [J_base   J_brazo] ∈ ℝ⁶ˣ⁹'),
p('La estructura de nueve coordenadas permite redundancia para una tarea de pose de seis componentes, siempre que el rango y las restricciones lo permitan. La pinza agrega una coordenada de apertura, sin agregar una séptima componente a la pose rígida de la herramienta. Para analizar movimientos simultáneos se deben considerar también fuerzas inerciales y márgenes de vuelco.'))

add(15,h('15.3 Energía y estructura del modelo dinámico'),
e('K = ½ Σᵢ [mᵢ v_Giᵀv_Gi + ωᵢᵀ I_Gi ωᵢ]'),
e('U = Σᵢ mᵢ g₀ z_Gi ;   L = K − U'),
e('d/dt(∂L/∂q̇ᵢ) − ∂L/∂qᵢ = τᵢ + τ_ext,i − τ_f,i'),
p('La energía cinética combina traslación y rotación de cada cuerpo. La energía potencial usa g₀ positivo y altura z; de ella se obtiene g_q = ∂U/∂q, el término de compensación gravitatoria de la ecuación dinámica. Las inercias se expresan con orientación y origen coherentes. Para referir una inercia al eje se emplea el teorema de ejes paralelos:'),
e('I_O = R I_G Rᵀ + m [(rᵀr) I₃ − r rᵀ]'),
p('Una distribución uniforme en la envolvente de un servo es una aproximación; su rotor, reductor e inercia reflejada pueden modificar el par dinámico. El análisis debe separar capacidad nominal continua, pico transitorio y par de bloqueo.'),
h('15.4 Rigidez y resistencia de los eslabones'),
e('A = π(D²−dᵢ²)/4 ;   I = π(D⁴−dᵢ⁴)/64 ;   J_p = π(D⁴−dᵢ⁴)/32'),
e('σ_f = M c/I ;   τ_t = T c/J_p ;   δ = F ℓ³/(3 E I)'),
p('Las expresiones corresponden a un tubo circular ideal y, para δ, a una viga en voladizo con carga puntual y pequeñas deformaciones. En el robot, uniones, abrazaderas, agujeros, apoyos y piezas FDM cambian las condiciones. No se asigna una tensión admisible a PLA impreso sin proceso, orientación y caracterización.'),
h('15.5 Margen al polígono de apoyo'),
e('h_e(r_G) = n_eᵀ (r_G,xy − a_e) ;   h_e > 0 para todos los bordes'),
p('a_e es un punto del borde y n_e su normal interior unitaria. El mínimo de h_e mide la distancia estática proyectada al borde bajo el modelo de apoyo. Una condición operativa necesita margen adicional para incertidumbre de masas, pendiente y aceleración. El factor por momentos no debe confundirse con un factor resistente de material.'),
fig('figuras/estabilidad.png','Análisis histórico del 01/11/2026 sobre apoyo nominal. El margen frontal de aproximadamente 0,85 mm y el factor 1,023 son resultados de ese modelo, no una medición del contacto real.'))

add(16,h('16.1 Presupuesto eléctrico y energía'),
e('P_total = Σⱼ Vⱼ Iⱼ ;   I_bat ≈ Σⱼ(Vⱼ Iⱼ/ηⱼ) / V_bat'),
e('E_bat,nom = V_nom C_Ah [Wh] ;   t_estimado = E_util / P_media'),
p('La autonomía exige capacidad útil, tensión bajo carga y potencia media medidas. Deben sumarse pérdidas de conversores y consumos simultáneos. La corriente de bloqueo sirve para evaluar una condición extrema de protección, no para estimar directamente el funcionamiento continuo.'),
h('16.2 Adquisición ADC de batería'),
e('R_th = R_sup R_inf/(R_sup+R_inf) ;   τ_RC = R_th C'),
p('Para 10 kΩ y 10 kΩ con 100 nF, R_th = 5 kΩ y τ_RC = 0,5 ms. El filtro reduce componentes rápidas; el tiempo de establecimiento y la impedancia de fuente deben ser compatibles con la adquisición ADC. La referencia real, tolerancias del divisor y caída de masa afectan la lectura de batería.'),
h('16.3 Interfaces de servicio y compatibilidad'),
p('La operación del enlace ESP–ATmega usa D18/D19. El selector E_RX/E_TX del shield y la cabecera directa ESP UART de 3,3 V cumplen funciones distintas. No se supone que una foto demuestre adaptación de niveles. La identificación de flash de 4 MiB está documentada por una consulta USB–TTL del usuario del 27/10; no certifica layout, modo SPI ni contenido cargado.'),
p('El mantenimiento vigente utiliza el flasher de software por D18/D19 con RESET del Mega libre. La ruta histórica D0/D1 con RESET del Mega a GND fue retirada el 06/11; su presencia en antecedentes no la convierte en el procedimiento actual. D32/D33 se destinan a KEY/RESET del ESP mediante transistores NPN y resistencias; la integración se debe identificar por su evidencia específica.'),
table(['Interfaz','Asignación','Observación'],[
['Motores FR / FL','IN D22,D24 / D26,D28; PWM D9 / D10.','Salidas de puente H; no alimentar motores desde GPIO.'],
['Motores RR / RL','IN D5,D6 / D7,D8; PWM D11 / D12.','Aplicar signos configurados por rueda.'],
['Hall A/B','FR A8/A12; FL A9/A13; RR A10/A14; RL A11/A15.','El orden de headers difiere del orden lógico de matrices.'],
['Línea y batería','IR1…IR5 A4…A0; batería A5.','IR digitales en configuración vigente.'],
['OLED y encoder','SDA/SCL D20/D21; CLK/DT D2/D3; SW D42.','I2C Manager; ningún botón menú adicional.'],
['Radar y enlace','TRIG/ECHO D30/D31; servo D13; UART1 D18/D19.','La consigna del servo no mide su ángulo real.']]))

add(16,h('16.4 Modelo electromecánico de un motor DC'),
p('Para explicar la relación entre acción eléctrica y velocidad se considera un motor DC equivalente, con corriente i, resistencia R_m, inductancia L_m, fuerza contraelectromotriz K_e ω_m, constante de par K_t e inercia J_m. Es un modelo de diseño, sin parámetros identificados del motor instalado.'),
e('v_m = R_m i + L_m di/dt + K_e ω_m'),
e('J_m dω_m/dt = K_t i − b_m ω_m − τ_carga'),
m('Matriz de estado con x = [i, ω_m]ᵀ',[
['−R_m/L_m','−K_e/L_m'],['K_t/J_m','−b_m/J_m']]),
e('dx/dt = A_m x + [v_m/L_m ; −τ_carga/J_m]'),
p('El puente H cambia el sentido de tensión y el PWM modifica su valor medio efectivo. En un modelo ideal v_media ≈ s D V_bus, con D entre 0 y 1 y s = ±1. Caídas en transistores, recirculación, batería y rozamiento alteran la respuesta; por eso un porcentaje PWM no representa un porcentaje garantizado de velocidad.'),
p('En una reducción mecánica de relación i_r se tiene ω_rueda = ω_motor/i_r y, idealmente, el par de salida se multiplica por i_r y por la eficiencia. La inercia de una carga referida al motor se divide por i_r². El radio efectivo relaciona finalmente giro de rueda y avance, bajo la hipótesis de rodadura que los rodillos Mecanum solo satisfacen aproximadamente.'))

add(17,h('17.3 Metrología y propagación de incertidumbre'),
e('σ_d² ≈ (Δt/2)² σ_c² + (c/2)² σ_Δt²'),
p('La aproximación supone errores pequeños e independientes en velocidad del sonido y tiempo de eco. La incertidumbre por orientación de superficie, eco espurio y ancho del haz puede exceder la contribución de cuantización. Se debe conservar el dato inválido como tal y registrar el tiempo de adquisición.'),
p('En la barra digital vigente se calcula un centroide de posiciones discretas de sensores activos. La fuente usa pesos transversales 2, 1, 0, −1, −2 y una corrección proporcional de rumbo. La polaridad y el orden deben corresponder a la barra física. Si no hay bits de línea válidos, la fuente detiene el seguimiento e informa la condición; no representa una línea centrada. [S4]'))

add(18,h('18.3 Flujo de comando y resultado'),
table(['Paso','Responsabilidad','Dato que debe conservarse'],[
['Solicitud','Web u operación local','Acción, unidades, parámetros y correlación.'],
['Transporte','ESP / broker WS–UART','Versión compatible, integridad y destino.'],
['Aceptación','ATmega según estado y condiciones','Respuesta de aceptación o rechazo.'],
['Ejecución','Servicio de control y salidas','Estado efectivo, causa de parada y medida.'],
['Observación','Telemetría y visualización','Marca temporal, validez y antigüedad.']]),
p('Una respuesta de aceptación no demuestra movimiento completado. El final de una misión debe observarse mediante estado y resultados asociados. Las secuencias locales pueden continuar sin que la WebSocket permanezca abierta cuando el modo y sus condiciones lo permiten; no se puede generalizar que toda pérdida de navegador detiene cualquier operación. La vigencia de comandos manuales y el STOP conservan su tratamiento específico.'),
h('18.4 Recursos de tiempo y memoria'),
p('El control se atiende nominalmente cada 10 ms. La fuente supervisa un intervalo de servicio superior a 100 ms cuando hay movimiento y corta salidas; los fallos repetidos tienen tratamiento adicional. El tiempo de render OLED, adquisición y comunicación compite por el mismo procesador. Un presupuesto estático de memoria no demuestra margen suficiente de pila o comportamiento de peor caso.'),
p('El análisis de recursos se vincula siempre a una compilación fechada. Por ejemplo, la evidencia del 09/11 registró una reducción de RAM estática ESP de 51.172 a 46.644 bytes. La modificación posterior de fuentes no queda validada por ese valor histórico. No se infiere heap disponible durante descarga 3D ni estabilidad WebSocket a partir de la cifra estática. [S14]'),
h('18.5 Interfaz local y mantenimiento'),
p('El panel local usa una OLED monocroma de 128×64 y un encoder con pulsador. El corto confirma o alterna; desde dashboard abre el menú completo 010002. El largo se ejecuta al soltar después de al menos 800 ms: desde dashboard abre rápidas 010003 y desde otros contextos vuelve al anterior. Volver explícito conserva ese retorno. Si hay cambios pendientes, la decisión Sí/No es obligatoria.'),
p('El Loader separa operación, diagnóstico, compilación y carga. La selección de una solicitud no acredita su ejecución y la escritura de una imagen no demuestra su funcionamiento en placa. Los registros deben conservar imagen, versión y resultado de cada operación; este informe no modifica ni ejecuta esas herramientas.'))

# Añadir resultados actuales sin reemplazar el estado pendiente del brazo.
add(19,h('19.2 Mediciones de base aportadas el 12 de noviembre'),
p('La ficha AFR2-20261112-01 resume una tanda de ajuste con firmware 1.28.0, velocidad de consigna 100 mm/s, sin carga y cinco repeticiones por tipo de movimiento. Las estadísticas siguientes se transcriben de esa evidencia; el resumen no define el significado estadístico del signo ± ni permite recuperar por sí solo instrumento, incertidumbre o todos los datos crudos. [S13]'),
table(['Movimiento','Medida externa','Error final y deriva'],[
['Avance 1000 mm','989 a 991 mm','Error −10,2 ± 0,8 mm; odometría−medida −0,2 ± 0,5 mm.'],
['Derecha −1000 mm','−989 a −992 mm','Error +10,2 ± 1,3 mm; deriva informada 0,0 mm.'],
['Giro +90° CCW','+91°','Error +1,0°; odometría−medida −2,2 ± 0,1°.'],
['Giro −90° CW','−91°','Error −1,0°; odometría−medida +2,0 ± 0,1°.']]),
p('En los tramos rectos, el error de llegada cercano a 10 mm coincide con la banda anterior de finalización; una odometría cercana a la referencia no obliga al controlador a agotar exactamente la meta. En los giros se superponen la tolerancia de parada y una escala angular guardada distinta de la esperada. Esta separación evita corregir una escala lineal para resolver un error originado en la condición de llegada.'),
h('19.3 Cambio propuesto y alcance de la evidencia'),
p('La fuente 1.28.1 reduce las bandas a 4 mm y 10 mrad, migra factores de giro y eleva la ganancia de rumbo durante traslación. Su estado documentado es fuente ATmega editada sin compilar ni cargar; ESP y Web solo actualizan versión. Las bandas menores describen el comportamiento esperado del controlador, sin constituir un nuevo resultado experimental.'),
p('Las mediciones sin carga no caracterizan el robot con brazo y objeto, el desempeño lateral sobre otro piso ni la estabilidad dinámica. La deriva observada en orientación requiere una referencia externa si el deslizamiento no aparece en los encoders. El sistema no incorpora una IMU que permita suponer esa corrección.'),
h('19.4 Métricas para ampliar la caracterización'),
e('e_j = y_med,j − y_ref,j ;   sesgo = (1/n) Σⱼ e_j'),
e('RMSE = √[(1/n) Σⱼ e_j²] ;   s_e = √[Σⱼ(e_j−sesgo)²/(n−1)]'),
p('El sesgo cuantifica error sistemático, RMSE combina sesgo y dispersión, y s_e describe dispersión de errores alrededor de su media. La repetibilidad del TCP exige repetir una pose bajo condiciones constantes; la exactitud compara con una referencia externa. No se calculan estas métricas del brazo sin sus mediciones.'))

replace_text('No se publican porcentajes de éxito, precisión, velocidad máxima, carga útil garantizada ni autonomía porque no existe en esta entrega una campaña de medidas que los respalde. El primer resultado del trabajo es una base documental y matemática consistente para preparar esas mediciones.',
             'Los resultados disponibles se presentan por subsistema y condición de ensayo. La evidencia de base no caracteriza la exactitud del TCP, la fuerza de pinza, la carga útil garantizada, la velocidad máxima ni la autonomía del conjunto completo.')

add(20,h('20.2 Aportes propios del desarrollo'),
p('El trabajo adapta el control a ATmega2560 con servicios AVR propios y organiza una única fuente de contratos para firmware, Web y herramientas. Incorpora operación local OLED/encoder, diagnóstico, sensado de batería, mantenimiento KEY/RESET y un Loader separado. La mecánica integra una revisión antropomorfa compacta con transmisión remota de codo y documentación por pieza.'),
p('El valor académico se concentra en conectar esos aportes: la dimensión CAD llega a la matriz, la matriz define una consigna, la consigna produce una acción supervisada y la medición externa permite revisar la interpretación. Las autorías y fuentes de referencia se conservan; una adaptación de plataforma no convierte en propios los diseños o resultados ajenos.'))

add(21,h('21.1 Matriz cualitativa de riesgos técnicos'),
p('Se propone una valoración documental de severidad S y probabilidad P de 1 a 5; prioridad R = S×P. Las probabilidades son juicios de ingeniería para organizar el trabajo y no frecuencias medidas. La tabla requiere revisión al conocer el montaje real.'),
table(['Riesgo y causa','S / P / R','Tratamiento y evidencia de cierre'],[
['Vuelco con brazo frontal; margen histórico insuficiente.','5 / 4 / 20','Restringir postura operativa; pesar conjunto y revisar apoyo antes de carga.'],
['Bus o potencia del brazo sin cierre de integración.','4 / 4 / 16','Definir niveles, distribución, fusibles y comportamiento bajo picos.'],
['Pérdida de rumbo por patinamiento.','4 / 3 / 12','Medida externa, control de condiciones y calibración por superficie.'],
['Interferencia o ajuste mecánico no liberado.','4 / 3 / 12','Planos funcionales, tolerancias e inspección de montaje.'],
['Cero o signo incorrecto entre motor, Hall y modelo.','4 / 3 / 12','Identificación por rueda/eje y registro; no corregir solo el visor.'],
['Fallo de control por tiempo o memoria insuficiente.','4 / 3 / 12','Presupuesto por versión y observación dinámica autorizada.'],
['Confundir simulación con prestación física.','3 / 3 / 9','Etiquetas de evidencia y conclusiones por condición medida.']]),
h('21.2 Secuencia de cierre del proyecto'),
p('La siguiente etapa debe cerrar primero interfaces eléctricas y mecánicas; después definir una envolvente de trabajo restringida por estabilidad y colisiones. La caracterización de base y brazo debe realizarse por separado antes de una secuencia combinada de recogida. Los ensayos deben registrar condiciones, instrumentos, datos crudos, referencia externa y criterios de aceptación acordados.'),
table(['Hito','Dependencia','Documento de cierre'],[
['H1 Diseño liberable','Materiales, tolerancias y componentes reales.','Planos y BOM revisados.'],
['H2 Integración eléctrica','Potencia, bus STS y protecciones.','Esquema y presupuesto eléctrico.'],
['H3 Calibración','Ceros, signos, CPR y masas.','Registro de parámetros por versión.'],
['H4 Caracterización','Instrumentos y protocolo acordados.','Mediciones con incertidumbre y condiciones.'],
['H5 Manipulación móvil','Hitos anteriores y envolvente segura.','Tasa de éxito y tiempo de ciclo del escenario definido.']]))

# Conclusiones sin predicciones de prestaciones.
groups[22]=[groups[22][0],
p('AutoFinalRobo2 reúne una plataforma móvil Mecanum, un diseño de brazo de seis ejes y una arquitectura de software distribuida. El modelo documentado relaciona coordenadas de mundo, base, montura y herramienta mediante transformaciones homogéneas; desarrolla cinemática directa e inversa, Jacobianos, transmisiones, medición y estructura dinámica.'),
p('La base cuenta con una primera evidencia experimental de avance, lateral y giro en condiciones sin carga. Esa evidencia permite distinguir error de escala, deriva y tolerancia de llegada, y motivó ajustes de firmware registrados en 1.28.1. La respuesta de esos ajustes todavía debe medirse: una modificación de fuente no sustituye un resultado.'),
p('La geometría v17 y sus planos permiten comunicar el diseño del manipulador. La fabricación, los ajustes funcionales, la potencia y el bus serie del brazo continúan como condiciones de cierre. El análisis histórico de estabilidad detecta un margen frontal por debajo del criterio adoptado; por lo tanto no se declara capacidad operativa cargada en todo el alcance.'),
p('La contribución del proyecto es una integración trazable entre diseño, matemática, control e interfaz. Su continuidad debe concentrarse en convertir los modelos en resultados medidos con condiciones explícitas, manteniendo las limitaciones de estabilidad y la correspondencia entre revisión de software, configuración y prototipo.')]

add(23,h('23.1 Fuentes incorporadas en la revisión D2'),
table(['ID','Documento local','Aporte'],[
['S12','Contracts/contracts/DIRECTIONAL_SCALE_FACTORS_20261111.md','Factores direccionales, semántica de odometría y persistencia.'],
['S13','Contracts/changes/2026-11-12-01-calibracion-final-goto.md','Tanda de mediciones 1.28.0 y ajustes de fuente 1.28.1.'],
['S14','Contracts/changes/2026-11-09-09-esp-memory-optimization.md','Evidencia histórica de memoria estática ESP.'],
['S15','Contracts/hardware/FLASHING.md y PLAN_KEY_RST_AUTOMATICO.md','Separación entre operación y mantenimiento vigente.'],
['S16','Contracts/docs/informe_20261109/contenido_informe.json','Borrador D1 de Koba Chajchir, base de esta adaptación.']]),
p('R4. Lynch, K. M. y Park, F. C. Modern Robotics: Mechanics, Planning, and Control. Cambridge University Press, 2017. Sitio oficial de autores y material complementario: https://hades.mech.northwestern.edu/index.php/Modern_Robotics . Referencia de formalismo; los parámetros y resultados del proyecto se identifican por las fuentes S1–S16.'),
p('Consulta de bibliografía externa para esta revisión: 12/11/2026. La ficha STS3250 citada corresponde al modelo y versión del documento identificado; otras variantes o fichas posteriores no se consideran equivalentes sin cotejo.'))
groups[25]=[groups[25][0],
p('Esta edición se acompaña de una versión PDF de lectura, una versión Word editable y fuentes Markdown/JSON. Las figuras mecánicas se conservan desde la geometría y los renders existentes; los esquemas explican relaciones y no reemplazan planos de fabricación.'),
p('El libro complementario de planos está en AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/laminas_20261109/. El borrador D1 permanece conservado en Contracts/docs/informe_20261109/. Los informes maestros y los originales de Documentacion/ no son sustituidos.'),
p('Las envolventes de trabajo y sus prompts diferidos permanecen en Contracts/docs/pendientes/espacio-trabajo-brazo/. Un contorno de alcance geométrico debe distinguirse del espacio libre de colisiones y del espacio operativo limitado por estabilidad.'),
table(['Dato académico editable','Contenido a completar'],[['Institución','[Completar]'],['Asignatura / curso','[Completar]'],['Docente','[Completar]'],['Integrantes','[Completar]'],['Comisión / ciclo lectivo','[Completar]']])]

# Corregir afirmaciones históricas del D1 que quedaron superadas.
for b in groups[19]:
    if b.get('kind')=='table' and b.get('head',[None])[0]=='Área':
        for row in b['rows']:
            if row[0]=='Base': row[1]='Matriz Mecanum, CPR nominal y tanda de base del 12/11/2026.'; row[2]='Campaña ampliada con carga, otras superficies y deriva angular.'
for b in groups[21]:
    if b.get('kind')=='heading' and b['text']=='21. Pendientes y decisiones para la siguiente revisión': b['text']='21. Pendientes y decisiones de integración'
    if b.get('kind')=='table':
        for row in b['rows']:
            if 'Completar carátula' in row[1]: row[1]='Completar datos académicos editables.'; row[2]='Institución, integrantes, docente y comisión.'

# Copiar solamente insumos existentes; no ejecutar scripts heredados.
groups[1]=[b for b in groups[1] if not (b.get('kind')=='table' and b.get('head',[None])[0]=='Elemento') and not b.get('text','').startswith('El proyecto integra una base de cuatro ruedas')]
source_table=next(b for b in groups[4] if b['kind']=='table')
groups[4]=[groups[4][0],
p('La documentación del brazo se apoya en la copia nativa v17 capturada el 02/11/2026. El primer libro contenía 14 láminas A3. El complemento del 09/11 amplía la cobertura a 224 hojas A3 en seis tomos, con 66 definiciones fabricadas o adaptadas, incluidas 30 definiciones impresas, y seis conjuntos. [S2, S11]'),
p('Cada definición incorpora vistas ortogonales, cotas nominales, representación sólida y cortes de referencia. Las hojas de interfaces agrupan superficies cilíndricas coaxiales para informar ejes, diámetros y tramos; no se cuentan caras como agujeros independientes. Los despieces conservan orientación relativa y listas por ocurrencia, sin afirmar una secuencia de desmontaje comprobada.'),
source_table,
h('4.1 Condiciones para liberar fabricación'),
p('Se deben definir material, proceso, orientación y compensación de impresión, ajustes de ejes y rodamientos, tolerancias, roscas, longitud de enganche, espesores y acabado. Los engranajes y poleas requieren un perfil fabricable o una referencia comercial; un disco de paso no es un diente. Los horns y fijaciones se contrastan con el componente comprado.'),
h('4.2 Gestión de configuración y cantidades'),
p('Una definición identifica geometría y una ocurrencia identifica cada instancia montada. La BOM debe conservar ambas cantidades para no subestimar masa ni compras. Las dimensiones extraídas del CAD son nominales; el plano de taller agrega referencias funcionales y criterios de inspección. Cada revisión de plano debe quedar vinculada al modelo que la originó.')]
# La cadena integrada acompaña al capítulo del manipulador móvil.
chain_fig=next(b for b in groups[6] if b.get('path')=='figuras/cadena.png')
groups[6].remove(chain_fig);groups[14].append(chain_fig)
replace_text('20.1 Visuales que aportan al informe final','20.1 Espacio de trabajo y restricciones')
replace_text('Las burbujas o envolventes de trabajo lateral y superior quedan como pendiente ligado al aviso de diseño terminado que solicitó el usuario. Deben salir de geometría, límites, TCP y criterio de colisión declarados, con contorno alcanzable distinto del contorno operativo seguro. Esta entrega no ejecuta los prompts diferidos ni inventa radios de alcance para imitarlos.',
             'La representación futura del espacio de trabajo deberá derivarse de la geometría, los límites articulares y el TCP. El conjunto de poses alcanzables debe diferenciarse del espacio libre de colisiones y de la región admisible por estabilidad. Un radio de alcance aislado no describe esas restricciones ni certifica capacidad de manipulación.')
replace_text('Las envolventes de trabajo y sus prompts diferidos permanecen en Contracts/docs/pendientes/espacio-trabajo-brazo/. Un contorno de alcance geométrico debe distinguirse del espacio libre de colisiones y del espacio operativo limitado por estabilidad.',
             'El análisis posterior de envolventes debe separar alcance geométrico, espacio libre de colisiones y espacio operativo limitado por estabilidad. La documentación de continuidad se conserva en Contracts/docs/pendientes/espacio-trabajo-brazo/.')
for b in groups[21]:
    if b.get('kind')=='table':
        for row in b['rows']:
            if 'Avisar que el diseño' in row[1]:row[1]='Cerrar geometría y restricciones de envolventes.';row[2]='Espacio alcanzable, libre de colisiones y operativo por separado.'
for g in groups:
    for b in g:
        if b.get('kind')=='table':
            b['rows']=[[v.replace('derivaciones del borrador','derivaciones del modelo').replace('Este borrador y el nuevo libro amplían esa cobertura.','El informe y el libro complementario amplían esa cobertura.') for v in row] for row in b['rows']]
# El cierre de metodología no debe crear una página aislada de metadatos.
groups[1]=[b for b in groups[1] if not b.get('text','').startswith('La revisión D2 amplía el D1')]
(HERE/'figuras').mkdir(exist_ok=True)
assets={
 'robot_ensamblado.png':BASE/'figuras/robot_ensamblado.png',
 'brazo_v17.png':BASE/'figuras/brazo_v17.png',
 'estabilidad.png':ROOT/'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/analisis_conjunto_20261101/estabilidad_planta.png',
}
for name,src in assets.items(): shutil.copyfile(src,HERE/'figuras'/name)
# Diagramas D2 se dibujan desde relaciones documentadas por el autor del generador.
B=[b for g in groups for b in g]
for b in B:
    if b.get('kind')=='image':
        # Las figuras originales se renumeran en la composición.
        import re
        b['caption']=re.sub(r'^Figura \d+\. ','',b['caption'])
(HERE/'contenido_informe.json').write_text(json.dumps({'revision':'D2','date':'2026-11-12','base':'D1 2026-11-09','author':'Koba Chajchir','blocks':B},ensure_ascii=False,indent=2),encoding='utf-8')
print(f'Contenido D2: {len(B)} bloques; {sum(b["kind"]=="chapter" for b in B)} secciones principales.')
