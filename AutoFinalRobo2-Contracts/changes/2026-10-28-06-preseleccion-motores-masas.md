# AFR2-20261028-06 — Comprobación de masa/par antes del nuevo CAD

28/10/2026. El usuario pide comparar motores nuevos con las piezas actuales y verificar el efecto de su propio peso antes de modelar. Se conserva íntegro el maestro v09; no se modifica Fusion ni se crean housings.

## Resultado de la preselección

[Informe completo y figura](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/preseleccion_actuadores_v10/README.md). Se conservan el inventario actual y las masas de piezas, con modelos externos de masa/CG/tensor de los candidatos. Fuentes comerciales primarias, datos de fabricante, incertidumbres de variantes y masa de empaque se separan de datos netos.

- GX55 en J5 agrega35 g; en50 ms sube la demanda de J2 de34,47 a35,64 kgf·cm, J3 de34,13 a35,28 y J4 de1,42 a1,87. El peso de un motor de muñeca perjudica al accionamiento J4 que lo lleva, aunque no se vuelva carga distal de su propio J5.
- Opción ligera analizada: GX3270 en J2, GX3270 actuales en J1/J3, DS9401 en J4, DS8930 en J5 y MG92B posicional160° en pinza. Los cambios de motor suman16 g frente al presupuesto actual;80 g de reserva localizada para montajes/cables llevan el aumento total a96 g y la masa a2.496,43 g con carga200 g, sin auto ni nueva batería/regulación.
- Con esa reserva,50 ms demanda37,19/36,70/1,77 kgf·cm en J2/J3/J4. No alcanza el objetivo preliminar×2; subir los GX a8,4 V tampoco lo resuelve. En100 ms y30°/s, los seis ejes pasan la comparación nominal de par×2 de esa alternativa. **No es capacidad de frenado real certificada ni una nueva configuración de firmware.**
- J4 también queda corto con MG90S para×2 en50 ms. El MG92B160° no conserva180°1:1 en J4, pero su recorrido sí alcanza la apertura funcional60 mm de la pinza. Se estudia allí, sujeto a variante posicional y calibración confirmadas.
- DS8930/DS9401 siguen sin recorrido180° documentado para esta selección, ni corrientes/curvas de servicio completas. DS3225 se evalúa solo como alternativa de importación; no se presenta como stock nacional. No se cierra compra o diseño por un resultado de masas.

## Comprobaciones ejecutadas

25 combinaciones de selección/masa, incluidas ocho sensibilidades del CG de J5 y una masa hipotética200 g en J3. Se calculan16.507 estados fuente de787 casos/trayectorias, con orientaciones J4/J6−90/0/+90, carga200 g,30°/s y frenadas suaves200/100/50 ms. La expansión de J1 contiene297.379 estados de carga por alternativa, aplicada a las secciones actuales. No es un barrido CAD ni todo el espacio continuo.

La suma de cuerpos/base de influencia reproduce el cálculo original con discrepancias del orden de10⁻⁸ N/10⁻⁹ N·m. Se propagan fuerzas/momentos por J1–J6, cortes de PVC, apoyos6000, eje, cuna y fijaciones J1; se actualizan CG y sensibilidad estática de vuelco. Se incluyen condiciones necesarias de fricción de pinza al variar su orientación. Faltan contacto de almohadillas, propiedades FDM, grado real de PVC, curvas de actuadores, alimentación y masa/CG/huella reales del auto. No hay FEA de sólidos, fabricación ni ensayo físico.

La exportación de estados completos≈182 MB se guarda localmente en `AutoFinalRobo2-CAD/exports/analisis-actuadores-v10/`, excluida por la política de regenerables. Se versionan fuentes, datos de entrada, JSON/CSV, figura y [manifest con SHA256](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/preseleccion_actuadores_v10/evidencia_calculo.json), para reproducir sin Fusion.

## Estados de módulos

| Módulo | Estado por esta ficha |
| --- | --- |
| atmega / esp / web | Sin cambios de producto, builds, flasheos o pruebas |
| CAD | Solo cálculo externo/documentación; maestro y geometría v09 conservados |
| Loader | Sin intervención; cambios concurrentes independientes |
| Contracts | Registro y enlaces de evidencia |

No se modelará la selección como si estuviera resuelta para50 ms: la opción ligera merece continuar como preselección bajo100 ms, pero sus datos mecánicos y de servicio deben confirmarse antes de convertirla en CAD definitivo.
