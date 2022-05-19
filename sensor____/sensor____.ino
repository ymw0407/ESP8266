#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DFRobot_BME280.h"
#include "Wire.h"
#include <SoftwareSerial.h>
#include "PMS.h"
SoftwareSerial Serial_PMS(12, 13);
PMS pms(Serial_PMS);
PMS::DATA data;
typedef DFRobot_BME280_IIC    BME;
BME   bme(&Wire, 0x77);   // select TwoWire peripheral and set sensor 
#define SEA_LEVEL_PRESSURE    1015.0f

const char* ssid = "wifi";
const char* password = "12345678"; 
const char* mqtt_server = "192.168.201.218";
const char* clientName = "030407Client"; 

WiFiClient espClient;
PubSubClient client(espClient);

void printLastOperateStatus(BME::eStatus_t eStatus)
{
  switch(eStatus) {
  case BME::eStatusOK:    Serial.println("everything ok"); break;
  case BME::eStatusErr:   Serial.println("unknow error"); break;
  case BME::eStatusErrDeviceNotDetected:    Serial.println("device not detected"); break;
  case BME::eStatusErrParameter:    Serial.println("parameter error"); break;
  default: Serial.println("unknow status"); break;
  }
}

void setup_wifi() {
 delay(10);
 // We start by connecting to a WiFi network
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid, password);
 
 while(WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP()); 
}

void callback(char* topic, byte* payload, unsigned int uLen) {
 char pBuffer[uLen+1];
 int i;
 for(i=0;i<uLen;i++){
 pBuffer[i]=payload[i];
 }
 pBuffer[i]='\0';
 Serial.println(pBuffer); // 1, 2
 if(pBuffer[0]=='1'){
 digitalWrite(14, HIGH);
 }else if(pBuffer[0]=='2'){
 digitalWrite(14, LOW);
 } 
}

void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect(clientName)) {
 Serial.println("connected");
 // ... and resubscribe
 client.subscribe("led");
 } else {
 Serial.print("failed, rc=");
 Serial.print(client.state());
 Serial.println(" try again in 5 seconds");
 // Wait 5 seconds before retrying
 delay(5000);
 }
 }
}

void setup() {
 pinMode(14, OUTPUT); // Initialize the BUILTIN_LED pin as an output
 Serial.begin(9600);
 Serial_PMS.begin(9600); 
 setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);
 bme.reset();
 Serial.println("bme read data test");
 while(bme.begin() != BME::eStatusOK) {
   Serial.println("bme begin faild");
   printLastOperateStatus(bme.lastOperateStatus);
   delay(2000);
 }
 Serial.println("bme begin success");
 delay(100);
}
void loop() {
 if (!client.connected()) {
 reconnect();
 }
 client.loop();
 if(pms.read(data)){
  float   temp = bme.getTemperature();
 uint32_t    press = bme.getPressure();
 float   alti = bme.calAltitude(SEA_LEVEL_PRESSURE, press);
 float   humi = bme.getHumidity();
 int pm1 = data.PM_AE_UG_1_0;  
 int pm25 = data.PM_AE_UG_2_5;
 int pm10 = data.PM_AE_UG_10_0;
  char message[64]="", pTmpBuf[50], pPressBuf[50], pAltiBuf[50], pHumBuf[50];
 dtostrf(temp, 5,2, pTmpBuf);
 dtostrf(press, 5,2, pPressBuf);
 dtostrf(alti, 5,2, pAltiBuf);
 dtostrf(humi, 5,2, pHumBuf);

 sprintf(message, "{\"tmp\":%s,\"press\":%s,\"altitude\":%s,\"humi\":%s,\"pm1\":%d,\"pm25\":%d,\"pm10\":%d}", pTmpBuf, pPressBuf, pAltiBuf, pHumBuf, pm1, pm25, pm10); 
 
 Serial.print("Publish message: ");
 Serial.println(message);
 client.publish("sensors", message);

 delay(3000); // 3초
 }
 }
