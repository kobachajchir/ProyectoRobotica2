# Build ESP con ROBO2

23/10/2026, AFR2-20261023-17. Retoma desde ESP/docs/PROTOCOLO_ROBO2.md y contrato UART_PROFILE_2.md. El código compartido/enlace ROBO2 ya estaba escrito por el trabajo paralelo; se revisó y compiló sin modificarlo.

Comando: Python de PlatformIO `-m platformio run -e esp12s_candidate`, dentro de AutoFinalRobo2-ESP. [Log](esp-build.txt): SUCCESS, salida 0, sin warnings/errors; recompiló broker.cpp, main.cpp y mega_link.cpp, enlazó firmware.elf y generó firmware.bin.

RAM estática: 40.096/81.920 bytes. Programa: 386.189/1.044.464 bytes del perfil candidato. No se midieron heap/pila, latencia ni flash física. No hubo carga, imagen LittleFS ni apertura de puertos. La matriz ROBO2_VALIDACION.md continúa siendo criterio de pruebas pendientes, no evidencia de interoperabilidad.
