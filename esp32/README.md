# Vibrations-Alarm bei Gefahrensituationen – M5StickC-Projekt

## Projektbeschreibung

Dieses Projekt verwendet einen **M5StickC** (oder ein ESP32-Board) mit einem Vibrationsmotor, um den Nutzer am Arm durch Vibration zu warnen, sobald eine Gefahrensituation besteht **und** sich eine Person im Gefahrenbereich befindet. Die Daten werden über WLAN und MQTT empfangen.

---

## Benötigte Bibliotheken

- [M5Unified](https://github.com/m5stack/M5Unified)
- `WiFi.h`
- `PubSubClient.h`
- `cJSON.h`
- **Eigenes** `secret.h` mit Zugangsdaten (siehe unten)

---

## Funktionsweise

1. **Verbindung:**  
   Das Gerät verbindet sich mit dem WLAN und dem MQTT-Broker.

2. **Datenempfang:**  
   Zwei MQTT-Topics werden abonniert:
   - Gefahreninformation (`lastMessage`)
   - Anwesenheit einer Person im Bereich (`lastMessage2`)

3. **Statusanzeige:**  
   - Aktueller Status wird auf dem Display angezeigt.
   - Anzeigen: Batteriestand, WLAN/MQTT-Status, animierte Wellenlinie.

4. **Vibrations-Alarm:**  
   - Nur wenn **beide Nachrichten** `"true"` sind (Gefahr und Person im Bereich), wird die Anzeige **rot** und der Vibrationsmotor läuft für 100 ms.
   - Die Vibration wird nicht-blockierend gesteuert und automatisch gestoppt.

---

## Installation & Inbetriebnahme

1. **Bibliotheken installieren**  
   Stelle sicher, dass alle oben genannten Bibliotheken installiert sind.

2. **`secret.h` anlegen**  
   Lege eine Datei `secret.h` mit folgendem Inhalt an:

   ```cpp
   const char* ssid = "DeinSSID";
   const char* password = "DeinWLANPasswort";
   const char* mqtt_server = "192.168.x.x";
   const int mqtt_port = 1883;
   const char* mqtt_topic = "dein/gefahren/topic";
   const char* mqtt_topic2 = "dein/person/topic";

