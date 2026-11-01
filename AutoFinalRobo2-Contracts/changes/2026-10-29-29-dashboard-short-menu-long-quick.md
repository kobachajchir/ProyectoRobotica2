# AFR2-20261029-29 — Dashboard: corto menu, largo pantallas rapidas

Pedido humano: separar los accesos que28 habia unificado. Corto desde010001 abre010002 (menu completo); largo >=800ms al soltar abre010003 (carrusel). No cambia el resto de la navegacion ni el dialogo obligatorio. Retorno por historial: menu vuelve al dashboard si se abrio con corto, o al carrusel si se abrio desde su tarjeta Configuracion. Fallback de menu vuelve al dashboard.

Fuentes: ATmega afr2_screens.c y afr2_menu_catalog.c; Web firmwareRenderer.ts, FirmwareLibraryPreview.tsx y EncoderControl.tsx. Catalogo compartido, guias e instrucciones actualizados. Procedencia AFR2 propia; autorias previas conservadas. Identidades y EX01 sin cambios; versiones0.11.0/Web0.4.0 no desplegadas.

Estado ATmega/Web: solo fuente y recursos copiados en modo copy-only. ESP no requiere adaptacion. Sin compilacion, validadores, pruebas, promocion ni flasheo; no se abrieron puertos.
