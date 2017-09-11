void setup()
{
  Serial.begin(500000);
  Serial.println("\n");

  int ret = startLoRa();
  if (!ret)
  {
    Serial.println("Cannot start LoRa!");
  } else
  {
    Serial.println("LoRa started.");
  }
}



void loop()
{ 
  // check for commands on Serial
  if (Serial) checkForCommand(Serial);

  // parse received telemetry data and write it to Serial
  parseLoRaData(Serial);
}

