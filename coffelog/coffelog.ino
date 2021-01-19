/*************************************************************
 * 
 * CoffeLog v1.1
 * 
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

/* Comment this out to disable prints and save space */
//#define BLYNK_PRINT Serial
#include <FS.h> 
#include <SPI.h>
#include <MFRC522.h>

//#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>

// -------- STA Mode
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <ArduinoJson.h> 




#define RST_PIN         D1        // Configurable, see typical pin layout above
#define SS_PIN          D8        // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.


#define redLED D3    
#define greenLED D4
#define ONOFF D2    

WidgetLED led_alive(V6);
WidgetLCD lcd(V7);

//#define APSTA_ADR 170
///////// end EEPROM addresses

// Config ============================================
#define push_delay 30 //  push_delay/10= 3sec

#define addnewmember 0 // change to 1 to be able to add new row in the table, then change back

boolean reset_eeprom_flag = 0; // reset eeprom values, when Reset button was pushed from blynk app 

char arrayToStore[10];                    // Must be greater than the length of string.
char arrayToStore1[10]; 
char arrayToStore2[10];
char arrayToStore3[10];
char arrayToStore4[10];
char arrayToStore5[10];
char arrayToStore6[10];
char arrayToStore7[10];
char arrayToStore8[10];       
char arrayToStore9[10];  

String read_rfid;
String ok_rfid_1; // User1
String ok_rfid_2; // User2
String ok_rfid_3; // User3
String ok_rfid_4; // User4
String ok_rfid_5; // USer5
String ok_rfid_6; // USer6
String ok_rfid_7; // User7
String ok_rfid_8; // User8
String ok_rfid_9; // User9

// END Config =========================================

int timer_value, timer_value_buffer;
boolean save_timer_flag;
boolean grinder_flag, manual_flag, lost_flag;
boolean locked1, locked2, locked3, locked4, locked5, locked6, locked7, locked8, locked9, lock_card_flag, unlock_card_flag;

int a;
int b;
int c;
int d;
int e;
int f;
int g;
int h;
int i;

byte locked1_address = 90;
byte locked2_address = 91;
byte locked3_address = 92;
byte locked4_address = 93;
byte locked5_address = 94;
byte locked6_address = 95;
byte locked7_address = 89;
byte locked8_address = 88;
byte locked9_address = 87;
int EEaddress0 = 96;
int EEaddress1 = 100;
int EEaddress2 = 104;
int EEaddress3 = 108;
int EEaddress4 = 112;
int EEaddress5 = 116;
int EEaddress6 = 120;
int EEaddress7 = 124;
int EEaddress8 = 128;
int EEaddress9 = 132;
int EEaddress21= 140;
int EEaddress22= 150;
int EEaddress23= 160;
int EEaddress24= 170;
int EEaddress25= 180;
int EEaddress26= 190;
int EEaddress27= 200;
int EEaddress28= 210;
int EEaddress29= 220;

/*
   Initialize.
*/

byte select_card;
boolean alive;

//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "8080";

// Change Blynk Token
//============================================================
char blynk_token[40] = "xxxxxxxxxxxxxxxxxxxxxx";
//============================================================

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


// constants won't change:
const long interval = 100;           // interval at which to blink (milliseconds)
unsigned long currentMillis;
unsigned long previousMillis = 0;        // will store last time update

BlynkTimer timer; // Create a Timer object called "timer"! 

// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
  //get data stored in virtual pin V0 from server
  Blynk.syncAll();
}

// Button Pushed from Blynk APP ================================================

BLYNK_WRITE(V1){
   reset_eeprom_flag = param.asInt();
}

BLYNK_WRITE(V2){
   select_card = param.asInt();
}

BLYNK_WRITE(V3){
   timer_value = param.asInt();
}

BLYNK_WRITE(V4){
   lock_card_flag = param.asInt();
   lock_card();
}

BLYNK_WRITE(V5){
   unlock_card_flag = param.asInt();
   lock_card();
}

BLYNK_WRITE(V8){
   save_timer_flag = param.asInt();
}

