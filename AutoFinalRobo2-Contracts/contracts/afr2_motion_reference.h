/* AFR2-20261030-13. Fixed chassis-to-wheel reference from user direction report.
 * Logical +X=forward, +Y=left, +yaw=CCW viewed from above.
 * Mirror of motion_reference.json; independent of per-wheel motor/Hall trims. */
#ifndef AFR2_MOTION_REFERENCE_H
#define AFR2_MOTION_REFERENCE_H
#define AFR2_FORWARD_WHEEL_SIGN (-1)
#define AFR2_YAW_WHEEL_SIGN (-1)
/* AFR2-20261107-41: izquierda/derecha informadas cruzadas; lateral invertido. */
#define AFR2_LATERAL_WHEEL_SIGN (-1)
#endif
