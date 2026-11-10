# AFR2-20261110-11 · Secuencia y simulador en un documento

Versión: 1.19.0 (subversión).

Pedido: integrar Añadir bloque de movimiento al auto con la simulación, editar pistas, sincronizarlas y ordenar la interfaz.

La vista previa recibe directamente el snapshot calculado por SequenceEditorPanel: se retira la copia de siete estados y el efecto de transferencia en ControlPage. Dibuja también la ruta completa del documento activo mientras está en pausa; seleccionar un bloque lleva el cursor a su inicio. Una misma normalización alimenta simulación, cuatro ruedas y programa físico existente.

La paleta de movimientos y las propiedades elegidas quedan junto a la escena. Añadir un movimiento en modo ruedas deriva sus cuatro consignas y agrega las esperas necesarias para iniciar al final común; incluye las cuatro ruedas y conserva las pistas activas previas. Editar rueda permite pasar directamente desde una pista derivada al modo independiente. Al volver al chasis se pide recuperar explícitamente su borrador, con Deshacer; no se presenta una conversión inversa con pérdidas como equivalente.

La vinculación de duración (opcional, activada inicialmente) actualiza bloques temporales de pistas activas que empiezan en el mismo instante. No cambia distancias Hall ni sincroniza bloques por su índice arbitrario. Igualar finales agrega esperas de cero velocidad, sin estirar movimientos ni cambiar velocidades. Los límites de bloques se respetan sin aplicar una operación parcial.

TimelineRow usa anchos proporcionales sobre un lienzo común, regla de tiempo y scroll horizontal compartido. Se retiran los mínimos porcentuales que deformaban la correspondencia temporal. Las secuencias por distancia conservan su estimación local; la finalización física sigue gobernada por Hall y los comandos vigentes. No hay nuevos comandos ni cambio de esquema JSON.

Fuentes: Web/src/features/control/{ControlPage,ControlMap,TopMap,ControlMapScene}.tsx, controlMapTypes.ts, sequences/{SequenceEditorPanel,TimelineRow}.tsx y styles/control-map.css. Solo fuente; sin tests, compilaciones, validaciones, navegador, paquetes ni cargas. ATmega/ESP solo versión compartida por esta ficha. No se afirma ejecución visual o física. Sin commit/push.
