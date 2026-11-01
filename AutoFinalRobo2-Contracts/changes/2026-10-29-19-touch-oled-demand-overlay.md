> **AFR2-20261029-19 compilado y ATmega cargado:** pedido humano29/10/2026. ATmega/ESP/Web esp y LittleFS completados; promoción local Web a ESP/data/dist. Carga COM8 final13:32:50,80066bytes escritos/verificados, avrdude exit0. ESP/LittleFS sin cargar. SRAM estática7085bytes; sin prueba física/dinámica. Evidencia en AutoFinalRobo2-Contracts/documentation/builds/2026-10-29-19/README.md. Estados inferiores sin build/carga son históricos.

# AFR2-20261029-19 — Overlay, Volver, OLED táctil y visor bajo demanda

Pedido humano: velar/difuminar fondo de Configuración y apariencia, Red y Pantalla en vivo; Volver bajo controles de menú; aviso/chevrones/bitmap orientado en OLED durante control táctil; consultar pantallas solo con un visor abierto.

Fuente implementada en Web, ATmega y broker ESP. [Contrato completo](../contracts/TOUCH_OLED_19.md), [procedencia por archivo](../provenance/touch-oled-20261029-19.json). ScreenCode0x030035; comando0x24/feature0x20000 y acciónVolver3. No se crean repositorios ni cambios en referencias.

- Web: FloatingModal con scrim+blur compartido; Volver en menús conectados y biblioteca. Contador de visores/suscripción explícita y eliminación del polling global de metadatos. Control táctil publica solo el estado visual último y libera al salir.
- ATmega: propiedad OLED con aviso, bitmap propio PROGMEM, orientación Q8 y chevrones; banda parcial64x40 y transporte diferencial existente. Toma manual cancela misiones/radar; cierre o caducidad restaura contexto. Sin yaw físico añadido.
- ESP: gate por sesión con caducidad3500ms; no envía capturas/metadatos sin suscripción. Traduce comando táctil/Volver y comprueba feature/rangos/ACK.

Estado de los tres módulos: solo fuente; sin tests, compilación, validadores, navegador, flasheo ni ensayo físico. No se ejecutó generador OLED ni empaquetado/promoción ESP. No se certifican SRAM/pila/flash, fidelidad visual, latencia ni hardware. Se preservan ediciones previas/concurrentes, incluida ficha20 de Hall/confirmación, que no se atribuye a esta entrega.
