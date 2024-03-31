
// pio run -t upload && pio device monitor -b 115200

#include <Ra01S.h>

//#define RF_FREQUENCY                                433000000 // Hz  center frequency
//#define RF_FREQUENCY                                866000000 // Hz  center frequency
#define RF_FREQUENCY                              836000000 // Hz  center frequency
#define TX_OUTPUT_POWER                             22        // dBm tx output power
#define LORA_BANDWIDTH                              6         // bandwidth
                                                              // 2: 31.25Khz
                                                              // 3: 62.5Khz
                                                              // 4: 125Khz
                                                              // 5: 250KHZ
                                                              // 6: 500Khz                                                               
#define LORA_SPREADING_FACTOR                       12         // spreading factor [SF5..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]

#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_PAYLOADLENGTH                          0         // 0: Variable length packet (explicit header)
                                                              // 1..255  Fixed length packet (implicit header)

//#define USE_EBYTE

#if 0
/*
 * for ATmega328/2560
 * VCC    3V3/3V3
 * GND    GND/GND
 * SCK    13/52
 * MISO   12/50
 * MOSI   11/51
 * NSS     5/5
 * RST     6/6
 * BUSY    7/7
 * TXEN    8/8 for EBYTE
 * RXEN    9/9 for EBYTE
 */

#ifdef USE_EBYTE
SX126x  lora(5,               //Port-Pin Output: SPI select
             6,               //Port-Pin Output: Reset 
             7,               //Port-Pin Input:  Busy
             8,               //Port-Pin Output: TXEN
             9                //Port-Pin Output: RXEN
             );

#else
SX126x  lora(5,               //Port-Pin Output: SPI select
             6,               //Port-Pin Output: Reset 
             7                //Port-Pin Input:  Busy
             );
#endif // USE_EBYTE

#endif // ATmega328/2560

#if 1
/*
 * for ESP8266
 * VCC    3V3
 * GND    GND
 * SCK    GPIO14
 * MISO   GPIO12
 * MOSI   GPIO13
 * NSS    GPIO2
 * RST    GPIO0
 * BUSY   GPIO16
 * TXEN   GPIO4 for EBYTE
 * RXEN   GPIO5 for EBYTE
 */

#ifdef USE_EBYTE
SX126x  lora(2,               //Port-Pin Output: SPI select
             0,               //Port-Pin Output: Reset 
             16,              //Port-Pin Input:  Busy
             4,               //Port-Pin Output: TXEN
             5                //Port-Pin Output: RXEN
             );
#else
SX126x  lora(15,               //Port-Pin Output: SPI select
             4,               //Port-Pin Output: Reset 
             5               //Port-Pin Input:  Busy
             );
#endif // USE_EBYTE

#endif // ESP8266

void setup() 
{
  delay(1000);
  Serial.begin(115200);

  lora.DebugPrint(true);

#ifdef USE_EBYTE
  Serial.println("Enable TCXO");
  int16_t ret = lora.begin(RF_FREQUENCY,              //frequency in Hz
                           TX_OUTPUT_POWER,           //tx power in dBm
                           3.3,                       //use TCXO
                           true);                     //use TCXO
  if (ret != ERR_NONE) while(1) {delay(1);}
#else
  Serial.println("Disable TCXO");
  int16_t ret = lora.begin(RF_FREQUENCY,              //frequency in Hz
                           TX_OUTPUT_POWER);          //tx power in dBm
  if (ret != ERR_NONE) while(1) {delay(1);}
#endif // USE_EBYTE

  lora.LoRaConfig(LORA_SPREADING_FACTOR, 
                  LORA_BANDWIDTH, 
                  LORA_CODINGRATE, 
                  LORA_PREAMBLE_LENGTH, 
                  LORA_PAYLOADLENGTH, 
                  true,               //crcOn  
                  false);             //invertIrq

}

void loop() 
{
  uint8_t txData[255];
  sprintf((char *)txData, "Hello World %lu", millis());
  uint8_t len = strlen((char *)txData);

  // Wait for transmission to complete
  if (lora.Send(txData, len, SX126x_TXMODE_SYNC)) {
    Serial.println("Send success");
  } else {
    Serial.println("Send fail");
  }

  // Do not wait for the transmission to be completed
  // lora.Send(txData, len, SX126x_TXMODE_ASYNC );

  delay(10000);
}
