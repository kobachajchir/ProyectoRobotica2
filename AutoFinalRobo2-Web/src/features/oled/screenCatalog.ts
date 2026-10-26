/** AFR2-20261024-09. Navigation and notification identities from Contracts.
 * Metadata does not imply a local renderer or physical/runtime verification. */
import { screens } from '../../../../AutoFinalRobo2-Contracts/documentation/screens.json';
import { withoutAccents } from '../../utils/displayText';

export type Afr2ScreenCode = `0x${string}`;
export interface Afr2ScreenMetadata {
  id: string;
  code: Afr2ScreenCode;
  name: string;
  role: string;
  renderer: string;
  parent: Afr2ScreenCode | null;
  returnsToContext: boolean;
  actions: readonly string[];
  localCredentials: boolean;
  itemCount: number | null;
  visibleRows: number;
  siblings: readonly Afr2ScreenCode[];
  navigationMode: string;
}

export function normalizeScreenCode(value: unknown): Afr2ScreenCode | null {
  if (typeof value !== 'string' || !/^0x[\da-f]{1,8}$/i.test(value)) return null;
  return `0x${Number.parseInt(value.slice(2), 16).toString(16).padStart(6, '0')}`;
}

export const AFR2_SCREEN_META = Object.fromEntries(screens.map(screen => {
  const code = normalizeScreenCode(screen.local_code)!;
  const parent = screen.parent?.startsWith('shared:screen/')
    ? normalizeScreenCode(`0x${screen.parent.slice('shared:screen/'.length)}`) : null;
  const metadata: Afr2ScreenMetadata = {
    id: screen.id, code, name: withoutAccents(screen.name), role: screen.role, renderer: screen.renderer,
    parent, returnsToContext: screen.parent === 'return-to-current-context',
    actions: screen.actions.map(withoutAccents),
    localCredentials: code === '0x03002e' || code === '0x040004',
    itemCount: code === '0x030021' ? null : 'item_count' in screen && typeof screen.item_count === 'number' ? screen.item_count : 0,
    visibleRows: screen.role === 'menu' ? 3 : 1,
    siblings: 'siblings' in screen && Array.isArray(screen.siblings) ? screen.siblings.map(value=>normalizeScreenCode(value)).filter((code):code is Afr2ScreenCode=>code!==null) : [],
    navigationMode: 'navigation_mode' in screen && typeof screen.navigation_mode==='string' ? screen.navigation_mode : 'menu',
  };
  return [code, metadata];
})) as Record<Afr2ScreenCode, Afr2ScreenMetadata>;

export function getScreenMetadata(code: string): Afr2ScreenMetadata | undefined {
  const normalized = normalizeScreenCode(code);
  return normalized ? AFR2_SCREEN_META[normalized] : undefined;
}
