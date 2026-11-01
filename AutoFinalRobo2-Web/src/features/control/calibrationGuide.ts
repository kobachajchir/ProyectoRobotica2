/** Explanations of the current numeric actions, keyed by stable contract ID.
 * Sources: ATMEGA_CONFIG_ACTIONS.json, afr2_app.c, afr2_product.c,
 * afr2_sensors.c and the encoder/OLED services. No recommended physical tuning
 * values are inferred from catalogue limits or firmware defaults. */
export interface ParameterGuide {
  title: string; group: string; unit: string; meaning: string; effect: string; example: string; detail: string;
}
const entry = (title: string, group: string, unit: string, meaning: string, effect: string, example: string, detail: string): ParameterGuide =>
  ({ title, group, unit, meaning, effect, example, detail });
export const CALIBRATION_GUIDE: Record<number, ParameterGuide> = {
  81: entry('Tiempo de pulsacion larga','Sistema / Encoder','ms','Umbral que distingue corto y largo al soltar SW.','Se aplica con SW liberado; no cambia el debounce ni introduce tiempo de corto.','800 ms = 0,8 s. Rango 800–3000 ms, pasos de 100 ms.','Guardar desde Preferencias para conservarlo al reiniciar.'),
  68: entry('Invertir servo','Sistema / Servo','0/1','Invierte la correspondencia entre angulo solicitado y pulso del unico servo.','Requiere salidas detenidas y servo/radar desactivados.','0: normal; 1: invertido.','Se aplica en RAM y se conserva mediante Guardar EEPROM.'),
  4: entry('Limite general de velocidad', 'Movimiento', 'mm/s',
    'Es el techo de velocidad usado por el controlador del robot y sus consignas de ruedas. No ordena avanzar ni representa una velocidad medida.',
    'Un valor mayor permite consignas mas rapidas; uno menor limita el movimiento. Si se reduce por debajo de la velocidad de linea configurada, el firmware tambien reduce esa velocidad.',
    '100 mm/s equivalen a 0,1 m/s.', 'Es un limite numerico. Alcanzarlo depende de motores, alimentacion, medicion Hall y calibracion.'),
  6: entry('Velocidad del seguimiento de linea', 'Linea IR', 'mm/s',
    'Es la velocidad de avance que se solicita mientras esta activo el modo de seguimiento de linea.',
    'Al aumentarla el robot recorre la linea mas rapido y necesita corregir en menos tiempo; al reducirla dispone de mas tiempo para reaccionar.',
    '80 mm/s corresponden a 8 cm por segundo de consigna.', 'No puede superar el limite general de velocidad. Cambiarla no inicia el modo Linea.'),
  7: entry('Ganancia de correccion de linea', 'Linea IR', 'mrad/s por unidad de error',
    'Convierte el desplazamiento de la linea respecto del sensor central en una consigna de giro. El firmware promedia los indices activos: -2, -1, 0, 1 y 2.',
    'Una ganancia mayor pide mas giro ante el mismo error; una menor produce una correccion mas suave. No es la Kp del PID de las ruedas.',
    'Ganancia 100 y error medio 1 producen una consigna de giro 100 mrad/s.', 'Los cinco canales, su polaridad y la calibracion de linea deben representar correctamente el suelo.'),
  9: entry('Objetivo relativo X', 'GOTO', 'mm',
    'Prepara la coordenada longitudinal del objetivo GOTO configurado. X positivo apunta hacia delante en el marco relativo del objetivo.',
    'Un valor positivo coloca el destino delante; uno negativo lo coloca detras. No es una lectura de posicion ni inicia una mision.',
    'X 500, Y 0 representa un objetivo relativo a 500 mm hacia delante.', 'Se interpreta junto con Y y con la odometria disponible. El formulario GOTO envia sus propios valores al iniciar una mision.'),
  10: entry('Objetivo relativo Y', 'GOTO', 'mm',
    'Prepara la coordenada lateral del objetivo GOTO. En la convencion del proyecto Y positivo es izquierda y Y negativo es derecha.',
    'El valor cambia el destino lateral configurado; no gira el robot ni mueve las ruedas por si solo.',
    'X 0, Y -300 representa un objetivo relativo a 300 mm hacia la derecha.', 'No confundir coordenadas del destino con velocidades. La mision requiere sus condiciones de odometria y calibracion.'),
  12: entry('Rueda seleccionada', 'Contexto de rueda', 'indice 0…3',
    'Selecciona la rueda que utilizan las operaciones que dependen del contexto de rueda del controlador.',
    '0=FL delantera izquierda; 1=FR delantera derecha; 2=RL trasera izquierda; 3=RR trasera derecha. Elegirla no la acciona.',
    'Valor 2 selecciona RL.', 'Cambiar de rueda reinicia el acumulado y la indicacion de error de la medicion Hall contextual.'),
  13: entry('PWM preparado para ensayo', 'Ensayo de rueda', 'cuentas PWM con signo',
    'Prepara una consigna electrica para la rueda seleccionada. El signo define el sentido solicitado; la magnitud expresa PWM, no velocidad en mm/s.',
    'Cero solicita salida nula. Mayor magnitud solicita mas PWM. Los limites y la rampa del controlador siguen interviniendo.',
    '+80 y -80 tienen igual magnitud con sentidos opuestos; 255 es la escala completa de 8 bits.', 'Es una preparacion: escribir el numero no ejecuta la prueba. No equivale al porcentaje mostrado por todos los formularios de prueba.'),
  14: entry('Duracion del ensayo de rueda', 'Ensayo de rueda', 'ms',
    'Es el tiempo configurado para una prueba individual de rueda, no un periodo de muestreo ni un tiempo permanente de movimiento.',
    'Un valor mayor mantiene la solicitud durante mas tiempo; uno menor acorta la accion. El controlador conserva sus condiciones de parada.',
    '500 ms son medio segundo.', 'Configurar la duracion no inicia un ensayo ni confirma que haya sido ejecutado.'),
  20: entry('Pulso preparado para el servo', 'Cabezal del sensor', 'µs',
    'Prepara el ancho del pulso de control del servo del sensor. Es una orden electrica nominal, no una medicion del angulo del eje.',
    'El intervalo 1000…2000 µs describe el recorrido nominal del programa. La correspondencia exacta con el angulo depende del servo y su ajuste.',
    '1500 µs es la referencia nominal central.', 'Esta accion prepara el pulso; la aplicacion del servo es otra accion. El control de angulo del radar tiene su interfaz propia en Sensores.'),
  25: entry('Tipo de lectura de los sensores de linea', 'Linea IR', '0, 1 o 2',
    'Indica como debe interpretar el firmware los cinco canales de linea: 0=sin tipo definido, 1=digital, 2=ADC analogico.',
    'Digital utiliza niveles 0/1. ADC compara cada lectura con el umbral del canal. Tipo 0 no permite confirmar una linea util para seguimiento.',
    'Valor 2 habilita la interpretacion mediante umbrales ADC.', 'Cambiar el tipo invalida la calibracion de linea anterior; la eleccion debe corresponder al hardware instalado.'),
  26: entry('Inversion de polaridad de los IR', 'Linea IR', 'mascara de 5 bits',
    'Cada bit invierte la interpretacion activa/inactiva de un canal IR despues de leerlo o compararlo con su umbral.',
    'Los pesos 1, 2, 4, 8 y 16 corresponden a IR1…IR5. Se suman los pesos de los canales que se desean invertir.',
    'Valor 5 = 1 + 4 invierte IR1 e IR3; 31 invierte los cinco.', 'No cambia el cableado ni los valores ADC crudos. Modificar la mascara invalida la calibracion de linea.'),
  27: entry('Canal IR seleccionado', 'Linea IR', 'indice 0…4',
    'Selecciona a que sensor corresponde el umbral ADC que se consulta o modifica con el parametro Umbral ADC.',
    '0=IR1, 1=IR2, 2=IR3, 3=IR4 y 4=IR5. Es un selector contextual; no activa ni desactiva ese sensor.',
    'Para editar IR3: seleccionar 2, consultar el umbral y despues modificarlo.', 'La etiqueta humana IR1…IR5 comienza en 1; el numero que se transmite comienza en 0.'),
  28: entry('Umbral ADC del canal seleccionado', 'Linea IR', 'cuentas ADC 0…1023',
    'Es el punto de corte de la lectura analogica del canal IR elegido. Antes de aplicar la inversion, una lectura mayor o igual al umbral se considera activa.',
    'Subirlo exige una lectura mas alta para activar el canal. Bajarlo acepta lecturas menores. La mascara de polaridad puede invertir el resultado.',
    'Umbral 512 separa lecturas menores de 512 de las mayores o iguales.', 'Actua en modo ADC, no como distancia ni porcentaje. Cambiarlo invalida la calibracion de linea.'),
  30: entry('Inversion de sentido de los motores', 'Traccion', 'mascara de 4 bits',
    'Define la correccion de sentido electrico de cada motor para que las consignas con signo coincidan con la convencion del robot.',
    'Los pesos 1, 2, 4 y 8 corresponden a FL, FR, RL y RR. Un bit activado invierte el sentido de esa salida.',
    'Valor 9 = 1 + 8 invierte FL y RR.', 'No es una orden de giro ni modifica la geometria Mecanum. La polaridad del conteo Hall tiene su configuracion propia.'),
  31: entry('Limite de salida PWM', 'Traccion', 'cuentas PWM 0…255',
    'Es la magnitud maxima permitida a la salida del regulador de las ruedas. Es un limite electrico, distinto del limite de velocidad.',
    'Un limite mayor deja utilizar mas PWM; uno menor recorta la salida aunque el regulador pida mas.',
    '128 es aproximadamente 50,2% de la escala 255.', 'No establece una velocidad fisica concreta ni demuestra que el motor pueda alcanzar la consigna con ese limite.'),
  32: entry('Rampa de cambio del PWM', 'Traccion', 'cuentas por ciclo nominal de 10 ms',
    'Limita cuanto puede acercarse el PWM aplicado al PWM solicitado en cada iteracion nominal del control.',
    'Valores mayores permiten cambios mas rapidos. Valores menores suavizan la variacion de la salida y hacen mas lenta su respuesta.',
    'Paso 4 permite variar hasta 4 cuentas por iteracion; no son 4 mm/s.', 'Es una rampa electrica discreta, no una aceleracion mecanica medida. Las condiciones de parada siguen siendo del controlador.'),
  33: entry('Kp del control de ruedas', 'PID de ruedas', 'Q8',
    'Es la contribucion proporcional al error actual entre velocidad solicitada y velocidad medida de cada rueda.',
    'A mayor Kp, mayor correccion para el mismo error. En el firmware el resultado combinado se divide por 256 antes de limitar el PWM.',
    '128/256=0,5 como coeficiente proporcional efectivo.', 'La escala es Q8; no ingresar 128 pensando que equivale a 128,0. No hay un ajuste universal para todos los motores.'),
  34: entry('Ki del control de ruedas', 'PID de ruedas', 'Q8 por iteracion',
    'Multiplica la suma acumulada del error de velocidad. Permite que errores persistentes aporten una correccion aunque sean pequeños.',
    'Aumentarla hace crecer mas rapido la contribucion acumulada; reducirla disminuye ese efecto. El acumulador tiene limites en el firmware.',
    '8/256=0,03125 multiplica el acumulado discreto.', 'La acumulacion ocurre en el lazo nominal de 10 ms. No es directamente una Ki continua en unidades por segundo.'),
  35: entry('Kd del control de ruedas', 'PID de ruedas', 'Q8 por iteracion',
    'Multiplica la diferencia entre el error actual y el de la iteracion anterior: representa una respuesta al cambio del error.',
    'Una Kd mayor da mas peso a cambios rapidos, incluida la variacion de la medicion. Cero elimina esta contribucion.',
    '64/256=0,25 multiplica la diferencia discreta de errores.', 'No se divide por un tiempo en segundos en esta formula. Debe interpretarse junto con Kp, Ki y la medicion Hall.'),
  36: entry('Configuracion de motores habilitada', 'Traccion', '0 o 1',
    'Declara si la configuracion del control de ruedas esta preparada para utilizarse. Es una condicion de habilitacion del producto.',
    '0 mantiene esa condicion deshabilitada; 1 la declara lista. No arranca motores ni sustituye los requisitos de Hall y geometria.',
    'Valor 1 por si solo no inicia Conduccion manual.', 'Es una declaracion de configuracion, no una comprobacion automatica de cableado, sentido o ajuste.'),
  37: entry('Diametro efectivo de la rueda', 'Geometria y odometria', 'mm',
    'Convierte las vueltas de rueda en distancia recorrida. Debe representar el diametro efectivo del conjunto que rueda sobre el suelo.',
    'Para igual cantidad de vueltas, un diametro mayor produce una distancia calculada mayor. Tambien afecta la velocidad estimada.',
    'Para 80 mm, una vuelta corresponde nominalmente a π×80≈251,3 mm.', 'Cambiarlo invalida la declaracion de geometria y la pose vigente. No es el radio de la rueda.'),
  38: entry('Suma geometrica Lx + Ly', 'Geometria Mecanum', 'mm',
    'Es la suma de las distancias longitudinal y lateral del centro del chasis a los centros de rueda. Interviene al combinar traslacion y giro.',
    'Para una misma velocidad angular, una suma mayor exige mas velocidad tangencial de ruedas. Tambien interviene al calcular el giro por odometria.',
    'Lx = 120 mm y Ly = 100 mm producen Lx + Ly = 220 mm.', 'No es el radio de la rueda. Cambiarla invalida la geometria declarada y la pose; se debe determinar con las medidas del robot.'),
  39: entry('Disponibilidad declarada de Hall', 'Geometria y odometria', '0 o 1',
    'Indica si el sistema de medicion Hall de ruedas esta conectado segun la configuracion del producto.',
    '0 declara que no esta disponible. 1 permite considerar ese requisito, pero no demuestra que lleguen pulsos correctos.',
    'Marcar 1 no genera cuentas Hall ni calibra una vuelta.', 'El cambio invalida la pose y la preparacion de odometria. La medicion/calibracion por rueda sigue siendo una operacion separada.'),
  40: entry('Geometria declarada valida', 'Geometria y odometria', '0 o 1',
    'Es la confirmacion de que las medidas geometricas configuradas estan listas para usarse.',
    '0 mantiene el requisito pendiente. 1 lo declara cumplido si la configuracion es admitida por el firmware.',
    'No se puede declarar geometria util con Lx+Ly igual a 0.', 'No mide el chasis ni calibra sensores. Cambiar la declaracion invalida la pose y requiere reconstruir la odometria.'),
  41: entry('Transiciones del encoder de la OLED', 'Interfaz local', '2 o 4 por paso',
    'Define cuantas transiciones electricas del encoder de navegacion se agrupan para producir un paso de seleccion en la OLED.',
    'Cambia la relacion entre el giro de la perilla y el avance por el menu. El firmware admite exactamente 2 o 4, nunca 3.',
    'Con 4 se requieren cuatro transiciones para un paso del menu.', 'Este parametro corresponde a la perilla de usuario; no es el numero de pulsos Hall por vuelta de una rueda.'),
  42: entry('Sentido del encoder de la OLED', 'Interfaz local', '0 o 1',
    'Invierte el sentido con que el giro de la perilla desplaza la seleccion de la interfaz local.',
    '0 conserva el sentido configurado como normal; 1 invierte la direccion de navegacion.',
    'Si girar a la derecha retrocede en el menu, esta opcion cambia esa correspondencia.', 'No invierte ruedas, servos ni el conteo de odometria. Tampoco modifica la accion corta/larga del pulsador SW.'),
  44: entry('Modo Wi-Fi solicitado', 'Comunicacion', '1, 2 o 3',
    'Prepara el modo Wi-Fi solicitado: 1=STA, 2=AP, 3=AP+STA. STA usa una red existente; AP crea una red para conectar clientes.',
    'La eleccion cambia el modo solicitado del borrador de red. No equivale a una conexion confirmada ni aplica credenciales por si sola.',
    '3 permite solicitar el funcionamiento simultaneo de AP y STA.', 'La aplicacion y confirmacion de la conexion pertenecen al flujo de Wi-Fi del ESP; escribir este numero no prueba que la red haya cambiado.'),
  53: entry('Contraste de la pantalla OLED', 'Pantalla', 'nivel 0…255',
    'Ajusta el byte de contraste enviado al controlador SSD1306 de la OLED.',
    'Valores mayores solicitan mas contraste/intensidad y valores menores lo reducen. No es un porcentaje de la pantalla Web.',
    '128 es un valor intermedio de la escala de 8 bits.', 'No cambia la resolucion ni el contenido de las pantallas. La apariencia final depende del modulo y su alimentacion.'),
  54: entry('Orientacion de la pantalla OLED', 'Pantalla', '0 o 1',
    'Selecciona la orientacion de presentacion de la OLED para adaptarla al montaje fisico.',
    '0 conserva la orientacion normal; 1 solicita la rotacion de 180°. No se ingresa el numero 180.',
    'Valor 1 equivale a elegir la orientacion invertida de la pantalla.', 'No rota el robot del simulador ni modifica el sistema de coordenadas de movimiento.'),
};

