#pragma once
#include <ESPAsyncWebServer.h>
#include "storage.h"

// One large file in flight, up to three paused requests without file/TCP buffers.
void serveAsset(AsyncWebServerRequest *request,const WebAsset &asset);
void tickAssets(uint32_t now);
void assetStatus(JsonObject out);
