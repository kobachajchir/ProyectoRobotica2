# AFR2-20261106-16 · Recursos del Loader compilado localmente

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex.

## Problema

Al abrir directamente el EXE de LOCALAPPDATA/AutoFinalRobo2Loader/bin, sin
argumento de carpeta, se buscaba tools/esp_bridge_esptool.py dentro de bin.
La consulta KEY/RST fallaba en NeedFile, antes de abrir COM.

## Adaptaciones por archivo

- AutoFinalRobo2-Loader/Compilar.ps1 escribe una referencia local de recursos
  por ejecutable: AFR2Loader.appdir o AFR2Loader.Mcp.appdir. Se actualiza aun
  si el EXE no necesita recompilarse. No contiene credenciales.
- AutoFinalRobo2-Loader/src/Program.cs mantiene prioridad del argumento
  explícito. Sin argumento, usa primero recursos adyacentes de distribución;
  para el EXE local usa su referencia .appdir. No depende del directorio de
  trabajo ni incorpora rutas del autor al binario.

El empaquetador tiene lista explícita y no incluye estas referencias locales.
No se modificó el ZIP anterior ni el perfil ni el protocolo KEY/RST.

## Estado real

Loader compilado por pedido del usuario; referencia local leída y wrapper
encontrado en la carpeta de fuente. Apertura del EXE corregido realizada.
El usuario inició luego una consulta KEY/RST desde la ventana en COM3:
job e0a2e08edad84df4ae8320e92d300ce0, registro 20261106-185412-610-14ddb7.txt.
El registro acredita uso del wrapper en D:/ProyectoRobotica2/AutoFinalRobo2-Loader/tools,
inicio de Python y apertura de COM3 a 115200. Se canceló a las 18:54:24 antes
de obtener respuesta del flasher, sin iniciar escritura. No acredita protocolo
KEY/RST, identificación de placa ni funcionamiento automático. El error anterior
correspondía a COM8; no se cambió el puerto ni se reintentó después de cancelar.
ATmega, ESP y Web: sin cambios por esta ficha; no se compiló ni flasheó firmware.

La comprobación inicial mediante Assembly.LoadFile en PowerShell no acredita
la resolución de AppDomain.BaseDirectory: corresponde al proceso anfitrión,
no al EXE cargado. No se la considera prueba del arranque real.
