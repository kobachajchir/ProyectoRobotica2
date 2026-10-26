/** Presentation text only: preserve identifiers, user data and the letter n with tilde. */
export function withoutAccents(text: string): string {
  return text.normalize('NFD').replace(/([aeiouAEIOU])\u0301/g, '$1').normalize('NFC');
}
