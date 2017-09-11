#include <LoRa.h>

#define LORA_FREQUENCY  866E6 // default: 866E6
#define LORA_SS_PIN     5     // default: 10
#define LORA_RESET_PIN  17    // default: 9

#define LORA_PACKET_MAGIC "PERN_RCTS_LRZPR01"

#define LORA_PACKET_TELEMETRY_TYPE 0x01
#define LORA_PACKET_STATUS_TYPE    0x02
#define LORA_PACKET_COMMAND_TYPE   0xA1

#define LORA_START_CMD_CODE        0x41
#define LORA_DEPLOY_CMD_CODE       0x42
#define LORA_EXPLODE_CMD_CODE      0x66

/*
 * initializes LoRa
 */
int startLoRa()
{
  LoRa.setSPIFrequency(8E6);      // default 8E6 Hz

  // SlaveSelect, Reset, optional: Dio0(2)(received callback mode)
  LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, 2);

  int ret = LoRa.begin(LORA_FREQUENCY);

  //LoRa.setFrequency(LORA_FREQUENCY); // default: 866E6 Hz
  LoRa.setTxPower(17);            // default: 17dB
  LoRa.setSpreadingFactor(7);     // default: 7
  LoRa.setSignalBandwidth(125E3); // default: 125E3 Hz
  LoRa.setCodingRate4(5);         // default: 5
  LoRa.setPreambleLength(8);      // default: 8
  LoRa.setSyncWord(0x34);         // default: 0x34

  LoRa.disableCrc(); // default: disabled
  //LoRa.enableCrc();

  return ret;
}

/*
 * sends command 
 */
int sendCommand(int type)
{
  LoRa.beginPacket();
  LoRa.print(LORA_PACKET_MAGIC);
  LoRa.write(LORA_PACKET_COMMAND_TYPE);
  LoRa.write(type);
  return LoRa.endPacket();
}

/*
 * parses lora data and saves it to Stream (eg. SD)
 */
void parseLoRaData(Stream &s)
{
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0) return;

  //while (LoRa.available()) Serial.print((char)(LoRa.read()));

  // check for magic string
  for (int i = 0; LORA_PACKET_MAGIC[i] != 0; i++)
  {
    if (LoRa.read() != LORA_PACKET_MAGIC[i]) return;
  }

  byte type = LoRa.read();

  s.printf("Received packet (0x%x). Length: %d bytes, RSSI: %d, SNR: %d\n", 
             type, LoRa.available(), LoRa.packetRssi(), LoRa.packetSnr());
  
  switch (type)
  {
    case LORA_PACKET_TELEMETRY_TYPE:
      // telemetry
      while (LoRa.available()) s.print((char)(LoRa.read()) );
      break;
    case LORA_PACKET_STATUS_TYPE:
      // status
      int stat = (int)(LoRa.read());
      s.printf("Rocket status: %d\n", stat);
      break;

    /*
    case LORA_PACKET_COMMAND_TYPE:
      // command
      // impossible on LoRa_Base
      break;
    //*/
  }
}

