/**
 * Empaquetador LittleFS local de AutoFinalRobo2-Web (`AFR2-20261023-09`).
 * - Opera exclusivamente dentro de AutoFinalRobo2-Web (staging -> artifacts/littlefs/dist).
 * - Genera versión comprimida .gz y manifest.json con rutas, MIME, tamaños y SHA-256.
 * - No escribe en AutoMicro ni flashea el ESP8266.
 */

import { readdir, readFile, mkdir, writeFile, realpath, rm, rename, lstat, copyFile } from 'node:fs/promises';
import { createHash } from 'node:crypto';
import { gzipSync } from 'node:zlib';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { candidateLinkerScript, filesystemBudgetBytes, estimateFilesystemBytes } from './web-budget.mjs';
import { isAssemblyNarrationAsset, requireSilentAssemblyHtml } from './assembly-audio-policy.mjs';

const MIME_BY_EXT = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'application/javascript; charset=utf-8',
  '.mjs': 'application/javascript; charset=utf-8',
  '.css': 'text/css; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.svg': 'image/svg+xml',
  '.png': 'image/png',
  '.webp': 'image/webp',
  '.ico': 'image/x-icon',
  '.wasm': 'application/wasm',
  '.glb': 'model/gltf-binary',
  '.txt': 'text/plain; charset=utf-8',
};

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const expectedDist = path.join(root, 'dist');
const source = await realpath(expectedDist);

if (source !== expectedDist) {
  throw new Error('El directorio dist no puede ser un enlace simbólico externo.');
}

// AFR2-20261024-03: use the actual Vite profile, not a label supplied after build.
const { profile: buildProfile, webVersion } = JSON.parse(await readFile(path.join(source, '.afr2-build.json'), 'utf8'));
const currentVersion = JSON.parse(await readFile(path.join(root, 'package.json'), 'utf8')).version;
if (webVersion !== currentVersion) throw new Error('dist pertenece a otra version o no identifica su compilacion. Volve a compilar antes de empaquetar.');
requireSilentAssemblyHtml(await readFile(path.join(source, 'index.html'), 'utf8'));
const profileIndex = process.argv.indexOf('--profile');
if (!['esp', 'full'].includes(buildProfile) || (profileIndex >= 0 && process.argv[profileIndex + 1] !== buildProfile)) {
  throw new Error('El perfil solicitado no coincide con dist. Volvé a compilar con build o build:esp.');
}

const artifactsRoot = path.join(root, 'artifacts', 'littlefs');
await mkdir(artifactsRoot, { recursive: true });
if ((await realpath(artifactsRoot)) !== artifactsRoot) {
  throw new Error('El directorio artifacts/littlefs debe pertenecer a AutoFinalRobo2-Web.');
}

const stagingDir = path.join(artifactsRoot, 'dist.staging');
const targetDir = path.join(artifactsRoot, 'dist');

async function guardDirectory(expected) {
  if (path.dirname(expected) !== artifactsRoot || !['dist', 'dist.staging'].includes(path.basename(expected))) {
    throw new Error('Destino de publicación inesperado.');
  }
  try {
    const info = await lstat(expected);
    if (!info.isDirectory() || info.isSymbolicLink() || (await realpath(expected)) !== expected) {
      throw new Error(`Directorio de publicación no seguro: ${expected}`);
    }
  } catch (error) {
    if (error.code !== 'ENOENT') throw error;
  }
}

// AFR2-20261023-15: check the absolute deletion target before each recursive cleanup.
await guardDirectory(stagingDir);
await rm(stagingDir, { recursive: true, force: true, maxRetries: 3, retryDelay: 100 });
await mkdir(stagingDir, { recursive: true });

const files = [];
let totalRawBytes = 0;
let totalGzipBytes = 0;

