// =====================================================================
//  ads1220.h  -  Driver minimo del ADS1220 (SBAS501D)
//  Configuracion del informe: MUX = AIN0-AIN1, G = 1 con PGA en bypass,
//  330 SPS, conversion continua, referencia externa REFP0/REFN0.
// =====================================================================
#pragma once
#include "hw_nrf.h"

namespace ads {

const uint8_t CMD_RESET = 0x06, CMD_START = 0x08, CMD_PWRDN = 0x02, CMD_RDATA = 0x10;
const uint8_t CMD_WREG  = 0x40, CMD_RREG  = 0x20;

// Registros 0x00..0x03 para la medicion ratiometrica (Tabla del informe)
const uint8_t REG_MEDIR[4]   = { 0x01, 0x84, 0x40, 0x00 };
// Monitor (V(REFP0)-V(REFN0))/4 contra la referencia interna de 2,048 V:
// MUX = 1100, VREF = 00. Permite medir I_exc = V_REF / R_REF y detectar
// circuito abierto (sin corriente no hay tension en R_REF).
const uint8_t REG_MONITOR[4] = { 0xC0, 0x84, 0x00, 0x00 };

static inline void seleccionar(bool s) { gpioEscribir(PIN_CS, !s); }

static inline bool comando(uint8_t c) {
  uint8_t b = c;
  seleccionar(true);
  bool ok = spiTransferir(&b, 1);
  seleccionar(false);
  return ok;
}

// Escribe los 4 registros y los lee de vuelta para verificar el bus.
static inline bool configurar(const uint8_t reg[4]) {
  uint8_t w[5] = { (uint8_t)(CMD_WREG | 3), reg[0], reg[1], reg[2], reg[3] };
  seleccionar(true); bool ok = spiTransferir(w, 5); seleccionar(false);
  uint8_t r[5] = { (uint8_t)(CMD_RREG | 3), 0, 0, 0, 0 };
  seleccionar(true); ok &= spiTransferir(r, 5); seleccionar(false);
  return ok && memcmp(&r[1], reg, 4) == 0;
}

static inline bool esperarDato(uint32_t ms) {
  uint32_t t0 = millis();
  while (gpioLeer(PIN_DRDY)) {
    if (millis() - t0 > ms) return false;
  }
  return true;
}

// Lee un resultado de 24 bits en complemento a dos.
static inline int32_t leerDato() {
  uint8_t b[4] = { CMD_RDATA, 0, 0, 0 };
  seleccionar(true); spiTransferir(b, 4); seleccionar(false);
  int32_t v = ((int32_t)b[1] << 16) | ((int32_t)b[2] << 8) | b[3];
  if (v & 0x800000) v |= (int32_t)0xFF000000;
  return v;
}

static inline bool iniciar() {
  comando(CMD_RESET);
  delay(1);                                    // t > 50 us tras RESET
  bool ok = configurar(REG_MEDIR);
  comando(CMD_PWRDN);                          // reposo: 0,1 uA tipico
  return ok;
}

} // namespace ads
