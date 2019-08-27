#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include <OneWire.h>
#include <SimpleDHT.h>

int pinDHT11 = 13;
SimpleDHT11 dht11;

MDNSResponder mdns;

// Wi-Fi
 const char* ssid = "Rostelecom_59C8";
 const char* password = "CSQ67hTx";
 byte arduino_mac[] = { 0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
 IPAddress ip(192,168,1,35);
 IPAddress gateway(192,168,1,1);
 IPAddress subnet(255,255,255,0);
 ESP8266WebServer server(80);

// пины
  int D0_pin = 16;  // встроенный светодиод
  int D4_pin = 2;   // встроенный светодиод
  int D1_pin = 5;

  #define radarPin 14 //D5
  #define led1_pin 5  //D1
  #define led2_pin 4  //D2
  #define led3_pin 0  //D3


// переменные 
  bool ledState = false; // состояние кнопки 3
  bool ledMillFlag = false;
  uint32_t ledStateMill = 0;
  int ledInterval = 5000; 

  int maxBrig = 1023;
  int ledBrig1 = 0; //яркость 1
  int ledBrig2 = 0; //яркость 2
  int ledBrig3 = 0; //яркость 3
  uint32_t brigMill = 0;
  uint8_t brigSpeed = 3; //чем меньше - тем быстрее розжиг

  bool radar = false; //состояние радара (движение есть/нет)




String webPage() // вэб страница
{

  byte temperature = 0;
  byte humidity = 0;
  dht11.read(pinDHT11, &temperature, &humidity, NULL); 
  String web; 
  web += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/> <meta charset=\"utf-8\"><title>ESP 8266</title><style>button{color:red;padding: 10px 27px;}</style></head>";
  web += "<h1 style=\"text-align: center;font-family: Open sans;font-weight: 100;font-size: 20px;\">ESP8266 Web Server</h1><div>";
  //++++++++++ LED-1  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----LED 1----</p>";
  if (digitalRead(D0_pin) == 1)
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #43a209;margin: 0 auto;\">ON</div>";
  }
  else 
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #ec1212;margin: 0 auto;\">OFF</div>";
  }
  web += "<div style=\"text-align: center;margin: 5px 0px;\"> <a href=\"socket1On\"><button>ON</button></a>&nbsp;<a href=\"socket1Off\"><button>OFF</button></a></div>";
  // ++++++++ LED-1 +++++++++++++
  
  //++++++++++ LED-2  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----LED 2----</p>";
  if (digitalRead(D4_pin) == 1)
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #43a209;margin: 0 auto;\">ON</div>";
  }
  else 
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #ec1212;margin: 0 auto;\">OFF</div>";
  }
  web += "<div style=\"text-align: center;margin: 5px 0px;\"> <a href=\"socket2On\"><button>ON</button></a>&nbsp;<a href=\"socket2Off\"><button>OFF</button></a></div>";
  // ++++++++ LED-2 +++++++++++++
  
  //++++++++++ LED-3  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----LED 3----</p>";
  if (ledState == true)
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #43a209;margin: 0 auto;\">ON</div>";
  }
  else 
  {
    web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #ec1212;margin: 0 auto;\">OFF</div>";
  }
  web += "<div style=\"text-align: center;margin: 5px 0px;\"> <a href=\"socket3On\"><button>ON</button></a>&nbsp;<a href=\"socket3Off\"><button>OFF</button></a></div>";
  // ++++++++ LED-3 +++++++++++++

  //++++++++++ DHT11 TEMP  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----DHT11 TEMP----</p>";
  web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #5191e4;margin: 0 auto;\">"+ String((int)temperature)+"</div>";
  // ++++++++ DHT11 TEMP  +++++++++++++
  
  //++++++++++ DHT11 H  +++++++++++++
  web += "<p style=\"text-align: center;margin-top: 0px;margin-bottom: 5px;\">----DHT11 H----</p>";
  web += "<div style=\"text-align: center;width: 98px;color:white ;padding: 10px 30px;background-color: #5191e4;margin: 0 auto;\">"+ String((int)humidity)+"</div>";
  // ++++++++ DHT11 H  +++++++++++++
  
  // ========REFRESH=============
  web += "<div style=\"text-align:center;margin-top: 20px;\"><a href=\"/\"><button style=\"width:158px;\">REFRESH</button></a></div>";
  // ========REFRESH=============
  
  
  web += "</div>";
  return(web);
}

