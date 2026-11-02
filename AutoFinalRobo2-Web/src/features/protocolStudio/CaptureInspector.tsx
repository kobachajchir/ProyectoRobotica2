/** AFR2-20261028-10. AutoMicro's stream map and selected-frame analysis,
 * retaining AFR2's raw bytes, encoding choices and capture boundaries. */
import { useMemo, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { UiIcon } from '../../components/ui/UiIcon';
import { Field } from '../../components/ui/Field';
import { commandById } from './catalog';
import { decodeText, formatBytes, hexByte, hexBytes } from './codec';
import type { OutputFormat, StreamSegment, TextFormat, Translation } from './codec';
import { CopyBlock, PacketOutputs, StudioPanel } from './StudioPrimitives';
import { PacketStructure } from './PacketStructure';

const PAGE_SIZE = 24;
const LABELS = { raw: 'Fuera de protocolo', frame: 'Framing correcto', invalid: 'Trama inválida', incomplete: 'Trama incompleta' };
export const OUTPUT_FORMATS: [OutputFormat, string][] = [['hex', 'HEX con espacios'], ['realterm', 'RealTerm · 0xHH'], ['c', 'Array C'], ['python', 'Python bytes'], ['decimal', 'Decimal']];
const offsetLabel = (offset: number) => `0x${offset.toString(16).toUpperCase().padStart(4, '0')}`;

function BlockAnalysis({ segment, bytes }: { segment: StreamSegment; bytes: number[] }) {
  const frame = segment.frame, data = bytes.slice(segment.offset, segment.offset + segment.length);
  return <div className="afr2-page-stack">
    {frame && <PacketStructure bytes={data} />}
    <StudioPanel title={frame ? `Análisis · ${commandById(frame.command)?.name ?? 'Comando desconocido'}` : 'Análisis del bloque'} badge={`${offsetLabel(segment.offset)} · ${segment.length} B`}>
      <p className="afr2-protocol-tag" data-kind={segment.kind}>{LABELS[segment.kind]}</p><p className="afr2-help">{segment.note}</p>
      {frame ? <>
        <dl className="afr2-protocol-fields"><div><dt>CMD</dt><dd>0x{hexByte(frame.command)}</dd></div><div><dt>Ruta</dt><dd>{hexByte(frame.route)} · {frame.route === 0x31 ? 'ESP → ATmega' : frame.route === 0x13 ? 'ATmega → ESP' : 'No permitida'} · {frame.command === 0x52 ? 'Aviso de arranque' : frame.route === 0x13 ? 'Respuesta' : 'Petición'}</dd></div><div><dt>XOR recibido / calculado</dt><dd>{hexByte(frame.checksum)} / {hexByte(frame.expectedChecksum)}</dd></div>{frame.fields.map(([label, value]) => <div key={label}><dt>{label}</dt><dd>{value}</dd></div>)}</dl>
        <details className="afr2-protocol-extra" open={frame.issues.length > 0}><summary>Comprobaciones de payload · {frame.issues.length ? `${frame.issues.length} observaciones` : 'sin observaciones en las reglas disponibles'}</summary>{frame.issues.length ? <ul className="afr2-protocol-issues">{frame.issues.map(issue => <li key={issue}>{issue}</li>)}</ul> : <p className="afr2-help">Se comprobaron las reglas implementadas para este comando. La interpretación de cuerpos es parcial; no confirma sesión negociada ni ejecución.</p>}</details>
        <div className="afr2-protocol-columns"><CopyBlock title="Payload HEX" value={hexBytes(frame.payload)} rows={3} /><CopyBlock title="Cuerpo HEX" value={hexBytes(frame.payload.slice(frame.bodyOffset))} rows={3} /></div>
      </> : <CopyBlock title="Texto escapado del bloque" value={decodeText(data, 'escaped')} rows={6} />}
    </StudioPanel>
    <StudioPanel title={frame ? 'Copiar paquete seleccionado' : 'Copiar bloque seleccionado'}><PacketOutputs bytes={data} /></StudioPanel>
  </div>;
}

export function CaptureInspector({ result, outputFormat, onOutputFormatChange, textFormat, onTextFormatChange }: {
  result: Translation; outputFormat: OutputFormat; onOutputFormatChange: (format: OutputFormat) => void;
  textFormat: TextFormat; onTextFormatChange: (format: TextFormat) => void;
}) {
  const [page, setPage] = useState(0);
  const [selected, setSelected] = useState<number | null>(() => {
    const frames = result.segments.filter(segment => segment.frame);
    return frames.length === 1 ? frames[0].offset : null;
  });
  const counts = result.segments.reduce((count, segment) => { count[segment.kind] += segment.kind === 'raw' ? segment.length : 1; return count; }, { frame: 0, invalid: 0, incomplete: 0, raw: 0 });
  const validUtf8 = useMemo(() => { try { new TextDecoder('utf-8', { fatal: true }).decode(Uint8Array.from(result.bytes)); return true; } catch { return false; } }, [result.bytes]);
  const ascii = useMemo(() => decodeText(result.bytes, 'ascii'), [result.bytes]);
  const resetCause = ascii.match(/rst cause:\s*(\d+)/i)?.[1], bootMode = ascii.match(/boot mode:\s*\((\d+),\s*(\d+)\)/i);
  const command = result.commandOnly === undefined ? undefined : commandById(result.commandOnly);
  const pages = Math.max(1, Math.ceil(result.segments.length / PAGE_SIZE));
  const current = result.segments.find(segment => segment.offset === selected);
  return <div className="afr2-page-stack">
    <StudioPanel title="Estado de la traducción" badge={`${result.bytes.length} bytes`}>
      <div className="afr2-protocol-stats" role="status"><span><strong>{counts.frame}</strong> tramas con framing correcto</span><span><strong>{counts.invalid}</strong> inválidas</span><span><strong>{counts.incomplete}</strong> incompletas</span><span><strong>{counts.raw}</strong> bytes fuera de protocolo</span></div>
      {command && <div className="afr2-protocol-notice"><strong>CMD 0x{hexByte(command.id)} · {command.name}</strong><p>{command.description} Un CMD suelto no constituye una trama.</p></div>}
      {(resetCause || bootMode) && <div className="afr2-protocol-notice"><strong>Texto de arranque ESP detectado</strong><p>{resetCause ? `rst cause: ${resetCause}. ` : ''}{bootMode ? `boot mode: (${bootMode[1]}, ${bootMode[2]}). ` : ''}Valores del texto capturado; los bytes posteriores se conservan para inspección.</p></div>}
      {counts.frame === 0 && <p className="afr2-help">No se encontraron tramas ROBO2 completas con framing correcto. El texto y los binarios siguen disponibles para convertir y copiar.</p>}
    </StudioPanel>
    <StudioPanel title="Mapa visual de la captura" description="Seleccioná un paquete o bloque para ver su análisis y sus formatos de copia.">
      <div className="afr2-protocol-stream-map">{result.segments.slice(page * PAGE_SIZE, (page + 1) * PAGE_SIZE).map(segment => {
        const data = result.bytes.slice(segment.offset, segment.offset + segment.length);
        return <button key={segment.offset} type="button" className="afr2-protocol-stream-card" data-kind={segment.kind} aria-pressed={selected === segment.offset} aria-controls="protocol-block-detail" onClick={() => setSelected(segment.offset)}><span className="afr2-protocol-stream-card-heading"><span className="afr2-protocol-tag">{LABELS[segment.kind]}</span><span className="afr2-protocol-offset">{offsetLabel(segment.offset)}</span></span><strong>{segment.frame ? `${commandById(segment.frame.command)?.name ?? 'CMD desconocido'} · 0x${hexByte(segment.frame.command)}` : `${segment.length} bytes`}</strong><code>{hexBytes(data.slice(0, 24))}{data.length > 24 ? ' …' : ''}</code><span className="afr2-protocol-stream-preview">{decodeText(data.slice(0, 48), 'escaped')}{data.length > 48 ? ' …' : ''}</span><span className="afr2-protocol-stream-action">{selected === segment.offset ? 'Bloque seleccionado' : 'Ver análisis'} <UiIcon name="arrow" size={16} className="inline-block align-[-3px]" /></span></button>;
      })}</div>
      {pages > 1 && <div className="afr2-action-row afr2-protocol-pagination"><Button disabled={page === 0} onClick={() => setPage(page - 1)}>Anterior</Button><span>Página {page + 1} de {pages}</span><Button disabled={page + 1 === pages} onClick={() => setPage(page + 1)}>Siguiente</Button></div>}
      <details className="afr2-protocol-extra"><summary>Stream continuo · todos los bytes</summary><p className="afr2-help">Los rangos están en orden de captura. Selecciona uno para inspeccionarlo; cada cabecera del monitor conserva su límite independiente.</p><div className="afr2-protocol-continuous" tabIndex={0} aria-label="Stream completo por bloques">{result.segments.map(segment => <button key={segment.offset} type="button" data-kind={segment.kind} aria-pressed={selected === segment.offset} aria-controls="protocol-block-detail" onClick={() => setSelected(segment.offset)}><span>{LABELS[segment.kind]} · {offsetLabel(segment.offset)} · {segment.length} B</span><code>{hexBytes(result.bytes.slice(segment.offset, segment.offset + segment.length))}</code></button>)}</div></details>
      <p className="afr2-help">El mapa muestra vistas previas. El stream y las salidas conservan los bytes completos. Framing correcto comprueba estructura, ruta y XOR; no confirma ejecución.</p>
    </StudioPanel>
    <div id="protocol-block-detail" role="region" aria-label="Detalle del bloque seleccionado">{current ? <BlockAnalysis key={current.offset} segment={current} bytes={result.bytes} /> : <div className="afr2-protocol-empty"><h3>Elige un bloque de la captura</h3><p>Su estructura, significado y salidas para copiar aparecerán aquí.</p></div>}</div>
    <div className="afr2-protocol-columns"><StudioPanel title="Conversión de bytes"><Field id="protocol-output-format" label="Formato de salida"><select id="protocol-output-format" value={outputFormat} onChange={event => onOutputFormatChange(event.target.value as OutputFormat)}>{OUTPUT_FORMATS.map(([value, label]) => <option key={value} value={value}>{label}</option>)}</select></Field><CopyBlock title="Captura convertida" value={formatBytes(result.bytes, outputFormat)} rows={10} /></StudioPanel><StudioPanel title="Conversión de texto"><Field id="protocol-text-format" label="Interpretación"><select id="protocol-text-format" value={textFormat} onChange={event => onTextFormatChange(event.target.value as TextFormat)}><option value="escaped">Texto escapado · conserva cada byte</option><option value="utf8">UTF-8</option><option value="ascii">ASCII legible · binarios como ·</option><option value="latin1">Latin-1 · controles escapados</option></select></Field><CopyBlock title="Texto decodificado" value={decodeText(result.bytes, textFormat)} rows={10} />{textFormat === 'utf8' && !validUtf8 && <p className="afr2-protocol-warning">Hay secuencias UTF-8 inválidas: � sustituye bytes que no se pudieron decodificar. HEX y texto escapado conservan los originales.</p>}{textFormat === 'ascii' && <p className="afr2-help">· representa un byte no imprimible. Usa Texto escapado para ver su valor.</p>}</StudioPanel></div>
    {result.metadata.length > 0 && <details className="afr2-protocol-card"><summary>Cabeceras del monitor · {result.metadata.length}</summary><p className="afr2-help">Son metadatos, sin bytes añadidos a la captura ni unión de tramas entre cabeceras.</p><pre className="afr2-protocol-metadata">{result.metadata.join('\n')}</pre></details>}
  </div>;
}
