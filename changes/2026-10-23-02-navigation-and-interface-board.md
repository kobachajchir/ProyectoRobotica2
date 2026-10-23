# AFR2-20261023-02 - contexto, navegación objetivo y PCB de interfaz

Fecha 2026-10-23. Cambio documental de requisitos/diseño, no wire ni firmware.

El usuario confirma cabeceras SDA/SCL ya expuestas por shield; se conserva equivalencia Mega D20/D21. Se sustituye propuesta anterior de cabecera única por J1 señales 1x5 (CLK D40, DT D41, SW D42, SDA, SCL) y J2 VCC/GND comunes 1x2; paso 2,54 mm. Orden pin1/huellas/tensión/dimensiones por confirmar al diseñar.

Árbol completo propuesto en documentation/MENU_PLAN.md. Navegación acordada permanece: giro, corto confirmar, largo menú, atrás por ítem; no botón separado. Nuevas vistas/acciones aún sin IDs ni handlers. screens.json mantiene solo las cuatro vistas actuales.

CAD futuro: PCB, plano y soporte3D con dos tornillos/tuercas que fijan conjunto a cubierta; diferido hasta planos/skill de ruteo del usuario. No se creó un repo de hardware.

Productores/consumidores: atmega/esp/web pendientes de implementar árbol/capabilities y reportes. No se cambiaron framing, payloads, IDs, ACK, timeouts ni código. Shared actualizado con enlaces y decisiones. Sin tests/build/validadores/flash/CI. Commit solo local, remoto no configurado; no push. AutoMicro permanece intacto.
