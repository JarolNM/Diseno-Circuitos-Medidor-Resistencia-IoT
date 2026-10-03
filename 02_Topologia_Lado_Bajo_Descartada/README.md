# Topología de lado bajo — descartada

## Qué es
Conectar $R_{test}$ con un extremo a GND (lado bajo) y medir su tensión **referida a tierra** (*single-ended*), con la corriente de excitación entrando por el extremo superior y volviendo por el plano de tierra.

## Por qué se descartó
1. **La corriente de 1 A vuelve por GND.** Cualquier resistencia del retorno (pistas, vías, conector, pinza) aparece en serie con $R_{test}$ en la medición. Con 10 mΩ de retorno el error sería de 10 mΩ, el 1 % de la lectura en 1 Ω, mucho mayor que todo el presupuesto de error del diseño (≈0,58 mΩ).
2. **Se rompe la medición Kelvin.** La tensión ya no se toma en los terminales de $R_{test}$ sino entre un terminal y la tierra del sistema, donde se suman las caídas del retorno de potencia y del ruido del convertidor.
3. **Rango de modo común.** Con el PGA en *bypass* (necesario para G = 1 con una señal de 1 V), el ADS1220 admite entradas de AVSS − 0,1 V a AVDD + 0,1 V. Un nodo a 0 V con ruido de conmutación queda pegado al límite inferior ([rango de modo común](https://github.com/JarolNM/Diseno-Circuitos-Medidor-Resistencia-IoT/blob/etapa-medicion/01_ADS1220_Solucion_Seleccionada/Datasheets_y_Notas_de_Aplicacion/ADS_COMMON_MODE_RANGE.pdf)).
4. **No es ratiométrica sin más cambios.** Para cancelar la exactitud de la corriente hace falta medir también $R_{REF}$ con la misma corriente, en serie y con su propio par Kelvin.

## Solución adoptada
Medición **diferencial con conexión Kelvin de 4 hilos** y $R_{REF}=1{,}2\ \Omega$ en serie con la misma corriente: Sense+/Sense− van a AIN0/AIN1 y los extremos de $R_{REF}$ a REFP0/REFN0. Los nodos quedan en 2,93 / 1,93 / 0,73 V, lejos de los límites del ADC. Ver `01_ADS1220_Solucion_Seleccionada/Sustentacion_Calculos`.
