#include <M5Unified.h>
#include <WiFi.h>
#include <time.h>
#include <PubSubClient.h>
#include "cJSON.h"

#include "secret.h"

const int vibPin = 26;

WiFiClient espClient;
PubSubClient client(espClient);

String lastMessage = "";
String lastMessage2 = "";
String statusMessage = "";

uint16_t currentColor = YELLOW;

// Farbcodes
#define BACKGROUND_COLOR BLACK
#define TEXT_COLOR WHITE
#define WIFI_CONNECTED_COLOR GREEN
#define WIFI_DISCONNECTED_COLOR RED
#define MQTT_CONNECTED_COLOR GREEN
#define MQTT_DISCONNECTED_COLOR RED
#define ANIMATION_COLOR ORANGE
#define GROUP_COLOR YELLOW
#define MESSAGE_COLOR CYAN

// Vibrationsteuerung
bool vibrating = false;
unsigned long vibrationEndTime = 0;

void startVibration(unsigned long duration) {
  digitalWrite(vibPin, HIGH);
  vibrating = true;
  vibrationEndTime = millis() + duration;
}

void stopVibrationIfNeeded() {
  if (vibrating && millis() > vibrationEndTime) {
    digitalWrite(vibPin, LOW);
    vibrating = false;
  }
}

// Optionaler kurzer Vibrationseffekt bei MQTT-Reconnect o.Ä.
void signal(short v) {
  digitalWrite(vibPin, HIGH);
  delay(v);
  digitalWrite(vibPin, LOW);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("MQTT Topic: ");
  Serial.println(topic);
  Serial.print("MQTT Message: ");
  Serial.println(message);

  if (String(topic) == mqtt_topic) {
    lastMessage = message;
  } else if (String(topic) == mqtt_topic2) {
    const char* valuePtr = strstr(message.c_str(), "\"value\"");
    if (valuePtr) {
      char value[10];
      sscanf(valuePtr, "\"value\" : \"%[^\"]\"", value);
      lastMessage2 = String(value);
    }
  }
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
    signal(20);
  }
}

void drawWifiStatus(int x, int y) {
  M5.Display.setTextColor(WiFi.status() == WL_CONNECTED ? GREEN : RED);
  M5.Display.drawString(WiFi.status() == WL_CONNECTED ? "WiFi: OK" : "WiFi: X", x, y);
}

void drawMqttStatus(int x, int y) {
  M5.Display.setTextColor(client.connected() ? GREEN : RED);
  M5.Display.drawString(client.connected() ? "MQTT: OK" : "MQTT: X", x, y);
}

float wavePhase = 0.0;
void waveAnimation(int x, int baseY) {
  M5.Display.setTextColor(currentColor);
  M5.Display.setTextSize(1);

  int waveWidth = 60;
  int step = 6;
  for (int i = 0; i < waveWidth; i += step) {
    float yOffset = sin((i + wavePhase) * 0.2) * 5.0;
    M5.Display.drawString("~", x + i, baseY + (int)yOffset);
  }

  wavePhase += 1.5;
  if (wavePhase > 360) wavePhase = 0;
}

void displayStatus() {
  M5.Display.fillScreen(BACKGROUND_COLOR);
  M5.Display.setTextDatum(top_left);
  client.setKeepAlive(12);

  int y = 5;

  M5.Display.setTextColor(TEXT_COLOR);
  M5.Display.setTextSize(2);
  M5.Display.drawString("Beleg Gruppe 3", 5, y);
  y += 25;

  M5.Display.setTextSize(1);
  drawWifiStatus(5, y);
  y += 15;
  drawMqttStatus(5, y);
  y += 20;

  M5.Display.setTextColor(currentColor);
  M5.Display.setTextSize(2);
  M5.Display.drawString("Energie-Messbox", 5, y);
  y += 25;

  waveAnimation(5, y);
  y += 20;

  M5.Display.setTextSize(1);
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
  M5.Display.setTextColor(currentColor);
  M5.Display.drawString("Status: "+statusMessage, 5, y);
  y += 12;

  String MP1 = lastMessage == "false" ? "-" : "!";
  String MP2 = lastMessage2 == "false" ? "-" : "!";
  String statusLine = "ZONE: "+ MP1 + " WARN: " + MP2;
  int textWidth = M5.Display.textWidth(statusLine);
  int screenW = M5.Display.width();
  int screenH = M5.Display.height();

  M5.Display.setTextColor(TEXT_COLOR);
  M5.Display.drawString(statusLine, screenW - textWidth - 5, screenH - 15);
  M5.Display.drawString("Batterie: " + String(M5.Power.getBatteryLevel()) + "%", 5, screenH - 15);

  // Farb- und Vibrationslogik
  if (client.connected()) {
    if (lastMessage == "true" && lastMessage2 == "true") {
      currentColor = RED;
      startVibration(100); // Nicht blockierend
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

  M5.Display.setBrightness(80);
  M5.Display.setTextColor(TEXT_COLOR);
  M5.Display.setTextSize(1);
  M5.Display.setRotation(1);

  pinMode(vibPin, OUTPUT);
  digitalWrite(vibPin, LOW);

  WiFi.begin(ssid, password);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000;
unsigned long lastDisplayUpdate = 0;
const unsigned long displayInterval = 100;

void loop() {
  M5.update();
  client.loop();

  // MQTT reconnect
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > reconnectInterval) {
      lastReconnectAttempt = now;
      reconnect();
    }
  }

  // Display aktualisieren
  unsigned long now = millis();
  if (now - lastDisplayUpdate >= displayInterval) {
    lastDisplayUpdate = now;
    displayStatus();
  }

  // Vibration beenden falls nötig
  stopVibrationIfNeeded();
}
