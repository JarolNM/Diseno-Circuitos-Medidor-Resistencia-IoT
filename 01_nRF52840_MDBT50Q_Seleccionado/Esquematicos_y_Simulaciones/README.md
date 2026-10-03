# Esquemáticos y validación — Etapa V

| Archivo | Contenido |
|---|---|
| `esquematico_MDBT50Q.png` | Módulo MDBT50Q-1MV2 (U5), VDD = VDDH = 3,3 V, desacople, *pull-ups* I2C/ALRT y señales EN_EXC, EN_BB, SPI y SWD |
| `conector_SWD_J6.png` | Conector de programación 2×5 de 1,27 mm |
| `maquina_de_estados_firmware.png` | Reposo → Despertar → Medir → Leer batería → Notificar BLE; comando 0x02 → modo *Ship* |
| `arquitectura_validacion_emulacion.png` | Entradas simuladas → firmware emulado (teléfono) → BLE real → aplicación (tablet) |
| `demostracion_emulador_y_tablet.png` | Emulador y aplicación funcionando ([video](https://youtu.be/Hf0qGaK_6uc)) |
| `app_pestana_panel.png` | Pestaña Panel de la aplicación |
| `consumo_en_reposo.png` | Reparto del consumo en reposo (≈ 36,9 µA) |
| `autonomia_vs_periodo.png` | Autonomía en función del periodo de medición |
