import { useEffect, useState } from 'react';

interface PackageFile { route: string; rawBytes: number; gzipBytes: number }
interface WebManifest {
  webVersion: string;
  packagedAt: string;
  modelAssetsUpdatedAt?: string;
  modelAssetsOnlyUpdate?: string;
  buildProfile: string;
  totalRawBytes: number;
  totalGzipBytes: number;
  estimatedFilesystemBytes?: number;
  filesystemBudgetBytes?: number;
  files: PackageFile[];
}
interface FilesystemHealth {
  usedBytes?: number;
  totalBytes?: number;
  mounted?: boolean;
  webReady?: boolean;
  fault?: string;
}
export interface PackageReport {
  source: 'esp-littlefs' | 'local-prepared';
  manifest: WebManifest;
  filesystem: FilesystemHealth | null;
  observedAt: string;
}
const record = (value: unknown): value is Record<string, unknown> => !!value && typeof value === 'object' && !Array.isArray(value);
const bytes = (value: unknown): value is number => typeof value === 'number' && Number.isSafeInteger(value) && value >= 0;

function parseManifest(value: unknown): WebManifest {
  if (!record(value) || value.schema !== 'afr2-littlefs-manifest-1' || value.project !== 'AutoFinalRobo2'
    || typeof value.webVersion !== 'string' || typeof value.packagedAt !== 'string' || typeof value.buildProfile !== 'string'
    || !bytes(value.totalRawBytes) || !bytes(value.totalGzipBytes) || !Array.isArray(value.files) || value.files.length > 256
    || !value.files.every(file => record(file) && typeof file.route === 'string' && bytes(file.rawBytes) && bytes(file.gzipBytes))) {
    throw new Error('El servidor no devolvió un manifiesto Web compatible.');
  }
  return {
    webVersion: value.webVersion, packagedAt: value.packagedAt, buildProfile: value.buildProfile,
    totalRawBytes: value.totalRawBytes, totalGzipBytes: value.totalGzipBytes,
    files: value.files.map(file => ({ route: String(file.route), rawBytes: Number(file.rawBytes), gzipBytes: Number(file.gzipBytes) })),
    ...(typeof value.modelAssetsUpdatedAt === 'string' ? { modelAssetsUpdatedAt: value.modelAssetsUpdatedAt } : {}),
    ...(typeof value.modelAssetsOnlyUpdate === 'string' ? { modelAssetsOnlyUpdate: value.modelAssetsOnlyUpdate } : {}),
    ...(bytes(value.estimatedFilesystemBytes) ? { estimatedFilesystemBytes: value.estimatedFilesystemBytes } : {}),
    ...(bytes(value.filesystemBudgetBytes) ? { filesystemBudgetBytes: value.filesystemBudgetBytes } : {}),
  };
}

/** Consulta HTTP de lectura. El origen viene del servidor; un paquete local nunca se presenta como instalado. */
export function useInfrastructurePackage() {
  const [revision, setRevision] = useState(0);
  const [report, setReport] = useState<PackageReport | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState('');
  useEffect(() => {
    const controller = new AbortController();
    let active = true;
    const timeout = window.setTimeout(() => controller.abort(), 8000);
    setLoading(true); setReport(null); setError('');
    const load = async () => {
      try {
        const response = await fetch('/api/web-manifest', { cache: 'no-store', signal: controller.signal });
        if (!response.ok) throw new Error('El servidor no tiene un reporte de archivos disponible.');
        const source = response.headers.get('X-AFR2-Package-Source');
        if (source !== 'esp-littlefs' && source !== 'local-prepared') throw new Error('El servidor no identifica el origen del paquete.');
        const manifest = parseManifest(await response.json());
        let filesystem: FilesystemHealth | null = null;
        if (source === 'esp-littlefs') {
          try {
            const healthResponse = await fetch('/api/health', { cache: 'no-store', signal: controller.signal });
            if (healthResponse.ok) {
              const health: unknown = await healthResponse.json();
              if (record(health) && health.product === 'AutoFinalRobo2' && record(health.filesystem)) {
                const fs = health.filesystem;
                filesystem = {
                  ...(bytes(fs.usedBytes) ? { usedBytes: fs.usedBytes } : {}),
                  ...(bytes(fs.totalBytes) ? { totalBytes: fs.totalBytes } : {}),
                  ...(typeof fs.mounted === 'boolean' ? { mounted: fs.mounted } : {}),
                  ...(typeof fs.webReady === 'boolean' ? { webReady: fs.webReady } : {}),
                  ...(typeof fs.fault === 'string' ? { fault: fs.fault } : {}),
                };
              }
            }
          } catch { /* El manifiesto sigue siendo útil si no se puede consultar la ocupación. */ }
        }
        if (active) setReport({ source, manifest, filesystem, observedAt: new Date().toISOString() });
      } catch (failure) {
        if (active) setError(controller.signal.aborted ? 'La consulta tardó demasiado. Podés volver a intentarlo.'
          : failure instanceof Error ? failure.message : 'No se pudo leer el paquete.');
      } finally {
        window.clearTimeout(timeout);
        if (active) setLoading(false);
      }
    };
    void load();
    return () => { active = false; window.clearTimeout(timeout); controller.abort(); };
  }, [revision]);
  return { report, loading, error, refresh: () => setRevision(value => value + 1) };
}
