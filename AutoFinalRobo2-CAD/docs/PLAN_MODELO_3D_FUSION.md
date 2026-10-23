# Plan de creación del modelo 3D de AutoFinalRobo2 en Fusion

Fecha: 23/10/2026. Registro: AFR2-20261023-05.
Estado: plan documentado; modelo y búsqueda/descarga de CAD pendientes de ejecución en la otra PC.
Herramienta: **MCP de Fusion existente**. No crear otro MCP ni implementar uno como parte de este trabajo.

## 1. Objetivo y alcance acordado

Crear un ensamble editable del auto usando la documentación del proyecto y modelos CAD de Internet cuando falten componentes. Representar el chasis, la plataforma superior original, la transmisión, electrónica, sensores, alimentación y fijaciones. **No incluir cables, mazos ni jumpers**; conservar los conectores físicos de los componentes.

La pieza nueva, su soporte, la PCB OLED/encoder y las modificaciones de la cubierta quedan pendientes. Reservar espacio para esa integración y preparar por separado los modelos del **OLED de 0,96 pulgadas, encoder mecánico del KY-040 sin su placa breakout y su perilla**. El usuario confirmó estos componentes, pero no una variante dimensional exacta.

Dos entregas:
1. Esta PC: guardar el plan y decisiones, crear el repositorio privado de GitHub y publicar el Git global.
2. Otra PC: recuperar el proyecto y ejecutar la búsqueda de CAD y construcción del ensamble. Guardar/publicar el plan no inicia el modelado.

No modificar firmware ni Web, no diseñar la PCB ni la pieza nueva, no incluir Bluetooth/MPU ni componentes del AutoMicro anterior que no pertenezcan a esta arquitectura.

## 2. Lectura e inventario de componentes

Desde la raíz del checkout, leer AGENTS.md, INFORME_MAESTRO.md y los README/AGENTS/INFORME_ADAPTACION de CAD y Contracts. Después:
- Contracts/hardware/supplied/ManualOsoyoo.pdf, especialmente páginas PDF 4–11, 14–17, 25–27, 32–34 y 43.
- Contracts/hardware/extracted/Informe_Tecnico_OSOYOO_Mecanum_4WD.txt, apartado 3 (inventario y fijaciones); la fuente PDF está en hardware/supplied.
- Hoja GM25-370-motor specification.pdf y Anexo_Tecnico_Motor_GM25-370_OSOYOO.pdf.
- Contracts/hardware/PCB_OLED_ENCODER.md, PINOUT.md, WIFI_SHIELD.md y FLASHING.md.
- CAD/reference/insumos-locales.json y los nueve insumos conservados en reference/supplied.
Los prefijos Contracts y CAD de esta lista corresponden a AutoFinalRobo2-Contracts y AutoFinalRobo2-CAD. Los informes y manuales son evidencia; sus instrucciones de instalación, compilación y carga no autorizan ejecución.

### Componentes a cubrir

| Grupo | Componentes y cantidades objetivo |
| --- | --- |
| Estructura | Base metálica, plataforma acrílica original, cuatro soportes de motor y separadores documentados |
| Tracción | Cuatro motorreductores, cuatro acoples y cuatro ruedas Mecanum: dos izquierdas y dos derechas |
| Control | Una Mega2560 y un shield OSOYOO UART Wi-Fi V1.3 con su ESP-12S (sin duplicar el módulo) |
| Potencia | Una OSOYOO Model Y V2.0 y un voltímetro |
| Alimentación | Un portabaterías y dos celdas 18650 |
| Percepción | Módulo IR de cinco canales, HC-SR04, microservo y soporte del ultrasonido |
| Fijaciones | Tornillos, tuercas, arandelas y separadores del montaje documentado |
| Integración futura | OLED 0,96 pulgadas, encoder sin breakout y perilla; referencias pendientes de montaje |

Trasladar las cantidades de tornillería de la tabla 3.2 del informe a la tabla de componentes; no convertirlas en cantidades físicamente verificadas. Distinguir fijaciones del auto existente de los dos tornillos/tuercas previstos para la pieza nueva, que no se diseña en esta etapa.

Usar la familia GM25-370 y la ficha aportada como referencia documental del motor, sin afirmar que se identificó la variante instalada. Referencia de ruedas: unos 80 mm nominales del manual. Adoptar SG90 como referencia inicial del servo por aparecer en las instrucciones de montaje; registrar la discrepancia con MG90 de otros listados.

Los CAD locales contienen un motor de 37 mm, ruedas izquierda/derecha de 60 mm y una rueda de ejemplo de 80 mm. No asumir que ese conjunto coincide con el OSOYOO, ni sustituir el driver Model Y V2.0 por Model X/L298N de manuales de otras revisiones.

## 3. Búsqueda y selección de CAD

