# AFR2-20261108-07 - Pantalla de inicio con la versión y política de versiones

Pedido del usuario: una animación de inicio durante los primeros 3 s que diga «AFR2 ATmega» y, debajo, la versión del
firmware, con la misma barra superior de tiempo de los avisos. Desde ahora cada cambio sube la versión, separando versión,
subversión y subversión mínima, con todos los módulos en la versión 1. Solo fuente: sin compilar, flashear ni probar.

Versión: 1.2.0 (subversión: pantalla nueva compatible).

## Pantalla de inicio (0x010004)

- Aparece al encender, antes del Inicio (0x010001). Dura 3 s (`AFR2_SPLASH_MS`) desde el primer `Afr2Ui_Tick`.
- Barra superior de 2 px, igual que la de los avisos, que se llena de izquierda a derecha en 3 s (repinta cada 100 ms).
- «AFR2» al doble de tamaño (fuente 7x10 escalada a 14x20 por letra, x=36, y=8) que aparece letra por letra cada 200 ms.
- «ATmega» (y=32) y «Firmware 1.2.0» (y=47) centrados. La versión es `AFR2_FIRMWARE_VERSION`, la compilada en el HEX.
- Corto o largo la saltean; el giro no hace nada. Los avisos que llegan durante ella (por ejemplo «ESP iniciada») esperan
  y arrancan su tiempo al terminar.
- No es un nodo del catálogo: es un estado de `Afr2Ui` (como Táctil 0x030035) que informa 0x010004 a la Web.
- Web: `renderSplash` en `firmwareRenderer.ts` (mismos píxeles), versión leída de `afr2_build_info.h` con `?raw`
  (`firmwareVersion.ts`). La biblioteca la muestra en bucle; también muestra «Sin red Wi-Fi» 0x03004B con sus dos filas.

## Política de versiones

`docs/VERSIONADO.md` y una sección en el AGENTS.md raíz. Una sola versión de producto `MAYOR.MENOR.PARCHE` para ATmega,
ESP y Web, que se sube junta en cada cambio de código cargable:

- Versión (MAYOR): rompe la compatibilidad (protocolo/EEPROM sin migración) o etapa nueva decidida por el usuario.
- Subversión (MENOR): función nueva compatible (pantalla, menú, preferencia, acción, comando, evento).
- Subversión mínima (PARCHE): corrección o ajuste sin función nueva.
- Documentación, CAD y Loader no la suben. El Loader mantiene su propia línea (2.0.0) y no se bajó a 1.

Historial: 1.0.0 (AFR2-20261108-02..05), 1.1.0 (AFR2-20261108-06), 1.2.0 (este cambio).

## Archivos

- Contracts: `contracts/afr2_build_info.h` (1.2.0), `docs/VERSIONADO.md`, `documentation/screens.json` (0x010004), CHANGELOG;
  `AGENTS.md` raíz (sección Versiones).
- ATmega: `afr2_screens.h` (`AFR2_SCREEN_SPLASH`, `AFR2_SPLASH_MS`, `splash`, `splash_ms`), `afr2_screens.c`
  (`render_splash`, `big_text_flash`, Tick/Apply/ReportedScreen).
- ESP: solo la versión compartida.
- Web: `package.json` y `package-lock.json` (1.2.0), `features/oled/firmwareRenderer.ts`, `firmwareVersion.ts` (nuevo),
  `FirmwareLibraryPreview.tsx`, `firmwareRenderer.sources.json` (hashes de `afr2_screens.c/.h`).

## Estado y límites

atmega=fuente editada, sin compilar ni cargar; esp=solo versión, sin compilar; web=fuente editada, sin build ni navegador.
+5 bytes de RAM en `Afr2Ui`; flash sin medir. Los 3 s se cuentan desde el arranque del programa, no desde que el panel
termina de encender: si la OLED tarda en inicializarse se verá algo menos. Sin prueba física.
