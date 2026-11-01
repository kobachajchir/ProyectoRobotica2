# AFR2-20261029-25 — Divisor sin diodo en toma conmutada

29/10/2026. Koba Chajchir confirma toma del voltímetro sin tensión sin batería ni durante programación y llave de corte de batería. Se propone conectar el divisor a esa toma después de la llave, sin diodo. Evidencia comunicada por el usuario, sin medición del agente.

Adaptación propia por Codex: `hardware/MEDICION_BATERIA.md` encabeza la propuesta vigente 10k/10k, Vpin = Vbat/2, consumo/disipación y capacitor 100 nF entre nodo de sensado y GND, en paralelo con resistencia inferior. Explica filtro RC y conexión física. Se conservan revisiones23/24 como antecedentes. Esta ficha registra el cambio y README/CHANGELOG agregan enlace; autorías previas conservadas.

ATmega/ESP/Web: sin cambios de código, wire, pantallas, imágenes o carga. Contracts: documentación de diseño. Sin tests, compilaciones, validadores, COM, flasheos ni pruebas físicas. No se certifican referencia, transitorios, memoria ni hardware.
