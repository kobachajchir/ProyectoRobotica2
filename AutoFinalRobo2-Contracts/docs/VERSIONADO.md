# Versionado de AutoFinalRobo2

Acordado con el usuario el 08/11/2026 (AFR2-20261108-07). Cada cambio de código que se pueda cargar sube la versión, así
se sabe qué versión tiene el auto. La versión cargada se ve en la pantalla de inicio del OLED (3 s), en
Sistema > Versiones y en la Web.

## Una versión de producto: `MAYOR.MENOR.PARCHE`

ATmega, ESP y Web llevan **la misma versión** y se suben juntos, aunque el cambio toque uno solo:

| Módulo | Dónde |
|---|---|
| ATmega y ESP | `AutoFinalRobo2-Contracts/contracts/afr2_build_info.h` (`AFR2_FIRMWARE_VERSION`, `_MAJOR`, `_MINOR`, `_PATCH`) |
| Web | `AutoFinalRobo2-Web/package.json` y las dos entradas `version` de la raíz de `package-lock.json` |

El Loader (`AutoFinalRobo2-Loader/src`, `LoaderVersion`, hoy 2.x) y CAD tienen su propia línea; no se mezclan con la del
producto.

## Qué sube cada cambio

| Tipo | Se sube | Cuándo |
|---|---|---|
| **Versión** | MAYOR (`2.0.0`) | Rompe la compatibilidad: un ATmega y un ESP de versiones distintas ya no se entienden (cambia `AFR2_EXT_VERSION`, el formato de trama o el saludo), la EEPROM cambia sin migración y se pierden los ajustes, o el usuario decide una etapa nueva del producto. |
| **Subversión** | MENOR (`1.3.0`) | Función nueva compatible: pantalla, menú, preferencia, acción, comando, evento o modo nuevo; migración de EEPROM que conserva los ajustes; un equipo con la versión anterior sigue funcionando (sin la función nueva). |
| **Subversión mínima** | PARCHE (`1.2.1`) | Corrección o ajuste sin función nueva: error, texto, ícono, tiempo, posición en pantalla, rendimiento, límites. |
| Sin cambio | — | Solo documentación, fichas, planes, informes, CAD o Loader. |

Reglas:

1. Una subida por cambio pedido (una ficha AFR2). Si un mismo cambio mezcla tipos, manda el mayor.
2. Al subir MENOR, PARCHE vuelve a 0; al subir MAYOR, MENOR y PARCHE vuelven a 0.
3. Una corrección hecha en la misma tarea, antes de cargar, no vuelve a subir: queda en la versión de ese cambio.
4. La ficha y la entrada del CHANGELOG dicen `Versión: X.Y.Z (versión | subversión | subversión mínima)`.
5. Subir la versión no es compilar ni cargar: el estado de cada módulo se sigue informando aparte.

## Historial

