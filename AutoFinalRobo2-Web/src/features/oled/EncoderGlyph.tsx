import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
import { ENCODER_LONG } from './encoderLongIcon';
import { ENCODER_TURN } from './reformResources';
export function EncoderGlyph({kind='short',scale=2,color='currentColor',mirror=false}:{kind?:'short'|'long'|'turn';scale?:number;color?:string;mirror?:boolean}){
  const image=kind==='turn'?ENCODER_TURN:kind==='long'?ENCODER_LONG:L.icons[L.symbols.AFR2_ICON_ENCODER];
  const pixels=[];
  for(let y=0;y<image.height;y++)for(let x=0;x<image.width;x++)if(image.bytes[y*Math.ceil(image.width/8)+(x>>3)]&(1<<(x&7)))pixels.push(<rect key={`${x}:${y}`} x={x} y={y} width={1} height={1}/>);
  return <svg aria-hidden="true" width={image.width*scale} height={image.height*scale} viewBox={`0 0 ${image.width} ${image.height}`} fill={color} shapeRendering="crispEdges" style={{flexShrink:0,transform:mirror?'scaleX(-1)':undefined}}>{pixels}</svg>;
}
