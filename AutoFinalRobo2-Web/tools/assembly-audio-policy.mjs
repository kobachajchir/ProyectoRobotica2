/** The visual assembly has no narration; reject stale players before promotion. */
export const isAssemblyNarrationAsset = route =>
  /^\/?audio\/armado-narracion\.(?:opus|m4a)(?:\.gz)?$/i.test(String(route).replaceAll('\\', '/'));

export function requireSilentAssemblyHtml(html) {
  if (html.includes('/audio/armado-narracion.')) {
    throw new Error('El indice todavia contiene el reproductor con narracion. Volve a compilar la Web; para el ESP usa npm run build:esp.');
  }
}
