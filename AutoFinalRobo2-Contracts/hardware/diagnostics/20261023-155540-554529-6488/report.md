# Diagnóstico ESP — 2026-10-23T15:55:40.554529-03:00

Resultado: **tools-ok-no-device-opened**

Puerto: `sin seleccionar`.

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
      "port": "COM4",
      "description": "Standard Serial over Bluetooth link (COM4)",
      "hwid": "BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_VID&00010075_PID&A013\\7&2AC49BED&0&DC69E2D024DF_C00000000"
    },
    {
      "port": "COM3",
      "description": "Standard Serial over Bluetooth link (COM3)",
      "hwid": "BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_LOCALMFG&0000\\7&2AC49BED&0&000000000000_00000000"
    },
    {
      "port": "COM6",
      "description": "Standard Serial over Bluetooth link (COM6)",
      "hwid": "BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_LOCALMFG&0000\\7&2AC49BED&0&000000000000_00000001"
    },
    {
      "port": "COM7",
      "description": "Standard Serial over Bluetooth link (COM7)",
      "hwid": "BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_LOCALMFG&0002\\7&2AC49BED&0&B8F653A0DAC0_C00000000"
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
      "board_build.filesystem": "littlefs"
    }
  },
  "web_gzip_bytes": 2925405
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
