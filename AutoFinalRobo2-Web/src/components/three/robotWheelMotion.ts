/** AFR2-20261025-02. Visual simulation, not measured wheel feedback. */
export interface RobotWheelValues { fl: number; fr: number; rl: number; rr: number }
export const ROBOT_WHEEL_KEYS = ['fl', 'fr', 'rl', 'rr'] as const;
// The final CAD asset contains Rueda80 assemblies. Nominal visual radius only;
// this does not replace physical diameter/Hall calibration on the controller.
export const MODEL_WHEEL_RADIUS_MM = 40;
export function wheelAnglesFromTravel(travelMm: RobotWheelValues): RobotWheelValues {
  return {
    fl: travelMm.fl / MODEL_WHEEL_RADIUS_MM, fr: travelMm.fr / MODEL_WHEEL_RADIUS_MM,
    rl: travelMm.rl / MODEL_WHEEL_RADIUS_MM, rr: travelMm.rr / MODEL_WHEEL_RADIUS_MM,
  };
}