async function visit(dir, rel = '') {
  const entries = await readdir(dir, { withFileTypes: true });
  entries.sort((a, b) => a.name.localeCompare(b.name));

  for (const item of entries) {
    if (item.isSymbolicLink()) {
      throw new Error(`No se admiten enlaces simbólicos en dist: ${item.name}`);
    }
    const relPath = path.join(rel, item.name);
    const fullPath = path.join(dir, item.name);
    if (isAssemblyNarrationAsset(relPath)) continue;

    if (relPath === '.afr2-build.json') continue; // Build metadata, not an HTTP asset.

    if (item.isDirectory()) {
      await visit(fullPath, relPath);
      continue;
    }
    if (!item.isFile()) continue;

    const rawBuffer = await readFile(fullPath);
    const gzBuffer = gzipSync(rawBuffer, { level: 9 });
    const sha256 = createHash('sha256').update(rawBuffer).digest('hex');

    const normalizedRel = relPath.replaceAll('\\', '/');
    const servedRel = `${normalizedRel}.gz`;
    if (servedRel.split('/').some(component => Buffer.byteLength(component, 'utf8') >= 32)) {
      throw new Error(`Nombre incompatible con LittleFS ESP8266 (máximo 31 bytes por componente): ${servedRel}`);
    }
    const outFile = path.join(stagingDir, `${relPath}.gz`);

    await mkdir(path.dirname(outFile), { recursive: true });
    await writeFile(outFile, gzBuffer);

    const ext = path.extname(item.name).toLowerCase();
    const contentType = MIME_BY_EXT[ext] ?? 'application/octet-stream';

    totalRawBytes += rawBuffer.length;
    totalGzipBytes += gzBuffer.length;

    files.push({
      route: `/${normalizedRel}`,
      servedPath: `/${servedRel}`,
      contentType,
      contentEncoding: 'gzip',
      rawBytes: rawBuffer.length,
      gzipBytes: gzBuffer.length,
      sha256,
    });
  }
}

await visit(source);

// Conservative accounting only: 8 KiB rounding per asset plus 64 KiB reserve.
// This is not a filesystem image measurement or evidence of the physical flash.
const estimatedFilesystemBytes = estimateFilesystemBytes(files);
if (buildProfile === 'esp' && estimatedFilesystemBytes > filesystemBudgetBytes) {
  throw new Error(`Paquete ESP demasiado grande: estimación ${estimatedFilesystemBytes}, presupuesto ${filesystemBudgetBytes} bytes.`);
}
if (files.length > 24) throw new Error('Storage del ESP admite hasta 24 activos Web.');

const manifest = {
  schema: 'afr2-littlefs-manifest-1',
  project: 'AutoFinalRobo2',
  webVersion: JSON.parse(await readFile(path.join(root, 'package.json'), 'utf8')).version,
  packagedAt: new Date().toISOString(),
  spaNavigation: 'hash-router',
  rootIndexServedPath: '/index.html.gz',
  buildProfile,
  candidateLinkerScript,
  estimatedFilesystemBytes,
  filesystemBudgetBytes: buildProfile === 'esp' ? filesystemBudgetBytes : null,
  totalRawBytes,
  totalGzipBytes,
  files,
};

const manifestText = `${JSON.stringify(manifest, null, 2)}\n`;
if (Buffer.byteLength(manifestText) > 12288) throw new Error('El manifiesto supera los 12288 bytes que lee el ESP.');
await writeFile(path.join(stagingDir, 'manifest.json'), manifestText, 'utf8');

await guardDirectory(targetDir);
await rm(targetDir, { recursive: true, force: true, maxRetries: 3, retryDelay: 100 });
try {
  await rename(stagingDir, targetDir);
} catch (error) {
  if (!['EPERM', 'EACCES', 'EBUSY'].includes(error.code)) throw error;
  // Some Windows directories reject rename despite allowing file creation.
  // Publish files to the checked destination and commit the manifest last.
  // This fallback is not an atomic directory swap. A failed copy has no manifest.
  await mkdir(targetDir, { recursive: true });
  await guardDirectory(targetDir);
  for (const file of files) {
    const relative = file.servedPath.slice(1);
    const destination = path.resolve(targetDir, relative);
    if (!destination.startsWith(targetDir + path.sep)) throw new Error('Asset fuera del destino.');
    await mkdir(path.dirname(destination), { recursive: true });
    await copyFile(path.join(stagingDir, relative), destination);
  }
  await copyFile(path.join(stagingDir, 'manifest.json'), path.join(targetDir, 'manifest.json'));
  await guardDirectory(stagingDir);
  await rm(stagingDir, { recursive: true, force: true, maxRetries: 3, retryDelay: 100 });
  console.log(`[AutoFinalRobo2-Web] Publicación por archivos (${error.code}); manifest escrito al final.`);
}

console.log(
  `[AutoFinalRobo2-Web] Paquete ${buildProfile} generado (${files.length} archivos, ${totalGzipBytes} bytes gzip; estimación FS ${estimatedFilesystemBytes} bytes).`
);
