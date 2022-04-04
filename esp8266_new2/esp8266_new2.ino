#define BLYNK_PRINT Serial

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <time.h>

char auth[] = "yZ7V8dmz4POMSQpRYsua_ZrdFCw50W76";
char curTime[20];
const char* ssid = "wifi";
const char* password = "yunbird1211";
const String endpoint = "http://www.kma.go.kr/wid/queryDFSRSS.jsp?zone=1141071000";
const String url = "http://openapi.airkorea.or.kr/openapi/services/rest/ArpltnInforInqireSvc/getCtprvnMesureSidoLIst?serviceKey=Hn8hkZzM%2B7YvGOv2I7um%2FU94g1HNg4Dp%2FB7PGzgCY2hPZKpX7DJ4urpTtr%2Bz%2FMq8XZydM%2BAbNiRWqNixDKrnKQ%3D%3D&numOfRows=1&pageNo=22&sidoName=%EC%84%9C%EC%9A%B8&searchCondition=HOUR";
String line1 = "";
String line2 = "";

IPAddress ip (192, 168, 0, 101); // 연결할, 고정 IP 주소
IPAddress gateway (192, 168, 0, 1); // 게이트웨이 주소
IPAddress subnet (255, 255, 255, 0); // 서브마스크 주소
IPAddress dns (168, 126, 63, 1); // DNS 주소
int ledPin = D5;
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);

  Blynk.begin(auth, ssid, password);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.config (ip, dns ,gateway, subnet);
  WiFi.begin(ssid, password);
 
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    configTime(9*3600, 0, "pool.ntp.org", "time.nist.gov");  // Timezone 9 for Korea
    while (!time(nullptr)) delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  get_weather();
  //get_PM();
}

void get_weather() {
  if ((WiFi.status() == WL_CONNECTED)) 
   {Serial.println("Starting connection to server(weather)...");
    HTTPClient http1;
    http1.begin(endpoint);       //Specify the URL
    int httpCode1 = http1.GET();  //Make the request
    if (httpCode1 > 0) {         //Check for the returning code
      line1 = http1.getString();
   }
  else {Serial.println("Error on HTTP request");}
    parsing();
    http1.end(); //Free the resources
  }
}

void get_PM() {
  if ((WiFi.status() == WL_CONNECTED)) 
   {Serial.println("Starting connection to server(PM)...");
    HTTPClient http2;
    http2.begin(url);       //Specify the URL
    int httpCode2 = http2.GET();  //Make the request
    if (httpCode2 > 0) {         //Check for the returning code
      line2 = http2.getString();
   }
  else {Serial.println("Error on HTTP request");}
    parsing2();
    http2.end(); //Free the resources
  }
}

// 온도 저장 변수
float temp0;
float temp1;
float temp2;
int PM25;
int PM10;
String announce_time;  
String wfEn;
String hour24;
String hour03;
String hour06;
String temp24;
String temp03;
String temp06;
String wfKor24;
String wfKor03;
String wfKor06;
String dataTime;  
String PM25_1;
String PM10_1;



void parsing() {
  int tm_start= line1.indexOf(F("<tm>")); // "<tm>"문자가 시작되는 인덱스 값('<'의 인덱스)을 반환한다. 
  int tm_end= line1.indexOf(F("</tm>"));  
  announce_time = line1.substring(tm_start + 4, tm_end); // +4: "<tm>"스트링의 크기 4바이트, 4칸 이동
  Serial.print(F("announce_time: ")); Serial.println(announce_time);

  
  int hour_start= line1.indexOf(F("<hour>"));
  int hour_end= line1.indexOf(F("</hour>"));
  hour24 = line1.substring(hour_start + 6, hour_end);
  Serial.print(F("hour: ")); Serial.println(hour24);
  
  String temp;
  int temp_start= line1.indexOf(F("<temp>"));
  int temp_end= line1.indexOf(F("</temp>"));
  temp = line1.substring(temp_start + 6, temp_end);
  Serial.print(F("temp: ")); Serial.println(temp);
  temp24 = temp.toFloat();   // 자료형 변경 String -> float
  Serial.print(F("temp24: ")); Serial.println(temp24);

  int wfEn_start= line1.indexOf(F("<wfKor>"));
  int wfEn_end= line1.indexOf(F("</wfKor>"));
  wfKor24 = line1.substring(wfEn_start + 7, wfEn_end);
  Serial.print(F("weather24: ")); Serial.println(wfKor24);
  int del_index = line1.indexOf(F("</data>")); 
  line1.remove(0, del_index + 7);                     // 시작 인덱스 부터 "</data>" 스트링 포함 삭제


  hour_start= line1.indexOf(F("<hour>"));
  hour_end= line1.indexOf(F("</hour>"));
  hour03 = line1.substring(hour_start + 6, hour_end);
  Serial.print(F("hour: ")); Serial.println(hour03);

  temp_start= line1.indexOf(F("<temp>"));
  temp_end= line1.indexOf(F("</temp>"));
  temp = line1.substring(temp_start + 6, temp_end);
  Serial.print(F("temp: ")); Serial.println(temp);
  temp03 = temp.toFloat();   // 자료형 변경 String -> float
  Serial.print(F("temp03: ")); Serial.println(temp03);

  wfEn_start= line1.indexOf(F("<wfKor>"));
  wfEn_end= line1.indexOf(F("</wfKor>"));
  wfKor03 = line1.substring(wfEn_start + 7, wfEn_end);
  Serial.print(F("weather03: ")); Serial.println(wfKor03);
  del_index = line1.indexOf(F("</data>"));
  line1.remove(0, del_index + 7);                     // 시작 인덱스 부터 "</data>" 스트링 포함 삭제


  hour_start= line1.indexOf(F("<hour>"));
  hour_end= line1.indexOf(F("</hour>"));
  hour06 = line1.substring(hour_start + 6, hour_end);
  Serial.print(F("hour: ")); Serial.println(hour06);
  
  temp_start= line1.indexOf(F("<temp>"));
  temp_end= line1.indexOf(F("</temp>"));
  temp = line1.substring(temp_start + 6, temp_end);
  Serial.print(F("temp: ")); Serial.println(temp);
  temp06 = temp.toFloat();   // 자료형 변경 String -> float
  Serial.print(F("temp06: ")); Serial.println(temp06);

  wfEn_start= line1.indexOf(F("<wfKor>"));
  wfEn_end= line1.indexOf(F("</wfKor>"));
  wfKor06 = line1.substring(wfEn_start + 7, wfEn_end);
  Serial.print(F("weather06: ")); Serial.println(wfKor06);
  line1 = ""; // 스트링 변수 line1 데이터 추출 완료 
}


