# Etapas IV-A y IV-B — Fuente de excitación de 1 A y adquisición (ADS1220)

Parte del proyecto **Medidor de resistencia de baja magnitud IoT** ([rama principal](https://github.com/JarolNM/Diseno-Circuitos-Medidor-Resistencia-IoT)).

| Carpeta | Contenido |
|---|---|
| `01_ADS1220_Solucion_Seleccionada/Datasheets_y_Notas_de_Aplicacion` | Nota SBAA275A de TI y fragmentos del ADS1220 usados en el informe |
| `01_ADS1220_Solucion_Seleccionada/Sustentacion_Calculos` | Ecuaciones y script: fuente de 1 A, presupuesto de voltaje, filtros, ecuación de medición, ENOB, presupuesto de error, rango; calculadora de TI |
| `01_ADS1220_Solucion_Seleccionada/Esquematicos_y_Simulaciones` | Esquemáticos de la fuente y del ADC, simulación PSpice y diagrama de tiempos |
| `02_Topologia_Lado_Bajo_Descartada` | Medición referida a tierra y por qué se descartó |
| `03_Alternativa_RTD_3_Hilos_Descartada` | Configuración de 3 hilos con IDAC y por qué se descartó |
| `04_Matriz_Comparativa_Otros_ADCs` | ADS1220 frente a ADS124S08, HX711, ADS1115 y el SAADC del nRF52840 |

**Resultado:** rango útil de 1 mΩ a 1,2 Ω, resolución de 10 µΩ (≈18 bits efectivos), exactitud de ±0,058 % típica y ±0,19 % en el peor caso en 1 Ω.
