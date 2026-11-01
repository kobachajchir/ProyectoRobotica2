# AFR2-20261031-11 — Titulo centrado bajo el auto

Pedido humano: centrar «Final Robotica 2» debajo del auto del logo. El centro
entre las ruedas del PNG vigente esta al63.5% del ancho (bordes547/1292px
en la franja inferior). Se usa ese anclaje para centrar el conjunto de bienvenida
y el titulo; el brazo extendido queda hacia la izquierda. El escalado de entrada
y salida conserva ese mismo origen visual.

El ancho del logo tambien respeta el limite de altura para que object-fit no
introduzca margenes laterales que desplacen el anclaje. El titulo ocupa el73%
derecho del ancho, con centro al63.5%, manteniendo separacion bajo las ruedas.
Solo CSS/Web fuente; sin tests, build, validadores ni navegador. Los demas
modulos y paquetesESP conservan su estado previo.
