#include <WiFi.h>
#include <WiFiAP.h> 
#include <WiFiClient.h> 
#include <WiFiGeneric.h> 
#include <WiFiMulti.h> 
#include <WiFiSTA.h> 
#include <WiFiScan.h> 
#include <WiFiServer.h> 
#include <WiFiType.h> 
#include <WiFiUdp.h> 
#define BLYNK_TEMPLATE_ID "" 
#define BLYNK_TEMPLATE_NAME "" 
#define BLYNK_AUTH_TOKEN "" 
char ssid[] = ""; //WiFi Name 
char pass[] = ""; //WiFi Password 
//Set the maximum wet and maximum dry value measured by the sensor 
int wetSoilVal = 930 ;  //min value when soil is wet 
int drySoilVal = 3000 ;  //max value when soil is dry
  //Set ideal moisture range percentage(%) in soil 
int moistPerLow =   
20 ;  //min moisture % 
int moistPerHigh =   
80 ;  //max moisture % 
#include <Adafruit_SSD1306.h> 
#include <WiFi.h> 
#include <WiFiClient.h> 
#include <BlynkSimpleEsp32.h> 
#include <DHT.h>   
#include <AceButton.h> 
#include <HTTPClient.h> 
using namespace ace_button;  
// Define connections to sensor 
#define SensorPin       
34  //D34 
#define DHTPin          
#define RelayPin        
#define wifiLed         
14  //D14 
25  //D25 
2   //D2 
#define RelayButtonPin  32  //D32 
#define ModeSwitchPin   33  //D33 
#define BuzzerPin       
26  //D26 
#define ModeLed         
15  //D15 
// Uncomment whatever type you're using! 
#define DHTTYPE DHT11     
// DHT 11 
//#define DHTTYPE DHT22   
//#define DHTTYPE DHT21   
// DHT 22, AM2302, AM2321 
// DHT 21, AM2301 
//Change the virtual pins according the rooms 
#define VPIN_MoistPer    
V1  
#define VPIN_TEMPERATURE V2 
#define VPIN_HUMIDITY    
V3 
#define VPIN_MODE_SWITCH V4 
#define VPIN_RELAY       
V5 
#define SCREEN_WIDTH 128 // OLED display width, in pixels 
#define SCREEN_HEIGHT 32 // OLED display height, in pixels 
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins) 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino 
reset pin) 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 
OLED_RESET); 
int     
int     
bool    
sensorVal; 
moisturePercentage; 
toggleRelay = LOW; //Define to remember the toggle state bool   
prevMode = true; 
int     temperature1 = 0; 
int     humidity1   = 0; 
String  currMode  = "A"; 
 
char auth[] = BLYNK_AUTH_TOKEN; 
 
// Twilio Credentials 
#define TWILIO_ACCOUNT_SID "" 
#define TWILIO_AUTH_TOKEN  "" 
#define TWILIO_PHONE_NUMBER "" 
#define RECIPIENT_PHONE_NUMBER "" 
 
ButtonConfig config1; 
AceButton button1(&config1); 
ButtonConfig config2; 
AceButton button2(&config2); 
 
void handleEvent1(AceButton*, uint8_t, uint8_t); 
void handleEvent2(AceButton*, uint8_t, uint8_t); 
 
BlynkTimer timer; 
DHT dht(DHTPin, DHTTYPE); 
 
void checkBlynkStatus() { // called every 3 seconds by SimpleTimer 
 
  bool isconnected = Blynk.connected(); 
  if (isconnected == false) { 
    Serial.print("Blynk Not Connected "); 
    digitalWrite(wifiLed, LOW); 
  } 
  if (isconnected == true) { 
    digitalWrite(wifiLed, HIGH); 
    //Serial.println("Blynk Connected"); 
  } 
} 
 
