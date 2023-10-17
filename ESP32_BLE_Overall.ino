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
uint8_t TIM = 0 , TIM1 = 0 ; //定时器参数 用来给采样频率和上报频率做参数
void IRAM_ATTR onTimer();

void setup() {
  Serial.begin(115200); //开启串口0
  EEPROM.begin(4096);    //申请空间，传入参数为size，为需要读写的数据字节最大地址+1，取值1~4096；
  BLE_init();           //蓝牙初始化
  /*TIM_init*/
  timer = timerBegin(0, 80, true);  //设置定时器0，80分频，定时器是否上下计数
  timerAttachInterrupt(timer, onTimer, true);  //定时器地址指针，中断函数名称，中断边沿触发类型
  timerAlarmWrite(timer, 1000000, true);  //操作定时器，定时时长单位us，是否自动重装载 目前是1s
  timerAlarmEnable(timer);  //打开定时器
  
  if(EEPROM.read(1) == 0 )//判断上电前是否已经存有系统的数据  空/满状态值，检测/上报频率。
  {
    state = 1;
  }
  else
  {
   FullDate  = EEPROM.read(20) ;
   EmptyDate = EEPROM.read(40) ;
   if(EEPROM.read(60) != 0 && EEPROM.read(40) != 0)
   {
    SamplingDate   = EEPROM.read(60) ;
    EscalationDate = EEPROM.read(80) ;
   }
   state = 4;

  }
}

void loop() {
   
  switch(state)
  {
    case 1 :EmptyAndFull(); break;     //标记 空和满
    case 2 :SetSampling();break;       //设置 采样间隔
    case 3 :SetEscalation();break;     //设置 上报间隔
    case 4 :Escalation();break;        //采样上报、预警
  }

}


//定时器中断函数
void IRAM_ATTR onTimer()
{
   TIM++;   //采样间隔参数
   TIM1++;  //上报间隔参数
   if(TIM>65536)
   {
    TIM  = TIM%SamplingDate;
    TIM1 = TIM1%EscalationDate;
   }
}
