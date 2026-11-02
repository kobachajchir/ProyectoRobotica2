# AFR2-20261102-18 — Vista de simulación compartida

02/11/2026. Pedido explícito de que el simulador de Programas se presente como el simulador del auto, extrayendo un componente común para las vistas que necesitan simulación.

## Antes / después

CollectorScene (Auto/Brazo/Conjunto) y ControlMapScene (secuencias/GOTO) construían Canvas, luces, suelo y presentación por separado. Se extrae `components/three/SimulationViewport.tsx`: mismo Canvas con render bajo demanda, densidad limitada, entorno por tema, cielo, suelo, cuadrícula de 100 mm, iluminación y slot de overlays. `SimulationZoom` concentra botones flotantes de acercar/alejar/reset y porcentaje accesible.

CollectorScene conserva sus adaptadores de articulaciones, objetos, cámara, zoom enlazado al encuadre, selección y gestos, usando el viewport y zoom compartidos. ControlMapScene usa ese mismo viewport, proyección ortográfica para todas sus vistas y el mismo SimulationRobot/SimulationOrbit que el simulador del auto. Conserva capas de destinos/ruta, pose calculada y cámara Superior/Perspectiva/Seguir. La órbita manual se habilita en mapas sin selección de destinos y fuera de Seguir, para conservar la selección de suelo de GOTO.

ControlMap usa SimulationZoom en el visor y mantiene Centrar/Ver ruta, lecturas, procedencia, reintento 3D y fallback TopMap. Programas y GOTO consumen ControlMap; Auto/Brazo/Conjunto consumen CollectorScene. No se montan motores CollectorProvider adicionales en secuencias ni se simula ejecución física con el estado de la escena.

Se adapta la altura/presentación de ControlMap al entorno del simulador del auto mediante simulation-viewport.css. No se reescriben timeline, motores cinemáticos, generación de programas, comandos, permisos ni raster OLED. Radar, inspección de modelos y simulador OLED conservan sus representaciones específicas: no son el mismo tipo de simulación mecánica.

## Archivos

- Web/src/components/three/SimulationViewport.tsx: Canvas, entorno y control de zoom comunes.
- Web/src/styles/simulation-viewport.css: visor/zoom y presentación del mapa.
- Web/src/features/collector/CollectorScene.tsx: consume componentes extraídos.
- Web/src/features/control/ControlMapScene.tsx: consume viewport, robot y órbita comunes; conserva adaptador de ruta.
- Web/src/features/control/ControlMap.tsx: zoom compartido y leyenda de cuadrícula actualizada.

## Estado y límites

atmega/esp sin cambios; web fuente refactorizada; shared/Contracts documentación. Sin cambios de API o protocolo. Los estados de simulación siguen siendo locales, no telemetría física. Cámara, órbita, zoom y carga 3D requieren comprobación posterior expresamente autorizada: no se afirma equivalencia visual o funcional verificada. No se ejecutaron tests, compilación, validadores, navegador, render, promoción ESP ni flasheos. Cambios concurrentes conservados. Sin commit ni publicación solicitados.
