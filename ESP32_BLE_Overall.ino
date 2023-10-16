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
   getLidarData(&Lidar) ;
  switch(state)
  {
    case 1 :EmptyAndFull(); break;     //标记 空和满
    case 2 :;break;     //设置 采样间隔
    case 3 :;break;     //设置 上报间隔
    case 4 :Escalation();break;     //采样上报
    case 5 :;break;     //满时报警
    case 6 :;break;     //一键复位
    case 7 :;break;     //
  }

  //  if(Lidar.receiveComplete == false);   //读取一个雷达数据包，并计算出长度信号强度等)
  // {
  //   getLidarData(&Lidar) ;
  // }
  // if(Lidar.receiveComplete == true   )  
  // {
  //   BLE_Serve();            //雷达广播服务函数
  // }
   delay(700);                              // 延时 断帧
}
