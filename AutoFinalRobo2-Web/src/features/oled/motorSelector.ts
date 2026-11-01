/** AFR2-20261030-18. Literal counterpart of Afr2MotorSelector_Render/Step. */
import type { Afr2OledBuffer } from './oledRasterizer';
import { gestureHint } from './firmwareHints';
import { MOTOR_IDS } from '../../domain/motorSelection';
export function renderMotorSelector(o:Afr2OledBuffer,selected:number){
  o.drawText(15,0,'Elegir un motor');
  MOTOR_IDS.forEach((name,i)=>o.drawText(9+32*i,22,name));
  o.fillRect(9+32*(selected&3),34,14,1);
  gestureHint(o,0,40,'turn','Elegir');gestureHint(o,0,53,false,'OK');gestureHint(o,66,53,true,'Volver');
}
