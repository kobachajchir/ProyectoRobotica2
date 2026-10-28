/** AFR2-20261028-09/10. AutoMicro Protocol Studio workflow and structure,
 * adapted to current ROBO2 contracts and the shared AFR2 visual system. */
import { useMemo, useState } from 'react';
import { Link } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import { Field } from '../../components/ui/Field';
import { FloatingModal } from '../../components/ui/FloatingModal';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { TaskSections } from '../../components/ui/TaskSections';
import { UiIcon } from '../../components/ui/UiIcon';
import { COMMANDS, RESULT_NAMES, bodySizeText, commandById } from './catalog';
import { MAX_INPUT_CHARS, buildFrame, formatBytes, translate } from './codec';
import type { BuilderInput, InputFormat, OutputFormat, TextFormat, Translation } from './codec';
import { encodeParameters, initialParameters, requestSchemas } from './guidedBodies';
import { CaptureInspector } from './CaptureInspector';
import { PacketStructure } from './PacketStructure';
import { CopyBlock, PacketOutputs, StudioPanel } from './StudioPrimitives';
import '../../styles/protocol-studio.css';

const BOOT_EXAMPLE = '--- Ejemplo de texto de arranque ESP ---\n0D 0A 20 65 74 73 20 4A 61 6E 20 20 38 20 32 30\n31 33 2C 72 73 74 20 63 61 75 73 65 3A 32 2C 20\n62 6F 6F 74 20 6D 6F 64 65 3A 28 33 2C 36 29 0D\n0A';
const INITIAL_BUILDER: BuilderInput = { command: 0x02, response: false, session: '1', transaction: '1', result: 0, body: '' };
const groupFor = (id: number) => id < 0x10 ? 'Enlace y estado base' : id < 0x20 ? 'Control y configuración' : id < 0x30 ? 'Pantalla y encoder' : id < 0x40 ? 'Trabajos Wi-Fi' : id < 0x50 ? 'Secuencias' : id < 0x60 ? 'Sistema y red' : 'Sensores, servo y radar';
const COMMAND_GROUPS = [...new Set(COMMANDS.map(item => groupFor(item.id)))];

