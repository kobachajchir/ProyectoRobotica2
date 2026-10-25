/** AFR2-20261024-02. Promote the compiled Web package only; never flash/format.
 * Preserve the previous manifest until all new files have been copied. */
import { readFile, mkdir, copyFile, rm, realpath, rename } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { candidateLinkerScript, filesystemBudgetBytes, estimateFilesystemBytes } from './web-budget.mjs';
const webRoot=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const projectRoot=await realpath(path.join(webRoot,'..'));
const source=await realpath(path.join(webRoot,'artifacts','littlefs','dist'));
const target=path.join(projectRoot,'AutoFinalRobo2-ESP','data','dist');
const inside=(root,file)=>{const rel=path.relative(root,file);return rel!=='' && !rel.startsWith(`..${path.sep}`) && rel!=='..' && !path.isAbsolute(rel);};
if(!inside(projectRoot,source))throw new Error('Web package is outside project root.');
await mkdir(target,{recursive:true});const destination=await realpath(target);
if(!inside(projectRoot,destination))throw new Error('ESP package is outside project root.');
function managed(root,entry) {
  const rel=String(entry.servedPath??'').replace(/^\/+/, '');
  const file=path.resolve(root,rel);
  if(!rel.endsWith('.gz') || rel.includes('\\') || !inside(root,file))throw new Error('Invalid managed asset path.');
  return file;
}
const manifestRaw=await readFile(path.join(source,'manifest.json'),'utf8');
const manifest=JSON.parse(manifestRaw);
if(manifest.schema!=='afr2-littlefs-manifest-1' || !Array.isArray(manifest.files))throw new Error('Invalid Web manifest.');
const estimate=estimateFilesystemBytes(manifest.files);
if(manifest.buildProfile!=='esp' || manifest.candidateLinkerScript!==candidateLinkerScript || manifest.estimatedFilesystemBytes!==estimate || estimate>filesystemBudgetBytes)throw new Error('Compilá primero con npm run build:esp. Solo se promueve el perfil ESP dentro de presupuesto.');
const espConfig=await readFile(path.join(projectRoot,'AutoFinalRobo2-ESP','platformio.ini'),'utf8');
if(!espConfig.includes(`board_build.ldscript = ${candidateLinkerScript}`))throw new Error('El layout candidato ESP y el paquete Web no coinciden.');
let previous=[];
try{const old=JSON.parse(await readFile(path.join(destination,'manifest.json'),'utf8'));if(Array.isArray(old.files))previous=old.files;}
catch(e){if(e.code!=='ENOENT')throw e;}
const current=new Set();
for(const entry of manifest.files) {
  const src=managed(source,entry),dst=managed(destination,entry);
  if(!inside(source,await realpath(src)))throw new Error('Source asset escapes package.');
  await mkdir(path.dirname(dst),{recursive:true});
  if(!inside(destination,await realpath(path.dirname(dst))) && path.dirname(dst)!==destination)throw new Error('Target parent escapes package.');
  await copyFile(src,dst);current.add(dst);
}
const temporary=path.join(destination,'manifest.json.tmp');
await copyFile(path.join(source,'manifest.json'),temporary);
await rename(temporary,path.join(destination,'manifest.json'));
for(const entry of previous){const file=managed(destination,entry);if(!current.has(file))await rm(file,{force:true});}
console.log(`[AutoFinalRobo2-Web] Activos promovidos a AutoFinalRobo2-ESP/data/dist (${manifest.files.length} archivos).`);
