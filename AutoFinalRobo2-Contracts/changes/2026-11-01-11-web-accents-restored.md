# AFR2-20261101-11 — Tildes recuperadas en la Web

01/11/2026. Decisión humana: «Recuperar tildes», pedida al cerrar la fase 4 del
[plan visual](../../AutoFinalRobo2-Web/docs/plan-visual-20261101/PLAN.md).
**Sustituye** AFR2-20261026-03 («Textos de la Web sin tildes»), que las había
quitado porque se mostraban mal. La causa probable, archivos con codificación
dañada, se corrigió en AFR2-20261101-05 (archivos UTF-8); que hoy se vean bien no
está comprobado en navegador.

## Método

1. **Líneas sin cambios desde la conversión:** se restauraron exactas desde la
   versión original del commit `d7d1e8c` (334 líneas), comparando cada línea
   actual con la versión sin tildes de la original.
2. **Líneas editadas después:** se cambiaron palabras de un vocabulario de 268
   pares sacados del mismo diff, más 17 sin ambigüedad en texto visible
   («Línea», «conexión», «menú», «Calibración»…) y 103 palabras escritas
   después de la conversión (singulares en -ción/-sión, «ángulo», «número»,
   «batería», «árbol», «catálogo»…). Solo dentro de cadenas con aspecto de
   texto (con espacio o mayúscula inicial) y texto JSX. Nunca en comentarios,
   imports, rutas, ids, parámetros de URL ni claves.
3. **Revisión manual** de lo ambiguo: «está/esta», «sí/si», imperativo voseo o
   indicativo («actualiza», «gira», «ingresa», «Prepara»), «estas/estás»,
   plurales en -iones (sin tilde) y nombres propios (Autodesk Fusion).
4. Se quitó `withoutAccents` (borraba tildes al mostrar nombres del catálogo
   OLED, etiquetas de Calibración y mensajes de error) y se borró
   `Web/src/utils/displayText.ts`, que quedó sin uso.

Respaldo previo de `src` en la carpeta temporal de la sesión; no forma parte del repo.

## Qué no cambia

- **Texto dibujado en la réplica OLED:** su fuente de píxeles 7×10 no tiene
  tildes y replica al ATmega. Quedan excluidos `features/oled/generated/`,
  `firmwareRenderer.ts`, `firmwareSensorRenderer.ts`, `reformCatalogue.ts`,
  `wifiCatalogue.ts` y `developer/uiSimulation.ts`. En `FirmwareLibraryPreview.tsx`
  solo se corrigieron a mano los mensajes HTML. Pantallas, bitmaps, firmas de
  `firmwareRenderer.sources.json`, navegación y ScreenCodes sin cambios.
- Identificadores y datos: rutas (`/documentacion`, `/simulacion`), parámetro
  `?seccion=` e ids (`linea`), campos `revision`/`version`, comandos, payloads,
  SSID y claves.
- Se conservan «solo» sin tilde (norma vigente), los imperativos de tú ya
  existentes («Selecciona un punto», «deja», «completas») y el título de la
  bienvenida «Final Robotica 2».
- Se mantiene un carácter «�» que ya estaba en `CaptureInspector.tsx` en `HEAD`.

## Comprobaciones hechas (por texto, no ejecución)

- Ninguna comparación (`===`, `case`, `includes`…) quedó contra un texto que
  ahora se produce con tilde, y ningún literal con tilde nueva se compara.
- Archivos en UTF-8, sin finales de línea mezclados (se normalizó `notify.tsx`)
  y sin imports sin usar en `src`.
- 75 archivos de `Web/src` con cambios de texto, más el borrado de
  `utils/displayText.ts`. Lista en el estado de la sesión; el diff de Git es la
  referencia.

## Productores/consumidores/wire

Sin cambios de protocolo. Los mensajes de error de `Afr2CommandError` ahora
muestran el texto tal como llega (con tildes si las trae); código y origen se
conservan.

## Estado por repo

- **atmega / esp:** sin cambios.
- **web:** fuente modificada. Sin tests, compilación, typecheck, validadores,
  navegador ni `build:esp`. **No verificado** que las tildes se vean bien en
  navegador ni en el visor; esa era la causa del pedido de 26-10.
- **shared/Contracts:** esta ficha; AFR2-20261026-03 queda sustituida.
- Sin commit local ni remoto.

## Pendiente

- Comprobar en navegador (claro/oscuro) que «configuración», «batería» y los
  títulos se ven bien; si vuelven a verse mal, revisar codificación del paquete
  servido por el ESP antes de volver a quitarlas.
- Descripciones del plan en voseo para las páginas nuevas: fase 5.
