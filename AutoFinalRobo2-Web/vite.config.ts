import { defineConfig, type Plugin } from 'vite';
import { readFile } from 'node:fs/promises';
import type { IncomingMessage, ServerResponse } from 'node:http';
import { fileURLToPath } from 'node:url';
import react from '@vitejs/plugin-react';
import tailwindcss from '@tailwindcss/vite';
import { viteSingleFile } from 'vite-plugin-singlefile';
import { ROBOT_MODELS } from './src/features/collector/robotModel';

/** AFR2-20261102-04: consulta local del paquete ya preparado; no genera ni promueve artefactos. */
function infrastructurePreview(): Plugin {
  const middleware = async (request: IncomingMessage, response: ServerResponse, next: () => void) => {
    if (request.url?.split('?')[0] !== '/api/web-manifest') { next(); return; }
    response.setHeader('Cache-Control', 'no-store');
    response.setHeader('Content-Type', 'application/json; charset=utf-8');
    response.setHeader('X-Content-Type-Options', 'nosniff');
    if (request.method !== 'GET') { response.statusCode = 405; response.setHeader('Allow', 'GET'); response.end('{"error":"GET required"}'); return; }
    try {
      const manifest = await readFile(new URL('../AutoFinalRobo2-ESP/data/dist/manifest.json', import.meta.url));
      response.setHeader('X-AFR2-Package-Source', 'local-prepared');
      response.end(manifest);
    } catch {
      response.statusCode = 503;
      response.end('{"error":"No hay un paquete local preparado"}');
    }
  };
  return {
    name: 'afr2-infrastructure-preview',
    configureServer(server) { server.middlewares.use(middleware); },
    configurePreviewServer(server) { server.middlewares.use(middleware); },
  };
}

// Explicit runtime assets: original PNGs and unused decoders never enter dist.
function runtimeAssets(embedded: boolean): Plugin {
  const modelAssets = ROBOT_MODELS.filter(m => m.visual.kind === 'gltf').map(m => {
    const url = m.visual.url;
    if (!url || !/^\/models\/[a-zA-Z0-9_./-]+\.glb$/.test(url) || url.includes('..')) throw new Error('Un modelo registrado requiere una ruta GLB local bajo /models/.');
    return url.slice(1);
  });
  // AFR2-20261102-01: banner de Inicio y banners de Modelos (renders CAD en WebP).
  const banners = ['brand/home-hero-1600.webp', 'brand/home-hero-mobile-720.webp',
    ...['auto', 'brazo', 'conjunto'].flatMap(id => [`brand/models/model-${id}-1200.webp`, `brand/models/model-${id}-640.webp`]),
    // Ayuda: renders CAD de Fusion y captura de Inicio en el encabezado de cada módulo.
    ...['atmega2560', 'esp12s-shield', 'web-inicio', 'oled-encoder'].map(id => `help/${id}.webp`)];
  const files = [...new Set([...banners, 'brand/Logo-robot-768.webp', 'brand/Logo-robot-256.webp', 'brand/favicon-robot.webp', 'brand/apple-touch-icon-robot.png',
    'models/AutoFinalRobo2.glb', 'models/AFR2_Cubo_50mm.glb', 'models/AFR2_Sensor_Bateria.glb',
    // The assembly player is visual only; narration sources are archived outside public/.
    'draco/draco_decoder.wasm', 'draco/draco_wasm_wrapper.js', ...modelAssets])];
  return { name: 'afr2-runtime-assets', async generateBundle() {
    const { version: webVersion } = JSON.parse(await readFile(new URL('./package.json', import.meta.url), 'utf8'));
    this.emitFile({ type: 'asset', fileName: '.afr2-build.json', source: JSON.stringify({ profile: embedded ? 'esp' : 'full', webVersion }) });
    for (const fileName of files) {
      this.emitFile({ type: 'asset', fileName, source: await readFile(new URL(`./public/${fileName}`, import.meta.url)) });
    }
  } };
}

export default defineConfig(({ mode }) => ({
  // Keep the public directory available to dev/preview; select build assets below.
  publicDir: fileURLToPath(new URL('./public', import.meta.url)),
  plugins: [tailwindcss(), react(), viteSingleFile(), runtimeAssets(mode === 'esp'), infrastructurePreview()],
  build: {
    copyPublicDir: false,
    target: 'esnext',
    cssCodeSplit: false,
    sourcemap: false,
    minify: 'terser',
    terserOptions: { compress: { passes: 3 }, mangle: true, format: { comments: 'some' }, maxWorkers: 1 },
    cssMinify: 'esbuild',
    reportCompressedSize: false,
    rollupOptions: {
      output: {
        inlineDynamicImports: true,
        entryFileNames: 'bundle.js',
      },
    },
  },
}));
