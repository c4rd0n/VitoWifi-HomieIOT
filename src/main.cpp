#include <Homie.h>
#include <VitoWiFi.h>

std::map<std::string, HomieNode*> homieNodes;

const char *boilierID("boiler");
const char *heating2ID("heating2");
const char *storageTankID("storageTank");
const char* tempType = "temperature";
const char* switchType = "switch";
const char* counterType = "counter";
HomieNode boilerNode(boilierID, "chaudiere", "boiler");
HomieNode heatingNode(heating2ID, "chauffage", "heating");
HomieNode storageTank(storageTankID,"DHW", "storageTank");


VitoWiFi_setProtocol(P300);
DPTemp outsideTemp("outsideTemp", boilierID, 0x5525);
DPTemp boilerWaterTemp("boilerTemp", boilierID,0x0810);
DPHours burnerHoursRun("burnerHoursRun", boilierID,0x08A7);
DPHours burner1HoursRun("burner1HoursRun", boilierID,0x0886);
DPHours burner2HoursRun("burner2HoursRun", boilierID,0x08AB);
DPStat pumpInternalStat("internalPump", boilierID, 0x7660);
DPCount burnerStarts("burnerStarts", boilierID,0x088A);
DPTemp smokeTemp("smokeTemp", boilierID,0x0808);

DPMode currentOperatingMode("currentOperatingMode", heating2ID, 0x3500);
DPTemp flowTemp("flowTemp", heating2ID, 0x3900);
DPTemp returnFlowTemp("returnFlowTemp", heating2ID, 0x080A);
DPStat pumpStat("circulationPump", heating2ID, 0x3906);
DPTemp roomTemp("roomTemp", heating2ID, 0x0898);

DPTemp hotWaterTemp("hotwatertemp", storageTankID, 0x0812);
DPTemp dischargeTemp("dischargeTemp", storageTankID, 0x0814);
DPStat hotWaterPump("hotWaterPump", storageTankID, 0x6513);

void globalCallbackHandler(const IDatapoint& dp, DPValue value) {
  std::map<std::string, HomieNode*>::iterator it = homieNodes.find(dp.getGroup()); 
  if(it != homieNodes.end()){
    char value_str[15] = {0};
    value.getString(value_str, sizeof(value_str));
    it->second->setProperty(dp.getName()).send(value_str);
  }
}

void setup() {
  VitoWiFi.setGlobalCallback(globalCallbackHandler);  // this callback will be used for all DPs without specific callback
                                                      // must be set after adding at least 1 datapoint
  VitoWiFi.setup(&Serial);

  homieNodes[boilierID] = &boilerNode;
  homieNodes[heating2ID] = &heatingNode;
  homieNodes[storageTankID] = &storageTank;

  boilerNode.advertise(outsideTemp.getName())
    .setName(outsideTemp.getName())
    .setDatatype("float")
    .setUnit("°C")
    .setFormat("-60:60");
 
  boilerNode.advertise(boilerWaterTemp.getName())
    .setName(boilerWaterTemp.getName())
    .setDatatype("float")
    .setUnit("°C")
    .setFormat("0:150");
  
  boilerNode.advertise(burnerHoursRun.getName())
    .setName(burnerHoursRun.getName())
    .setDatatype("float")
    .setFormat("0:1150000");
  
  boilerNode.advertise(burner1HoursRun.getName())
    .setName(burner1HoursRun.getName())
    .setDatatype("float")
    .setFormat("0:1150000");
  
  boilerNode.advertise(burner2HoursRun.getName())
    .setName(burner2HoursRun.getName())
    .setDatatype("float")
    .setFormat("0:1150000");
  
  boilerNode.advertise(pumpInternalStat.getName())
    .setName(pumpInternalStat.getName())
    .setDatatype("boolean");

  boilerNode.advertise(smokeTemp.getName())
    .setName(smokeTemp.getName())
    .setDatatype("float")
    .setUnit("°C");

  heatingNode.advertise(currentOperatingMode.getName())
    .setName(currentOperatingMode.getName())
    .setDatatype("integer")
    .setFormat("0:3");
  
  heatingNode.advertise(flowTemp.getName())
    .setName(flowTemp.getName())
    .setDatatype("float")
    .setFormat("0:150");
  
  heatingNode.advertise(returnFlowTemp.getName())
    .setName(returnFlowTemp.getName())
    .setDatatype("float")
    .setFormat("0:150");
  
  heatingNode.advertise(pumpStat.getName())
    .setName(pumpStat.getName())
    .setDatatype("boolean");

  heatingNode.advertise(roomTemp.getName())
    .setName(roomTemp.getName())
    .setDatatype("float")
    .setFormat("0:64");

  storageTank.advertise(hotWaterTemp.getName())
    .setName(hotWaterTemp.getName())
    .setDatatype("float")
    .setFormat("0:150");

  storageTank.advertise(dischargeTemp.getName())
    .setName(dischargeTemp.getName())
    .setDatatype("float")
    .setFormat("0:150");

  storageTank.advertise(hotWaterPump.getName())
    .setName(hotWaterPump.getName())
    .setDatatype("boolean");

  Homie.disableLogging();
  Homie_setFirmware("VitoWiFi", "2.0.7");

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