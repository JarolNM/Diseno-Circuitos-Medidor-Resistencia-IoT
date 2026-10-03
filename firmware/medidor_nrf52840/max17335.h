// =====================================================================
//  max17335.h  -  Lectura del MAX17335 por I2C y comando de modo Ship
//  Direcciones y escalas segun el datasheet del MAX17335:
//   - Direccion I2C 0x6C (8 bits) = 0x36 (7 bits) para 000h-0FFh.
//   - Registros de 16 bits, LSB primero.
//   - Voltaje 78,125 uV/LSB; porcentaje 1/256 %; tiempo 5,625 s/LSB;
//     corriente 1,5625 uV / R_SENSE (con 10 mOhm = 0,15625 mA/LSB).
// =====================================================================
#pragma once
#include "hw_nrf.h"

namespace bat {

const uint8_t DIR = 0x36;
const uint8_t REG_REPSOC   = 0x06;   // estado de carga
const uint8_t REG_CONFIG   = 0x0B;   // Config.SHIP = bit 7
const uint8_t REG_VCELL    = 0x1A;   // voltaje de celda
const uint8_t REG_CURRENT  = 0x1C;   // corriente instantanea
const uint8_t REG_AVGCURR  = 0x1D;   // corriente promedio
const uint8_t REG_TTF      = 0x20;   // tiempo a carga completa
const uint8_t REG_COMMSTAT = 0x61;   // proteccion de escritura
const uint8_t REG_PCKP     = 0xDB;   // voltaje de PACK+ (0,3125 mV/LSB)

struct Estado {
  bool     ok;
  uint16_t vcellMv;
  uint8_t  soc;          // %
  float    corrienteMa;  // + carga, - descarga
  uint16_t pckpMv;
  uint8_t  carga;        // 0 sin cargador, 1 cargando, 2 completa
  uint16_t ttfMin;       // 0xFFFF = no aplica
};

static inline bool leer16(uint8_t reg, uint16_t* v) {
  uint8_t r[2];
  if (!i2cEjecutar(DIR, &reg, 1, r, 2)) return false;
  *v = (uint16_t)r[0] | ((uint16_t)r[1] << 8);
  return true;
}

static inline bool escribir16(uint8_t reg, uint16_t v) {
  uint8_t w[3] = { reg, (uint8_t)(v & 0xFF), (uint8_t)(v >> 8) };
  return i2cEjecutar(DIR, w, 3, nullptr, 0);
}

static inline Estado leer() {
  Estado e = {};
  uint16_t soc, vc, cur, ttf, pckp;
  e.ok = leer16(REG_REPSOC, &soc) && leer16(REG_VCELL, &vc) &&
         leer16(REG_AVGCURR, &cur) && leer16(REG_TTF, &ttf) &&
         leer16(REG_PCKP, &pckp);
  if (!e.ok) return e;

  e.vcellMv     = (uint16_t)(((uint32_t)vc * 5u) / 64u);      // 78,125 uV = 5/64 mV
  uint32_t s = ((uint32_t)soc + 128u) >> 8;                    // redondeo a 1 %
  e.soc         = (uint8_t)(s > 100u ? 100u : s);
  e.corrienteMa = (int16_t)cur * (1.5625f / R_SENSE_MOHM);    // uV / mOhm = mA
  e.pckpMv      = (uint16_t)(((uint32_t)pckp * 5u) / 16u);     // 0,3125 mV = 5/16 mV

  bool cargador = e.pckpMv > PCKP_CARGADOR_MV;
  if (!cargador)                           e.carga = 0;
  else if (e.corrienteMa > I_CARGANDO_MA)  e.carga = 1;
  else                                     e.carga = 2;

  if (e.carga == 1 && ttf != 0xFFFF)
    { uint32_t m = ((uint32_t)ttf * 3u + 16u) / 32u;            // 5,625 s = 3/32 min
      e.ttfMin = (uint16_t)(m > 0xFFFEu ? 0xFFFEu : m); }
  else
    e.ttfMin = 0xFFFF;
  return e;
}

// Apagado: Config.SHIP = 1. El MAX17335 abre los FETs al vencer el
// Shutdown Timer (nDelayCfg.UVPTimer) y solo despierta con el cargador.
static inline bool entrarShip() {
  // Quitar proteccion de escritura: 0x0000 dos veces seguidas en CommStat
  bool ok = escribir16(REG_COMMSTAT, 0x0000) && escribir16(REG_COMMSTAT, 0x0000);
  uint16_t cfg;
  ok = ok && leer16(REG_CONFIG, &cfg);
  ok = ok && escribir16(REG_CONFIG, cfg | (1u << 7));
  return ok;
}

} // namespace bat
