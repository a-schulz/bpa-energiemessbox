#include <M5Unified.h>
#include <WiFi.h>
#include <time.h>
#include <PubSubClient.h>
#include "cJSON.h"

#include "secret.h"

// Pin-Definition für den Vibrationsmotor
const int vibPin = 26;
short aState = 0;

uint16_t currentColor = YELLOW;

void signal(short v) {
    digitalWrite(vibPin, HIGH);
    delay(v);
    digitalWrite(vibPin, LOW);
}

// Open MQTT Broker
const char* mqtt_server = "192.168.150.9";
const int mqtt_port = 1883;                     
const char* mqtt_topic = "personInArea"; ///
const char* mqtt_topic2 = "sm-repository/sm-repo/submodels/aHR0cHM6Ly9leGFtcGxlLmNvbS9pZHMvc20vNTQ2NV83MDAzXzQwNTJfMTg4Mw/submodelElements/hasMalfunction/updated";

WiFiClient espClient;
PubSubClient client(espClient);
String lastMessage = "";
String lastMessage2 = "";
String statusMessage = "";

// Farbdefinitionen
#define BACKGROUND_COLOR BLACK
#define TEXT_COLOR WHITE
#define WIFI_CONNECTED_COLOR GREEN
#define WIFI_DISCONNECTED_COLOR RED
#define MQTT_CONNECTED_COLOR GREEN
#define MQTT_DISCONNECTED_COLOR RED
#define ANIMATION_COLOR ORANGE
#define GROUP_COLOR YELLOW
#define MESSAGE_COLOR CYAN


bool personInArea = false;
bool hasMalfunction = false;

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("MQTT Topic: ");
  Serial.println(topic);
  Serial.print("MQTT Message: ");
  Serial.println(message);


  // Status speichern je nach Topic
  if (String(topic) == mqtt_topic) {
    //personInArea = (message == "true");
    lastMessage = message;
  } else if (String(topic) == mqtt_topic2) {
    const char *valuePtr = strstr(message.c_str(), "\"value\"");

    if (valuePtr) {
        char value[10]; // Platz für z. B. "true" oder "false"
        sscanf(valuePtr, "\"value\" : \"%[^\"]\"", value);
        printf("Wert von 'value': %s\n", value);
        lastMessage2 = String(value);
    } else {
        printf("'value' nicht gefunden.\n");
    }

  }

  // Nur wenn beide Bedingungen erfüllt sind
}


void reconnect() {
  if (!client.connected()) {
    Serial.print("Versuche MQTT-Verbindung...");
    String clientId = "M5StickClient-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(mqtt_topic);
      client.subscribe(mqtt_topic2);
    } else {
      Serial.print(client.state());
    }
  }
  signal(20);
}

void drawWifiStatus(int x, int y) {
  if (WiFi.status() == WL_CONNECTED) {
    M5.Display.setTextColor(GREEN);
    M5.Display.drawString("WiFi: OK", x, y);
  } else {
    M5.Display.setTextColor(RED);
    M5.Display.drawString("WiFi: X", x, y);
  }
}

void drawMqttStatus(int x, int y) {
  if (client.connected()) {
    M5.Display.setTextColor(GREEN);
    M5.Display.drawString("MQTT: OK", x, y);
  } else {
    M5.Display.setTextColor(RED);
    M5.Display.drawString("MQTT: X", x, y);
  }
}


float wavePhase = 0.0;
void waveAnimation(int x, int baseY) {
  M5.Display.setTextColor(currentColor);
  M5.Display.setTextSize(1);

  int waveWidth = 60; // Pixelbreite der Welle
  int step = 6;       // Abstand zwischen Punkten
  for (int i = 0; i < waveWidth; i += step) {
    float yOffset = sin((i + wavePhase) * 0.2) * 5.0; // Wellenhöhe
    M5.Display.drawString("~", x + i, baseY + (int)yOffset);
  }

  wavePhase += 1.5;
  if (wavePhase > 360) wavePhase = 0;
}

void displayStatus() {
  //Serial.println("Tick");
  M5.Display.fillScreen(BACKGROUND_COLOR);
  M5.Display.setTextDatum(top_left);
  client.setKeepAlive(12);  // in Sekunden


  int y = 5;

  // Uhrzeit
  M5.Display.setTextColor(TEXT_COLOR);
  M5.Display.setTextSize(2);
  M5.Display.drawString("Beleg Gruppe 3", 5, y);
  y += 25;

  // WLAN & MQTT Status (kleiner und untereinander)
  M5.Display.setTextSize(1);
  drawWifiStatus(5, y);
  y += 15;
  drawMqttStatus(5, y);
  y += 20;

  // Projektname
  M5.Display.setTextColor(currentColor);
  M5.Display.setTextSize(2);
  M5.Display.drawString("Energie-Messbox", 5, y);
  y += 25;

  waveAnimation(5, y);
  y += 20;

  // Nachricht (beschränkt auf eine Höhe)
  M5.Display.setTextColor(currentColor);
  switch(currentColor){
    case RED:
      statusMessage = "Warnung!";
      break;
    case CYAN:
      statusMessage = "Keine lokale Gefahrenlage.";
      break;
    default:
      statusMessage = "Keine Verbindung!";
      break;
  }

  M5.Display.setTextSize(1);
  M5.Display.drawString("Status: "+statusMessage, 5, y);
  y += 12;

  // Topic-Statusanzeige (unten rechts)
  String MP1 = lastMessage == "false" ? "-" : "!";
  String MP2 = lastMessage2 == "false" ? "-" : "!";

  //String statusLine = "PiA:" + personSymbol + "  hM:" + malfunctionSymbol;
  String statusLine = "ZONE: "+ MP1 + " WARN: " + MP2;
  int textWidth = M5.Display.textWidth(statusLine);
  int screenW = M5.Display.width();
  int screenH = M5.Display.height();

  M5.Display.setTextColor(TEXT_COLOR);
  M5.Display.setTextSize(1);
  M5.Display.drawString(statusLine, screenW - textWidth - 5, screenH - 15);

  // Batterieanzeige – ganz unten
  int batY = M5.Display.height() - 15;
  M5.Display.setTextColor(TEXT_COLOR);
  M5.Display.drawString("Batterie: " + String(M5.Power.getBatteryLevel()) + "%", 5, batY);
  if (client.connected()) {
    if (lastMessage == "true" && lastMessage2 == "true") {
      currentColor = RED;
      signal(300); // z.B. 150 ms Vibration
    } else {
      currentColor = CYAN;
    }
    } else {
      currentColor = ORANGE;
   } 
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  
  // Display Setup
  M5.Display.setBrightness(80);
  M5.Display.setTextColor(TEXT_COLOR);
  M5.Display.setTextSize(1);
  M5.Display.setRotation(1); // Wichtig: Seitwärts-Display

  // Vibrations-Pin
  pinMode(vibPin, OUTPUT);
  digitalWrite(vibPin, LOW);

  // WiFi verbinden
  
  WiFi.begin(ssid, password);

  // MQTT Setup
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000; // alle 5 Sekunden
unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 100; // alle 100 ms aktualisieren

void loop() {
  M5.update();

  client.loop();


  // Display-Update (nicht blockierend)
  unsigned long now = millis();
  if (now - lastDisplayUpdate >= displayInterval) {
    lastDisplayUpdate = now;
    displayStatus();
  }

  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      reconnect(); // kann blockieren
    }
  }
  if (lastMessage == "true" && lastMessage2 == "true") {
    signal(100);
  }
}