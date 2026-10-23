#include <Arduino.h>
// Esqueleto de composicion, aun sin red/bridge/FS configurados.
// No habilita firmware AT ni usa credenciales heredadas.
void setup() { Serial.begin(115200); Serial.setDebugOutput(false); }
void loop() { yield(); }
