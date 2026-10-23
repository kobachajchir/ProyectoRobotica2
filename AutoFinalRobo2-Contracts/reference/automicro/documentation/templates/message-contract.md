# Nombre del mensaje

ID estable: `shared:message/<nombre>/v1` · Esquema: versión documental explícita.
La versión del documento no introduce un byte nuevo al wire existente. Registrar
por separado versión de API/frame/payload y revisión de sus fuentes.

| Propiedad | Valor con evidencia |
| --- | --- |
| Productor / consumidor | Repositorio, archivo y símbolo de ambos extremos |
| Destino / transporte / sentido | Nodo y UART/USB/WebSocket, sin confundir identidades |
| Nombre JSON / ID binario | Ambos o indicar que uno no existe |
| Correlación | requestId, ruta+comando, transfer ID, sesión o secuencia |
| Autorización y guardas | Nodo, transporte, permiso, modo, disponibilidad y frescura |
| Aceptación | Qué reconoce la cola o ACK intermedio |
| Finalización | Mensaje efectivo y criterio para completar el cliente |
| Errores / timeout | Rechazo, NACK, descarte, pérdida de conexión y plazos de cada capa |

## Request

| Offset o campo JSON | Tipo wire | Unidad / rango | Opcional / default | Transformación y evidencia |
| --- | --- | --- | --- | --- |
| Campo | Ancho, endian, signo | Unidad física o semántica | Regla real del decoder | Símbolos productor/consumidor |

## Response y eventos

Repetir mapa por campo; distinguir resultado de solicitud y efecto final.
Declarar longitud/versionado y qué ocurre con valores desconocidos.

## Interoperabilidad y diferencias de lenguaje

Conversión C/C++/TypeScript, representación de float/bool/texto, buffers y vida de
datos. No deducir equivalencia a partir del nombre o tamaño de un struct.

## Hallazgos y actualización

Enlazar IDs AMDOC y sus anclas. Si cambia contrato efectivo, crear ficha AMI y
actualizar productores/consumidores con estado real. Registrar evidencia de fuente
por separado de compilación, prueba, despliegue y hardware.