void parsing2() {
  int dataTime_start= line2.indexOf(F("<dataTime>")); // "<tm>"문자가 시작되는 인덱스 값('<'의 인덱스)을 반환한다. 
  int dataTime_end= line2.indexOf(F("</dataTime>"));  
  dataTime = line2.substring(dataTime_start + 10, dataTime_end); // +4: "<tm>"스트링의 크기 4바이트, 4칸 이동
  Serial.print(F("dataTime: ")); Serial.println(dataTime);
  
  
  int PM25_start= line2.indexOf(F("<pm25Value>")); // "<tm>"문자가 시작되는 인덱스 값('<'의 인덱스)을 반환한다. 
  int PM25_end= line2.indexOf(F("</pm25Value>"));  
  PM25_1 = line2.substring(PM25_start + 11, PM25_end); // +4: "<tm>"스트링의 크기 4바이트, 4칸 이동
  Serial.print(F("PM2.5: ")); Serial.println(PM25_1);
  PM25 = PM25_1.toInt();   // 자료형 변경 String -> float
  
  int PM10_start= line2.indexOf(F("<pm10Value>")); // "<tm>"문자가 시작되는 인덱스 값('<'의 인덱스)을 반환한다. 
  int PM10_end= line2.indexOf(F("</pm10Value>"));  
  PM10_1 = line2.substring(PM10_start + 11, PM10_end); // +4: "<tm>"스트링의 크기 4바이트, 4칸 이동
  Serial.print(F("PM10: ")); Serial.println(PM10_1);
  PM10 = PM10_1.toInt();   // 자료형 변경 String -> float
  line2 = "";
}


