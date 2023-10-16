#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <stdio.h>
#include <string.h>
#include <EEPROM.h>
#include "com.h"
#include "date.h"
#include "BLE_RXTX.h"

hw_timer_t* timer= NULL;  //定义存放定时器的指针
void IRAM_ATTR onTimer();

void setup() {
  Serial.begin(115200); //开启串口0
  EEPROM.begin(4096);    //申请空间，传入参数为size，为需要读写的数据字节最大地址+1，取值1~4096；
  BLE_init();           //蓝牙初始化
  timer = timerBegin(0, 80, true);  //设置定时器0，80分频，定时器是否上下计数
  timerAttachInterrupt(timer, onTimer, true);  //定时器地址指针，中断函数名称，中断边沿触发类型
  timerAlarmWrite(timer, 1000000, true);  //操作那个定时器，定时时长单位us，是否自动重装载 目前是1s
  timerAlarmEnable(timer);  //打开那个定时器
if(EEPROM.read(20) == 0 && EEPROM.read(40) == 0)
{
  Serial.print("00000");
  state = 1;
}
else
{
  Serial.print(EEPROM.read(20));
  Serial.print("    ");
  Serial.print(EEPROM.read(40));
  EmptyDate = EEPROM.read(40) ;
  FullDate  = EEPROM.read(20) ;
  state = 4;
}
}

void loop() {
   
  switch(state)
  {
    case 1 :EmptyAndFull(); break;     //标记 空和满
    case 2 :;break;     //设置 采样间隔
    case 3 :;break;     //设置 上报间隔
    case 4 :Escalation();break;     //采样上报
    case 5 :Earlywarning();break;     //满时报警
    case 6 :;break;     //一键复位
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


//定时器中断函数
void IRAM_ATTR onTimer()
{
  Serial.println("!!!!");
}
