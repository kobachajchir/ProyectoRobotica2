---
name: afr2-screen-authoring
description: Crea y adapta juntas las pantallas OLED del ATmega y su copia Web en AutoFinalRobo2, con ScreenCodes compartidos, renderers, recursos, wrappers y navegación del encoder.
---

# Pantallas AutoFinalRobo2

Usar para añadir o portar una pantalla, menú, diálogo, aviso, recurso gráfico o acción de UI local de este proyecto. La biblioteca fuente F4 está en el repo hermano AutoFinalRobo2-Atmega/reference/f4/Core. Los destinos vinculados son AutoFinalRobo2-Atmega/src y AutoFinalRobo2-Web/src/features/oled. No editar ni registrar cambios en D:/AutoProyecto.

## Antes de editar

Leer [arquitectura y navegación](references/architecture.md), `documentation/SCREEN_SYSTEM.md`, `documentation/NAVIGATION.md` y `documentation/screens.json` desde la raíz de Contracts. Localizar el renderer original y sus wrappers/contexto hasta conocer datos, variante y acciones; no deducir comportamiento solo del nombre o ID. Para reportes Web consultar el contrato propio, no asumir payloads históricos.

## Crear o adaptar

1. Definir identidad estable, rol (vista/acción/invocador), padre, selección, variante y campos con unidad/validez. Reusar IDs existentes; reservar uno nuevo en screens.json antes de copiar macros.
2. Separar snapshot de datos, wrapper de navegación/acciones y renderer puro. Renderers no leen registros/sensores, no esperan y no envían I2C ni comandos. Las acciones pasan por servicios autorizados y muestran resultado real.
3. Usar la API Afr2Oled y sus recursos flash. Si falta una primitiva, agregarla al núcleo con clipping y semántica explícita; no introducir HAL/DMA ni una segunda implementación TWI. Consultar memoria de framebuffer/snapshot/stack y PROGMEM. Preservar autorías/hash de recursos copiados.
4. Giro selecciona/edita; corto confirma; largo SW abre menú y su liberación no produce corto. No existe botón menú. Atrás SOLO por ítem Volver del submenú. En edición definir borrador/guardar/descartar; largo no guarda ni ejecuta acciones.
5. Documentar qué conoce la pantalla: unknown/stale no equivale a cero. No usar MPU/balance ni datos simulados para completar vistas. Nuevas pantallas GOTO requieren requisitos reales de odometría.
6. Actualizar screens.json, informes de módulo, overlays/connections.json y hallazgos afectados. Si cambia reporte/action wire, crear ficha AFR2 y marcar consumidores pendientes. Para esquema usar [plantilla de ficha](assets/screen-record.json).

## ATmega y Web siempre vinculados

Por pedido explícito del usuario, cada pantalla creada o modificada debe entregarse también en la biblioteca Web dentro de la misma tarea. No dejar su contraparte para otro agente o una tarea futura. Aplicar la misma regla a menús, avisos, variantes, iconos, fuentes, clipping y scroll.

- Mantener una identidad en documentation/screens.json, consumida por Web/src/features/oled/screenCatalog.ts. Conservar ScreenCode, nombre, rol, padre/retorno, variante, datos y acciones; no crear listas Web independientes de cuatro pantallas.
- Copiar los recursos del AVR mediante Web/tools/sync-oled-library.mjs. Adaptar también firmwareRenderer.ts cuando cambie cualquier algoritmo C de dibujo y revisar firmwareRenderer.sources.json solo después de portar el comportamiento. Nunca actualizar hashes únicamente para hacer pasar un build.
- Leer [vinculación y fidelidad](references/web-linking.md) antes de cambiar renderers. Mismos bytes de fuente/iconos y mismas coordenadas, bordes, inversión, clipping, selección, firstVisible y tiempos; no sustituir el diseño por HTML/SVG aproximado.
- El visor conectado muestra el framebuffer AVR completo de 1024 bytes. La copia local sirve a la biblioteca/preview identificado y recibe el mismo snapshot preparado; no inventar telemetría ni predecir acciones. Si falta captura, mostrar indisponibilidad. Mantener protegidas las pantallas con credenciales.
- Revisar el consumidor ESP y los permisos si cambian datos/wire. Registrar en una ficha AFR2 el estado separado de ATmega/ESP/Web y la procedencia por archivo. Incluir la copia Web en el paquete ESP cuando esté autorizada la compilación/promoción; nunca confundir copia local con flasheo.

## Cierre

Indicar archivos ATmega y Web vinculados, ScreenCode compartido, recursos, decisiones visuales/navegación, presupuesto, origen y pendientes. No cerrar una pantalla nueva como completa si falta su copia Web. No afirmar paridad F4/Web ni funcionamiento físico por lectura de fuente. No ejecutar tests, compilaciones, validadores ni flasheos sin pedido expreso del usuario; esa restricción del proyecto también se aplica a la skill.

Backend TWI, I2C Manager y composición AVR existen; su presencia y los builds no acreditan funcionamiento físico. Se puede completar una pantalla y su copia Web en fuente sin ejecutar compilación si no está autorizada. No activar hardware ni publicar por implicación.
