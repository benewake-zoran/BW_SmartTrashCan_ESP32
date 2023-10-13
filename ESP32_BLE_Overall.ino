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
Serial.begin(115200); //开启串口0
BLE_init();           //蓝牙初始化
}

void loop() {
  

   if(Lidar.receiveComplete == false);   //读取一个雷达数据包，并计算出长度信号强度等)
  {
    getLidarData(&Lidar) ;
  }
  if(Lidar.receiveComplete == true   )  
  {
    BLE_Serve();            //雷达广播服务函数
  }
   delay(700);                              // 延时 断帧
}
