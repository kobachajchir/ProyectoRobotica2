import type { UiIconName } from '../../components/ui/UiIcon';

export type Journey = 'web' | 'model';
export interface FlowStep {
  title: string;
  short: string;
  place: string;
  icon: UiIconName;
  input: string;
  output: string;
  description: string;
  details: string[];
  source: string;
}

const packageStep: FlowStep = {
  title: 'Comprimir y reunir', short: 'gzip + manifiesto', place: 'En la computadora', icon: 'programs',
  input: 'HTML, GLB, WebP y Draco', output: 'Archivos .gz + manifest.json',
  description: 'Cada recurso se comprime por separado con gzip nivel 9. El manifiesto relaciona la dirección que pide el navegador con el archivo guardado, su tipo y sus tamaños.',
  details: ['El GLB puede tener geometría Draco por dentro y una envoltura gzip por fuera: son dos capas distintas.', 'El manifiesto declara tamaños y SHA-256. El servidor comprueba manifiesto y tamaños; no vuelve a calcular todos los hashes.', 'El paquete se prepara en Web/artifacts/littlefs/dist y se copia a ESP/data/dist. Preparar archivos todavía no los carga en la placa.'],
  source: 'Web/tools/package-web.mjs · Web/tools/copy-web-to-esp.mjs',
};
const imageStep: FlowStep = {
  title: 'Crear la imagen de LittleFS', short: 'Imagen .bin', place: 'En la computadora', icon: 'board',
  input: 'Carpeta ESP/data', output: 'littlefs.bin',
  description: 'La herramienta de LittleFS convierte el árbol de archivos en una imagen binaria del sistema de archivos. Conserva carpetas, nombres y contenido para escribirlos en la flash.',
  details: ['La imagen del filesystem y el binario del firmware ESP son artefactos separados.', 'El tamaño de littlefs.bin incluye la estructura y el espacio de la partición; no equivale a la suma de los .gz.', 'La configuración candidata usa flash de 4 MiB y LittleFS nominal de 3 MiB. El presupuesto del empaquetador es 2.800 KiB; no es una medición de la placa.'],
  source: 'ESP/platformio.ini · Web/tools/web-budget.mjs',
};
const flashStep: FlowStep = {
  title: 'Guardar en la flash', short: 'Memoria persistente', place: 'En el ESP8266', icon: 'board',
  input: 'Firmware + imagen LittleFS', output: 'Programa y archivos en flash',
  description: 'La carga escribe cada binario en la región que le corresponde. Al arrancar, el firmware monta LittleFS y localiza el manifiesto de la Web.',
  details: ['La memoria flash conserva los archivos al apagar. La RAM se usa para ejecutar el servidor y atender conexiones.', 'El ESP aloja la Web; el ATmega se ocupa del control del robot. React y los modelos no se ejecutan en el ATmega.', 'El layout debe corresponder a la flash de la placa. Esta explicación no realiza cargas ni acredita que una versión esté instalada.'],
  source: 'ESP/platformio.ini · ESP/src/storage.cpp',
};
const httpStep: FlowStep = {
  title: 'Servir por Wi-Fi', short: 'HTTP + gzip', place: 'Del ESP al navegador', icon: 'wifi',
  input: 'GET / o GET /models/…glb', output: 'Respuesta HTTP comprimida',
  description: 'El navegador pide una ruta. El ESP la busca en el manifiesto y transmite el archivo .gz de LittleFS con su tipo de contenido original y Content-Encoding: gzip.',
  details: ['GET / se resuelve como /index.html. Solo las rutas declaradas en el manifiesto se sirven como recursos Web.', 'El ESP envía los bytes comprimidos: no expande la página ni la geometría 3D en su memoria.', 'El índice usa no-store; otros activos usan ETag y revalidación. La caché puede evitar repetir una descarga.', 'Los comandos y la telemetría circulan por WebSocket /ws; son un flujo distinto de la descarga HTTP de los archivos.'],
  source: 'ESP/src/main.cpp · ESP/src/storage.cpp',
};

