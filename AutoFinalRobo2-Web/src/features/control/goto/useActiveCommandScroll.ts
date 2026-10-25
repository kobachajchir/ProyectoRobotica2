import { useCallback, useEffect, useLayoutEffect, useRef, useState } from 'react';

/** AFR2-20261024-10. Scroll FLIP: commit the native scroll offset, then animate
 * only the list transform back to zero. No page scrolling or focus changes.
 * Cancel at the current visual position; wheel/touch/keys can take over. */
export function useActiveCommandScroll(activeId: string | null, follow: boolean, playing: boolean, expanded = true) {
  const viewportRef = useRef<HTMLDivElement>(null);
  const listRef = useRef<HTMLOListElement>(null);
  const animationRef = useRef<Animation | null>(null);
  const previousId = useRef<string | null>(null);
  const [reduceMotion, setReduceMotion] = useState(() => window.matchMedia('(prefers-reduced-motion: reduce)').matches);
  useEffect(() => {
    const query = window.matchMedia('(prefers-reduced-motion: reduce)');
    const changed = () => setReduceMotion(query.matches);
    query.addEventListener('change', changed); return () => query.removeEventListener('change', changed);
  }, []);
  const interrupt = useCallback(() => {
    const viewport = viewportRef.current, list = listRef.current, animation = animationRef.current;
    if (!viewport || !list || !animation) return;
    const transform = getComputedStyle(list).transform;
    const offset = transform === 'none' ? 0 : new DOMMatrixReadOnly(transform).m42;
    animationRef.current = null; animation.cancel();
    viewport.scrollTop -= offset;
  }, []);
  useLayoutEffect(() => {
    const before = previousId.current; previousId.current = activeId;
    const viewport = viewportRef.current, list = listRef.current;
    if (!viewport || !list || !activeId || !follow) return interrupt;
    const row = Array.from(list.children).find(element => (element as HTMLElement).dataset.commandId === activeId) as HTMLElement | undefined;
    if (!row) return interrupt;
    const from = viewport.scrollTop;
    const to = Math.max(0, Math.min(viewport.scrollHeight - viewport.clientHeight, row.offsetTop - (viewport.clientHeight - row.offsetHeight) / 2));
    viewport.scrollTop = to;
    const delta = viewport.scrollTop - from;
    if (!reduceMotion && document.documentElement.dataset.input !== 'keyboard' && playing && before !== null && before !== activeId && Math.abs(delta) >= 1) {
      const style = getComputedStyle(list);
      const token = style.getPropertyValue('--duration-normal').trim();
      const duration = parseFloat(token) * (token.endsWith('ms') ? 1 : 1000);
      const easing = style.getPropertyValue('--ease-in-out').trim();
      if (Number.isFinite(duration) && duration > 0 && easing) {
        const animation = list.animate([{ transform: `translateY(${delta}px)` }, { transform: 'translateY(0)' }], { duration, easing });
        animationRef.current = animation;
        animation.onfinish = () => { if (animationRef.current === animation) animationRef.current = null; };
      }
    }
    return interrupt;
  }, [activeId, follow, playing, reduceMotion, interrupt, expanded]);
  return { viewportRef, listRef, interrupt };
}
