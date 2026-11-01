# AFR2-20261101-06 — Seccion propia de Simulador UI

Pedido: presentar Simulador UI como Protocol Studio, con acceso debajo de su
tarjeta en Desarrollador y seccion dedicada con el mismo lenguaje visual.

Se aplica el manual0.1.9 autorizado en este alcance. ProtocolStudioPage y su
tarjeta son la fuente canonica Confirmada por el pedido; tokens y primitivas
existentes son supporting. Decision VIS-049, composicion Propuesta y ejecucion
No verificada. No se redefine identidad ni se altera el simulador AVR local.

## Adaptacion por archivo; autorias anteriores conservadas

- Web/src/features/developer/DeveloperPage.tsx: retirar simulador embebido y
  colocar tarjeta con las mismas clases que Protocol Studio, inmediatamente debajo.
- Web/src/features/developer/UiSimulatorPage.tsx: ruta dedicada con SectionHeader,
  insignia, ayuda FloatingModal y retorno a Desarrollador como Protocol Studio.
- Web/src/features/developer/UiSimulator.tsx: TaskSections para Simulador y
  Arbol/catalogo, sin perder estado local entre pestanas. Al inspeccionar una
  pantalla vuelve al simulador; reloj pausado cuando queda oculta esa pestana.
- Web/src/app/App.tsx: ruta /desarrollador/ui-simulator con lazy/Suspense.
- Web/src/components/navigation/RouteNav.tsx: titulo correcto para la nueva ruta.
- Web/src/styles/ui-simulator.css: tipografia, campos, radios y materiales del
  estudio reutilizados desde tokens/protocol-studio.css, en ambos temas.
- Web/README.md e INFORME_ADAPTACION.md: estado de fuente y referencia de entrega.
- Contracts/README.md y CHANGELOG.md: referencia y registro de entrega.
- Contracts/docs/design-system/index.md y coverage-20261101-06.json: fuente
  canonica, cobertura y limites.

La sesion se conserva entre pestanas y se descarta al abandonar la seccion.
La carga diferida evita montar el encoder/reloj en Desarrollador. Catalogo,
renderer OLED, giro/corto/largo, servicios ficticios y reglas Si/No conservados.

ATmega/ESP/CAD/Loader sin cambios. Solo fuente Web y documentacion; sin tests,
compilaciones, validadores, generadores, navegador, promocion ni hardware.
Revision por lectura de ruta, props y estructura; no se certifica aspecto ni
interaccion en ejecucion. La revision del problema WS es una tarea separada y
no se presenta como corregida por esta reorganizacion.
