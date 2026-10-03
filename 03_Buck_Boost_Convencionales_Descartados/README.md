# *Buck-boost* convencionales — alternativas descartadas para el riel lógico

| Regulador | I_Q típ. | Corriente | Motivo del descarte | Datasheet |
|---|---|---|---|---|
| TPS63020 (TI) | ≈25 µA | interruptores de 4 A | Su I_Q (siempre encendido) superaría al resto del sistema en reposo | [Ver](https://www.ti.com/lit/ds/symlink/tps63020.pdf) |
| TPS63001 (TI) | ≈40–50 µA | interruptores de 1,8 A | I_Q aún mayor | [Ver](https://www.ti.com/lit/ds/symlink/tps63001.pdf) |
| TPS63802 (TI) | ≈11 µA | 2 A | Demasiado consumo para estar siempre encendido → **se usa solo para el riel de excitación**, apagado entre mediciones | [Ver](https://www.ti.com/lit/ds/symlink/tps63802.pdf) |

## Motivo del descarte
El riel lógico está encendido el 100 % del tiempo, así que su corriente de reposo pesa directamente en la autonomía. El presupuesto total en reposo es de unos 37 µA; un regulador de 25–50 µA lo duplicaría. Además, los *buck-boost* convencionales cambian entre modos *buck*, *buck-boost* y *boost*, con transitorios en esas transiciones.

El **TPS63900** ofrece 75 nA de I_Q con operación de modo único, sin esas transiciones, y garantiza 400 mA, muy por encima de los ≈6 mA de la lógica.

**Seleccionado:** TPS63900 (riel lógico) + TPS63802 (riel de excitación, solo durante el pulso).
