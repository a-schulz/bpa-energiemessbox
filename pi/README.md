## Node Red

### Prerequisites

You can start the services using Docker Compose:

```bash
docker-compose up -d
```

Alternatively, start Node Red with the following command:

```bash
docker run -it -p 1880:1880 --name nodered --restart always -v node_red_data:/data nodered/node-red
```

### Install Node Red Packages

Hamburger menu -> Manage Palette -> Install
- `node-red-contrib-opcua`
- optional but recommended: `node-red-dashboard`

### OPC UA

#### Add OPC UA Items

NodeID's:
- `ns=2;s=|var|CECC-LK.Application.GVL.rPressure`
- `ns=2;s=|var|CECC-LK.Application.GVL.rFlow`

### Add OPC UA Client

Choose Get or Subscribe

Connection details:
- URL: `opc.tcp://192.168.14.131:4840`

### REST / Websocket

Get position using REST API:

tbd.


### Integrate Drools
tbd.
- send person coordinates and current pressure and flow to Drools
- receive evaluation if person is in danger.

Define in drools the important coordinates for the machine (can be changed by the user in later process).
Define threshold values for pressure and flow to detect malfunction.

### Publish to MQTT
_Note: mqtt is a default package in node red_

#### Message to be send via MQTT
- `timestamp` of the warning start

#### Add MQTT Out Node

- MQTT Server URL `localhost:1883`.
  - see also: [MQTT Broker](#mqtt-broker)
- Define topic as `x-fab/security/warning`.

## MQTT Broker

The MQTT broker is automatically started as part of the Docker Compose configuration. If you need to run it separately:

```bash
docker run -it -p 1883:1883 --name mosquitto --restart always -v mosquitto_data:/mosquitto/data eclipse-mosquitto
```

## AAS

To get the Basic Setup for the AAS see: https://basyx.org/get-started/introduction:


### Access the BaSyx containers
- AAS Environment: [http://localhost:8081](http://localhost:8081)
- AAS Registry: [http://localhost:8082](http://localhost:8082)
- Submodel Registry: [http://localhost:8083](http://localhost:8083)

### Include your own Asset Administration Shells
To include your own Asset Administration Shells, you can either put them in the `aas` folder or upload them via the AAS Web UI.

## Docker Compose Setup

A Docker Compose file is provided in the project root directory to easily start all services with a single command:

```bash
docker-compose up -d
```

This will start:
- Node-RED on port 1880
- Mosquitto MQTT broker on port 1883

To stop all services:

```bash
docker-compose down
```

To view logs:

```bash
docker-compose logs -f
```
`
