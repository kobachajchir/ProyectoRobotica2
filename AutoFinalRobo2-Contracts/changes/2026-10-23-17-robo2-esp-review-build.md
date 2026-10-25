# AFR2-20261023-17 — Retoma ROBO2 desde ESP y compilación

23/10/2026. El usuario entregó `ESP/docs/PROTOCOLO_ROBO2.md` para continuar el trabajo. Se leyó junto con UART_PROFILE_2.md y ROBO2_VALIDACION.md y se contrastó con afr2_wire.h, mega_link.h/.cpp y broker.cpp actuales. El trabajo paralelo ya había incorporado ROBO2 a la ESP: no se duplicó el parser ni se reescribió el enlace. No se modificó código fuente en esta ficha.

La fuente revisada usa ROBO2 de cinco bytes, dos tokens 3A/A5, versión 1/perfil 2, N<=64 y XOR compartidos. Rutas explícitas por nibble: ESP→Mega 31 y Mega→ESP 13; el receptor exige ambos identificadores. Antes de aceptar respuesta exige transacción activa, plazo vigente, CMD/sesión/transacción coincidentes, resultado/longitud y cuerpo coherentes. El broker recibe fallos RX como MEGA_LINK_DOWN. La Web conserva afr2-ws-1. Son observaciones de fuente, no resultados de pruebas de los casos comunes.

Se recompiló el firmware con Python PlatformIO `-m platformio run -e esp12s_candidate`: **SUCCESS, salida 0, sin warnings ni errores** en el log. RAM estática 40.096/81.920 bytes (48,9 %), programa 386.189/1.044.464 bytes (37,0 % del perfil). ELF/BIN regenerados; esos límites pertenecen al perfil candidato, no a una identificación de flash física ni a medición de heap/pila.

[Log completo](../documentation/builds/2026-10-23-17-robo2-esp/esp-build.txt) · [Resumen](../documentation/builds/2026-10-23-17-robo2-esp/README.md). La compilación anterior de ESP/perfil provisional documentada en 15-esp-web-build-fixes.md no se atribuye a ROBO2; esta ficha registra la compilación posterior del nuevo header/enlace.

Documentación adaptada: guía ESP PROTOCOLO_ROBO2.md, README e INFORME_ADAPTACION ESP, nota de estado en UART_PROFILE_2.md y ROBO2_VALIDACION.md, changelog y continuidad de Contracts. No cambia framing, comandos, offsets, validación ni estados del firmware ATmega. No se recompiló Web ni AVR en esta ficha. No se ejecutaron casos de recepción, tests de comportamiento, puertos, flasheos ni banco. La interoperabilidad física, flash/layout y productores del robot siguen pendientes.
