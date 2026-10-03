# Alternativa de 3 hilos (estilo RTD, IDAC del ADS1220) — descartada

## Qué es
La configuración de 3 hilos de la nota de TI SBAA275A: dos fuentes IDAC **iguales** del ADS1220 inyectan corriente por dos hilos, y la caída del hilo de retorno se compensa porque ambas corrientes recorren hilos de igual resistencia ([2, 3 y 4 hilos](https://github.com/JarolNM/Diseno-Circuitos-Medidor-Resistencia-IoT/blob/etapa-medicion/01_ADS1220_Solucion_Seleccionada/Datasheets_y_Notas_de_Aplicacion/Configuraciones%20de%202%2C%203%20y%204%20hilos.pdf), [SBAA275A](https://www.ti.com/lit/an/sbaa275a/sbaa275a.pdf)).

## Por qué se descartó
| Problema | Efecto en una resistencia de 1 Ω |
|---|---|
| IDAC máximo de 1,5 mA | Señal de solo 1,5 mV a fondo de escala (frente a 1 V con 1 A): ~670 veces menos señal, y el ruido de 10 µV equivaldría a ≈7 mΩ |
| Desajuste entre las dos IDAC (del orden de 0,1 % o más) | La compensación del hilo deja de ser exacta; el error residual es del orden de mΩ |
| Requiere hilos de igual resistencia | Las pinzas y cables reales no lo garantizan |
| Exactitud del IDAC de ±6 % máx. | Obliga igualmente a medir contra una referencia |

Para RTD de 100 Ω funciona bien, porque la señal es de cientos de mV; para 1 Ω no alcanza la resolución de miliohmios.

## Solución adoptada
**Kelvin de 4 hilos** con excitación externa de **1 A** (OPA365 + CSD13380F3) y medición ratiométrica: la resistencia de los hilos de fuerza no entra en la medición y los de sensado solo conducen ≈15 nA ([Análisis de 4 hilos](https://github.com/JarolNM/Diseno-Circuitos-Medidor-Resistencia-IoT/blob/etapa-medicion/01_ADS1220_Solucion_Seleccionada/Datasheets_y_Notas_de_Aplicacion/An%C3%A1lisis%20de%204%20hilos.pdf)).
