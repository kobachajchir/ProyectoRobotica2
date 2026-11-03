/* AFR2-20261103-30. Banco de la OLED: la misma pantalla con sus controles para Pantalla en vivo y para la
 * biblioteca y la simulación de Ayuda → OLED. Barra superior con la última entrada, barra de estado,
 * avisos, pantalla con selector de filas y encoder. Cada uso solo aporta el lienzo y las acciones. */
import type { ComponentProps, ReactNode } from 'react';
import { EncoderControl, type EncoderControlProps } from './EncoderControl';
import { MenuSideControls } from './MenuSideControls';

type SideProps = ComponentProps<typeof MenuSideControls>;

export interface OledWorkbenchProps {
  /** Encabezado propio del uso (por ejemplo, título y selector de vista en Ayuda). */
  header?: ReactNode;
  /** Texto de la pill «Última entrada». */
  lastInput: string;
  /** Acción a la derecha de la barra superior (por ejemplo, Actualizar pantalla). */
  headerAction?: ReactNode;
  statusBar?: ReactNode;
  notices?: ReactNode;
  /** Lienzo ya dibujado; el banco le pone el marco, el estado de caducidad y el selector de filas. */
  screen: ReactNode;
  /** Captura caducada: se marca el marco sin cambiar el tamaño. */
  stale?: boolean;
  busy?: boolean;
  /** Marco de espera: mismo tamaño que una captura, con un texto encima. */
  placeholderText?: string;
  /** Cuando es null o no tiene filas, el selector lateral no se dibuja. */
  side: SideProps | null;
  encoder: EncoderControlProps;
  /** Contenido bajo la pantalla (avisos de la captura, ayudas). */
  caption?: ReactNode;
  /** Pantalla en vivo: nada se selecciona ni recibe foco con el teclado. */
  lockFocus?: boolean;
}

export function OledWorkbench({ header, lastInput, headerAction, statusBar, notices, screen, stale, busy, placeholderText, side, encoder, caption, lockFocus = false }: OledWorkbenchProps) {
  const guard = lockFocus ? {
    'data-lock-focus': '' as const,
    onFocusCapture: (event: React.FocusEvent<HTMLElement>) => { if (event.target instanceof HTMLElement && event.target !== event.currentTarget) event.target.blur(); },
    onMouseDown: (event: React.MouseEvent<HTMLElement>) => { if (event.target instanceof Element && !event.target.closest('input, textarea')) event.preventDefault(); },
  } : {};
  return <div className="afr2-oled-bench" {...guard}>
    {header}
    <div className="afr2-oled-bench__topbar">
      <span className="afr2-oled-bench__last afr2-tabular">Última entrada: <strong>{lastInput}</strong></span>
      {headerAction}
    </div>
    {statusBar && <div className="afr2-oled-bench__statusbar">{statusBar}</div>}
    {notices && <div className="afr2-oled-bench__notices empty:hidden">{notices}</div>}
    <div className="afr2-oled-bench__body">
      <div className="afr2-oled-bench__display">
        <div className="afr2-oled-with-side">
          <div className={`afr2-oled-bench__screen${placeholderText ? ' afr2-oled-bench__placeholder' : ''}`} data-stale={stale || undefined} aria-busy={busy}>
            {screen}
            {placeholderText && <p className="afr2-oled-bench__placeholder-text" role="status">{placeholderText}</p>}
          </div>
          {side && <MenuSideControls {...side} />}
        </div>
        {caption}
      </div>
      <div className="afr2-oled-bench__controls">
        <EncoderControl {...encoder} />
      </div>
    </div>
  </div>;
}
