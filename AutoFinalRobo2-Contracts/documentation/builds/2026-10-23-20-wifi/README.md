# Build del flujo Wi-Fi ESP/Web

AFR2-20261023-20. [ESP](esp-build.txt): PlatformIO esp12s_candidate SUCCESS, exit 0, sin warnings/errors; RAM estática 40.272 bytes, programa 386.797 bytes según perfil candidato. [Web](web-build.txt): TypeScript/Vite/postbuild exit 0, seis assets, 2.855.479 bytes gzip en Web/artifacts/littlefs/dist. Incluye las fuentes visuales vigentes del trabajo paralelo.

No hubo pruebas de radio/navegador, puertos, flasheo, imagen FS ni build AVR. La aplicación con hardware y persistencia interrumpida siguen pendientes. ESP/data/dist no se actualizó. Se editaron exclusivamente servicio Wi-Fi ESP, contexto/formulario/tipos Wi-Fi Web y documentación Contracts; no enlace Mega.
