from pathlib import Path
import os,subprocess
root=Path.cwd();tmp=Path(os.environ['TEMP'])/'afr2-memory-20261109-09';stub=tmp/'http-stubs';stub.mkdir(exist_ok=True)
(stub/'Arduino.h').write_text('''#pragma once
#include <cstdint>
#include <string>
#include <cstddef>
using String=std::string;
extern uint32_t clockNow;
inline uint32_t millis(){return clockNow;}
struct EspMock {uint32_t heap=20000,block=18000;uint32_t getFreeHeap(){return heap;}uint32_t getMaxFreeBlockSize(){return block;}};
extern EspMock ESP;
''')
(stub/'ArduinoJson.h').write_text('''#pragma once
struct JsonCell {template<class T> JsonCell &operator=(T){return *this;}};
struct JsonObject {JsonCell operator[](const char*){return {};}};
struct JsonDocument {};
''')
(stub/'LittleFS.h').write_text('#pragma once\ninline int LittleFS=1;\n')
(stub/'ESPAsyncWebServer.h').write_text('''#pragma once
#include "Arduino.h"
#include <memory>
#include <functional>
#include <map>
struct AsyncWebServerResponse {int status;std::map<String,String> headers; explicit AsyncWebServerResponse(int s):status(s){};void addHeader(const String &key,const String &value){headers[key]=value;}};
class AsyncWebServerRequest;
using AsyncWebServerRequestPtr=std::weak_ptr<AsyncWebServerRequest>;
extern int fileOpens;
class AsyncWebServerRequest:public std::enable_shared_from_this<AsyncWebServerRequest> {
public:
 std::unique_ptr<AsyncWebServerResponse> response;std::function<void()> disconnected;
 ~AsyncWebServerRequest(){if(disconnected)disconnected();}
 AsyncWebServerResponse *beginResponse(int,const String&,const String&){return new AsyncWebServerResponse(503);}
 AsyncWebServerResponse *beginResponse(int,const String&,const String&,bool){return nullptr;}
 AsyncWebServerResponse *beginResponse(int fs,const String &path,const String &mime){++fileOpens;return new AsyncWebServerResponse(200);}
 void send(AsyncWebServerResponse *r){response.reset(r);}
 AsyncWebServerRequestPtr pause(){return weak_from_this();}
 void onDisconnect(std::function<void()> cb){disconnected=cb;}
 void disconnect(){if(disconnected){auto cb=std::move(disconnected);disconnected={};cb();}}
};
'''.replace(' AsyncWebServerResponse *beginResponse(int,const String&,const String&){return new AsyncWebServerResponse(503);}', ' AsyncWebServerResponse *beginResponse(int code,const char*,const char*){return new AsyncWebServerResponse(code);}'))
source=tmp/'check-http.cpp'
source.write_text('''#include "http_assets.h"
#include <cassert>
#include <iostream>
uint32_t clockNow=1;EspMock ESP;int fileOpens=0;
int main(){
 WebAsset large{"/model.glb","/dist/model.glb.gz","model/gltf-binary","hash",200000};
 WebAsset small{"/small.js","/dist/small.js.gz","text/javascript","hash",1000};
 auto a=std::make_shared<AsyncWebServerRequest>();auto b=std::make_shared<AsyncWebServerRequest>();auto c=std::make_shared<AsyncWebServerRequest>();
 serveAsset(a.get(),large);assert(fileOpens==1 && a->response->status==200);assert(a->response->headers["Content-Encoding"]=="gzip");
 serveAsset(b.get(),large);serveAsset(c.get(),large);assert(!b->response && !c->response && fileOpens==1);
 auto s=std::make_shared<AsyncWebServerRequest>();serveAsset(s.get(),small);assert(s->response->status==200 && fileOpens==2);
 b.reset();tickAssets(clockNow);assert(fileOpens==2);a->disconnect();tickAssets(clockNow);assert(fileOpens==3 && c->response->status==200);
 auto d=std::make_shared<AsyncWebServerRequest>();serveAsset(d.get(),large);clockNow+=30000;tickAssets(clockNow);assert(d->response->status==503 && d->response->headers["Retry-After"]=="1");
 c->disconnect();auto e=std::make_shared<AsyncWebServerRequest>();ESP.heap=7000;serveAsset(e.get(),large);assert(!e->response);ESP.heap=20000;tickAssets(clockNow);assert(e->response->status==200);e->disconnect();
 auto f=std::make_shared<AsyncWebServerRequest>();ESP.heap=7000;serveAsset(f.get(),large);auto g=std::make_shared<AsyncWebServerRequest>();auto h=std::make_shared<AsyncWebServerRequest>();auto i=std::make_shared<AsyncWebServerRequest>();serveAsset(g.get(),large);serveAsset(h.get(),large);serveAsset(i.get(),large);assert(i->response->status==503);f.reset();g.reset();h.reset();tickAssets(clockNow);ESP.heap=20000;
 std::cout<<"PASS: single large download, small-file bypass, queued cancellation, timeout, low-memory wait and admission limit\\n";
}
''')
compiler=Path('C:/Qt/Tools/mingw1310_64/bin/g++.exe');src=root/'AutoFinalRobo2-ESP/src'
subprocess.run([str(compiler),'-std=c++17','-I'+str(stub),'-I'+str(src),str(source),str(src/'http_assets.cpp'),'-o',str(tmp/'check-http.exe')],check=True)
env=dict(os.environ);env['PATH']=str(compiler.parent)+os.pathsep+env['PATH'];subprocess.run([str(tmp/'check-http.exe')],env=env,check=True)
