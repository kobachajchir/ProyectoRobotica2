/** Critically damped visual interpolation. Never produces telemetry samples. */
export function advanceServo(value:number,velocity:number,target:number,delta:number):[number,number]{
  const dt=Math.min(Math.max(delta,0),.05),omega=35,difference=value-target,c=velocity+omega*difference,decay=Math.exp(-omega*dt);
  return [target+(difference+c*dt)*decay,(velocity-omega*c*dt)*decay];
}
