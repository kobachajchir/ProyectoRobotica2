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
| 1.16.0 | subversión | AFR2-20261110-08: tarjeta de repositorio en Desarrollador, QR local y enlace directo a GitHub. Web en fuente; ATmega/ESP solo versión compartida. Sin compilar/cargar. |
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
