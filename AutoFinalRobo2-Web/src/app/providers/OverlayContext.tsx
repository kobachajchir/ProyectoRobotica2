import { createContext, useContext } from 'react';
import type { ModalOrigin } from '../../components/ui/FloatingModal';

export const OverlayContext = createContext<{ openOled: (origin: ModalOrigin) => void; modalOpen: boolean; oledOpen: boolean } | null>(null);
export function useOverlays() {
  const value = useContext(OverlayContext);
  if (!value) throw new Error('useOverlays requiere AppShell');
  return value;
}
