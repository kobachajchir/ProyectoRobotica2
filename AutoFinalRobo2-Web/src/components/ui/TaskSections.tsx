import { useId, useState, type ReactNode } from 'react';

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
      <div role="tablist" aria-label={resolvedLabel} className="afr2-task-nav">
        {items.map((item, index) => (
          <button
            key={item.id}
            type="button"
            role="tab"
            id={`${uid}-tab-${item.id}`}
            aria-controls={`${uid}-panel-${item.id}`}
            aria-selected={active === item.id}
            tabIndex={active === item.id ? 0 : -1}
            onClick={() => select(item.id)}
            onKeyDown={(event) => {
              let next = index;
              if (event.key === 'ArrowRight') next = (index + 1) % items.length;
              else if (event.key === 'ArrowLeft') next = (index - 1 + items.length) % items.length;
              else if (event.key === 'Home') next = 0;
              else if (event.key === 'End') next = items.length - 1;
              else return;
              event.preventDefault();
              select(items[next].id);
              document.getElementById(`${uid}-tab-${items[next].id}`)?.focus();
            }}
          >
            {item.label}
          </button>
        ))}
      </div>
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
