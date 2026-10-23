# Backend TWI AVR pendiente

[Maestro](../../../INFORME_MAESTRO.md) · [Informe Atmega](../../INFORME_ADAPTACION.md).

Implementar Afr2I2cHardware.start/poll/abort para registros TWBR/TWSR/TWCR/TWDR. El Manager preparado acepta dirección 7 bits; desplazar solo al construir SLA+W. start debe aceptar y conservar el buffer o devolver BUSY sin retenerlo; nunca bloquear. IRQ TWI avanza START/SLA/DATA/STOP y publica resultado. poll devuelve OK solo tras completar STOP; NACK/bus error son ERROR. abort desactiva transferencia/IRQ antes de liberar el buffer y agenda recuperación acotada. No escribir OLED fuera del Manager.

El ejemplo de composición describe orden, pero no existe main ni reloj/GPIO/TWI productivo. Se eligió biblioteca parcial para no generar firmware aparentemente utilizable con drivers vacíos. No se ejecutó compilación.
