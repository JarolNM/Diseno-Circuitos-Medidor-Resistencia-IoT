// =====================================================================
//  Medidor de resistencia de baja magnitud IoT - Firmware nRF52840
//  Modulo Raytac MDBT50Q-1MV2 | Arduino + Adafruit nRF52 BSP (Bluefruit)
//
//  Medicion ratiometrica Kelvin de 4 hilos:
//    EN_BB (TPS63802) -> 5 ms -> EN_EXC (pulso 1 A) -> 5 ms ->
//    4 conversiones ADS1220 (330 SPS) -> monitor de V_REF -> apagado
//    R_test = codigo_promedio * R_REF / 2^23   (G = 1)
//
//  Servicio GATT 5f1cXXXX-7a3b-4e2d-9c81-3b6f0a2d4e10:
//    0001 Resistencia  float32 (ohm)  NaN = sin medicion, -1 = bateria baja,
//                                     -2 = fuera de rango / circuito abierto
//    0002 SOC          uint8  (%)
//    0003 T. de carga  uint16 (min)   0xFFFF = no aplica
//    0004 Estado carga uint8  (0 sin cargador, 1 cargando, 2 completa)
//    0005 Comando      uint8 [+uint8] 01 medir, 02 apagar (Ship),
//                                     03/04 auto on/off, 05 T periodo (5-255 s)
//    0006 V_CELL       uint16 (mV)
//    0007 Config.      [auto 0/1, periodo s]
// =====================================================================
#include <bluefruit.h>
#include <math.h>
#include "config.h"
#include "hw_nrf.h"
#include "ads1220.h"
#include "max17335.h"

// ------------------------------------------------------------- UUIDs
// UUID base en orden little-endian; los bytes 12-13 llevan el XXXX.
static void uuidMedidor(uint16_t id, uint8_t out[16]) {
  static const uint8_t base[16] = { 0x10, 0x4E, 0x2D, 0x0A, 0x6F, 0x3B, 0x81, 0x9C,
                                    0x2D, 0x4E, 0x3B, 0x7A, 0x00, 0x00, 0x1C, 0x5F };
  memcpy(out, base, 16);
  out[12] = (uint8_t)(id & 0xFF);
  out[13] = (uint8_t)(id >> 8);
}

static uint8_t u_svc[16], u_res[16], u_soc[16], u_ttf[16], u_est[16], u_cmd[16], u_vc[16], u_cfg[16];
BLEService        *svc;
BLECharacteristic *chRes, *chSoc, *chTtf, *chEst, *chCmd, *chVcell, *chCfg;

// ------------------------------------------------------------- Estado
static volatile bool pedirMedicion = false;
static volatile bool pedirApagado  = false;
static volatile bool configCambio  = false;
static bool          autoActiva    = true;
static uint8_t       periodoS      = PERIODO_DEF_S;
static SemaphoreHandle_t despertar;            // lo libera un comando BLE

static bat::Estado bateria = {};

// ------------------------------------------------------------- Utilidades BLE
static void publicar(BLECharacteristic* c, const void* d, uint16_t n) {
  c->write(d, n);                               // valor para lecturas
  if (Bluefruit.connected()) c->notify(d, n);   // y notificacion si esta suscrito
}

static void publicarConfig() {
  uint8_t cfg[2] = { (uint8_t)(autoActiva ? 1 : 0), periodoS };
  publicar(chCfg, cfg, 2);
}

static void publicarBateria() {
  if (!bateria.ok) return;
  publicar(chSoc,   &bateria.soc, 1);
  publicar(chVcell, &bateria.vcellMv, 2);       // nRF52 es little-endian
  publicar(chEst,   &bateria.carga, 1);
  publicar(chTtf,   &bateria.ttfMin, 2);
}

