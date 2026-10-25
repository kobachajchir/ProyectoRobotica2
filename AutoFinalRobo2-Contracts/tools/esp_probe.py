#!/usr/bin/env python3
"""AFR2-20261023-12/13. Diagnose ESP8266 without writing/erasing flash.

Uses esptool 4/5 CLI in the current Python environment. The last step uploads
esptool's volatile RAM stub to read eight header bytes; no firmware is flashed.
"""
import argparse
import configparser
import datetime
import importlib
import importlib.util
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "hardware" / "diagnostics"


def resolve_esptool(report):
    # PlatformIO keeps esptool outside site-packages. Scope the search to this
    # process and its children; no installation or persistent PATH changes.
    if importlib.util.find_spec("esptool") is None:
        packaged = Path.home() / ".platformio" / "packages" / "tool-esptoolpy"
        if (packaged / "esptool" / "__init__.py").is_file():
            sys.path.insert(0, str(packaged))
            previous = os.environ.get("PYTHONPATH", "")
            os.environ["PYTHONPATH"] = str(packaged) + (os.pathsep + previous if previous else "")
    try:
        tool = importlib.import_module("esptool")
    except ImportError as exc:
        raise RuntimeError("No se pudo cargar esptool/dependencias en este Python: " + str(exc)) from exc
    report["esptool_version"] = tool.__version__
    report["esptool_path"] = tool.__file__
    major = int(tool.__version__.split(".")[0])
    if major not in (4, 5):
        raise RuntimeError("Se admite esptool 4.x/5.x; version detectada: " + tool.__version__)
    return major


def decode_header(data):
    result = {"status": "unrecognized-or-empty", "offset": 0}
    if len(data) != 8 or data[0] not in (0xE9, 0xEA):
        return result
    sizes = {0: 512*1024, 1: 256*1024, 2: 1<<20, 3: 2<<20,
             4: 4<<20, 5: 2<<20, 6: 4<<20, 8: 8<<20, 9: 16<<20}
    result.update(status="header-only-not-image-verification", magic=hex(data[0]),
                  spi_mode={0: "QIO", 1: "QOUT", 2: "DIO", 3: "DOUT"}.get(data[2], "unknown"),
                  configured_flash_bytes=sizes.get(data[3] >> 4),
                  frequency_mhz={0: 40, 1: 26, 2: 20, 15: 80}.get(data[3] & 15),
                  size_code=data[3] >> 4)
    return result


def local_context():
    context = {"filesystem_layout": "not-confirmed", "source": "host-files-not-device"}
    ini = ROOT.parent / "AutoFinalRobo2-ESP" / "platformio.ini"
    if ini.exists():
        parser = configparser.ConfigParser(interpolation=None)
        parser.read(str(ini), encoding="utf-8-sig")
        context["candidate_environments"] = {
            section: {k: v for k, v in parser.items(section)
                      if k in ("platform", "board", "board_build.filesystem", "board_build.ldscript",
                               "board_build.flash_mode", "board_build.flash_size")}
            for section in parser.sections() if section.startswith("env:")}
    manifest = ROOT.parent / "AutoFinalRobo2-ESP" / "data" / "dist" / "manifest.json"
    if manifest.exists():
        doc = json.loads(manifest.read_text(encoding="utf-8-sig"))
        if doc.get("schema") == "afr2-littlefs-manifest-1" and doc.get("project") == "AutoFinalRobo2":
            context["web_gzip_bytes"] = doc.get("totalGzipBytes")
    return context


