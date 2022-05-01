#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "604ho2_2.4G";
const char* password = "lks0710000000"; // 와이파이 AP, 또는 스마트폰의 핫스판 이름
const char* mqtt_server = "192.168.0.17";
const char* clientName = "030407Client"; // 다음 이름이 중복되지 않게 꼭 수정 바람 - 생년월일 추천
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

int led=14; // D1 mini에 있는 led를 사용
int timeIn=1000; // led가 깜박이는 시간을 mqtt 통신에서 전달받아 저장
void setup() {
 pinMode(led, OUTPUT);
 Serial.begin(115200);
 setup_wifi();
 client.setServer(mqtt_server, 1883);
 client.setCallback(callback);
}
void setup_wifi() {
 delay(10);
 // We start by connecting to a WiFi network
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}

// 통신에서 문자가 들어오면 이 함수의 payload 배열에 저장된다. 
void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived [");
 Serial.print(topic);
 Serial.print("] ");
 for (int i = 0; i < length; i++) {
 Serial.print((char)payload[i]);
 }
 Serial.println();
 // payload로 들어온 문자를 정수로 바꾸기 위해 String inString에 저장후에
 // toInt() 함수를 사용해 정수로 바꾸어 timeIn에 저장한다.
 String inString="";
 for (int i = 0; i < length; i++) {
 inString += (char)payload[i];
 }
 timeIn=inString.toInt();
}
// mqtt 통신에 지속적으로 접속한다. 
void reconnect() {
 // Loop until we're reconnected
 while (!client.connected()) {
 Serial.print("Attempting MQTT connection...");
 // Attempt to connect
 if (client.connect(clientName)) {
 Serial.println("connected");
 // Once connected, publish an announcement...
 client.publish("inTopic", "Reconnected");
 // ... and resubscribe
 client.subscribe("outTopic");
 } else {
 Serial.print("failed, rc=");
 Serial.print(client.state());
 Serial.println(" try again in 5 seconds");
 // Wait 5 seconds before retrying
 delay(5000);
 }
 }
}
void loop() {
 if (!client.connected()) {
 reconnect();
 }
 client.loop();
 // 들어온 timeIn 값에 따라 led가 점멸하게 한다.
 digitalWrite(led, HIGH); // turn the LED on (HIGH is the voltage level)
 delay(timeIn); // wait for a second
 digitalWrite(led, LOW); // turn the LED off by making the voltage LOW
 delay(timeIn); 
}
