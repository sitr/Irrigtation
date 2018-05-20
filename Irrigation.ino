#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
    
MDNSResponder mdns;

const char* ssid = "[Your network id]";
const char* password = "[Password to your wifi]";

ESP8266WebServer server(80);

const int NUMBER_OF_ZONES = 4;
// NB! These are the GPIO pins of the ESP8266, not the pin numbers on the WeMo D1
// Mapping available at https://github.com/esp8266/Arduino/blob/master/variants/d1_mini/pins_arduino.h#L49-L61
int zoneOutputPins[] = {14, 12, 13, 15}; //D5, D6, D7, D8
const char* homeSeerHost="http://[ip address to your homeseer installation]/JSON";

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
      sendStatusMessage(200, "Ok");
   });
   server.on("/water", handleValve);
   server.onNotFound(handleNotFound);

   server.begin();
   Serial.println("HTTP server started");
}

void handleNotFound() {
    sendStatusMessage(404, "Not found");
}

void sendStatusMessage(int httpCode, String messageText) {
    String status;
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject& statusMessage = jsonBuffer.createObject();
    statusMessage["status"] = messageText;
    statusMessage.prettyPrintTo(status);
    server.send(httpCode, "application/json", status);
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
    if(!isValidValveCommand(valveCommand) || !isValidValve(valveId)) {
        sendStatusMessage(400, "Not a valid command");
        return;
    }
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
}