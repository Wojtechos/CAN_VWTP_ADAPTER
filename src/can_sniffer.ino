#include <SPI.h>
#include <mcp_can.h>
#include "driver/gpio.h"

#define CAN_INIT_RETRY_CNT 10

// designed for ESP32C3 super mini pin layout
#define SCK_PIN       4
#define MISO_PIN      5
#define MOSI_PIN      6
#define CS_ECU_PIN    7
#define CS_RADIO_PIN  10
#define ECU_INT_PIN   0

MCP_CAN CAN_ECU(CS_ECU_PIN);
MCP_CAN CAN_RADIO(CS_RADIO_PIN);

volatile bool msg_incoming = false;

void IRAM_ATTR interrupt_handler() {
  msg_incoming = true;
}

bool init_can_module(MCP_CAN *can, unsigned int tries, unsigned int mcp_id) {
  // Initializes connection between MCU and given CAN module.
  // Params:
  //    &can - (MCP)CAN module 
  //    tries - number of retries
  //    mcp_id - id of can module for distinguishment in logs
  // Returns true if initializtion completed successfully, otherwise false is returned. 

  while (tries > 0) {
    if (can->begin(MCP_ANY, CAN_100KBPS, MCP_8MHZ) == CAN_OK) {
      Serial.println("MCP2515 " + String(mcp_id) + " initialised!");
      return true;
    } else {
      Serial.print("MCP2515 " + String(mcp_id) + " not found. Tries left: ");
      Serial.println(tries - 1);
      delay(500);
      tries--;
    }
  }

  return false; 
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  gpio_set_drive_capability((gpio_num_t)SCK_PIN, GPIO_DRIVE_CAP_3);
  gpio_set_drive_capability((gpio_num_t)MOSI_PIN, GPIO_DRIVE_CAP_3);

  pinMode(ECU_INT_PIN, INPUT_PULLUP);
  pinMode(CS_ECU_PIN, OUTPUT);
  digitalWrite(CS_ECU_PIN, HIGH);
  pinMode(CS_RADIO_PIN, OUTPUT);
  digitalWrite(CS_RADIO_PIN, HIGH);

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);
  
  Serial.println("Starting setup");
  bool can_ecu_ok = init_can_module(&CAN_ECU, CAN_INIT_RETRY_CNT, 1);
  bool can_radio_ok = init_can_module(&CAN_RADIO, CAN_INIT_RETRY_CNT, 2);

  if (!can_radio_ok || !can_ecu_ok) {
    Serial.println("ERROR: Could not connect to one or both CAN modules. Halting.");
    while (1) {
      delay(100); 
    }
  }

  CAN_RADIO.setMode(MCP_NORMAL);
  CAN_ECU.setMode(MCP_LISTENONLY);

  attachInterrupt(digitalPinToInterrupt(ECU_INT_PIN), interrupt_handler, FALLING);
  
  Serial.println("Setup complete. Waiting for message.");
}

void loop() {
  if (msg_incoming) {
    msg_incoming = false;

    while (CAN_ECU.checkReceive() == CAN_MSGAVAIL) {
    
      long unsigned int rx_id;
      unsigned char len = 0;
      unsigned char rx_buf[16];

      if (CAN_ECU.readMsgBuf(&rx_id, &len, rx_buf) == CAN_OK) {
            if (rx_id == 0x271) {   
          Serial.println("Sending msg");
          CAN_RADIO.sendMsgBuf(0x575, 0, len, rx_buf);
        }
        else { 
          byte is_ext = (rx_id & 0x80000000) ? 1 : 0;
          Serial.println("Sending msg");
          CAN_RADIO.sendMsgBuf(rx_id, is_ext, len, rx_buf);
        }
      }
    }
  }
}
