#include "oled-wing-adafruit.h"
SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

OledWingAdafruit display;

bool connected = false;

const size_t UART_TX_BUF_SIZE = 20;

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);

void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) { // for receiving a message
  for (size_t i = 0; i < len; i++){
    Serial.write(data[i]);
  }
  if (data[0] == '1'){ // if entered 1 display on the oled display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("you enterd 1");
    display.display();
  }
  if (data[0] == '0'){ // if entered 0 disconnect
    BLE.disconnect();
  }
}
const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);

// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
  RGB.control(true);
  Serial.begin(9600);

  display.setup();
	display.clearDisplay();
	display.display();

  BLE.on();
  BLE.addCharacteristic(txCharacteristic);
  BLE.addCharacteristic(rxCharacteristic);

  BleAdvertisingData data;
  data.appendServiceUUID(serviceUuid);
  BLE.advertise(&data);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  if(BLE.connected()){ // if connected flash a blue light
    RGB.color(0, 0, 255);
  } else{ // if disconnected flash a yellow light
    RGB.color(255, 255, 0);
  }
  if(BLE.connected() && connected == false){ // if connected send a message
    delay(5000);
    uint8_t txBuf[30] = "connected!"; 
    txCharacteristic.setValue(txBuf, 30);
    connected = true;
  } 
  if(!BLE.connected()) {
    connected = false;
  }
  // The core of your code will likely live here.

}