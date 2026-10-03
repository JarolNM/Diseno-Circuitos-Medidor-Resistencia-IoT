# Sustentación de cálculos — Etapa V (nRF52840) y presupuesto de consumo

Los valores se reproducen con `calculos_consumo.py`. Firmware: [`firmware/medidor_nrf52840`](https://github.com/JarolNM/Diseno-Circuitos-Medidor-Resistencia-IoT/tree/main/firmware/medidor_nrf52840) (rama `main`).

## 1. Modo de reposo del nRF52840
Se adopta **System ON, RAM completa, despertar por RTC = 3,16 µA**: la pila BLE y el estado del programa están en RAM y el equipo sigue anunciándose ([Consumo de CPU y modos](../Datasheets_y_Notas_de_Aplicacion/Consumo%20del%20CPU%20y%20modos%20de%20Power%20Management.pdf)). El regulador DC/DC interno reduce la radio de 10,6 a 4,8 mA en TX a 0 dBm ([Consumo de radio](../Datasheets_y_Notas_de_Aplicacion/Consumo%20de%20radio%20TXRX.pdf)).

## 2. Publicidad BLE (un evento por segundo, tres canales)
$$Q_{adv}\approx3\,(4{,}8\ \text{mA}\times0{,}30\ \text{ms}+4{,}6\ \text{mA}\times0{,}15\ \text{ms})+4{,}5\ \mu\text{C}=10{,}9\ \mu\text{C}\Rightarrow I_{adv}=10{,}9\ \mu\text{A}$$
(Tiempos por canal supuestos; se pueden contrastar con el [Online Power Profiler](https://devzone.nordicsemi.com/power/) de Nordic.)

## 3. Consumo en reposo referido a la batería
Riel lógico → batería: factor $3{,}3/(3{,}6\times0{,}85)=1{,}078$.

| Bloque | Corriente |
|---|---|
| MAX17335 (*hibernate*) | 21 µA |
| Publicidad BLE (×1,078) | 11,74 µA |
| nRF52840 System ON + RTC (×1,078) | 3,41 µA |
| TPS63802 apagado (máx.) | 0,6 µA |
| ADS1220 *power-down* (×1,078) | 0,11 µA |
| TPS63900 (I_Q) | 0,075 µA |
| **Total** | **≈ 36,9 µA** |

## 4. Carga por medición (V_BATT = 3,6 V)
| Término | Carga |
|---|---|
| Pulso: 0,926 A × 20 ms | 18,52 mC |
| MCU (5 mA, 60 ms) + ADS1220 + OPA365 + divisor | 0,41 mC |
| Capacitores de salida del TPS63802 (122 µF) | 0,34 mC |
| **Q_med** | **≈ 19,3 mC** |

## 5. Autonomía (C_util = 3015 mAh, I_prom = I_reposo + Q_med/T)
| Periodo T | I_prom | Autonomía |
|---|---|---|
| 10 s | 1,96 mA | ≈ 64 días |
| **60 s** | **358 µA** | **≈ 351 días** |
| 900 s | 58,3 µA | ≈ 5,9 años |
| Solo a pedido | 36,9 µA | ≈ 9,3 años |

Por encima de unos pocos años domina la autodescarga de la celda, así que esos valores son límites superiores. En modo *Ship* (8 µA) el consumo teórico daría para décadas.

## 6. Asignación de pines
| GPIO | Función |
|---|---|
| P0.02 | EN_EXC → ON del TPS22916B |
| P0.03 | ALRT del MAX17335 |
| P0.04 | EN_BB → EN del TPS63802 |
| P0.14 / P0.15 / P0.16 | SPI: SCLK / MOSI / MISO |
| P0.17 | CS del ADS1220 |
| P0.20 | DRDY del ADS1220 |
| P0.26 / P0.27 | I2C: SDA / SCL |
| P0.18, SWDIO, SWDCLK | Programación (J6) |
