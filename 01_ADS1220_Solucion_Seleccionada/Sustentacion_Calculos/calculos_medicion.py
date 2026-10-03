"""Sustentacion de calculos - Etapas IV-A (fuente de 1 A) y IV-B (ADS1220).
Ejecutar: python calculos_medicion.py"""
import math

I_EXC, R_SENSE, R_REF, R_ON = 1.0, 0.05, 1.2, 0.07
V_EXC = 3.0

# ---------------- IV-A: fuente de corriente ----------------
V_REF_EXC = I_EXC * R_SENSE                                  # Ec. (9)
print(f"V_REF,exc = {I_EXC} A x {R_SENSE} ohm = {V_REF_EXC*1e3:.0f} mV ; P(R_sense) = {I_EXC**2*R_SENSE*1e3:.0f} mW")
V_SW = V_EXC - I_EXC * R_ON
R2 = 1e3
R1 = R2 * (V_SW / V_REF_EXC - 1)                             # Ec. (11)
print(f"V_sw = {V_SW:.2f} V ; R1 = {R1/1e3:.1f} kohm -> E96 57,6 kohm ; I_divisor = {V_SW/(57.6e3+1e3)*1e6:.0f} uA")
R_GATE = 100
for c in (120e-12, 156e-12):
    print(f"f_pole (C_ISS={c*1e12:.0f} pF) = {1/(2*math.pi*R_GATE*c)/1e6:.1f} MHz")   # Ec. (10)
print(f"Compensacion opcional 1 k / 1 nF -> cruce {1/(2*math.pi*1e3*1e-9)/1e3:.0f} kHz\n")

# ---------------- IV-B: presupuesto de voltaje ----------------
R_TEST = 1.0
V_R = I_EXC * (R_ON + R_TEST + R_REF + R_SENSE)               # Ec. (12)
print(f"Caida total = {V_R:.2f} V ; queda para el MOSFET = {V_EXC - V_R:.2f} V (necesita ~73 mV)")
V_MOS_CC = V_EXC - R_ON - R_REF - R_SENSE                    # R_test = 0
print(f"Con R_test = 0: MOSFET absorbe {V_MOS_CC:.2f} V -> {V_MOS_CC*I_EXC:.2f} W, {V_MOS_CC*I_EXC*20e-3*1e3:.0f} mJ por pulso")
v_ain0 = V_EXC - R_ON
v_ain1 = v_ain0 - R_TEST
v_refn = v_ain1 - R_REF
print(f"Nodos: AIN0 = {v_ain0:.2f} V, AIN1 = REFP0 = {v_ain1:.2f} V, REFN0 = {v_refn:.2f} V")
print(f"V_REF = {v_ain1 - v_refn:.2f} V  (>= 0,75 V requerido)\n")

# ---------------- Filtros RC ----------------
R, C_DIF, C_CM = 499, 100e-9, 10e-9
f_dif = 1 / (2 * math.pi * 2 * R * (C_DIF + C_CM / 2))       # Ec. (13)
f_cm  = 1 / (2 * math.pi * R * C_CM)
tau   = 2 * R * (C_DIF + C_CM / 2)
print(f"f_dif = {f_dif/1e3:.2f} kHz ; f_cm = {f_cm/1e3:.1f} kHz ; tau = {tau*1e6:.0f} us ; "
      f"asentamiento 24 bits = {math.log(2**24)*tau*1e3:.2f} ms (ventana 5 ms)\n")

# ---------------- Ecuacion de medicion y resolucion ----------------
G = 1
print(f"Fondo de escala = R_REF/G = {R_REF/G} ohm ; cuantizacion = {R_REF/2**23*1e9:.0f} nOhm")
ruido_90 = 10.55e-6                                           # uVrms a 90 SPS (datasheet)
ruido_330 = ruido_90 * math.sqrt(330 / 90)
ruido_4 = ruido_330 / math.sqrt(4)
print(f"Ruido 330 SPS = {ruido_330*1e6:.1f} uVrms ; promedio de 4 = {ruido_4*1e6:.1f} uVrms "
      f"-> {ruido_4/I_EXC*1e6:.0f} uOhm")
FS = 2 * R_REF                                                # +-1,2 V
print(f"ENOB = log2({FS} V / 10 uV) = {math.log2(FS/10e-6):.1f} bits ; "
      f"libres de ruido = log2({FS} V / 66 uVpp) = {math.log2(FS/66e-6):.1f} bits\n")

# ---------------- Presupuesto de error (R_test = 1 ohm) ----------------
fuentes = {  # (tipico, maximo) en uOhm
    "Tolerancia R_REF 0,05 %":     (500, 500),
    "Deriva R_REF 25 ppm/C, 10 C": (250, 250),
    "Ganancia ADC":                (150, 1000),
    "Offset ADC":                  (4, 30),
    "Ruido (promedio de 4)":       (10, 66),
    "INL":                         (7, 18),
    "Corriente de entrada x 499":  (7.5, 15),
}
for k, (t, m) in fuentes.items():
    print(f"  {k:30s} {t:7.1f} {m:7.1f}")
tip = math.sqrt(sum(t**2 for t, _ in fuentes.values()))
mx = sum(m for _, m in fuentes.values())
print(f"Total: tipico (RSS) = {tip:.0f} uOhm ({tip/1e4:.3f} %) ; peor caso (suma) = {mx:.0f} uOhm ({mx/1e4:.2f} %)\n")

# ---------------- Incertidumbre en todo el rango (Ecs. 15 y 16) ----------------
def u_tip(r): return math.sqrt((5.79e-4 * r) ** 2 + (14.9e-6) ** 2)
def u_max(r): return 1.75e-3 * r + 129e-6
print("R_test        u_tip              u_max")
for r in (1.0, 0.1, 0.01, 0.001):
    print(f"{r*1e3:7.0f} mOhm  {u_tip(r)*1e6:7.1f} uOhm ({u_tip(r)/r*100:5.2f} %)  "
          f"{u_max(r)*1e6:7.0f} uOhm ({u_max(r)/r*100:5.1f} %)")

# ---------------- Registros ----------------
print("\nRegistros ADS1220: 0x00=0x01 (AIN0-AIN1, G=1, PGA bypass) 0x01=0x84 (330 SPS, continuo) "
      "0x02=0x40 (REFP0/REFN0) 0x03=0x00")
print(f"Conversion a 330 SPS = {1/330*1e3:.2f} ms ; 4 conversiones = {4/330*1e3:.1f} ms (+5 ms de asentamiento)")
