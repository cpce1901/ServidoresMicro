#include <WiFi.h>
#include <ESP32Time.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Ticker.h>

// Datos de conexion a red

char* ssid_casa = "Jessica2.4";
const char* password_casa =  "167832873";

// Datos de nueva red

const char *ssid_riego = "Riego";
const char *password_riego = "cpce_1901";

// Configuracion servidor NTP

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600;
const int dayligthOffset_sec = 0;

// Objeto de reloj

ESP32Time rtc;

// Objeto timmer

Ticker timer;

// Objetos websokets y server

AsyncWebServer server(80);
WebSocketsServer websockets(81);

// Definimos pin de salida de rele

#define rele 2

// Declaramos funcion para su uso posterior

void enviarDatosSensor();

// Funcion en caso de ser una pagina no encontrada

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Página no encontrada");
}

// Funcion encargada de sincronizar websoquets

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type)
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] ¡Desconectado!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Conectado en %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Texto: %s\n", num, payload);
      String mensaje = String((char*)( payload));
      Serial.println(mensaje);

      DynamicJsonDocument doc(200); // documento (capacidad)
      DeserializationError error = deserializeJson(doc, mensaje);
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      int estadoLed = doc["Led"]; // el estado será de 0 o 1
      digitalWrite(rele, estadoLed);
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(rele, OUTPUT);

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

// Carga de html como texto
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
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

  server.onNotFound(notFound);
  server.begin();

  websockets.begin();
  websockets.onEvent(webSocketEvent);

  timer.attach(1, enviarDatosSensor);
}

void loop() {

  websockets.loop();

}

void enviarDatosSensor() {

  String JSON_Data = "{\"day\":";
  JSON_Data += rtc.getDay();
  JSON_Data += ",\"month\":";
  JSON_Data += rtc.getMonth();
  JSON_Data += ",\"year\":";
  JSON_Data += rtc.getYear();
  JSON_Data += ",\"hrs\":";
  JSON_Data += rtc.getHour(true);
  JSON_Data += ",\"minute\":";
  JSON_Data += rtc.getMinute();
  JSON_Data += ",\"secon\":";
  JSON_Data += rtc.getSecond();
  JSON_Data += "}";

  Serial.println(JSON_Data);
  websockets.broadcastTXT(JSON_Data);  // envia datos a todos los clientes conectados
}