export function explainEditorValue(id: number, value: number) {
  if (!Number.isFinite(value)) return 'Ingresa un numero valido para interpretar este valor.';
  if (id >= 33 && id <= 35) return `${value} / 256 = ${(value / 256).toFixed(5)} como coeficiente Q8 efectivo.`;
  if (id === 26 || id === 30) {
    const names = id === 26 ? ['IR1', 'IR2', 'IR3', 'IR4', 'IR5'] : ['FL', 'FR', 'RL', 'RR'];
    return names.map((name, bit) => `${name}: ${(value & (1 << bit)) !== 0 ? 'invertido' : 'normal'}`).join(' · ');
  }
  if (id === 12) return ['FL · delantera izquierda', 'FR · delantera derecha', 'RL · trasera izquierda', 'RR · trasera derecha'][value] ?? 'Indice de rueda fuera de rango.';
  if (id === 27) return `Canal IR${value + 1}; el indice transmitido es ${value}.`;
  if (id === 25) return ['Sin tipo definido', 'Lectura digital', 'Lectura analogica ADC'][value] ?? 'Tipo fuera de rango.';
  if (id === 44) return ({ 1: 'STA · red existente', 2: 'AP · punto de acceso', 3: 'AP + STA' } as Record<number, string>)[value] ?? 'Modo fuera de rango.';
  if ([36, 39, 40, 42].includes(id)) return value === 1 ? 'Opcion declarada activa (1).' : value === 0 ? 'Opcion declarada inactiva (0).' : 'Solo se admiten 0 o 1.';
  if(id===81)return `${value} ms = ${(value/1000).toFixed(1)} s; largo se ejecuta al soltar.`;
  if (id === 54) return value === 1 ? 'Orientacion solicitada 180°.' : value === 0 ? 'Orientacion normal.' : 'Solo se admiten 0 o 1.';
  if (id === 37) return `Una vuelta equivale nominalmente a ${(Math.PI * value).toFixed(1)} mm para este diametro.`;
  if (id === 31) return `Limite equivalente a ${(value / 255 * 100).toFixed(1)}% de la escala PWM.`;
  return `${value} ${CALIBRATION_GUIDE[id]?.unit ?? ''} en el editor. Todavia no implica una aplicacion al robot.`;
}
