#include <Arduino.h>
#include <RadioLib.h>

// XL1262 LoRa pin mapping to ESP32-C3
#define PIN_LORA_SCK   10
#define PIN_LORA_MISO  5
#define PIN_LORA_MOSI  6
#define PIN_LORA_CS    7
#define PIN_LORA_RESET 8
#define PIN_LORA_BUSY  3
#define PIN_LORA_DIO1  4

// radio configuration
#define RF_FREQUENCY                                915000000
#define TX_OUTPUT_POWER                             14
#define MIN_TX_OUTPUT_POWER                         2
#define LORA_BANDWIDTH                              0
#define LORA_SPREADING_FACTOR                       9
#define LORA_CODINGRATE                             1
#define LORA_PREAMBLE_LENGTH                        8
#define LORA_SYMBOL_TIMEOUT                         0
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

// initialize the RadioLib SX1262 instance using ESP32 hardware SPI
SX1262 radio = new Module(PIN_LORA_CS, PIN_LORA_DIO1, PIN_LORA_BUSY, PIN_LORA_RESET);

void setup() {
  Serial.begin(115200);
  while(!Serial);

  // start SPI
  SPI.begin(PIN_LORA_SCK, PIN_LORA_MISO, PIN_LORA_MOSI, PIN_LORA_CS);

  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success"));
  } else {
    Serial.print(F("failed, code ")); Serial.println(state);
    while (true) { delay(10); }
  }

  // apply requested configuration
  radio.setFrequency(RF_FREQUENCY / 1000000.0);
  radio.setBandwidth(LORA_BANDWIDTH == 0 ? 125.0 : (LORA_BANDWIDTH == 1 ? 250.0 : 500.0));
  radio.setSpreadingFactor(LORA_SPREADING_FACTOR);
  radio.setCodingRate(LORA_CODINGRATE + 4);
  radio.setOutputPower(TX_OUTPUT_POWER);
  radio.setPreambleLength(LORA_PREAMBLE_LENGTH);
  radio.invertIQ(LORA_IQ_INVERSION_ON);
  if(LORA_FIX_LENGTH_PAYLOAD_ON) {
    radio.implicitHeader(LORA_SYMBOL_TIMEOUT);
  } else {
    radio.explicitHeader();
  }
}

void loop() {
  Serial.print(F("[SX1262] Transmitting ... "));
  int state = radio.transmit("hello");
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println(F("OK"));
  } else {
    Serial.print(F("failed, code ")); Serial.println(state);
  }
  delay(5000);
}
