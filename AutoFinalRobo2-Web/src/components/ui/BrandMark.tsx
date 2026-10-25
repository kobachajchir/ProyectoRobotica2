import { useState } from 'react';

export const AFR2_BRAND_LOGO = '/brand/Logo-1024.webp';
const SMALL_LOGO = '/brand/Logo-256.webp';
export interface BrandMarkProps { size?: 'header' | 'hero'; className?: string }

/** Own mark, with transparent canvas and no badge, frame, glow or card. VIS-016. */
export function BrandMark({ size = 'header', className = '' }: BrandMarkProps) {
  const [failed, setFailed] = useState(false);
  const px = size === 'hero' ? 80 : 44;
  return <span className={`afr2-brand-mark afr2-brand-mark--${size} ${className}`} style={{ width: px, height: px }}>
    {failed ? <span className="afr2-brand-mark__fallback">AFR2</span> : <img src={SMALL_LOGO} alt="AutoFinalRobo2" width={px} height={px}
      decoding="async" loading="eager" className="afr2-brand-mark__image" onError={() => setFailed(true)} />}
  </span>;
}
