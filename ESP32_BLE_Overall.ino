#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <stdio.h>
#include <string.h>
#include "com.h"
#include "date.h"
#include "BLE_RXTX.h"


void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
BLE_init();

}

void loop() {
  // put your main code here, to run repeatedly:
  getLidarData(&Lidar);
  BLE_Serve();
  Serial.print("长度为: ");
  Serial.println(Lidar.distance);
}
