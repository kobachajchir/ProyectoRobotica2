import React from 'react';

export type ButtonVariant = 'primary' | 'secondary' | 'subtle' | 'emergency' | 'danger';
export type ButtonSize = 'sm' | 'md' | 'lg';

export interface ButtonProps extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  variant?: ButtonVariant;
  size?: ButtonSize;
  loading?: boolean;
  icon?: React.ReactNode;
  leftIcon?: React.ReactNode;
}

export function Button({
  variant = 'secondary',
  size = 'md',
  loading = false,
  icon,
  leftIcon,
  className = '',
  children,
  disabled,
  style,
  ...rest
}: ButtonProps) {
  const isDisabled = disabled || loading;
  const resolvedIcon = icon ?? leftIcon;

  const sizeClass =
    size === 'sm'
      ? 'min-h-[44px] px-4 py-2 text-sm rounded-lg gap-1.5'
      : size === 'lg'
        ? 'min-h-[48px] px-5 py-2.5 text-base rounded-xl gap-2.5'
        : 'min-h-[44px] px-4 py-2 text-sm rounded-xl gap-2';

  const getVariantStyle = (): React.CSSProperties => {
    if (variant === 'primary') {
      return {
        backgroundColor: 'var(--action)',
        color: 'var(--text-on-action)',
        borderColor: 'var(--action-border)',
      };
    }
    if (variant === 'emergency') {
      return {
        backgroundColor: 'var(--surface-solid)',
        color: 'var(--error)',
        borderColor: 'var(--error)',
        borderWidth: '2px',
        minHeight: 'var(--stop-button-min-height)',
        boxShadow: 'none',
      };
    }
    if (variant === 'danger') {
      return {
        backgroundColor: 'var(--surface-solid)',
        color: 'var(--error)',
        borderColor: 'var(--error)',
      };
    }
    if (variant === 'subtle') {
      return {
        backgroundColor: 'transparent',
        color: 'var(--text)',
        borderColor: 'var(--border-subtle)',
      };
    }
    return {
      backgroundColor: 'var(--surface-subtle)',
      color: 'var(--text-strong)',
      borderColor: 'var(--border-default)',
    };
  };

  return (
    <button
      type="button"
      disabled={isDisabled}
      data-variant={variant}
      data-size={size}
      className={`afr2-pressable inline-flex items-center justify-center border font-semibold select-none ${sizeClass} ${
        isDisabled ? 'cursor-not-allowed' : 'cursor-pointer'
      } ${className}`.trim()}
      style={{
        ...getVariantStyle(),
        ...style,
      }}
      {...rest}
    >
      {loading ? (
        <span
          className="inline-block h-4 w-4 shrink-0 animate-spin rounded-full border-2 border-current border-t-transparent"
          style={{ animationDuration: 'var(--duration-spinner)' }}
          aria-hidden="true"
        />
      ) : (
        resolvedIcon
      )}
      {children}
    </button>
  );
}
