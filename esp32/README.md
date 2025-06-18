# Vibrations-Alarm bei Gefahrensituationen – M5StickC-Projekt

## Projektbeschreibung

Dieses Projekt verwendet einen **M5StickC** (oder ein ESP32-Board) mit einem Vibrationsmotor, um den Nutzer am Arm durch Vibration zu warnen, sobald eine Gefahrensituation im Druckluftsystem besteht **und** sich der Mitarbeiter im definierten Gefahrenbereich befindet. Die Daten werden über WLAN und MQTT empfangen.

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
   - `sm-repository/sm-repo/submodels/.../hasMalfunction/updated` (QoS 1): Information über erkannte Störungen
   - `personInArea` (QoS 2): Information darüber, ob sich die Person im Gefahrenbereich befindet

3. **Statusanzeige:**  
   - Aktueller Status wird auf dem Display angezeigt
   - Batteriestand, WLAN/MQTT-Status, animierte Wellenlinie
   - Verschiedene Farben für unterschiedliche Zustände

4. **Vibrations-Alarm:**  
   - Starke Vibration für 3 Sekunden, wenn beide Bedingungen erfüllt sind:
     - Eine Druckluftstörung wurde erkannt (`hasMalfunction` ist `true`)
     - Person befindet sich im Gefahrenbereich (`personInArea` ist `true`)
   - Kurze, leichte Vibration bei Verbindungsverlust als Rückmeldung
   - Vibration wird nicht-blockierend gesteuert und automatisch gestoppt

---

## Installation & Inbetriebnahme

1. **Projekt vorbereiten**
   - Laden Sie den ESP-Ordner herunter und öffnen Sie es in einer PlatformIO-Umgebung

2. **Zugangsdaten einrichten**
   - Benennen Sie die Datei `dummysecret.h` zu `secret.h` um
   - Passen Sie die Datei mit Ihren WLAN- und MQTT-Einstellungen an:

   ```cpp
   const char* ssid = "IhrWLANNetzwerk";
   const char* password = "IhrWLANPasswort";
   const char* mqtt_server = "192.168.x.x"; // IP-Adresse des MQTT-Brokers
   const int mqtt_port = 1883;              // Standard-MQTT-Port
   const char* mqtt_topic = "personInArea"; // Topic für Positionsdaten
   const char* mqtt_topic2 = "sm-repository/sm-repo/submodels/.../hasMalfunction/updated"; // Topic für Störungen
   ```

3. **Verbindung herstellen**
   - Verbinden Sie den ESP32 mittels USB-C-Kabel mit Ihrem PC
   - Laden Sie die Software auf den Chip

4. **Funktionstest**
   - Nach dem Start versucht der ESP32 automatisch, eine Verbindung herzustellen
   - Bei erfolgreicher Verbindung wechselt die Anzeige von Gelb zu Blau
   - Bei Verbindungsverlust erfolgt eine leichte Vibration als Feedback
   - Bei Gefahrensituation im überwachten Bereich vibriert der Handschuh stark

## Fehlerbehebung

- **ESP32 verbindet sich nicht:** Überprüfen Sie WLAN-Name und Passwort in der `secret.h`
- **Keine MQTT-Verbindung:** Überprüfen Sie die IP-Adresse des MQTT-Brokers und die Port-Einstellung
- **Keine Vibration bei Gefahr:** Stellen Sie sicher, dass beide Topics korrekt konfiguriert sind

## Hinweise

Der Handschuh ist ein wichtiger Teil des IIoT Gefahrenbereich-Warnsystems und arbeitet zusammen mit dem Docker-basierten Backend. Für die vollständige Systeminstallation beachten Sie bitte die Anweisungen in der Hauptdokumentation.

