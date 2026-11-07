# Loader 1.2.0 — adaptación WinForms

AFR2-20261106-03. Koba Chajchir / AFR2; adaptación mediante Codex.

Modo APLICAR. Fuente canónica: manual e índice de esta carpeta (0.1.11),
con tokens de AutoFinalRobo2-Web/src/styles/tokens.css. Objetivo: Loader.
La extensión a controles WinForms es una propuesta de implementación dentro
del pedido humano; no redefine la identidad global. Se conserva el logo azul
AFR2 indicado por el usuario y los recursos UNER.

Reglas aplicadas: superficies sólidas; tipografía Segoe UI; acento carmesí
para navegación; verde para compilar/cargar; rojo para avisos. Foco por teclado
visible, respuesta inmediata y sin animación ornamental durante operaciones.
Claro/Oscuro/Sistema se guardan en preferencias locales y se conservan al
guardar las otras preferencias. Sistema atiende cambios de Windows.

| Before | After | Why |
|---|---|---|
| Colores fijos | Paleta central clara y oscura | Contraste consistente |
| Icono del desplegable pegado al borde | Icono a 16 px y texto a 48 px | Separación legible |
| Texto seleccionado en aviso KEY/RST | Instrucciones sin selección; foco en casilla vacía | Espacio marca sin confirmar automáticamente |

Cobertura visual real: capturas de Operación, Flasher y Monitor en oscuro;
Operación, Conversor y Configuración en claro, y cambio Claro→Oscuro en
Configuración. La primera pasada encontró texto claro sobre fondo claro al
cambiar de tema; se corrigió registrando los colores originales antes de
modificar el árbol. Se corrigieron además enlaces del grid en oscuro.
Desplegables y barras de desplazamiento conservan partes nativas de Windows.
El manual HTML tiene su propia superficie clara de lectura.

La revisión no abrió puertos ni ejecutó comandos de firmware. No se provocó
el aviso físico mediante Detectar ESP para evitar iniciar una operación real;
el cambio de foco se revisó en fuente. No se afirma validación de hardware,
todos los escalados DPI o una segunda computadora.

Skills consultadas: apple-design, emil-design-eng y visual-identity-system.
Se aplican jerarquía, espaciado, contraste y teclado a una utilidad nativa.
