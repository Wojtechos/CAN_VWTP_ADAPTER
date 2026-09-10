# CAN_VWTP_ADAPTER
Adapter is meant for older VW group cars (platform PQ 25) with VW TP 1.6 CAN protocol enabling them to communicate with newer radios which use VW TP 2.0 (RCD 330/340 etc.). <br> 
This bridge was specifically built for **pre-facelift Skoda Fabia 2**, so the format of messages and their IDs may differ from car to car (VW golf, polo etc.). <br>

### Components
- **Radio** - RCD340G
- **Radio CANbus protocol** - VW TP 2.0 
- **Car** - Skoda Fabia mk2 (2008)
- **Car CANbus protocol** - VW TP 1.6 
<hr>
- **MCU** - ESP32C3 super mini
- **CAN module** - MCP2515

## System Architecture Overview
```mermaid
  flowchart LR
    Power(["12V"]) 
    Power --> Stepdown["Stepdown 12V to 5V"]
    Stepdown --> MCU["ESP32C3"]
    MCU --> LOGCONV1["Logic Converter"]
    MCU --> LOGCONV2["Logic Converter"]
    LOGCONV1 --> CANMOD1["CAN module"] 
    LOGCONV2 --> CANMOD2["CAN module"]
    CANMOD1 <-- ECU(["CAR ecu"]) 
    CANMOD2 --> RADIO(["CAR radio"])
```
