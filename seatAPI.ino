#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <WebServer.h>

const int motorPins[4][2] = {{32, 14}, {15, 33}, {27, 12}, {22, 20}};

const char *SSID = "Elias";
const char *PWD = "elias12345678";
WebServer server(80);

void setup() {
	Serial.begin(115200);

  //------------
  //Connect WiFi
  //------------

  Serial.print("Connecting to ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
	
  //----
  //Pins
  //----

	for(int i = 0; i < 4; i++) {
    for (int j = 0; j < 2; j++) {
      pinMode(motorPins[i][j], OUTPUT);
    }
  }

  //---------------------
  //Setup RestAPI Routing
  //---------------------

  server.on("/api/scene/runwithcallback", HTTP_POST, handleRequest);
  server.begin();
}

void loop() {
  server.handleClient();
}


void handleRequest() {
  if (server.hasArg("plain") == false) {
    Serial.write("Invalid body!");
  } 

  DynamicJsonDocument doc(1024);
  String body = server.arg("plain");
  deserializeJson(doc, body.c_str());

  String input = doc["Input"];

  String id = input.substring(3, 5);

  String value = input.substring(6, value.length() - 1);

  String annotation = value.substring(0, 1);
  int valToMove = value.substring(1, value.length() - 1).toInt();

  if (id.equals("00")) {
    allMotorsOff();

  } else if (id.equals("01")) {
    if (annotation.equals("p")){
      motorForward(0, valToMove);
    } else {
      motorBackward(0, valToMove);
    }


  } else if (id.equals("02")) {
    if (annotation.equals("p")){
      motorForward(1, valToMove);
    } else {
      motorBackward(1, valToMove);
    }

  } else if (id.equals("03")) {
    if (annotation.equals("p")){
      motorBackward(2, valToMove);
    } else {
      motorForward(2, valToMove);
    }

  } else if (id.equals("04")) {
    if (annotation.equals("p")){
      motorForward(3, valToMove);
    } else {
      motorBackward(3, valToMove);
    }
  }

  String resp = "{\"Output\": \"OK-STATUS#BMWCS-HYP1: #0xC8-CONFIRMED " + input + "\"}";

  server.send(200, "application/json", resp.c_str());
}


/*
----------------------------------
Utility methods for motor controls
----------------------------------
*/

static void allMotorsOff() {
  Serial.write("\nAll motors off");
  for(int i = 0; i < 4; i++) {
    motorOff(i);
  }
}


static void motorOff(int id) {
  digitalWrite(motorPins[id][0], LOW);
  digitalWrite(motorPins[id][1], LOW);
}

static void motorForward(int id, int value) {
  digitalWrite(motorPins[id][0], HIGH);
  digitalWrite(motorPins[id][1], LOW);
}

static void motorBackward(int id, int value) {
  digitalWrite(motorPins[id][0], HIGH);
  digitalWrite(motorPins[id][1], HIGH);
}
