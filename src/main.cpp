#include <Homie.h>
#include <VitoWiFi.h>

HomieNode boilerNode("boiler", "boiler");
HomieNode heating1Node("heating1", "heating");
HomieNode storageTankNode("storageTank","storageTank");
std::map<std::string, HomieNode*> nodes;
VitoWiFi_setProtocol(P300);
DPTemp outsideTemp("outsideTemp", boilerNode.getId(), 0x5525);
DPTemp boilerWaterTemp("boilerTemp", boilerNode.getId(),0x0810);
DPHours burnerHoursRun("burnerHoursRun", boilerNode.getId(),0x08A7);
DPStat pumpInternalStat("internalPump", boilerNode.getId(), 0x7660);
DPRaw burnerStarts("burnerStarts", boilerNode.getId(),0x088A);

DPStat pumpStat("pump", heating1Node.getId(), 0x2906);
DPTemp hotWaterTemp("hotwatertemp", storageTankNode.getId(), 0x0812);


void globalCallbackHandler(const IDatapoint& dp, DPValue value) {
  std::map<std::string, HomieNode*>::iterator it = nodes.find(dp.getGroup()); 
  if(it != nodes.end()){
    char value_str[15] = {0};
    value.getString(value_str, sizeof(value_str));
    it->second->setProperty(dp.getName()).send(value_str);
  }
}

void setup() {
  burnerStarts.setLength(4);

  VitoWiFi.setGlobalCallback(globalCallbackHandler);  // this callback will be used for all DPs without specific callback
                                                      // must be set after adding at least 1 datapoint
  VitoWiFi.setup(&Serial);

  nodes[boilerNode.getId()] = &boilerNode;
  nodes[heating1Node.getId()] = &heating1Node;
  nodes[storageTankNode.getId()] = &storageTankNode;

  Homie.disableLogging();
  Homie_setFirmware("VitoWiFi", "1.0.2");
  Homie.setup();
}

void loop() {
  static unsigned long lastMillis = 0;
  static bool optolinkIsConnected = false;
  if (millis() - lastMillis > 60 * 1000UL) {  // read all values every 60 seconds
    lastMillis = millis();
    VitoWiFi.readAll();
  }
  VitoWiFi.loop();
  Homie.loop();
} 