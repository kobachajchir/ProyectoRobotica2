> **AFR2-20261029-20 compilado SIN cargar:** producto/comunicación/biblioteca Loader exit0 sin errores/advertencias, final29/10/2026 13:00:16. Flash80066bytes; SRAM7085bytes, margen1107bytes, main276 y Ui_Render133bytes según .su individuales; no certifica máximo dinámico de pila/ISR. [Evidencia](../documentation/builds/2026-10-29-20/README.md). Sin tests/validadores ni ensayo físico, Web fuente vinculada sin build/promoción; placa conserva15 y el pedido de no flashear sigue vigente. Estado pendiente de compilación inferior histórico.

# AFR2-20261029-20 — Asistente Hall y confirmación genérica de cambios

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

Pedido humano: selección de rueda al iniciar conteo, guía de una vuelta manual,
corto finaliza, largo cancela con retorno/aviso, resultados por las cuatro
ruedas, recalibración, guardar/descartar conservados y diálogo obligatorio Si/No
al salir sin guardar. Usuario confirma Hall ya entrega pulsos; no se cambia
su adquisición ni se atribuye a este código esa evidencia física.

[Contrato de flujo y persistencia](../contracts/HALL_CALIBRATION_WIZARD.md).
Skill aplicada: afr2-screen-authoring; coproductor OLED/Web en la misma entrega.

## Archivos y procedencia

- Atmega/include/afr2_menu_catalog.h, src/ui/afr2_menu_catalog.c: tres nodos al
  final (0x030036..38;0x030035 ya reservado por presentación táctil19). Menú
  Hall6opciones, oculta END conservando offsets flash/IDs de acciones existentes.
- Atmega/include/afr2_screens.h, src/ui/afr2_screens.c: selector→conteo, corto
  finaliza, largo cancela intento preservando drafts previos; cuatro CPR con
  asterisco. Confirmación reusable por acciones, dos opciones sin atrás y espera
  explícita al commit. Volver contextual y decoder largo/corto existentes.
- Atmega/include/afr2_app.h, src/services/afr2_app.c:16bytes de drafts CPR,
  máscaras/signos/guardado, captura solo al terminar medición válida, aplicación
  por rueda y journal asíncrono. Estado no depende de la última rueda elegida.
  Save fallido no borra drafts; commit exitoso actualiza saved_config existente.
- Atmega/src/services/afr2_app_wire.c: impide click/giro/remoto UI_ACTION en
  modales que invocan servicios locales; táctil no sustituye selector/diálogo.
  No modifica formato/versión wire ni comandos de movimiento o STOP.
- Web/src/features/oled/firmwareRenderer.ts: mismas coordenadas/textos/números,
  selector, conteo, cuatro CPR y diálogo. Snapshot preparado opcional para
  compatibilidad de fuentes; desconocidos --, sin mediciones ficticias.
- Web/src/features/oled/FirmwareLibraryPreview.tsx: vista local identifica
  diseños nuevos; giro selecciona rueda/SiNo, no confirma mediciones físicas ni
  guarda/descarta datos. Estado remoto real sigue framebuffer autoritativo.
- Web/src/features/oled/firmwareRenderer.sources.json: revisión de hashes
  posteriores al port. Recursos generados/catálogo se sincronizan en siguiente
  build Web autorizado; no se ejecuta build Web/promoción a ESP por esta ficha.
- Contracts/documentation/screens.json, MENU_PLAN/NAVIGATION/SCREEN_SYSTEM y
  README/informes: identidades, excepciones contextuales y estado real.

Código original AFR2, autorías previas preservadas. Se mantuvieron cambios
existentes de presentación táctil19 encontrados al retomar; no se revirtieron
sus APIs ni cambios del Loader de otros trabajos. No se editó reference/ ni
se escribieron registros de D:/AutoProyecto.

## Memoria y estado

Nuevos datos AVR:19bytes por App (CPR16+3máscaras),4bytes por UI de confirmación,
17bytes por snapshot (CPR16+mask). Sin malloc ni framebuffer adicional; nuevos
textos fijos usan PROGMEM salvo avisos que siguen el mecanismo existente.
El incremento de stack y texto de avisos requiere presupuesto real de build;
no se certifica pila/ISR por lectura o tamaño estático. No se afirman timings
ni pruebas de rueda/EEPROM reales.

ATmega/Web implementados en fuente. No ejecutar tests, validadores ni ensayos
físicos en esta entrega. Compilación AVR cubierta por pedido previo de compilar
al terminar; resultado pendiente al redactar esta ficha. No flashear:
«No lo subas aun» sigue vigente. Web sin build/promoción y ESP sin cambios;
placa conserva15. HEX18 o anterior no contiene este asistente.
