// Empaqueta solo dentro de este repo: no escribe en AutoMicro ni en ESP.
import {readdir,readFile,mkdir,writeFile,realpath} from 'node:fs/promises';
import {gzipSync} from 'node:zlib';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const source=await realpath(path.join(root,'dist'));
const target=path.join(root,'artifacts','littlefs','dist');
if(source!==path.join(root,'dist'))throw new Error('dist no puede ser un enlace externo');
await mkdir(target,{recursive:true});
if(await realpath(target)!==target)throw new Error('Destino debe pertenecer a este repo');
const files=[];
async function visit(dir,rel=''){
 for(const item of await readdir(dir,{withFileTypes:true})){
  if(item.isSymbolicLink())throw new Error('No se admiten enlaces en dist');
  const name=path.join(rel,item.name), full=path.join(dir,item.name);
  if(item.isDirectory()){await visit(full,name);continue;}
  if(!item.isFile())continue;
  const out=path.join(target,name+'.gz');
  await mkdir(path.dirname(out),{recursive:true});
  if(await realpath(path.dirname(out))!==path.dirname(out))throw new Error('Directorio externo');
  // wx evita sobrescribir un paquete previo: revisar/promover antes de generar otro.
  const data=gzipSync(await readFile(full));await writeFile(out,data,{flag:'wx'});
  files.push({path:name.replaceAll('\\','/'),gzip_bytes:data.length});
 }
}
await visit(source);
await writeFile(path.join(target,'manifest.json'),JSON.stringify({project:'AutoFinalRobo2',files},null,2),{flag:'wx'});
console.log('Paquete local generado. No copiado a ESP ni flasheado.');
