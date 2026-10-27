# Diagnóstico ESP — 2026-10-27T13:28:30.927426-03:00

Resultado: **failed**

Puerto: `COM16`.

## Herramientas y puertos (sin implicar conexion)

```json
{
  "python": "C:\\Users\\kobac\\.platformio\\penv\\Scripts\\python.exe",
  "python_version": "3.11.7",
  "esptool_version": "4.5.1",
  "esptool_path": "C:\\Users\\kobac\\.platformio\\packages\\tool-esptoolpy\\esptool\\__init__.py",
  "pyserial_version": "3.5",
  "ports": [
    {
      "port": "COM7",
      "description": "Standard Serial over Bluetooth link (COM7)",
      "hwid": "BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_LOCALMFG&0000\\9&34B2468B&0&000000000000_00000002"
    },
    {
      "port": "COM14",
      "description": "Standard Serial over Bluetooth link (COM14)",
      "hwid": "BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_LOCALMFG&0000\\9&34B2468B&0&000000000000_00000003"
    },
    {
      "port": "COM13",
      "description": "Standard Serial over Bluetooth link (COM13)",
      "hwid": "BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_VID&00010075_PID&A013\\9&34B2468B&0&DC69E2D024DF_C00000000"
    },
    {
      "port": "COM1",
      "description": "Communications Port (COM1)",
      "hwid": "ACPI\\PNP0501\\0"
    },
    {
      "port": "COM5",
      "description": "Standard Serial over Bluetooth link (COM5)",
      "hwid": "BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_LOCALMFG&005D\\9&34B2468B&0&B8F653A0DAC0_C00000000"
    },
    {
      "port": "COM3",
      "description": "Arduino Leonardo (COM3)",
      "hwid": "USB VID:PID=2341:8036 SER=HIDEF"
    },
    {
      "port": "COM16",
      "description": "Arduino Mega 2560 (COM16)",
      "hwid": "USB VID:PID=2341:0042 SER=054433A49373513098D8 LOCATION=1-10:x.0"
    }
  ]
}
```

## Mediciones

```json
{
  "connection": false,
  "flash": null,
  "image_header": null,
  "comparison": null
}
```

## Contexto local (no medido en la placa)

```json
{
  "filesystem_layout": "not-confirmed",
  "source": "host-files-not-device",
  "candidate_environments": {
    "env:esp12s_candidate": {
      "platform": "espressif8266 @ 4.2.1",
      "board": "esp12e",
      "board_build.filesystem": "littlefs",
      "board_build.ldscript": "eagle.flash.4m3m.ld"
    }
  },
  "web_gzip_bytes": 1571104
}
```

## Pendientes para continuar

- El ID JEDEC/tamaño identificado no determina el reparto firmware/LittleFS/OTA.
- Los bits de modo/frecuencia/tamaño son configuración de la imagen, no una prueba de compatibilidad SPI.
- Elegir layout con flash medida, binario/mapa de firmware y margen FS para bundle, metadata y preferencias.
- Confirmar inicio/fin LittleFS desde linker/mapa o diagnóstico de un firmware conocido; no se busca una tabla ESP32.
- Una sincronización ROM exitosa confirma comunicación serie, no verifica tensiones, alimentación bajo carga ni todo el cableado.
- Sin erase/write/format. Puede haberse cargado un stub temporal en RAM para leer la cabecera.
- Al terminar: KEY suelto y reset manual del ESP; restaurar cableado de operación cuando corresponda.

## Error / siguiente intento

chip_id fallo. Ver chip_id.txt; no se ejecutaron los pasos siguientes.

Cerrar monitores serie; revisar puerto, masa, niveles y aislamiento del Mega. Volver a entrar manualmente al cargador con KEY/RST según hardware/FLASHING.md y repetir.
