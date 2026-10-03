# Cargadores lineales — alternativas descartadas

Opción evaluada: usar un cargador lineal independiente más un *fuel gauge* y un protector separados, en lugar del MAX17335 (que integra los tres).

| Cargador | I_CHG máx. | Qué le falta frente al MAX17335 | Datasheet |
|---|---|---|---|
| MCP73831 (Microchip) | 500 mA | Sin *fuel gauge* ni protección; corriente máxima por debajo de 1 A | [Ver](https://ww1.microchip.com/downloads/en/DeviceDoc/MCP73831-Family-Data-Sheet-DS20001984H.pdf) |
| BQ24075 (TI) | 1,5 A | Tiene *power path*, pero requiere *fuel gauge* y protector aparte | [Ver](https://www.ti.com/lit/ds/symlink/bq24075.pdf) |
| TP4056 + DW01A | 1 A | Cargador básico; protección con otro IC ([DW01A](https://uelectronics.com/wp-content/uploads/2021/05/Datasheet-DW01A.pdf)) y sin medición de SOC | [Ver](https://dlnmh9ip6v2uc.cloudfront.net/datasheets/Prototyping/TP4056.pdf) |

## Motivo del descarte
1. **Más componentes y área:** cargador + protector + *fuel gauge* son 2–3 IC con sus pasivos, frente a un solo WLP de 1,9 × 2,5 mm.
2. **Sincronización por firmware:** el SOC y el tiempo de carga tendrían que combinarse en el microcontrolador a partir de IC distintos.
3. **Sin perfil JEITA ni protecciones de tres velocidades** integradas en la misma lógica que controla la carga.
4. El MAX17335 entrega en un solo bus I2C todo lo que la aplicación necesita: SOC, V_CELL, corriente, TTF y el modo *Ship* para apagar el equipo.

**Seleccionado:** MAX17335 — ver `01_MAX17335_Solucion_Seleccionada`.
