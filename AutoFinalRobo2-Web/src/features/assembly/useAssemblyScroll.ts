import { useCallback, useEffect, useRef, useState } from 'react';
import { clamp01 } from './assemblyTimeline';

/** Native document scroll owns the timeline; no wheel hijack for vertical input.
 * Horizontal wheel gestures map only while the pinned scene is in view.
 * The slider, chapter links and touch panning use that same source of truth. */
export function useAssemblyScroll() {
  const track = useRef<HTMLElement>(null);
  const stage = useRef<HTMLDivElement>(null);
  const [progress, setProgress] = useState(0);
  const [systemReduced, setSystemReduced] = useState(() => window.matchMedia('(prefers-reduced-motion: reduce)').matches);
  const [gentle, setGentle] = useState(false);
  const reduced = systemReduced || gentle;

  const metrics = useCallback(() => {
    if (!track.current || !stage.current) return null;
    const top = parseFloat(getComputedStyle(stage.current).top) || 0;
    const box = track.current.getBoundingClientRect();
    return { start: window.scrollY + box.top - top, distance: Math.max(1, box.height - stage.current.offsetHeight) };
  }, []);

  useEffect(() => {
    const query = window.matchMedia('(prefers-reduced-motion: reduce)');
    const change = () => setSystemReduced(query.matches);
    query.addEventListener('change', change);
    return () => query.removeEventListener('change', change);
  }, []);

  useEffect(() => {
    let frame = 0;
    const read = () => {
      frame = 0; const view = metrics();
      if (view) setProgress(clamp01((window.scrollY - view.start) / view.distance));
    };
    const schedule = () => { if (!frame) frame = requestAnimationFrame(read); };
    const resize = new ResizeObserver(schedule);
    if (track.current) resize.observe(track.current);
    if (stage.current) resize.observe(stage.current);
    const topbar = document.querySelector('.afr2-topbar');
    const measureHeader = () => {
      if (topbar && track.current) track.current.style.setProperty('--assembly-top', `${topbar.getBoundingClientRect().height + 12}px`);
      schedule();
    };
    const headerResize = new ResizeObserver(measureHeader);
    if (topbar) headerResize.observe(topbar);
    measureHeader();
    const wheel = (event: WheelEvent) => {
      if (event.ctrlKey || Math.abs(event.deltaX) <= Math.abs(event.deltaY)) return;
      const view = metrics(); if (!view) return;
      if (window.scrollY < view.start - 2 || window.scrollY > view.start + view.distance + 2) return;
      if ((window.scrollY <= view.start && event.deltaX < 0) || (window.scrollY >= view.start + view.distance && event.deltaX > 0)) return;
      const target = event.target as HTMLElement;
      if (target.closest('button, input, a, nav')) return;
      event.preventDefault();
      const pixels = event.deltaX * (event.deltaMode === 1 ? 16 : event.deltaMode === 2 ? window.innerHeight : 1);
      window.scrollBy({ top: pixels, behavior: 'instant' });
    };
    window.addEventListener('scroll', schedule, { passive: true });
    window.addEventListener('resize', schedule, { passive: true });
    const element = track.current;
    element?.addEventListener('wheel', wheel, { passive: false });
    read();
    return () => {
      cancelAnimationFrame(frame); resize.disconnect(); headerResize.disconnect();
      window.removeEventListener('scroll', schedule); window.removeEventListener('resize', schedule);
      element?.removeEventListener('wheel', wheel);
    };
  }, [metrics]);

  const seek = useCallback((next: number) => {
    const view = metrics(); if (!view) return;
    // Keep keyboard navigation and scrubbing immediate, with no queued motion.
    window.scrollTo({ top: view.start + view.distance * clamp01(next), behavior: 'instant' });
    setProgress(clamp01(next));
  }, [metrics]);
  return { track, stage, progress, seek, reduced, gentle, setGentle, systemReduced };
}
