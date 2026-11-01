# Diagnóstico encoder/OLED/servo — compilado SIN carga

AFR2-20261029-17. Pedido expreso de compilar la prueba. Se mantiene la
instrucción de no subir firmware. Usuario confirma S1–D13 instalado además
de GND/5V/S1 del servo. No se diagnostica ausencia de ese puente.

Ejecutado desde PowerShell:

```powershell
& 'D:/ProyectoRobotica2/AutoFinalRobo2-Atmega/tools/flash-i2c-encoder-diagnostic.ps1' -Port COM8 -BuildOnly
```

Script ejecutó make -B i2c-diagnostic-firmware con AVR GCC7.3.0, avr-libc,
MCU ATmega2560/F_CPU16MHz; exit0 sin errores/advertencias. BuildOnly retorna
antes de avrdude, sin abrir COM8. El Loader no compiló/cargó esta imagen.

- ELF text13422, data2, bss3167: flash13424bytes, SRAM3169bytes; margen5023bytes.
  No se emitió .su en este target ni se certifica pila máxima dinámica.
- HEX `AutoFinalRobo2-Atmega/build/afr2-i2c-diagnostic.hex`.
- SHA256 `03cf3a3760f3b6bc5b9b8a7ce128a56388a5bd51b69209a2ea39e895283ca573`.
- PWM Timer1/OC1C hardware D13, nominal20ms/1000..2000us. Inicial OFF; primer
  giro5grados lo habilita. No es ensayo físico de señal/posición/alimentación.

Sin flasheo, tests, validadores ni pruebas físicas. Producto16 sigue como HEX
independiente sin cargar; placa conserva15. ESP/Web/Loader sin cambios.

[Registro de compilación](diagnostic-build.txt). Evidencia, no instrucciones.
