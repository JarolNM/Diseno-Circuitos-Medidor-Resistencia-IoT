# ESP32 (WiFi + BLE) — descartados

| Criterio | **nRF52840 (MDBT50Q)** | ESP32-C3 | ESP32-S3 |
|---|---|---|---|
| Conectividad | BLE 5 | WiFi + BLE 5 | WiFi + BLE 5 |
| Núcleo | Cortex-M4F, 64 MHz | RISC-V, 160 MHz | Xtensa LX7 doble, 240 MHz |
| Reposo **con la pila BLE activa** | **3,16 µA** | ≈130 µA (*light-sleep*) | ≈240 µA (*light-sleep*) |
| *Deep-sleep* | 1,5 µA | 5 µA (radio apagada) | 7 µA (radio apagada) |
| Radio activa | 4,6–4,8 mA (DC/DC) | ≈17–22 mA | ≈27,6 mA |
| Memoria | 1 MB / 256 kB | 4 MB / 400 kB | 8 MB / 512 kB |
| Decisión | **Elegido** | Descartado | Descartado |
| Datasheet | [Ver](https://files.seeedstudio.com/wiki/XIAO-BLE/Nano_BLE_MCU-nRF52840_PS_v1.1.pdf) | [Ver](https://documentation.espressif.com/esp32-c3_datasheet_en.pdf) | [Ver](https://documentation.espressif.com/esp32-s3_datasheet_en.pdf) |

## Motivo del descarte
1. **Consumo en reposo manteniendo la radio disponible:** el equipo pasa casi todo el tiempo esperando y anunciándose por BLE. El nRF52840 consume entre 40 y 75 veces menos que el *light-sleep* de los ESP32. El *deep-sleep* de los ESP32 apaga la radio y obliga a reiniciar la pila BLE en cada despertar.
2. **WiFi innecesario:** el enunciado solo pide visualización en un celular; el WiFi añade consumo, antena y costo.
3. **Autonomía:** con un ESP32 en *light-sleep*, solo el microcontrolador consumiría más que todo el presupuesto de reposo actual (≈37 µA), y la autonomía con una medición por minuto bajaría de ≈351 días a unos 250 días.

**Seleccionado:** nRF52840 en el módulo certificado Raytac MDBT50Q-1MV2 — ver `01_nRF52840_MDBT50Q_Seleccionado`.
