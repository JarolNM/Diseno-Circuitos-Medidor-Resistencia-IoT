# Etapas I y II — Batería y gestión de batería (MAX17335)

Parte del proyecto **Medidor de resistencia de baja magnitud IoT** ([rama principal](https://github.com/JarolNM/Diseno-Circuitos-Medidor-Resistencia-IoT)).

| Carpeta | Contenido |
|---|---|
| `01_MAX17335_Solucion_Seleccionada/Datasheets_y_Notas_de_Aplicacion` | Datasheet del MAX17335 y de la celda M35A, y fragmentos usados en el informe |
| `01_MAX17335_Solucion_Seleccionada/Sustentacion_Calculos` | Ecuaciones y script de cálculo: capacidad útil, corriente de carga, FETs, *pull-ups* I2C |
| `01_MAX17335_Solucion_Seleccionada/Esquematicos_y_Simulaciones` | Recortes del esquemático de la etapa y de la entrada USB-C |
| `02_Cargadores_Lineales_Descartados` | MCP73831, BQ24075, TP4056 + DW01A y por qué se descartaron |
| `03_Fuel_Gauges_Alternativos_Descartados` | MAX17330, MAX17048, MAX17055, BQ27441-G1 y por qué se descartaron |

**Decisión:** celda Molicel INR-18650-M35A + MAX17335 (carga, protección y SOC en un solo IC), I_CHG = 1 A, R_SENSE = 10 mΩ, FETs FDPC4044.
