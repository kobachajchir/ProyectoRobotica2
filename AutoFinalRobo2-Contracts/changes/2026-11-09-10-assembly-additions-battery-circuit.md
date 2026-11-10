# AFR2-20261109-10 — Etapa 7: Nuestros agregados y circuito de batería compacto

Pedido humano: retirar OLED, encoder y soporte de la etapa6; crear una nueva7, ocultar el auto, mostrar frontalmente el panel visto desde arriba y luego girarlo a su posición de montaje. Mostrar también el sensor de batería con dos resistencias, capacitor y pin de salida. Reutilizar panel, portapilas y voltímetro desde el modelo existente en el navegador; exportar de Blender solo cuatro componentes y cables. Corrección final: cables cortos, rectos, rojo a izquierda y GND negro a derecha, paralelos; portapilas al centro y voltímetro arriba con derivaciones, sin cruces. Acercamiento al robot desde mitad de la etapa12 hasta el agarre.

Versión: **1.11.0 (subversión)**. Nueva sección compatible y asset de presentación; ATmega/ESP solo versión compartida por esta ficha.

## Recorrido Web en fuente

- Ahora hay14capítulos: la etapa6 queda dedicada a los sensores originales. La nueva7 es «Nuestros agregados»; las siguientes se renumeran.
- El reloj mecánico se mantiene detenido en su posición anterior a las ruedas durante la etapa7. Se desvanece el auto y se presenta el panel extraído de su GLB, con su OLED/encoder/soporte, orientado frontalmente y con vista elevada.
- Después se presenta el divisor a escala de detalle y se abre el plano para incluir el portapilas y el voltímetro. Estos últimos se extraen del mismo modelo en el navegador y se alinean con el cableado compacto; sus fuentes no se editan ni se descargan duplicadas.
- Al finalizar la etapa7 el panel gira y se acomoda en el auto. Portapilas/voltímetro vuelven a sus posiciones originales; el diagrama de cables del detalle se desvanece. No se estiran los cables de la composición compacta para fingir un ruteo físico instalado.
- Textos: OLED0,96″,128×64,I²C; encoder rotativo con botón y navegación por giro/corto/largo; sensor de batería por divisor10k/10k y filtro104/100nF. Se explica8V de batería como referencia del100%→4V en A5, reconversión de lectura analógica a voltaje y mapeo al nivel de batería. No se cambian calibración, referencia ADC ni parámetros del firmware.
- El acercamiento comienza a mitad de la nueva etapa12, progresa hacia el encuadre real del conjunto y centra al robot durante el agarre. Se reduce el espacio reservado a textos para aprovechar el banner.
- Las copias de materiales y transformaciones permanecen locales al recorrido. Se conservan la reversibilidad, opciones de accesibilidad, modo de movimiento reducido y ausencia de órdenes reales al robot.

## Asset producido en Blender

- `AutoFinalRobo2-Web/public/models/AFR2_Sensor_Bateria_Cableado.glb`: **11.692bytes**, **7.969bytes gzip**, **1.400triángulos**, una malla con materiales por componente/cable, Draco14bits de posición/6de normal.
- Solo R1/R4/C3/J3 y cables rojos/negros. OLED, encoder, soporte, voltímetro, portapilas y celdas no se incorporan al GLB nuevo.
- STEP reales de las tres rutas aportadas por el usuario, convertidos con FreeCAD y simplificados en Blender4.5.1 LTS. Mallas de cuerpos/terminales reutilizadas; cables nuevos como polilíneas compactas con sección de pocos lados.
- Fuente editable, scripts y procedencia en `AutoFinalRobo2-CAD/mecanica/sensor-bateria/`; hashes en `entrega_agregados.json`. El esquema original del usuario se conserva.
- La composición es ilustrativa. Posiciones de los bornes y ruteo visible son una propuesta de presentación, sin aprobación eléctrica, mecánica o de fabricación. No se realizó ERC/DRC.

## Integración

`assemblyAdditions.ts` reutiliza nodos del auto; `assemblyTimeline.ts` coordina la nueva etapa y reloj mecánico; `AssemblyScene.tsx` carga el único asset nuevo y adapta el encuadre; `AssemblyPage.tsx` cambia textos/contador/carga; `assemblyCar.ts` evita duplicar piezas ya presentadas. `modelAssetLoading.ts` obtiene tamaños sin depender de la query de revisión. `vite.config.ts` incluye explícitamente el circuito y el cubo auxiliar en el paquete de ejecución (el cubo ya existía, pero faltaba en esa lista).

Manifiesto de tamaños actualizado para el GLB producido, sin ejecutar generadores generales. La preparación de recursos del siguiente build autorizado incluirá el archivo. No se compiló la Web ni se generó/promovió una imagen LittleFS; no se cargó en el ESP físico.

## Estados

| Módulo | Estado de esta ficha |
| --- | --- |
| CAD | Escena nueva, cuatro modelos y cableado producidos y exportados; fuentes originales conservadas. |
| web | Fuente y asset integrados; sin compilación, ejecución de navegador o prueba del recorrido. |
| atmega / esp | Solo versión compartida; lógica conservada por esta ficha. Sin carga. |
| Loader | Sin cambios. |

No se ejecutaron tests, builds, validadores, renders, puertos ni flasheos. La lectura de escena y las exportaciones se realizaron como parte del modelado solicitado; no acreditan paridad, rendimiento, montaje o funcionamiento físico. No se modifica el trabajo concurrente de Wi-Fi/RAM, la OLED o el esquema KiCad.
