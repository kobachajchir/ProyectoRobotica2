> AFR2-20261024-15: el estado normativo de implementación es [ALINEACION_MAESTRO](ALINEACION_MAESTRO.md). ROBO2/perfil2 EX01v6, menú y permisos/gates conservados. Se completa adquisición de control en configuración y diagnóstico local FS/paquete; decisiones inferiores mantienen su contexto histórico.

# Decisiones

## Firmware ESP y perfil UART / AFR2-20261023-11

- ADR-024: [perfil AFR2 1](../contracts/UART_PROFILE_1.md) sobre UNER v2, con sesión/transacción en payload, N<=64 y seis IDs propios. Estado: implementado en fuente ESP/AVR, sin compilar ni probar. Concreta ADR-007 sin asumir compatibilidad histórica ni habilitar movimiento.
- ADR-025: LittleFS con autoformat=false, assets exactos por manifiesto y preferencias privadas temporales+rename; sin upload/editor HTTP. [Contrato HTTP/WS/Wi-Fi](../contracts/PROTOCOL.md). Layout físico y capacidad real pendientes de identificar; no se adopta linker ESP01.
- ADR-026: composición AVR de comunicaciones separada de la aplicación completa. Informa UART y datos unknown; no confirma STOP sin callback autoritativo ni concede lease. Endpoint ESP usa estados/capacidades actuales y la Web fuente los consume por latidos/eventos. Motores/GOTO pendientes.

| ID | Decisión | Estado |
| --- | --- | --- |
| ADR-001 | Cuatro repos independientes | Sustituida por ADR-018 el 2026-10-23; historial conservado |
| ADR-002 | AVR C, ESP Arduino/LittleFS, React/Vite; sin AT/DMA/MPU | Acordado |
| ADR-003 | I2C Manager, solo OLED, servicio cooperativo | Acordado; backend pendiente |
| ADR-004 | Encoder UI; corto confirma, largo abre menú, atrás por ítem | Acordado; no botón menú |
| ADR-005 | UI D40/D41/D42, OLED D20/D21 | Propuesto; comprobar PCB real |
| ADR-006 | GOTO relativo por cuatro encoders de rueda | Diseño; sin orientación absoluta |
| ADR-007 | UNER v2 candidato | IDs/payload/capabilities por cerrar |
| ADR-008 | UART propia 115200 8N1 candidata | Independiente de AT de fábrica y de upload; error AVR/robustez pendientes |
| ADR-009 | ESP-12S nominal 32 Mbit / 4 MiB según ficha V1.1 | Chip/flash real, modo SPI y layout LittleFS por identificar |
| ADR-010 | Un responsable del control con lease; espectadores separados | Propuesto; STOP prioritario |

No hay .ino completo del proveedor entre los ocho adjuntos. El PDF conceptual muestra fragmentos con dos IR D52/D53, distintos del módulo de cinco canales A4..A0. No reconstruir un sketch y llamarlo original. Las URLs del manual son referencias, no instrucciones para instalar software ni firmware AT.

## Ampliación 2026-10-23

- ADR-011: el usuario confirma SDA/SCL expuestos en el shield. Usar las cabeceras con esos rótulos; D20/D21 conservan equivalencia lógica.
- ADR-012: nueva PCB OLED/encoder con J1 1x5 de señales y J2 1x2 VCC/GND, paso 2,54 mm, Dupont. Alimentación y masa comunes; tensión por identificar.
- ADR-013: soporte 3D y cubierta fijan conjunto con dos tornillos/tuercas. Dimensiones y tornillería pendientes de planos del usuario; CAD diferido.
- ADR-014: árbol funcional en documentation/MENU_PLAN.md como propuesta para implementar; no asigna IDs wire ni amplía firmware en este cambio.

## Evidencia del shield 2026-10-23 / AFR2-20261023-03

- ADR-015: documentación OSOYOO UART WiFi Shield V1.3 aportada. Regulación y adaptación UART integradas declaradas; recorrido por conector no probado. Selector E_RX/E_TX distinto del puerto directo ESP UART de 3,3 V.
- ADR-016: conservar UART1 Mega D18/D19 para operación; selectores D4/D5 y D0/D1 aislados. D5 pertenece al motor RR. Mapa UNO no cambia I2C Mega D20/D21.
- ADR-017: carga ESP con USB Mega exige D0→RX ESP / TX ESP→D1 e aislamiento; jumpers normales RX/TX no resuelven ese cruce. KEY/RST previstos para arranque manual, sin auto-reset supuesto.
- AT permanece excluido. `9600 kbps` del proveedor es una errata probable de unidad, no baud medido. VCC OLED/encoder y CAD siguen pendientes. [Informe y fuentes](../hardware/WIFI_SHIELD.md).

