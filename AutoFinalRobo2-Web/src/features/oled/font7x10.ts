/** Literal font bytes copied at build from AVR font7x10.c.
 * Original F4 corpus/author attribution preserved in Contracts provenance. */
import { FIRMWARE_LIBRARY } from './generated/firmwareLibrary';
export const FONT_7X10_BYTES = FIRMWARE_LIBRARY.font;
export function afr2Font7x10Row(character: number, row: number): number {
  if (row >= 10) return 0;
  if (character < 32 || character > 126) character = 63;
  return FONT_7X10_BYTES[(character - 32) * 10 + row];
}