void loop() {
  Blynk.run();
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1){
    digitalWrite(ledPin, LOW);
    value = LOW;
  }



  // Return the response
  client.println("<title>IoT</title>");;
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<html lang='ko'>");
  
  client.println("<head>");
  
  client.println("<meta charset='utf-8'>");
  client.println("<title>IoT</title>"); 
  
  client.println("<style>");

  client.println("@import url('https://fonts.googleapis.com/css2?family=Nanum+Myeongjo&family=Nanum+Pen+Script&display=swap');");
  client.println(".font {font-family: 'Nanum Pen Script'}");
  client.println(".footer-font {font-size: 15px}");
  client.println(".font-sizeup {font-family: 'Nanum Pen Script'; font-size: 23px;}");
  client.println(".font매우나쁨 {font-family: 'Nanum Pen Script'; font-size: 23px; color: rgb(255, 38, 17);}");
  client.println(".font나쁨 {font-family: 'Nanum Pen Script'; font-size: 23px; color: rgb(255, 104, 19);}");
  client.println(".font보통 {font-family: 'Nanum Pen Script'; font-size: 23px; color: rgb(0, 210, 23);}");
  client.println(".font좋음 {font-family: 'Nanum Pen Script'; font-size: 23px; color: rgb(0, 187, 255);}");
  
  client.println("p {font-size: 20px}");
  client.println("span {font-size: 20px}");
  client.println("html, body {height: 100%; margin: 0;}");
  client.println("div {height: 100%; margin: 0;}");
  client.println("header {background-color: rgb(196, 255, 150); padding: 10px}");
  client.println("footer {border:1px solid Black; height: 100px;}");
  client.println("button {width:120px; height: 40px; color:#fff; background: rgb(196, 163, 255); font-size: 16px; border:none; border-radius: 20px; box-shadow: 0 4px 16px rgba(196, 163, 255, 0.3); transition: 0.3s; }");
  client.println("button:focus{outline:0;}");
  client.println("button:hover{background-color: rgba(196, 163, 255, 0.9); cursor: pointer; box-shadow: 0 2px 4px rgba(196, 163, 255, 0.6);}");
  
  client.println(".container {background-color:#F0F0F0; width:960px; margin:0 auto; padding:10px;}");
  client.println(".content {background-color:#F5F5F5; float:right; width:660px; height:400px; text-align: center;}");
  client.println(".sideinfo { background-color:#DCDAD9; float:left; width:300px; height:400px; }");
  client.println(".position {position: absolute; left: 35%; top: 50%}");
  
  client.println("</style>"); 

  client.println("</head>");
  
  client.println("<body>");
  client.println("<div class='container'>");
  client.println("<header class='font'><h1>Internet of Things</h1><h3>21214 윤민우</h3></header>");
  client.println("<div class='sideinfo'><br /><span class='font'>&nbsp;&nbsp;현재 상태 : </span>"); if(value == HIGH) {client.print("<span class='font'>&nbsp;ON</span>");} else {client.print("<span class='font'>&nbsp;OFF</span>");}
  client.print ("<br /><br /><span class='font'>&nbsp;&nbsp현재 시간 :&nbsp;&nbsp</span>" );
  client.print (curTime);
  client.print ("<br /><br /><span class='font'>&nbsp;&nbspPM2.5(초미세먼지) :&nbsp;&nbsp</span>" );
  client.print (PM25);
  client.print ("<span class='font'>㎍/㎥</span><br />");
  client.print ("<span class='font'>&nbsp;&nbsp초미세먼지 :&nbsp;&nbsp</span>" );
  client.print ("<span class='font'></span>"); 
  if(PM25 > 0 && PM25 <= 15) {client.print("<span class='font좋음'>좋음</span>");} 
  else if(PM25 > 15 && PM25 <= 35) {client.print("<span class='font보통'>보통</span>");} 
  else if(PM25 > 35 && PM25 <= 75) {client.print("<span class='font나쁨'>나쁨</span>");} 
  else if(PM25 > 75){client.print("<span class='font매우나쁨'>매우나쁨</span>");} 
  else{client.print("<span class='font'>정보 없음</span>");}
  client.print ("<br /><br /><span class='font'>&nbsp;&nbspPM10(미세먼지) :&nbsp;&nbsp</span>" );
  client.print (PM10);
  client.print ("<span class='font'>㎍/㎥</span><br />");
  client.print ("<span class='font'>&nbsp;&nbsp미세먼지 :&nbsp;&nbsp</span>" );
  client.print ("<span class='font'></span>"); 
  if(PM10 > 0 && PM10 <= 30) {client.print("<span class='font좋음'>좋음</span>");} 
  else if(PM10 > 30 && PM10 <= 80) {client.print("<span class='font보통'>보통</span>");} 
  else if(PM10 > 80 && PM10 <= 150) {client.print("<span class='font나쁨'>나쁨</span>");} 
  else if(PM10 > 150){client.print("<span class='font매우나쁨'>매우나쁨</span>");} 
  else{client.print("<span class='font'>정보 없음</span>");}  
  client.print ("<br /><br /><span class='font'>&nbsp;&nbsp기준 :&nbsp;&nbsp</span>" );
  client.print (hour06);
  client.print ("<span class='font'>시</span><br /><br />");
  client.print ("<span class='font'>&nbsp;&nbsp온도 :&nbsp;&nbsp</span>" );
  client.print (temp06);
  client.print ("<span class='font'>℃</span><br /><br />");
  client.print ("<span class='font'>&nbsp;&nbsp날씨 :&nbsp;&nbsp</span>" );
  client.print (wfKor06);
  client.print ("<br /></div>");

  client.println("<div class='content', 'postion'>");
  client.println("<br /><br /><br/><br /><p class='font'>릴레이 : <a href=\"/LED=ON\"><button class='font-sizeup'>ON</button></a></p>");
  client.println("<br /><br /><p class='font'>릴레이 : <a href=\"/LED=OFF\"><button class='font-sizeup'>OFF</button></a></p>");
  client.println("</div>");
  client.println("<footer><h5>© copyright 2020 by 윤민우</h5><p class='footer-font'>It's the practice of esp8266 + html, css</p></footer>");
  client.println("</div>");
  client.println("</body>");
  client.println("</html>");
  client.println("");

  

 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
 
}
