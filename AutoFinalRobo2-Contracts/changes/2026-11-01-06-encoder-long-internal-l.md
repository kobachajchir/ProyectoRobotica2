# AFR2-20261101-06 - Pulsación larga con L interna

01/11/2026. Corrección explícita del usuario: el símbolo de pulsación larga es
el encoder con unaL dentro, no con una barrita debajo. La primera variante
propuesta por el agente era incorrecta y se retira.

## Revisión y corrección

Se consultan bitmaps y usos en la referenciaF4/Web resguardada y fuentes locales
AutoMicro en D:/Microcontroladores y checkoutOneDrive. Se encuentra el bitmap
básico13×13, pero no un recurso separado de largo conL. No se modifica ninguna
fuente AutoMicro ni se afirma haber recuperado exactamente el bitmap original.

Nueva variante `input.encoder.long` centralizada en `assets/encoder-gestures.json`,
adaptada del bitmap básico conservando perímetro y dimensiones. L3×5 oscura en
su centro claro según indicación humana, sin marca exterior. Procedencia y
limitación registradas. No altera el recurso original copiado de AutoMicro.

EncoderHint largo pasa a usar esta variante y conserva flecha, gap4px y centro
vertical. Corto conserva icono básico+OK. Se actualiza la especificación y
retira `hold-underline` del registro vigente. No cambian acciones, navegación,
umbral o QR codificado por esta corrección de representación.

Se regeneran atlas, dashboards, cards y lámina de gestos con el recurso nuevo.
Se regenera la composición del ejemploQR usando el mismo payloadAFR2:DEMO;
no se realiza comparación de bytes ni prueba de decodificación. Cambio de
representación del retorno, no una revisión de escaneabilidad.

## Estado por módulo

- shared/Contracts: recurso, generador, skill y artefactos documentales.
- atmega/esp/web: sin cambios de código/recursos activos por esta ficha.
- AutoMicro: fuentes consultadas solo en lectura.
- CAD/Loader: sin cambios.
- Skill personal sincronizada; sin commit/push ni promociónESP.

Sin tests, builds, validadores, inspección de las salidas generadas, navegador,
decoder, teléfono, puertos o flasheos. La revisión de fuente y generación de
artefactos no acreditan coincidencia pixel a pixel con un original no localizado.

[Recurso](../.agents/skills/oled128-ui-design/assets/encoder-gestures.json) ·
[Gestos](../.agents/skills/oled128-ui-design/assets/encoder-gestures-proposal.png).