export function ProtocolStudioPage() {
  const [active, setActive] = useState('builder');
  const [helpOpen, setHelpOpen] = useState(false);
  const [input, setInput] = useState('');
  const [format, setFormat] = useState<InputFormat>('hex');
  const [textFormat, setTextFormat] = useState<TextFormat>('escaped');
  const [outputFormat, setOutputFormat] = useState<OutputFormat>('hex');
  const [result, setResult] = useState<Translation | null>(null);
  const [error, setError] = useState('');
  const [revision, setRevision] = useState(0);
  const [builder, setBuilder] = useState<BuilderInput>(INITIAL_BUILDER);
  const [variant, setVariant] = useState('empty');
  const [parameters, setParameters] = useState<Record<string, string>>({});
  const command = commandById(builder.command)!;
  const schemas = requestSchemas(builder.command);
  const form = schemas.find(item => item.id === variant) ?? schemas[0];
  const manualOverride = !!builder.body.trim();
  const built = useMemo(() => {
    try {
      const body = !builder.response && !builder.body.trim() ? form ? encodeParameters(form, parameters) : command.example ?? '' : builder.body;
      return { bytes: buildFrame({ ...builder, body }), body, error: '' };
    } catch (failure) { return { bytes: null, body: '', error: failure instanceof Error ? failure.message : 'No se pudo construir la trama.' }; }
  }, [builder, form, parameters, command]);
  function changeCommand(id: number) {
    const next = requestSchemas(id)[0];
    setBuilder(previous => ({ ...previous, command: id, body: '' }));
    setVariant(next?.id ?? ''); setParameters(initialParameters(next));
  }
  function updateInput(value: string) { setInput(value); setResult(null); setError(''); }
  function runTranslation(value = input, nextFormat = format) {
    try { setResult(translate(value, nextFormat)); setError(''); setRevision(previous => previous + 1); }
    catch (failure) { setResult(null); setError(failure instanceof Error ? failure.message : 'No se pudo interpretar la entrada.'); }
  }
  function inspectBuilder() {
    if (!built.bytes) return;
    const value = formatBytes(built.bytes, 'hex');
    setInput(value); setFormat('hex'); runTranslation(value, 'hex'); setActive('translator');
  }
  return <div className="afr2-route-view afr2-page-stack afr2-protocol-studio">
    <SectionHeader title="Protocol Studio" description="Construye un paquete o interpreta una captura del protocolo." badge="ROBO2 · EX01v8" actions={<div className="afr2-action-row"><Button variant="subtle" onClick={() => setHelpOpen(true)}>Cómo usar el estudio</Button><Link className="afr2-developer-back" to="/desarrollador"><UiIcon name="back" />Desarrollador</Link></div>} />
    <TaskSections label="Herramientas de protocolo" value={active} onValueChange={setActive} items={[
      { id: 'builder', label: 'Constructor', content: <div className="afr2-protocol-builder">
        <div className="afr2-page-stack">
          <StudioPanel title="Constructor de paquete" badge="Preparación local">
            <div className="afr2-protocol-subpanel"><h3>Ruta del paquete</h3><Field id="protocol-direction" label="Origen y destino"><select id="protocol-direction" value={builder.response ? 'response' : 'request'} onChange={event => setBuilder(previous => ({ ...previous, response: event.target.value === 'response', body: '' }))}><option value="request">ESP (3) → ATmega (1) · petición · 31</option><option value="response">ATmega (1) → ESP (3) · respuesta · 13</option></select></Field><div className="afr2-protocol-columns"><Field id="protocol-session" label="Sesión u32"><input id="protocol-session" value={builder.session} onChange={event => setBuilder(previous => ({ ...previous, session: event.target.value }))} autoComplete="off" /></Field><Field id="protocol-transaction" label="Transacción u32"><input id="protocol-transaction" value={builder.transaction} onChange={event => setBuilder(previous => ({ ...previous, transaction: event.target.value }))} autoComplete="off" /></Field></div><p className="afr2-help">Sesión y transacción 1 son ejemplos editables; no representan una sesión negociada.</p></div>
            <div className="afr2-protocol-subpanel"><h3>Comando</h3><Field id="protocol-command" label="Seleccionar comando"><select id="protocol-command" value={builder.command} onChange={event => changeCommand(Number(event.target.value))}>{COMMAND_GROUPS.map(group => <optgroup key={group} label={group}>{COMMANDS.filter(item => groupFor(item.id) === group).map(item => <option key={item.id} value={item.id} disabled={item.retired || item.readOnly}>0x{item.id.toString(16).toUpperCase().padStart(2, '0')} · {item.name}{item.retired ? ' · retirado' : item.readOnly ? ' · solo lectura' : ''}</option>)}</optgroup>)}</select></Field><div className="afr2-protocol-command-note"><strong>{command.name}</strong><p>{command.description}</p></div></div>
            {builder.response && <Field id="protocol-result" label="Resultado"><select id="protocol-result" value={builder.result} onChange={event => setBuilder(previous => ({ ...previous, result: Number(event.target.value) }))}>{RESULT_NAMES.map((name, value) => <option key={name} value={value}>{value} · {name}</option>)}</select></Field>}
          </StudioPanel>
          {!builder.response && form && <StudioPanel title="Parámetros" badge={manualOverride ? 'HEX manual activo' : 'Cuerpo automático'}>
            {schemas.length > 1 && <Field id="protocol-variant" label="Variante del comando"><select id="protocol-variant" value={form.id} onChange={event => { const next = schemas.find(item => item.id === event.target.value)!; setVariant(next.id); setParameters(initialParameters(next)); }}>{schemas.map(item => <option key={item.id} value={item.id}>{item.label}</option>)}</select></Field>}
            {manualOverride && <p className="afr2-protocol-warning">El cuerpo HEX manual reemplaza estos parámetros. Puedes vaciarlo para volver al formulario.</p>}
            {form.fields.length ? <fieldset disabled={manualOverride} className="afr2-protocol-parameters"><legend className="sr-only">Parámetros de {command.name}</legend>{form.fields.map(item => <Field key={item.id} id={`protocol-param-${item.id}`} label={item.label} unit={item.unit ?? item.type} hint={item.options ? undefined : `${item.type} · entero${item.min !== undefined ? ` · mínimo ${item.min}` : ''}${item.max !== undefined ? ` · máximo ${item.max}` : ''}${item.step && item.step !== 1 ? ` · múltiplo de ${item.step}` : ''}`}>
              {item.options ? <select id={`protocol-param-${item.id}`} value={parameters[item.id] ?? item.initial} onChange={event => setParameters(previous => ({ ...previous, [item.id]: event.target.value }))}>{item.options.map(option => <option key={option.value} value={option.value}>{option.label}</option>)}</select> : <input id={`protocol-param-${item.id}`} type="number" step={item.step ?? 1} min={item.min} max={item.max} value={parameters[item.id] ?? item.initial} onChange={event => setParameters(previous => ({ ...previous, [item.id]: event.target.value }))} aria-describedby={`protocol-param-${item.id}-hint`} />}
            </Field>)}</fieldset> : <p className="afr2-help">{form.prefix?.length ? 'El comando incluye una identidad o confirmación fija; no requiere campos adicionales.' : 'Esta consulta no requiere cuerpo de aplicación.'}</p>}
            {form.prefix?.length === 4 && form.prefix.every(byte => byte === 0) && <p className="afr2-help">El constructor antepone reserved:u32=0. Los límites operativos dependen del estado y la configuración del firmware.</p>}
          </StudioPanel>}
          <StudioPanel title="Cuerpo de aplicación" description="No incluye sesión, transacción ni resultado; se agregan automáticamente.">
            <details className="afr2-protocol-extra" open={builder.response || !form}><summary>HEX manual · {builder.response ? 'respuestas' : 'avanzado'}</summary><Field id="protocol-body" label="Bytes HEX separados por espacios" hint={builder.response ? 'Si el resultado es un error, deja el cuerpo vacío.' : 'Si completas este campo, reemplaza el cuerpo generado por los parámetros.'}><textarea id="protocol-body" rows={4} value={builder.body} spellCheck={false} onChange={event => setBuilder(previous => ({ ...previous, body: event.target.value }))} aria-describedby="protocol-body-hint" /></Field>{manualOverride && !builder.response && form && <Button variant="subtle" onClick={() => setBuilder(previous => ({ ...previous, body: '' }))}>Volver a parámetros</Button>}</details>
            {!builder.response && form && <CopyBlock title="Cuerpo preparado HEX" value={built.body} rows={3} />}
            <p className="afr2-help">{(builder.response ? command.replyBytes : command.requestBytes) === undefined ? 'Cuerpo manual según contrato. Su semántica y longitud específica no se comprueban por completo.' : `Cuerpo ${builder.response ? 'OK' : 'de petición'}: ${bodySizeText(builder.response ? command.replyBytes : command.requestBytes)} bytes.`}</p>
          </StudioPanel>
        </div>
        <div className="afr2-page-stack">
          {built.bytes ? <PacketStructure bytes={built.bytes} /> : <StudioPanel title="Estructura del paquete"><div className="afr2-protocol-empty" role="status"><h3>Prepara el paquete</h3><p>{built.error}</p></div></StudioPanel>}
          <StudioPanel title="Salidas listas para copiar" description="El mismo paquete en cuatro formatos, con decimal como salida adicional.">
            {built.bytes ? <><PacketOutputs bytes={built.bytes} /><Button variant="primary" onClick={inspectBuilder}>Abrir en el traductor</Button></> : <p className="afr2-help">Completa los campos para generar las salidas.</p>}
            <p className="afr2-help">El estudio prepara y copia bytes localmente. La trama no se envía al robot.</p>
          </StudioPanel>
        </div>
      </div> },
      { id: 'translator', label: 'Traductor / escáner', content: <div className="afr2-page-stack">
        <StudioPanel title="Traductor / escáner" description="Pega un comando, una trama o un bloque largo del monitor. El estudio separa paquetes y bytes fuera de protocolo.">
          <div className="afr2-protocol-input-heading"><div><p className="afr2-help">HEX con espacios o continuo, 0xHH, arrays C y Python bytes. También reconoce nombres del catálogo y CMD sueltos.</p></div><Field id="protocol-input-format" label="Entrada"><select id="protocol-input-format" value={format} onChange={event => { setFormat(event.target.value as InputFormat); setResult(null); setError(''); }}><option value="hex">HEX / captura de monitor</option><option value="decimal">Bytes decimales</option><option value="text">Texto UTF-8 → bytes</option></select></Field></div>
          <label className="afr2-protocol-input-label" htmlFor="protocol-capture">Captura o comando</label>
          <textarea id="protocol-capture" value={input} onChange={event => updateInput(event.target.value)} rows={8} spellCheck={false} autoCapitalize="off" autoCorrect="off" aria-invalid={!!error} aria-describedby={error ? 'protocol-input-error' : 'protocol-input-hint'} placeholder={'--- 00:30:12 | COM8 | 74880 | UTF-8 ---\n0D 0A 20 65 74 73 20 4A 61 6E 20 20 38 20 32 30\n31 33 2C 72 73 74 20 63 61 75 73 65 3A 32\n\nTambién puedes escribir ALIVE o 0x02.'} onKeyDown={event => { if ((event.ctrlKey || event.metaKey) && event.key === 'Enter') { event.preventDefault(); runTranslation(); } }} />
          <p id="protocol-input-hint" className="afr2-help">Hasta 64 KiB de bytes / {MAX_INPUT_CHARS.toLocaleString('es-AR')} caracteres. Ctrl+Enter traduce. Cabeceras --- ... --- admitidas; arrays solo numéricos sin 0x se leen en decimal.</p>
          {error && <p id="protocol-input-error" className="afr2-protocol-error" role="alert">{error}</p>}
          <div className="afr2-action-row"><Button variant="primary" disabled={!input.trim()} onClick={() => runTranslation()}>Traducir / escanear</Button><Button disabled={!built.bytes} onClick={inspectBuilder}>Cargar frame generado</Button><Button disabled={!input} onClick={() => updateInput('')}>Limpiar</Button><Button variant="subtle" onClick={() => { updateInput(BOOT_EXAMPLE); setFormat('hex'); }}>Ejemplo de arranque</Button></div>
        </StudioPanel>
        {result ? <CaptureInspector key={revision} result={result} outputFormat={outputFormat} onOutputFormatChange={setOutputFormat} textFormat={textFormat} onTextFormatChange={setTextFormat} /> : !error && <div className="afr2-protocol-empty"><h3>Tu captura, campo por campo</h3><p>Traduce para ver el mapa del stream, los paquetes detectados y el texto convertido.</p></div>}
      </div> },
    ]} />
    <FloatingModal open={helpOpen} onClose={() => setHelpOpen(false)} title="Cómo usar Protocol Studio" description="Constructor, traductor y escáner de bloques en un mismo espacio.">
      <div className="afr2-page-stack"><p><strong>Constructor.</strong> Elige dirección y comando; completa los parámetros. El cuerpo se codifica en little-endian y se calculan N y XOR. Los bloques de la trama muestran offsets y significado.</p><p><strong>HEX manual.</strong> Reemplaza el cuerpo del formulario. Las respuestas se preparan con resultado y cuerpo manual; un error no lleva cuerpo.</p><p><strong>Salidas.</strong> Copia hexadecimal, RealTerm, C o Python. Abre el mismo paquete en el traductor o cárgalo desde esa pestaña.</p><p><strong>Traductor.</strong> Acepta capturas, arrays, CMD o nombres. El mapa separa tramas, candidatos inválidos, incompletos y bytes crudos. Selecciona un bloque para analizarlo; Stream continuo conserva todos los bytes.</p><p><strong>Texto y arranque ESP.</strong> Texto escapado conserva cada byte; UTF-8 advierte sustituciones. El log de arranque se identifica por su texto sin deducir causas de cableado o de firmware.</p><p className="afr2-help">Framing ROBO2 correcto no confirma sesión negociada ni acción ejecutada. La interpretación semántica es parcial. Esta herramienta no transmite comandos ni abre COM.</p></div>
    </FloatingModal>
  </div>;
}