BLYNK_WRITE(V9){
   manual_flag = param.asInt();
}

BLYNK_WRITE(V10){
   lost_flag = param.asInt();
}

// SEND to Blynk =============================================================================
void sendUptime()
{
  // This function sends Arduino up time every 1 second to Virtual Pin (V5)
  // In the app, Widget's reading frequency should be set to PUSH
  // You can send anything with any interval using this construction
  // Don't send more that 10 values per second

   if(!grinder_flag) {
                   alive = !alive;
                   //digitalWrite(LED, alive);
          
                   //lcd.clear();
                   lcd.print(0,0,"Card reading ...");
                   
                     
                   if(alive) { 
                               led_alive.on();
                               
                   }
                   else {
                          led_alive.off();
                         
                   }

                    if(reset_eeprom_flag) reset_eeprom();

                    if(addnewmember){
                                    Blynk.virtualWrite(V0,"add",0,"User1", a);
                                    Blynk.virtualWrite(V0,"add",1,"User2", b);
                                    Blynk.virtualWrite(V0,"add",2,"User3", c);
                                    Blynk.virtualWrite(V0,"add",3,"User4", d);
                                    Blynk.virtualWrite(V0,"add",4,"User5", e);
                                    Blynk.virtualWrite(V0,"add",5,"User6", f);
                                    Blynk.virtualWrite(V0,"add",6,"Guest", g);
                                    Blynk.virtualWrite(V0,"add",7,"User7", h);
                                    Blynk.virtualWrite(V0,"add",8,"User8", i);
                    }
                    else {
                            if (!locked1) Blynk.virtualWrite(V0,"update",0,"1    User1", a);
                            else Blynk.virtualWrite(V0,"update",0,"1  Locked  User1", a);
                            if (!locked2) Blynk.virtualWrite(V0,"update",1,"2    User2", b);
                            else Blynk.virtualWrite(V0,"update",1,"2  Locked  User2", b);
                            if (!locked3) Blynk.virtualWrite(V0,"update",2,"3    User3", c);
                            else Blynk.virtualWrite(V0,"update",2,"3  Locked  User3", c);
                            if (!locked4) Blynk.virtualWrite(V0,"update",3,"4    User4", d);
                            else Blynk.virtualWrite(V0,"update",3,"4  Locked  User4", d);
                            if (!locked5) Blynk.virtualWrite(V0,"update",4,"5    User5", e);
                            else Blynk.virtualWrite(V0,"update",4,"5  Locked  User5", e);
                            if (!locked6) Blynk.virtualWrite(V0,"update",5,"6    User6", f);
                            else Blynk.virtualWrite(V0,"update",5,"6  Locked  User6", f);
                             if (!locked7) Blynk.virtualWrite(V0,"update",6,"7    Guest", g);
                            else Blynk.virtualWrite(V0,"update",6,"7  Locked  Guest", g);
                            if (!locked8) Blynk.virtualWrite(V0,"update",7,"8    User7", h);
                            else Blynk.virtualWrite(V0,"update",7,"8  Locked  User7", h);
                            if (!locked9) Blynk.virtualWrite(V0,"update",8,"9    User8", i);
                            else Blynk.virtualWrite(V0,"update",8,"9  Locked  User8", i);
                         }
                               
                   
    }
  
}


