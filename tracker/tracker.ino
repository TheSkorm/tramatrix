/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h> 
#include <EEPROM.h>

ESP8266WebServer server(80);
MDNSResponder mdns;

//connect DIN to D7
//connect CLK to D5
//connect CS  to D8

Max72xxPanel matrix = Max72xxPanel(D8, 1, 1);

const char* hostSSID = "Tramatrix";

const char* host = "ws.tramtracker.com.au";

const char* post1 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">  <soap12:Header>    <PidsClientHeader xmlns=\"http://www.yarratrams.com.au/pidsservice/\">      <ClientGuid>157786eb-d34a-407c-850b-c5d182e6556f</ClientGuid>      <ClientType>DASHBOARDWIDGET</ClientType>      <ClientVersion>1.0</ClientVersion>      <ClientWebServiceVersion>6.4.0.0</ClientWebServiceVersion>    </PidsClientHeader>  </soap12:Header> <soap12:Body>    <GetNextPredictedRoutesCollection xmlns=\"http://www.yarratrams.com.au/pidsservice/\">      <stopNo>";
const char* post2 = "</stopNo>      <routeNo>0</routeNo>      <lowFloor>false</lowFloor>    </GetNextPredictedRoutesCollection>  </soap12:Body></soap12:Envelope>";
const char* url = "/pidsservice/pids.asmx";

const char* roothtml = "<!DOCTYPE html><html lang=\"en\"> <head> <meta charset=\"utf-8\"> <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>Tramatrix</title><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css\"><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap-theme.min.css\"><!--[if lt IE 9]> <script src=\"https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js\"></script> <script src=\"https://oss.maxcdn.com/respond/1.4.2/respond.min.js\"></script><![endif]--> </head> <body> <nav class=\"navbar navbar-inverse navbar-fixed-top\"> <div class=\"container\"> <div class=\"navbar-header\"> <button type=\"button\" class=\"navbar-toggle collapsed\" data-toggle=\"collapse\" data-target=\"#navbar\" aria-expanded=\"false\" aria-controls=\"navbar\"> <span class=\"sr-only\">Toggle navigation</span> <span class=\"icon-bar\"></span> <span class=\"icon-bar\"></span> <span class=\"icon-bar\"></span> </button> <a class=\"navbar-brand\" href=\"#\">Tramatrix</a> </div><div id=\"navbar\" class=\"collapse navbar-collapse\"> <ul class=\"nav navbar-nav\"> <li class=\"active\"><a href=\"#\">Config</a></li></ul> </div></div></nav> <div class=\"container\"> <div class=\"starter-template\" style=\"padding: 40px 15px;\"> <h1>Tramatrix Config</h1> <p class=\"lead\"></p></div><form action=\"/update\" method=\"get\"> <div class=\"form-group\"> <label for=\"ssid\">SSID</label> <input class=\"form-control\" name=\"ssid\" id=\"ssid\" placeholder=\"SSID\"> </div><div class=\"form-group\"> <label for=\"key\">Wireless Key</label> <input name=\"key\" type=\"password\" class=\"form-control\" id=\"key\" placeholder=\"Password\"> </div><div class=\"form-group\"> <label for=\"stop\">Stop ID</label> <input name=\"stop\" class=\"form-control\" id=\"stop\" placeholder=\"Stop ID\"> </div><button type=\"submit\" class=\"btn btn-default\">Submit</button> </form> </div></body></html>";
const char* updatedhtml = "<!DOCTYPE html><html lang=\"en\"> <head> <meta charset=\"utf-8\"> <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>Tramatrix</title><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css\"><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap-theme.min.css\"><!--[if lt IE 9]> <script src=\"https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js\"></script> <script src=\"https://oss.maxcdn.com/respond/1.4.2/respond.min.js\"></script><![endif]--> </head> <body> <nav class=\"navbar navbar-inverse navbar-fixed-top\"> <div class=\"container\"> <div class=\"navbar-header\"> <button type=\"button\" class=\"navbar-toggle collapsed\" data-toggle=\"collapse\" data-target=\"#navbar\" aria-expanded=\"false\" aria-controls=\"navbar\"> <span class=\"sr-only\">Toggle navigation</span> <span class=\"icon-bar\"></span> <span class=\"icon-bar\"></span> <span class=\"icon-bar\"></span> </button> <a class=\"navbar-brand\" href=\"#\">Tramatrix</a> </div><div id=\"navbar\" class=\"collapse navbar-collapse\"> <ul class=\"nav navbar-nav\"> <li class=\"active\"><a href=\"#\">Config</a></li></ul> </div></div></nav> <div class=\"container\"> <div class=\"starter-template\" style=\"padding: 40px 15px;\"> <h1>Tramatrix Config</h1> <p class=\"lead\">Configuration Updated</p></div></div></body></html>";


