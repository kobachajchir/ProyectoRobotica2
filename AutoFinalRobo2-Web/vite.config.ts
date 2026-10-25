import { defineConfig, type Plugin } from 'vite';
import { readFile } from 'node:fs/promises';
import { fileURLToPath } from 'node:url';
import react from '@vitejs/plugin-react';
import tailwindcss from '@tailwindcss/vite';
import { viteSingleFile } from 'vite-plugin-singlefile';

// Explicit runtime assets: original PNGs and unused decoders never enter dist.
function runtimeAssets(embedded: boolean): Plugin {
  const files = ['brand/Logo-1024.webp', 'brand/Logo-256.webp', 'brand/favicon.webp',
    'models/AutoFinalRobo2.glb', 'draco/draco_decoder.wasm', 'draco/draco_wasm_wrapper.js'];
  return { name: 'afr2-runtime-assets', async generateBundle() {
    this.emitFile({ type: 'asset', fileName: '.afr2-build.json', source: JSON.stringify({ profile: embedded ? 'esp' : 'full' }) });
    for (const fileName of files) {
      this.emitFile({ type: 'asset', fileName, source: await readFile(new URL(`./public/${fileName}`, import.meta.url)) });
    }
  } };
}

export default defineConfig(({ mode }) => ({
  // Keep the public directory available to dev/preview; select build assets below.
  publicDir: fileURLToPath(new URL('./public', import.meta.url)),
  plugins: [tailwindcss(), react(), viteSingleFile(), runtimeAssets(mode === 'esp')],
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