// ------------------------------------------------------------- Medicion
// Devuelve R en ohm, o -1 (bateria baja) o -2 (fuera de rango / abierto).
static float medirResistencia() {
  if (bateria.ok && bateria.vcellMv < V_CORTE_MV) return -1.0f;

  gpioEscribir(PIN_EN_BB, true);                // arranca el riel de 3,0 V
  delay(T_ARRANQUE_BB_MS);
  gpioEscribir(PIN_EN_EXC, true);               // pulso de 1 A
  delay(T_ASENTAMIENTO_MS);                     // switch, lazo y filtros RC

  bool    ok   = ads::configurar(ads::REG_MEDIR);
  bool    sat  = false;
  int64_t suma = 0;
  uint8_t n    = 0;

  if (ok) {
    ads::comando(ads::CMD_START);
    for (uint8_t i = 0; i < N_CONVERSIONES; i++) {
      if (!ads::esperarDato(T_DRDY_MAX_MS)) { ok = false; break; }
      int32_t c = ads::leerDato();
      if (c >= 0x7FFF00 || c <= -0x7FFF00) sat = true;   // saturacion
      suma += c;
      n++;
    }
  }

  // Verificacion de la corriente: (V_REFP0 - V_REFN0)/4 contra 2,048 V
  float iExc = 0.0f;
  if (ok && ads::configurar(ads::REG_MONITOR)) {
    ads::comando(ads::CMD_START);
    if (ads::esperarDato(T_DRDY_MAX_MS)) {
      float vRef = 4.0f * ads::leerDato() * 2.048f / 8388608.0f;
      iExc = vRef / R_REF_OHM;
    }
  }

  gpioEscribir(PIN_EN_EXC, false);              // fin del pulso
  gpioEscribir(PIN_EN_BB, false);               // apaga el TPS63802
  ads::comando(ads::CMD_PWRDN);
  ads::configurar(ads::REG_MEDIR);              // deja la config. lista

  if (!ok || n == 0 || sat || iExc < I_EXC_MIN_A) return -2.0f;

  float r = (float)((double)suma / n) * R_REF_OHM / 8388608.0f;   // Ec. ratiometrica, G = 1
  if (r < -0.001f || r >= FONDO_ESCALA_OHM * 0.999f) return -2.0f;
  if (r < 0.0f) r = 0.0f;
  return r;
}

// ------------------------------------------------------------- Callbacks BLE
static void alEscribirComando(uint16_t, BLECharacteristic*, uint8_t* d, uint16_t len) {
  if (len < 1) return;
  switch (d[0]) {
    case 0x01: pedirMedicion = true; break;
    case 0x02: pedirApagado  = true; break;
    case 0x03: autoActiva = true;  configCambio = true; break;
    case 0x04: autoActiva = false; configCambio = true; break;
    case 0x05:
      if (len >= 2) {
        uint8_t t = d[1];
        periodoS = t < PERIODO_MIN_S ? PERIODO_MIN_S : t;   // uint8: maximo 255
        configCambio = true;
      }
      break;
    default: return;
  }
  xSemaphoreGive(despertar);
}

static void alConectar(uint16_t) {
  configCambio = true;                          // refresca todo al conectar
  xSemaphoreGive(despertar);
}

// ------------------------------------------------------------- BLE: servicio
static BLECharacteristic* crearCaracteristica(uint8_t* uuid, uint8_t props, uint16_t len, bool fija) {
  BLECharacteristic* c = new BLECharacteristic(BLEUuid(uuid));
  c->setProperties(props);
  c->setPermission(SECMODE_OPEN, (props & (CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP)) ? SECMODE_OPEN
                                                                                     : SECMODE_NO_ACCESS);
  if (fija) c->setFixedLen(len); else c->setMaxLen(len);
  return c;
}