| Versión | Tipo | Cambio |
|---|---|---|
| 1.39.8 | subversión mínima | AFR2-20261118-03: correcciones de la auditoría de modos (manual-pwm/sequence cuentan como Manual en la Web; Reposo con fallo devuelve OK). ATmega y Web en fuente; ESP solo versión. |
| 1.39.7 | subversión mínima | AFR2-20261118-02: «Iniciar movimiento» del seguidor se deshabilita solo sin línea a la vista (Web y OLED). ATmega y Web en fuente; ESP solo versión. |
| 1.39.6 | subversión mínima | AFR2-20261118-01: el seguidor frena al perder la línea y exige verla para arrancar. ATmega en fuente; ESP y Web solo versión. |
| 1.39.5 | subversión mínima | AFR2-20261117-17: selector de zoom de los simuladores en la vista completa del seguidor (60 % por defecto). Web en fuente; ATmega y ESP solo versión. |
| 1.39.4 | subversión mínima | AFR2-20261117-16: vista completa del seguidor con la cámara más alejada (×0,6) y piso de 4 m. Web en fuente; ATmega y ESP solo versión. |
| 1.39.3 | subversión mínima | AFR2-20261117-15: Programas → Seguidor de línea con la vista primero, tarjetas plegadas y botón «Vista completa» como Control táctil. Web en fuente; ATmega y ESP solo versión. |
| 1.39.2 | subversión mínima | AFR2-20261117-14: copia Web del dashboard y del selector de modo del OLED (desbloquea npm run dev). Web en fuente; ATmega y ESP solo versión. |
| 1.39.1 | subversión mínima | AFR2-20261117-13: matemática de los simuladores en un panel flotante arrastrable (Mostrar/Ocultar matemáticas). Web en fuente; ATmega y ESP solo versión. |
| 1.39.0 | subversión | AFR2-20261117-12: visor de seguimiento a pantalla completa (interfaz del control táctil) con Iniciar/Pausar/Detener y riel de Información, Parámetros y Calibrar. Web en fuente; ATmega y ESP solo versión. |
| 1.38.0 | subversión | AFR2-20261117-11: modos reales Reposo/Manual/Seguir línea/Ir (ATmega, ESP, OLED, Web), Seguidor en filas plegables, GOTO como «Ir» y modales con desenfoque. |
| 1.37.0 | subversión | AFR2-20261117-10: Card de sincronización en Controles rápidos OLED (tarjeta 8/9) y modal Web, activable solo con Internet real (si no, «Sin Internet»), sondeo ESP/Web, `0x30 op18` y `0x53` con `AFR2_EXT_F_SYNC_CONTROL` (EX01 v13). ATmega, ESP y Web en fuente. |
| 1.36.0 | subversión | AFR2-20261117-09: el ATmega informa versiones (ATmega, ESP, Web) en la autodetección del Loader 2.5.0; línea base en [COMPATIBILIDAD_VERSIONES.md](COMPATIBILIDAD_VERSIONES.md). ATmega en fuente; ESP y Web solo versión. |
| 1.35.3 | subversión mínima | AFR2-20261117-08: «Reconectando a» con la red STA configurada en vez del nombre del AP (ESP). ESP en fuente; ATmega y Web solo versión. |
| 1.35.2 | subversión mínima | AFR2-20261117-07: textos y tablas constantes del ATmega de RAM a flash (−344 B de RAM, sin cambio de comportamiento). ATmega compilado; ESP y Web solo versión. |
| 1.35.1 | subversión mínima | AFR2-20261117-06: botones redondos del encoder bajo la pantalla con leyenda, pills de la barra que se estiran en hover y captura del Loader en Hosting. Web en fuente; ATmega y ESP solo versión. |
| 1.35.0 | subversión | AFR2-20261117-05: copia de la sincronización en Firebase (`autofinalrobo2`, Auth anónimo, escritor con clave guardada en el ESP), vinculación de una compu con código en el OLED (CMD `0x57 SYNC_PAIR`) y vista remota `#/remoto` de solo lectura. ATmega, ESP y Web en fuente. |
| 1.34.0 | subversión | AFR2-20261117-04: Modo de robot sincroniza el menú OLED y la tarjeta rápida con Reposo, Manual, Seguidor de linea e Ir, más Volver contextual. ATmega/Web en fuente; ESP solo versión. |
| 1.33.2 | subversión mínima | AFR2-20261117-04: Ir con imagen interior cover, crossfade reversible de 400 ms, escala 1.04→1 y marco azul/violeta de 2 px, sin desplazar tarjeta ni contenido. Web en fuente; ATmega/ESP solo versión. Se conserva SVG hasta recibir imagen limpia. |
| 1.33.1 | subversión mínima | AFR2-20261117-03: corrige TS2339 en la sincronización online; `RobotPose` presente ya pasó el filtro `valid === true` de `parseRobotState`. Web en fuente; ATmega/ESP solo versión. |
| 1.33.0 | subversión | AFR2-20261117-01: sección Seguidor de línea (cinta fija a escala y auto en movimiento), accesos rápidos Ir/Controlar/Seguidor en Inicio con fondo al pasar, y deltas de sincronización por hash FNV-1a por capa. Web en fuente; ATmega y ESP solo versión. |
| 1.32.0 | subversión | AFR2-20261116-01: QR en Configuración (URL/IP + Wi-Fi), SoftAP máx. 4 clientes y redirección `/busy` con 1 cliente Web activo, icono online/sincronizando y aviso tras 5 fallos (Cerrar por defecto / Desactivar), sincronización online por capas (deltas 5–10 s y botón explícito en simuladores), panel y mejoras de seguidor de línea, y compensación por rueda `motor_trim` (EEPROM v14). ATmega, ESP y Web en fuente. |
| 1.31.4 | subversión mínima | AFR2-20261113-05: selector de secciones (pills) con la misma separación en todas las páginas; el del Simulador sale del encabezado. Web en fuente. |
| 1.31.3 | subversión mínima | AFR2-20261113-04: Simulador GOTO con el mismo margen y padding de encabezado y selector de vista que el de secuencias. Web en fuente. |
| 1.31.2 | subversión mínima | AFR2-20261113-03: Línea IR con cinta negra bajo el modelo 3D y ancho de cinta configurable (18 mm por defecto). Web en fuente. |
| 1.31.1 | subversión mínima | AFR2-20261113-02: Línea IR con el modelo 3D del auto en vista superior (IR y línea sobre la posición real del sensor). Web en fuente. |
| 1.31.0 | subversión | AFR2-20261113-01: GOTO con orientación «Frontal»/«Seguir camino» y nodos de giro sobre el camino (ventana con auto 3D, campo y deslizador); estado de la línea IR en Sensores. Web en fuente. |
| 1.30.0 | subversión | AFR2-20261112-07: herramienta «Lápiz» en el GOTO: un trazo continuo que arranca en el último destino se simplifica a tramos rectos (comando `path`). Web en fuente. |
| 1.29.0 | subversión | AFR2-20261112-04: botón «Burbuja» en los simuladores con brazo: esfera de malla con centro en la base y radio igual al alcance máximo. Web en fuente. |
| 1.28.2 | subversión mínima | AFR2-20261112-03: OLED en vivo con layout fijo (aviso único, columna lateral reservada, lienzo 2:1 que no colapsa) y «Al cambiar» pide la pantalla solo al cambiar de pantalla. Web en fuente. |
| 1.28.1 | subversión mínima | AFR2-20261112-01: calibración final del GOTO (tolerancias 4 mm / 10 mrad, EEPROM v13, rumbo en traslación). ATmega en fuente. |
| 1.28.0 | subversión | AFR2-20261111-08: recorrido GOTO completo cargado en el ATmega (robot.route.*, 0x46..0x49) que sigue sin WebSocket/Wi-Fi/enlace, pantalla OLED de avance, trayectoria real en la Web, giro corregido 1,028, pruebas mínimas con dirección de medida. Solo fuente. |
| 1.27.1 | subversión mínima | AFR2-20261111-07: un ensayo GOTO enviado sigue aunque se corte el WebSocket (origen local, MOTION_HALT no lo corta). ATmega en fuente; ESP/Web solo versión. |
| 1.27.0 | subversión | AFR2-20261111-06: factores direccionales de odometría, pantallas OLED/Web y EEPROM v12 con migración. Solo fuente. |
| 1.26.2 | subversión mínima | AFR2-20261111-05: PWM manual sin cortes por lazo/Hall y errores visibles sin cerrar el táctil ni encadenar STOP. ATmega/Web fuente; ESP solo versión. |
| 1.26.1 | subversión mínima | AFR2-20261111-04: la pill de modo cambia el modo; Manual se conserva y salta los permisos de conducción. ATmega/ESP/Web en fuente. |
| 1.26.0 | subversión | AFR2-20261111-03: precarga de los modelos 3D al conectar, con toast de avance y reintento. Solo fuente Web; ATmega/ESP solo versión. |
| 1.25.0 | subversión | AFR2-20261111-02: odometría lateral ×0,775 (medido 77,5 cm por metro) y menú OLED Pruebas examen con pantalla de ejecución. ATmega/Web en fuente, ESP solo versión. |
| 1.24.0 | subversión | AFR2-20261111-01: «Pruebas mínimas» en Conducir con registro y exportación, y slider «Cajas a recolectar» con meta de misión en el simulador del recolector. Solo fuente Web; ATmega/ESP solo versión. |
| 1.22.0 | subversión | AFR2-20261110-14: limpieza real de fallo Web, CMD 0x56 y tiempos de F1; control antes de render. Solo fuente. |
| 1.23.2 | subversión mínima | AFR2-20261110-18: logo de bienvenida a 768 px para que el paquete ESP entre en el presupuesto LittleFS. Solo fuente Web. |
| 1.23.1 | subversión mínima | AFR2-20261110-17: renders CAD de Fusion y captura de Inicio en las tarjetas de Ayuda; versión Web leída de package.json. Solo fuente Web. |
| 1.23.0 | subversión | AFR2-20261110-16: envío GOTO completo, giros de 90°, barra lateral táctil, blur de modales, pill de modo, Armado simplificado. Solo fuente Web. |
| 1.22.1 | parche | AFR2-20261110-15: guarda de tormentas de IRQ Hall/encoder, F1 aislado no enclavante (3 en 2 s enclavan), FAULT/NOISE visibles en TAP. Solo fuente. |
| 1.21.0 | subversión | AFR2-20261110-13: búsqueda desde AP, configuración/reinicio AP y consulta privada de última red conectada. Solo fuente. |
| 1.20.0 | subversión | AFR2-20261110-12: GOTO local con arcos, Bézier, edición por clics, borrador y deshacer/rehacer. Solo fuente. |
| 1.19.0 | subversión | AFR2-20261110-11: editor y mapa con documento único, pistas proporcionales, vinculación temporal y esperas. Solo fuente. |
| 1.18.0 | subversión | AFR2-20261110-10: Estado → Fallos local, cono radar OFF y batería automática desde arranque; AVR/Web vinculados. Solo fuente. |
| 1.17.0 | subversión | AFR2-20261110-09: centro de alertas en la cabecera, recuperación guiada, corrección del caso de arranque/limpieza de Fallo1 y recorrido visual sin envío de narración. ATmega/Web en fuente, ESP solo versión. Sin compilar/cargar en esta etapa. |
| 1.16.0 | subversión | AFR2-20261110-08: tarjeta de repositorio en Desarrollador, QR local y enlace directo a GitHub. Web en fuente; ATmega/ESP solo versión compartida. Sin compilar/cargar. |
| 1.15.0 | subversión | AFR2-20261110-03: recolección virtual de cajas y entrega en drop circular con contador. Solo fuente. |
| 1.14.1 | subversión mínima | AFR2-20261110-02: fondo de cuadrícula compartido por la navegación Web. Solo fuente. |
| 1.14.0 | subversión | AFR2-20261110-01: botón para activar/silenciar la narración de Armado. Solo fuente. |
| 1.13.0 | subversión | AFR2-20261109-12: pantallas OLED reales y entorno de recolección en Armado. Solo fuente, sin compilar/cargar. |
| 1.0.0 | versión | AFR2-20261108-02: primera versión 1 en ATmega, ESP y Web (incluye -03, -04 y -05). |
| 1.1.0 | subversión | AFR2-20261108-06: selector «Sin red Wi-Fi» (Iniciar AP / Seguir esperando) y preferencia Reintento Wi-Fi. |
| 1.2.0 | subversión | AFR2-20261108-07: pantalla de inicio de 3 s con la versión y esta política de versiones. |
| 1.3.0 | subversión | AFR2-20261108-08: el flasher ATmega muestra estado, imagen y progreso de carga del ESP en la OLED (Loader 2.1.0). |
| 1.4.0 | subversión | AFR2-20261108-09: el log de depuración del ATmega elige desde el Loader qué copiar al USB (log, tramas de cada sentido del enlace o bytes crudos del ESP), detecta el ruido de la ROM del ESP y cuenta sus reinicios (Loader 2.2.0). |
| 1.5.0 | subversión | AFR2-20261108-10: interruptor «Ultrasonido» en Controles rápidos (el HC-SR04 ya no mide por defecto), ícono de largo en el dashboard y TAP a 115200 salvo enlace completo a 250000 (Loader 2.2.1). |
| 1.6.0 | subversión | AFR2-20261108-12: Controles rápidos en orden Modo, Motores, Ultrasonido, Radar, Servo, Línea, Rastro, Ajustes; la tarjeta del dashboard recorre todos esos estados con ícono. |
| 1.7.0 | subversión | AFR2-20261108-13: Línea de Controles rápidos enciende los sensores IR sin motores; el radar enciende servo y HC-SR04 y se detiene si se apaga alguno (Loader 2.4.0: vista Reinicios). |
| 1.7.1 | subversión mínima | AFR2-20261108-14: elegir STA, AP o AP+STA (o Desconectar) en el OLED detiene el movimiento y reemplaza el trabajo Wi-Fi pendiente en vez de pedir «Detener / ver red». |
| 1.8.0 | subversión | AFR2-20261109-01: radar y ángulo del servo con ecos en vivo, servo centrado al arrancar, avisos «Reconectando a» / «Iniciando AP» / «Cambiando Wi-Fi», el ESP cambia de modo sin configuración guardada y levanta el AP si nadie responde «Sin red Wi-Fi» en 20 s. |