unsigned long lastgoodtime;
long lastgoodtilltram;

unsigned long previousMillis = 0;        // will store last time LED was updated

const long interval = 5000; 

String stopid = "";

void handleNotFound(){
 // digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
 // digitalWrite(led, 0);
}

void handleRoot() {
  //digitalWrite(led, 1);
  server.send(200, "text/html", roothtml);
  //digitalWrite(led, 0);
}

void handleUpdate(){
  for (uint8_t i=0; i<server.args(); i++){
    if (server.argName(i) == "ssid"){
      String ssid = String(server.arg(i));
      Serial.println(ssid);
      int x;
      for (x=0;x<ssid.length();x++)
        EEPROM.write(x,ssid[x]);
      EEPROM.write(x,0x00); // terminate string
    } else if (server.argName(i) == "stop"){
      stopid = String(server.arg(i));
      Serial.println(stopid);
      int x;
      for (x=0;x<stopid.length();x++)
        EEPROM.write(128+x,stopid[x]);
      EEPROM.write(128+x,0x00); // terminate string
    } else if (server.argName(i) == "key"){
      String key = String(server.arg(i));
      Serial.println(key);
      int x;
      for (x=0;x<key.length();x++)
        EEPROM.write(x+64,key[x]);
      EEPROM.write(x+64,0x00); // terminate string
    }
  }
  EEPROM.commit();
  server.send(200, "text/html", updatedhtml);
  softReset();
}

void setup() {
  delay(1000);
  matrix.setIntensity(4);
  matrix.fillScreen(LOW); // clear the screen
  matrix.write();
  mdns.addService("http", "tcp", 80);
  Serial.begin(115200);
  EEPROM.begin(256);
  delay(10);
  byte value = 0;
  char eepromSSID[64];
  char eepromKEY[64];
  char eepromSTOP[64];
  for (int x=0;x<64;x++)
    eepromSSID[x] = EEPROM.read(x);
  for (int x=0;x<64;x++)
    eepromKEY[x] = EEPROM.read(64+x);
  for (int x=0;x<64;x++)
    eepromSTOP[x] = EEPROM.read(128+x);
  stopid = String(eepromSTOP);
  Serial.println(stopid);
  // We start by connecting to a WiFi network
  matrix.drawChar(3, 1, '-', HIGH, LOW, 1);
  matrix.write();
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");

  Serial.print(eepromSSID);
    Serial.print("/");
    Serial.println(eepromKEY);
  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
          for (int i = 0; i < strlen(eepromSSID); ++i) {
          Serial.printf("%02x ", eepromSSID[i]);
        }
        Serial.println("");
        
  server.begin();
  WiFi.begin(eepromSSID, eepromKEY);
  
  int x = 0;
  while (WiFi.status() != WL_CONNECTED && x < 60) {
    delay(500);
    Serial.print(".");
    x++;
  }

  if (WiFi.status() != WL_CONNECTED){
    while(1) {
      yield();
      WiFi.softAP(hostSSID);
      IPAddress myIP = WiFi.softAPIP();
      matrix.fillScreen(LOW); // clear the screen
      matrix.drawChar(3, 1, 'S', HIGH, LOW, 1);
      matrix.write();
      server.handleClient();
      MDNS.begin("tramatrix", WiFi.localIP());
    }
  } else {
      MDNS.begin("tramatrix", WiFi.localIP());
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  matrix.fillScreen(LOW); // clear the screen
  matrix.drawChar(3, 1, 'C', HIGH, LOW, 1);
  matrix.write();
}


void loop() {
    unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
   previousMillis = currentMillis;  
 
 updateData();
  }
 server.handleClient();
}

