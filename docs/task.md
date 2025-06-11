## Belegaufgabe:
Entwicklung einer Applikation zur Warnung vor Gefahrenbereichen im IIoT Testbeds der HTWD Dresden

### Aufgabenstellung: 
Im IIoT Testbed der HTW Dresden soll eine Applikation entwickelt werden, welche einen Mitarbeitenden warnt, wenn dieser einen Bereich der Fertigung betritt, in welchem aktuell eine technische Störung vorliegt. Exemplarisch soll hierfür ein Druckabfall in der Druckluftversorgung der Anlage durch eine Leckage simuliert werden. Über eine Energiemessbox stehen aktuelle Informationen zum Druckluftsystem zur Verfügung. Es sollen verschiedene, individuell anpassbare Regeln für die Reaktion auf eine Gefährdung konfigurierbar sein. Modellieren Sie den Prozess als BPMN Modell und setzen Sie diesen mit den gegebenen technischen Vorgaben um.

### Technologische Vorgaben:
1. OPC-UA zur Abfrage und Bereitstellung der Statusinformationen zur Druckluftversorgung über eine Energiemessbox
2. MQTT für die flexible, lose Kopplung der einzelnen Systemkomponenten.
3. AAS für die Modellierung des Druckluftsystems als Asset (inkl. statischer und dynamischer Eigenschaften)
4. NodeRed für die Low-Code Entwicklung
5. Business Rule Management System (z.B. Drools) für die flexible Konfiguration der Geschäftsregeln zur Reaktion auf unterschiedliche Gefahrenlagen