# Etapa III — Regulación de potencia (TPS63900 + TPS63802)

Parte del proyecto **Medidor de resistencia de baja magnitud IoT** ([rama principal](https://github.com/JarolNM/Diseno-Circuitos-Medidor-Resistencia-IoT)).

| Carpeta | Contenido |
|---|---|
| `01_TPS63900_Solucion_Seleccionada/Datasheets_y_Notas_de_Aplicacion` | Fragmentos del datasheet del TPS63900 usados en el informe |
| `01_TPS63900_Solucion_Seleccionada/Sustentacion_Calculos` | Configuración R2D, inductor, capacidad de los rieles, corriente de entrada, divisor del TPS63802, C_res y elección de 3,0 V |
| `01_TPS63900_Solucion_Seleccionada/Esquematicos_y_Simulaciones` | Recortes del esquemático de ambos convertidores |
| `02_Reguladores_Buck_Puros_Descartados` | TPS62840 y TPS62740: no dan 3,3 V con la celda por debajo de 3,3 V |
| `03_Buck_Boost_Convencionales_Descartados` | TPS63020 y TPS63001: I_Q demasiado alta para un riel siempre encendido |

**Decisión:** TPS63900 (75 nA) para el riel lógico de 3,3 V, siempre activo; TPS63802 (2 A) para el riel de excitación de 3,0 V, encendido solo durante el pulso.