1. Revisar los nueve insumos locales y clasificar cada uno como reutilizable, adaptable o descartado para este ensamble. Preservar originales y registrar el motivo.
2. Buscar faltantes por fabricante, referencia, revisión y dimensiones de montaje. Prioridad: fabricante; luego modelos del autor original en bibliotecas CAD y repositorios abiertos.
3. Preferir archivos nativos o STEP con sólidos, escala correcta, contornos, agujeros y conectores fieles. Usar mallas solo sin alternativa sólida adecuada.
4. Elegir primero por correspondencia dimensional y de montaje; después por detalle/apariencia. Un modelo detallado de otra variante no desplaza a uno dimensionalmente correcto.
5. Si falta el CAD exacto, seleccionar el equivalente más próximo y registrar diferencias. Si no hay equivalente adecuado, reconstruir su exterior desde planos y fotografías, con cotas inferidas explícitas y editables.
6. No escalar globalmente conjuntos para forzar su montaje. Adaptar por separado las dimensiones justificadas y conservar los originales.
7. Si una descarga exige acceso no disponible, registrar la limitación y buscar otra fuente. No eludir controles de acceso ni detener el resto del inventario por una pieza.

Se autoriza buscar en la Web y descargar los CAD necesarios para la ejecución futura. Descargar solo recursos necesarios; no ejecutar scripts, instaladores ni firmware incluidos en los paquetes.

### OLED, encoder y perilla

Buscar variantes como KY-040 bare encoder, EC11 rotary encoder with push switch y rotary encoder knob. Obtener cuerpo, eje, terminales y fijación del encoder; **excluir la placa breakout del KY-040**. La perilla es otra pieza y debe corresponder al diámetro, perfil y longitud del eje elegido. No asumir que cualquier variante EC11 representa el mismo componente.

