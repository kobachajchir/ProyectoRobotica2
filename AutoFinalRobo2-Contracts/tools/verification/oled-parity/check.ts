// AFR2-20261106-02: same fixtures as frames.c; compares all 1024 bytes.
import {readFileSync,writeFileSync} from 'node:fs';
import {renderFirmwareScreen,firmwareNodeCount} from '../../../../AutoFinalRobo2-Web/src/features/oled/firmwareRenderer';
import {FIRMWARE_LIBRARY as L} from '../../../../AutoFinalRobo2-Web/src/features/oled/generated/firmwareLibrary';
import {renderQrPayload} from '../../../../AutoFinalRobo2-Web/src/features/oled/qrScreen';
import {Afr2OledBuffer} from '../../../../AutoFinalRobo2-Web/src/features/oled/oledRasterizer';
const base:any={quickFlags:0,qrAllowed:false,lines:['','',''],labels:['','',''],disabled:[0,0,0],sequenceState:0,sequenceCount:0,sequenceIndex:0,sequenceKind:0,sequenceDurationMs:0,sequenceElapsedMs:0,networkValid:false,networkSta:false,networkAp:false,networkSsid:'',networkApSsid:'',networkPassword:'',networkIp:'',networkApIp:''};
const raw=readFileSync(process.argv[2]),fail:any[]=[];let frame=0;
if(L.nodes.length!==70 || raw.length!==(70*3*10+4)*1024)throw new Error('Fixture/catalogue frame count changed; review both harnesses');
for(let scenario=0;scenario<10;scenario++)for(let variant=0;variant<3;variant++)for(let node=0;node<70;node++){
 const s:any={...base};
 if(scenario===1 || scenario===2)Object.assign(s,{networkTotal:16,networkCount:8,networkScanState:2,networkPage:scenario-1,networkTruncated:true,labels:['Red demo','Red demo','Red demo'],rowSecurity:[2,2,2]});
 if(scenario>=3 && scenario<=5)s.networkScanState=scenario===3?1:scenario===4?2:3;
 if(scenario===6)Object.assign(s,{hallPending:5,hallCpr:[100,101,102,103],irKind:1,irValid:true,lineValid:true,lineBits:21,irRaw:[0,1,0,1,0]});
 if(scenario===7)Object.assign(s,{networkValid:true,networkAp:true,networkMode:2,networkServer:true,qrAllowed:true,networkApSsid:'AFR2-demo',networkApIp:'192.0.2.1'});
 const selection=variant?firmwareNodeCount(node,false,s)-1:0;
 const u:any={node,selection,firstVisible:Math.max(0,selection-2),clockMs:1000,noticeStartedMs:0,scrollStartedMs:0,redraw:true,scrollRedraw:false,keyboard:false,keyboardField:false,key:0,length:0,ssid:'',edit:false,editingLabel:'',draft:0,notice:null,holdProgress:variant*8};
 if(scenario===8 || scenario===9){u.notice={kind:scenario===8?16+L.symbols.AFR2_NET_AP_READY:0,title:'Operacion',text:'Aceptada',ms:3000};u.noticeStartedMs=100;}
 const o=new Afr2OledBuffer();renderFirmwareScreen(u,o,s);
 const expected=raw.subarray(frame*1024,(frame+1)*1024);let bytes=0;for(let i=0;i<1024;i++)bytes+=Number(expected[i]!==o.buffer[i]);if(bytes)fail.push({scenario,node,variant,bytes});frame++;
}
for(const payload of ['http://192.0.2.1/','A'.repeat(220),'A'.repeat(300),'A'.repeat(353)]){const o=new Afr2OledBuffer();renderQrPayload(o,payload);const expected=raw.subarray(frame*1024,(frame+1)*1024);let bytes=0;for(let i=0;i<1024;i++)bytes+=Number(expected[i]!==o.buffer[i]);if(bytes)fail.push({payloadBytes:payload.length,bytes});frame++;}
writeFileSync(process.argv[3],JSON.stringify({frames:frame,passed:frame-fail.length,fail},null,2));console.log(JSON.stringify({frames:frame,passed:frame-fail.length,fail}));process.exitCode=fail.length?1:0;