// SETUP =======================================================================
void setup() {
  // put your setup code here, to run once:

                  // pinMode(A0, INPUT);           // Vinput and Batt voltage
                   pinMode(redLED, OUTPUT);      // GPIO2 Led shared output
                   digitalWrite(redLED, HIGH); 
                   pinMode(greenLED, OUTPUT);      //
                   digitalWrite(greenLED, HIGH);  
                   pinMode(ONOFF, OUTPUT);    // Grinder ON/OFF button control
                   digitalWrite(ONOFF, HIGH); 
                  // pinMode(wifi_AP_MODE, INPUT);

                                    
  // Debug console
//  Serial.begin(115200);

Serial.begin(115200);
  Serial.println();
//delay(3000);

  //clean FS, for testing
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        //DynamicJsonBuffer jsonBuffer;
        DynamicJsonDocument jsonBuffer(1024);
        //JsonObject& json = jsonBuffer.parseObject(buf.get());
        deserializeJson(jsonBuffer, buf.get());
        //json.printTo(Serial);
        auto error = serializeJson(jsonBuffer, Serial);
        if (!error) {
          Serial.println("\nparsed json");

       ///   strcpy(mqtt_server, jsonBuffer["mqtt_server"]);
       ///   strcpy(mqtt_port, jsonBuffer["mqtt_port"]);
       ///   strcpy(blynk_token, jsonBuffer["blynk_token"]);

        } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
///  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
///  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
///  WiFiManagerParameter custom_blynk_token("blynk", "blynk token", blynk_token, 33);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //set static ip
  //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  
  //add all your parameters here
///  wifiManager.addParameter(&custom_mqtt_server);
///  wifiManager.addParameter(&custom_mqtt_port);
///  wifiManager.addParameter(&custom_blynk_token);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //set minimu quality of signal so it ignores AP's under that quality
  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();
  
  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  //wifiManager.setTimeout(120);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  //read updated parameters
 /// strcpy(mqtt_server, custom_mqtt_server.getValue());
 /// strcpy(mqtt_port, custom_mqtt_port.getValue());
 /// strcpy(blynk_token, custom_blynk_token.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    //DynamicJsonBuffer jsonBuffer;
    DynamicJsonDocument jsonBuffer(1024);
    //JsonObject& json = jsonBuffer.createObject();
   /// jsonBuffer["mqtt_server"] = mqtt_server;
   /// jsonBuffer["mqtt_port"] = mqtt_port;
   ///  jsonBuffer["blynk_token"] = blynk_token;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    //json.printTo(Serial);
    //json.printTo(configFile);
    serializeJson(jsonBuffer, Serial);
    serializeJson(jsonBuffer, configFile);
    
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  /// Serial.print("Blynk Token : ");
  /// Serial.println(blynk_token);


Blynk.config(blynk_token);
// Blynk.begin(blynk_token, ssid, password);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

timer.setInterval(5000L, sendUptime); //  Here you set interval (1sec) and which function to call

                    

EEPROM.begin(256);
  
  
  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();         // Init MFRC522 card
  delay(100);
  
  EEPROM.get(locked1_address, locked1);
  EEPROM.get(locked2_address, locked2);
  EEPROM.get(locked3_address, locked3);
  EEPROM.get(locked4_address, locked4);
  EEPROM.get(locked5_address, locked5);
  EEPROM.get(locked6_address, locked6);
  EEPROM.get(locked7_address, locked7);
  EEPROM.get(locked8_address, locked8);
  EEPROM.get(locked9_address, locked9);
  
  EEPROM.get(EEaddress21, arrayToStore1);
  EEPROM.get(EEaddress22, arrayToStore2);
  EEPROM.get(EEaddress23, arrayToStore3);
  EEPROM.get(EEaddress24, arrayToStore4);
  EEPROM.get(EEaddress25, arrayToStore5);
  EEPROM.get(EEaddress26, arrayToStore6);
  EEPROM.get(EEaddress27, arrayToStore7);
  EEPROM.get(EEaddress28, arrayToStore8);
  EEPROM.get(EEaddress29, arrayToStore9);
    
  timer_value = EEPROM.get(EEaddress0, timer_value);
  timer_value_buffer = timer_value;
  Serial.println(timer_value);
  
  a = EEPROM.get(EEaddress1, a);
  Serial.print(a);
  Serial.print(" User1 ");
  Serial.println(arrayToStore1);
  ok_rfid_1 = arrayToStore1;
  
  b = EEPROM.get(EEaddress2, b);
  Serial.print(b);
  Serial.print(" User2 ");
  Serial.println(arrayToStore2);
  ok_rfid_2 = arrayToStore2;
  
  c = EEPROM.get(EEaddress3, c);
  Serial.print(c);
  Serial.print(" User3 ");
  Serial.println(arrayToStore3);
  ok_rfid_3 = arrayToStore3;
  
  d = EEPROM.get(EEaddress4, d);
  Serial.print(d);
  Serial.print(" User4 ");
  Serial.println(arrayToStore4);
  ok_rfid_4 = arrayToStore4;
  
  e = EEPROM.get(EEaddress5, e);
  Serial.print(e);
  Serial.print(" User5 ");
  Serial.println(arrayToStore5);
  ok_rfid_5 = arrayToStore5;
  
  f = EEPROM.get(EEaddress6, f);
  Serial.print(f);
  Serial.print(" User6 ");
  Serial.println(arrayToStore6);
  ok_rfid_6 = arrayToStore6;
  
  g = EEPROM.get(EEaddress7, g);
  Serial.print(g);
  Serial.print(" Guest ");
  Serial.println(arrayToStore7);
  ok_rfid_7 = arrayToStore7;
  
  h = EEPROM.get(EEaddress8, h);
  Serial.print(h);
  Serial.print(" User7 ");
  Serial.println(arrayToStore8);
  ok_rfid_8 = arrayToStore8;

  i = EEPROM.get(EEaddress9, i);
  Serial.print(i);
  Serial.print(" User8 ");
  Serial.println(arrayToStore9);
  ok_rfid_9 = arrayToStore9;

                    
                    
}

