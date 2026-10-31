/** Public asset routes shared by inspection and simulation. */
export const MODEL_ASSETS = {
  auto: { label: 'Auto', url: '/models/AutoFinalRobo2.glb', revision: '20261025-final' },
  brazo: { label: 'Brazo robotico', url: '/models/AFR2_Brazo_v17_Blanco.glb', revision: '20261031-v17-complete' },
  cubo: { label: 'Cubo auxiliar', url: '/models/AFR2_Cubo_50mm.glb', revision: '20261031-cube50' },
} as const;
export function modelAssetUrl(id: keyof typeof MODEL_ASSETS) {
  const asset = MODEL_ASSETS[id];
  return `${asset.url}?v=${encodeURIComponent(asset.revision)}`;
}
