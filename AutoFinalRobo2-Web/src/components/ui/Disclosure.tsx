import type { ReactNode } from 'react';
import { Chevron } from './Chevron';

export function Disclosure({
  title,
  meta,
  children,
}: {
  title: string;
  meta?: string;
  children: ReactNode;
}) {
  return (
    <details className="afr2-disclosure">
      <summary>
        <span>
          {title}
          {meta ? <small className="ml-2 text-xs text-[var(--text-muted)] font-normal">({meta})</small> : null}
        </span>
        <Chevron />
      </summary>
      <div className="afr2-disclosure-body">{children}</div>
    </details>
  );
}