void readPorts(){
 radar = digitalRead(radarPin);
} 

void task(){

 //задержка нижней подсветки
    if (radar == true){    //если радар активен
        ledState = true;    //то led включить
        ledMillFlag = false; //и флаг опустить
    }  
    if (ledState == true && radar == false && ledMillFlag == false){  //если (led включен) и (радар не активен) и (флаг опущен)
        ledMillFlag = true;                                            // то флаг поднять
        ledStateMill = millis();                                       //  и запомнить милисекунды
    }
    if (millis() - ledStateMill > ledInterval && ledMillFlag == true){  //если (прошло 5000мс) и (флаг поднят)
        ledMillFlag = false;                                             //  то флаг опустить 
        ledState = false;                                                //  и led выключить
    }

 //розжиг
    //led 1
    if (millis() - brigMill > brigSpeed && ledState == true && ledBrig1 !=maxBrig){ 
      ledBrig1++;
      brigMill=millis();
    }
    if (millis() - brigMill > brigSpeed && ledState == false && ledBrig1 !=0){
      ledBrig1--;
      brigMill=millis();
    }//led 1
    //led 2
    if (millis() - brigMill > brigSpeed && ledState == true && ledBrig1 > maxBrig/3 && ledBrig2 !=maxBrig){
      ledBrig2++;
      brigMill=millis();
    }
    if (millis() - brigMill > brigSpeed && ledState == false && ledBrig1 < maxBrig/3 && ledBrig2 !=0){
      ledBrig2--;
      brigMill=millis();
    }//led 2
    // led 3
    if (millis() - brigMill > brigSpeed && ledState == true && ledBrig2 > maxBrig/3 && ledBrig3 !=maxBrig){
      ledBrig3++;
      brigMill=millis();
    }
    if (millis() - brigMill > brigSpeed && ledState == false && ledBrig2 < maxBrig/3 && ledBrig3 !=0){
      ledBrig3--;
      brigMill=millis();
    }//led 3


}

void writePorts(){
 analogWrite(led1_pin, ledBrig);
 analogWrite(led2_pin, ledBrig);
 analogWrite(led3_pin, ledBrig);

}

void setup(void){
  // preparing GPIOs
  pinMode(D0_pin, OUTPUT);
  digitalWrite(D0_pin, LOW);

  pinMode(D4_pin, OUTPUT);
  digitalWrite(D4_pin, LOW);

  pinMode(radarPin, INPUT);
  
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);
 


  delay(100);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  
 
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
   Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  
  //+++++++++++++++++++++++ START  LED-1 ++++++++++++++++++++
  server.on("/", [](){
    server.send(200, "text/html", webPage());
  });
  server.on("/socket1On", [](){
    digitalWrite(D0_pin, HIGH);
    server.send(200, "text/html", webPage());
    delay(100);
    
  });
  server.on("/socket1Off", [](){
    digitalWrite(D0_pin, LOW);
    server.send(200, "text/html", webPage());
    delay(100);
 });   
   //+++++++++++++++++++++++ END  LED-1 ++++++++++++++++++++ 
    
   //+++++++++++++++++++++++ START  LED-2  ++++++++++++++++++++ 
  
  server.on("/socket2On", [](){
    digitalWrite(D4_pin, HIGH);
    server.send(200, "text/html", webPage());
    delay(100);    
  });
  server.on("/socket2Off", [](){
    digitalWrite(D4_pin, LOW);
    server.send(200, "text/html", webPage());
    delay(100);
    });  
   // +++++++++++++++++++++++ END  LED-2 ++++++++++++++++++++
   
   //+++++++++++++++++++++++ START  LED-3  ++++++++++++++++++++ 

  server.on("/socket3On", [](){
    maxBrig = 1023;//ledState = true;//digitalWrite(D1_pin, HIGH);
    server.send(200, "text/html", webPage());
    delay(100);    
   
  });
  server.on("/socket3Off", [](){
    maxBrig = 411;//ledState = false; //digitalWrite(D1_pin, LOW);
    server.send(200, "text/html", webPage());
    delay(100);

   }); 
   // +++++++++++++++++++++++ END  LED-3 ++++++++++++++++++++
    
    
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  server.handleClient();
  readPorts();
  task();
  writePorts();

} 

