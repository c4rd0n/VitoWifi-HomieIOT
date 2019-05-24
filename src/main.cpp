#include <Homie.h>
#include <VitoWiFi.h>

HomieNode boilerNode("boiler", "boiler");
HomieNode heating2Node("heating2", "heating");
HomieNode storageTankNode("storageTank","storageTank");
std::map<std::string, HomieNode*> nodes;
VitoWiFi_setProtocol(P300);
DPTemp outsideTemp("outsideTemp", boilerNode.getId(), 0x5525);
DPTemp boilerWaterTemp("boilerTemp", boilerNode.getId(),0x0810);
DPHours burnerHoursRun("burnerHoursRun", boilerNode.getId(),0x08A7);
DPHours burner1HoursRun("burner1HoursRun", boilerNode.getId(),0x0886);
DPHours burner2HoursRun("burner2HoursRun", boilerNode.getId(),0x08AB);
DPStat pumpInternalStat("internalPump", boilerNode.getId(), 0x7660);
DPCount burnerStarts("burnerStarts", boilerNode.getId(),0x088A);
DPTemp smokeTemp("smokeTemp", boilerNode.getId(),0x0808);

DPMode currentOperatingMode("currentOperatingMode", heating2Node.getId(), 0x3500);
DPTemp flowTemp("flowTemp", heating2Node.getId(), 0x3900);
DPTemp returnFlowTemp("returnFlowTemp", heating2Node.getId(), 0x080A);
DPStat pumpStat("circulationPump", heating2Node.getId(), 0x3906);
DPTemp roomTemp("roomTemp", heating2Node.getId(), 0x0898);

DPTemp hotWaterTemp("hotwatertemp", storageTankNode.getId(), 0x0812);
DPTemp dischargeTemp("dischargeTemp", storageTankNode.getId(), 0x0814);
DPStat hotWaterPump("hotWaterPump", storageTankNode.getId(), 0x6513);

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
  nodes[heating2Node.getId()] = &heating2Node;
  nodes[storageTankNode.getId()] = &storageTankNode;

  Homie.disableLogging();
  Homie_setFirmware("VitoWiFi", "1.0.5");
  Homie.setup();
}

void loop() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 60 * 1000UL) {  // read all values every 60 seconds
    lastMillis = millis();
    VitoWiFi.readAll();
  }
  VitoWiFi.loop();
  Homie.loop();
} 