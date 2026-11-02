"""Encode chosen UTF-8 content as an OLED QR screen and packed bitmaps.

Offline encoder: bundled Project Nayuki implementation, MIT license retained.
Generates artifacts only. Does not decode, test a scanner, flash or use hardware.
"""
import argparse
import json
from pathlib import Path
import re
import struct
import sys
import zlib

from draw_component_atlas import Oled
from vendor.qrcodegen import DataTooLongError, QrCode, QrSegment

QUIET_MODULES = 4
ECC = {'L': QrCode.Ecc.LOW, 'M': QrCode.Ecc.MEDIUM,
       'Q': QrCode.Ecc.QUARTILE, 'H': QrCode.Ecc.HIGH}


def qr_pixels(text, box=62, minimum_module=2, ecc='M'):
    """Return the white=1/light pixel square, retaining the entire quiet zone."""
    if not text:
        raise ValueError('El contenido QR no puede estar vacio.')
    # ASCII numeric/alphanumeric optimization preserves the input verbatim.
    # Explicit ECI26 identifies UTF-8 for non-ASCII text, rather than relying
    # on a phone's heuristic for accented/multibyte content.
    segments = QrSegment.make_segments(text) if text.isascii() else [
        QrSegment.make_eci(26), QrSegment.make_bytes(text.encode('utf-8'))]
    try:
        code = QrCode.encode_segments(segments, ECC[ecc], 1, 40, -1, True)
    except DataTooLongError as error:
        raise ValueError('El contenido supera la capacidad QR Model2. Usar un enlace o identificador mas corto.') from error
    modules = code.get_size()
    extent = modules + 2 * QUIET_MODULES
    scale = box // extent
    if scale < minimum_module:
        raise ValueError(
            f'QR version {code.get_version()} ({modules} modulos) no entra en '
            f'{box}x{box} con {minimum_module}px por modulo y margen4. '
            'Acortar el contenido o elegir explicitamente --min-module-px 1 '
            'para una variante densa si cabe; escaneo fisico no comprobado.')
    size = extent * scale
    offset = (box - size) // 2
    pixels = [[True] * box for _ in range(box)]
    # Dark modules are OLED pixels OFF. Quiet zone and light modules are ON.
    for my in range(modules):
        for mx in range(modules):
            if code.get_module(mx, my):
                x = offset + (QUIET_MODULES + mx) * scale
                y = offset + (QUIET_MODULES + my) * scale
                for py in range(y, y + scale):
                    for px in range(x, x + scale):
                        pixels[py][px] = False
    actual_ecc = next(name for name, level in ECC.items() if level == code.get_error_correction_level())
    return pixels, {
        'version': code.get_version(), 'symbol_modules': modules,
        'quiet_modules_per_side': QUIET_MODULES,
        'module_pixels': scale, 'symbol_and_quiet_pixels': size,
        'square_pixels': box, 'outer_padding_before_pixels': offset,
        'outer_padding_after_pixels': box - size - offset,
        'quiet_pixels_per_side': QUIET_MODULES * scale,
        'requested_ecc': ecc, 'actual_ecc': actual_ecc,
        'utf8_bytes': len(text.encode('utf-8')),
        'encoding': 'optimized-ASCII' if text.isascii() else 'UTF-8-ECI26',
        'mask': code.get_mask(), 'phone_scan': 'not-performed',
        'physical_oled': 'not-verified',
    }


def screen_pixels(square, title='QR', lines=()):
    """QR left, labels/controls exclusively in the right-side column."""
    if len(lines) > 2:
        raise ValueError('La columna lateral admite hasta dos lineas de detalle.')
    for label in [title, *lines]:
        if len(label) > 8 or any(ord(char) < 32 or ord(char) > 126 for char in label):
            raise ValueError('Los rotulos OLED admiten hasta8 caracteres ASCII por linea; el contenido QR admite UTF-8.')
    output = Oled()
    size = len(square)
    x, y = 1, (64 - size) // 2
    for py, row in enumerate(square):
        for px, light in enumerate(row):
            output.bits[y + py][x + px] = light
    # No global Header/Footer may overwrite or invert the QR quiet zone.
    output.text(67, 3, title)
    for index, label in enumerate(lines):
        output.text(67, 20 + index * 12, label)
    output.footer(ok=False)  # back hint is entirely in x94..125
    return output.bits, {'x': x, 'y': y, 'width': size, 'height': size,
                         'text_column_x': 67, 'text_column_width': 60}


