---
name: afr2-screen-authoring
description: Crea y adapta pantallas OLED, renderers, wrappers y navegación del encoder en AutoFinalRobo2, conservando separación de estado/dibujo/I2C Manager y documentación del contrato remoto.
---

# Pantallas AutoFinalRobo2

Usar para añadir o portar una pantalla, menú, diálogo, aviso, recurso gráfico o acción de UI local de este proyecto. La biblioteca fuente F4 está en el repo hermano AutoFinalRobo2-Atmega/reference/f4/Core; el destino es AutoFinalRobo2-Atmega/src y sus headers. No editar ni registrar cambios en D:/AutoProyecto.

## Antes de editar

Leer [arquitectura y navegación](references/architecture.md), `documentation/SCREEN_SYSTEM.md`, `documentation/NAVIGATION.md` y `documentation/screens.json` desde la raíz de Contracts. Localizar el renderer original y sus wrappers/contexto hasta conocer datos, variante y acciones; no deducir comportamiento solo del nombre o ID. Para reportes Web consultar el contrato propio, no asumir payloads históricos.

## Crear o adaptar

1. Definir identidad estable, rol (vista/acción/invocador), padre, selección, variante y campos con unidad/validez. Reusar IDs existentes; reservar uno nuevo en screens.json antes de copiar macros.
2. Separar snapshot de datos, wrapper de navegación/acciones y renderer puro. Renderers no leen registros/sensores, no esperan y no envían I2C ni comandos. Las acciones pasan por servicios autorizados y muestran resultado real.
3. Usar la API Afr2Oled y sus recursos flash. Si falta una primitiva, agregarla al núcleo con clipping y semántica explícita; no introducir HAL/DMA ni una segunda implementación TWI. Consultar memoria de framebuffer/snapshot/stack y PROGMEM. Preservar autorías/hash de recursos copiados.
4. Giro selecciona/edita; corto confirma; largo SW abre menú y su liberación no produce corto. No existe botón menú. Atrás SOLO por ítem Volver del submenú. En edición definir borrador/guardar/descartar; largo no guarda ni ejecuta acciones.
5. Documentar qué conoce la pantalla: unknown/stale no equivale a cero. No usar MPU/balance ni datos simulados para completar vistas. Nuevas pantallas GOTO requieren requisitos reales de odometría.
6. Actualizar screens.json, informes de módulo, overlays/connections.json y hallazgos afectados. Si cambia reporte/action wire, crear ficha AFR2 y marcar consumidores pendientes. Para esquema usar [plantilla de ficha](assets/screen-record.json).

## Cierre

Indicar archivos, decisiones visuales/navegación, presupuesto, origen y pendientes. No afirmar paridad F4/Web ni funcionamiento físico por lectura de fuente. No ejecutar tests, compilaciones, validadores ni flasheos sin pedido expreso del usuario; esa restricción del proyecto también se aplica a la skill.

La base actual es parcial: falta backend TWI y composición AVR. Se puede completar una pantalla en fuente sin afirmar que funciona en placa. No activar hardware ni publicar por implicación.
