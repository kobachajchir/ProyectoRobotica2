# AFR2-20261024-15 — Alineación del plan maestro

24/10/2026. Pedido humano: revisar alineación de los módulos y completar lo
pendiente de implementación del plan maestro. Se contrastan fuentes activas,
contratos, catálogo de comandos, MENU_PLAN y planes de módulo. Referencias y
documentos del proveedor no se convierten en instrucciones de ejecución.

Se completa Calibración Web: adquisición automática del lease al escribir,
consulta reciente de salidas apagadas, parámetro previamente leído, operaciones
serializadas y cancelación de pasos pendientes por STOP/foco/enlace/tarea. No
se amplían permisos del AVR ni se añade calibración Hall remota.

Se completa diagnóstico LittleFS/paquete: metadatos opcionales webVersion y
packagedAt del empaquetador; ESP conserva perfil/hash del índice/tamaños solo
tras aceptar el manifiesto y los publica con salud FS en system.info, hello y
/api/health. Web los muestra dentro de Información de hardware. Manifiestos
anteriores son compatibles; faltantes se muestran desconocidos. Comprobación
de manifiesto/tamaños, sin prometer hashes recalculados en placa.
Se elimina controlLease=none fijo de /api/health; estado de mando real por WS.

ROBO2/perfil2, EX01v6, afr2-ws-1,33comandos y59ScreenCodes se conservan. AVR
sin nueva fuente; firmware nominal0.8.0. ESP/Web adaptados. Las compilaciones y
la promoción local de activos se registran en evidencia15; AVR conserva build12.
Sin nuevo activo, dependencia, CAD, upload, imagen FS, tests o navegador.

Planes/maestros se actualizan con matriz de fuente/consumidor/condición y las
copias anteriores se conservan. No se cierra aceptación pendiente: Hall físico,
CPR/signos/geometría, memoria/pila, flash/SPI, runtime y planos CAD siguen
necesitando datos y comprobaciones. Macros Hall/memoria0 no se cambian.

[Matriz vigente](../docs/ALINEACION_MAESTRO.md),
[evidencia](../documentation/builds/2026-10-24-15/README.md),
[procedencia por archivo](../provenance/master-alignment-20261024.json).
