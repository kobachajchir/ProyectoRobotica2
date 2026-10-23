# Carpeta src/input

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

## Utilidad y reformas específicas

### [Encoder de interfaz y pulsador único](../../docs/modules/encoder-ui.md)

encoder.c F4 procesa un mando rotativo y su pulsador. Con la ampliación solicitada vuelve a ser una referencia útil para UI; no mide las cuatro ruedas. user_button queda como antecedente del botón eliminado.

Reformas: CLK D40/PG1 y DT D41/PG0 se leen juntos; SW D42/PL7 activo bajo. Inicializar desde niveles reales, evitar gesto de arranque pulsado, rechazar transición cuadratura inválida y ajustar signo/transiciones por detent. Base: 1 ms poll, 20 ms debounce, 800 ms largo exclusivo; ajustar sin asumir hardware probado. El corto sale al soltar solo si no se emitió largo. No usar polling de UI para Hall motriz.

## Archivos vecinos

- [afr2_encoder_ui.c](<afr2_encoder_ui.c>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../AutoFinalRobo2-Contracts/provenance/files.json).
