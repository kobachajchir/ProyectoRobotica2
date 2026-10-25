# Compilación ESP/Web — 23/10/2026

AFR2-20261023-15. Ambas compilaciones terminaron con salida 0 después de corregir las advertencias C++ y el error de empaquetado EPERM de Windows. Fuente AVR no tocada ni compilada.

- [ESP, primer intento](esp-attempt-1.txt): compiló, con seis advertencias Wclass-memaccess.
- [Web, primer intento](web-attempt-1.txt): TypeScript/Vite OK; postbuild falló en rename EPERM.
- [ESP, final](esp-final.txt): recompilación de las fuentes corregidas, SUCCESS y cero warnings.
- [Web, final](web-final.txt): TypeScript, Vite y empaquetado completados; fallback de publicación por archivos ejecutado.
- [Resultados JSON](results.json): seis assets descomprimidos y cotejados por tamaños y SHA-256; hash del BIN y fuentes capturadas después de build.

RAM estática ESP 39.980 bytes; programa 385.481 bytes; BIN 389.632 bytes. El perfil candidato permite 81.920 bytes RAM y 1.044.464 bytes de programa: no es identificación de flash física ni comprobación de heap/pila. Paquete Web de seis archivos, 2.925.630 bytes gzip; creado en Web/artifacts/littlefs/dist. ESP/data/dist conserva la versión anterior.

No se ejecutó buildfs/upload ni se abrió hardware. La compilación actual no valida compatibilidad con el protocolo ATmega que el usuario entregará; seguir con su documentación antes de modificar ese enlace.
