"""Sustentacion de calculos - Etapa III (TPS63900, riel logico 3,3 V)
y Etapa III-B (TPS63802, riel de excitacion 3,0 V).  Ejecutar: python calculos_regulacion.py"""

# ---------- TPS63900: inductor ----------
I_PICO_MAX = 1.6                 # A, limite pico garantizado (V_I=1,8 V, V_O=3,6 V, "ilimitado")
I_SAT_MIN  = 1.2 * I_PICO_MAX    # margen del 20 % del fabricante
print(f"I_SAT minimo = 1,2 x {I_PICO_MAX} = {I_SAT_MIN:.2f} A")
for parte, isat in [("XFL4020-222ME", 3.5), ("SRN3015TA-2R2M", 1.7), ("DFE252010F-2R2M", 3.1),
                    ("DFE201612E-2R2M", 2.4), ("DFE201210U-2R2M", 2.0)]:
    print(f"  {parte:16s} I_SAT={isat} A -> {'cumple' if isat >= I_SAT_MIN else 'NO cumple'}"
          f"{'  (seleccionado, margen %.2fx)' % (isat/I_PICO_MAX) if parte.startswith('DFE252010F') else ''}")

# ---------- Capacidad del riel logico ----------
I_LOGICA = 4.8 + 0.34 + 0.5      # mA: nRF52840 TX + ADS1220 + pull-ups/varios
print(f"\nCarga logica maxima ~ {I_LOGICA:.1f} mA (se toma 6 mA) -> margen 400/6 = {400/6:.0f}x")

# ---------- Corriente de entrada durante el pulso (Ec. 6) ----------
V_EXC, I_EXC, ETA = 3.0, 1.0, 0.9
print("\nV_BATT (V):  " + "  ".join(f"{v:4.1f}" for v in (4.2, 3.6, 3.3, 3.0, 2.5)))
print("I_in (A):    " + "  ".join(f"{V_EXC*I_EXC/(ETA*v):4.2f}" for v in (4.2, 3.6, 3.3, 3.0, 2.5)))

# ---------- TPS63802: divisor de realimentacion (Ec. 7) ----------
V_FB, R1, R2 = 0.5, 453e3, 90.9e3
print(f"\nV_O = 0,5 x (1 + {R1/1e3:.0f}k/{R2/1e3:.1f}k) = {V_FB*(1+R1/R2):.3f} V")

# ---------- Capacitancia de reserva (Ec. 8) ----------
C_RES, T_RESP = 100e-6, 20e-6
print(f"Delta V = 1 A x 20 us / 100 uF = {I_EXC*T_RESP/C_RES:.2f} V  (margen disponible 0,68 V)")

# ---------- Por que 3,0 V y no 3,3 V (rango de entrada del ADS1220 con PGA en bypass) ----------
R_ON = 0.07
for v in (3.3, 3.0):
    nodo   = v * 1.01 - I_EXC * R_ON           # peor caso: +1 % en V_EXC
    limite = 3.3 * 0.985 + 0.1                 # AVDD minimo (-1,5 %) + 0,1 V
    print(f"V_EXC = {v} V: Sense+ = {nodo:.3f} V, limite AVDD+0,1 = {limite:.3f} V -> margen {1e3*(limite-nodo):.0f} mV")
