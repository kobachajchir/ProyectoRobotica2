# AFR2-20261028-15 — Selección directa del BIN AT Ai-Thinker

Autoría: fuentes propias AFR2, Koba Chajchir; adaptación mediante Codex.

El usuario señala que el firmware descargado viene como
`Ai-Thinker_ESP8266_DIO_32Mbit_V1.5.4.1_20161202.bin`.
Loader 0.9.4 permite elegirlo directamente en **Paquete AT / BIN...**;
conserva selección de manifiestos JSON para otros paquetes.

## Mapa reconocido

Se admite exclusivamente el contenido descargado del fabricante en
AFR2-20261028-14: 4194304 bytes y SHA-256
`ac30e8b6aee2cfbd58fd1e39d72486641f56fefab345038cb94c847167992103`.
El reconocimiento no depende del nombre de archivo. No se convierte ni modifica
el BIN. El manifiesto equivalente se construye en memoria, sin generar JSON.

Se interpreta como imagen completa de flash, desde 0x0, con una región firmware
de 4 MiB. La cabecera observada E9/03/02/40 declara DIO/40 MHz/4 MiB;
es metadata del archivo, no medición de la placa. La colocación a cero es una
inferencia del formato de imagen completa y cabecera al comienzo, no una regla
para BIN arbitrarios ni una prueba física de este firmware en el ESP del usuario.
La confirmación de carga expone imagen completa, offset, hashes y reemplazo de
toda la flash. Se conservan consulta de capacidad real, igualdad con perfil,
comprobaciones de cambios de archivo y verificación final. No crea LittleFS AT
ni declara separables las regiones internas de esta imagen monolítica.

Otros BIN se rechazan con explicación para usar manifiesto de su proveedor.
No se adivina mapa por extensión, nombre o tamaño solamente.

## Estado

Fuentes revisadas mediante lectura/diff. Sin compilación, tests, validadores,
GUI, COM ni flasheos. Pendiente comprobar interfaz e instalación. El feedback
0.9.3 y conversor 0.9.2 se conservan. ATmega, ESP, Web y CAD sin cambios de
fuente por esta ficha; no se cambia el firmware instalado ni perfiles de usuario.

Procedencia por archivo: `../provenance/loader-at-bin-directo-20261028.json`.
