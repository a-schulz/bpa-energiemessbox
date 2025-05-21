#include <M5Unified.h>
#include <WiFi.h>
#include <time.h>
#include <PubSubClient.h>
#include "cJSON.h"

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
//const char* ssid = "IIoT-Students";
//const char* password = "b6H948!%^%$9$P";

const char* ssid = "nein";
const char* password = "00001111";

// Open MQTT Broker
const char* mqtt_server = "192.168.143.84";
const int mqtt_port = 1883;                     
const char* mqtt_topic = "personInArea"; ///
const char* mqtt_topic2 = "sm-repository/sm-repo/submodels/aHR0cHM6Ly9leGFtcGxlLmNvbS9pZHMvc20vNTQ2NV83MDAzXzQwNTJfMTg4Mw/submodelElements/failure/updated";

WiFiClient espClient;
PubSubClient client(espClient);
String lastMessage = "";
String lastMessage2 = "";

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
    //hasMalfunction = (message == "true");
    //String message = "..";
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
  if (!client.connected()){
    currentColor = ORANGE;
    if(check%30==0){
      reconnect();
      signal(20);
      check = 0;
    }
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
  softSignal();
  client.setKeepAlive(12);  // in Sekunden


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

  // Topic-Statusanzeige (unten rechts)
  //String personSymbol = personInArea ? "O" : "X";
  //String malfunctionSymbol = hasMalfunction ? "O" : "X";

  //String statusLine = "PiA:" + personSymbol + "  hM:" + malfunctionSymbol;
  String statusLine = lastMessage + "/" + lastMessage2;
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
  if (lastMessage == "true" && lastMessage2 == "true") {
    currentColor = RED;
    signal(300); // z.B. 150 ms Vibration
    signal(300);
    signal(300);
  } else {
    currentColor = CYAN;
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

  // ✅ Zeitzone setzen (Berlin / Mitteleuropa inkl. Sommerzeit)
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();

  // ✅ Zeit via NTP holen
  configTime(0, 0, "pool.ntp.org");

  // ⏰ RTC auf aktuelle Zeit setzen
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    M5.Rtc.setDateTime(timeinfo);
  }


  // MQTT Setup
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  M5.update();

  client.loop();
  
  displayStatus();
  
  delay(100);
}