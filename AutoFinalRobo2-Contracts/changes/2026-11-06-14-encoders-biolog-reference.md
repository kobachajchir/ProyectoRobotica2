# AFR2-20261106-14 — Referencia de encoders BioLog y cotejo estático

Pedido: incorporar el PDF aportado y contrastarlo con nuestro código.

Se conserva el [original](../hardware/supplied/Pinout_encoders_AGV_BioLog.pdf)
y se agrega el [cotejo por pin y comportamiento](../hardware/COTEJO_ENCODERS_BIOLOG_20261106.md).
Cableado A/B y alimentación documental coincidentes. Diferencias: BioLog x4
frente a AFR2 x1, orden de ruedas, CPR1500 frente a calibración individual,
signos/configuración propios y diámetro80,59 frente a80mm nominales.
No se adoptan constantes ni scripts del documento.

ATmega/ESP/Web: sin cambios en fuentes, artefactos, wire ni persistencia.
CAD/Loader sin cambios. Sin tests, compilaciones, validadores, COM, cargas ni
pruebas físicas. Revisión por lectura de fuente y de ambas páginas del PDF.

Archivos y procedencia: [registro](../provenance/encoders-biolog-20261106-14.json).
