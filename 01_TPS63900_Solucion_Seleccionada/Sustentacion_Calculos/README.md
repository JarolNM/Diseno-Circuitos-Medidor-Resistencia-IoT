# Sustentación de cálculos — Etapa III (TPS63900) y III-B (TPS63802)

Los valores se reproducen con `calculos_regulacion.py`.

## 1. TPS63900 — riel lógico de 3,3 V
**Configuración R2D** (SEL a SYS+, en alto): $R_{CFG1}=36{,}5\ \text{k}\Omega$, $R_{CFG2}=0\ \Omega \Rightarrow V_{O(2)}=3{,}3$ V con límite de corriente de entrada "ilimitado"; $R_{CFG3}=16{,}2\ \text{k}\Omega \Rightarrow V_{O(1)}=3{,}3$ V. Resistencias de 1 % (E96). Fuentes: [Pin Configuration](../Datasheets_y_Notas_de_Aplicacion/Pin%20Configuration%20tps63900.pdf), [Input Current Limit](../Datasheets_y_Notas_de_Aplicacion/Input%20Current%20Limit.pdf), [Typical Application](../Datasheets_y_Notas_de_Aplicacion/Typical%20Application%20Schematic.pdf).

**Inductor:** corriente pico garantizada en el peor caso 1,6 A → $I_{SAT,min}=1{,}2\times1{,}6=1{,}92$ A ([Inductor Selection](../Datasheets_y_Notas_de_Aplicacion/nductor%20Selection%20Input%20Capacitor%20Selection%20.pdf)).

| Inductor 2,2 µH | I_SAT | ¿Cumple? |
|---|---|---|
| Coilcraft XFL4020-222ME | 3,5 A | Sí (más grande) |
| Bourns SRN3015TA-2R2M | 1,7 A | **No** |
| **Murata DFE252010F-2R2M** | **3,1 A** | **Sí — seleccionado (margen 1,94×, DCR 97 mΩ)** |
| Murata DFE201612E-2R2M | 2,4 A | Sí |
| Murata DFE201210U-2R2M | 2,0 A | Sí (margen mínimo) |

**Capacitores:** 10 µF de entrada y 22 µF de salida, cerámicos X5R/X7R.

**Capacidad del riel lógico:** nRF52840 en TX (4,8 mA) + ADS1220 (0,34 mA) + *pull-ups* ≲ 6 mA frente a 400 mA garantizados → margen ≈ 67×.

## 2. Corriente tomada de SYS durante el pulso de 1 A
$$I_{in}=\frac{V_{exc}\,I_{exc}}{\eta\,V_{BATT}},\qquad \eta=0{,}9 \tag{6}$$

| V_BATT (V) | 4,2 | 3,6 | 3,3 | 3,0 | 2,5 |
|---|---|---|---|---|---|
| I_in (A) | 0,79 | 0,93 | 1,01 | 1,11 | **1,33** |

El máximo (1,33 A) dimensiona los FETs del MAX17335. El firmware inhibe la medición con V_CELL < 3,0 V.

## 3. TPS63802 — riel de excitación de 3,0 V
$$V_O=V_{FB}\left(1+\frac{R_1}{R_2}\right)=0{,}5\left(1+\frac{453}{90{,}9}\right)=2{,}99\ \text{V} \tag{7}$$

Inductor 0,47 µH (XFL4015-471ME, I_SAT = 5,4 A), 10 µF de entrada, 22 µF de salida, EN = EN_BB (P0.04), MODE = GND. Datasheet: [TPS63802](https://www.ti.com/lit/ds/symlink/tps63802.pdf).

**Capacitancia de reserva:**
$$\Delta V=\frac{I\,t_{resp}}{C_{res}}=\frac{1\ \text{A}\times20\ \mu\text{s}}{100\ \mu\text{F}}=0{,}2\ \text{V} \tag{8}$$
inferior al margen de 0,68 V del presupuesto de voltaje (Etapa IV-B).

## 4. ¿Por qué 3,0 V y no 3,3 V para la excitación?
El nodo más alto que ve el ADS1220 (Sense+ ≈ V_exc − I·R_ON) debe quedar bajo AVDD + 0,1 V con el PGA en *bypass*, y AVDD viene de otro convertidor:

| V_exc | Sense+ (peor caso) | Límite AVDD+0,1 V | Margen |
|---|---|---|---|
| 3,3 V | 3,263 V | 3,350 V | 87 mV (insuficiente) |
| **3,0 V** | **2,960 V** | **3,350 V** | **390 mV** |
