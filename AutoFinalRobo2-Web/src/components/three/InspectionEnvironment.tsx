/** AFR2-20261101-03. Inspection environment using the simulator's canonical
 * sky/ground/grid tokens. Original model authors/materials remain unchanged. */
import { useEffect, useState } from 'react';
import { useTheme } from '../../app/providers/ThemeProvider';

function readPalette() {
  const css=getComputedStyle(document.documentElement);
  const get=(key:string)=>css.getPropertyValue(key).trim();
  return { sky:get('--sim-scene-sky'), floor:get('--sim-scene-ground'), grid:get('--sim-scene-grid'), reference:get('--sim-scene-ink') };
}
export function useInspectionPalette() {
  const { resolvedTheme }=useTheme();
  const [palette,setPalette]=useState(readPalette);
  useEffect(()=>{
    // ThemeProvider applies the root class in an effect; sample after that commit.
    const frame=requestAnimationFrame(()=>setPalette(readPalette()));
    return ()=>cancelAnimationFrame(frame);
  },[resolvedTheme]);
  return palette;
}
export function InspectionEnvironment({palette,extent}:{palette:ReturnType<typeof readPalette>;extent:number}){
  const groundSize=Math.max(4,extent*8),gridSize=Math.max(.25,extent*4);
  return <>
    <color attach="background" args={[palette.sky]} />
    <ambientLight intensity={1.4} />
    <directionalLight position={[3,5,2]} intensity={2} />
    <directionalLight position={[-3,2,-2]} intensity={.7} />
    <mesh rotation={[-Math.PI/2,0,0]} position={[0,-.003,0]}>
      <planeGeometry args={[groundSize,groundSize]} />
      <meshBasicMaterial color={palette.floor} toneMapped={false} />
    </mesh>
    <gridHelper args={[gridSize,20,palette.grid,palette.grid]} />
  </>;
}