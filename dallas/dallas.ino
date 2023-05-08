#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 7

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress deviceAddress;
void setup(void)
{
  Serial.begin(115200);
  Serial.println("Dallas Temperature IC Control Library Demo");

  sensors.begin();
  sensors.setResolution(12);

  
	if (!sensors.getAddress(deviceAddress, 0)) {
		
	}
  sensors.setHighAlarmTemp(deviceAddress,25);
  sensors.setLowAlarmTemp(deviceAddress,18);

}

void loop(void)
{
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(sensors.getTempCByIndex(0));

  for (size_t i = 0; i < 8; i++)
  {
    Serial.print(deviceAddress[i]);
    Serial.print(" ");
  }
  Serial.println();
  
  Serial.println(sensors.getResolution());
  Serial.println(sensors.getHighAlarmTemp(deviceAddress));
  Serial.println(sensors.getLowAlarmTemp(deviceAddress));

  Serial.println(sensors.hasAlarm());
}