static void iniciarBle() {
  Bluefruit.autoConnLed(false);                 // sin LED de conexion
  Bluefruit.begin();
  Bluefruit.setTxPower(TX_POTENCIA_DBM);
  Bluefruit.setName(NOMBRE_BLE);
  sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);   // regulador DC/DC interno (REG1)
  Bluefruit.Periph.setConnectCallback(alConectar);

  uuidMedidor(0x0000, u_svc); uuidMedidor(0x0001, u_res); uuidMedidor(0x0002, u_soc);
  uuidMedidor(0x0003, u_ttf); uuidMedidor(0x0004, u_est); uuidMedidor(0x0005, u_cmd);
  uuidMedidor(0x0006, u_vc);  uuidMedidor(0x0007, u_cfg);

  svc = new BLEService(BLEUuid(u_svc));
  svc->begin();

  const uint8_t RN = CHR_PROPS_READ | CHR_PROPS_NOTIFY;
  chRes   = crearCaracteristica(u_res, RN, 4, true);  chRes->begin();
  chSoc   = crearCaracteristica(u_soc, RN, 1, true);  chSoc->begin();
  chTtf   = crearCaracteristica(u_ttf, RN, 2, true);  chTtf->begin();
  chEst   = crearCaracteristica(u_est, RN, 1, true);  chEst->begin();
  chCmd   = crearCaracteristica(u_cmd, CHR_PROPS_WRITE | CHR_PROPS_WRITE_WO_RESP, 2, false);
  chCmd->setWriteCallback(alEscribirComando);
  chCmd->begin();
  chVcell = crearCaracteristica(u_vc,  RN, 2, true);  chVcell->begin();
  chCfg   = crearCaracteristica(u_cfg, RN, 2, true);  chCfg->begin();

  float nan = NAN;
  chRes->write(&nan, 4);                        // sin medicion desde el encendido

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addService(*svc);       // la app filtra por este UUID
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setIntervalMS(ADV_INTERVALO_MS, ADV_INTERVALO_MS);
  Bluefruit.Advertising.start(0);               // anuncia siempre
}

// ------------------------------------------------------------- Apagado
static void apagarEquipo() {
  gpioEscribir(PIN_EN_EXC, false);
  gpioEscribir(PIN_EN_BB, false);
  ads::comando(ads::CMD_PWRDN);
  Bluefruit.Advertising.restartOnDisconnect(false);
  Bluefruit.Advertising.stop();
  if (Bluefruit.connected()) Bluefruit.disconnect(Bluefruit.connHandle());
  delay(200);
  bat::entrarShip();                            // el MAX17335 corta SYS+
  while (true) delay(1000);                     // espera a quedarse sin energia
}

// ------------------------------------------------------------- Arduino
void setup() {
  hwIniciarPines();
  despertar = xSemaphoreCreateBinary();
  ads::iniciar();
  bateria = bat::leer();
  iniciarBle();
  publicarBateria();
  publicarConfig();
}

void loop() {
  static uint32_t ultimaMedicion = 0;
  static uint32_t ultimaBateria  = 0;
  uint32_t ahora = millis();

  // Espera dormido (System ON, tickless idle de FreeRTOS) hasta el
  // siguiente evento: comando BLE, medicion periodica o lectura de bateria.
  uint32_t espera = T_BATERIA_MS - (ahora - ultimaBateria);
  if (autoActiva) {
    uint32_t transcurrido = ahora - ultimaMedicion;
    uint32_t periodoMs    = (uint32_t)periodoS * 1000u;
    uint32_t faltaMedir   = transcurrido >= periodoMs ? 0 : periodoMs - transcurrido;
    if (faltaMedir < espera) espera = faltaMedir;
  }
  if (espera > T_BATERIA_MS) espera = 0;        // desborde: ya se cumplio el plazo
  if (espera > 0) xSemaphoreTake(despertar, ms2tick(espera));

  ahora = millis();
  if (pedirApagado) apagarEquipo();

  bool tocaBateria = (ahora - ultimaBateria) >= T_BATERIA_MS;
  bool tocaMedir   = pedirMedicion ||
                     (autoActiva && (ahora - ultimaMedicion) >= (uint32_t)periodoS * 1000u);

  if (configCambio) {
    configCambio = false;
    publicarConfig();
    tocaBateria = true;
  }

  if (tocaMedir || tocaBateria) {
    bat::Estado b = bat::leer();
    if (b.ok) bateria = b;
    publicarBateria();
    ultimaBateria = ahora;
  }

  if (tocaMedir) {
    pedirMedicion  = false;
    ultimaMedicion = ahora;
    float r = medirResistencia();
    publicar(chRes, &r, 4);
  }
}
