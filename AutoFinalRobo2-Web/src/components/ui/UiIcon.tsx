import type { SVGProps } from 'react';

export type UiIconName = 'home' | 'control' | 'sensors' | 'wifi' | 'screen' | 'cube' | 'book' | 'panel' | 'back' | 'settings' | 'close' | 'arrow' | 'radar' | 'servo' | 'wheel' | 'antenna';
const paths: Record<UiIconName, string> = {
  radar: 'M2 20a10 10 0 0 1 20 0H2Zm4 0a6 6 0 0 1 12 0M12 20l-5-9M12 20v-2',
  servo: 'M5 12h14v8H5v-8Zm7 0V6m-6 0h12M4 15H2m20 0h-2',
  wheel: 'M12 3a9 9 0 1 0 0 18 9 9 0 0 0 0-18Zm0 5v8m-4-4h8M8 5l-2 2m12 10-2 2',
  antenna: 'M12 9v12m-5 0h10M8 6a6 6 0 0 0 0 8m8-8a6 6 0 0 1 0 8M5 3a10 10 0 0 0 0 14M19 3a10 10 0 0 1 0 14M12 7h.01',
  home: 'm3 10 9-7 9 7M5 9v12h5v-7h4v7h5V9',
  control: 'M8 7h8c3 0 4 2 5 9 .5 4-2 5-4 2l-2-2H9l-2 2c-2 3-4.5 2-4-2C4 9 5 7 8 7ZM7 10v5M4.5 12.5h5M16 11h.01M18 14h.01',
  sensors: 'M3 12h4l3-8 4 16 3-8h4',
  wifi: 'M3 8a15 15 0 0 1 18 0M6 12a10 10 0 0 1 12 0M9 16a5 5 0 0 1 6 0M12 20h.01',
  screen: 'M4 3h16a1 1 0 0 1 1 1v12a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1ZM8 21h8M12 17v4',
  cube: 'm12 2 9 5v10l-9 5-9-5V7l9-5Zm0 10 9-5M12 12 3 7m9 5v10M7.5 4.5l9 5',
  book: 'M12 5c-3-2-6-2-9-1v16c3-1 6-1 9 1 3-2 6-2 9-1V4c-3-1-6-1-9 1Zm0 0v16',
  panel: 'M4 3h16a1 1 0 0 1 1 1v16a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1ZM9 3v18',
  back: 'm14 6-6 6 6 6M8 12h12',
  settings: 'M4 7h16M4 17h16M8 4v6M16 14v6',
  close: 'm6 6 12 12M18 6 6 18',
  arrow: 'M5 12h14m-5-5 5 5-5 5',
};

/** Small, consistent outline family; no icon package required by the ESP bundle. */
export function UiIcon({ name, ...props }: SVGProps<SVGSVGElement> & { name: UiIconName }) {
  return <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.7"
    strokeLinecap="round" strokeLinejoin="round" aria-hidden="true" {...props}><path d={paths[name]} /></svg>;
}
