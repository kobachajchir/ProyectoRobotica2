import { Fragment } from 'react';

/** AFR2-20261103-26. Texto de Ayuda: lo que va entre `comillas invertidas` (archivos, rutas, comandos,
 * pines, mensajes) se resalta en celeste como parte importante del código. */
export function HelpRich({ text }: { text: string }) {
  return <>{text.split('`').map((part, index) => index % 2
    ? <code key={index} className="afr2-help-code">{part}</code>
    : <Fragment key={index}>{part}</Fragment>)}</>;
}