// END Setup ================================================== 


// Begin Functions ==================================================

/*
   Helper routine to dump a byte array as hex values to Serial.
*/
void dump_byte_array(byte *buffer, byte bufferSize) {
  read_rfid = "";
  for (byte i = 0; i < bufferSize; i++) {
    read_rfid = read_rfid + String(buffer[i], HEX);
  }
}

void reset_eeprom(){
                      EEPROM.put(EEaddress1, 0); 
                      EEPROM.put(EEaddress2, 0); 
                      EEPROM.put(EEaddress3, 0); 
                      EEPROM.put(EEaddress4, 0); 
                      EEPROM.put(EEaddress5, 0); 
                      EEPROM.put(EEaddress6, 0);
                      EEPROM.put(EEaddress7, 0);
                      EEPROM.put(EEaddress8, 0);
                      EEPROM.put(EEaddress9, 0);
                      EEPROM.commit();
                      Blynk.virtualWrite(V1, LOW); // Reset button OFF
                      reset_eeprom_flag = a = b = c = d = e = f = g = h = i = 0;
                     
                    }

void check1() {
  a++;
  Serial.println(a);
  digitalWrite(greenLED, LOW);
  lcd.print(0,1, "     User1      ");
  EEPROM.put(EEaddress1, a);
  EEPROM.commit();
  delay(1000);
  digitalWrite(greenLED, HIGH);
  grinder_flag = 1;
}
void check2() {
  b++;
  Serial.println(b);
  digitalWrite(greenLED, LOW);
  lcd.print(0,1, "     User2      ");
  EEPROM.put(EEaddress2, b);
  EEPROM.commit();
  delay(1000);
  digitalWrite(greenLED, HIGH);
  grinder_flag = 1;
}
void check3() {
  c++;
  Serial.println(c);
  digitalWrite(greenLED, LOW);
  lcd.print(0,1, "     User3      ");
  EEPROM.put(EEaddress3, c);
  EEPROM.commit();
  delay(1000);
  digitalWrite(greenLED, HIGH);
  grinder_flag = 1;
}
void check4() {
  d++;
  Serial.println(d);
  digitalWrite(greenLED, LOW);
  lcd.print(0,1, "     User4      ");
  EEPROM.put(EEaddress4, d);
  EEPROM.commit();
  delay(1000);
  digitalWrite(greenLED, HIGH);
  grinder_flag = 1;
}

void check5() {
  e++;
  Serial.println(e);
  digitalWrite(greenLED, LOW);
  lcd.print(0,1, "     User5      ");
  EEPROM.put(EEaddress5, e);
  EEPROM.commit();
  delay(1000);
  digitalWrite(greenLED, HIGH);
  grinder_flag = 1;
}

