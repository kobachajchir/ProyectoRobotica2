"""AFR2 service bridge transport. Source-only; no I/O until explicitly launched.

Run the configured esptool entry after opening COM with DTR/RTS inactive and
waiting for the Mega bootloader to finish. Keep the same serial handle: closing
and reopening between the wait and esptool would defeat the wait on auto-reset.
"""
import argparse
import os
import re
import runpy
import sys
import time


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--esptool", required=True)
    parser.add_argument("--bridge-baud", required=True, type=int, choices=(9600, 38400, 57600, 115200))
    parser.add_argument("--startup-seconds", required=True, type=int, choices=range(1, 16))
    parser.add_argument("command", nargs=argparse.REMAINDER)
    options = parser.parse_args()
    command = options.command
    if command[:1] == ["--"]:
        command = command[1:]
    if not command or not os.path.isfile(options.esptool):
        parser.error("Falta el comando o la entrada esptool configurada.")
    for flag, expected in (("--baud", str(options.bridge_baud)), ("--before", "no_reset"), ("--after", "no_reset")):
        try:
            value = command[command.index(flag) + 1].replace("-", "_")
        except (ValueError, IndexError):
            parser.error("Falta opción explícita: " + flag)
        if value != expected:
            parser.error("Opción incompatible con puente fijo: " + flag)
    import serial
    original = serial.serial_for_url

    def open_bridge(url, *args, **kwargs):
        if not re.fullmatch(r"COM[1-9][0-9]{0,4}", url, re.IGNORECASE):
            raise ValueError("El transporte de servicio requiere un COM local explícito.")
        caller_deferred_open = kwargs.get("do_not_open", False)
        kwargs["do_not_open"] = True
        kwargs["baudrate"] = options.bridge_baud
        port = original(url, *args, **kwargs)
        original_open = port.open

        def delayed_open():
            try:
                original_open()
                print("AFR2: COM abierto a %d; esperando %d s al Mega, sin transmitir." %
                      (options.bridge_baud, options.startup_seconds), file=sys.stderr, flush=True)
                time.sleep(options.startup_seconds)
                port.reset_input_buffer()
                print("AFR2: espera terminada; esptool usará este mismo puerto abierto.", file=sys.stderr, flush=True)
            except BaseException:
                port.close()
                raise

        port.dtr = False
        port.rts = False
        port.open = delayed_open
        if not caller_deferred_open:
            port.open()
        return port

    serial.serial_for_url = open_bridge
    entry = os.path.abspath(options.esptool)
    sys.path.insert(0, os.path.dirname(entry))
    sys.argv = [entry] + command
    try:
        runpy.run_path(entry, run_name="__main__")
    finally:
        serial.serial_for_url = original


if __name__ == "__main__":
    main()
