import React from 'react';

export type GlassVariant =
  | 'chrome'
  | 'panel'
  | 'dense'
  | 'cell'
  | 'popover'
  | 'dialog'
  | 'emergency';

export interface GlassSurfaceProps extends React.HTMLAttributes<HTMLElement> {
  as?: React.ElementType;
  variant?: GlassVariant;
  children: React.ReactNode;
}

const VARIANT_CLASS: Record<GlassVariant, string> = {
  chrome: 'afr2-glass-chrome',
  panel: 'afr2-glass-panel',
  dense: 'afr2-glass-dense',
  cell: 'afr2-surface-cell',
  popover: 'afr2-glass-popover',
  dialog: 'afr2-glass-dialog',
  emergency: 'afr2-surface-emergency',
};

export function GlassSurface({
  as: Component = 'div',
  variant = 'panel',
  className = '',
  children,
  ...rest
}: GlassSurfaceProps) {
  return (
    <Component className={`${VARIANT_CLASS[variant]} ${className}`.trim()} {...rest}>
      {children}
    </Component>
  );
}
