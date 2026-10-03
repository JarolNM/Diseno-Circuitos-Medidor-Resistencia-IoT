"""Sustentacion de calculos - Etapas I y II (celda Molicel M35A + MAX17335).
Ejecutar:  python calculos_gestion_bateria.py
Cada bloque reproduce una ecuacion o tabla del informe."""
import math

# ---------------- Etapa I: celda ----------------
C_TIP, C_MIN, V_NOM = 3450, 3350, 3.6          # mAh, mAh, V (datasheet M35A)
E_Wh   = V_NOM * C_TIP / 1000
C_UTIL = 0.9 * C_MIN                            # Ec. (1)
print(f"Energia de la celda      E = {E_Wh:.1f} Wh")
print(f"Capacidad utilizable     C_util = 0.9 x {C_MIN} = {C_UTIL:.0f} mAh\n")

# ---------------- Corriente de carga (Tabla de opciones) ----------------
R_SENSE = 10e-3                                 # ohm
V_PCKP  = 5.0
print("I_CHG   V_CSP-CSN   t_carga   P_FET(3.0V)  P_FET(3.6V)")
for i in (0.5, 1.0, 1.7):
    v_cs  = i * R_SENSE * 1e3                  # mV   Ec. (2)
    t     = 1.1 * C_TIP / 1000 / i             # h    (factor 1,1 por fase CV)
    p30   = (V_PCKP - 3.0) * i                 # W    carga lineal
    p36   = (V_PCKP - 3.6) * i
    print(f"{i:4.1f} A   {v_cs:5.1f} mV   {t:5.1f} h   {p30:6.2f} W     {p36:6.2f} W")
print()

# ---------------- FETs de carga/descarga (FDPC4044) ----------------
P_FET, R_THJA, T_AMB = 2.0, 47, 25             # W, C/W (1 in^2 Cu), C
dT = P_FET * R_THJA
print(f"FDPC4044: dT = {P_FET} W x {R_THJA} C/W = {dT:.0f} C -> T_J = {T_AMB+dT:.0f} C (< 150 C)")
V_BATT_MAX = 4.2
print(f"V_DS requerido > 2 x {V_BATT_MAX} = {2*V_BATT_MAX:.1f} V  (FDPC4044: 30 V)")
I_PICO = 3.0 * 1.0 / (0.9 * 2.5)               # corriente del pulso a 2,5 V (Ec. 6, etapa III)
print(f"I_D maximo (pulso a V_BATT = 2,5 V) = {I_PICO:.2f} A")
print(f"R_DS(on) max por FET para caida <= 75 mV: {0.075/ (I_PICO*2) *1e3:.0f} mOhm aprox. (2 FETs en serie) -> criterio 25 mOhm\n")

# ---------------- Pull-ups I2C (NXP UM10204, modo rapido 400 kHz) ----------------
VDD, VOL, IOL = 3.3, 0.4, 3e-3
TR, CB = 300e-9, 100e-12
RP_MIN = (VDD - VOL) / IOL                      # Ec. (3)
RP_MAX = TR / (0.8473 * CB)                     # Ec. (4)
RP = 1.5e3
print(f"Rp_min = {RP_MIN:.0f} ohm,  Rp_max = {RP_MAX/1e3:.2f} kohm  -> adoptado {RP/1e3:.1f} kohm")
print(f"t_r con 1,5 k y 100 pF = {0.8473*RP*CB*1e9:.0f} ns (limite 300 ns)\n")

# ---------------- Tiempo de carga desde la app ----------------
for soc in (0.2, 0.65):
    t = C_TIP/1000 * (1 - soc) / 1.0
    print(f"t_carga (SOC {soc*100:.0f} %, 1 A) = {t:.2f} h = {t*60:.0f} min")