def save_report(directory, report):
    (directory / "report.json").write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    lines = ["# Diagnóstico ESP — " + report["timestamp"], "",
             "Resultado: **" + report["status"] + "**", "",
             "Puerto: `" + str(report.get("port") or "sin seleccionar") + "`.", "",
             "## Herramientas y puertos (sin implicar conexion)", "", "```json",
             json.dumps({k: report.get(k) for k in ("python", "python_version", "esptool_version", "esptool_path", "pyserial_version", "ports")},
                        indent=2, ensure_ascii=False), "```", "",
             "## Mediciones", "", "```json",
             json.dumps({k: report.get(k) for k in ("connection", "flash", "image_header", "comparison")},
                        indent=2, ensure_ascii=False), "```", "",
             "## Contexto local (no medido en la placa)", "", "```json",
             json.dumps(report.get("local_context"), indent=2, ensure_ascii=False), "```", "",
             "## Pendientes para continuar", "",
             "- El ID JEDEC/tamaño identificado no determina el reparto firmware/LittleFS/OTA.",
             "- Los bits de modo/frecuencia/tamaño son configuración de la imagen, no una prueba de compatibilidad SPI.",
             "- Elegir layout con flash medida, binario/mapa de firmware y margen FS para bundle, metadata y preferencias.",
             "- Confirmar inicio/fin LittleFS desde linker/mapa o diagnóstico de un firmware conocido; no se busca una tabla ESP32.",
             "- Una sincronización ROM exitosa confirma comunicación serie, no verifica tensiones, alimentación bajo carga ni todo el cableado.",
             "- Sin erase/write/format. Puede haberse cargado un stub temporal en RAM para leer la cabecera.",
             "- Al terminar: KEY suelto y reset manual del ESP; restaurar cableado de operación cuando corresponda."]
    if report.get("error"):
        lines += ["", "## Error / siguiente intento", "", report["error"], "",
                  "Cerrar monitores serie; revisar puerto, masa, niveles y aislamiento del Mega. "
                  "Volver a entrar manualmente al cargador con KEY/RST según hardware/FLASHING.md y repetir."]
    (directory / "report.md").write_text("\n".join(lines) + "\n", encoding="utf-8")


def run_step(directory, report, major, name, arguments=(), stub=False):
    # Fixed read-only allowlist; user input can select a port, never a command.
    if name not in ("chip_id", "flash_id", "read_flash"):
        raise ValueError("Command not permitted")
    spelling = (lambda s: s.replace("_", "-")) if major == 5 else (lambda s: s)
    cmd = [sys.executable, "-m", "esptool", "--chip", "esp8266", "--port", report["port"],
           "--baud", "115200", "--before", spelling("no_reset"), "--after", spelling("no_reset")]
    if not stub:
        cmd += ["--no-stub"]
    cmd += [spelling(name)] + list(arguments)
    print("Consultando " + name + " ...", flush=True)
    try:
        proc = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                              timeout=45, encoding="utf-8", errors="replace")
        text, code = proc.stdout, proc.returncode
    except subprocess.TimeoutExpired as exc:
        text = exc.stdout or b""
        if isinstance(text, bytes):
            text = text.decode("utf-8", errors="replace")
        text += "\nTimeout de 45 segundos.\n"
        code = 124
    (directory / (name + ".txt")).write_text(text, encoding="utf-8")
    report["steps"].append({"command": cmd, "exit_code": code, "log": name + ".txt", "ram_stub": stub})
    if code:
        raise RuntimeError(name + " fallo. Ver " + name + ".txt; no se ejecutaron los pasos siguientes.")
    return text


