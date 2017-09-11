#include <LoRa.h>

#define LORA_FREQUENCY  866E6 // default: 866E6
#define LORA_SS_PIN     4     // default: 10
#define LORA_RESET_PIN  5     // default: 9


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
 * sends command string 
 */
int sendTelemetry(float microsRunning, 
  float ax, float ay, float az, float gx, float gy, float gz, 
  float mx, float my, float mz,float gtemp, float mtemp, 
  float pitchRaw, float rollRaw, float yawRaw, float rollMadgwick, float pitchMadgwick, float yawMadgwick, 
  float valueRawAdcCh1, float valueRawAdcCh2, 
  float MPX2010valueKpa, float MPXHZ6115valueKpa,float MPX2010valueKpaKalman, float MPXHZ6115valueKpaKalman, 
  float AltitudeFromPressure, float WindSpeedFromPressure) 
{
  LoRa.beginPacket();
  LoRa.print(LORA_PACKET_MAGIC);
  LoRa.write(LORA_PACKET_TELEMETRY_TYPE);

  // temporary copied from SDlog.ino
  LoRa.print((float)((float)microsRunning/1000000),2);
  LoRa.print(";");
  
  LoRa.print(ax);
  LoRa.print(";");
  LoRa.print(ay);
  LoRa.print(";");
  LoRa.print(az);
  LoRa.print(";");
  
  LoRa.print(gx);
  LoRa.print(";");
  LoRa.print(gy);
  LoRa.print(";");
  LoRa.print(gz);
  LoRa.print(";");
  
  LoRa.print(mx);
  LoRa.print(";");
  LoRa.print(my);
  LoRa.print(";");
  LoRa.print(mz);
  LoRa.print(";");
  
  LoRa.print(gtemp);
  LoRa.print(";");
  LoRa.print(mtemp);
  LoRa.print(";");
  
  LoRa.print(pitchRaw);
  LoRa.print(";");
  LoRa.print(rollRaw);
  LoRa.print(";");
  LoRa.print(yawRaw);
  LoRa.print(";");
  
  LoRa.print(rollMadgwick);
  LoRa.print(";");
  LoRa.print(pitchMadgwick);
  LoRa.print(";");
  LoRa.print(yawMadgwick);
  LoRa.print(";");
  
  LoRa.print(valueRawAdcCh1);
  LoRa.print(";");
  LoRa.print(valueRawAdcCh2);
  LoRa.print(";");
  
  LoRa.print(MPX2010valueKpa);
  LoRa.print(";");
  LoRa.print(MPXHZ6115valueKpa);
  LoRa.print(";");
  LoRa.print(MPX2010valueKpaKalman);
  LoRa.print(";");
  LoRa.print(MPXHZ6115valueKpaKalman);
  LoRa.print(";");
  LoRa.print(AltitudeFromPressure);
  LoRa.print(";");
  LoRa.print(WindSpeedFromPressure);
  
  LoRa.println(";");

  return LoRa.endPacket();
}

int sendStatus()
{
  LoRa.beginPacket();
  LoRa.print(LORA_PACKET_MAGIC);
  LoRa.write(LORA_PACKET_STATUS_TYPE);

  // TODO: paste rocket status data here
  LoRa.write(STAT);
  return LoRa.endPacket();
}

/*
 * parses lora data and saves it to the Stream (eg. SD)
 */
void parseLoRaData(Stream &s)
{
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0) return;

  // check for magic string
  for (int i = 0; LORA_PACKET_MAGIC[i] != 0; i++)
  {
    if (LoRa.read() != LORA_PACKET_MAGIC[i]) return;
  }

  byte type = LoRa.read();

  s.printf("Received packet (0x%2x). Length: %d bytes, RSSI: %d, SNR: %d\n", 
             type, LoRa.available(), LoRa.packetRssi(), LoRa.packetSnr());
  
  switch (type)
  {
    /*
    case LORA_PACKET_TELEMETRY_TYPE:
      // telemetry
      // impossible in LoRa_Rocket
      break;
    case LORA_PACKET_STATUS_TYPE:
      // status
      // impossible in LoRa_Rocket      
      break;
    //*/
    case LORA_PACKET_COMMAND_TYPE:
      // command
      byte cmdType = LoRa.read();
      if (cmdType == LORA_START_CMD_CODE)
      {
        // TODO: ignition function here
        STAT=1;
        s.println("Ignition...");
      } else
      if (cmdType == LORA_DEPLOY_CMD_CODE)
      {
        // TODO: parachute deployment function here
        STAT=2;
        s.println("Parachute deploy...");
      } 

      // sending status back
      sendStatus();
      break;
  }
}

