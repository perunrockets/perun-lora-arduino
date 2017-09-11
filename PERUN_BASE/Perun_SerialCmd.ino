#define PERUN_CMD_PREFIX  "PR"
#define PERUN_START_CMD   PERUN_CMD_PREFIX"+START"
#define PERUN_DEPLOY_CMD  PERUN_CMD_PREFIX"+DEPLOY"
#define PERUN_LORA_CMD    PERUN_CMD_PREFIX"+LORA"

#include <Arduino.h>
#include <LoRa.h>

/*
   execute function based on command received from Serial
*/
int executeCommand(Stream &s, String cmd)
{
  if (cmd.equals(PERUN_START_CMD))
  {
    s.println("\t===PERUN START CMD===");
    int ret = sendCommand(LORA_START_CMD_CODE);
    if (!ret) {
      s.println("Cannot send start command!");
    }
  }
  else if (cmd.equals(PERUN_DEPLOY_CMD))
  {
    s.println("\t===PERUN DEPLOY CMD===");
    int ret = sendCommand(LORA_DEPLOY_CMD_CODE);
    if (!ret) {
      s.println("Cannot send deploy command!");
    }
  }
  else if (cmd.equals(PERUN_LORA_CMD))
  {
    s.println("\t===PERUN LORA CMD===");
    LoRa.dumpRegisters(s);
    s.println("\t===");
  }  
  else if (cmd.equals(PERUN_CMD_PREFIX))
  {
    s.println("OK");
  }
  else
  {
    s.println("Unknown command!\n");
    return 0;
  }

  s.flush();
  return 1;
}

/*
 * check for command on Stream (eg. Serial)
 */
int checkForCommand(Stream &s)
{
  if (s.available() > 0)
  {
    String cmd = s.readString();
    cmd.trim(); // remove newline character
    cmd.toUpperCase();
    return executeCommand(s, cmd);
  }
  return -1;
}

