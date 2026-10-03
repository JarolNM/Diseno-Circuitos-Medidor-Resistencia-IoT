# Esquemáticos y simulaciones — Etapas IV-A y IV-B

| Archivo | Contenido |
|---|---|
| `esquematico_fuente_excitacion_1A.png` | TPS22916B (U1), OPA365 (U4), divisor R29–R30 (V_REF,exc = 50 mV), CSD13380F3 (Q4), R_gate = 100 Ω, R_sense = 50 mΩ |
| `esquematico_ADS1220_y_red_Kelvin.png` | ADS1220 (U6), filtros RC de AIN0/AIN1, resistencias serie del SPI, desacople y conectores Kelvin J2/J3/J4 |
| `red_de_referencia_RREF.png` | R_REF = 1,2 Ω (R10) con sensado Kelvin hacia REFP0/REFN0 y su filtro RC |
| `simulacion_PSpice_circuito_fuente_1A.png` | Circuito simulado en PSpice con modelos de TI (switch + OPA365 + MOSFET + R_sense + R_test = 1 Ω) |
| `simulacion_PSpice_tension_en_Rtest.png` | Resultado: pulsos de 1 V sobre R_test (I_exc = 1 A), 20 ms cada 200 ms |
| `diagrama_de_tiempos_medicion.png` | Secuencia EN_BB → EN_EXC → 4 conversiones del ADS1220 dentro del pulso de 20 ms |