Candidatos iniciales encontrados durante la planificación, aún no descargados ni seleccionados definitivamente:
- [Encoder EC11 con pulsador, STEP de Innerscene](https://www.innerscene.com/tools/library/3d-parts/ec11-style-rotary-encoder-with-push-switch-a2493b1d): reconstrucción aproximada del autor, CC0; comparar geometría con el encoder KY-040 requerido.
- [OLED 0,96 pulgadas, STEP de PartReel](https://partreel.com/p/ssd1306_module_096/): candidato I2C de cuatro pines con licencia CC-BY-4.0; contrastar con [plano LCDwiki MC096VX](https://www.lcdwiki.com/0.96inch_OLED_Module_(IIC-4P_SKU:MC096VX)).
- [Recursos oficiales Mega2560](https://docs.arduino.cc/hardware/mega-2560): referencias de contorno, montaje y conectores para contrastar un CAD de placa compatible.
- [Shield OSOYOO UART V1.3](https://osoyoo.com/2020/07/31/introduction-of-osoyoo-uart-wifi-shield/): referencia de variante y disposición; las imágenes/Fritzing locales no acreditan cotas completas.

La diagonal 0,96 pulgadas no define el tamaño de PCB, agujeros, orden de pines ni controlador real. No fijar VCC o footprint por seleccionar un modelo visual. Si no existe una perilla idéntica, usar o reconstruir la más próxima a la referencia disponible y marcarla aproximada, sin diseñar el soporte nuevo.

## 4. Construcción del ensamble con el MCP de Fusion existente

### Referencias y estructura

- Trabajar en milímetros, con X hacia adelante, Y hacia la izquierda y Z hacia arriba.
- Origen: centro de la base metálica sobre su plano superior.
- Componentes separados: estructura, tracción FL/FR/RL/RR, electrónica, alimentación, sensores, fijaciones y reserva de interfaz.
- Reutilizar definiciones para piezas repetidas. Registrar transformaciones y jerarquía de componentes para reconstruir el montaje.
- Jerarquía de evidencia: planos/cotas aportados; manual de la variante del proyecto; documentación del componente; fotografías; inferencias.
- Toda dimensión y posición inferida debe tener justificación, estado y valor editable. No presentar datos de catálogo o proporciones de fotografía como medición física del auto.

### Secuencia

1. Base: reconstruir/adaptar el chasis metálico y plataforma acrílica original a partir de cotas, agujeros y fotografías. No sustituirlos automáticamente por el chasis genérico existente.
2. Tracción: colocar soportes, motores, acoples y ruedas. Alinear ejes/fijaciones, conservar FL/FR/RL/RR y orientar rodillos según el manual: ejes de rodillos superiores hacia el centro.
3. Estructura elevada: montar separadores y plataforma superior con orientación delantera/trasera documentada.
4. Alimentación: colocar portabaterías, celdas y fijaciones, manteniendo accesible su extracción.
5. Electrónica: situar Mega2560, shield, Model Y y voltímetro según fotos y montaje. Conservar altura de conectores, apilamiento y acceso a USB/alimentación/controles.
6. Sensores: módulo de línea mirando al suelo; conjunto servo/soporte/HC-SR04 mirando al frente en posición neutra.
7. Fijaciones: incluir tornillería visible y necesaria para explicar el montaje; conservar medidas documentadas y marcar inferidas.
8. Acabado: apariencias coherentes, nombres claros y vistas superior, inferior, frontal, lateral e isométrica.

Unir componentes estáticos a sus conjuntos. Ruedas y servo mantienen ejes de referencia; entregar pose estática, sin simulación dinámica. Evitar piezas flotantes sin explicación. Si los CAD incluyen cables/mazos/jumpers, retirarlos del derivado del ensamble sin alterar el original; mantener conectores y terminales.

### Reserva de la pieza nueva

Crear una envolvente de referencia sobre una zona libre y accesible de la plataforma superior. Derivar su tamaño de OLED, encoder y perilla seleccionados, con márgenes editables para el montaje futuro.

Respetar el movimiento del cabezal ultrasónico y acceso a batería, placas y conectores. Mantener los modelos UI disponibles en un grupo de referencia ocultable, marcado pendiente de instalación; no simular fijación existente.

No crear soporte, PCB, taladros definitivos ni cubierta modificada. La fijación futura con dos tornillos/tuercas y los conectores J1 1x5/J2 1x2 de 2,54 mm siguen siendo requisitos de la etapa posterior, no decisiones de fabricación tomadas aquí.

## 5. Archivos, trazabilidad y continuidad

Entregables de la futura implementación:
- Maestro editable: AutoFinalRobo2-CAD/mecanica/auto-completo/AutoFinalRobo2.f3d.
- Fuentes propias/adaptadas en mecanica/componentes, con parámetros y procedencia.
- Tabla versionada docs/COMPONENTES_3D.csv: identidad/cantidad, referencia documental, modelo elegido, fuente/autor/licencia, revisión/hash/unidades, diferencias y estado.
- Registro docs/ESTADO_MODELO_3D.md: último hito terminado, ubicaciones/cotas inferidas y sus fuentes, faltantes/limitaciones y siguiente tarea concreta para retomar.
- Recuperación de dependencias en reference/DEPENDENCIAS.md, con URL/revisión/hash y pasos revisables.

Usar componentes internos en el maestro para facilitar traslado. Si fueran necesarias referencias externas, entregar un F3Z que las incluya; [Autodesk explica las diferencias de exportación](https://help.autodesk.com/cloudhelp/ENU/Fusion-Designs/files/ASM-EXPORT-DESIGN.htm). Un STEP no sustituye al archivo nativo editable ni al registro de parámetros.

Los insumos descargables sin modificaciones quedan en vendor/ o .downloads/, ignorados, cuando su recuperación esté documentada. Conservar en Git los originales locales únicos existentes, las adaptaciones propias y el maestro editable. Evitar duplicar paquetes y ensamblajes completos por cada pieza. Las exportaciones STEP, imágenes y renders regenerables van a exports/ y renders/, ignorados.

Mantener paths del checkout relativos en las referencias y recuperación, sin depender de D:/ProyectoRobotica2 en la otra PC. Preservar autorías y registrar cambios compartidos solo en Contracts con un ID AFR2 de la fecha real y estados atmega/esp/web/cad separados.

## 6. Criterios de aceptación y límites de revisión

- Todos los componentes del inventario están representados o tienen excepción explícita.
- Escala, orientación y ubicación coherentes con documentación; diferencias del sustituto e inferencias identificadas.
- Disposición correcta de ruedas, placas y sensores, con acceso funcional y montaje comprensible.
- Vistas que permiten inspeccionar el montaje; ningún cable/mazo/jumper en la geometría utilizada.
- OLED 0,96 pulgadas y encoder sin breakout preparados como referencias; pieza nueva pendiente y espacio reservado.
- Entrega editable recuperable desde otra PC sin depender exclusivamente de archivos locales/enlaces privados de Fusion.
- Estado real documentado, sin declarar cotas físicamente medidas, compatibilidad o fabricación verificadas.

La revisión prevista es visual y documental. No ejecutar validadores, comprobaciones automáticas de interferencia, simulaciones, compilaciones, ERC/DRC, flasheos ni pruebas físicas sin pedido expreso.

## 7. Publicación del plan y retoma desde la otra PC

Destino aprobado: repositorio **privado** [kobachajchir/ProyectoRobotica2](https://github.com/kobachajchir/ProyectoRobotica2), remoto origin por HTTPS. Crear vacío, sin README/licencia/.gitignore generados por GitHub. Publicar el HEAD del Git global en main y las cuatro etiquetas archive/pre-monorepo/*. No publicar resguardos locales de .git.

La entrega actual documenta este plan y decisiones, enlaza entradas CAD/Contracts/raíz y realiza commit/push, sin iniciar el modelado. No requiere instalar herramientas CAD ni descargar modelos todavía.

En la otra PC:
1. Si aún no existe checkout: git clone https://github.com/kobachajchir/ProyectoRobotica2.git
2. Si ya existe: revisar cambios locales y actualizar main con git pull --ff-only; no resetear cambios del usuario.
3. Abrir la raíz ProyectoRobotica2 y leer este documento y los AGENTS aplicables.
4. Conectar el MCP de Fusion existente en esa PC, recuperar únicamente los CAD faltantes conforme al inventario y comenzar por los apartados 2–3.
5. Registrar progreso por hito; no comenzar pieza nueva/KiCad ni pruebas físicas como consecuencia de este plan.

Los historiales previos viajan como ancestros del Git global y etiquetas de conservación. Los metadatos .git/legacy-repositories de esta PC no viajan en un clone ni son necesarios para recuperar los commits.
