# *Fuel gauges* — alternativas descartadas

| Dispositivo | I_Q (reposo) | Carga / protección | Motivo del descarte | Datasheet |
|---|---|---|---|---|
| MAX17330 (ADI) | 21 µA (hibernate) | Sí (misma arquitectura) | Marcado *Not Recommended for New Designs*; el MAX17335 es su versión vigente | [Ver](https://www.analog.com/media/en/technical-documentation/data-sheets/max17330.pdf) |
| MAX17048 (ADI) | 3–5 µA (hibernate) | No | Solo estima SOC por voltaje (ModelGauge); no carga ni protege, no da corriente ni TTF | [Ver](https://www.analog.com/media/en/technical-documentation/data-sheets/max17048-max17049.pdf) |
| MAX17055 (ADI) | 7 µA (hibernate) | No | Mismo algoritmo m5 EZ, pero requiere cargador y protector aparte | [Ver](https://www.analog.com/media/en/technical-documentation/data-sheets/max17055.pdf) |
| BQ27441-G1 (TI) | 9 µA (hibernate) | No | Requiere cargador y protector aparte; consumo mayor en modo normal (≈93 µA) | [Ver](https://www.ti.com/lit/ds/symlink/bq27441-g1.pdf) |

## Criterio
El enunciado pide mostrar el **porcentaje de batería** y el **tiempo de carga**, y el diseño busca mínimo tamaño y consumo. El MAX17335 es el único que integra carga AccuCharge, protección de lado alto y ModelGauge m5 EZ (SOC, corriente, TTF) en un IC vigente, con 21 µA en *hibernate* y 8 µA en modo *Ship*.

**Seleccionado:** MAX17335 — ver `01_MAX17335_Solucion_Seleccionada`.
