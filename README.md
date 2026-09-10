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
    subgraph CAN Adapter
      Stepdown["Stepdown 12V to 5V"]
      MCU["ESP32C3"]
      LOGCONV1["Logic Converter"]
      LOGCONV2["Logic Converter"]
      CANMOD1["CAN module"]
      CANMOD2["CAN module"] 
    end
    Power --> Stepdown
    Stepdown --> MCU
    MCU --> LOGCONV1
    MCU --> LOGCONV2
    LOGCONV1 --> CANMOD1 
    LOGCONV2 --> CANMOD2
    ECU(["CAR ecu"]) --> CANMOD1 
    CANMOD2 --> RADIO(["CAR radio"])
    LOGCONV1 --> MCU
    LOGCONV2 --> MCU
    CANMOD1 --> LOGCONV1
    CANMOD2 --> LOGCONV2
```
