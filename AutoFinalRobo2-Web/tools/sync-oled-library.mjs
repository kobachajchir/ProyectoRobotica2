/** AFR2-20261024-09. Build input generator, not a firmware runner.
 * Copies AVR catalogue/font/icon bytes; original kobac/F4 attribution retained.
 * Layout algorithms are ported in firmwareRenderer.ts and reviewed by source hash.
 */
import { readFile, mkdir, writeFile } from 'node:fs/promises';
import { createHash } from 'node:crypto';
import { fileURLToPath } from 'node:url';

const root = new URL('../', import.meta.url);
const avr = new URL('../AutoFinalRobo2-Atmega/', root);
const contract = new URL('../AutoFinalRobo2-Contracts/contracts/', root);
const sources = {};
const copyOnly=process.argv.includes('--copy-only');
async function source(base, path) {
  // Hash the reviewed text independently of Git's Windows CRLF checkout.
  const text = (await readFile(new URL(path, base), 'utf8')).replace(/\r\n/g, '\n');
  sources[`${base === avr ? 'atmega' : 'contracts'}:${path}`] = createHash('sha256').update(text).digest('hex');
  return text.replace(/\/\*[\s\S]*?\*\//g, '').replace(/\/\/[^\r\n]*/g, '');
}
const symbols = {};
function number(text) {
  let token = text.trim();
  if (Object.hasOwn(symbols, token)) return symbols[token];
  token = token.replace(/[uUlL]+$/g, '');
  if (/^(?:0x[\da-f]+|-?\d+)$/i.test(token)) return Number(token);
  if (token === 'sizeof(items)/sizeof(items[0])-1') return items.length - 1;
  throw new Error(`Unsupported AVR catalogue expression: ${token}`);
}
function enums(text) {
  for (const block of text.matchAll(/enum\s*\{([\s\S]*?)\}/g)) {
    let next = 0;
    for (const entry of block[1].split(',').map(x => x.trim()).filter(Boolean)) {
      const [name, value] = entry.split('=').map(x => x.trim());
      if (value) next = number(value);
      symbols[name] = next++;
    }
  }
}
function body(text, name) {
  const match = text.match(new RegExp(`\\b${name}\\s*\\[[^=]*=\\s*\\{([\\s\\S]*?)\\};`));
  if (!match) throw new Error(`AVR resource missing: ${name}`);
  return match[1];
}
function rows(text) {
  return [...text.matchAll(/\{([^{}]+)\}/g)].map(match =>
    match[1].split(',').map(x => x.trim()).filter(Boolean).map(x => x.startsWith('"') ? JSON.parse(x) : number(x)));
}
function bytes(text) { return [...text.matchAll(/0x[\da-f]+/gi)].map(x => Number(x[0])); }

enums(await source(avr, 'include/afr2_menu_catalog.h'));
enums(await source(avr, 'include/afr2_icons.h'));
enums(await source(contract, 'afr2_network_wire.h'));
enums(await source(contract, 'afr2_sequence_labels.h'));
const catalogue = await source(avr, 'src/ui/afr2_menu_catalog.c');
const items = rows(body(catalogue, 'items'));
const extra = rows(body(catalogue, 'extra'));
const reform=rows(body(catalogue,'reform'));
const wifiReform=rows(body(catalogue,'wifiReform'));
const nodes = rows(body(catalogue, 'nodes'));
const screenDocument = JSON.parse(await readFile(new URL('../AutoFinalRobo2-Contracts/documentation/screens.json', root), 'utf8'));
const screenCodes = screenDocument.screens.map(screen => Number(screen.local_code));
if (!copyOnly && new Set(screenCodes).size !== screenCodes.length) throw new Error('Duplicate ScreenCode in Contracts.');
for (const node of nodes) if (!copyOnly && !screenCodes.includes(node[5])) throw new Error(`AVR node ${node[5]} needs its shared Web screen entry.`);
for (let kind = 1; kind <= symbols.AFR2_NET_EVENT_MAX; kind++) {
  if (!copyOnly && !screenCodes.includes(0x040000 + kind)) throw new Error(`AVR notification ${kind} needs its shared Web screen entry.`);
}
const font = bytes(body(await source(avr, 'src/display/font7x10.c'), 'Font7x10'));
const iconSource = await source(avr, 'src/ui/afr2_icons.c');
const iconTable = body(iconSource, 'icons');
const icons = [...iconTable.matchAll(/\{\s*(\w+),\s*(\d+),\s*(\d+)\s*\}/g)].map(x => ({
  width: Number(x[2]), height: Number(x[3]), bytes: bytes(body(iconSource, x[1])),
}));
const sequence = await source(avr, 'src/ui/afr2_sequence_screen.c');
const sequenceNames = [...body(sequence, 'names').matchAll(/"([^"]*)"/g)].map(x => x[1]);
const sequenceIcons = [...body(sequence, 'icons').matchAll(/\{([^}]+)\}/g)].map(x => x[1].split(',').map(number));
const sensorSource=await source(avr,'src/ui/afr2_sensor_screens.c');
const sensorSine=body(sensorSource,'sine').split(',').map(x=>number(x.trim())).filter(x=>Number.isFinite(x));
const touchSource = await source(avr, 'include/afr2_touch_bitmap.h');
const touchCar = bytes(body(touchSource, 'afr2_touch_car'));
const touchWidth = number(touchSource.match(/#define AFR2_TOUCH_CAR_WIDTH\s+(\d+)/)?.[1] ?? 'missing-touch-width');
const touchHeight = number(touchSource.match(/#define AFR2_TOUCH_CAR_HEIGHT\s+(\d+)/)?.[1] ?? 'missing-touch-height');
const touchSine = body(touchSource, 'afr2_touch_sin').split(',').map(number);
for (const path of ['include/afr2_screens.h', 'src/display/afr2_oled.c', 'src/ui/afr2_screens.c', 'src/ui/afr2_network_screen.c','src/ui/afr2_ui_components.c','src/ui/afr2_qr_screen.c','src/third_party/qrcodegen.c','include/qrcodegen.h']) await source(avr, path);

const reviewed = JSON.parse(await readFile(new URL('src/features/oled/firmwareRenderer.sources.json', root), 'utf8'));
for (const [path, hash] of Object.entries(reviewed)) {
  if (!copyOnly && sources[path] !== hash) throw new Error(`AVR renderer changed: ${path}. Port/review firmwareRenderer.ts before building.`);
}
const destination = new URL('src/features/oled/generated/firmwareLibrary.ts', root);
await mkdir(new URL('.', destination), { recursive: true });
const output = `/** Generated by tools/sync-oled-library.mjs from current AVR sources.\n * Original F4 font/XBM resources: kobac; AFR2 AVR adaptations preserved.\n * Do not edit resource bytes here. No hardware or tests are run. */\n`
  + `export const FIRMWARE_LIBRARY = ${JSON.stringify({ symbols, nodes, items, extra, reform, wifiReform, font, icons, sequenceNames, sequenceIcons, sensorSine, sources })} as const;\n`;
await writeFile(destination, output);
await writeFile(new URL('src/features/oled/touchBitmap.ts', root), '/** AFR2-20261030-15. User-supplied silhouette converted to XBM; same AVR bytes. */\nexport const TOUCH_CAR_WIDTH = ' + touchWidth + ';\nexport const TOUCH_CAR_HEIGHT = ' + touchHeight + ';\nexport const TOUCH_CAR = ' + JSON.stringify(touchCar) + ';\nexport const TOUCH_SIN = ' + JSON.stringify(touchSine) + ';\n');
console.log(`OLED library: ${nodes.length} AVR nodes, ${icons.length} icons, ${font.length} font bytes -> ${fileURLToPath(destination)}`);
