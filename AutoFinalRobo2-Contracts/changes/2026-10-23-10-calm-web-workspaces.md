# AFR2-20261023-10 — Web organizada por tareas y estilo más sobrio

23/10/2026. Pedido humano explícito de reformar la interfaz existente con Apple Design, secciones separadas y menos densidad. No continúa la retención anterior de implementación porque el nuevo pedido autoriza esta reforma concreta.

Antes: acciones, diagnóstico y formularios simultáneos. Después: navegación lateral/escritorio y horizontal/móvil; Inicio de accesos; tareas de Control, Wi-Fi y Sensores separadas; detalles OLED/3D y documentación plegados; conexión/tema a pedido. Mayor espaciado, tipografía contenida, sin halos ni sombras pesadas. Logo y paleta propios conservados. Sonner bajo cabecera para evitar cubrir STOP.

## Navegación y contrato
Sin cambio wire ni nuevos comandos: se conservan handlers/payloads existentes. TaskSections cambia solo la tarea visible y conserva borradores en página. Al cambiar tarea se interrumpe el hold manual activo; al salir de Control se solicita STOP si había hold. STOP permanece accesible en cabecera sticky; no se simula confirmación física. OLED conserva giro/SW único y Volver por ítem. Una corrección de presentación evita mostrar el parámetro de pasos del encoder al elegir calibración de motores/odometría.

## Estado por módulo
- atmega: fuente previa sin cambios.
- esp: sin cambios; sin copia de dist ni flasheo.
- web: refactor de interfaz en fuente, sin nueva compilación ni ejecución de pruebas/validadores/renderizado. Dist previo no representa esta edición hasta un build posterior autorizado.
- shared: VIS-014 y manual/tokens 0.1.4; validaciones previas de 0.1.3 no se atribuyen a esta revisión.

## Revisión y límites
Inspección de los componentes y lecturas de los archivos editados. No se ejecutaron npm install/start/build, tests, escáneres, validadores ni pruebas físicas. Pendientes: evaluación visual real en ambos temas y viewports, uso táctil/teclado y comprobación del corte de hold con backend disponible, solo cuando se solicite.

## Archivos adaptados
Web: components/ui/TaskSections.tsx y Disclosure.tsx nuevos; navigation/RouteNav.tsx; ui/SectionHeader.tsx y Button.tsx; notifications/notify.tsx; features/home/HomePage.tsx, control/ControlPage.tsx, wifi/WifiPage.tsx, sensors/SensorsPage.tsx, oled/OledPage.tsx, docs/DocsPage.tsx, model3d/Model3DPage.tsx; styles/tokens.css e index.css. Adaptación propia sobre la fuente Web activa de AFR2-20261023-09; no se alteran recursos binarios ni atribuciones históricas. Ningún archivo de referencia fue editado.
