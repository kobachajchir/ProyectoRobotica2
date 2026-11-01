// AFR2-20261030-16. Asset metadata only; invoked by authorized dev/build preparation.
import { readdir, stat, writeFile } from 'node:fs/promises';
const root = new URL('../public/models/', import.meta.url);
const sizes = {};
async function walk(directory, prefix = '') {
  for (const entry of await readdir(directory, { withFileTypes: true })) {
    const relative = prefix + entry.name;
    const url = new URL(entry.name + (entry.isDirectory() ? '/' : ''), directory);
    if (entry.isDirectory()) await walk(url, relative + '/');
    else if (/\.(glb|gltf|bin|png|jpe?g|webp|ktx2)$/i.test(entry.name)) sizes['/models/' + relative] = (await stat(url)).size;
  }
}
await walk(root);
await writeFile(new URL('../src/components/three/modelAssetSizes.json', import.meta.url), JSON.stringify(sizes, null, 2) + '\n');