export const JOURNEYS: Record<Journey, FlowStep[]> = {
  web: [
    {
      title: 'Escribir la aplicación', short: 'React + TypeScript', place: 'En la computadora', icon: 'programs',
      input: 'Componentes, estilos y lógica', output: 'Fuentes .tsx, .ts y .css',
      description: 'La interfaz se desarrolla en React con TypeScript. Las pantallas, los controles y la comunicación con el robot viven en estos archivos editables.',
      details: ['React describe la interfaz y actualiza lo que ves cuando cambian los datos.', 'En desarrollo, Vite sirve las fuentes desde la computadora. Ese servidor local no es el ESP.', 'Los modelos y el decodificador Draco son recursos locales de la Web; no hace falta un CDN para descargarlos desde la placa.'],
      source: 'Web/src/app/App.tsx · Web/package.json',
    },
    {
      title: 'Transformar y minimizar', short: 'Vite + Terser', place: 'En la computadora', icon: 'settings',
      input: 'React, TypeScript, CSS y dependencias', output: 'dist/index.html + recursos',
      description: 'Vite transforma las fuentes para el navegador. Terser minimiza JavaScript y esbuild minimiza CSS; el complemento singlefile reúne el código principal y los estilos dentro del HTML.',
      details: ['Minimizar reduce nombres, espacios y expresiones. Comprimir con gzip es el paso siguiente.', 'Los modelos GLB, las imágenes, el decodificador y el Worker de cálculo permanecen como recursos independientes cuando corresponda.', 'La configuración usa tres pasadas de compresión de Terser y no publica mapas de fuentes.'],
      source: 'Web/vite.config.ts · Web/package.json',
    },
    packageStep, imageStep, flashStep, httpStep,
    {
      title: 'Abrir la respuesta', short: 'El navegador quita gzip', place: 'En tu celular o computadora', icon: 'screen',
      input: 'Respuesta HTTP con gzip', output: 'HTML, JavaScript y CSS legibles',
      description: 'La capa HTTP del navegador descomprime la respuesta automáticamente. Luego interpreta el HTML y ejecuta el JavaScript de la aplicación.',
      details: ['El navegador recibe código ya transformado; no compila las fuentes TypeScript del repositorio.', 'El tamaño descomprimido del archivo y el tráfico de red son medidas diferentes.', 'Las rutas con # se resuelven en React Router. Cambiar de sección no exige otra página HTML al servidor.'],
      source: 'Web/src/app/App.tsx · ESP/src/main.cpp',
    },
    {
      title: 'Usar la aplicación', short: 'React + Three.js', place: 'En tu celular o computadora', icon: 'control',
      input: 'Aplicación + recursos solicitados', output: 'La interfaz que estás usando',
      description: 'Tu dispositivo dibuja la interfaz y las escenas 3D. El ESP proporciona los archivos y el enlace con el robot; la capacidad gráfica la aporta el navegador.',
      details: ['Los modelos se solicitan al abrir las vistas que los necesitan y se comparten mediante la caché de carga.', 'Three.js y React Three Fiber dibujan la escena con WebGL. La cámara, las luces y la simulación se gestionan en el navegador.', 'El módulo 3D tiene carga bajo demanda en React; la configuración singlefile integra el JavaScript principal en el mismo HTML de producción.'],
      source: 'Web/src/components/three/RobotModel.tsx · Web/vite.config.ts',
    },
  ],
  model: [
    {
      title: 'Modelar en Fusion', short: 'Sólidos y ensambles', place: 'En la computadora', icon: 'cube',
      input: 'Piezas CAD y geometría propia', output: 'Maestro editable + mallas exportadas',
      description: 'En Fusion se construyen y ensamblan las piezas, se colocan los componentes y se conservan las dimensiones y posiciones del diseño.',
      details: ['El maestro CAD conserva el diseño editable. La Web utiliza una copia visual derivada.', 'La exportación convierte las superficies en triángulos y conserva la información de instancias y transformaciones que necesita el ensamble.', 'Los scripts de exportación y sus manifiestos permiten reconstruir la escena. Una representación 3D no acredita tolerancias ni resistencia física.'],
      source: 'CAD/docs/MODELO_FINAL_WEB_20261025.md · CAD/mecanica/auto-completo/fusion_exportar_final.py',
    },
    {
      title: 'Preparar en Blender', short: 'Escena y copia visual', place: 'En la computadora', icon: 'cube',
      input: 'Mallas e instancias de Fusion', output: 'Escena .blend lista para exportar',
      description: 'Blender reconstruye la escena a partir de la exportación. Allí se preparan materiales, nombres y jerarquías, y se aligera una copia para la Web.',
      details: ['Los nombres y grupos de ruedas, motores y articulaciones permiten mover las piezas desde la aplicación.', 'La copia visual simplificada se mantiene separada de los maestros de diseño.', 'Las cámaras, luces y el suelo de los renders de estudio no forman parte del GLB del vehículo. La Web crea su propia escena.'],
      source: 'CAD/docs/MODELO_FINAL_WEB_20261025.md · CAD/mecanica/auto-completo/blender_final_20261025.py',
    },
    {
      title: 'Exportar GLB con Draco', short: 'Geometría comprimida', place: 'En la computadora', icon: 'cube',
      input: 'Escena visual de Blender', output: 'Archivo .glb',
      description: 'GLB es el contenedor binario de la escena glTF. Draco comprime su geometría para reducir el archivo que después descargará el navegador.',
      details: ['La cuantización y simplificación reducen precisión o detalle en la copia visual; el maestro CAD se conserva.', 'Auto, brazo y cubo son archivos independientes. El conjunto se monta en la Web a partir de auto y brazo.', 'Web/public/models contiene los modelos; Web/public/draco aporta el decodificador JavaScript y WebAssembly local.'],
      source: 'Web/src/components/three/modelAssets.ts · CAD/docs/MODELO_FINAL_WEB_20261025.md',
    },
    packageStep, imageStep, flashStep, httpStep,
    {
      title: 'Reconstruir y dibujar', short: 'gzip → Draco → WebGL', place: 'En tu celular o computadora', icon: 'screen',
      input: 'Respuesta HTTP del modelo .glb.gz', output: 'Mallas, materiales y escena interactiva',
      description: 'Primero el navegador retira gzip. Después GLTFLoader abre el GLB y DRACOLoader usa el decodificador WebAssembly para reconstruir las mallas. Three.js las dibuja con WebGL.',
      details: ['La descompresión gzip pertenece a HTTP; la decodificación Draco pertenece al cargador 3D.', 'Las mallas reconstruidas ocupan RAM y memoria gráfica del dispositivo. El peso del GLB no mide ese consumo.', 'El ESP no renderiza el robot ni descomprime Draco: sirve los archivos que necesita el navegador.'],
      source: 'Web/src/components/three/RobotModel.tsx · Web/src/components/three/modelAssetLoading.ts',
    },
  ],
};
