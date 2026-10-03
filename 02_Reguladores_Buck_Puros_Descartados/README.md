# Reguladores *buck* puros — alternativas descartadas

| Regulador | Topología | I_Q | Motivo del descarte | Datasheet |
|---|---|---|---|---|
| TPS62840 (TI) | Buck | 60 nA | No puede dar 3,3 V cuando V_BATT < 3,3 V | [Ver](https://www.ti.com/lit/ds/symlink/tps62840.pdf) |
| TPS62740 (TI) | Buck | 360 nA | Mismo problema: solo reduce tensión | [Ver](https://www.ti.com/lit/ds/symlink/tps62740.pdf) |

## Motivo del descarte
La celda Li-ion trabaja entre 4,2 V y 2,5 V (corte de medición en 3,0 V). Un *buck* solo puede **bajar** la tensión: cuando la celda cae por debajo de unos 3,4 V (3,3 V + caída del regulador) el riel lógico deja de estar regulado, y eso ocurre en una parte importante de la curva de descarga. Se necesita topología **buck-boost**, que entrega 3,3 V con la entrada por encima o por debajo de la salida.

**Seleccionado:** TPS63900 (buck-boost, 75 nA) — ver `01_TPS63900_Solucion_Seleccionada`.
