/*------------------------------------------------------------------------------

ESP MONITOR
Version : 1.0
Author  : NyessId

------------------------------------------------------------------------------*/
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <FS.h>
#define softwareName "ESP Monitor"
#define softwareVersion "V 1.0"

ESP8266WebServer server;
LiquidCrystal_I2C lcd(0x27,20,4); 

uint8_t pin_led           = 2;
const int maxPingAttempts = 5;

const char* ssid          = "";
const char* ap_ssid       = softwareName;
const char* password      = "espmon123";     // password saat mode AP

unsigned long
statusMillis  = 0,
connMillis    = 0,
pageMillis    = 0;

String
s_data1       = "",
s_data2       = "",
s_data3       = "",
s_data4       = ""; 

byte insecond    = 0;

byte smile[8] = {
0b00000,
0b00000,
0b01010,
0b00000,
0b10001,
0b01110,
0b00000,
0b00000
};

char webpage[] PROGMEM = R"=====(
<html>
<head>
  <meta charset="utf-8">
  <meta http-equiv="pragma" content="no-cache" />
  <meta http-equiv="expires" content="-1" />
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no" />
  <title>ESP Monitor</title>
  <style>
body,
div,
form,
html,
input,
label
{
    margin: 0;
    padding: 0;
    border: 0;
    font-family: sans-serif, Arial
}
body,html {
    min-height: 100%;
    overflow-x: hidden
}
body {
    background: #343434
}
a {
    color: #fff
}
input,
label {
    vertical-align: middle;
    white-space: normal;
    background: 0;
    line-height: 1
}
label {
    color: #fff;
    position: relative;
    display: block
}
p::first-letter {
    text-transform: uppercase
}
.main {
    min-height: calc(100vh - 90px);
    width: 100%;
    display: -webkit-box;
    display: -ms-flexbox;
    display: flex;
    -webkit-box-orient: vertical;
    -webkit-box-direction: normal;
    -ms-flex-direction: column;
    flex-direction: column
}
.ie-fixMinHeight {
    display: -webkit-box;
    display: -ms-flexbox;
    display: flex
}
h1 {
    color: #fff;
    font-size: 32px !important;
    margin-bottom: 10px
}
h3 {
    color: #fff;
}
p,span {
    color: #fff;
    fon-size: 12px
}

* {
    -webkit-box-sizing: border-box;
    box-sizing: border-box;
    font-size: 16px
}
.wrap {
    margin: auto;
    padding: 50px;
    -webkit-transition: width .3s ease-in-out;
    transition: width .3s ease-in-out
}
.bordered {
    border: solid 1px #eee;
    border-radius: 8px;
    padding: 5px 20px 20px 20px;
}

.footer {
    margin-top: 15px;
    color: #fff;
    font-size:12px !important;
    text-align: center
}

.footer a{
    font-size:12px !important;
    font-weight: bold;
    text-decoration: none
}

@media only screen and (min-width:1px) and (max-width:575px) {
    .wrap {
        width: 100%
    }
}
form {
    width: 100%;
    margin-bottom: 20px
}
@-webkit-keyframes fadeIn {
    from {
        opacity: 0
    }
    to {
        opacity: 1
    }
}
@keyframes fadeIn {
    from {
        opacity: 0
    }
    to {
        opacity: 1
    }
}
.fadeIn {
    -webkit-animation-name: fadeIn;
    animation-name: fadeIn
}
.animated {
    -webkit-animation-duration: 1s;
    animation-duration: 1s;
    -webkit-animation-fill-mode: both;
    animation-fill-mode: both
}
.center-text {
    text-align: center;
    border: 0 !important
}

input {
    outline: 0;
    -webkit-appearance: none;
    -moz-appearance: none;
    appearance: none;
    text-decoration: none
}
input:focus {
    outline: 0;
    text-decoration: none
}
input[type=password],input[type=text],select {
    width: 100%;
    color: #555;
    outline: 0;
    text-decoration: none;
    border: 1px solid background-color:rgba(255, 255, 255, .8);
    height: 30px;
    padding: 3px 20px 3px 20px;
    margin-bottom: 20px;
    border-radius: 6px;
    background-color: rgba(255, 255, 255, .8);
    -webkit-transition: -webkit-box-shadow .3s ease-in-out;
    transition: -webkit-box-shadow .3s ease-in-out;
    transition: box-shadow .3s ease-in-out;
    transition: box-shadow .3s ease-in-out, -webkit-box-shadow .3s ease-in-out
}
input[type=password]:focus,input[type=text]:focus {
    -webkit-box-shadow: 0 0 5px 0 rgba(255, 255, 255, 1);
    box-shadow: 0 0 5px 0 rgba(255, 255, 255, 1)
}
button {
    background: #3e4d59;
    color: #fff;
    font-weight: bold;
    border: 0;
    cursor: pointer;
    text-align: center;
    width: 100%;
    height: 44px;
    border-radius: 6px;
    -webkit-transition: background .3s ease-in-out;
    transition: background .3s ease-in-out
}
button:focus,button::hover {
    background: #33404a
}
@media(min-width:576px) {
    .wrap {
        width: 410px
    }
    * {
        font-size: 14px !important
    }
}
  </style>
