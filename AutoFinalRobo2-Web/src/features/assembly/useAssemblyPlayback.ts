import { useCallback, useEffect, useRef, useState } from 'react';
import { ASSEMBLY_DURATION, assemblyProgressAtTime, assemblyTimeAtProgress } from './assemblyPlaybackTimeline';

/** Visual playback uses the browser's monotonic frame clock, never media time.
 * Pausing or manual scrubbing preserves the same scene progress. */
export function useAssemblyPlayback(progress: number, seek: (value: number) => void) {
  const [playing, setPlaying] = useState(false);
  const frame = useRef(0), active = useRef(false);
  const generation = useRef(0);
  const pause = useCallback(() => {
    generation.current++;
    active.current = false; cancelAnimationFrame(frame.current); setPlaying(false);
  }, []);
  const toggle = useCallback(() => {
    if (active.current) { pause(); return; }
    const start = progress >= .999 ? 0 : progress;
    const run = ++generation.current, offset = assemblyTimeAtProgress(start), began = performance.now();
    seek(start); active.current = true; setPlaying(true);
    const tick = (now: number) => {
      if (!active.current || generation.current !== run) return;
      const seconds = offset + (now - began) / 1000;
      seek(seconds >= ASSEMBLY_DURATION ? 1 : assemblyProgressAtTime(seconds));
      if (seconds >= ASSEMBLY_DURATION) pause(); else frame.current = requestAnimationFrame(tick);
    };
    frame.current = requestAnimationFrame(tick);
  }, [pause, progress, seek]);
  useEffect(() => {
    const manual = (event: Event) => {
      if (event.target instanceof Element && event.target.closest('[data-assembly-play]')) return;
      pause();
    };
    const hidden = () => { if (document.hidden) pause(); };
    window.addEventListener('wheel', manual, { passive: true });
    window.addEventListener('touchstart', manual, { passive: true });
    window.addEventListener('pointerdown', manual, { passive: true });
    window.addEventListener('keydown', manual);
    window.addEventListener('blur', pause);
    document.addEventListener('visibilitychange', hidden);
    return () => {
      active.current = false; cancelAnimationFrame(frame.current);
      generation.current++;
      window.removeEventListener('wheel', manual); window.removeEventListener('touchstart', manual);
      window.removeEventListener('pointerdown', manual); window.removeEventListener('keydown', manual);
      window.removeEventListener('blur', pause); document.removeEventListener('visibilitychange', hidden);
    };
  }, [pause]);
  const manualSeek = useCallback((value: number) => { pause(); seek(value); }, [pause, seek]);
  return { playing, toggle, manualSeek, pause };
}