def main():
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(errors="replace")
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", help="Puerto de la ESP (por ejemplo COM7); no se prueban otros puertos")
    parser.add_argument("--check-only", action="store_true", help="Solo conectar/identificar, sin leer flash")
    parser.add_argument("--list-ports", action="store_true", help="Enumerar puertos sin abrirlos")
    parser.add_argument("--check-tools", action="store_true", help="Validar Python/esptool/pyserial y CLI sin abrir puertos")
    args = parser.parse_args()
    stamp = datetime.datetime.now().astimezone()
    directory = OUTPUT / (stamp.strftime("%Y%m%d-%H%M%S-%f") + "-" + str(os.getpid()))
    directory.mkdir(parents=True, exist_ok=False)
    report = {"schema": "afr2-esp-probe-1", "change_id": "AFR2-20261023-13",
              "timestamp": stamp.isoformat(), "status": "started", "steps": [],
              "partition": {"status": "not-determined", "filesystem_offsets": None},
              "connection": False, "port": args.port, "python": sys.executable,
              "python_version": sys.version.split()[0]}
    print("Informe: " + str(directory))
    result = 1
    try:
        try:
            report["local_context"] = local_context()
        except (OSError, ValueError, configparser.Error) as exc:
            report["local_context"] = {"error": str(exc), "filesystem_layout": "not-confirmed"}
        try:
            from serial.tools import list_ports
        except ImportError:
            raise RuntimeError("Falta pyserial. Usar Python con esptool instalado: python -m pip install esptool. No se instala automaticamente.")
        ports = list(list_ports.comports())
        report["ports"] = [{"port": p.device, "description": p.description, "hwid": p.hwid} for p in ports]
        report["pyserial_version"] = importlib.import_module("serial").VERSION
        for p in ports:
            print(p.device + " - " + p.description)
        if args.check_tools:
            resolve_esptool(report)
            command = [sys.executable, "-m", "esptool", "--help"]
            cli = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                 timeout=15, encoding="utf-8", errors="replace")
            (directory / "esptool_help.txt").write_text(cli.stdout, encoding="utf-8")
            report["steps"].append({"command": command, "exit_code": cli.returncode,
                                    "log": "esptool_help.txt", "opens_device": False})
            if cli.returncode:
                raise RuntimeError("esptool CLI no funciona; ver esptool_help.txt")
            report["status"] = "tools-ok-no-device-opened"
            return 0
        if args.list_ports:
            report["status"] = "ports-listed-no-device-opened"
            return 0
        if not args.port:
            if not ports or not sys.stdin.isatty():
                raise RuntimeError("Indicar --port COMx. No se abrio ningun puerto.")
            args.port = input("Puerto conectado a la ESP (ej. COM7): ").strip()
        report["port"] = args.port
        if args.port.casefold() not in {p.device.casefold() for p in ports}:
            raise RuntimeError("El puerto seleccionado no figura en el inventario. No se abrio ningun puerto.")
        major = resolve_esptool(report)
        print("Cerrar monitor serie. Usar conexion de servicio, Mega aislado y ESP en cargador manual KEY/RST.")
        print("No se conmutan DTR/RTS intencionadamente; abrir el puerto puede afectar lineas del adaptador.")
        if not args.check_only:
            print("La lectura final usa un stub temporal en RAM; no modifica la flash.")
        run_step(directory, report, major, "chip_id")
        report["connection"] = True
        save_report(directory, report)  # Preserve the first successful milestone immediately.
        if args.check_only:
            report["status"] = "connection-ok-flash-not-queried"
            return 0
        output = run_step(directory, report, major, "flash_id")
        match = re.search(r"Detected flash size:\s*(\d+)\s*(KB|MB)\b", output, re.I)
        report["flash"] = {"detected_bytes": int(match[1]) * (1024 if match[2].upper() == "KB" else 1048576) if match else None,
                           "manufacturer": None, "device": None}
        for label, key in (("Manufacturer", "manufacturer"), ("Device", "device")):
            value = re.search(label + r":\s*(?:0x)?([0-9a-f]+)\b", output, re.I)
            if value:
                report["flash"][key] = value[1]
        save_report(directory, report)
        with tempfile.TemporaryDirectory(prefix="header-", dir=str(directory)) as scratch:
            header = Path(scratch) / "header.bin"
            run_step(directory, report, major, "read_flash", ("0x0", "8", str(header)), stub=True)
            report["image_header"] = decode_header(header.read_bytes())
        physical = report["flash"]["detected_bytes"]
        configured = report["image_header"].get("configured_flash_bytes")
        web = report["local_context"].get("web_gzip_bytes")
        report["comparison"] = {"header_size_matches_detected": physical == configured if physical and configured else None,
                                "web_payload_fits_entire_chip": web < physical if isinstance(web, int) and physical else None,
                                "web_fits_littlefs_partition": None}
        report["status"] = "readout-complete-layout-pending" if physical else "partial-flash-size-unparsed"
        result = 0 if physical else 2
    except (Exception, KeyboardInterrupt) as exc:
        report["status"] = "failed" if not isinstance(exc, KeyboardInterrupt) else "interrupted"
        report["error"] = str(exc) or "Interrumpido por el usuario."
        print(report["error"], file=sys.stderr)
    finally:
        save_report(directory, report)
        print("Resultado: " + report["status"] + "\nGuardado: " + str(directory / "report.md"))
    return result


if __name__ == "__main__":
    sys.exit(main())
