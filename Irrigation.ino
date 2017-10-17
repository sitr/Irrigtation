#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <TimerObject.h>
    
MDNSResponder mdns;

const char* ssid = "satin2_2_4_GHz";
const char* password = "Henrik123";

ESP8266WebServer server(80);
HTTPClient http;

const int NUMBER_OF_ZONES = 4;
int zoneOutputPins[] = {0, 1, 3, 4};
const char* host="http://192.168.0.109/JSON";


TimerObject *timer1 = new TimerObject(1000 * 60 * 10);

void setup(void) {
   resetPins();
   delay(1000);

   Serial.begin(115200);
   WiFi.begin(ssid, password);

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

   //Routes
   server.on("/", [](){
      sendStatusMessage(200, "System status: Ok");
   });

   server.on("/water", handleValve);
   server.onNotFound(handleNotFound);

   server.begin();
   Serial.println("HTTP server started");
   sendPing();
   timer1->setOnTimer(&sendPing);
   timer1->Start();
}

void handleNotFound() {
    sendStatusMessage(400, "Could not find page");
}

void sendStatusMessage(int httpCode, String messageText) {
    String status;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& statusMessage = jsonBuffer.createObject();
    statusMessage["status"] = messageText;
    statusMessage.prettyPrintTo(status);
    server.send(httpCode, "application/json", status);
}

void sendPing() {
    http.begin(host);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST("{'action' : 'controlbyvalue', 'deviceref' : '277', 'value' : '100'}");
    if(httpCode > 0) {
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        }
    }
    else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
}

bool isValidValve(String valve) {
    String validValves[] = {"1", "2", "3", "4"};
    for (int i = 0; i <= 3; i++) {
        if (validValves[i] == valve)
            return true;
    }
    return false;
}

bool isValidValveCommand(String command) {
    if(command == "0" || command == "1")
        return true;
    return false;
}

void resetPins() {
    for(int i = 0; i <= NUMBER_OF_ZONES; i++) {
        pinMode(zoneOutputPins[i], OUTPUT);
        digitalWrite(zoneOutputPins[i], LOW);
    }
}

void handleValve() {
    String data = server.arg("plain");
    Serial.print(data);
    StaticJsonBuffer<200> jBuffer;
    JsonObject& jObject = jBuffer.parseObject(data);

    String valveId = jObject["valveId"];
    String valveCommand = jObject["command"];
    if(!isValidValveCommand(valveCommand) || !isValidValve(valveId))
        sendStatusMessage(404, "This is not a valid command");
    resetPins();
    digitalWrite(zoneOutputPins[valveId.toInt() - 1], valveCommand.toInt());
    Serial.println("");
    Serial.println("Nummer: " + valveId);
    Serial.println("Kommando: " + valveCommand);

    String status;
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& valve = root.createNestedObject("valve");
    valve["number"] = valveId;
    valve["command"] = valveCommand;
    root.prettyPrintTo(status);
    server.send(200, "application/json", status);
}
 
void loop(void) {
    server.handleClient();
    timer1->Update();
}