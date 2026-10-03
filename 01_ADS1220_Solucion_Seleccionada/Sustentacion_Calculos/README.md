# Sustentación de cálculos — Etapas IV-A (fuente de 1 A) y IV-B (ADS1220)

Todos los valores se reproducen con `calculos_medicion.py`. La calculadora oficial de TI está en `ADS1220_Excel_Calculator.xlsx`.
Teoría: nota de TI SBAA275A ([copia](../Datasheets_y_Notas_de_Aplicacion/sbaa275a.pdf)), [Medición ratiométrica](../Datasheets_y_Notas_de_Aplicacion/Medici%C3%B3n%20ratiom%C3%A9trica.pdf), [Ecuaciones de medición](../Datasheets_y_Notas_de_Aplicacion/Ecuaciones%20de%20medici%C3%B3n.pdf).

## 1. Fuente de corriente pulsada (OPA365 + CSD13380F3 + R_sense)
$$I_{exc}=\frac{V_{REF,exc}}{R_{sense}}\Rightarrow V_{REF,exc}=1\ \text{A}\times0{,}05\ \Omega=50\ \text{mV} \tag{9}$$
$R_{sense}$ disipa 50 mW durante el pulso.

Divisor desde la salida conmutada ($V_{sw}=3{,}0-1\ \text{A}\times70\ \text{m}\Omega=2{,}93$ V), con $R_2=1\ \text{k}\Omega$:
$$R_1=R_2\left(\frac{V_{sw}}{V_{REF,exc}}-1\right)=57{,}6\ \text{k}\Omega\ (\text{E96}) \tag{11}$$
Consume 50 µA solo durante el pulso.

Polo de compuerta con $R_{gate}=100\ \Omega$ y $C_{ISS}=120$–$156$ pF:
$$f_{pole}=\frac{1}{2\pi R_{gate}C_{ISS}}=10{,}2\text{–}13{,}3\ \text{MHz} \tag{10}$$
Compensación opcional $R_f=1\ \text{k}\Omega$, $C_f=1$ nF → cruce en 159 kHz.

## 2. Presupuesto de voltaje (R_test = 1 Ω, V_exc = 3,0 V)
$$V_R=I_{exc}(R_{ON}+R_{test}+R_{REF}+R_{sense})=1\times(0{,}07+1+1{,}2+0{,}05)=2{,}32\ \text{V} \tag{12}$$
Quedan **0,68 V** para el MOSFET, que solo necesita ≈73 mV. Con $R_{test}=0$ el MOSFET absorbe 1,68 V (1,68 W, 34 mJ por pulso), dentro de su SOA.

| Nodo | Tensión |
|---|---|
| AIN0 (Sense+) | 2,93 V |
| AIN1 = REFP0 | 1,93 V |
| REFN0 | 0,73 V |
| V_REF = REFP0 − REFN0 | 1,20 V (≥ 0,75 V requerido) |

Todos dentro de AVSS − 0,1 V a AVDD + 0,1 V ([rango de modo común](../Datasheets_y_Notas_de_Aplicacion/ADS_COMMON_MODE_RANGE.pdf)).

## 3. Filtros RC (R = 499 Ω, C_dif = 100 nF, C_cm = 10 nF, C0G)
$$f_{dif}=\frac{1}{2\pi(2R)(C_{dif}+C_{cm}/2)}=1{,}52\ \text{kHz},\qquad f_{cm}=\frac{1}{2\pi RC_{cm}}=31{,}9\ \text{kHz} \tag{13}$$
τ = 105 µs; asentamiento a 24 bits (16,6 τ) = 1,74 ms, dentro de la ventana de 5 ms.

## 4. Ecuación de medición
$$\text{Código}=2^{23}G\frac{I_{exc}R_{test}}{I_{exc}R_{REF}}\Rightarrow R_{test}=\frac{\text{Código}\cdot R_{REF}}{2^{23}G} \tag{14}$$
$I_{exc}$ se cancela. Con $G=1$ (PGA en *bypass*): fondo de escala $R_{REF}/G=1{,}2\ \Omega$, cuantización $1{,}2/2^{23}\approx143$ nΩ.

## 5. Resolución real del ADS1220
| Concepto | Valor |
|---|---|
| Ruido a 330 SPS ($10{,}55\sqrt{330/90}$) | ≈ 20 µV rms |
| Ruido con 4 promedios | 10 µV rms → **10 µΩ** con 1 A |
| ENOB = log₂(2,4 V / 10 µV) | **≈ 18 bits** (de 24) |
| Bits libres de ruido = log₂(2,4 V / 66 µVpp) | ≈ 15 bits |

## 6. Presupuesto de error (R_test = 1 Ω)
| Fuente | Típ. (µΩ) | Máx. (µΩ) |
|---|---|---|
| Tolerancia de R_REF (±0,05 %) | 500 | 500 |
| Deriva de R_REF (25 ppm/°C, ΔT = 10 °C) | 250 | 250 |
| Ganancia del ADC (0,015 % / 0,1 %) | 150 | 1000 |
| Offset (4 / 30 µV) | 4 | 30 |
| Ruido (promedio de 4) | 10 | 66 |
| INL (6 / 15 ppm FSR) | 7 | 18 |
| Corriente de entrada × 499 Ω | 7,5 | 15 |
| **Total (RSS / suma)** | **≈ 580 (±0,058 %)** | **≈ 1880 (±0,19 %)** |

Para cualquier lectura:
$$u_{tip}=\sqrt{(5{,}79\times10^{-4}R)^2+(14{,}9\ \mu\Omega)^2} \tag{15}$$
$$u_{max}=1{,}75\times10^{-3}R+129\ \mu\Omega \tag{16}$$

| R_test | u_tip | u_max |
|---|---|---|
| 1 Ω | 0,58 mΩ (0,058 %) | 1,88 mΩ (0,19 %) |
| 100 mΩ | 60 µΩ (0,06 %) | 0,30 mΩ (0,30 %) |
| 10 mΩ | 16 µΩ (0,16 %) | 0,15 mΩ (1,5 %) |
| 1 mΩ | 15 µΩ (1,5 %) | 0,13 mΩ (13 %) |

**Rango útil: 1 mΩ a 1,2 Ω.**

## 7. Registros del ADS1220
| Reg. | Valor | Configuración |
|---|---|---|
| 0x00 | 0x01 | MUX = AIN0–AIN1, G = 1, PGA_BYPASS = 1 |
| 0x01 | 0x84 | 330 SPS, modo normal, conversión continua |
| 0x02 | 0x40 | VREF = REFP0/REFN0, IDAC apagados |
| 0x03 | 0x00 | IDAC sin enrutar |

Codificación: [mapa de registros](../Datasheets_y_Notas_de_Aplicacion/ADS_REGISTER_MAP.pdf). A 330 SPS cada conversión dura 3,03 ms: 4 conversiones caben en el pulso de 20 ms tras 5 ms de asentamiento.