void check6() {
  f++;
  Serial.println(f);
  digitalWrite(greenLED, LOW);
  lcd.print(0,1, "     User6      ");
  EEPROM.put(EEaddress6, f);
  EEPROM.commit();
  delay(1000);
  digitalWrite(greenLED, HIGH);
  grinder_flag = 1;
}

void check7() {
  g++;
  Serial.println(g);
  digitalWrite(greenLED, LOW);
  lcd.print(0,1, "     Guest      ");
  EEPROM.put(EEaddress7, g);
  EEPROM.commit();
  delay(1000);
  digitalWrite(greenLED, HIGH);
  grinder_flag = 1;
}

void check8() {
  h++;
  Serial.println(h);
  digitalWrite(greenLED, LOW);
  lcd.print(0,1, "     User7      ");
  EEPROM.put(EEaddress8, h);
  EEPROM.commit();
  delay(1000);
  digitalWrite(greenLED, HIGH);
  grinder_flag = 1;
}

void check9() {
  i++;
  Serial.println(i);
  digitalWrite(greenLED, LOW);
  lcd.print(0,1, "     User8      ");
  EEPROM.put(EEaddress9, i);
  EEPROM.commit();
  delay(1000);
  digitalWrite(greenLED, HIGH);
  grinder_flag = 1;
}

void save_time(){
                  save_timer_flag = 0;
                  lcd.print(0,0, "    Command     ");
                  lcd.print(0,1, "   Save timer   ");
                  EEPROM.put(EEaddress0, timer_value);
                  EEPROM.commit();
                  delay(1000);
                  Blynk.virtualWrite(V8, LOW); // Save button OFF
                  timer_value_buffer = timer_value;
  }

void red_pulse(){
                  digitalWrite(redLED, LOW);
                  delay(1000);
                  digitalWrite(redLED, HIGH);
  
  }

