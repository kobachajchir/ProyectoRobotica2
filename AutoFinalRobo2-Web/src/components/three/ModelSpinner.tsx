/** AFR2-20261111-08. Spinner animado para los lugares que esperan un modelo 3D (Suspense, botones, progreso). */
import './modelAssetProgress.css';

export function ModelSpinner({ label = 'Cargando modelo 3D…', size = 'md', className = '' }: { label?: string; size?: 'sm' | 'md'; className?: string }) {
  return <span className={`afr2-model-spinner afr2-model-spinner--${size} ${className}`.trim()} role="status" aria-live="polite">
    <span className="afr2-model-spinner__ring" aria-hidden="true" />
    {label && <span className="afr2-model-spinner__label">{label}</span>}
  </span>;
}