void renderTime(long minutestotram){
if (minutestotram < 10) {
    matrix.fillScreen(LOW); // clear the screen
    matrix.drawChar(3, 1, 48 + minutestotram, HIGH, LOW, 1);
    matrix.write();
  } else if (minutestotram < 20) {
    matrix.fillScreen(LOW); // clear the screen
    matrix.drawChar(-1, 1, 48 + 1, HIGH, LOW, 1);
    matrix.drawChar(3, 1, 48 + minutestotram-10, HIGH, LOW, 1);
    matrix.write();
  }
}


void updateData()
{
    long secondstotram;
   Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    //return;
  }



  Serial.print("Requesting URL: ");
  Serial.println(url);
  String postmessage =  post1;
  postmessage += stopid;
  postmessage += post2;
  Serial.println(postmessage);
  // This will send the request to the server
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Length: " + postmessage.length() + "\r\n" +
               "Content-Type: text/xml" + "\r\n" +
               "Connection: close\r\n\r\n" + postmessage + "\r\n\r\n");
  delay(1500);



  while (client.available()) {

    int predyear ;
    int predmonth ;
    int predday;
    int predhour ;
    int predminute ;
    int predsecond ;

    String line = client.readStringUntil('<');
    Serial.println(line);
    // 2015-09-04T14:15:15+10:00</PredictedArrivalDateTime
    // 2015-09-04T14:11:40.1108156+10:00</RequestDateTime
    if (line.startsWith("PredictedArrivalDateTime")) {
      predyear = line.substring(25, 25 + 4).toInt();
      predmonth = line.substring(30, 30 + 2).toInt();
      predday = line.substring(33, 33 + 2).toInt();
      predhour = line.substring(36, 36 + 2).toInt();
      predminute = line.substring(39, 39 + 2).toInt();
      predsecond = line.substring(42, 42 + 2).toInt();


    }
    if (line.startsWith("RequestDateTime")) {
      int reqyear = line.substring(16, 16 + 4).toInt();
      int reqmonth = line.substring(21, 21 + 2).toInt();
      int reqday = line.substring(24, 24 + 2).toInt();
      int reqhour = line.substring(27, 27 + 2).toInt();
      int reqminute = line.substring(30, 30 + 2).toInt();
      int reqsecond = line.substring(33, 33 + 2).toInt();

      // so the years and months will make this in accurate but considering that our longest time is going to be 9 minutes it doesn't matter
      secondstotram = (365 * 30 * 24 * 60 * 60 * (predyear - reqyear)) + (30 * 24 * 60 * 60 * (predmonth - reqmonth)) + (24 * 60 * 60 * (predday - reqday)) + (60 * 60 * (predhour - reqhour) + 60 * (predminute - reqminute)) + (predsecond - reqsecond);
      break;
    }

  }

  Serial.println();
  Serial.println("closing connection");

  int minutestotram = secondstotram / 60;
  Serial.print("Next tram in : ");
  Serial.println(minutestotram);

  if (minutestotram < 100000) { // because this ends up a large number if the request fails we just catch it - better way to do this but meh
    lastgoodtime = millis();
    lastgoodtilltram = secondstotram;
  }
  if (minutestotram < 20) {
    renderTime(minutestotram);
  } else if (minutestotram < 100000) {
    matrix.fillScreen(LOW); // clear the screen
    matrix.drawChar(3, 1, 'W', HIGH, LOW, 1);
    matrix.write();
  } else {
    Serial.println("Failed to get minutes");
    long minutescalculated = (lastgoodtilltram - ((millis() - lastgoodtime) / 1000)) / 60;
    Serial.print("Calculated minutes :");
    Serial.println(minutescalculated);
    if (minutescalculated >= 0 && lastgoodtime) {
      renderTime(minutescalculated);
    } else {
      matrix.fillScreen(LOW); // clear the screen
      matrix.drawChar(3, 1, 70, HIGH, LOW, 1);
      matrix.write();
    }
  }
}


void softReset(){ // hacking around my bad code
 while(1);
}
