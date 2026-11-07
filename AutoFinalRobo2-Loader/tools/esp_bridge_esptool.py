"""AFR2 service bridge transport. AFR2-20261106-18: automatic boot shared by all esptool sessions. Source-only; no I/O until explicitly launched.

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


# AFR2-20261106-11. Optional automatic KEY/RST through the ATmega service image
# (D32 -> KEY transistor, D33 -> RST transistor). Opt-in only: without
# --auto-key-rst nothing of this protocol is ever transmitted.
CTL_MAGIC = b"\xa5AFR2CTL"
RSP_MAGIC = b"\xa5AFR2RSP"
CMD_QUERY, CMD_ENTER_BOOT, CMD_RUN = 0x01, 0x10, 0x11
CMD_AUTO_OFF, CMD_AUTO_ON = 0x20, 0x21


def describe(reply):
    status, key, rst, auto = reply[:4]
    return "auto=%s key=%s rst=%s%s" % ("sí" if auto else "no", "apretado" if key else "suelto",
                                         "apretado" if rst else "suelto", " (orden rechazada)" if status else "")


def configure(port, enable):
    """PC -> flasher: set or clear automatic KEY/RST and read the state back."""
    want = CMD_AUTO_ON if enable else CMD_AUTO_OFF
    reply = control(port, want, 1.5)
    if reply is None or reply[0] != 0:
        raise RuntimeError("AFR2: el flasher no confirmó la configuración de KEY/RST automáticos.")
    if bool(reply[3]) != bool(enable):
        raise RuntimeError("AFR2: el flasher informa KEY/RST automáticos = %s, distinto de lo pedido (%s)." %
                           ("sí" if reply[3] else "no", "sí" if enable else "no"))
    print("AFR2-KEYRST: configurado desde la PC; %s" % describe(reply), file=sys.stderr, flush=True)
    return reply


def control(port, cmd, timeout):
    """Send one control frame; return (status, key_held, rst_held, auto_enabled, boot_test_supported) or None."""
    previous = port.timeout
    port.timeout = 0.1  # esptool opens with no timeout; never block waiting for a reply
    try:
        port.reset_input_buffer()
        port.write(CTL_MAGIC + bytes([cmd, cmd ^ 0xFF]))
        port.flush()
        deadline = time.monotonic() + timeout
        buffer = b""
        while time.monotonic() < deadline:
            buffer += port.read(port.in_waiting or 1)
            at = buffer.find(RSP_MAGIC)
            if at >= 0 and len(buffer) >= at + 11:
                reply = buffer[at + 8:at + 11]
                if reply[0] == cmd:
                    return reply[1], bool(reply[2] & 1), bool(reply[2] & 2), bool(reply[2] & 4), bool(reply[2] & 8)
                buffer = buffer[at + 1:]
        return None
    finally:
        port.timeout = previous


def flasher_keyrst(options, parser):
    """Open COM once, wait for the Mega, then query or set the automatic KEY/RST mode."""
    if not options.port or not re.fullmatch(r"COM[1-9][0-9]{0,4}", options.port, re.IGNORECASE):
        parser.error("Se requiere --port con un COM local explícito.")
    import serial
    port = serial.Serial(None, options.bridge_baud, timeout=0.1)
    port.port = options.port
    port.dtr = False
    port.rts = False
    port.open()
    try:
        print("AFR2: COM abierto a %d; esperando %d s al Mega, sin transmitir." %
              (options.bridge_baud, options.startup_seconds), file=sys.stderr, flush=True)
        time.sleep(options.startup_seconds)
        port.reset_input_buffer()
        reply = control(port, CMD_QUERY, 1.5)
        if reply is None:
            raise RuntimeError("AFR2: el ATmega no responde como flasher AFR2 con KEY/RST. Cargá el flasher actual.")
        print("AFR2-KEYRST: flasher AFR2 identificado; %s" % describe(reply), file=sys.stderr, flush=True)
        if options.flasher_keyrst != "query":
            reply = configure(port, options.flasher_keyrst == "on")
            if options.flasher_keyrst == "off":
                print("AFR2-KEYRST: el flasher no tocará KEY ni RST hasta nueva orden.", file=sys.stderr, flush=True)
    finally:
        port.close()
    return 0


# AFR2-20261106-17. No esptool or flash I/O; framed capture from UART1.
CMD_BOOT_TEST = 0x14
BOOT_MAGIC = b"\xa5AFR2BOOT"

def parse_boot_mode(payload):
    match = re.search(rb"boot\s+mode:\s*\(\s*(\d+)\s*,\s*(\d+)\s*\)", payload)
    return (int(match[1]), int(match[2])) if match else None

def capture_programming_boot(port):
    previous_timeout = port.timeout
    port.timeout = 0.1
    try:
        port.reset_input_buffer()
        port.write(CTL_MAGIC + bytes([CMD_BOOT_TEST, CMD_BOOT_TEST ^ 0xFF]))
        port.flush()
        data, deadline, payload = b"", time.monotonic() + 5.0, None
        capture_status = None
        while time.monotonic() < deadline:
            data += port.read(port.in_waiting or 1)
            if len(data) > 4096:
                raise RuntimeError("Respuesta de diagnóstico fuera de límite.")
            at = data.find(RSP_MAGIC)
            if at < 0 or len(data) < at + 11:
                continue
            if data[at + 8] != CMD_BOOT_TEST:
                raise RuntimeError("Respuesta de diagnóstico inesperada.")
            capture_status = data[at + 9]
            if capture_status not in (0, 3):
                raise RuntimeError("El flasher rechazó la prueba (status=%s). Sin boot acreditado." % capture_status)
            if data[at + 10] & 3:
                raise RuntimeError("El flasher no confirmó KEY/RST sueltos al terminar.")
            header = at + 11
            if len(data) < header + len(BOOT_MAGIC) + 2:
                continue
            if data[header:header + len(BOOT_MAGIC)] != BOOT_MAGIC:
                raise RuntimeError("Cabecera de captura boot inválida.")
            size_at = header + len(BOOT_MAGIC)
            size = int.from_bytes(data[size_at:size_at + 2], "little")
            if size > 256:
                raise RuntimeError("Longitud de captura boot inválida.")
            start = size_at + 2
            if len(data) >= start + size:
                payload = data[start:start + size]
                break
        if payload is None:
            raise RuntimeError("No llegó una captura boot completa. No se acredita la secuencia.")
        print("AFR2-BOOTTEST: capture_status=%s" % capture_status, flush=True)
        print("AFR2-BOOTTEST: sequence=confirmed", flush=True)
        print("AFR2-BOOTTEST: bytes=%d" % len(payload), flush=True)
        print("BOOT ASCII: " + payload.decode("ascii", errors="backslashreplace"), flush=True)
        print("BOOT HEX: " + payload.hex(" "), flush=True)
        if capture_status != 0:
            raise RuntimeError("Captura con errores UART/overflow (status=%s); bytes mostrados, sin boot acreditado." % capture_status)
        mode = parse_boot_mode(payload)
        print("AFR2-BOOTTEST: mode=%s" % (mode[0] if mode else "none"), flush=True)
        if mode is None:
            raise RuntimeError("Secuencia confirmada, pero no se recibió boot mode legible.")
        if mode[0] != 1:
            raise RuntimeError("Se recibió boot mode %d; no es arranque de programación UART." % mode[0])
        print("Boot de programación observado. No acredita respuesta ROM ida/vuelta ni carga de firmware.", flush=True)
        return mode
    finally:
        port.timeout = previous_timeout


def boot_test(options, parser):
    if not options.port or not re.fullmatch(r"COM[1-9][0-9]{0,4}", options.port, re.IGNORECASE):
        parser.error("Se requiere --port con un COM local explícito.")
    import serial
    port = serial.Serial(None, options.bridge_baud, timeout=0.1)
    port.port, port.dtr, port.rts = options.port, False, False
    configured = False
    port.open()
    try:
        time.sleep(options.startup_seconds)
        found = control(port, CMD_QUERY, 1.5)
        if found is None or found[0] != 0:
            raise RuntimeError("No responde el flasher AFR2; no se accionó KEY/RST.")
        if not found[4]:
            raise RuntimeError("El flasher instalado no admite captura boot a 74880. Cargá la nueva versión del flasher; no se accionó KEY/RST.")
        configured = True  # cleanup even if enabling is sent but its reply is lost
        configure(port, True)
        capture_programming_boot(port)
    finally:
        try:
            if configured:
                configure(port, False)  # releases pins, without another ESP reset
        finally:
            port.close()


def retry_unsupported_sync():
    """AFR2-20261106-32. Through the Mega bridge a stray byte after the ROM boot log can make the ROM
    answer SYNC with "invalid command" (esptool UnsupportedCommandError). esptool only retries
    FatalError, so a single stray reply aborted the whole --connect-attempts loop (the write step
    failed right after a successful flash_id). Treat it as one failed SYNC attempt instead."""
    try:
        from esptool import loader, util
    except ImportError:
        return  # esptool layout not recognised: leave behaviour untouched
    original_sync = loader.ESPLoader.sync

    def sync(self, *args, **kwargs):
        try:
            return original_sync(self, *args, **kwargs)
        except util.UnsupportedCommandError as error:
            raise util.FatalError("SYNC rechazado por la ROM (comando no soportado); se reintenta: %s" % error)

    loader.ESPLoader.sync = sync


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--test-keyrst-boot", action="store_true")
    parser.add_argument("--esptool")
    parser.add_argument("--auto-key-rst", action="store_true")
    parser.add_argument("--flasher-keyrst", choices=("query", "on", "off"),
                        help="Solo consulta/configura el flasher y termina; no ejecuta esptool.")
    parser.add_argument("--port")
    parser.add_argument("--bridge-baud", required=True, type=int, choices=(9600, 38400, 57600, 115200))
    parser.add_argument("--startup-seconds", required=True, type=int, choices=range(1, 16))
    parser.add_argument("command", nargs=argparse.REMAINDER)
    options = parser.parse_args()
    if options.test_keyrst_boot:
        return boot_test(options, parser)
    if options.flasher_keyrst:
        return flasher_keyrst(options, parser)
    if not options.esptool:
        parser.error("Falta --esptool.")
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

        original_close = port.close
        state = {"entered": False, "configured": False}

        def delayed_open():
            try:
                original_open()
                print("AFR2: COM abierto a %d; esperando %d s al Mega, sin transmitir." %
                      (options.bridge_baud, options.startup_seconds), file=sys.stderr, flush=True)
                time.sleep(options.startup_seconds)
                port.reset_input_buffer()
                if options.auto_key_rst:
                    # Identify the AFR2 service image before any control byte matters.
                    found = control(port, CMD_QUERY, 1.5)
                    if found is None:
                        raise RuntimeError("AFR2: el ATmega no responde como flasher AFR2 con KEY/RST automáticos. "
                                           "No se envió ninguna orden de arranque. Cargá el flasher actual o usá el modo manual.")
                    print("AFR2-KEYRST: flasher AFR2 identificado; %s" % describe(found), file=sys.stderr, flush=True)
                    if found[0] != 0:
                        raise RuntimeError("AFR2: el flasher rechazó la identificación; no se acciona KEY/RST.")
                    state["configured"] = True
                    configure(port, True)
                    state["entered"] = True  # cleanup even if entry reply is lost
                    if found[4]:
                        print("AFR2-AUTO: iniciando KEY/RST y captura boot en esta misma sesión COM.", file=sys.stderr, flush=True)
                        capture_programming_boot(port)
                        print("AFR2-AUTO: boot_mode=1", file=sys.stderr, flush=True)
                        print("AFR2: boot UART confirmado; KEY/RST sueltos. La ROM conserva modo programación hasta otro reset.", file=sys.stderr, flush=True)
                    else:
                        started = control(port, CMD_ENTER_BOOT, 3.0)
                        if started is None or started[0] != 0 or not started[1] or started[2] or not started[3]:
                            raise RuntimeError("AFR2: el flasher no confirmó la secuencia KEY/RST automática.")
                        print("AFR2-AUTO: entry_confirmed; flasher anterior sin captura boot. KEY mantenido; falta respuesta ROM.", file=sys.stderr, flush=True)
                print("AFR2: espera terminada; esptool usará este mismo puerto abierto.", file=sys.stderr, flush=True)
            except BaseException:
                close_bridge()
                raise

        def close_bridge():
            # AFR2-20261106-18. One owner/COM; cleanup is confirmed, never inferred.
            if port.is_open and options.auto_key_rst and state["configured"]:
                configured, entered = state["configured"], state["entered"]
                state["configured"] = state["entered"] = False
                try:
                    if entered:
                        reply = control(port, CMD_RUN, 2.0)
                        if reply is None or reply[0] != 0 or reply[1] or reply[2]:
                            print("AFR2: reinicio normal o pines sueltos SIN confirmar; no se afirma arranque desde flash.", file=sys.stderr, flush=True)
                        else:
                            print("AFR2: KEY/RST sueltos y pulso RST normal confirmados por el flasher; firmware de aplicación no comprobado.", file=sys.stderr, flush=True)
                    if configured:
                        configure(port, False)
                except Exception as error:
                    print("AFR2: limpieza KEY/RST sin confirmar (%s). Revisá el estado del flasher." % error, file=sys.stderr, flush=True)
                finally:
                    original_close()
            else:
                original_close()

        opened.append(close_bridge)
        port.dtr = False
        port.rts = False
        port.open = delayed_open
        port.close = close_bridge
        if not caller_deferred_open:
            port.open()
        return port

    opened = []
    serial.serial_for_url = open_bridge
    entry = os.path.abspath(options.esptool)
    sys.path.insert(0, os.path.dirname(entry))
    retry_unsupported_sync()
    sys.argv = [entry] + command
    try:
        runpy.run_path(entry, run_name="__main__")
    finally:
        serial.serial_for_url = original
        for closer in opened:  # esptool may exit without closing: still release KEY
            try:
                closer()
            except Exception:
                pass


if __name__ == "__main__":
    main()
