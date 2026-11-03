/* AFR2-20261102-24: primitivas de las páginas de Explorar (Infraestructura, Loader), extraídas
   sin cambios de InfrastructurePage (AFR2-20261102-07). Estilos en styles/infrastructure.css. */
import { useEffect, useRef, useState, type CSSProperties, type ReactNode } from 'react';
import { UiIcon, type UiIconName } from '../../components/ui/UiIcon';

/** Marca la sección como visible la primera vez que entra en pantalla (para animar su entrada una sola vez). */
export function useReveal<T extends HTMLElement>() {
  const ref = useRef<T>(null);
  const [visible, setVisible] = useState(false);
  useEffect(() => {
    const node = ref.current;
    if (!node || typeof IntersectionObserver === 'undefined') { setVisible(true); return; }
    const observer = new IntersectionObserver(entries => {
      if (entries.some(entry => entry.isIntersecting)) { setVisible(true); observer.disconnect(); }
    }, { rootMargin: '0px 0px -10% 0px' });
    observer.observe(node);
    return () => observer.disconnect();
  }, []);
  return [ref, visible] as const;
}
export function Reveal({ as: Tag = 'section', className = '', children, ...rest }: { as?: 'section' | 'div'; className?: string; children: ReactNode } & React.HTMLAttributes<HTMLElement>) {
  const [ref, visible] = useReveal<HTMLElement>();
  return <Tag ref={ref as never} className={`afr2-infra-reveal ${className}`} data-visible={visible} {...rest}>{children}</Tag>;
}

export function Wire({ delay = 0, label }: { delay?: number; label?: string }) {
  return <span className="afr2-infra-wire" aria-hidden="true" style={{ '--wire-delay': `${delay}s` } as CSSProperties}>
    <i /><i />{label && <em>{label}</em>}
  </span>;
}
export function FlowNode({ icon, title, detail, zone, onPick }: { icon: UiIconName; title: string; detail: string; zone: string; onPick: () => void }) {
  return <button type="button" className="afr2-infra-node" data-zone={zone} onClick={onPick} aria-label={`${title}: ${detail}. Ver esta etapa en el recorrido`}>
    <span className="afr2-infra-node__icon"><UiIcon name={icon} size={22} /></span>
    <strong>{title}</strong><span>{detail}</span>
  </button>;
}
export function Stat({ icon, label, value, detail, bytes }: { icon: UiIconName; label: string; value: string; detail: string; bytes?: number }) {
  return <div className="afr2-infra-stat"><span className="afr2-infra-stat__icon"><UiIcon name={icon} size={18} /></span>
    <span className="afr2-infra-stat__label">{label}</span><strong title={bytes === undefined ? undefined : `${bytes.toLocaleString('es-AR')} bytes`}>{value}</strong><small>{detail}</small></div>;
}

