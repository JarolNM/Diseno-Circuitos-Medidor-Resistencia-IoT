// =====================================================================
//  config.h  -  Pines y constantes del medidor de baja resistencia
//  Los pines son numeros de GPIO del nRF52840 (puerto P0), NO numeros
//  de pin de Arduino: el firmware programa los perifericos por registro
//  y no depende de la "variant" de la tarjeta elegida en el IDE.
// =====================================================================
#pragma once
#include <stdint.h>

// ---- Asignacion de pines (Tabla de pines del informe) ---------------
#define PIN_EN_EXC   2u    // P0.02 -> ON del TPS22916B (activo en alto)
#define PIN_ALRT     3u    // P0.03 <- ALRT del MAX17335 (drenador abierto, pull-up 10 k)
#define PIN_EN_BB    4u    // P0.04 -> EN del TPS63802 (riel de excitacion 3,0 V)
#define PIN_SCLK    14u    // P0.14 -> SCLK del ADS1220
#define PIN_MOSI    15u    // P0.15 -> DIN  del ADS1220
#define PIN_MISO    16u    // P0.16 <- DOUT/DRDY del ADS1220
#define PIN_CS      17u    // P0.17 -> CS   del ADS1220 (activo en bajo)
#define PIN_DRDY    20u    // P0.20 <- DRDY del ADS1220 (activo en bajo)
#define PIN_SDA     26u    // P0.26 <> SDA  del MAX17335 (pull-up 1,5 k)
#define PIN_SCL     27u    // P0.27 -> SCL  del MAX17335 (pull-up 1,5 k)

// ---- Medicion --------------------------------------------------------
#define R_REF_OHM          1.2f    // Resistencia de referencia (0,05 %)
#define FONDO_ESCALA_OHM   1.2f    // R_REF / G con G = 1
#define T_ARRANQUE_BB_MS   5u      // EN_BB -> EN_EXC (arranque del TPS63802)
#define T_ASENTAMIENTO_MS  5u      // EN_EXC -> primera conversion
#define N_CONVERSIONES     4u      // conversiones promediadas por medicion
#define T_DRDY_MAX_MS      10u     // tiempo maximo de espera de DRDY
#define I_EXC_MIN_A        0.5f    // por debajo: circuito abierto / sin excitacion
#define V_CORTE_MV         3000u   // V_CELL minima para medir

// ---- Medicion automatica ----------------------------------------------
#define PERIODO_DEF_S      60u
#define PERIODO_MIN_S      5u
#define PERIODO_MAX_S      255u
#define T_BATERIA_MS       60000u  // actualizacion de bateria sin medir

// ---- Gestor de bateria -------------------------------------------------
#define R_SENSE_MOHM       10.0f   // R_SENSE del MAX17335
#define PCKP_CARGADOR_MV   4500u   // PCKP por encima de esto = cargador conectado
#define I_CARGANDO_MA      30.0f   // corriente que se considera "cargando"

// ---- BLE ---------------------------------------------------------------
#define NOMBRE_BLE         "Medidor-R"
#define ADV_INTERVALO_MS   1000u   // publicidad cada 1 s (presupuesto de consumo)
#define TX_POTENCIA_DBM    0
