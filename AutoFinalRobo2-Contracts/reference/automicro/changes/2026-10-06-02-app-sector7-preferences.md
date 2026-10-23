# AMI-20261006-02 — aplicación de 480 KiB y preferencias en sector 1

- Fecha: 2026-10-06, America/Argentina/Buenos_Aires.
- Origen: FirmwareF4, fuente local modificada.
- Estado: implementado en fuente; no recompilado, validado ni flasheado en esta entrega.
- Área: distribución Flash, manifiesto bootable y migración persistente.

## Contrato anterior y nuevo

La aplicación conserva base 0x08008000, pero su región pasa de 0x58000 bytes
(352 KiB, hasta 0x0805FFFF) a **0x78000 bytes (480 KiB, hasta 0x0807FFFF)**.
Sector 7 deja de reservarse para preferencias antiguas y puede contener código.
Bootloader conserva sector 0 (16 KiB) y preferencias sector 1 (16 KiB).

El manifiesto mantiene magic, schema v1, tamaño de 64 bytes y offsets existentes.
El valor inicial de flags cambia de bit0=1 a **flags=0** en builds normales.
El bootloader existente ya acepta imágenes hasta el máximo físico con ese bit
apagado; conserva la restricción a 352 KiB si el manifiesto solicita preservar
sector 7. El patcher actual ya contempla ambos tamaños; no se cambia su ABI.

El linker permite un build transitorio protegido añadiendo
`-Wl,--defsym=AUTOMICRO_PRESERVE_LEGACY_SECTOR7=1`: selecciona simultáneamente
352 KiB y flags=1. Sin ese símbolo, selecciona 480 KiB y flags=0.

Preferencias mantienen formato V12 y lectores V1..V11. No se renumeran campos
ni se cambia el registro V12: sigue teniendo 332 bytes y capacidad para 49
registros por sector. Todos los ajustes, PIN, MPU, PID, redes y calibraciones
blanco/pista se serializan en sector 1. No se incluyen valores sensibles reales
en esta ficha. No se modifican comandos UNER, JSON, payloads ni pantallas.

## Migración y flujo completo

1. Al iniciar, un registro válido en sector 1 tiene prioridad. Si es anterior
   a V12, se convierte y guarda con el formato vigente.
2. El adaptador STM32 comprueba identidad/tamaño del manifiesto empaquetado:
   solo permite lectura legacy si la imagen termina antes del sector 7, tiene
   tamaño válido y CRC no marcado como pendiente. La comprobación de CRC completa
   sigue siendo responsabilidad del bootloader; no se ejecuta de nuevo aquí.
3. Si sector 1 no tiene registro válido y esa lectura está habilitada, el core
   busca el registro antiguo, lo convierte y guarda en sector 1. El guardado
   existente comprueba contenido, checksum y commit antes de declarar migrado.
4. Si la imagen ocupa sector 7, el adaptador proporciona rango legacy vacío.
   El core nunca lo consulta; tampoco accede a dirección cero. Si faltan registros,
   inicializa y guarda los valores actuales por defecto en sector 1.
5. Fallo de copia conserva estado MIGRATION_FAILED; fallo de escritura conserva
   WRITE_FAILED. No se borra el sector 7 desde el servicio de preferencias.
6. La migración es local al arranque, sin nuevos ACK/eventos ni sesión remota.
   Los endpoints y diagnósticos de preferencias existentes conservan su contrato.

## Impacto en repositorios

| Repositorio | Estado | Motivo |
| --- | --- | --- |
| FirmwareF4 | origen | Linker, manifiesto, adaptador Flash, inicialización y documentación |
| Firmware ESP01 | no_aplica | Sin cambios de comandos, transporte o payloads consumidor |
| Web ESP01 | no_aplica | Sin nuevo esquema JSON ni flujo remoto |
| AutoMicroQtDesktop | no_aplica | Sin nuevo esquema UNER ni flujo remoto |

## Orden de integración y precauciones concretas

La placa debe haber ejecutado correctamente la copia a sector 1 **antes** de
instalar una imagen que ocupe el antiguo sector 7. Si allí hay datos únicos,
respaldarlos y ejecutar primero una imagen de hasta 352 KiB o el build protegido;
confirmar guardado/migración y después instalar la imagen ampliada.

La imagen actual medida antes de este cambio cabe antes del sector 7, pero no
se recompiló esta versión: no se afirma su nuevo tamaño. Programación selectiva
debe preservar sector 1; un borrado masivo destruye preferencias. Si una carga
ya sobrescribió el único log legacy, el firmware no puede recuperarlo.

El código de bootloader del repositorio ya soporta este máximo; no se modifica
su ubicación ni su validador. No se confirmó qué bootloader está instalado en
la placa. Los binarios existentes todavía corresponden al mapa previo y deben
recompilarse antes de aplicar este cambio físico.

## Evidencia y límites

Implementación en FirmwareF4 preservando cambios locales anteriores; otras
fuentes consumidor no se editaron. No se ejecutaron compilaciones, tests,
validaciones ni flasheo para esta modificación. El pedido anterior de compilar
se registró por separado; sus tamaños no se presentan como nueva medición.
Se agregan **128 KiB de capacidad** de aplicación, independiente del tamaño final.
El registro central se publica; los cambios de firmware permanecen locales.

## Compilación posterior solicitada

2026-10-06: F4 Debug recompilado con GNU ARM 14.3.1 y `make -j4 all`, exit 0,
incluido enlace y empaquetado. El manifiesto final tiene flags=0x00000000:
sector 7 disponible para la aplicación. BIN final 274.284 bytes; CRC32
0x85A2F2A9. Región de aplicación 491.520 bytes (480 KiB); libre **217.236 bytes
(212,14 KiB; 44,20%)**. GNU size: text 266.000, data 8.180, bss 26.248.
Configuración vigente -Og en Core, resto de reglas Debug conservadas.
Artefactos .bootable.elf/.bin/.hex/.sha256 en FirmwareF4/Debug.
Permanece advertencia linker LOAD RWX. Sin tests, flasheo ni migración física
comprobada; las afirmaciones iniciales de falta de compilación quedan superadas
por este resultado. Log: FirmwareF4/Debug/build-app480.log.

2026-10-06, pedido posterior: Debug vuelve a **-O0 sin optimización**, con -g3.
Se restauró el nivel por defecto sin optimización en .cproject para C/C++ y
las reglas Debug vigentes usan -O0. Recompilación `make -j4 all` exit 0,
incluido empaquetado: BIN **372.368 bytes**, CRC32 0x3DEA47F3, flags=0.
Libre sobre 480 KiB: **119.152 bytes (116,36 KiB; 24,24%)**. GNU size:
text 364.084, data 8.180, bss 26.256. Estos son los artefactos Debug actuales,
reemplazando la medición -Og anterior. Persiste advertencia RWX, sin flasheo.
La imagen ahora entra **11.920 bytes en sector 7**: no puede migrar desde su
antiguo log; antes de instalarla, la placa debe tener preferencias en sector 1.
Log: FirmwareF4/Debug/build-app480-o0.log.
