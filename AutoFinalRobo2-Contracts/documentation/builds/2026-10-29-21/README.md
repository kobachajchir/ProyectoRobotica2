# Loader1.1.3 — compilación y recarga

AFR2-20261029-21. GUI y MCP compilados con Compilar.ps1 bajo Windows PowerShell/.NET Framework csc, salida0 sin errores/advertencias. Se preparó primero en .tmp/verify-flasher-21 y luego en la instalación de usuario. Dos procesos MCP1.1.2 sin GUI activa se cerraron para sustituir el ejecutable. La ventana anterior ya estaba cerrada por el usuario/entorno al recargar.

Nueva ventana ejecutando AFR2 Loader1.1.3; loader_status confirma versión1.1.3, sesión cb5a4fe248ac4dcabd25778dc2e90c50, busy=false y acceptsRequests=true. La recarga reinicia opciones por sesión: autoAcceptRequests=false, skipManualChecks=false. Perfil guardado conserva método mega_reset; el selector de puerto inicial es COM3. Para verificar la placa instalada en esta conversación hay que seleccionar COM8. No se cambió de método ni se abrió el COM automáticamente.

Sin operación verify_mega_flasher, prueba física, test o nuevo flasheo en esta revisión. El botón queda disponible para una comprobación explícita. Fuentes del flasher AVR sin cambios; el servicio instalado sigue siendo el de la carga13:36:03.