## Organización CAD y Git / AFR2-20261023-04

- ADR-018: un único repositorio Git local en D:/ProyectoRobotica2, con cinco módulos: Atmega, ESP, Web, Contracts y CAD. Reemplaza ADR-001 por pedido del usuario. Sin remotos ni publicación; historiales previos conservados como ancestros y etiquetas, con metadatos originales resguardados dentro del nuevo .git.
- ADR-019: AutoFinalRobo2-CAD reúne esquema/PCB KiCad, pieza nueva/soporte/cubierta y ensamblaje completo del auto. Los nueve insumos existentes de CADS se trasladan sin cambiar su contenido a CAD/reference/supplied. La autoría, licencia y URL de esos modelos siguen por identificar; no acreditan geometría del robot real.
- ADR-020: versionar fuentes, manifests, lockfiles y bibliotecas CAD propias. Excluir dependencias instaladas, descargas recuperables, cachés, builds, backups automáticos y exportaciones regenerables. Conservar evidencia e insumos sin recuperación documentada. [Política](ESTRUCTURA_Y_GIT.md).
- Crear la estructura no resuelve cotas, huellas, alimentación ni tornillería. El diseño nuevo y cualquier ERC/DRC, build, test o prueba física siguen pendientes de indicaciones/autorización según las reglas del proyecto.

## Plan Fusion y origin / AFR2-20261023-05

- ADR-021: modelo futuro del auto base mediante el MCP de Fusion existente. Usar inventario/manual local de la variante Model Y, mejores CAD correspondientes y descargas Web; inferencias justificadas, explícitas y editables. Sin cables, mazos ni jumpers; conectores físicos conservados.
- ADR-022: pieza nueva/soporte/PCB y cubierta modificada pendientes. Reservar espacio; preparar referencias OLED 0,96 pulgadas y encoder mecánico KY-040 sin placa breakout, más perilla. No declarar esos componentes instalados.
- ADR-023: destino aprobado privado kobachajchir/ProyectoRobotica2, origin HTTPS y rama remota main, con los cuatro historiales/etiquetas conservados. La entrega solo guarda/publica el [plan](../../AutoFinalRobo2-CAD/docs/PLAN_MODELO_3D_FUSION.md); no inicia el modelado.


## ROBO2 y composición local / AFR2-20261023-15

- ADR-027: cabecera ASCII ROBO2 de5 bytes, tokens3A/A5, framing versión1 y
  perfil2. Mantiene semántica F4 de ruta emisor nibble alto/receptor bajo y
  validación de ambos extremos con un único header C/C++. Sustituye candidato
  ADR-007 y framing de ADR-024; sin compatibilidad con UNER/perfil1.
- ADR-028: composición AVR local UART/Timer0/TWI/Manager/OLED/encoder escrita;
  opción UART sola conservada con board común. Concreta ADR-003 y amplía ADR-026.
  Sin DMA/HAL, cuatro vistas locales, sin anuncio de servicios remotos UI ni
  sensores/potencia por implicación. Estado fuente no compilada/probada.
- ADR-029: guía de ESP docs/PROTOCOLO_ROBO2.md documenta enviar/recibir/validar y
  pendientes; contrato normativo y casos comunes en Contracts. No duplicar parser.
  Actualizar ambos firmwares juntos en una futura verificación autorizada.

## Entrega ATmega19, continuación24/10/2026

Árbol44 nodos y servicios de producto integrados en fuente, sin compilar/probar.
User override: largo SW en teclado cancela y vuelve al contexto anterior; fuera
de teclado abre principal. Conectar borrador Wi-Fi exige clic físico en auto,
también al ingresar credenciales desde Web. Único WS es complemento ESP pendiente.
UI_ACTION no concede permisos de motor/calibración/EEPROM; usar APIs dedicadas.
Hall x1 propuesto A8..A15, deshabilitado hasta conectar/confirmar niveles; CPR/signo
por una vuelta manual medida, no por11/75. Fuente/aceptación se distinguen.
EX01/perfil2 no se mezcla con perfil3/secuencias diseñado21.
[Matriz y evidencias](PLAN_ATMEGA.md) · [contrato](../contracts/ROBO2_PRODUCT_EXTENSION.md).
