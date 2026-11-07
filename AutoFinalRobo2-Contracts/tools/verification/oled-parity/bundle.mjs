import {createRequire} from 'node:module';
import {fileURLToPath} from 'node:url';
const require=createRequire(new URL('../../../../AutoFinalRobo2-Web/package.json',import.meta.url));
const {build}=require('esbuild');
await build({entryPoints:[fileURLToPath(new URL('./check.ts',import.meta.url))],bundle:true,platform:'node',format:'esm',outfile:process.argv[2]});
