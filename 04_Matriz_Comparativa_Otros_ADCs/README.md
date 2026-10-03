# Matriz comparativa de ADCs

| Criterio | **ADS1220** | ADS124S08 | HX711 | ADS1115 | SAADC del nRF52840 |
|---|---|---|---|---|---|
| Resolución | **24 bits** | 24 bits | 24 bits | 16 bits | 12 bits (14 con *oversampling*) |
| Velocidad máx. | 2000 SPS | 4000 SPS | 80 SPS | 860 SPS | 200 ksps |
| Referencia externa diferencial (ratiométrica) | **Sí (REFP0/REFN0)** | Sí | No | No | No |
| PGA / *bypass* | 1–128, *bypass* | 1–128 | 32–128 | Rango por PGA | Ganancia fija por pasos |
| Entradas | 4 | 12 | 2 | 4 | 8 |
| Interfaz | SPI | SPI | Serie propietaria | I2C | Interna |
| Encapsulado | VQFN-16 (3,5 × 3,5 mm) | VQFN-32 | SOP-16 | VSSOP-10 | — |
| Decisión | **Elegido** | Más canales y área de los necesarios | Sin referencia externa, solo 80 SPS (no caben 4 conversiones en 20 ms) | Sin referencia externa y resolución insuficiente | Resolución insuficiente para µΩ |
| Datasheet | [Ver](https://www.ti.com/lit/ds/symlink/ads1220.pdf) | [Ver](https://www.ti.com/lit/ds/symlink/ads124s08.pdf) | [Ver](https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf) | [Ver](https://www.ti.com/lit/ds/symlink/ads1115.pdf) | [Ver](https://files.seeedstudio.com/wiki/XIAO-BLE/Nano_BLE_MCU-nRF52840_PS_v1.1.pdf) |

## Criterio decisivo
La medición es **ratiométrica**: la tensión de $R_{REF}$ debe entrar como referencia del ADC para que se cancele la exactitud de la corriente. Solo el ADS1220 y el ADS124S08 lo permiten; el ADS1220 es más pequeño y basta con sus 4 entradas. Además, a 330 SPS permite 4 conversiones dentro del pulso de 20 ms.