void lock_card(){
                    switch (select_card){
                                          case 1: if (lock_card_flag) {
                                                                        lock_card_flag = 0;
                                                                        locked1 = 1;
                                                                        EEPROM.put(locked1_address, 1);
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V4, LOW); // Lock button OFF
                                                                       } 
                                                  else if(unlock_card_flag) {
                                                                              unlock_card_flag = 0;
                                                                              locked1 = 0;
                                                                              EEPROM.put(locked1_address, 0);
                                                                              EEPROM.commit();
                                                                              Blynk.virtualWrite(V5, LOW); // UnLock button OFF
                                                                            }
                                                  else if(lost_flag) {
                                                                        lost_flag = 0;
                                                                        ok_rfid_1 = read_rfid;
                                                                        ok_rfid_1.toCharArray(arrayToStore, ok_rfid_1.length()+1);  // Convert string to array.
                                                                        EEPROM.put(EEaddress21, arrayToStore);                 // To store data
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V10, LOW); // UnLock button OFF  
                                                                                                                                   
                                                                      }                             
                                                  
                                                  break; 
                                                  
                                          case 2: if (lock_card_flag) {
                                                                        lock_card_flag = 0;
                                                                        locked2 = 1;
                                                                        EEPROM.put(locked2_address, 1);
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V4, LOW); // Lock button OFF
                                                                       } 
                                                  else if(unlock_card_flag) {
                                                                              unlock_card_flag = 0;
                                                                              locked2 = 0;
                                                                              EEPROM.put(locked2_address, 0);
                                                                              EEPROM.commit();
                                                                              Blynk.virtualWrite(V5, LOW); // UnLock button OFF
                                                                            }
                                                  else if(lost_flag) {
                                                                        lost_flag = 0;
                                                                        ok_rfid_2 = read_rfid;
                                                                        ok_rfid_2.toCharArray(arrayToStore, ok_rfid_2.length()+1);  // Convert string to array.
                                                                        EEPROM.put(EEaddress22, arrayToStore);                 // To store data
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V10, LOW); // UnLock button OFF  
                                                                                                                                   
                                                                      }                             
                                                                       
                                                  break;

                                          case 3: if (lock_card_flag) {
                                                                        lock_card_flag = 0;
                                                                        locked3 = 1;
                                                                        EEPROM.put(locked3_address, 1);
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V4, LOW); // Lock button OFF
                                                                       } 
                                                  else if(unlock_card_flag) {
                                                                              unlock_card_flag = 0;
                                                                              locked3 = 0;
                                                                              EEPROM.put(locked3_address, 0);
                                                                              EEPROM.commit();
                                                                              Blynk.virtualWrite(V5, LOW); // UnLock button OFF
                                                                            }
                                                  else if(lost_flag) {
                                                                        lost_flag = 0;
                                                                        ok_rfid_3 = read_rfid;
                                                                        ok_rfid_3.toCharArray(arrayToStore, ok_rfid_3.length()+1);  // Convert string to array.
                                                                        EEPROM.put(EEaddress23, arrayToStore);                 // To store data
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V10, LOW); // UnLock button OFF  
                                                                                                                                   
                                                                      }                             
                                                                       
                                                  break; 

                                          case 4: if (lock_card_flag) {
                                                                        lock_card_flag = 0;
                                                                        locked4 = 1;
                                                                        EEPROM.put(locked4_address, 1);
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V4, LOW); // Lock button OFF
                                                                       } 
                                                  else if(unlock_card_flag) {
                                                                              unlock_card_flag = 0;
                                                                              locked4 = 0;
                                                                              EEPROM.put(locked4_address, 0);
                                                                              EEPROM.commit();
                                                                              Blynk.virtualWrite(V5, LOW); // UnLock button OFF
                                                                            }
                                                  else if(lost_flag) {
                                                                        lost_flag = 0;
                                                                        ok_rfid_4 = read_rfid;
                                                                        ok_rfid_4.toCharArray(arrayToStore, ok_rfid_4.length()+1);  // Convert string to array.
                                                                        EEPROM.put(EEaddress24, arrayToStore);                 // To store data
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V10, LOW); // UnLock button OFF  
                                                                                                                                   
                                                                      }                             
                                                                       
                                                  break;

                                         case 5: if (lock_card_flag) {
                                                                        lock_card_flag = 0;
                                                                        locked5 = 1;
                                                                        EEPROM.put(locked5_address, 1);
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V4, LOW); // Lock button OFF
                                                                       } 
                                                  else if(unlock_card_flag) {
                                                                              unlock_card_flag = 0;
                                                                              locked5 = 0;
                                                                              EEPROM.put(locked5_address, 0);
                                                                              EEPROM.commit();
                                                                              Blynk.virtualWrite(V5, LOW); // UnLock button OFF
                                                                            }
                                                  else if(lost_flag) {
                                                                        lost_flag = 0;
                                                                        ok_rfid_5 = read_rfid;
                                                                        ok_rfid_5.toCharArray(arrayToStore, ok_rfid_5.length()+1);  // Convert string to array.
                                                                        EEPROM.put(EEaddress25, arrayToStore);                 // To store data
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V10, LOW); // UnLock button OFF  
                                                                                                                                   
                                                                      }   
                                                                       
                                                  break;  

                                         case 6: if (lock_card_flag) {
                                                                        lock_card_flag = 0;
                                                                        locked6 = 1;
                                                                        EEPROM.put(locked6_address, 1);
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V4, LOW); // Lock button OFF
                                                                       } 
                                                  else if(unlock_card_flag) {
                                                                              unlock_card_flag = 0;
                                                                              locked6 = 0;
                                                                              EEPROM.put(locked6_address, 0);
                                                                              EEPROM.commit();
                                                                              Blynk.virtualWrite(V5, LOW); // UnLock button OFF
                                                                            }
                                                  else if(lost_flag) {
                                                                        lost_flag = 0;
                                                                        ok_rfid_6 = read_rfid;
                                                                        ok_rfid_6.toCharArray(arrayToStore, ok_rfid_6.length()+1);  // Convert string to array.
                                                                        EEPROM.put(EEaddress26, arrayToStore);                 // To store data
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V10, LOW); // UnLock button OFF  
                                                                                                                                   
                                                                      }   
                                                                       
                                                  break;  

                                        case 7: if (lock_card_flag) {
                                                                        lock_card_flag = 0;
                                                                        locked7 = 1;
                                                                        EEPROM.put(locked7_address, 1);
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V4, LOW); // Lock button OFF
                                                                       } 
                                                  else if(unlock_card_flag) {
                                                                              unlock_card_flag = 0;
                                                                              locked7 = 0;
                                                                              EEPROM.put(locked7_address, 0);
                                                                              EEPROM.commit();
                                                                              Blynk.virtualWrite(V5, LOW); // UnLock button OFF
                                                                            }
                                                  else if(lost_flag) {
                                                                        lost_flag = 0;
                                                                        ok_rfid_7 = read_rfid;
                                                                        ok_rfid_7.toCharArray(arrayToStore, ok_rfid_7.length()+1);  // Convert string to array.
                                                                        EEPROM.put(EEaddress27, arrayToStore);                 // To store data
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V10, LOW); // UnLock button OFF  
                                                                                                                                   
                                                                      }                             
                                                                       
                                                  break; 

                                        case 8: if (lock_card_flag) {
                                                                        lock_card_flag = 0;
                                                                        locked8 = 1;
                                                                        EEPROM.put(locked8_address, 1);
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V4, LOW); // Lock button OFF
                                                                       } 
                                                  else if(unlock_card_flag) {
                                                                              unlock_card_flag = 0;
                                                                              locked8 = 0;
                                                                              EEPROM.put(locked8_address, 0);
                                                                              EEPROM.commit();
                                                                              Blynk.virtualWrite(V5, LOW); // UnLock button OFF
                                                                            }
                                                  else if(lost_flag) {
                                                                        lost_flag = 0;
                                                                        ok_rfid_8 = read_rfid;
                                                                        ok_rfid_8.toCharArray(arrayToStore, ok_rfid_8.length()+1);  // Convert string to array.
                                                                        EEPROM.put(EEaddress28, arrayToStore);                 // To store data
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V10, LOW); // UnLock button OFF  
                                                                                                                                   
                                                                      }   
                                                                                        
                                                                       
                                                  break; 

                                       case 9: if (lock_card_flag) {
                                                                        lock_card_flag = 0;
                                                                        locked9 = 1;
                                                                        EEPROM.put(locked9_address, 1);
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V4, LOW); // Lock button OFF
                                                                       } 
                                                  else if(unlock_card_flag) {
                                                                              unlock_card_flag = 0;
                                                                              locked9 = 0;
                                                                              EEPROM.put(locked9_address, 0);
                                                                              EEPROM.commit();
                                                                              Blynk.virtualWrite(V5, LOW); // UnLock button OFF
                                                                            }
                                                  else if(lost_flag) {
                                                                        lost_flag = 0;
                                                                        ok_rfid_9 = read_rfid;
                                                                        ok_rfid_9.toCharArray(arrayToStore, ok_rfid_9.length()+1);  // Convert string to array.
                                                                        EEPROM.put(EEaddress29, arrayToStore);                 // To store data
                                                                        EEPROM.commit();
                                                                        Blynk.virtualWrite(V10, LOW); // UnLock button OFF  
                                                                                                                                   
                                                                      }   
                                                                                      
                                                                       
                                                  break;                                                                                                                                                         
                      
                      }
}

