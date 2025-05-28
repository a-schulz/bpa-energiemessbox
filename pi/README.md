## Node Red

### Voraussetzungen

Sie können die Dienste mit Docker Compose starten:

```bash
docker-compose up -d
```

Alternativ können Sie Node Red mit folgendem Befehl starten:

```bash
docker run -it -p 1880:1880 --name nodered --restart always -v node_red_data:/data nodered/node-red
```

### Node Red Pakete installieren

Hamburger-Menü -> Palette verwalten -> Installieren
- `node-red-contrib-opcua`
- optional, aber empfohlen: `node-red-dashboard`

### OPC UA

#### OPC UA Elemente hinzufügen

NodeID's:
- `ns=2;s=|var|CECC-LK.Application.GVL.rPressure`
- `ns=2;s=|var|CECC-LK.Application.GVL.rFlow`

### OPC UA Client hinzufügen

Wählen Sie Get oder Subscribe

Verbindungsdetails:
- URL: `opc.tcp://192.168.14.131:4840`

### REST / Websocket

Position über REST API abrufen:

tbd.


### Drools integrieren
tbd.
- Personenkoordinaten sowie aktuellen Druck und Durchfluss an Drools senden
- Auswertung erhalten, ob sich die Person in Gefahr befindet.

Definieren Sie in Drools die wichtigen Koordinaten für die Maschine (kann später im Prozess vom Benutzer geändert werden).
Definieren Sie Schwellenwerte für Druck und Durchfluss, um Störungen zu erkennen.

### Über MQTT veröffentlichen
_Hinweis: mqtt ist ein Standardpaket in Node Red_

#### Nachricht, die über MQTT gesendet werden soll
tbd.

#### MQTT Out Node hinzufügen

- MQTT Server URL `localhost:1883`.
  - siehe auch: [MQTT Broker](#mqtt-broker)
- Topic als tbd.

## MQTT Broker

Der MQTT Broker wird automatisch im Rahmen der Docker Compose Konfiguration gestartet. Falls Sie ihn separat ausführen müssen:

```bash
docker run -it -p 1883:1883 --name mosquitto --restart always -v mosquitto_data:/mosquitto/data eclipse-mosquitto
```

## AAS

Für die Grundkonfiguration der AAS siehe: https://basyx.org/get-started/introduction:
REST API Referenz: 
- https://wiki.basyx.org/en/latest/content/user_documentation/API/aas.html
- oder lokal: http://localhost:8081/swagger-ui/index.html

### Zugriff auf die BaSyx Container
- AAS Environment: [http://localhost:8081](http://localhost:8081)
- AAS Registry: [http://localhost:8082](http://localhost:8082)
- Submodel Registry: [http://localhost:8083](http://localhost:8083)

### Eigene Asset Administration Shells einbinden
Um eigene Asset Administration Shells einzubinden, können Sie diese entweder in den Ordner `aas` legen oder über die AAS Web-Benutzeroberfläche hochladen.

## Docker Compose Einrichtung

Im Projektstammverzeichnis ist eine Docker Compose-Datei vorhanden, um alle Dienste mit einem einzigen Befehl einfach zu starten:

```bash
docker-compose up -d
```

Dadurch werden gestartet:
- Node-RED auf Port 1880
- Mosquitto MQTT Broker auf Port 1883

Um alle Dienste zu stoppen:

```bash
docker-compose down
```

Um Protokolle anzuzeigen:

```bash
docker-compose logs -f
```
