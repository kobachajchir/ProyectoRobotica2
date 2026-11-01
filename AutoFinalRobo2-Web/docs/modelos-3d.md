# Activos3D del visor y simulador

AFR2-20261031-06. Referencias compartidas en
`src/components/three/modelAssets.ts`.

| Modelo | Ruta publica | Tamano bruto | Uso |
|---|---|---:|---|
| Auto |/models/AutoFinalRobo2.glb|880232bytes|Visor, control y simulador; salida SG90 separada|
| Brazo completo |/models/AFR2_Brazo_v17_Blanco.glb|780972bytes|Visor y simulador articulado|
| Cubo auxiliar |/models/AFR2_Cubo_50mm.glb|1784bytes|Visor y piezas del simulador|

El limite800k corresponde al nuevo brazo. Se recuperan33solidos que
habian quedado fuera por visibilidad:425instancias enBlender. La geometria
original se conserva en una coleccion separada de la mallaWeb.

`/models/catalog.json` identifica rutas,bytes ySHA256. Los archivos comprimidos
se cargan enESP/data/dist/models con nombres.glb.gz y entradas del manifiesto;
Draco requiere los archivos depublic/draco. El script
`python tools/sync-model-assets.py` realiza estas copias sin compilar.

La paginaModelos3D tiene Auto/Brazo/Cubo, referencias por grupo y controles
de inspeccion. El auto conserva el visor existente. El brazo deja de mostrar
el pendiente anterior y usa exactamente elGLB del simulador.

La revisión06 corrige el montaje v17 y el pivote del sensor usando el CAD,
y separa eje/horn/tornillo del SG90 en Blender conservando sus materiales.
El GLB nuevo está enpublic. Los paquetesESP/artifacts/dist conservan los
activos de la revisión05; se actualizarán con una compilación posterior.
La interfaz nueva, la animación y la alineación en navegador no se comprobaron.

**LosJS/HTML de los paquetes
conservan el build anterior**: no se ejecutaron compilacion, tests,
navegador ni flasheo. Una futura compilacion incorpora la interfaz nueva;
los modelos enpublic se incluyen automaticamente en ese build.
