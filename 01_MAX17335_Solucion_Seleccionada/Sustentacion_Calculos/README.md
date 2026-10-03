# Sustentación de cálculos — Etapas I y II (celda y MAX17335)

Los valores se reproducen con `calculos_gestion_bateria.py` (`python calculos_gestion_bateria.py`).

## 1. Celda Molicel INR-18650-M35A
| Parámetro | Valor | Fuente |
|---|---|---|
| Tensión nominal | 3,6 V | [Datasheet M35A](../Datasheets_y_Notas_de_Aplicacion/INR18650M35A-V2-80096%20%281%29.pdf) |
| Capacidad típica / mínima | 3450 / 3350 mAh | ídem |
| Energía | E = 3,6 V × 3,45 Ah = **12,4 Wh** | cálculo |

Capacidad utilizable (envejecimiento, temperatura y corte a 3,0 V):

$$C_{util} = 0{,}9\,C_{min} = 0{,}9 \times 3350 = \mathbf{3015\ mAh} \tag{1}$$

## 2. Corriente de carga
$$I_{CHG} = \frac{V_{CSP-CSN}}{R_{SENSE}},\qquad R_{SENSE}=10\ \text{m}\Omega \tag{2}$$

Carga lineal: $P_{FET} = (V_{PCKP}-V_{BATT})\,I_{CHG}$, tiempo $t \approx 1{,}1\,C/I_{CHG}$.

| I_CHG | V_CSP−CSN | t_carga | P_FET (3,0 V) | P_FET (3,6 V) |
|---|---|---|---|---|
| 0,5 A | 5 mV | 7,6 h | 1,0 W | 0,7 W |
| **1,0 A** | **10 mV** | **3,8 h** | **2,0 W** | **1,4 W** |
| 1,7 A | 17 mV | 2,2 h | 3,4 W | 2,4 W |

Se adopta **1,0 A**: carga en menos de 4 h y limita el FET a 2 W.
Rango permitido con 10 mΩ y fuente de 5 V: 185–2331 mA ([Component Selection](../Datasheets_y_Notas_de_Aplicacion/Component%20Selection.pdf)).

## 3. FETs de carga y descarga (FDPC4044)
| Criterio | Requerido | FDPC4044 |
|---|---|---|
| V_DS > 2·V_BATT,máx | > 8,4 V | 30 V |
| I_D (pulso a 2,5 V, Ec. 6 de la Etapa III) | 1,33 A | ✔ |
| R_DS(on) para caída ≤ 75 mV | ≤ 25 mΩ por FET | ≤ 6,4 mΩ |
| Disipación en carga | 2 W | ΔT = 2 W × 47 °C/W = **94 °C → T_J ≈ 119 °C** (< 150 °C) |

## 4. Pull-ups del bus I2C (NXP UM10204, 400 kHz)
$$R_{p,min}=\frac{V_{DD}-V_{OL}}{I_{OL}}=\frac{3{,}3-0{,}4}{3\ \text{mA}}=967\ \Omega \tag{3}$$
$$R_{p,max}=\frac{t_r}{0{,}8473\,C_b}=\frac{300\ \text{ns}}{0{,}8473\times100\ \text{pF}}=3{,}54\ \text{k}\Omega \tag{4}$$
Adoptado **1,5 kΩ** → t_r = 127 ns.

## 5. Tiempo de carga mostrado en la aplicación
$$t_{carga}\approx\frac{C_{bat}(1-SOC)}{I_{carga}}$$
Ejemplos: SOC 20 % → 2,76 h; SOC 65 % → 72 min. El firmware usa el registro TTF del MAX17335 (5,625 s/LSB).

## 6. Modos de bajo consumo (MAX17335)
| Modo | I_Q típ. | Uso |
|---|---|---|
| Active | 35 µA | — |
| **Hibernate** | **21 µA** | reposo (nHibCfg = 0x8909) |
| Ship | 8 µA | apagado desde la app (Config.SHIP = 1) |
