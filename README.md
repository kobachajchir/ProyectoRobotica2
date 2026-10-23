# AutoFinalRobo2-esp

Leer [informe de adaptación](INFORME_ADAPTACION.md) y [maestro](../INFORME_MAESTRO.md).

reference/esp01/src conserva fuentes y fragmentos completos, linker y parche de buffers como referencia. El platformio activo usa framework Arduino/LittleFS y perfil candidato esp12e; confirmar flash/board del ESP12S antes de build. Fuente main.cpp actual solo inicia Serial y yield: no crea AP, servidor ni enlace al Mega todavía. No se importan preferencias/credenciales ni el filesystem anterior. Hay que portar catálogo, bridge y rutas antes de servir control real.

[Contrato compartido](../AutoFinalRobo2-Contracts/README.md). No se han ejecutado compilaciones, tests, validaciones ni flasheos.
