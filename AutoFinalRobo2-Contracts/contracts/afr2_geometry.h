/* AFR2-20261030-07. Nominal CAD geometry; not physical calibration. */
#ifndef AFR2_GEOMETRY_H
#define AFR2_GEOMETRY_H
#define AFR2_FIXED_WHEEL_MM 80u
#define AFR2_FIXED_RADIUS_SUM_MM 170u /* 62 + 107.66 mm, rounded nominal */
/* AFR2-20261111-02: medido por el usuario. Un lateral de 1000 mm por odometria recorria 775 mm reales
 * (4 pruebas iguales, izquierda y derecha): los rodillos mecanum patinan de costado. La odometria escala
 * el desplazamiento lateral por 0,775. Avance y retroceso daban 1000 mm exactos: no se tocan. */
#define AFR2_LATERAL_ODOM_PERMILLE 775u
/* AFR2-20261111-08: medido por el usuario: cada giro de 90 grados por odometria terminaba en 92,5 reales (360 -> +10).
 * Las ruedas cuentan menos giro del real; la odometria lo escala por 92,5/90 = 1,028 en ambos sentidos. */
#define AFR2_TURN_ODOM_PERMILLE 1028u
#endif
