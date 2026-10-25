# AFR2-20261025-01 — Control Web directo

25/10/2026. El usuario pide quitar referencias a un control externo y confirma
expresamente eliminar también el permiso interno Web. La primera revisión de
terminología se amplía a la eliminación funcional; el estado final es EX01v7.

Web elimina estado/métodos/errores de autorización y envía las acciones directas.
Modo/prueba/configuración/secuencias dejan de depender de adquisición previa.
Se mantienen límites, consultas de salidas, invalidación STOP/foco/tarea y
prioridad de parada. Actividad imperativa se registra antes de una espera para
que salir/perder foco no dependa de un render posterior. No nuevos componentes.

ESP elimina concesión/renovación/liberación, tokens de acceso y sus respuestas.
Registra presencia interactiva Web y la retransmite como edad de actividad por
WEB_PULSE, sin autorizar ni iniciar salidas. Desconexión invalida trabajos y pide
STOP. ATmega elimina Acquire, bandera de acceso y comprobaciones de token;
aplica acciones directas y cancela una operación Web al caducar la presencia.
Correlación UART/programa/revisión/generación se conserva, separada de derechos.

OLED elimina tres opciones; acciones2/3/56 retiradas sin reasignación. Catálogo
y copia Web ajustan rangos, inserción Reinicio y counts4/7 con59identidades.
Se revisa hash del catálogo después de portar el índice C/TS y se regeneran los
recursos; no se cambian primitivas de dibujo o recursos gráficos. Textos activos
y documentos vigentes normalizados; referencias/archivos de evidencia históricos
se conservan. Árbol completo y navegación giro/corto/largo/Volver conservados,
salvo las opciones retiradas por el nuevo pedido.

Firmware0.9.0 y Web0.2.0: los tres módulos compilan; paquete local actualizado.
Perfiles anteriores no habilitan servicios de robot en ESPv7. Gates de memoria
y Hall0 siguen vigentes. Sin tests/validadores independientes, navegador,
flasheos, serie, pruebas físicas, CAD o publicación.

[Contrato7](../contracts/DIRECT_CONTROL_7.md),
[evidencia](../documentation/builds/2026-10-25-01/README.md),
[procedencia](../provenance/direct-web-control-20261025.json).
