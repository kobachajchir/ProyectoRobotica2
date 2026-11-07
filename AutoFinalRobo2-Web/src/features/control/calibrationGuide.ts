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
  81: entry('Tiempo de pulsación larga','Sistema / Encoder','ms','Umbral que distingue corto y largo al soltar SW.','Se aplica con SW liberado; no cambia el debounce ni introduce tiempo de corto.','800 ms = 0,8 s. Rango 800–3000 ms, pasos de 100 ms.','Guardar desde Preferencias para conservarlo al reiniciar.'),
  68: entry('Invertir servo','Sistema / Servo','0/1','Invierte la correspondencia entre ángulo solicitado y pulso del único servo.','Requiere salidas detenidas y servo/radar desactivados.','0: normal; 1: invertido.','Se aplica en RAM y se conserva mediante Guardar EEPROM.'),
  4: entry('Límite general de velocidad', 'Movimiento', 'mm/s',
    'Es el techo de velocidad usado por el controlador del robot y sus consignas de ruedas. No ordena avanzar ni representa una velocidad medida.',
    'Un valor mayor permite consignas más rápidas; uno menor limita el movimiento. Si se reduce por debajo de la velocidad de línea configurada, el firmware también reduce esa velocidad.',
    '100 mm/s equivalen a 0,1 m/s.', 'Es un límite numérico. Alcanzarlo depende de motores, alimentación, medición Hall y calibración.'),
  6: entry('Velocidad del seguimiento de línea', 'Línea IR', 'mm/s',
    'Es la velocidad de avance que se solicita mientras está activo el modo de seguimiento de línea.',
    'Al aumentarla el robot recorre la línea más rápido y necesita corregir en menos tiempo; al reducirla dispone de más tiempo para reaccionar.',
    '80 mm/s corresponden a 8 cm por segundo de consigna.', 'No puede superar el límite general de velocidad. Cambiarla no inicia el modo Línea.'),
  7: entry('Ganancia de corrección de línea', 'Línea IR', 'mrad/s por unidad de error',
    'Convierte el desplazamiento de la línea respecto del sensor central en una consigna de giro. El firmware promedia los indices activos: -2, -1, 0, 1 y 2.',
    'Una ganancia mayor pide más giro ante el mismo error; una menor produce una corrección más suave. No es la Kp del PID de las ruedas.',
    'Ganancia 100 y error medio 1 producen una consigna de giro 100 mrad/s.', 'Los cinco canales, su polaridad y la calibración de línea deben representar correctamente el suelo.'),
  9: entry('Objetivo relativo X', 'GOTO', 'mm',
    'Prepara la coordenada longitudinal del objetivo GOTO configurado. X positivo apunta hacia delante en el marco relativo del objetivo.',
    'Un valor positivo coloca el destino delante; uno negativo lo coloca detras. No es una lectura de posición ni inicia una misión.',
    'X 500, Y 0 representa un objetivo relativo a 500 mm hacia delante.', 'Se interpreta junto con Y y con la odometría disponible. El formulario GOTO envia sus propios valores al iniciar una misión.'),
  10: entry('Objetivo relativo Y', 'GOTO', 'mm',
    'Prepara la coordenada lateral del objetivo GOTO. En la convención del proyecto Y positivo es izquierda y Y negativo es derecha.',
    'El valor cambia el destino lateral configurado; no gira el robot ni mueve las ruedas por sí solo.',
    'X 0, Y -300 representa un objetivo relativo a 300 mm hacia la derecha.', 'No confundir coordenadas del destino con velocidades. La misión requiere sus condiciones de odometría y calibración.'),
  12: entry('Rueda seleccionada', 'Contexto de rueda', 'índice 0…3',
    'Seleccioná la rueda que utilizan las operaciones que dependen del contexto de rueda del controlador.',
    '0=FL delantera izquierda; 1=FR delantera derecha; 2=RL trasera izquierda; 3=RR trasera derecha. Elegirla no la acciona.',
    'Valor 2 selecciona RL.', 'Cambiar de rueda reinicia el acumulado y la indicación de error de la medición Hall contextual.'),
  13: entry('PWM preparado para ensayo', 'Ensayo de rueda', 'cuentas PWM con signo',
    'Prepara una consigna eléctrica para la rueda seleccionada. El signo define el sentido solicitado; la magnitud expresa PWM, no velocidad en mm/s.',
    'Cero solicita salida nula. Mayor magnitud solicita más PWM. Los límites y la rampa del controlador siguen interviniendo.',
    '+80 y -80 tienen igual magnitud con sentidos opuestos; 255 es la escala completa de 8 bits.', 'Es una preparación: escribir el número no ejecuta la prueba. No equivale al porcentaje mostrado por todos los formularios de prueba.'),
  14: entry('Duración del ensayo de rueda', 'Ensayo de rueda', 'ms',
    'Es el tiempo configurado para una prueba individual de rueda, no un periodo de muestreo ni un tiempo permanente de movimiento.',
    'Un valor mayor mantiene la solicitud durante más tiempo; uno menor acorta la acción. El controlador conserva sus condiciones de parada.',
    '500 ms son medio segundo.', 'Configurar la duración no inicia un ensayo ni confirma que haya sido ejecutado.'),
  20: entry('Pulso preparado para el servo', 'Cabezal del sensor', 'µs',
    'Prepara el ancho del pulso de control del servo del sensor. Es una orden eléctrica nominal, no una medición del ángulo del eje.',
    'El intervalo 1000…2000 µs describe el recorrido nominal del programa. La correspondencia exacta con el ángulo depende del servo y su ajuste.',
    '1500 µs es la referencia nominal central.', 'Esta acción prepara el pulso; la aplicación del servo es otra acción. El control de ángulo del radar tiene su interfaz propia en Sensores.'),
  25: entry('IR digital habilitado', 'Línea IR', '0 o 1',
    '0 deshabilita la adquisición; 1 lee los cinco niveles digitales del módulo instalado.',
    'El preset cambia la sensibilidad del módulo, no una escala analógica en la Web.',
    'Valor 1 habilita lectura digital.', 'Cambiar habilitación invalida la calibración; no inicia seguimiento.'),
  26: entry('Inversión de polaridad de los IR', 'Línea IR', 'mascara de 5 bits',
    'Cada bit invierte la interpretación activa/inactiva de un canal IR después de leer su nivel digital.',
    'Los pesos 1, 2, 4, 8 y 16 corresponden a IR1…IR5. Se suman los pesos de los canales que se desean invertir.',
    'Valor 5 = 1 + 4 invierte IR1 e IR3; 31 invierte los cinco.', 'No cambia el cableado ni los niveles eléctricos originales. Modificar la mascara invalida la calibración de línea.'),
  27: entry('Tomar fondo', 'Línea IR', 'muestra digital',
    'Captura los cinco niveles eléctricos sobre el fondo.', 'Inicia una calibración nueva.',
    'Colocar los cinco sensores sobre el fondo y tomar muestra.', 'No es un parámetro numérico; requiere el comando de calibración.'),
  28: entry('Tomar línea', 'Línea IR', 'muestra digital',
    'Captura los cinco niveles sobre la línea después de tomar el fondo.',
    'La confirmación exige que cada canal cambie de estado.',
    'Colocar los cinco sensores sobre la superficie de línea y tomar muestra.', 'No declara calibración válida sin transiciones en todos los canales.'),
  30: entry('Inversión de sentido de los motores', 'Tracción', 'mascara de 4 bits',
    'Define la corrección de sentido eléctrico de cada motor para que las consignas con signo coincidan con la convención del robot.',
    'Los pesos 1, 2, 4 y 8 corresponden a FL, FR, RL y RR. Un bit activado invierte el sentido de esa salida.',
    'Valor 9 = 1 + 8 invierte FL y RR.', 'No es una orden de giro ni modifica la geometría Mecanum. La polaridad del conteo Hall tiene su configuración propia.'),
  31: entry('Límite de salida PWM', 'Tracción', 'cuentas PWM 0…255',
    'Es la magnitud máxima permitida a la salida del regulador de las ruedas. Es un límite eléctrico, distinto del límite de velocidad.',
    'Un límite mayor deja utilizar más PWM; uno menor recorta la salida aunque el regulador pida más.',
    '128 es aproximadamente 50,2% de la escala 255.', 'No establece una velocidad física concreta ni demuestra que el motor pueda alcanzar la consigna con ese límite.'),
  32: entry('Rampa de cambio del PWM', 'Tracción', 'cuentas por ciclo nominal de 10 ms',
    'Limita cuanto puede acercarse el PWM aplicado al PWM solicitado en cada iteración nominal del control.',
    'Valores mayores permiten cambios más rápidos. Valores menores suavizan la variación de la salida y hacen más lenta su respuesta.',
    'Paso 4 permite variar hasta 4 cuentas por iteración; no son 4 mm/s.', 'Es una rampa eléctrica discreta, no una aceleración mecánica medida. Las condiciones de parada siguen siendo del controlador.'),
  33: entry('Kp del control de ruedas', 'PID de ruedas', 'Q8',
    'Es la contribución proporcional al error actual entre velocidad solicitada y velocidad medida de cada rueda.',
    'A mayor Kp, mayor corrección para el mismo error. En el firmware el resultado combinado se divide por 256 antes de limitar el PWM.',
    '128/256=0,5 como coeficiente proporcional efectivo.', 'La escala es Q8; no ingresar 128 pensando que equivale a 128,0. No hay un ajuste universal para todos los motores.'),
  34: entry('Ki del control de ruedas', 'PID de ruedas', 'Q8 por iteración',
    'Multiplica la suma acumulada del error de velocidad. Permite que errores persistentes aporten una corrección aunque sean pequeños.',
    'Aumentarla hace crecer más rápido la contribución acumulada; reducirla disminuye ese efecto. El acumulador tiene límites en el firmware.',
    '8/256=0,03125 multiplica el acumulado discreto.', 'La acumulación ocurre en el lazo nominal de 10 ms. No es directamente una Ki continua en unidades por segundo.'),
  35: entry('Kd del control de ruedas', 'PID de ruedas', 'Q8 por iteración',
    'Multiplica la diferencia entre el error actual y el de la iteración anterior: representa una respuesta al cambio del error.',
    'Una Kd mayor da más peso a cambios rápidos, incluida la variación de la medición. Cero elimina esta contribución.',
    '64/256=0,25 multiplica la diferencia discreta de errores.', 'No se divide por un tiempo en segundos en esta fórmula. Debe interpretarse junto con Kp, Ki y la medición Hall.'),
  36: entry('Configuración de motores habilitada', 'Tracción', '0 o 1',
    'Declara si la configuración del control de ruedas está preparada para utilizarse. Es una condición de habilitación del producto.',
    '0 mantiene esa condición deshabilitada; 1 la declara lista. No arranca motores ni sustituye los requisitos de Hall y geometría.',
    'Valor 1 por sí solo no inicia Conducción manual.', 'Es una declaración de configuración, no una comprobación automática de cableado, sentido o ajuste.'),
  37: entry('Diametro efectivo de la rueda', 'Geometría y odometría', 'mm',
    'Convierte las vueltas de rueda en distancia recorrida. Debe representar el diámetro efectivo del conjunto que rueda sobre el suelo.',
    'Para igual cantidad de vueltas, un diámetro mayor produce una distancia calculada mayor. También afecta la velocidad estimada.',
    'Para 80 mm, una vuelta corresponde nominalmente a π×80≈251,3 mm.', 'Cambiarlo invalida la declaración de geometría y la pose vigente. No es el radio de la rueda.'),
  38: entry('Suma geométrica Lx + Ly', 'Geometría Mecanum', 'mm',
    'Es la suma de las distancias longitudinal y lateral del centro del chasis a los centros de rueda. Interviene al combinar traslación y giro.',
    'Para una misma velocidad angular, una suma mayor exige más velocidad tangencial de ruedas. También interviene al calcular el giro por odometría.',
    'Lx = 120 mm y Ly = 100 mm producen Lx + Ly = 220 mm.', 'No es el radio de la rueda. Cambiarla invalida la geometría declarada y la pose; se debe determinar con las medidas del robot.'),
  39: entry('Disponibilidad declarada de Hall', 'Geometría y odometría', '0 o 1',
    'Indica si el sistema de medición Hall de ruedas está conectado según la configuración del producto.',
    '0 declara que no está disponible. 1 permite considerar ese requisito, pero no demuestra que lleguen pulsos correctos.',
    'Marcar 1 no genera cuentas Hall ni calibra una vuelta.', 'El cambio invalida la pose y la preparación de odometría. La medición/calibración por rueda sigue siendo una operación separada.'),
  40: entry('Geometría declarada valida', 'Geometría y odometría', '0 o 1',
    'Es la confirmación de que las medidas geometricas configuradas están listas para usarse.',
    '0 mantiene el requisito pendiente. 1 lo declara cumplido si la configuración es admitida por el firmware.',
    'No se puede declarar geometría útil con Lx+Ly igual a 0.', 'No mide el chasis ni calibra sensores. Cambiar la declaración invalida la pose y requiere reconstruir la odometría.'),
  41: entry('Transiciones del encoder de la OLED', 'Interfaz local', '2 o 4 por paso',
    'Define cuantas transiciones eléctricas del encoder de navegación se agrupan para producir un paso de selección en la OLED.',
    'Cambia la relación entre el giro de la perilla y el avance por el menú. El firmware admite exactamente 2 o 4, nunca 3.',
    'Con 4 se requieren cuatro transiciones para un paso del menú.', 'Este parámetro corresponde a la perilla de usuario; no es el número de pulsos Hall por vuelta de una rueda.'),
  42: entry('Sentido del encoder de la OLED', 'Interfaz local', '0 o 1',
    'Invierte el sentido con que el giro de la perilla desplaza la selección de la interfaz local.',
    '0 conserva el sentido configurado como normal; 1 invierte la dirección de navegación.',
    'Si girar a la derecha retrocede en el menú, esta opción cambia esa correspondencia.', 'No invierte ruedas, servos ni el conteo de odometría. Tampoco modifica la acción corta/larga del pulsador SW.'),
  44: entry('Modo Wi-Fi solicitado', 'Comunicación', '1, 2 o 3',
    'Prepara el modo Wi-Fi solicitado: 1=STA, 2=AP, 3=AP+STA. STA usa una red existente; AP crea una red para conectar clientes.',
    'La elección cambia el modo solicitado del borrador de red. No equivale a una conexión confirmada ni aplica credenciales por sí sola.',
    '3 permite solicitar el funcionamiento simultaneo de AP y STA.', 'La aplicación y confirmación de la conexión pertenecen al flujo de Wi-Fi del ESP; escribir este número no prueba que la red haya cambiado.'),
  53: entry('Contraste de la pantalla OLED', 'Pantalla', 'nivel 0…255',
    'Ajusta el byte de contraste enviado al controlador SSD1306 de la OLED.',
    'Valores mayores solicitan más contraste/intensidad y valores menores lo reducen. No es un porcentaje de la pantalla Web.',
    '128 es un valor intermedio de la escala de 8 bits.', 'No cambia la resolución ni el contenido de las pantallas. La apariencia final depende del módulo y su alimentación.'),
  54: entry('Orientación de la pantalla OLED', 'Pantalla', '0 o 1',
    'Seleccioná la orientación de presentación de la OLED para adaptarla al montaje físico.',
    '0 conserva la orientación normal; 1 solicita la rotación de 180°. No se ingresa el número 180.',
    'Valor 1 equivale a elegir la orientación invertida de la pantalla.', 'No rota el robot del simulador ni modifica el sistema de coordenadas de movimiento.'),
};