BLYNK_CONNECTED() { 
  Blynk.syncVirtual(VPIN_MoistPer); 
  Blynk.syncVirtual(VPIN_RELAY); 
  Blynk.syncVirtual(VPIN_TEMPERATURE); 
  Blynk.syncVirtual(VPIN_HUMIDITY); 
  //Blynk.syncVirtual(VPIN_MODE_SWITCH); 
  Blynk.virtualWrite(VPIN_MODE_SWITCH, prevMode); 
} 
void sendSMS(const char* message, int temperature, int humidity, int 
moisturePercentage) { 
  HTTPClient http;
String url = "https://api.twilio.com/2010-04-01/Accounts/" 
String(TWILIO_ACCOUNT_SID) + "/Messages.json"; 
   
  http.begin(url.c_str()); 
  http.setAuthorization(TWILIO_ACCOUNT_SID, TWILIO_AUTH_TOKEN); 
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 
 
  // Compose the message body with temperature, humidity, and moisture 
percentage 
  String body = "To=" + String(RECIPIENT_PHONE_NUMBER) + "&From=" + 
String(TWILIO_PHONE_NUMBER) + 
                "&Body=" + String(message) +""+  // %0A for newline 
                "Temperature: " + String(temperature) + " C" + 
                "Humidity: " + String(humidity) + " %" + 
                "Moisture Percentage: " + String(moisturePercentage) + 
" %"; 
 
  int httpResponseCode = http.POST(body); 
  Serial.print("HTTP Response Code: "); 
  Serial.println(httpResponseCode); 
 
  if (httpResponseCode > 0) { 
    String responseBody = http.getString(); 
    Serial.println("Response Body:"); 
    Serial.println(responseBody); 
    Serial.println("SMS Sent successfully."); 
  } else { 
    Serial.print("Error sending SMS. Response code: "); 
    Serial.println(httpResponseCode); 
  } 
 
  http.end(); 
} 
 
 
 
 
BLYNK_WRITE(VPIN_RELAY) { 
  if(!prevMode){ 
    toggleRelay = param.asInt(); 
    digitalWrite(RelayPin, toggleRelay); 
     if (toggleRelay == HIGH) { 
      sendSMS("Relay Turned ON - Watering Started!", temperature1, 
humidity1, moisturePercentage); 
    } else { 
     sendSMS("Relay Turned OFF - Watering Stoped!", temperature1, 
humidity1, moisturePercentage); 
    }
[7/16/2025 8:18 PM] Rakesh Chinna: } 
  else{ 
    Blynk.virtualWrite(VPIN_RELAY, toggleRelay); 
  } 
} 
 
BLYNK_WRITE(VPIN_MODE_SWITCH) { 
  if(prevMode !=  param.asInt()){ 
    prevMode =  param.asInt(); 
    currMode = prevMode ? "A" : "M"; 
    digitalWrite(ModeLed, prevMode); 
    controlBuzzer(500); 
    if(!prevMode && toggleRelay == HIGH){ 
      digitalWrite(RelayPin, LOW); 
      toggleRelay = LOW; 
      Blynk.virtualWrite(VPIN_RELAY, toggleRelay); 
    } 
   }    
} 
 
void controlBuzzer(int duration){ 
  Serial.print("Buzzer Duration: "); 
  Serial.println(duration); 
  digitalWrite(BuzzerPin, HIGH); // Turn on the buzzer 
  delay(duration);               // Wait for the specified duration 
  digitalWrite(BuzzerPin, LOW);  // Turn off the buzzer 
} 
 
void displayData(String line1 , String line2){ 
  display.clearDisplay(); 
  display.setTextSize(2); 
  display.setCursor(30,2); 
  display.print(line1); 
  display.setTextSize(1); 
  display.setCursor(1,25); 
  display.print(line2); 
  display.display(); 
} 
 
void getMoisture(){ 
  sensorVal = analogRead(SensorPin); 
 
  if (sensorVal > (wetSoilVal - 100) && sensorVal < (drySoilVal + 100) 
){ 
    moisturePercentage = map(sensorVal ,drySoilVal, wetSoilVal, 0, 
100); 
    // Print result to serial monitor 
    Serial.print("Moisture Percentage: ");
    Serial.print(moisturePercentage); 
    Serial.println(" %"); 
  } 
  else{ 
    Serial.println(sensorVal); 
  } 
  delay(100); 
} 
 
void getWeather(){ 
   
  float h = dht.readHumidity(); 
  float t = dht.readTemperature(); // or dht.readTemperature(true) for 
Fahrenheit 
   
  if (isnan(h) || isnan(t)) { 
    Serial.println("Failed to read from DHT sensor!"); 
    return; 
  } 
  else { 
    humidity1 = int(h); 
    temperature1 = int(t); 
   // Serial.println(temperature1); 
   // Serial.println(humidity1); 
  }   
} 
 
void sendSensor() 
{  
  getMoisture(); // get Moisture reading 
  getWeather(); // get reading from DHT11 
   
  displayData(String(moisturePercentage) + " %", "T:" + 
String(temperature1) + " C,  H:" + String(humidity1) + " %  " + 
currMode); 
   
  Blynk.virtualWrite(VPIN_MoistPer, moisturePercentage); 
  Blynk.virtualWrite(VPIN_TEMPERATURE, temperature1); 
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity1); 
} 
 
