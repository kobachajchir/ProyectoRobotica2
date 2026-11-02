import { UiIcon } from './UiIcon';

export function Chevron({ open, className = '' }: { open?: boolean; className?: string }) {
  // AFR2-20261101-07 / VIS-050: mismo trazo del registro (Lucide chevron-down); la rotacion sigue en CSS.
  return <UiIcon name="chevron-down" className={`afr2-chevron ${className}`} data-open={open} />;
}
