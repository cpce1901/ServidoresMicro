#include <ESP32Time.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// Datos de conexion a red

char* ssid_casa = "Jessica2.4";
const char* password_casa = "167832873";

// Datos de nueva red

const char* ssid_riego = "Riego";
const char* password_riego = "cpce_1901";

// Configuracion servidor NTP

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600;
const int dayligthOffset_sec = 0;

// Definimos pin de salida de rele

#define rele 4

// Definimos pin de entrada de sensor

#define sensor 18

// Variables globales

int estadoLed;
int estadoSensor;

// Variables globales de tiempo

unsigned long Task_1 = 0;
unsigned long Task_2 = 0;
unsigned long Task_3 = 0;
unsigned long Task_4 = 0;

// Programas ON

String on_1 = "";
String on_2 = "";
String on_3 = "";
String on_4 = "";
String on_5 = "";

// Programas OFF

String off_1 = "";
String off_2 = "";
String off_3 = "";
String off_4 = "";
String off_5 = "";

// Objeto de reloj

ESP32Time rtc;

// Objetos websokets y server

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);  // the websocket uses port 81 (standard port for websockets


// Declaramos funcion para su uso posterior

void enviarDatosSensor();

// Funcion en caso de ser una pagina no encontrada

void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Pagina no encontrada");
}

void setup() {

  Serial.begin(115200);

  pinMode(rele, OUTPUT);
  pinMode(sensor, INPUT);

  digitalWrite(rele, HIGH);


  WiFi.mode(WIFI_MODE_APSTA);

  WiFi.softAP(ssid_riego, password_riego);

  WiFi.begin(ssid_casa, password_casa);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando a  WiFi..");
  }

  Serial.print("Conexion para riego: ");
  Serial.println(WiFi.softAPIP());

  Serial.print("Coenxion dentro de casa: ");
  Serial.println(WiFi.localIP());

  // Sincronizamos reloj internet con reloj local

  configTime(gmtOffset_sec, dayligthOffset_sec, ntpServer);
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);
  }

  if (!SPIFFS.begin(true)) {
    Serial.println("A ocurrido un error al montar SPIFFS");
    return;
  }

  listFilles();
  readJson();

  // Carga de html como texto
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // Carga de css como texto
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Carga de js como texto
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/script.js", "text/js");
  });

  // Carga de icono
  server.on("/cancel.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/cancel.png", "image/png");
  });

  server.onNotFound(notFound);
  server.begin();

  webSocket.begin();                  // start websocket
  webSocket.onEvent(webSocketEvent);  // define a callback function -> what does the ESP32 need to do when an event from the websocket is received? -> run function "webSocketEvent()"
}

void loop() {

  if (millis() - Task_1 > 100) {
    Task_1 = millis();
    out_on(on_1);
    out_on(on_2);
    out_on(on_3);
    out_on(on_4);
    out_on(on_5);
    out_off(off_1);
    out_off(off_2);
    out_off(off_3);
    out_off(off_4);
    out_off(off_5);
    //Serial.println(rtc.getTime());
  }

  if (millis() - Task_2 > 200) {
    Task_2 = millis();
    if (estadoLed == 1) {
      digitalWrite(rele, LOW);
    } else {
      digitalWrite(rele, HIGH);
    }
  }

  if (millis() - Task_3 > 1000) {
    Task_3 = millis();
    enviarDatosSensor();
  }

  if (millis() - Task_4 > 150) {
    Task_4 = millis();
    if (estadoSensor == 1) {
      int read_sensor = digitalRead(sensor);
      if (read_sensor == 1) {
        estadoLed = 1;
      } else {
        estadoLed = 0;
      }

    } else {
      Serial.println("Aun no esta habilitado el sensor de movimiento");
    }
  }

  webSocket.loop();
}

// Funcion encargada de sincronizar websoquets

