# Firmware del nRF52840 (MDBT50Q-1MV2)

Arduino + Adafruit nRF52 BSP (Bluefruit). Los periféricos (GPIO, SPIM2, TWIM0) se programan por registro, así que los pines son los GPIO reales del esquemático y no dependen de la *variant* elegida en el IDE.

## Archivos
| Archivo | Función |
|---|---|
| `medidor_nrf52840.ino` | Lógica principal: servicio GATT, medición periódica o a pedido, apagado |
| `config.h` | Pines y constantes (R_REF, tiempos, periodo, umbrales) |
| `hw_nrf.h` | Acceso directo a GPIO, SPI (SPIM2) e I2C (TWIM0), con el *workaround* de la anomalía 89 |
| `ads1220.h` | Driver del ADS1220 (registros 0x01, 0x84, 0x40, 0x00 y monitor de V_REF) |
| `max17335.h` | Lectura de SOC, V_CELL, corriente, PCKP y TTF, y comando de modo *Ship* |

## Secuencia de una medición
1. Si V_CELL < 3,0 V → responde −1 (batería baja) sin medir.
2. EN_BB en alto (arranca el TPS63802) → 5 ms → EN_EXC en alto (pulso de 1 A) → 5 ms de asentamiento.
3. Cuatro conversiones del ADS1220 a 330 SPS, promediadas.
4. Una conversión de (V_REFP0 − V_REFN0)/4 contra la referencia interna para verificar que circula corriente (detecta circuito abierto).
5. Se apagan el pulso y el convertidor, y el ADC pasa a *power-down*.
6. R = código·R_REF/2²³; si satura o supera 1,2 Ω → −2 (fuera de rango).

## Servicio GATT (UUID base `5f1cXXXX-7a3b-4e2d-9c81-3b6f0a2d4e10`)
| UUID | Característica | Formato |
|---|---|---|
| 0001 | Resistencia | float32 (Ω): NaN = sin medición, −1 = batería baja, −2 = fuera de rango |
| 0002 | SOC | uint8 (%) |
| 0003 | Tiempo de carga | uint16 (min), 0xFFFF = no aplica |
| 0004 | Estado de carga | uint8: 0 sin cargador, 1 cargando, 2 completa |
| 0005 | Comando | 01 medir, 02 apagar (*Ship*), 03/04 auto on/off, 05 T = periodo (5–255 s) |
| 0006 | V_CELL | uint16 (mV) |
| 0007 | Configuración | [auto 0/1, periodo s] |

## Compilar y grabar
1. Arduino IDE → Gestor de tarjetas → **Adafruit nRF52** → tarjeta **Adafruit Feather nRF52840 Express**.
2. La tarjeta no tiene USB de datos: se programa por **SWD (J6)** con un J-Link.
3. Primero *Herramientas → Grabar bootloader* (con el programador J-Link) y luego *Programa → Subir usando programador*.

## Nota
El código usa CS del ADS1220 en P0.17. Ver el informe para la revisión del esquemático.
