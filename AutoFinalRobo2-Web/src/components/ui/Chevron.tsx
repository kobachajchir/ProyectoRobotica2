export function Chevron({ open, className = '' }: { open?: boolean; className?: string }) {
  return <svg className={`afr2-chevron ${className}`} data-open={open} viewBox="0 0 20 20" width="20" height="20" fill="none" aria-hidden="true"><path d="m5 7.5 5 5 5-5" stroke="currentColor" strokeWidth="1.7" strokeLinecap="round" strokeLinejoin="round" /></svg>;
}
