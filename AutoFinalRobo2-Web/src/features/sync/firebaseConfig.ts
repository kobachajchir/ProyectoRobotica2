/**
 * AFR2-20261117-05 — Configuración pública de la app web de Firebase (proyecto `autofinalrobo2`).
 *
 * No es una credencial: Firebase la entrega a cada navegador que abre la app y solo identifica el proyecto.
 * La protección de los datos está en `firebase/firestore.rules` (roles escritor/lector) y en la clave de
 * escritura que guarda el ESP, que nunca está en el código fuente.
 * Obtenida con `firebase apps:sdkconfig WEB 1:470235617274:web:747343f836591ef63ff643`.
 */
export const FIREBASE_WEB_CONFIG = {
  projectId: 'autofinalrobo2',
  apiKey: 'AIzaSyCHzi7y8LxrveuaxNFhJ_Su4B251DQTBcg',
  authDomain: 'autofinalrobo2.firebaseapp.com',
  appId: '1:470235617274:web:747343f836591ef63ff643',
} as const;
