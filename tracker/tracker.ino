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

//connect DIN to D7
//connect CLK to D5
//connect CS  to D8

Max72xxPanel matrix = Max72xxPanel(D8, 1, 1);

const char* ssid     = "";
const char* password = "";

const char* host = "ws.tramtracker.com.au";

const char* post = "<?xml version=\"1.0\" encoding=\"utf-8\"?><soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">  <soap12:Header>    <PidsClientHeader xmlns=\"http://www.yarratrams.com.au/pidsservice/\">      <ClientGuid>157786eb-d34a-407c-850b-c5d182e6556f</ClientGuid>      <ClientType>DASHBOARDWIDGET</ClientType>      <ClientVersion>1.0</ClientVersion>      <ClientWebServiceVersion>6.4.0.0</ClientWebServiceVersion>    </PidsClientHeader>  </soap12:Header> <soap12:Body>    <GetNextPredictedRoutesCollection xmlns=\"http://www.yarratrams.com.au/pidsservice/\">      <stopNo>1354</stopNo>      <routeNo>0</routeNo>      <lowFloor>false</lowFloor>    </GetNextPredictedRoutesCollection>  </soap12:Body></soap12:Envelope>";
const char* contentlength = "816";
const char* url = "/pidsservice/pids.asmx";

unsigned long lastgoodtime;
long lastgoodtilltram;

void setup() {
  matrix.setIntensity(4);
  matrix.fillScreen(LOW); // clear the screen
  matrix.write();
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  matrix.drawChar(3, 0, '-', HIGH, LOW, 1);
  Serial.println();
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
  matrix.fillScreen(LOW); // clear the screen
  matrix.drawChar(3, 0, 'C', HIGH, LOW, 1);
  matrix.write();
}


void loop() {
  long secondstotram;
  delay(5000);

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

  // This will send the request to the server
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Length: " + contentlength + "\r\n" +
               "Content-Type: text/xml" + "\r\n" +
               "Connection: close\r\n\r\n" + post + "\r\n\r\n");
  delay(1500);



  while (client.available()) {

    int predyear ;
    int predmonth ;
    int predday;
    int predhour ;
    int predminute ;
    int predsecond ;

    String line = client.readStringUntil('<');
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
    matrix.drawChar(3, 0, 'W', HIGH, LOW, 1);
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
      matrix.drawChar(3, 0, 70, HIGH, LOW, 1);
      matrix.write();
    }
  }

}

void renderTime(long minutestotram){
if (minutestotram < 10) {
    matrix.fillScreen(LOW); // clear the screen
    matrix.drawChar(3, 0, 48 + minutestotram, HIGH, LOW, 1);
    matrix.write();
  } else if (minutestotram < 20) {
    matrix.fillScreen(LOW); // clear the screen
    matrix.drawChar(-1, 0, 48 + 1, HIGH, LOW, 1);
    matrix.drawChar(3, 0, 48 + minutestotram-10, HIGH, LOW, 1);
    matrix.write();
  }
}

