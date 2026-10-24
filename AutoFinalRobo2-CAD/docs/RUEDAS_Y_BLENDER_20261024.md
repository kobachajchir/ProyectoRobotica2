# Ruedas y Blender — AFR2-20261024-01

24/10/2026. Auto base representado sin cables; nueva pieza/PCB OLED y encoder pendientes.

1. Se conservó oculto el Wheel80mm importado por el usuario. Sin rediseño ni escala: Rueda80_ManoA y su reflejo ManoB, dos instancias por mano, iguales en diagonales, rodillos en X.
2. Ejes de motores conservados: X, centros Y=-14 y Z=±62 mm. Boca del acople X=±81 mm; agujero de apriete orientado a cara plana D, Y=-15,4 mm. Cuatro prisioneros con centro axial X=±84 mm.
3. Usuario autorizó cuatro casquillos de referencia: Ø6 exterior/Ø4 interior, longitud adoptada 10 mm, ventana radial al prisionero. Sin tolerancias de fabricación confirmadas. Capas del chasis y poses previas conservadas.
4. Fusion AutoFinalRobo2 guardado en **v16** con descripción de ruedas, manos opuestas, alineación D y casquillos. [F3D](../mecanica/auto-completo/AutoFinalRobo2-20261024-ruedas.f3d), [poses](ensamble-fusion-v16.json), [script de ruedas](../mecanica/auto-completo/fusion_ruedas_20261024.py). Casquillos conservados como sólidos en F3D.
5. Exportados 18 USDZ y dos capas OBJ/MTL, manifest de 26 instancias, en exports/2026-10-24/parts. [Exportador](../mecanica/auto-completo/fusion_exportar_blender.py). OBJ en cm; USDZ con escala. El F3D permite regenerar salidas ignoradas por Git. Script conserva exportaciones existentes: usar otra carpeta para otra revisión. Autoría/licencia de modelos aportados pendiente de identificación.
6. [Importador](../mecanica/auto-completo/blender_desde_fusion.py): Fusion X derecha/Y arriba/frente -Z → Blender X derecha/Z arriba/frente +Y, metros. Poses del manifest y desplazamiento global Z=+0,05305135 m para apoyar en suelo. Relaciones entre piezas conservadas.
7. [Maestro Blender](../mecanica/auto-completo/AutoFinalRobo2-20261024.blend), Blender 4.5.1 LTS. Colecciones AFR2_Modelo_Detallado oculta, AFR2_Modelo_Ligero visible y escena previa conservada AFR2_EscenaPrevia. Dos cámaras, tres luces y suelo de estudio excluidos del GLB.
8. Mallas compartidas, materiales equivalentes unificados, disolución coplanar y reducción del detalle fino de electrónica. Ruedas con mayor conservación de geometría. RGB sRGB convertido a lineal. No había mapas de imagen independientes en USDZ: materiales por cara y detalles geométricos conservados; GLB sin texturas de imagen.
9. [Reducción/exportación](../mecanica/auto-completo/blender_ligero.py). Scripts documentan receta; no se volvieron a ejecutar desde cero sobre el maestro terminado. Detallado conserva geometría previa a reducción.

**exports/2026-10-24/AutoFinalRobo2-ligero.glb: 827.232 bytes**, menor que 900.000, margen 72.768. 26 objetos, 20 mallas únicas, 79 materiales PBR; 101.881 triángulos únicos y 139.675 con instancias. [Registro](blender-exportacion-20261024.json).

Draco nivel 10, cuantización posición 13 bits, normales 8, UV 12. El visor requiere **KHR_draco_mesh_compression**. Sin integración/prueba en Web o visor externo. El exportador emitió avisos de reparación interna de algunas mallas CAD; sin suite de validación adicional. Versión ligera simplifica detalles finos y no sustituye CAD para fabricación.

Imágenes en renders/2026-10-24: fusion-frente.png, fusion-atras.png, fusion-lado-derecho.png, fusion-lado-izquierdo.png; blender-final-frente.png y blender-final-atras.png muestran la colección ligera.

Pendientes variantes físicas, tolerancias, inferencias, tornillería completa y futura PCB/pieza OLED/encoder. Atmega/ESP/Web sin cambios. Sin tests, builds, interferencias, simulaciones, flasheos ni pruebas físicas. Sin commit/push solicitado.
