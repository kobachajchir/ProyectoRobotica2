/** AFR2-20261101-04. Fit the visible model, excluding floors/workspace aids.
 * Uses projected box corners so wide/mobile viewports share the same margin. */
import { OrthographicCamera, PerspectiveCamera, Vector3, type Box3, type Camera } from 'three';
export type ModelView = 'perspective' | 'top' | 'side';
/** AFR2-20261102-03: dirección de cámara de los banners de Modelos (Blender (-0.62, 1, 0.42) en ejes Three). */
export const BANNER_VIEW_DIRECTION: readonly [number, number, number] = [-.62, .42, -1];
export function frameModel(camera:Camera,bounds:Box3,viewport:{width:number;height:number},view:ModelView='perspective',perspectiveDirection?:readonly [number,number,number]) {
  if(bounds.isEmpty() || viewport.width<=0 || viewport.height<=0)return null;
  const center=bounds.getCenter(new Vector3()),size=bounds.getSize(new Vector3());
  const direction=(view==='top'?new Vector3(0,1,0):view==='side'?new Vector3(1,0,0):(perspectiveDirection?new Vector3(...perspectiveDirection):new Vector3(1,.65,1))).normalize();
  if(view==='top')camera.up.set(0,0,-1);else camera.up.set(0,1,0);
  const right=new Vector3().crossVectors(camera.up,direction).normalize();
  const up=new Vector3().crossVectors(direction,right).normalize();
  const margin=1.14,diagonal=Math.max(size.length(),.001);
  let halfWidth=0,halfHeight=0,distance=diagonal*2;
  const corners:Vector3[]=[];
  for(const x of [bounds.min.x,bounds.max.x])for(const y of [bounds.min.y,bounds.max.y])for(const z of [bounds.min.z,bounds.max.z]){
    const point=new Vector3(x,y,z).sub(center);corners.push(point);
    halfWidth=Math.max(halfWidth,Math.abs(point.dot(right)));
    halfHeight=Math.max(halfHeight,Math.abs(point.dot(up)));
  }
  if(camera instanceof OrthographicCamera){
    camera.zoom=Math.min((camera.right-camera.left)/(2*Math.max(halfWidth,.0005)*margin),(camera.top-camera.bottom)/(2*Math.max(halfHeight,.0005)*margin));
  }else if(camera instanceof PerspectiveCamera){
    camera.aspect=viewport.width/viewport.height;camera.zoom=1;
    const tanV=Math.tan(camera.fov*Math.PI/360),tanH=tanV*camera.aspect;
    distance=Math.max(...corners.map(point=>point.dot(direction)+margin*Math.max(Math.abs(point.dot(right))/tanH,Math.abs(point.dot(up))/tanV)),diagonal*.5);
  }
  camera.position.copy(center).addScaledVector(direction,distance);
  if(camera instanceof PerspectiveCamera || camera instanceof OrthographicCamera){
    camera.near=Math.min(camera.near,diagonal/100);
    camera.far=Math.max(camera.far,distance+diagonal*2);
    camera.updateProjectionMatrix();
  }
  camera.lookAt(center);camera.updateMatrixWorld(true);
  return center;
}