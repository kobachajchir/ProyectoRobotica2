# Carpeta reference/web/src/assets

[Maestro](../../../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../../../INFORME_ADAPTACION.md).

Fuentes históricas de referencia: no forman parte del producto adaptado.

## Utilidad y reformas específicas

### [Recursos, estilos y empaquetado](../../../../docs/modules/assets.md)

Se conservan recursos necesarios para leer/reutilizar la UI anterior; el GLB es el robot previo y no representa Mecanum.

Reformas: Crear identidad/modelo del nuevo robot; retirar 3D pesado si excede LittleFS/bundle. El package activo es React/Vite mínimo, sin postbuild hacia otro repo. El empaquetador produce artifacts/littlefs/dist dentro de Web, no publica ni flashea. Versiones iniciales tomadas del manifest origen; falta instalar/fijar lock propio y revisar requisitos Node. No se ejecutó build/npm.

## Archivos vecinos

- [AutoLogo.svg](<AutoLogo.svg>)
- [github-mark.svg](<github-mark.svg>)
- [GithubSvg.svg](<GithubSvg.svg>)
- [Left_1.svg](<Left_1.svg>)
- [LogoControl.png](<LogoControl.png>)
- [LogoControl.webp](<LogoControl.webp>)
- [LogoControlV2.png](<LogoControlV2.png>)
- [LogoControlV2.webp](<LogoControlV2.webp>)
- [LogoDashboard.png](<LogoDashboard.png>)
- [LogoDashboard.webp](<LogoDashboard.webp>)
- [LogoDashboardV2.png](<LogoDashboardV2.png>)
- [LogoDashboardV2.webp](<LogoDashboardV2.webp>)
- [LogoMPUIR.png](<LogoMPUIR.png>)
- [LogoMPUIR.webp](<LogoMPUIR.webp>)
- [LogoMPUIRV2.png](<LogoMPUIRV2.png>)
- [LogoMPUIRV2.webp](<LogoMPUIRV2.webp>)
- [LogoSeguidor.png](<LogoSeguidor.png>)
- [LogoSeguidor.webp](<LogoSeguidor.webp>)
- [LogoSeguidorV2.png](<LogoSeguidorV2.png>)
- [LogoSeguidorV2.webp](<LogoSeguidorV2.webp>)
- [LogoWifiAP.png](<LogoWifiAP.png>)
- [LogoWifiAP.webp](<LogoWifiAP.webp>)
- [LogoWifiAPV2.png](<LogoWifiAPV2.png>)
- [LogoWifiAPV2.webp](<LogoWifiAPV2.webp>)
- [react.svg](<react.svg>)
- [RenderSistemaWeb.png](<RenderSistemaWeb.png>)
- [RenderVehiculo.png](<RenderVehiculo.png>)
- [Right.svg](<Right.svg>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../../../AutoFinalRobo2-Contracts/provenance/files.json).
