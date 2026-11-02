import { useCallback, useEffect, useMemo, useRef, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { UiIcon } from '../../components/ui/UiIcon';
import { OledCanvas128x64 } from '../oled/OledCanvas128x64';
import { Afr2OledBuffer } from '../oled/oledRasterizer';
import { renderFirmwareScreen } from '../oled/firmwareRenderer';
import { getScreenMetadata } from '../oled/screenCatalog';
import { createUiSimulation, simulationSnapshot } from './uiSimulation';
import { SIMULATION_FLOW, type SimulationFlowNode } from './uiSimulationFlow';

/**
 * AFR2-20261102-01. Árbol de pantallas como mapa conceptual: crece desde el dashboard (raíz) hacia
 * cada pantalla por las opciones del menú y los gestos del encoder. Mismo grafo que el simulador UI
 * (SIMULATION_FLOW, adaptado del ATmega). Una pantalla alcanzable por varios caminos aparece una vez
 * completa y como referencia en los demás, para no crear ciclos. Tocar un nodo muestra su ficha.
 */

interface TreeNode {
  key: string;
  node: number;            // -1: grupo de pantallas alcanzadas por acciones o avisos
  via?: string;            // opción o gesto que lleva a esta pantalla
  children: TreeNode[];
  reference: boolean;
}

const ROOT_TRANSITIONS: Record<number, { label: string; target: number }[]> = {
  0: [{ label: 'SW corto', target: 1 }, { label: 'SW largo al soltar', target: 58 }],
  23: [{ label: 'SW corto: opciones', target: 59 }],
  49: [{ label: 'SW corto: opciones', target: 59 }],
  53: [{ label: 'SW corto: iniciar conteo', target: 54 }],
};
const OTHERS = 'otros';

function buildTree(): TreeNode {
  const seen = new Set<number>();
  let refs = 0;
  const make = (node: number, via?: string): TreeNode | null => {
    const entry = SIMULATION_FLOW[node];
    if (!entry) return null;
    if (seen.has(node)) return { key: `ref-${refs++}`, node, via, children: [], reference: true };
    seen.add(node);
    const tree: TreeNode = { key: `n-${node}`, node, via, children: [], reference: false };
    const links = [
      ...entry.options.filter(item => !(item.action || item.target === 255 || item.label.startsWith('Volver'))).map(item => ({ label: item.label, target: item.target })),
      ...(ROOT_TRANSITIONS[node] ?? []),
    ];
    for (const link of links) { const child = make(link.target, link.label); if (child) tree.children.push(child); }
    return tree;
  };
  const root = make(0)!;
  const rest = SIMULATION_FLOW.filter(entry => !seen.has(entry.node));
  if (rest.length) {
    const group: TreeNode = { key: OTHERS, node: -1, children: [], reference: false };
    for (const entry of rest) { const child = make(entry.node); if (child) group.children.push(child); }
    root.children.push(group);
  }
  return root;
}

const W = 216, H = 58, COL = 268, ROW = 70, PAD = 16;
interface Placed { tree: TreeNode; depth: number; x: number; y: number; open: boolean; parent: string | null }

function layout(root: TreeNode, expanded: ReadonlySet<string>) {
  const placed: Placed[] = [];
  const edges: { key: string; x1: number; y1: number; x2: number; y2: number; reference: boolean }[] = [];
  let row = 0, depthMax = 0;
  const place = (tree: TreeNode, depth: number, parent: string | null): number => {
    depthMax = Math.max(depthMax, depth);
    const open = expanded.has(tree.key) && tree.children.length > 0;
    let y: number;
    if (!open) { y = PAD + row * ROW; row++; }
    else {
      const ys = tree.children.map(child => place(child, depth + 1, tree.key));
      y = (ys[0] + ys[ys.length - 1]) / 2;
      tree.children.forEach((child, index) => edges.push({ key: `${tree.key}>${child.key}`, x1: PAD + depth * COL + W, y1: y + H / 2, x2: PAD + (depth + 1) * COL, y2: ys[index] + H / 2, reference: child.reference }));
    }
    placed.push({ tree, depth, x: PAD + depth * COL, y, open, parent });
    return y;
  };
  place(root, 0, null);
  return { placed, edges, width: PAD * 2 + depthMax * COL + W, height: PAD * 2 + Math.max(row - 1, 0) * ROW + H };
}

const ROLE: Record<string, string> = { view: 'Vista', menu: 'Menú', action: 'Acción', notice: 'Aviso', dialog: 'Diálogo', editor: 'Editor', invoker: 'Invocador' };
// La raíz es el dashboard (010001); su nombre de firmware es el del proyecto.
const title = (entry: SimulationFlowNode) => entry.node === 0 ? 'Dashboard' : getScreenMetadata(entry.code)?.name ?? entry.title;

function preview(node: number) {
  try {
    const state = createUiSimulation(); state.node = node;
    const output = new Afr2OledBuffer();
    renderFirmwareScreen(state, output, simulationSnapshot(state));
    return output;
  } catch {
    return null; // pantallas que necesitan un evento o dato previo
  }
}

export function ScreenTreeMap({ onOpen, openLabel = 'Abrir en el simulador' }: { onOpen?: (code: string) => void; openLabel?: string }) {
  const root = useMemo(buildTree, []);
  const byKey = useMemo(() => {
    const map = new Map<string, { tree: TreeNode; parent: TreeNode | null }>();
    const walk = (tree: TreeNode, parent: TreeNode | null) => { map.set(tree.key, { tree, parent }); tree.children.forEach(child => walk(child, tree)); };
    walk(root, null);
    return map;
  }, [root]);
  const [expanded, setExpanded] = useState<Set<string>>(() => new Set([root.key, ...root.children.map(child => child.key)]));
  const [selected, setSelected] = useState(root.key);
  const canvas = useRef<HTMLDivElement>(null);
  const { placed, edges, width, height } = useMemo(() => layout(root, expanded), [root, expanded]);

  const toggle = (key: string) => setExpanded(previous => {
    const next = new Set(previous); if (next.has(key)) next.delete(key); else next.add(key); return next;
  });
  const reveal = useCallback((key: string) => {
    // Abre la rama hasta el nodo y lo trae a la vista dentro del mapa (no desplaza la página).
    setExpanded(previous => {
      const next = new Set(previous);
      let current = byKey.get(key)?.parent;
      while (current) { next.add(current.key); current = byKey.get(current.key)?.parent ?? null; }
      return next;
    });
    setSelected(key);
  }, [byKey]);
  useEffect(() => {
    const box = canvas.current, item = canvas.current?.querySelector<HTMLElement>(`[data-key="${selected}"]`);
    if (!box || !item) return;
    const left = item.offsetLeft - 24, right = item.offsetLeft + item.offsetWidth + 24, top = item.offsetTop - 24, bottom = item.offsetTop + item.offsetHeight + 24;
    if (left < box.scrollLeft) box.scrollLeft = left; else if (right > box.scrollLeft + box.clientWidth) box.scrollLeft = right - box.clientWidth;
    if (top < box.scrollTop) box.scrollTop = top; else if (bottom > box.scrollTop + box.clientHeight) box.scrollTop = bottom - box.clientHeight;
  }, [selected, placed]);

  const current = byKey.get(selected);
  const target = current?.tree.reference ? [...byKey.values()].find(item => !item.tree.reference && item.tree.node === current.tree.node) : current;
  const entry = target && target.tree.node >= 0 ? SIMULATION_FLOW[target.tree.node] : null;
  const meta = entry ? getScreenMetadata(entry.code) : null;
  const image = useMemo(() => entry ? preview(entry.node) : null, [entry]);
  const path: TreeNode[] = [];
  for (let item = target ?? null; item; item = item.parent ? byKey.get(item.parent.key) ?? null : null) path.unshift(item.tree);

  return <div className="afr2-tree-map">
    <div className="afr2-tree-map__toolbar">
      <p className="afr2-help">Crece desde el dashboard por las opciones del menú y los gestos del encoder. Tocá una pantalla para ver su ficha; la flecha abre o cierra su rama.</p>
      <div className="afr2-action-row">
        <Button size="sm" onClick={() => setExpanded(new Set([...byKey.values()].filter(item => item.tree.children.length).map(item => item.tree.key)))}>Expandir todo</Button>
        <Button size="sm" onClick={() => setExpanded(new Set([root.key]))}>Contraer</Button>
      </div>
    </div>
    <div className="afr2-tree-map__layout">
      <div ref={canvas} className="afr2-tree-map__canvas" role="group" aria-label="Mapa de pantallas del robot">
        <div className="afr2-tree-map__plane" style={{ width, height }}>
          <svg className="afr2-tree-map__edges" width={width} height={height} aria-hidden="true">
            {edges.map(edge => { const mid = (edge.x1 + edge.x2) / 2; return <path key={edge.key} data-reference={edge.reference}
              d={`M${edge.x1} ${edge.y1} C${mid} ${edge.y1} ${mid} ${edge.y2} ${edge.x2} ${edge.y2}`} />; })}
          </svg>
          {placed.map(({ tree, x, y, open, depth }) => {
            const node = tree.node >= 0 ? SIMULATION_FLOW[tree.node] : null;
            const name = node ? title(node) : 'Por acciones, eventos o inspección';
            const isSelected = selected === tree.key;
            return <div key={tree.key} className="afr2-tree-node" data-key={tree.key} data-root={depth === 0} data-reference={tree.reference}
              data-group={tree.node < 0} data-selected={isSelected} style={{ left: x, top: y, width: W, height: H }}>
              <button type="button" className="afr2-tree-node__body" aria-pressed={isSelected}
                aria-label={`${name}${node ? `, ${node.code}` : ''}${tree.reference ? ', referencia a otra rama' : ''}`}
                onClick={() => setSelected(tree.key)}>
                {tree.via && tree.via !== name && <span className="afr2-tree-node__via">{tree.via}</span>}
                <strong>{name}</strong>
                <code>{tree.reference ? `↪ ${node?.code}` : node?.code ?? `${tree.children.length} pantallas`}</code>
              </button>
              {tree.children.length > 0 && <button type="button" className="afr2-tree-node__toggle" aria-expanded={open}
                aria-label={`${open ? 'Ocultar' : 'Mostrar'} ${tree.children.length} subpantallas de ${name}`} onClick={() => toggle(tree.key)}>
                <UiIcon name="chevron-down" size={16} style={{ transform: `rotate(${open ? 90 : -90}deg)` }} />
                <span>{tree.children.length}</span>
              </button>}
            </div>;
          })}
        </div>
      </div>
      <aside className="afr2-tree-map__detail" aria-live="polite" aria-label="Ficha de la pantalla">
        {target && target.tree.node < 0 ? <>
          <p className="afr2-kicker">Grupo</p>
          <h3>Por acciones, eventos o inspección</h3>
          <p className="afr2-help">Pantallas que no se abren desde una opción de menú: avisos, resultados de acciones, editores y vistas que se alcanzan por un evento del robot o desde el catálogo.</p>
        </> : entry ? <>
          <p className="afr2-kicker">{target?.tree.key === root.key ? 'Pantalla principal' : ROLE[meta?.role ?? ''] ?? 'Pantalla'}</p>
          <h3>{title(entry)}</h3>
          <dl className="afr2-tree-map__facts">
            <div><dt>ScreenCode</dt><dd><code>{entry.code}</code></dd></div>
            <div><dt>Tipo</dt><dd>{ROLE[meta?.role ?? ''] ?? meta?.role ?? '—'}</dd></div>
            <div><dt>Se llega por</dt><dd>{target?.tree.via ?? (target?.tree.key === root.key ? 'Arranque' : 'Evento o acción')}</dd></div>
            <div><dt>Subpantallas</dt><dd>{target?.tree.children.length ?? 0}</dd></div>
          </dl>
          {image && <div className="afr2-tree-map__preview"><OledCanvas128x64 compact oledBuffer={image} screenCode={entry.code} sourceMode="local-ui-simulation" showGrid={false} scale={2} /></div>}
          <dl className="afr2-tree-map__gestures">
            <div><dt>Girar</dt><dd>{entry.gestures.rotate}</dd></div>
            <div><dt>SW corto</dt><dd>{entry.gestures.short}</dd></div>
            <div><dt>SW largo</dt><dd>{entry.gestures.long}</dd></div>
          </dl>
          {path.length > 1 && <p className="afr2-tree-map__path" aria-label="Camino desde el dashboard">
            {path.map((item, index) => { const step = item.node >= 0 ? title(SIMULATION_FLOW[item.node]) : 'Acciones'; return <span key={item.key}>
              {index > 0 && <UiIcon name="arrow" size={16} />}
              <button type="button" onClick={() => reveal(item.key)}>{step}</button>
            </span>; })}
          </p>}
          {target && target.tree.children.length > 0 && <div className="afr2-tree-map__children">
            <p className="afr2-kicker">Pantallas que se abren desde acá</p>
            <ul>{target.tree.children.map(child => { const node = child.node >= 0 ? SIMULATION_FLOW[child.node] : null; return <li key={child.key}>
              <button type="button" onClick={() => reveal(child.key)}>{node ? title(node) : 'Acciones'}{node && <code>{node.code}</code>}</button>
            </li>; })}</ul>
          </div>}
          {current?.tree.reference && <p className="afr2-help">Esta pantalla también se abre desde otra rama; la ficha muestra su ubicación principal.</p>}
          {onOpen && <Button variant="primary" icon={<UiIcon name="screen" />} onClick={() => onOpen(entry.code)}>{openLabel}</Button>}
        </> : <p className="afr2-help">Elegí una pantalla del mapa.</p>}
      </aside>
    </div>
  </div>;
}
