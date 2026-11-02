# AFR2-20261102-04 — Explorar infraestructura

02/11/2026. Pedido humano: explicar visualmente cómo se desarrolla, transforma,
comprime, almacena y sirve la aplicación, con Fusion/Blender/Draco, React,
pesos de archivos, flash, filesystem y vuelta al navegador.

## Comportamiento incorporado

- Nuevo destino **Explorar → Infraestructura**, `#/infraestructura`, disponible
  también en Inicio y Más por la definición compartida de navegación. Enlace
  contextual desde la portada de Modelos y simulación.
- Mapa general con las dos ramas de origen y su unión en paquete gzip,
  LittleFS/flash, HTTP y navegador.
- Dos recorridos de ocho etapas seleccionables (Web y modelo 3D), con lugar de
  ejecución, entrada, salida, explicación, detalles y referencia a fuentes del
  proyecto. Controles Anterior/Siguiente y selección directa.
- Pesos del catálogo GLB de auto, brazo y cubo; conjunto como suma auto+brazo,
  sin duplicar recursos ni cargar una escena 3D para presentar esta información.
- Tabla del manifiesto con tamaños antes/después de gzip, barras comparativas,
  reducción o aumento por archivo, totales, versión y fechas. Distingue paquete
  local preparado del manifiesto leído de LittleFS en el ESP.
- Consulta de ocupación/capacidad LittleFS solo cuando el origen es la placa;
  estimación del paquete y tamaño de imagen binaria explicados por separado.
- Glosario de minimización/gzip/Draco, flash/LittleFS/RAM, paquete/imagen/firmware
  y tareas de cada equipo. El ESP transmite gzip, el navegador lo descomprime.
- Estado de carga, ausencia de reporte, error y reintento. Cancelación y timeout;
  sin sondeo periódico, HEAD de modelos, recursos externos o comandos físicos.

## Procedencia y adaptación por archivo

Web/src:
- `features/infrastructure/InfrastructurePage.tsx`: vista nueva; usa PageHeader,
  UiIcon, GlassSurface, Button, Disclosure y SegmentedControl existentes.
- `features/infrastructure/infrastructureFlow.ts`: contenido basado en
  `vite.config.ts`, `package.json`, `tools/package-web.mjs`,
  `tools/copy-web-to-esp.mjs`, `tools/web-budget.mjs`, fuentes ESP de storage/HTTP,
  cargadores/modelos Web y `CAD/docs/MODELO_FINAL_WEB_20261025.md`.
- `features/infrastructure/useInfrastructurePackage.ts`: lectura de manifiesto
  y health; estados y procedencia sin asumir despliegue.
- `app/App.tsx`, `app/navigation.ts`, `features/models/ModelsPage.tsx`:
  ruta, destino y enlace contextual; se conservan cambios ajenos presentes.
- `styles/infrastructure.css`, `styles/index.css`: disposición responsive con
  tokens existentes. Sin redefinir paleta, tipografía ni movimiento global.

Web/vite.config.ts: middleware dev/preview de lectura de ruta fija para el
paquete local ya preparado, identificado por cabecera. No genera artefactos.
Web/docs/ui-navigation-20261101.json: destino agregado sin alterar el estado
histórico de verificación de otras rutas.

ESP/src/main.cpp: `GET /api/web-manifest`, ruta fija que transmite el manifiesto
público solo con paquete disponible. Sin edición de storage, configuración Wi-Fi,
comandos ni transporte UART. [Contrato HTTP](../contracts/WEB_INFRASTRUCTURE.md).

Contracts: esta ficha, contrato, cobertura visual, CHANGELOG, README e informe
maestro. Solo lectura de documentación CAD; no se editan maestros ni exportaciones.

## Identidad visual y límites

Dirección Confirmada por pedido humano. Primitivas/tokens actuales Observados;
se continúa su aplicación local autorizada, sin cambiar la identidad. Composición
de mapa, explorador y métricas Propuesta. Cobertura de fuente registrada en
`docs/design-system/coverage-20261102-04.json`; ejecución No verificada.

Cinco reglas: tokens de ambos temas; controles compartidos; foco y texto visibles;
datos con procedencia; cero movimiento continuo decorativo. El mapa se apila en
celular, la tabla tiene scroll local y el explorador pasa a dos columnas desde
840 px. No se afirma comprobación de estos comportamientos en navegador.

## Estado real

- **atmega:** sin cambios.
- **esp:** fuente HTTP modificada; sin compilar ni cargar.
- **web:** fuente implementada; sin tests, compilación, validadores ni navegador.
- **CAD / Loader:** sin cambios por esta entrega.
- Sin generación/promoción de paquete, creación de imagen, apertura de puertos
  ni flasheos. Los artefactos existentes conservan su estado previo.
- No se creó commit ni publicación. Cambios concurrentes ajenos preservados.
