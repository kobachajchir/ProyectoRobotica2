# AFR2-20261023-09 — Implementación de la aplicación Web, cliente de protocolo, réplica OLED 128×64 y Sonner

- **ID**: `AFR2-20261023-09`
- **Fecha**: `2026-10-23`
- **Módulos afectados**: `AutoFinalRobo2-Contracts`, `AutoFinalRobo2-Web`
- **Estado por módulo**:
  - `atmega`: `partial-source` (sin cambios de código C; sus 5 fuentes C fueron validadas con `gcc -fsyntax-only -std=gnu11 -Wall -Wextra` con `0` advertencias/errores, y replicadas bit a bit en el visor Web)
  - `esp`: `not-implemented` (esqueleto sin cambios de código; contrato WS y `data/dist` con `manifest.json` de LittleFS generados y promovidos)
  - `web`: `built-and-packaged` (`npm install`, `tsc -b`, `vite build` con `viteSingleFile`, `package-web.mjs` y `copy-web-to-esp.mjs` ejecutados con `0` errores por pedido expreso del usuario)

---

## 1. Resumen de cambios en `AutoFinalRobo2-Contracts`

1. **`contracts/PROTOCOL.md`**:
   - Formalizado el sobre JSON WebSocket `afr2-ws-1` (`req`, `res`, `err`, `evt`), los tres estados independientes (`wsState`, `megaLinkState`, `controlLease`), el handshake `system.hello` (`5000 ms`) con discriminador `"AutoFinalRobo2"`, el latido `system.alive` (`2000 ms` / `6000 ms`), la prioridad de `robot.stop`, la política `single-latest-slot` para `robot.drive` y el servido HTTP por `HashRouter` desde LittleFS.
2. **`contracts/commands.json` (`afr2-command-draft-2`)**:
   - Especificados los 19 comandos (`system.hello`, `system.alive`, `control.acquire`, `control.release`, `robot.stop`, `robot.state`, `robot.drive`, `robot.mode`, `robot.test.wheel`, `robot.sensors`, `robot.encoders`, `robot.calibrate`, `robot.goto.start`, `robot.goto.cancel`, `robot.screen.get`, `robot.ui.action`, `wifi.status`, `wifi.scan`, `wifi.configure`) con esquemas de payload, timeouts, idempotencia, requisitos de lease y `module_status` real.
3. **`contracts/capabilities.json` (`afr2-capabilities-design-2`)**:
   - Desglosadas las capacidades por subsistema (`uart_bridge`, `mecanum_drive`, `wheel_odometry`, `goto_relative`, `line_array_5ch`, `ultrasonic_hcsr04`, `oled_128x64`, `ui_encoder_sw`) separando `present`, `implemented`, `calibrated`, `valid` y `freshness`.
4. **`documentation/screens.json` (`afr2-screens-2`)**:
   - Vinculadas las 4 pantallas locales (`0x010001`, `0x010002`, `0x020001`, `0x020002`) con su renderer Web bit a bit (`web:src/features/oled/oledRasterizer.ts`).

---

## 2. Resumen de cambios en `AutoFinalRobo2-Web`

1. **Configuración y activos (`package.json`, `tsconfig*.json`, `vite.config.ts`, `index.html`, `public/`)**:
   - Migración del esqueleto JS a TypeScript con React 19, `react-router-dom`, `@tailwindcss/vite`, `sonner@^2.0.8`, `three`, `@react-three/fiber` y `@react-three/drei`.
   - Incorporación de `public/brand/Logo.png` (copiado de `D:/ProyectoRobotica2/assets/Logo.png`) y activos locales 3D (`public/draco/*`, `public/models/auto_micro.glb`).
2. **Sistema de Identidad Visual `v0.1.3` y `Sonner`**:
   - `src/styles/tokens.css` y `src/styles/index.css`: 107 tokens, 6 densidades de vidrio, acento carmesí/rubí (`#b81d33` / `#ff6b7d`), superficie de emergencia bermellón (`#c93b1b` / `#ff8a65`), reglas `mobile-native` (`100dvh`, `touch-action: manipulation`, `@media (hover: hover) and (pointer: fine)`) y fallbacks de accesibilidad.
   - `src/components/notifications/notify.tsx`: `<Afr2Toaster />` único en raíz, posición adaptativa (`top-right` escritorio / `top-center` móvil para no tapar el botón `STOP` inferior), botón explícito de cierre (`toast.dismiss(id)`) e ID estable `afr2-connection`.
3. **Cliente de protocolo y 7 vistas**:
   - Implementados `wsEnvelope.ts`, `afr2Client.ts`, `ConnectionContext.tsx`, `RouteNav.tsx`, `HomePage.tsx`, `ControlPage.tsx`, `SensorsPage.tsx`, `WifiPage.tsx`, `OledPage.tsx` (`font7x10.ts`, `oledRasterizer.ts`, `OledCanvas128x64.tsx`, `EncoderControl.tsx`), `Model3DPage.tsx` y `DocsPage.tsx`.
4. **Herramientas de empaquetado LittleFS**:
   - Actualizado `tools/package-web.mjs` y creado `tools/copy-web-to-esp.mjs`.
