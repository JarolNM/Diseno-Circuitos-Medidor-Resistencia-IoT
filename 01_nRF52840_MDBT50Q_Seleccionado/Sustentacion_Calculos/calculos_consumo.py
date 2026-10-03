"""Sustentacion de calculos - Etapa V (nRF52840) y presupuesto de consumo/autonomia.
Ejecutar: python calculos_consumo.py"""

# ---------- Publicidad BLE (1 evento por segundo, 3 canales) ----------
I_TX, T_TX = 4.8e-3, 0.30e-3        # A, s por canal (DC/DC, 0 dBm)
I_RX, T_RX = 4.6e-3, 0.15e-3
Q_CPU = 4.5e-6                       # C de arranque de CPU por evento
Q_ADV = 3 * (I_TX * T_TX + I_RX * T_RX) + Q_CPU
print(f"Carga por evento de publicidad = {Q_ADV*1e6:.1f} uC -> I_adv = {Q_ADV*1e6:.1f} uA (intervalo 1 s)")

# ---------- Consumo en reposo referido a la bateria ----------
FACTOR = 3.3 / (3.6 * 0.85)          # riel logico -> bateria (eta = 0,85 a uA)
logica = {"nRF52840 System ON + RAM + RTC": 3.16, "Publicidad BLE": Q_ADV * 1e6,
          "ADS1220 power-down": 0.1}
directo = {"MAX17335 hibernate": 21.0, "TPS63900 IQ": 0.075, "TPS63802 apagado (max)": 0.6}
I_REPOSO = sum(logica.values()) * FACTOR + sum(directo.values())
print(f"Factor 3,3/(3,6 x 0,85) = {FACTOR:.3f}")
for k, v in logica.items():  print(f"  {k:32s} {v:6.3f} uA x factor = {v*FACTOR:6.2f} uA")
for k, v in directo.items(): print(f"  {k:32s} {v:6.3f} uA")
print(f"TOTAL EN REPOSO = {I_REPOSO:.1f} uA\n")

# ---------- Carga por medicion ----------
I_IN = 3.0 * 1.0 / (0.9 * 3.6)       # A a V_BATT = 3,6 V
Q_PULSO = I_IN * 20e-3
Q_MCU   = 5e-3 * 60e-3
Q_ADS   = 0.3e-3 * 20e-3
Q_OPA   = 4.6e-3 * 20e-3
Q_DIV   = 50e-6 * 20e-3
Q_AUX   = (Q_MCU + Q_ADS + Q_OPA + Q_DIV) * 1.0185
Q_C     = 122e-6 * 3.0 * I_IN        # capacitores de salida del TPS63802 que se pierden
Q_MED   = Q_PULSO + Q_AUX + Q_C
print(f"I_in = {I_IN:.3f} A ; Q_pulso = {Q_PULSO*1e3:.2f} mC ; Q_aux = {Q_AUX*1e3:.2f} mC ; Q_C = {Q_C*1e3:.2f} mC")
print(f"Q_med = {Q_MED*1e3:.2f} mC por medicion\n")

# ---------- Autonomia ----------
C_UTIL = 3015e-3 * 3600              # C (3015 mAh)
print("Periodo   I_prom        Autonomia")
for T in (10, 60, 900, None):
    I = I_REPOSO * 1e-6 + (Q_MED / T if T else 0)
    h = C_UTIL / I / 3600
    lab = f"{T:>4d} s" if T else "a pedido"
    print(f"{lab:8s}  {I*1e6:8.1f} uA   {h:8.0f} h = {h/24:6.0f} dias = {h/24/365:4.1f} anos")
print(f"\nModo Ship (8 uA): {C_UTIL/8e-6/3600/24/365:.0f} anos teoricos (limita la autodescarga)")