| 1.9.0 | subversión | AFR2-20261109-02: política AP persistente, detención AP y AP puro sin búsqueda STA. Solo fuente. |
| 1.9.1 | subversión mínima | AFR2-20261109-03: permisos PWM restituidos, OFF/política desconocida Wi-Fi y recursos regenerados; sin compilar/cargar. |

| 1.10.0 | subversión | AFR2-20261109-04: Armado del robot en Explorar, ensamblaje por scroll del auto y brazo V17 con demostración de agarre. Solo fuente, sin compilar ni cargar. |
| 1.10.1 | subversión mínima | AFR2-20261109-05: tarjeta del dashboard72×22, icono18 y texto centrados en AVR/Web. Solo fuente, sin compilar/cargar. |
| 1.10.2 | subversión mínima | AFR2-20261109-06: navegación Wi-Fi por padre, STA sin AP fallback y credenciales directas. |
| 1.10.3 | subversión mínima | AFR2-20261109-07: Armado como banner de producto, modelo flotante con iluminación de estudio y fade por scroll. Solo fuente, sin compilar/cargar. |
| 1.10.4 | subversión mínima | AFR2-20261109-08: buffers WS compartidos sin String intermedio, control de memoria/fragmentación y de acumulación de estados. Solo fuente, sin compilar/cargar. |
| 1.10.5 | subversión mínima | AFR2-20261109-09: latidos/estados livianos, cola compacta, textos en flash, liberación Wi-Fi y límite HTTP. ESP compilado: −4528 B RAM estática; sin cargar. |
| 1.11.0 | subversión | AFR2-20261109-10: nueva etapa7 Nuestros agregados, circuito compacto y cableado desde Blender, modelos reutilizados y acercamiento final. Solo fuente y assets; sin compilar/cargar. |
| 1.12.0 | subversión | AFR2-20261109-11: Play/Pausa20s de Armado, OLED/encoder y sensor separados, introducción centrada y subtítulo. Solo fuente. |
