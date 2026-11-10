# Informe D2 · fuentes

10/11/2026 · AFR2-20261110-19. Sustituye al borrador D1 de [informe_20261109](../informe_20261109/README.md), que se conserva.

| Archivo | Contenido |
|---|---|
| `AFR2_Informe_D2.md` | Texto del informe (Markdown de pandoc con ecuaciones LaTeX). `PLANO#n` marca un enlace a la hoja n del libro de planos. |
| `figuras/` | Renders del CAD, láminas de planos citadas, logo y figuras propias (`fig_*.png`). |
| `fuentes/check.py` | Verificación numérica de los modelos del brazo y de la base. |
| `fuentes/figs.py` | Figuras propias y simulación GOTO; escribe `figuras/goto_resultados.json`. |
| `fuentes/generar_informe.py` | Markdown → DOCX: anexo de planos, enlaces relativos, estilos, tablas, encabezado. |
| `fuentes/exportar_pdf.ps1` | Actualiza índices con Word y exporta el PDF. |
| `fuentes/enlaces_pdf.py` | Convierte los enlaces a planos del PDF en acciones que abren la hoja citada. |

## Regenerar

Requiere Python 3 con numpy, matplotlib, python-docx y pypdf; pandoc 3; Microsoft Word.

```bash
python AutoFinalRobo2-Contracts/docs/informe_20261110/fuentes/figs.py AutoFinalRobo2-Contracts/docs/informe_20261110/figuras
```

```bash
python AutoFinalRobo2-Contracts/docs/informe_20261110/fuentes/generar_informe.py Revision/R02_20261110/01_Informe
```

```bash
powershell -File AutoFinalRobo2-Contracts/docs/informe_20261110/fuentes/exportar_pdf.ps1 -Docx Revision/R02_20261110/01_Informe/AFR2_Informe_D2.docx
```

```bash
python AutoFinalRobo2-Contracts/docs/informe_20261110/fuentes/enlaces_pdf.py Revision/R02_20261110/01_Informe/AFR2_Informe_D2.pdf
```

Los enlaces se escriben relativos a `Revision/R02_AAAAMMDD/01_Informe/` y apuntan al libro de planos de R01. Para otra revisión, ajustar `PLANO_REL` en `generar_informe.py`.