def pack_rows(pixels):
    """Rows left-to-right, LSB first, 1=OLED lit; unused tail bits are zero."""
    width, height = len(pixels[0]), len(pixels)
    stride = (width + 7) // 8
    data = bytearray(stride * height)
    for y, row in enumerate(pixels):
        for x, light in enumerate(row):
            if light:
                data[y * stride + x // 8] |= 1 << (x % 8)
    return data


def pack_pages(pixels):
    """Native SSD1306/AFR2 pages: x+128*(y//8), bit y%8, 1=OLED lit."""
    data = bytearray(1024)
    for y, row in enumerate(pixels):
        for x, light in enumerate(row):
            if light:
                data[x + 128 * (y // 8)] |= 1 << (y % 8)
    return data


def png_bytes(pixels, scale=1):
    # Standard grayscale PNG; nearest-neighbor integer expansion only.
    width, height = len(pixels[0]) * scale, len(pixels) * scale
    raw = bytearray()
    for row in pixels:
        expanded = bytes(value for light in row for value in [255 if light else 0] * scale)
        for _ in range(scale):
            raw.extend(b'\x00')
            raw.extend(expanded)
    def chunk(kind, data):
        return struct.pack('!I', len(data)) + kind + data + struct.pack('!I', zlib.crc32(kind + data) & 0xffffffff)
    return (b'\x89PNG\r\n\x1a\n' +
            chunk(b'IHDR', struct.pack('!2I5B', width, height, 8, 0, 0, 0, 0)) +
            chunk(b'IDAT', zlib.compress(bytes(raw))) + chunk(b'IEND', b''))


def c_header(name, data, width, height, encoding):
    guard = name.upper() + '_H'
    macro = name.upper() + '_FLASH'
    rows = ['    ' + ', '.join(f'0x{byte:02x}' for byte in data[index:index + 16]) + ','
            for index in range(0, len(data), 16)]
    return '\n'.join([
        '/* Generated OLED QR bitmap. Content is encoded in the pixels.',
        f' * {encoding}; 1=lit/light, 0=off/dark. Phone scan not performed.',
        ' * Do not rescale, invert or crop the QR/quiet zone. */',
        f'#ifndef {guard}', f'#define {guard}', '#include <stdint.h>',
        '#ifdef __AVR__', '#include <avr/pgmspace.h>', f'#define {macro} PROGMEM',
        '#else', f'#define {macro}', '#endif',
        f'#define {name.upper()}_WIDTH {width}u', f'#define {name.upper()}_HEIGHT {height}u',
        f'#define {name.upper()}_BYTES {len(data)}u',
        f'static const uint8_t {name}[{len(data)}] {macro} = {{',
        *rows, '};', f'#undef {macro}', '#endif', ''])


def main():
    parser = argparse.ArgumentParser(description='Contenido elegido -> QR62 y pantalla OLED128x64, sin servicios externos.')
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument('--text', help='Texto exacto a codificar; no se modifica ni acorta.')
    source.add_argument('--input-file', type=Path, help='Archivo UTF-8; conserva saltos de linea y espacios.')
    parser.add_argument('--output-dir', type=Path, required=True)
    parser.add_argument('--title', default='QR')
    parser.add_argument('--line', action='append', default=[])
    parser.add_argument('--ecc', choices=list(ECC), default='M')
    parser.add_argument('--min-module-px', type=int, choices=[1, 2], default=2)
    parser.add_argument('--name', default='oled_qr', help='Identificador C del bitmap.')
    parser.add_argument('--replace', action='store_true', help='Reemplazar los archivos de esta salida si ya existen.')
    args = parser.parse_args()
    try:
        if not re.fullmatch(r'[A-Za-z][A-Za-z0-9_]*', args.name):
            raise ValueError('--name debe ser un identificador C que empiece por letra.')
        text = args.text if args.text is not None else args.input_file.read_bytes().decode('utf-8')
        square, metadata = qr_pixels(text, minimum_module=args.min_module_px, ecc=args.ecc)
        screen, placement = screen_pixels(square, args.title, args.line)
        row_data, page_data = pack_rows(square), pack_pages(screen)
        files = {
            'qr.png': png_bytes(square),
            'screen.png': png_bytes(screen),
            'screen-preview.png': png_bytes(screen, 6),
            'qr-onbits.h': c_header(args.name, row_data, 62, 62, 'Row-major LSB, stride8 bytes'),
            'screen-pages.h': c_header(args.name + '_screen', page_data, 128, 64, 'Vertical pages LSB, x+128*(y/8)'),
            'qr-metadata.json': json.dumps({
                'schema': 'oled128-qr-1', **metadata, 'placement': placement,
                'bitmap_polarity': '1=lit/light; 0=off/dark',
                'row_bitmap': {'width': 62, 'height': 62, 'stride_bytes': 8, 'bytes': len(row_data)},
                'screen_bitmap': {'width': 128, 'height': 64, 'bytes': len(page_data)},
                'input_content_in_metadata': False,
                'state': 'generated-not-decoded-not-physically-verified',
            }, indent=2, ensure_ascii=False) + '\n',
        }
        existing = [name for name in files if (args.output_dir / name).exists()]
        if existing and not args.replace:
            raise ValueError('La salida ya tiene archivos generados; elegir otra carpeta o --replace.')
        args.output_dir.mkdir(parents=True, exist_ok=True)
        for name, content in files.items():
            path = args.output_dir / name
            if isinstance(content, bytes):
                path.write_bytes(content)
            else:
                path.write_text(content, encoding='utf-8')
        print(f"Generado QR v{metadata['version']} / ECC {metadata['actual_ecc']} / "
              f"{metadata['module_pixels']}px por modulo / box62. Escaneo no ejecutado.")
    except (ValueError, OSError, UnicodeError) as error:
        print(f'No se genero QR: {error}', file=sys.stderr)
        return 2
    return 0


if __name__ == '__main__':
    sys.exit(main())