// END Functions ================================================================


// Begin Loop ===================================================================
void loop() {
  // put your main code here, to run repeatedly:

if (millis() < currentMillis) {
    currentMillis = 0;
  }

currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {   // 100ms timer
                                                      // save the last time
                                                      previousMillis = currentMillis;

                                                      if(save_timer_flag) save_time();
                                                      if(manual_flag) {
                                                                       manual_flag = 0;
                                                                       check7(); // increment Guest
                                                      }
                                                      
                                                      if(grinder_flag){
                                                                        timer_value_buffer--;
                                                                        if(timer_value_buffer > 0) {
                                                                                                  digitalWrite(ONOFF, LOW);
                                                                                                  //Serial.println(timer_value_buffer);
                                                                                               }
                                                                        else {
                                                                                digitalWrite(ONOFF, HIGH); 
                                                                                grinder_flag = 0;
                                                                                timer_value_buffer = timer_value;
                                                                                Blynk.virtualWrite(V9, LOW); // button OFF
                                                                             }
                                                        }
                                                        
                                                      else{ 
                                                        
                                                      
                                                                //Serial.println("reading");
                                                                // Look for new cards
                                                                if ( ! mfrc522.PICC_IsNewCardPresent())
                                                                  return;
                                                              
                                                                // Select one of the cards
                                                                if ( ! mfrc522.PICC_ReadCardSerial())
                                                                  return;
                                                              
                                                                dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
          
                                                                Serial.println(read_rfid); 
                                                                
                                                                if (read_rfid == ok_rfid_1) {
                                                                  if(!locked1) check1();
                                                                  else {
                                                                    lcd.print(0,1, " Card 1 locked! ");
                                                                    red_pulse();
                                                                    }
                                                                }
                                                                else if (read_rfid == ok_rfid_2) {
                                                                  if(!locked2) check2();
                                                                  else {
                                                                    lcd.print(0,1, " Card 2 locked! ");
                                                                    red_pulse();
                                                                    }
                                                                }
                                                                else if (read_rfid == ok_rfid_3) {
                                                                  if(!locked3) check3();
                                                                  else {
                                                                     lcd.print(0,1, " Card 3 locked! ");
                                                                     red_pulse();
                                                                     }
                                                                }
                                                                else if (read_rfid == ok_rfid_4) {
                                                                  if(!locked4) check4();
                                                                  else {
                                                                    lcd.print(0,1, " Card 4 locked! ");
                                                                    red_pulse();
                                                                    }
                                                                }
                                                                else if (read_rfid == ok_rfid_5) {
                                                                  if(!locked5) check5();
                                                                  else { 
                                                                    lcd.print(0,1, " Card 5 locked! ");
                                                                    red_pulse();
                                                                    }
                                                                  
                                                                }
                                                                else if (read_rfid == ok_rfid_6) {
                                                                  if(!locked6) check6();
                                                                  else { 
                                                                    lcd.print(0,1, " Card 6 locked! ");
                                                                    red_pulse();
                                                                    }
                                                                }
                                                                else if (read_rfid == ok_rfid_7) {
                                                                  if(!locked7) check7();
                                                                  else { 
                                                                    lcd.print(0,1, " Card 7 locked! ");
                                                                    red_pulse();
                                                                    }
                                                                }
                                                                else if (read_rfid == ok_rfid_8) {
                                                                  if(!locked8) check8();
                                                                  else { 
                                                                    lcd.print(0,1, " Card 8 locked! ");
                                                                    red_pulse();
                                                                    }
                                                                }
                                                                else if (read_rfid == ok_rfid_9) {
                                                                  if(!locked9) check9();
                                                                  else { 
                                                                    lcd.print(0,1, " Card 9 locked! ");
                                                                    red_pulse();
                                                                    }
                                                                }
                                                                else if(!lost_flag){
                                                                                    lcd.clear();
                                                                                    lcd.print(0,0,"Unknown card ID ");
                                                                                    lcd.print(0,1, read_rfid);
                                                                                    red_pulse();
                                                                       
                                                                                  }
                                                                else {
                                                                       lcd.clear();
                                                                       lcd.print(0,0,"New card? ");
                                                                       lcd.print(0,1, read_rfid);
                                                                       red_pulse();
                                                                       lock_card();
                                                                     }
                                                           }
                                             } 
  Blynk.run();
  timer.run(); // BlynkTimer is working...

}
// END Loop =====================================================