export function explainEditorValue(id: number, value: number) {
  if (!Number.isFinite(value)) return 'Ingresa un número valido para interpretar este valor.';
  if (id >= 33 && id <= 35) return `${value} / 256 = ${(value / 256).toFixed(5)} como coeficiente Q8 efectivo.`;
  if (id === 26 || id === 30) {
    const names = id === 26 ? ['IR1', 'IR2', 'IR3', 'IR4', 'IR5'] : ['FL', 'FR', 'RL', 'RR'];
    return names.map((name, bit) => `${name}: ${(value & (1 << bit)) !== 0 ? 'invertido' : 'normal'}`).join(' · ');
  }
  if (id === 12) return ['FL · delantera izquierda', 'FR · delantera derecha', 'RL · trasera izquierda', 'RR · trasera derecha'][value] ?? 'Índice de rueda fuera de rango.';
  if (id === 27) return `Canal IR${value + 1}; el índice transmitido es ${value}.`;
  if (id === 25) return ['Sin tipo definido', 'Lectura digital', 'Lectura analogica ADC'][value] ?? 'Tipo fuera de rango.';
  if (id === 44) return ({ 1: 'STA · red existente', 2: 'AP · punto de acceso', 3: 'AP + STA' } as Record<number, string>)[value] ?? 'Modo fuera de rango.';
  if ([36, 39, 40, 42].includes(id)) return value === 1 ? 'Opción declarada activa (1).' : value === 0 ? 'Opción declarada inactiva (0).' : 'Solo se admiten 0 o 1.';
  if(id===81)return `${value} ms = ${(value/1000).toFixed(1)} s; largo se ejecuta al soltar.`;
  if (id === 54) return value === 1 ? 'Orientación solicitada 180°.' : value === 0 ? 'Orientación normal.' : 'Solo se admiten 0 o 1.';
  if (id === 37) return `Una vuelta equivale nominalmente a ${(Math.PI * value).toFixed(1)} mm para este diámetro.`;
  if (id === 31) return `Límite equivalente a ${(value / 255 * 100).toFixed(1)}% de la escala PWM.`;
  return `${value} ${CALIBRATION_GUIDE[id]?.unit ?? ''} en el editor. Todavía no implica una aplicación al robot.`;
}
