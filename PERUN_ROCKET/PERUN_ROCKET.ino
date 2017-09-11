#define LORA_TELEMETRY_INTERVAL 5000

uint64_t lastTelemetrySentTime;

void setup()
{
  Serial.begin(115200);
  Serial.println("\n");

  int ret = startLoRa();
  if (!ret)
  {
    Serial.println("Cannot start LoRa!");
  } else
  {
    Serial.println("LoRa started.");
  }

  lastTelemetrySentTime = millis();
}

// TEMP
int STAT = 0;

void loop()
{
  unsigned long microsRunning = micros();

  //*
  // TODO: replace test data with actual telemetry data functions
  double PresureMPX2010Kalman = 100012.3, PresureMPXHZ6115Kalman = 100045.6;
  float AltitudeFromPressure = 234.56;
  float WindSpeedFromPressure = 12.34;
  float ax_=0,ay_=1,az_=2,gx_=3,gy_=4,gz_=5,mx_=6,my_=7,mz_=8;
  float temp_m=123,temp_g=456;
  float rollMadgwick=0.1, pitchMadgwick=0.2, yawMadgwick=0.3;
  float rollRaw=0.11, pitchRaw=0.12, yawRaw=0.13;
  int16_t valueRawAdcCh1=1024,valueRawAdcCh2 = 0;
  float MPX2010valueKpa = 123456.789;
  float MPXHZ6115valueKpa = 123654.654;  
  //* XXX   T E S T    D A T A   XXX */
  
  // send telemetry every LORA_TELEMETRY_INTERVAL ms
  if (millis() - lastTelemetrySentTime >= LORA_TELEMETRY_INTERVAL)
  {
    sendTelemetry(microsRunning,
      ax_,ay_,az_,gx_,gy_,gz_,mx_,my_,mz_,temp_g,temp_m,
      pitchRaw,rollRaw,yawRaw,rollMadgwick,pitchMadgwick,yawMadgwick,
      valueRawAdcCh1,valueRawAdcCh2, MPX2010valueKpa,MPXHZ6115valueKpa,
      PresureMPX2010Kalman, PresureMPXHZ6115Kalman, 
      AltitudeFromPressure, WindSpeedFromPressure);
    
    lastTelemetrySentTime = millis();
  }

  parseLoRaData(Serial);
}
