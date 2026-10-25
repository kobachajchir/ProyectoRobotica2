# AFR2-20261024-12 — Ajustes, pistas, distancia y GOTO compacto

Pedido humano: agrupar hardware/reinicio, corregir edición de pistas y terminal,
permitir distancia real por pulsos y simplificar recorrido GOTO. Último ajuste:
ver mapa y comando actual juntos, con X/Y/rumbo/tiempo en una barra derecha.

## Web

Información de hardware agrupa conexión, ESP, ATmega y arranques en bloques
verticales, con última actualización exitosa y botón con icono dentro. Reinicio
es el otro desplegable principal; apariencia usa botones. Retirados la opción
de mando y el texto redundante de conexión. Conexión automática existente intacta.

Chasis/Pistas independientes son reversibles y conservan sus borradores. El
número de pistas activas se puede reducir/ampliar; las ocultas aportan cero.
Etiquetas FL/FR/RL/RR separadas. Seleccionar pista aplica acento; papelera44px
junto a Editable y márgenes simétricos abre confirmación con FloatingModal
animado existente. Deshacer restaura. Terminal Finalización oculta la paleta
y bloquea inserciones, duplicación del terminal o arrastres después de él.

Bloques de movimiento ofrecen ms o mm de recorrido de rueda; espera conserva
tiempo. Documentos/biblioteca v2 importan v1 sin borrar su clave de respaldo.
Simulación estima tiempo; no transforma tiempo ideal en evidencia de distancia.

GOTO tiene mapa y comando actual en la columna principal; coordenadas, rumbo
y tiempo apilados en la barra derecha junto al reproductor. Ajustes secundarios
plegados; Recorrido local por encima de botones. Lista cerrada muestra actual
y medias vecinas, sin scrollbar; desplegada muestra edición y Seguir activo.
Conserva fade140ms/FLIP220ms y reduced motion. En pantallas angostas la barra
se apila. Modelo final, unidades y suelo no se modifican. No dependencias nuevas.

## ESP y ATmega

[EX01v6](../contracts/EX01_SEQUENCE_DISTANCE_6.md), firmware0.8.0. BEGIN14 y
APPEND28 permiten instrucciones por rueda por tiempo/distancia, independientes
o con barreras de fase; tiempos puros mantienen formato previo. ESP negocia
versión/bit antes de emitir nuevos cuerpos. AVR reutiliza buffer640bytes y
calcula objetivo Hall con distancia, diámetro y cuentas por vuelta; conserva
watchdogs, lease, STOP y no reanudación. Máximo32registros/120s; Web rechaza
exceso sin truncar. En Chasis, distancia es la rueda de mayor recorrido, no
automáticamente desplazamiento del centro. Pivotes también admiten recorrido.

`AFR2_SEQUENCE_MEMORY_APPROVED=0` y `AFR2_HALL_CONNECTED=0` permanecen. Esto
es fuente compilada con gates0, **no función física habilitada/probada**. La
optimización puede eliminar la rama física; no acredita recursos al habilitarla.
Wrapper de ejecución conserva pantalla/layout y anuncia ruedas/tiempo máximo
para el modo nuevo. Biblioteca OLED copiada,59identidades,48nodos/11avisos y
navegación SW/Volver permanecen; no se crean pantallas sin contraparte Web.

## Evidencia y alcance

| Módulo | Estado real12 |
| --- | --- |
| Web | TypeScript/Vite/gzip completados;7activos promovidos a ESP/data/dist; sin navegador |
| ESP | Firmware candidato compilado; layout3MiB conserva carácter candidato |
| ATmega | Producto, comunicación y biblioteca compilados con gates0; sin placa |
| Contracts | Protocolo, estados, guías y procedencia actualizados |
| CAD | Sin cambios |

El ID11 ya corresponde al cambio de IRQ del encoder/Hall, que se conserva.
La compilación AVR actual incluye esa fuente11; no verifica frecuencia, pérdidas,
cableado ni rama Hall habilitada. No se reescriben evidencias históricas11.
La guía Web refleja su pinout documentado D2/D3 y recableado pendiente.

Web:1472196bytes gzip; estimación FS1556480 sobre presupuesto2867200.
AVR:64406bytes flash,4891SRAM estática incluida noinit, sin aprobación de pila.
ESP:46088bytes RAM,424869flash. [Logs y manifest](../documentation/builds/2026-10-24-12/README.md).

No se ejecutaron tests, validadores independientes, navegador, imagen FS,
monitor serie, reinicios reales, flasheos ni ensayos físicos. La denegación previa
del navegador se respeta. Autorías previas conservadas; no se modifica reference/
ni repositorios de referencia. Sin commit/publicación remota.
[Registro por archivo](../provenance/control-hardware-distance-20261024.json).
