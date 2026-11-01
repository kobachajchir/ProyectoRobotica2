# Asistente de una vuelta Hall — AFR2-20261029-20

## Recorrido local

Pruebas → Hall una vuelta conserva selector de rueda, Estado / cuentas,
Iniciar conteo, Guardar medición, Descartar y Volver. Finalizar vuelta deja de
ser una opción visible; su acción interna16/17 se conserva para el asistente.

Iniciar conteo abre0x030036, selección numerada1FL,2FR,3RL,4RR. Giro cambia
la selección; corto confirma rueda e inicia conteo, abriendo0x030037. Pantalla
indica Gire una vuelta, rueda, pulsos firmados, Click: finalizar y Largo: cancelar.
Girar encoder en esta fase no cambia rueda ni ejecuta motores. La rueda se
gira manualmente. Corto finaliza y valida según el servicio Hall existente:
delta no nulo, sin inversión de sentido durante la medición, máximo1000000.
Retorna al menú contextual y notifica Operación/Completada o Medición inválida.
No se acredita una vuelta física por software: la confirma el operador.

Largo en selector/conteo cancela el intento actual, retorna al menú Hall y
notifica Operación/Cancelada. No borra una medición anterior pendiente de esa
rueda ni de las demás. Su liberación no confirma por el decoder corto/largo
existente. Es excepción contextual solicitada al largo que abre principal.

## Mediciones y persistencia

Cada rueda conserva su último resultado válido pendiente en RAM. Una nueva
medición válida reemplaza solo su rueda. Estado / cuentas0x030015 muestra las
cuatro CPR (pulsos por vuelta, x1 flanco ascendenteA) en orden FL/FR/RL/RR;
asterisco indica pendiente y -- ausencia de CPR calibrado. No son las cuentas
acumuladas de movimiento. Lecturas instantáneas restantes siguen por wire.
Guardar medición aplica todas las pendientes, corrige signo cuando el delta
es negativo respecto de la polaridad actual e inicia el journal existente.
Se conservan drafts hasta finalizar commit EEPROM; entonces se limpia máscara
y aparece Guardadas. No se informa persistencia completada al solo aceptarla.
No cambia formato EEPROMv4 ni calibraciones de ruedas que no fueron medidas.

Descartar borra los drafts Hall de todas las ruedas sin alterar los CPR guardados.
Volver desde Hall con drafts abre diálogo obligatorio0x030038: Si/No, giro
selecciona y corto confirma, sin Volver. Si inicia guardado y espera commit antes
de salir; No descarta drafts y sale. Largo no evade esa elección. Una salida
con largo hacia principal desde menú/estado Hall también pregunta si hay drafts.
Sin drafts, Volver conserva retorno contextual y largo abre principal.

## Diálogo reutilizable

Afr2Ui_ConfirmSave(ui,save_action,discard_action,target) recibe acciones de
servicio y destino contextual; no conoce datos de calibración. Si un servicio
falla, conserva elección/datos y muestra aviso. La acción de guardado debe
iniciar una escritura asíncrona; Afr2Ui_SaveCompleted(ui) solo se llama al
commit confirmado. confirm_wait muestra Guardando... y bloquea acciones hasta
terminar. Un futuro consumidor debe conectar esa señal de finalización y su
semántica de descarte, sin asumir guardado síncrono. Uso inicial: Hall.

## Identidad y consumidores

0x030036 selección,0x030037 conteo y0x030038 diálogo reutilizable se añaden sin
cambiar IDs anteriores.0x030014 menú y0x030015 estado cambian presentación.
Catálogo Contracts y renderer C/TypeScript vinculados, recursos de tipografía
existentes; sin nuevos iconos. Snapshot local incorpora hall_cpr[4]/hall_pending
y líneas preparadas de conteo. No cambia framebuffer físico1024bytes ni wire.
No se añade operación remota capaz de confirmar el asistente: UI_ACTION y
presentación táctil no interfieren en modales locales. La biblioteca Web puede
mostrar diseños y selector; no fabrica pulsos ni ejecuta guardar/cancelar.

Estado real por módulo y límites de memoria/build/carga en
[ficha20](../changes/2026-10-29-20-hall-wizard-save-confirm.md).
