# Documentación propia

La referencia AMDOC conserva claves f4/esp/web/qt/shared e IDs originales. Para código nuevo usar atmega/esp/web/shared y IDs de módulo `<repo>:module/<archivo-ancla>`; hallazgos AFRDOC-NNN persistentes. Markdown/JSON deben describir el mismo módulo sin convertir extracción en verificación.

Ficha: responsabilidad/capa, dependencias, propiedad del handle/buffers, construcción/ciclo de vida, flujo de callbacks/contexto main/IRQ, unidades/precauciones, hallazgos, evidencia de archivo/símbolo/hash y firmas de API. Distinguir núcleo, adaptador y composición. No llamar portable a un renderer que lee globales de hardware.

overlays/connections.json mantiene flujos entre módulos. review/findings.json conserva historial y selectores documentales. Cambiar solo las secciones afectadas, no regenerar el corpus histórico AutoMicro. Si cambia wire, registrar AFR2 en esta central. Fuente/diseño/build/test/hardware son estados separados.
