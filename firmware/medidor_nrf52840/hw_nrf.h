// =====================================================================
//  hw_nrf.h  -  Acceso directo a GPIO, SPIM2 y TWIM0 del nRF52840
//  Se usa acceso por registro para que los pines sean los del
//  esquematico sin depender de la tabla de pines de la "variant".
// =====================================================================
#pragma once
#include <Arduino.h>
#include "config.h"

// ---------------------------------------------------------------- GPIO
static inline void gpioEscribir(uint32_t pin, bool nivel) {
  if (nivel) NRF_P0->OUTSET = (1UL << pin);
  else       NRF_P0->OUTCLR = (1UL << pin);
}

static inline void gpioSalida(uint32_t pin, bool nivel) {
  gpioEscribir(pin, nivel);
  NRF_P0->PIN_CNF[pin] = (GPIO_PIN_CNF_DIR_Output      << GPIO_PIN_CNF_DIR_Pos)  |
                         (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos) |
                         (GPIO_PIN_CNF_PULL_Disabled    << GPIO_PIN_CNF_PULL_Pos)  |
                         (GPIO_PIN_CNF_DRIVE_S0S1       << GPIO_PIN_CNF_DRIVE_Pos);
}

static inline void gpioEntrada(uint32_t pin, uint32_t pull, uint32_t drive = GPIO_PIN_CNF_DRIVE_S0S1) {
  NRF_P0->PIN_CNF[pin] = (GPIO_PIN_CNF_DIR_Input     << GPIO_PIN_CNF_DIR_Pos)  |
                         (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                         (pull                       << GPIO_PIN_CNF_PULL_Pos)  |
                         (drive                      << GPIO_PIN_CNF_DRIVE_Pos);
}

static inline bool gpioLeer(uint32_t pin) { return (NRF_P0->IN >> pin) & 1UL; }

// Anomalia 89 del nRF52840 (corriente estatica de ~400 uA tras usar
// SPIM/TWIM): se apaga y enciende el periferico al terminar.
static inline void ciclarPeriferico(volatile void* base) {
  volatile uint32_t* power = (volatile uint32_t*)((uint32_t)base + 0xFFC);
  *power = 0; (void)*power; *power = 1;
}

// ---------------------------------------------------------------- SPIM2
// Modo 1 (CPOL = 0, CPHA = 1), MSB primero, 1 MHz. Transferencia en el
// mismo buffer (full duplex). Maximo 8 bytes por transaccion.
static inline bool spiTransferir(uint8_t* buf, uint8_t n) {
  static uint8_t tx[8], rx[8];          // EasyDMA solo accede a RAM
  if (n == 0 || n > sizeof(tx)) return false;
  memcpy(tx, buf, n);

  NRF_SPIM_Type* s = NRF_SPIM2;
  s->PSEL.SCK  = PIN_SCLK;
  s->PSEL.MOSI = PIN_MOSI;
  s->PSEL.MISO = PIN_MISO;
  s->FREQUENCY = SPIM_FREQUENCY_FREQUENCY_M1;
  s->CONFIG    = (SPIM_CONFIG_ORDER_MsbFirst  << SPIM_CONFIG_ORDER_Pos) |
                 (SPIM_CONFIG_CPHA_Trailing   << SPIM_CONFIG_CPHA_Pos)  |
                 (SPIM_CONFIG_CPOL_ActiveHigh << SPIM_CONFIG_CPOL_Pos);
  s->ORC        = 0x00;
  s->TXD.PTR    = (uint32_t)tx;  s->TXD.MAXCNT = n;
  s->RXD.PTR    = (uint32_t)rx;  s->RXD.MAXCNT = n;
  s->ENABLE     = SPIM_ENABLE_ENABLE_Enabled;
  s->EVENTS_END = 0;
  s->TASKS_START = 1;

  uint32_t t0 = millis();
  bool ok = true;
  while (!s->EVENTS_END) {
    if (millis() - t0 > 5) { s->TASKS_STOP = 1; ok = false; break; }
  }
  s->EVENTS_END = 0;
  s->ENABLE = SPIM_ENABLE_ENABLE_Disabled;
  ciclarPeriferico(s);
  memcpy(buf, rx, n);
  return ok;
}

// ---------------------------------------------------------------- TWIM0
// I2C a 400 kHz. Los registros del MAX17335 son de 16 bits, LSB primero.
static inline bool i2cEjecutar(uint8_t dir7, uint8_t* tx, uint8_t ntx,
                               uint8_t* rx, uint8_t nrx) {
  static uint8_t btx[4], brx[4];
  if (ntx > sizeof(btx) || nrx > sizeof(brx)) return false;
  memcpy(btx, tx, ntx);

  NRF_TWIM_Type* t = NRF_TWIM0;
  t->PSEL.SCL  = PIN_SCL;
  t->PSEL.SDA  = PIN_SDA;
  t->FREQUENCY = TWIM_FREQUENCY_FREQUENCY_K400;
  t->ADDRESS   = dir7;
  t->TXD.PTR   = (uint32_t)btx;  t->TXD.MAXCNT = ntx;
  t->RXD.PTR   = (uint32_t)brx;  t->RXD.MAXCNT = nrx;
  t->SHORTS    = (nrx > 0) ? (TWIM_SHORTS_LASTTX_STARTRX_Msk | TWIM_SHORTS_LASTRX_STOP_Msk)
                           :  TWIM_SHORTS_LASTTX_STOP_Msk;
  t->ERRORSRC  = t->ERRORSRC;            // limpia errores previos
  t->EVENTS_STOPPED = 0;
  t->EVENTS_ERROR   = 0;
  t->ENABLE    = TWIM_ENABLE_ENABLE_Enabled;
  t->TASKS_STARTTX = 1;

  uint32_t t0 = millis();
  bool ok = true;
  while (!t->EVENTS_STOPPED) {
    if (t->EVENTS_ERROR) { t->EVENTS_ERROR = 0; t->TASKS_STOP = 1; ok = false; }
    if (millis() - t0 > 5) { t->TASKS_STOP = 1; ok = false; break; }
  }
  if (t->ERRORSRC) ok = false;
  t->EVENTS_STOPPED = 0;
  t->SHORTS = 0;
  t->ENABLE = TWIM_ENABLE_ENABLE_Disabled;
  ciclarPeriferico(t);
  if (ok && nrx) memcpy(rx, brx, nrx);
  return ok;
}

static inline void hwIniciarPines() {
  gpioSalida(PIN_EN_EXC, false);        // pulso apagado
  gpioSalida(PIN_EN_BB,  false);        // TPS63802 apagado
  gpioSalida(PIN_CS,     true);         // ADS1220 sin seleccionar
  gpioSalida(PIN_SCLK,   false);        // reposo de SCLK en modo 1
  gpioSalida(PIN_MOSI,   false);
  gpioEntrada(PIN_MISO, GPIO_PIN_CNF_PULL_Disabled);
  gpioEntrada(PIN_DRDY, GPIO_PIN_CNF_PULL_Disabled);
  gpioEntrada(PIN_ALRT, GPIO_PIN_CNF_PULL_Disabled);   // pull-up externo
  gpioEntrada(PIN_SDA,  GPIO_PIN_CNF_PULL_Disabled, GPIO_PIN_CNF_DRIVE_S0D1);
  gpioEntrada(PIN_SCL,  GPIO_PIN_CNF_PULL_Disabled, GPIO_PIN_CNF_DRIVE_S0D1);
}
