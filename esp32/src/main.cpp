#include <M5Unified.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Pin-Definition für den Vibrationsmotor
const int vibPin = 26;
short aState = 0;

uint16_t currentColor = YELLOW;

void signal(short v) {
    digitalWrite(vibPin, HIGH);
    delay(v);
    digitalWrite(vibPin, LOW);
}

// CREDENTIALS
const char* ssid = "nein";
const char* password = "00001111";

// Open MQTT Broker
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;                     
const char* mqtt_topic = "test/topic";

WiFiClient espClient;
PubSubClient client(espClient);
String lastMessage = "";

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

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("MQTT: " + message);
  lastMessage = message;
  if (message == "true") {
    signal(100); // kurze Vibration
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Versuche MQTT-Verbindung...");
    String clientId = "M5StickClient-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("verbunden!");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Fehler, rc=");
      Serial.print(client.state());
      Serial.println(" Wiederhole in 5 Sekunden...");
      signal(100);
      delay(5000);
    }
  }
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

int check = 0;
void softSignal(){
  check++;
  if(check%10==0){
    signal(20);
  }
  if (!client.connected()&&WiFi.status() == WL_CONNECTED){
    currentColor = ORANGE;
    signal(20);
  } else {
    currentColor = CYAN;
  }
}


void animateConnection(int x, int y) {
  String r;
  switch(aState) {
    case 0: r = "[    ]"; break;
    case 1: r = "[=   ]"; break;
    case 2: r = "[==  ]"; break;
    case 3: r = "[=== ]"; break;
    case 4: r = "[ ===]"; break;
    case 5: r = "[  ==]"; break;
    case 6: r = "[   =]"; break;
    default: r = "[    ]"; aState = -1; break;
  }
  aState++;
  
  M5.Display.setTextColor(ANIMATION_COLOR);   
  M5.Display.drawString(r, x, y);
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
  M5.Display.fillScreen(BACKGROUND_COLOR);
  M5.Display.setTextDatum(top_left);
  softSignal();

  int y = 5;

  // Uhrzeit
  auto t = M5.Rtc.getTime();
  String timeStr = String(t.hours) + ":" + 
                   (t.minutes < 10 ? "0" : "") + String(t.minutes);
  M5.Display.setTextColor(TEXT_COLOR);
  M5.Display.setTextSize(2);
  M5.Display.drawString(timeStr, 5, y);
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

  // Laufende Animation
  //animateConnection(5, y);
  waveAnimation(5, y);
  y += 20;

  // Nachricht (beschränkt auf eine Höhe)
  if (lastMessage != "") {
    M5.Display.setTextColor(MESSAGE_COLOR);
    M5.Display.setTextSize(1);
    M5.Display.drawString("Nachricht:"+lastMessage, 5, y);
    y += 12;
    //M5.Display.drawString(lastMessage, 5, y);
    //y += 15;
  }

  // Batterieanzeige – ganz unten
  int batY = M5.Display.height() - 15;
  M5.Display.setTextColor(TEXT_COLOR);
  M5.Display.drawString("Batterie: " + String(M5.Power.getBatteryLevel()) + "%", 5, batY);
}

void setup() {
  M5.begin();
  
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

void loop() {
  M5.update();
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  displayStatus();
  
  delay(100);
}