</head>
<body>
  <form>
      <div class="ie-fixMinHeight">
        <div class="main">
          <div class="wrap animated fadeIn">
            <h1 class="center-text">ESP Monitor</h1>
            <div class="bordered">
              <h3 id="wifiStatus"></h3>
              <h3 id="connectedSSID"></h3>
              <label for="ssid">Nama ssid</label>      
              <input type="text" value="" id="ssid" placeholder="SSID" autocomplete="off">
              <div id="passwordField">
                 <label for="password">Password ssid</label>
                 <input type="password" value="" id="password" placeholder="PASSWORD" autocomplete="off">
              </div>
              <label for="encryptionMode"></label>
              <select id="encryptionMode" onchange="handleEncryptionMode()">
                 <option value="wpa" selected>Mode Enkripsi: WPA</option>
                 <option value="open">Mode Enkripsi: Open</option>
              </select>
              <button id="savebtn" type="button" onclick="saveFunction()">SIMPAN</button>
            </div>
            <div class="footer">ESP8266 Monitor Hotspot MikroTik by <a href="https://www.tiktok.com/@nys.pjr" target="_blank">NyessId</a></>
           </div>
        </div>
      </div>
  </form>
</body>
<script>
    function handleEncryptionMode() {
      var encryptionMode = document.getElementById("encryptionMode").value;
      var passwordField = document.getElementById("passwordField");
      
      if (encryptionMode === "open") {
        passwordField.style.display = "none";
      } else {
        passwordField.style.display = "block";
      }
    }
    
    function updateWiFiStatus() {
      var wifiStatusDiv = document.getElementById("wifiStatus");
      var connectedSSIDDiv = document.getElementById("connectedSSID");

      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (xhr.readyState === 4 && xhr.status === 200) {
          var wifiStatus = JSON.parse(xhr.responseText);
          wifiStatusDiv.textContent = "Status Wi-Fi : " + wifiStatus.status;
        }
      };
      xhr.open("GET", "/wifi-status", true);
      xhr.send();

      // Tampilkan nama SSID yang terhubung
      getConnectedSSID(function(connectedSSID) {
        connectedSSIDDiv.textContent = "SSID : " + connectedSSID;
      });
    }

    function getConnectedSSID(callback) {
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (xhr.readyState === 4 && xhr.status === 200) {
          var response = JSON.parse(xhr.responseText);
          var connectedSSID = response.ssid;
          callback(connectedSSID);
        }
      };
      xhr.open("GET", "/connected-ssid", true);
      xhr.send();
    }

    function saveFunction(){
      var ssid = document.getElementById("ssid").value;
      var password = document.getElementById("password").value;
      var data = {ssid:ssid, password:password};
      var encryptionMode = document.getElementById("encryptionMode").value;
      if (ssid.trim() === "") {
        alert("Input SSID!");
        return;
      }

      if (encryptionMode === "wpa" && password.trim() === "") {
        alert("Input Password!");
        return;
      }
      
      var xhr = new XMLHttpRequest();
      xhr.open("POST", "/update", true);
      xhr.onreadystatechange = function() {
        if (xhr.readyState === 4) {
          if (xhr.status === 200) {
            alert("Data berhasil dikirim.");
            updateWiFiStatus();
          } else {
            alert("Gagal mengirim data.");
          }
        }
      };
      xhr.send(JSON.stringify(data));
    }

    updateWiFiStatus();
    setInterval(updateWiFiStatus, 5000);
  </script>
</html>
)=====";

void wifiConnect(){
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();      
  delay(1000);
  if(SPIFFS.exists("/config.json")){
    const char * _ssid = "", *_pass = "";
    File configFile = SPIFFS.open("/config.json", "r");
    if(configFile){
      size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      configFile.close();
      DynamicJsonDocument doc(512);
      DeserializationError error = deserializeJson(doc,buf.get());
      if(!error){
        _ssid = doc["ssid"];
        _pass = doc["password"];
        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _pass);
        unsigned long startTime = millis();
        lcd.clear();
        delay(300);
        lcd.setCursor(0,0);
        lcd.print(softwareName);
        lcd.setCursor(0,1);
        lcd.print("Menunggu Wi-Fi..");
        while (WiFi.status() != WL_CONNECTED) 
        {
          delay(500);
          Serial.print(".");
          digitalWrite(pin_led,!digitalRead(pin_led));
          if ((unsigned long)(millis() - startTime) >= 5000) break;
        }
      }
    }
  }
  
  if (WiFi.status() == WL_CONNECTED){
    digitalWrite(pin_led,HIGH);
    lcd.clear();
    delay(300);
    lcd.setCursor(0,0);
    lcd.print(softwareName);
    lcd.setCursor(0,1);
    lcd.print("Wi-Fi terhubung!");
    delay(2000);
    lcd.setCursor(0,1);
    lcd.print("Menunggu data.. ");
  } else { 
    lcd.clear();
    delay(300);
    lcd.setCursor(0,0);
    lcd.print("Wi-Fi Error!");
    lcd.setCursor(0,1);
    lcd.print("Auto Mode : AP");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, password); 
    digitalWrite(pin_led,LOW);      
  }
  Serial.println("");
  WiFi.printDiag(Serial);
}


