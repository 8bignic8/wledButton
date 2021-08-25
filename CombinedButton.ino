/**
 * Changed by Nico Geiger 24082021
 * WiFiManager advanced demo, contains advanced configurartion options
 * Implements TRIGGEN_PIN button press, press for ondemand configportal, hold for 3 seconds for reset settings.
 */
/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
 
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFi.h>
#include <HTTPClient.h>
#include <Preferences.h>

///DeepSleep Part
#define Threshold 40 /* Greater the value, more the sensitivity */

Preferences preferences;
WiFiManager wm; // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )

// Define Trigger Ping in running 
uint8_t TRIGGER_PIN = 0; // defining Trigger Pin
const char* password  = "42042042";
const char* ssid      = "WLED_Button"; //let it blank "" for no password
byte touch;
int pushed;
String finalReQ;
RTC_DATA_ATTR int bootCount = 0;
touch_pad_t touchPin;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;

///Variable for the http Req
String reqOne = "http://10.0.1.86/win=&T=2"; //initalizing
String reqTwo = "http://10.0.1.86/win=&T=0";
String reqThr = "http://10.0.1.86/win=&T=1";

void checkButton(){
  // check for button press
  Serial.println(digitalRead(TRIGGER_PIN));
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    // poor mans debounce/press-hold, code not ideal for production
    delay(50);
    if( digitalRead(TRIGGER_PIN) == LOW ){
      Serial.println("Button Pressed");
      // still holding button for 3000 ms, reset settings, code not ideal for production
      delay(3000); // reset delay hold
      if( digitalRead(TRIGGER_PIN) == HIGH ){
        Serial.println("Button Held");
        Serial.println("Erasing Config, restarting");
        wm.resetSettings();
        ESP.restart();
      }
    }
  }
}


String getParam(String name){
  //read parameter from server, for customhmtl input
  String value;
  if(wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void callback(){
  //placeholder callback function for Touch Trigger
  finalReQ = reqTwo+"&SN=1";
  Serial.print(finalReQ);
  
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
   //To get the Value
   reqOne = getParam("ShortPress");
   reqTwo = getParam("DublePress");
   reqThr = getParam("Hold");
   TRIGGER_PIN = getParam("TriggerP").toInt();
   
   pinMode(TRIGGER_PIN, INPUT);
   
   preferences.begin("Buttons", false); //initialazing the Library 
   preferences.putString("ShortPress", reqOne); //Saving in ShortPress that inforamtion of reqOne
   preferences.putString("DublePress", reqTwo);
   preferences.putString("Hold", reqThr);
   preferences.putInt("TriggerP", TRIGGER_PIN);
   preferences.end();
   Serial.println(reqOne);
   Serial.println(reqTwo);
   Serial.println(reqThr); 
   Serial.println(TRIGGER_PIN);
}

void requestToHTTP(String request){
  //touchPin = esp_sleep_get_touchpad_wakeup_status(); // Reads the touch Pin
  //Send an HTTP POST request every 10 minutes
  //if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    Serial.print("HTTP Req");
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      
      // Bulid your server request
      String serverPath = request;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode); //Result Code
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis(); 
}



void setup() {
  //wm.resetSettings(); // Delete all Settings
  

  
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.begin(115200);
   //Save the number of interrupts
   preferences.begin("Buttons", false); //initialazing the Library 
   pushed = pushed +1;
   preferences.putInt("pushed", pushed); //Saving in ShortPress that inforamtion of reqOne
   preferences.end();
  ++bootCount;
  Serial.println("Pushed: " + String(pushed)); // How many Boots there been

  //Touchpad Filter
  //touch_pad_filter_start(T3);
  //touch_pad_set_filter_period(T3);
  
   preferences.begin("Buttons", false); //initialazing the Library 
  //Initializing The data base with initial String
  if(preferences.getString("ShortPress","")==""){
   preferences.putString("ShortPress", reqOne); //Saving in ShortPress that inforamtion of reqOne
   preferences.putString("DublePress", reqTwo);
   preferences.putString("Hold", reqThr);
   preferences.putInt("TriggerP", TRIGGER_PIN);
  }
  else{
   reqOne = preferences.getString("ShortPress", ""); //Saving in ShortPress that inforamtion of reqOne
   reqTwo = preferences.getString("DublePress", ""); // Second Argument is in case there isnt any return
   reqThr = preferences.getString("Hold", "");
   TRIGGER_PIN = preferences.getInt("TriggerP", 14); 
    }
  preferences.end();

  finalReQ = reqOne;
  //Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T3, callback, Threshold);
  //esp_sleep_enable_touchpad_wakeup();
  
  //Serial.setDebugOutput(true);  
  checkButton(); //checks for AP Mode
  //delay(1000);
  Serial.println("\n Starting");

  
  if(reqOne == ""){
    wm.resetSettings(); // wipe settings
   }

   
  // add a custom input field
  int customFieldLength = 40;

  // Text input Field
  const char* custom_radio_str = "<br/><label for='AllBu'>Short press </label> <input type='text' name='ShortPress' value='http://10.0.1.86/win=&T=2&SN=0'>Duble Press </label> <input type='text' name='DublePress' value='http://10.0.1.86/win=&T=1&A=128'>Hold </label> <input type='text' name='Hold' value='http://10.0.1.86/win=&T=0'>ResetPin_LOW <input type='text' name='TriggerP' value='0'>";
  new (&custom_field) WiFiManagerParameter(custom_radio_str); // custom html input
  
  wm.addParameter(&custom_field);
  wm.setSaveParamsCallback(saveParamCallback);
  // custom menu via array or vector
  checkButton(); //checks for AP Mode
  
  Serial.println("is now TriggerPin: ");
  Serial.println(TRIGGER_PIN);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
  // wm.setMenu(menu,6);
  std::vector<const char *> menu = {"wifi","param","exit","sep","info","restart"};
  wm.setMenu(menu);

  // set dark theme
  wm.setClass("invert");


  //set static ip
  // wm.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); // set static ip,gw,sn
  // wm.setShowStaticFields(true); // force show static ip fields
  // wm.setShowDnsFields(true);    // force show dns field always

  // wm.setConnectTimeout(20); // how long to try to connect for before continuing
  wm.setConfigPortalTimeout(30); // auto close configportal after n seconds
  wm.setCaptivePortalEnable(true); // disable captive portal redirection
  wm.setAPClientCheck(true); // avoid timeout if client connected to softap

  // wifi scan settings
  // wm.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
  wm.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%
  // wm.setShowInfoErase(false);      // do not show erase button on info page
  // wm.setScanDispPerc(true);       // show RSSI as percentage not graph icons
  
  // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect(ssid,password); // password protected ap

  if(!res) {
    Serial.println("Failed to connect or hit timeout");
    ESP.restart();
  } 
  else {
    //if you get here you have connected to the WiFi    
    Serial.println("Internet connected :)");
  }

/// Here should be the right httpRequest depending on the button pressing

  delay(1000);
  
  Serial.println("sending request");
  Serial.println(finalReQ);
  requestToHTTP(finalReQ);
  //Serial.println(reqOne);
  
  esp_sleep_enable_touchpad_wakeup(); //Loads the Touchpad again
  
  ///Keep on sleeping :)
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  //Nothing here!!!
}



void loop() {
  
  // put your main code here, to run repeatedly:
}
