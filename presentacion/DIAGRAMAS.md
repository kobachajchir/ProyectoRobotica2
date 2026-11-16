# Diagramas D-01 … D-11

SVG en [`diagramas/`](diagramas/) (vectoriales, editables) y PNG ×2 en [`diagramas/png/`](diagramas/png/). Se regeneran con
`python diagramas/generar_diagramas.py` (solo escribe en esa carpeta). Leyenda de colores: gris = proceso · amarillo = decisión ·
azul = entrada/dato · violeta = hardware · rojo = fallo/rechazo · verde = correcto/fin · oscuro = inicio/fin · guiones = nota.

| ID | Archivo | Filmina · zona | Qué muestra |
| --- | --- | --- | --- |
| D-01 | `D-01_arquitectura_general.svg` | 1 · B | Operador → Web → ESP → ATmega → actuadores; panel y sensores; Loader por USB |
| D-02 | `D-02_conexionado.svg` | 3 · B | Mega al centro con cada periférico y sus pines |
| D-03a | `D-03a_ciclo_principal_ATmega.svg` | 4 · D | Ciclo principal (`main.c`) en 8 pasos |
| D-03b | `D-03b_servicio_de_control.svg` | 4 (apoyo) / 6 | `Afr2Product_Task`: vencimientos, F1, odometría, PID, F3/F4, `Motor_Write` |
| D-04 | `D-04_flujo_GOTO.svg` | 6 · B | Pedido GOTO → validación → lazo cada 10 ms → llegada |
| D-05 | `D-05_ciclo_mision_recolectora.svg` | 9 · D | Ciclo de la misión con sus cuatro finales |
| D-06 | `D-06_circuito_KEY_RST_NPN.svg` | 12 · A | Esquemático de los dos canales NPN y secuencia de carga |
| D-07 | `D-07_navegacion_OLED_encoder.svg` | 10 · C | Corto/largo, Inicio, Menú, Rápidas, retorno, Sí/No |
| D-08 | `D-08_cadena_cinematica.svg` | 7 · A | Cadena mundo → base → montura → brazo → TCP |
| D-09 | `D-09_flujo_Loader.svg` | 11 · C | Pasos del Loader de la fuente al equipo |
| D-10 | `D-10_pipeline_odometria.svg` | 6 · A | Pulsos Hall → pose (x, y, ψ) |
| D-11 | `D-11_patrones_rueda_mecanum.svg` | 5 · B | Signos de rueda en avance, lateral y giro |

## Notas de uso

- Insertar como imagen y escalar **proporcionalmente** al ancho de la zona. Los diagramas altos (D-03a, D-03b, D-04, D-05, D-10) van en zonas verticales
  (3–4 columnas × 5 filas); si no caben, partirlos en dos columnas copiando el SVG y recortando en el PowerPoint (Formato de imagen → Recortar).
- Para cambiar un texto: abrir el SVG en un navegador o editor vectorial, o editar `generar_diagramas.py` y regenerar.
- Contenido verificado contra: `main.c` (D-03a), `afr2_product.c` (D-03b, D-04, D-10), `ROBOT_RECOLECTOR.md` (D-05), `PLAN_KEY_RST_AUTOMATICO.md` (D-06),
  `UI_SCREEN_FLOW.md` (D-07), `AFR2_Informe_Borrador_D1.md` (D-08, D-11), `Loader/README.md` (D-09), `PINOUT.md` (D-02).
- D-05 y D-06 son **diseño/simulación**, no comportamiento verificado en hardware (el pie de cada diagrama lo dice).