void controlMoist(){ 
  if(prevMode){ 
    if (moisturePercentage < (moistPerLow)){ 
      if (toggleRelay == LOW){ 
        controlBuzzer(500); 
        digitalWrite(RelayPin, HIGH); 
        toggleRelay = HIGH;
[7/16/2025 8:19 PM] Rakesh Chinna: sendSMS("Relay Turned ON - Watering Started!", temperature1, 
humidity1, moisturePercentage); 
        Blynk.virtualWrite(VPIN_RELAY, toggleRelay); 
        delay(1000); 
      }       
    } 
    if (moisturePercentage > (moistPerHigh)){ 
      if (toggleRelay == HIGH){ 
        controlBuzzer(500); 
        digitalWrite(RelayPin, LOW); 
        toggleRelay = LOW; 
         sendSMS("Relay Turned OFF - Watering Stopped.", temperature1, 
humidity1, moisturePercentage); 
        Blynk.virtualWrite(VPIN_RELAY, toggleRelay); 
        delay(1000); 
      }  
    }  
  } 
  else{ 
    button1.check(); 
  } 
} 
  
void setup() { 
  // Set up serial monitor 
  Serial.begin(115200); 
  
  // Set pinmodes for GPIOs 
  pinMode(RelayPin, OUTPUT); 
  pinMode(wifiLed, OUTPUT); 
  pinMode(ModeLed, OUTPUT); 
  pinMode(BuzzerPin, OUTPUT); 
 
  pinMode(RelayButtonPin, INPUT_PULLUP); 
  pinMode(ModeSwitchPin, INPUT_PULLUP); 
 
  digitalWrite(wifiLed, LOW); 
  digitalWrite(ModeLed, LOW); 
  digitalWrite(BuzzerPin, LOW); 
 
  dht.begin();    // Enabling DHT sensor 
 
  config1.setEventHandler(button1Handler); 
  config2.setEventHandler(button2Handler); 
   
  button1.init(RelayButtonPin); 
  button2.init(ModeSwitchPin);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed")); 
    for(;;); 
  } 
  delay(1000);   
  display.setTextSize(1); 
  display.setTextColor(WHITE); 
  display.clearDisplay(); 
 
  WiFi.begin(ssid, pass); 
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server 
is connected every 2 seconds 
  timer.setInterval(3000L, sendSensor); // display and send sensor 
reading every 3 seconds 
  Blynk.config(auth); 
  //delay(1000); 
  controlBuzzer(1000);  
  digitalWrite(ModeLed, prevMode); 
} 
 void loop() { 
 
  Blynk.run(); 
  timer.run(); // Initiates SimpleTimer 
   
  button2.check(); 
  controlMoist();  
} 
 
void button1Handler(AceButton* button, uint8_t eventType, uint8_t 
buttonState) { 
  Serial.println("EVENT1"); 
  switch (eventType) { 
    case AceButton::kEventReleased: 
      //Serial.println("kEventReleased"); 
      digitalWrite(RelayPin, !digitalRead(RelayPin)); 
      toggleRelay = digitalRead(RelayPin); 
      Blynk.virtualWrite(VPIN_RELAY, toggleRelay); 
       if (toggleRelay == HIGH) { 
        sendSMS("Relay Turned ON - Watering Started!", temperature1, 
humidity1, moisturePercentage); 
      } else { 
        sendSMS("Relay Turned OFF - Watering Stoped!", temperature1, 
humidity1, moisturePercentage); 
      } 
      break; 
  } 
}
[7/16/2025 8:19 PM] Rakesh Chinna: void button2Handler(AceButton* button, uint8_t eventType, uint8_t 
buttonState) { 
  Serial.println("EVENT2"); 
  switch (eventType) { 
    case AceButton::kEventReleased: 
      //Serial.println("kEventReleased"); 
      if(prevMode && toggleRelay == HIGH){ 
        digitalWrite(RelayPin, LOW); 
        toggleRelay = LOW; 
        Blynk.virtualWrite(VPIN_RELAY, toggleRelay); 
      } 
      prevMode = !prevMode; 
      currMode = prevMode ? "A" : "M"; 
      digitalWrite(ModeLed, prevMode); 
      Blynk.virtualWrite(VPIN_MODE_SWITCH, prevMode); 
      controlBuzzer(500); 
      break; 
  } 
}