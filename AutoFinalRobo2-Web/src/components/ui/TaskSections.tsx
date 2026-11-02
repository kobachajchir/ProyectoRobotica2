import { useId, useState, type ReactNode } from 'react';
import { SegmentedControl } from './SegmentedControl';

interface TaskItem {
  id: string;
  label: string;
  content: ReactNode;
}
export interface TaskSectionsProps {
  label?: string;
  ariaLabel?: string;
  items: TaskItem[];
  value?: string;
  activeId?: string;
  onValueChange?: (value: string) => void;
  onChange?: (value: string) => void;
}

/** One workspace at a time. Page-owned drafts survive workspace changes. */
export function TaskSections({
  label,
  ariaLabel,
  items,
  value,
  activeId,
  onValueChange,
  onChange,
}: TaskSectionsProps) {
  const uid = useId();
  const [selected, setSelected] = useState(items[0]?.id ?? '');
  const active = value ?? activeId ?? selected;
  const resolvedLabel = label ?? ariaLabel ?? 'Secciones';
  const select = (id: string) => {
    setSelected(id);
    onValueChange?.(id);
    onChange?.(id);
  };
  return (
    <div className="afr2-workspace">
      {/* AFR2-20261101-09 / VIS-052: misma API; la navegación usa la ranura con pill deslizante. */}
      <SegmentedControl
        className="afr2-task-nav"
        variant="tabs"
        ariaLabel={resolvedLabel}
        idPrefix={uid}
        items={items.map((item) => ({ value: item.id, label: item.label }))}
        value={active}
        onChange={select}
      />
      {items.map((item) => (
        <section
          key={item.id}
          role="tabpanel"
          tabIndex={0}
          id={`${uid}-panel-${item.id}`}
          aria-labelledby={`${uid}-tab-${item.id}`}
          hidden={active !== item.id}
          className="afr2-task-panel"
        >
          {active === item.id ? item.content : null}
        </section>
      ))}
    </div>
  );
}
