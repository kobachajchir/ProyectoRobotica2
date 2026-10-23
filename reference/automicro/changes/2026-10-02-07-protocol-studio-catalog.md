# AMI-20261002-07 — Catálogo completo asignado en Protocol Studio

- Fecha: 2026-10-02 (America/Buenos_Aires)
- Origen: Web ESP01, cambios locales sin publicar el repositorio Web.
- Estado del origen: implementado; compilación Web completada y gzip copiado automáticamente a ESP.
- Área: constructor/traductor UNER y documentación del catálogo.

## Contrato anterior y nuevo

Studio tenía una lista manual incompleta; 51 estaba etiquetado GET_CURRENT_SCREEN, 52 como REQUEST_SCREEN_PAGE, 19 figuraba sin handler y hex4 emitía solo un byte. Ahora selector y traductor consumen un manifiesto único con 106 IDs nombrados: 95 asignaciones operativas observadas y 11 declaradas/rechazadas/internas para lectura. El archivo no incorpora IDs RESERVED anónimos como operaciones válidas.

Los IDs wire de firmware no cambian. Se corrige la interpretación Web: 51 autentica, 52 consulta pantalla, 56 cambia página, 57/58 giran encoder, 76 reinicia en perfil. u32/u16/f32 se codifican LE; PIN son cuatro valores numéricos 0..9. Los formularios PID/AP/auth/credenciales/Canvas seleccionan campos por máscara/scope/operación. Se rechazan números truncados, bytes hex inválidos y payloads mayores de 255 bytes. Los códigos de pantalla ahora ocupan cuatro bytes reales.

Catálogo y esquemas: [inventario legible](../docs/PROTOCOL_STUDIO_COMMANDS.md), [fotografía machine-readable](../catalog/protocol-studio.json). En Web: src/features/protocolStudio/commandManifest.json es la fuente runtime; docs/PROTOCOL_STUDIO_COMMANDS.md contiene las asignaciones. Los codecs de snapshots reutilizan src/protocol/f4Payloads.ts. El constructor no habilita comandos declarados como si fueran operativos.

## Flujo completo

1. Studio permite construir offline una solicitud/evento y leer frames UNER; no cambia endpoints JSON ni envía automáticamente comandos al firmware.
2. UNER v2 conserva cabecera, ruta, ID, longitud y XOR. La ruta distingue solicitudes/respuestas; los formularios representan request/evento y las respuestas se ingresan como bytes manuales con ruta invertida.
3. Se muestran política de finalización, timeout de registro, destino y permiso; ACK_THEN_SAME_COMMAND requiere respuesta final por mismo ID. ACK/NACK llevan original_cmd y status/reason opcional.
4. Los snapshots MPU/IR/telemetría/control/boot, autenticación, streams, PID, perfiles, emisores y reportes de pantalla se interpretan con tamaños/formatos separados de los requests. GOTO es modo 3 sin objetivo.
5. Los controles de teclado siguen siendo eventos físicos: girar puede cruzar fila/hacer wrap según la pantalla; 96 no informa cursor ni buffer. Referencia: docs/SCREEN_BEHAVIOR.md.
6. Los comandos internos y los permisos publicados no habilitan uso Web público; ESP sigue asignando el nodo de sesión. No guardar PIN/contraseñas reales en ejemplos.

## Acciones y pendientes

- [x] Incorporar todos los IDs nombrados del registro inspeccionado al traductor.
- [x] Generar selector desde el mismo manifiesto; 6F deja de quedar oculto.
- [x] Corregir IDs anteriores, serialización LE y formularios nuevos.
- [x] Documentar cobertura form/raw, disponibilidad, formatos, fuentes y excepciones.
- [x] Añadir npm run protocol:inventory en Web para futuras revisiones optativas de asignación de IDs/nombres.
- [x] Compilación final Web (TypeScript + Vite) y copia automática de data/dist/index.html.gz, 1.762.099 bytes.
- [ ] Formularios dedicados para payloads variables con coverage=raw (IDs ya asignados).
- [ ] Armonizar metadata/tabla/handlers ESP: 43/44/45/46/4D están operativos en handlers aunque el registro los marca no implementados; 43 solo repite el byte y no demuestra cambio del encoder F4. 47 rechaza siempre; productor 5A no confirmado.
- [ ] Corregir orden 75/76 en registro ESP que utiliza búsqueda binaria. Registrado como defecto de firmware pendiente; no se modificó firmware en esta entrega.
- [ ] Prueba de interfaz y extremo a extremo con placas.

## Impacto en repositorios

| Repositorio | Estado | Acción o motivo |
| --- | --- | --- |
| FirmwareF4 | no_aplica | Sin cambios wire/firmware; fuente de schemas y enums inspeccionada. |
| Firmware ESP01 | pendiente | Inconsistencias de metadata y orden 75/76 documentadas; solamente data/dist/index.html.gz se actualiza por postbuild Web. |
| Web ESP01 | origen | Manifiesto, constructor, traductor, inventario y herramienta optativa implementados localmente. |
| AutoMicroQtDesktop | no_aplica | Esta entrega afecta la herramienta Studio Web; no cambia IDs ni schemas del productor. |

## Evidencia y límites

Inspección de ESP src/f4_command_registry.c, src/uner_cmdspec.c y src/uner_app_handlers.cpp; F4 Core/Src/uner_app.c, Core/Src/oled_canvas.c, Core/Inc/app_preferences.h y referencia wire F4. No se presume disponibilidad física ni paridad de todos los metadatos. La herramienta de inventario compara IDs/nombres, no valida automáticamente todos los formatos. Tests y prueba física no ejecutados. Compilación final npm run build completada tras los últimos ajustes. Postbuild copió solo index.html.gz (1.762.099 bytes), conservó modelos/Draco y no flasheó. Advertencia no bloqueante de Browserslist desactualizado. La herramienta optativa de inventario no fue ejecutada.

## Orden de integración

Recompilar Web genera gzip y lo copia a data/dist mediante el postbuild existente. Cargar filesystem del ESP para utilizar la Web nueva en placa. No se requiere cambio de firmware por el catálogo; corregir pendientes ESP en una entrega separada con su ficha. Los repositorios de código no se publican en esta entrega.
