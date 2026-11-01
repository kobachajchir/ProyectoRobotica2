/** AFR2-20261030-18. Shared order in firmware, raster and native Web controls. */
export const MOTOR_IDS = ['FL','FR','RL','RR'] as const;
export type MotorId = typeof MOTOR_IDS[number];
export function stepMotor(selected:number,delta:number){return ((selected+delta)%4+4)%4;}
