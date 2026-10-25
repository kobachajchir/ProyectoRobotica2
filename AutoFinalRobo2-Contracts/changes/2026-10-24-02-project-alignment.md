# AFR2-20261024-02 — Alineación Atmega / ESP / Web y compilaciones

Pedido humano: revisar documentación y avances de los tres módulos, completar
comandos/productores/consumidores faltantes y compilar corrigiendo errores.
Trabajo sobre los avances locales existentes, sin revertirlos ni publicar.

## Diferencias encontradas y resueltas

- GOTO Web enviaba ángulo/velocidad que ESP rechazaba aunque anunciaba capacidad.
  EX01 v3 integra los cuatro campos y publica límites reales del AVR.
- Calibración Web enviaba un objeto genérico sin handler ESP. Ahora usa los IDs,
  unidades y rangos del catálogo compartido; lectura, RAM, confirmación de línea
  y guardado/descartado EEPROM tienen acciones y respuestas diferenciadas.
- OLED tenía outbox Wi-Fi sin consumidor ESP/modal Web. Se agrega adaptador
  asíncrono con estado, redes, borrador efímero, confirmación física, resultado y
  versiones. Wi-Fi directo sigue disponible sin Mega. Una única sesión WS.
- Consulta Hall mostraba toast sin actualizar contadores. Se conecta al estado
  compartido y se corrige caducidad de sensores/pérdida de enlace.
- Capturas OLED temporalmente ocupadas podían bloquear el polling indefinidamente.
  Se reporta BUSY reintentable. Corregido el error TypeScript al nombrar IDs nuevos.
- Cancelación durante inicio de secuencias carecía de identidad estable hasta ACK.
  Se reserva antes del envío; STOP/revocación descartan respuestas anteriores.
- Se corrigen advertencias C/C++, semántica STOP en fault, límites angulares de
  GOTO, borrado/caducidad de borradores y cancelación al perder foco/salir de Control.
- Largo SW vuelve a abrir principal incluso desde teclado sin enviar datos;
  se preservan el árbol completo y Volver por opción.
- Web reconstruida y promovida a ESP/data/dist, evitando servir la revisión antigua.
  Promoción conserva manifest anterior hasta completar copia y acota rutas/archivos.

[Matriz/offsets/contrato vigente](../contracts/EX01_ALIGNMENT_3.md).

## Estado real

| Módulo | Resultado | Límite |
|---|---|---|
| atmega | firmware, communication-firmware y library compilados; ELF/HEX/mapa | Sin ejecución física; Hall macro0 y calibración pendientes |
| esp | firmware compilado con esp12s_candidate | No identifica flash/SPI/layout real ni margen heap bajo carga |
| web | TypeScript/Vite/postbuild compilados; paquete promovido a ESP | Sin ejecución/revisión visual de navegador en esta tarea |
| shared | catálogo, capacidades, contrato y documentación sincronizados | Compilar no prueba interoperabilidad eléctrica ni funcionamiento del auto |

## Evidencia y memoria

Logs en [documentación de builds](../documentation/builds/2026-10-24-02/README.md).
La primera compilación detectó TS7053 en LiveOledViewer y advertencias de
indentación C/C++; se corrigieron. El inicializador naked de reset no admite
análisis .su de GCC; solo esa unidad se compila sin ese flag, conservando avisos
Wall/Wextra. Se conserva análisis .su del resto y mapa de enlace, sin estimarlo
como cota completa de pila de avr-libc/libm/ISR o como prueba de hardware.

La revisión automática rechazó activar AFR2_SEQUENCE_MEMORY_APPROVED=1 porque
no encontró aprobación específica para memoria/pila y calibración. Se conservó
el define0, se completó el resto y se dejó documentado el bloqueo. El editor y
simulador permanecen; la capacidad física no se anuncia. No se intentó eludir
el rechazo ni se cambió AFR2_HALL_CONNECTED.

No se ejecutaron tests, validadores independientes, servidor/navegador, puertos
serie, imagen FS, flasheos ni pruebas físicas. No se cambiaron CAD, cableado,
fuses, credenciales ni repositorios de D:/AutoProyecto. No commit/push de los
avances concurrentes por esta tarea. Fuentes y adaptaciones por archivo en
[procedencia](../provenance/project-alignment-20261024.json).
