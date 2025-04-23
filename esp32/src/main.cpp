#include <M5Unified.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Pin-Definition für den Vibrationsmotor
const int vibPin = 26;
short aState = 0;
void signal(short v){
    digitalWrite(vibPin, HIGH);
    delay(v);
    digitalWrite(vibPin, LOW);
}
//CREDENTIALS
const char* ssid = "nein";
const char* password = "00001111";

//Open MQTT Broker
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "/messbox/status";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Nachricht empfangen [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Zeige die Nachricht auf dem Display an
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(GREEN);
  M5.Display.println("MQTT-Nachricht:");
  M5.Display.println(message);
  delay(1000);
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
      delay(5000);
    }
  }
}

void animate(){
  String r;
  switch(aState){
    case 0:
      r = "Oooo";
      aState++;
      break;
    case 1:
      r = "oOoo";
      aState++;
      break;
    case 2:
      r = "ooOo";
      aState++;
      break;
    case 3:
      r = "oooO";
      aState=0;
  }
  M5.Display.setTextColor(BLUE);   
  M5.Display.println(r);           
}

void wifiSet(){
  if(WiFi.status() != WL_CONNECTED){
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED) {
      M5.Display.fillScreen(BLACK);
      M5.Display.setCursor(0, 0);

      animate();

      M5.Display.setTextColor(RED); 
      M5.Display.println("WIFI");

      M5.Display.setTextColor(WHITE); 
      M5.Display.println("\nVerbinde..");
      M5.Display.println("\nVersuch:\n"+ String(attempt));
      attempt++;
      delay(500);
    }             
  } else {
    animate();
    M5.Display.setTextColor(GREEN);  
    M5.Display.println("WIFI");
  }
}


void setup() {
  M5.begin();

  // Setup für Display
  M5.Display.setBrightness(80);
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(0, 0);

  // Setup für Vibrations-Pin
  pinMode(vibPin, OUTPUT);
  digitalWrite(vibPin, LOW);  // initial aus

  WiFi.begin(ssid, password);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  M5.Display.fillScreen(BLACK);

  auto timestamp = M5.Rtc.getTime();

  String status;  
  M5.Display.setCursor(0, 0);
  wifiSet();

  if (!client.connected()) {
    status = "Fehler";
    reconnect();
  } else {
    status = "Verbunden";
  }
  client.loop();


  M5.Display.setTextColor(WHITE);              
  M5.Display.println("\nGruppe:\n\nEnergie-\nMessbox\n\nStatus:\n"+status);

  // Vibration für 500ms einschalten
  //signal(300);

  //M5.Display.printf("Looped. Time: %02d:%02d:%02d\n", 
  //  timestamp.hours,
  //  timestamp.minutes,
  //  timestamp.seconds);
  
  delay(500);
  M5.update();
}