void webSocketEvent(byte num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("Client " + String(num) + " disconnected");
      break;
    case WStype_CONNECTED:
      Serial.println("Client " + String(num) + " connected");
      break;
    case WStype_TEXT:
      // try to decipher the JSON string received
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      } else {

        const int estadoled = doc["led"];
        const int estadosensor = doc["sen"];
        const char* on1 = doc["on1"];
        const char* on2 = doc["on2"];
        const char* on3 = doc["on3"];
        const char* on4 = doc["on4"];
        const char* on5 = doc["on5"];

        const char* off1 = doc["off1"];
        const char* off2 = doc["off2"];
        const char* off3 = doc["off3"];
        const char* off4 = doc["off4"];
        const char* off5 = doc["off5"];


        Serial.println("led: " + String(estadoled));
        Serial.println("sensor: " + String(estadosensor));
        Serial.println("on1: " + String(on1));
        Serial.println("off1: " + String(off1));
        Serial.println("on2: " + String(on2));
        Serial.println("off2: " + String(off2));
        Serial.println("on3: " + String(on3));
        Serial.println("off3: " + String(off3));
        Serial.println("on4: " + String(on4));
        Serial.println("off4: " + String(off4));
        Serial.println("on5: " + String(on5));
        Serial.println("off5: " + String(off5));

        estadoLed = estadoled;
        estadoSensor = estadosensor;

        on_1 = String(on1);
        on_2 = String(on2);
        on_3 = String(on3);
        on_4 = String(on4);
        on_5 = String(on5);

        off_1 = String(off1);
        off_2 = String(off2);
        off_3 = String(off3);
        off_4 = String(off4);
        off_5 = String(off5);

        recDataJson();
      }
      Serial.println("");
      break;
  }
}

void enviarDatosSensor() {

  String jsonString = "";
  StaticJsonDocument<1024> doc;
  JsonObject object = doc.to<JsonObject>();
  object["day"] = rtc.getDay();
  object["month"] = rtc.getMonth();
  object["year"] = rtc.getYear();
  object["hrs"] = rtc.getHour(true);
  object["minute"] = rtc.getMinute();
  object["secon"] = rtc.getSecond();
  object["on_1"] = on_1;
  object["on_2"] = on_2;
  object["on_3"] = on_3;
  object["on_4"] = on_4;
  object["on_5"] = on_5;
  object["off_1"] = off_1;
  object["off_2"] = off_2;
  object["off_3"] = off_3;
  object["off_4"] = off_4;
  object["off_5"] = off_5;
  object["estateLed"] = estadoLed;
  object["estateSensor"] = estadoSensor;

  serializeJson(doc, jsonString);

  Serial.println(jsonString);

  webSocket.broadcastTXT(jsonString);
}

void out_on(String valor) {

  if (rtc.getTime() == valor) {
    estadoLed = 1;
    digitalWrite(rele, !estadoLed);
  }
}

void out_off(String valor) {

  if (rtc.getTime() == valor) {
    estadoLed = 0;
    digitalWrite(rele, !estadoLed);
  }
}

void listFilles() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file) {
    Serial.print("ARCHIVO: ");
    Serial.println(file.name());
    file = root.openNextFile();
  }
  root.close();
  file.close();
}

void readJson() {

  File file = SPIFFS.open("/datos.json");
  if (file) {
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, file);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    } else {
      const char* on1 = doc["on_1"];
      const char* on2 = doc["on_2"];
      const char* on3 = doc["on_3"];
      const char* on4 = doc["on_4"];
      const char* on5 = doc["on_5"];

      const char* off1 = doc["off_1"];
      const char* off2 = doc["off_2"];
      const char* off3 = doc["off_3"];
      const char* off4 = doc["off_4"];
      const char* off5 = doc["off_5"];

      Serial.println("on1: " + String(on1));
      Serial.println("off1: " + String(off1));
      Serial.println("on2: " + String(on2));
      Serial.println("off2: " + String(off2));
      Serial.println("on3: " + String(on3));
      Serial.println("off3: " + String(off3));
      Serial.println("on4: " + String(on4));
      Serial.println("off4: " + String(off4));
      Serial.println("on5: " + String(on5));
      Serial.println("off5: " + String(off5));

      on_1 = String(on1);
      on_2 = String(on2);
      on_3 = String(on3);
      on_4 = String(on4);
      on_5 = String(on5);

      off_1 = String(off1);
      off_2 = String(off2);
      off_3 = String(off3);
      off_4 = String(off4);
      off_5 = String(off5);
    }
    file.close();
  }
}

void recDataJson() {

  File outfile = SPIFFS.open("/datos.json", "w");
  StaticJsonDocument<512> object;
  object["on_1"] = on_1;
  object["on_2"] = on_2;
  object["on_3"] = on_3;
  object["on_4"] = on_4;
  object["on_5"] = on_5;
  object["off_1"] = off_1;
  object["off_2"] = off_2;
  object["off_3"] = off_3;
  object["off_4"] = off_4;
  object["off_5"] = off_5;
  if (serializeJson(object, outfile) == 0) {
    Serial.println("Error al grabar archivo");
  } else {
    Serial.println(outfile);
    Serial.println("Archivo Grabado");
  }
  outfile.close();
}
