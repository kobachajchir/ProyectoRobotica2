# AFR2-20261117-15 - Programas → Seguidor de línea: vista primero y «Vista completa» como Control táctil

Autoría: Koba Chajchir / AFR2.
Versión: 1.39.3 (subversión mínima).

Estados:
- web = en fuente;
- atmega = solo versión;
- esp = solo versión.

Sin cambio de contrato.

## Pedido

- En Programas → Seguidor de línea, la vista del auto sobre la cinta va primero.
- Debajo van las tarjetas «Seguidor de línea» y «Ajuste y calibración», plegadas al entrar.
- El enlace «Abrir la vista completa del seguidor» pasa a ser un botón con la misma forma y ubicación que «Control táctil» en Conducir.

## Cambios (Web)

- `src/features/control/LineFollowerPanel.tsx`:
  - **Encabezado:** `afr2-manual-heading`, el mismo de «Conducción manual», con el título «Seguidor de línea» y, a la derecha, el botón primario «Vista completa» (icono de sensores). El botón abre el visor de seguimiento inmersivo (`LineFollowerOverlay`, AFR2-20261117-12), igual que «Control táctil» abre la conducción táctil.
  - **Orden:** encabezado, vista en vivo, tarjeta del seguidor y tarjeta de ajuste y calibración. Las dos tarjetas con `defaultOpen={false}`.
  - Se quita el enlace a `/seguidor-linea` debajo de la vista.
  - En la sección propia (`showView=false`) no cambia nada: esa página ya tiene su vista grande y su botón del visor.

## Estado de validación

Revisado en `npm run dev` (5173), en Programas → Seguidor de línea:
- el orden es encabezado, vista y dos tarjetas plegadas;
- «Vista completa» abre el visor;
- sin errores en consola.

Sin compilar.
