/** AFR2-20261028-10. Shared presentation for the AutoMicro-inspired workspaces. */
import { useRef, useState } from 'react';
import type { ReactNode } from 'react';
import { Button } from '../../components/ui/Button';
import { notify } from '../../components/notifications/notify';
import { formatBytes } from './codec';
import type { OutputFormat } from './codec';

export function StudioPanel({ title, description, badge, children, className = '' }: { title: string; description?: string; badge?: ReactNode; children: ReactNode; className?: string }) {
  return <section className={`afr2-protocol-card ${className}`}><div className="afr2-protocol-panel-heading"><div><h2>{title}</h2>{description && <p className="afr2-help">{description}</p>}</div>{badge && <span className="afr2-protocol-badge">{badge}</span>}</div>{children}</section>;
}

export function CopyBlock({ title, value, rows = 5 }: { title: string; value: string; rows?: number }) {
  const output = useRef<HTMLTextAreaElement>(null);
  const [manualCopy, setManualCopy] = useState(false);
  async function copy() {
    try {
      await navigator.clipboard.writeText(value);
      setManualCopy(false);
      notify.success({ id: 'protocol-copy', title: `${title} copiado` });
    } catch {
      setManualCopy(true); output.current?.focus(); output.current?.select();
      notify.info({ id: 'protocol-copy', title: 'Texto seleccionado', description: 'Usa Ctrl+C o la opción Copiar de tu dispositivo.' });
    }
  }
  return <div className="afr2-protocol-output"><div className="afr2-protocol-output-heading"><h3>{title}</h3><Button size="sm" disabled={!value} onClick={() => void copy()} aria-label={`Copiar ${title}`}>Copiar</Button></div><textarea ref={output} readOnly aria-label={title} rows={rows} value={value} spellCheck={false} />{manualCopy && <p className="afr2-help" role="status">Texto seleccionado para copiar manualmente.</p>}</div>;
}

const OUTPUTS: [OutputFormat, string][] = [['hex', 'Hexadecimal'], ['realterm', 'RealTerm'], ['c', 'Array C'], ['python', 'Python bytes']];
export function PacketOutputs({ bytes }: { bytes: number[] }) {
  return <div className="afr2-page-stack"><div className="afr2-protocol-output-grid">{OUTPUTS.map(([format, title]) => <CopyBlock key={format} title={title} value={formatBytes(bytes, format)} rows={4} />)}</div><details className="afr2-protocol-extra"><summary>Salida decimal</summary><CopyBlock title="Bytes decimales" value={formatBytes(bytes, 'decimal')} rows={3} /></details></div>;
}