void handleSettingsUpdate(){
  String data = server.arg("plain");
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc,data);
  if(!error){
    if(doc.containsKey("data1") && doc.containsKey("data2")&& doc.containsKey("data3")) {
       String data1 = doc["data1"];         
       String data2 = doc["data2"]; 
       String data3 = doc["data3"];
       String data4 = doc["data4"];
       s_data1 = data1;
       s_data2 = data2;
       s_data3 = data3;
       s_data4 = data4;
       server.send(200, "application/json", "{\"status\" : \"ok\"}");
    } else {
       File configFile = SPIFFS.open("/config.json", "w");
       serializeJson(doc, configFile);  
       configFile.close();
       server.send(200, "application/json", "{\"status\" : \"ok\"}");
       delay(500);
       wifiConnect();
    }
  }
}

void handleWiFiStatus() {
  String wifiStatus = WiFi.status() == WL_CONNECTED ? "Terhubung" : "Terputus";
  server.send(200, "application/json", "{\"status\":\"" + wifiStatus + "\"}");
}

void handleConnectedSSID() {
  String connectedSSID = WiFi.SSID();
  server.send(200, "application/json", "{\"ssid\":\"" + connectedSSID + "\"}");
}

void welcomePage(){
  if(s_data1 != 0){
  lcd.clear();
  delay(200);
  lcd.setCursor(0,0);
  lcd.print("Selamat datang");
  lcd.setCursor(0,1);
  lcd.print("di "+s_data1);
  lcd.createChar(1, smile); 
  lcd.setCursor(15,1);
  lcd.write(1);
  }
}

void wifiPassPage(){
  if(s_data2 != 0){
  lcd.clear();
  delay(200);
  lcd.setCursor(0,0);
  lcd.print("Sandi Wi-Fi :");
  lcd.setCursor(0,1);
  lcd.print(s_data2);
  }
}

void closedPage(){
  if(s_data3 != 0){
  lcd.clear();
  delay(200);
  lcd.setCursor(0,0);
  lcd.print("Warkop Tutup :");
  lcd.setCursor(0,1);
  lcd.print("Pukul "+s_data3.substring(0,5)+" WIB");
  }
}

void userActPage(){
  if(s_data4 != 0){
  lcd.clear();
  delay(200);
  lcd.setCursor(0,0);
  lcd.print("User Active :");
  lcd.setCursor(0,1);
  lcd.print(s_data4+" users");
  }
}

void setup(){
  Serial.begin(9600);
  lcd.init();
  lcd.clear();
  delay(1000);
  lcd.backlight();
  lcd.clear();
  delay(300);
  lcd.setCursor(0,0);
  lcd.print(softwareName);
  lcd.setCursor(0,1);
  lcd.print(softwareVersion);
  pinMode(pin_led, OUTPUT);
  SPIFFS.begin();
  WiFi.hostname(softwareName);
  wifiConnect();
  server.on("/",[](){server.send_P(200,"text/html", webpage);});
  server.on("/wifi-status", handleWiFiStatus);
  server.on("/connected-ssid", handleConnectedSSID);
  server.on("/update", HTTP_POST, handleSettingsUpdate);
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  server.begin();
}

void loop(){
  server.handleClient();
  if(WiFi.status() != WL_CONNECTED){
    if(millis() - statusMillis >= 60000) {
      statusMillis = millis(); 
      Serial.println("Wi-Fi Error!, menghubungkan kembali..");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Wi-Fi Error!");
      lcd.setCursor(0,1);
      lcd.print("Menghubungkan.. ");
      if(millis() - connMillis >= 2000){
         connMillis = millis();
         wifiConnect();
         
       }
     }
     insecond = 0;
    } else {
        if(millis() - pageMillis >= 1200){
           pageMillis = millis();
           insecond++;
             if(insecond == 2){
               welcomePage();
             }
             if(insecond == 4){
               wifiPassPage();
             }
             if(insecond == 14){
               closedPage();
             }
             if(insecond == 16){
               userActPage();
               insecond = 0;
             }
          }
      }
}
