const fs=require('fs'),vm=require('vm'),assert=require('assert'),path=require('path');
const root=process.cwd();const ts=require(path.join(root,'AutoFinalRobo2-Web/node_modules/typescript'));
const loaded={};const intervals=new Map();let clockId=0;
class FakeSocket {
 static OPEN=1;static CONNECTING=0; static latest;
 constructor(){this.readyState=1;this.sent=[];FakeSocket.latest=this;}
 send(text){this.sent.push(JSON.parse(text));}
 close(){this.readyState=3;}
 deliver(frame){this.onmessage?.({data:JSON.stringify({schema:'afr2-ws-1',ts:5000,...frame})});}
}
const globals={WebSocket:FakeSocket,window:{location:{protocol:'http:',host:'robot.local'}},document:{visibilityState:'visible',hasFocus:()=>true},Date,console,Map,Set,Promise,Error,setTimeout:()=>++clockId,clearTimeout:()=>{},setInterval:fn=>{const id=++clockId;intervals.set(id,fn);return id;},clearInterval:id=>intervals.delete(id)};
function load(file){
 if(loaded[file])return loaded[file].exports;
 const module={exports:{}};loaded[file]=module;
 if(file.endsWith('.json')){module.exports=JSON.parse(fs.readFileSync(file,'utf8'));return module.exports;}
 const code=ts.transpileModule(fs.readFileSync(file,'utf8'),{compilerOptions:{module:ts.ModuleKind.CommonJS,target:ts.ScriptTarget.ES2022,esModuleInterop:true}}).outputText;
 const req=name=>load(path.resolve(path.dirname(file),name)+(name.endsWith('.json')?'':'.ts'));
 vm.runInNewContext('(function(require,module,exports){'+code+'\n})',{...globals},{filename:file})(req,module,module.exports);
 return module.exports;
}
(async()=>{
 const {Afr2Client}=load(path.join(root,'AutoFinalRobo2-Web/src/protocol/afr2Client.ts'));
 const client=new Afr2Client();client.connect();let ws=FakeSocket.latest;ws.onopen();
 const hello=ws.sent.at(-1);assert.equal(hello.cmd,'system.hello');
 ws.deliver({type:'res',requestId:hello.requestId,cmd:'system.hello',ok:true,status:'completed',payload:{product:'AutoFinalRobo2',controlMode:'direct',megaLink:'ready',capabilities:{manual_pwm:{implemented:true,limits:{maxInputPct:100}}}}});
 await new Promise(setImmediate);assert.equal(client.getState().wsState,'ready');
 for(const fn of intervals.values())fn();const alive=ws.sent.at(-1);assert.equal(alive.cmd,'system.alive');
 ws.deliver({type:'res',requestId:alive.requestId,cmd:'system.alive',ok:true,status:'completed',payload:{megaLink:'ready',brokerTs:5000}});
 await new Promise(setImmediate);assert.equal(client.getState().capabilities.manual_pwm.implemented,true);
 ws.deliver({type:'evt',event:'system.status',payload:{megaLink:'ready',connectionDiagnostics:{freeHeapBytes:20000}}});
 assert.equal(client.getState().capabilities.manual_pwm.implemented,true);
 ws.deliver({type:'evt',event:'system.status',payload:{megaLink:'ready',capabilities:{manual_pwm:{implemented:false}}}});
 assert.equal(client.getState().capabilities.manual_pwm.implemented,false);
 ws.deliver({type:'evt',event:'system.status',payload:{megaLink:'ready',capabilities:{manual_pwm:{implemented:true}}}});
 client.connect();assert.notEqual(client.getState().capabilities.manual_pwm?.implemented,true);
 client.disconnect();assert.equal(intervals.size,0);
 console.log('PASS: full hello, lightweight heartbeat, partial status, permission revocation and reconnect');
})().catch(e=>{console.error(e);process.exitCode=1;});
