import React from 'react';

export interface FieldProps {
  id: string;
  label: string;
  unit?: string;
  hint?: string;
  error?: string;
  children: React.ReactNode;
  className?: string;
}

export function Field({ id, label, unit, hint, error, children, className = '' }: FieldProps) {
  const hintId = hint ? `${id}-hint` : undefined;
  const errorId = error ? `${id}-error` : undefined;

  return (
    <div className={`flex flex-col gap-1.5 ${className}`.trim()}>
      <div className="flex items-center justify-between gap-2">
        <label htmlFor={id} className="text-xs font-semibold" style={{ color: 'var(--text-strong)' }}>
          {label}
        </label>
        {unit && (
          <span
            className="afr2-tabular rounded px-1.5 py-0.5 text-[11px] font-medium"
            style={{
              backgroundColor: 'var(--surface-subtle)',
              color: 'var(--text-muted)',
              border: '1px solid var(--border-subtle)',
            }}
          >
            {unit}
          </span>
        )}
      </div>
      {children}
      {hint && !error && (
        <p id={hintId} className="text-xs" style={{ color: 'var(--text-muted)' }}>
          {hint}
        </p>
      )}
      {error && (
        <p id={errorId} role="alert" className="text-xs font-medium" style={{ color: 'var(--error)' }}>
          {error}
        </p>
      )}
    </div>
  );
}
