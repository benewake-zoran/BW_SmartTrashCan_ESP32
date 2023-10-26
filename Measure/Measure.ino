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
#include "HardwareSerial.h"   
#include "LoRaWan_APP.h"
#include "LoRa.h"


hw_timer_t* Timer= NULL;              //定义存放定时器的指针  这个名字不能叫做timer,会和LoRa库里边冲突
uint8_t TIM = 0 , TIM1 = 1 ,TIM2 = 0; //定时器参数 用来给采样频率和上报频率做参数
void IRAM_ATTR onTimer();             //定时器中断回调
void Collect_Callback();              //串口中断回调

#define UART_FULL_THRESH_DEFAULT 2048         //修改缓冲区大小，这个是HardwareSerial.h文件中说的修改方法，我试了，并没有发挥作用
#define CJ_RxPin 44                           //设置RX管脚
#define CJ_TxPin 43                           //设置TX管脚
HardwareSerial Serial_CJ(0);                  //定向串口0


void setup() {
  Serial2.begin(115200, SERIAL_8N1, 45, 46);             //开启串口2 用作雷达数据读取
  Serial_CJ.begin(115200,SERIAL_8N1,CJ_RxPin,CJ_TxPin);  //初始化串口0，初始化参数可以去HardwareSerial.h文件中查看
	Serial_CJ.onReceive(Collect_Callback);                 //定义串口中断函数
  EEPROM.begin(4096);                                    //申请空间，传入参数为size，为需要读写的数据字节最大地址+1，取值1~4096；
  
  BLE_init();                                            //蓝牙初始化
  LoRaInit();
  
  /*TIM_init*/
  Timer = timerBegin(0, 80, true);             //设置定时器0，80分频，定时器是否上下计数
  timerAttachInterrupt(Timer, onTimer, true);  //定时器地址指针，中断函数名称，中断边沿触发类型
  timerAlarmWrite(Timer, 1000000, true);       //操作定时器，定时时长单位us，是否自动重装载 目前是1s
  timerAlarmEnable(Timer);                     //打开定时器
  
  if(EEPROM.read(1) == 0 )     //判断上电前是否已经存有系统的数据，有的话把数据读出来。 空/满状态值，检测/上报频率。
  {
    state = 1;
  }
  else
  {
   FullDate  = EEPROM.read(20) ;
   EmptyDate = EEPROM.read(40) ;
   if(EEPROM.read(60) != 0 && EEPROM.read(80) != 0 && EEPROM.read(100) != 0)
   {
    SamplingDate   = EEPROM.read(60) ;
    EscalationDate = EEPROM.read(80) ;
    WarningValue =  EEPROM.read(100) ;
   }
   state = 4;
  }

   getLidarData(&Lidar) ;
}

void loop() {
   //四种状态
  switch(state)
  {
    case 1 :EmptyAndFull(); break;     //标记 空或满
    case 2 :SetSampling();break;       //设置 采样间隔
    case 3 :SetEscalation();break;     //设置 上报间隔
    case 4 :Escalation();break;        //采样上报、预警
    case 5 :SetEarlyWarning();break;   //设置预警值 
  }

  
   SerialInterruptHandle();    //处理串口0中断传出的信息
   Errorback();//检测系统可能遇到的错误
}




//定时器中断函数
void IRAM_ATTR onTimer()
{
   TIM++;   //采样间隔参数
   TIM1++;  //上报间隔参数
   TIM2++;
   if(TIM>65536)//限制范围
   {
    TIM  = TIM%SamplingDate;
    TIM1 = TIM1%EscalationDate;
    TIM2 = 0;
   }
}



/*
           拟定的命令：
        1.设置当前状态为 空               5B 05 01 00 61
        2.设置当前状态为 满               5B 05 01 01 62
        3.恢复出厂设置                   5B 05 02 00 62
        4.设置检测频率                   5B 05 03  P  SU
        5.设置上报频率                   5B 05 04  P  SU
        
           P：数据    SU：检验和
*/
//串口0 中断回调函数 处理串口传进来的指令
void Collect_Callback(){               
  static char i = 0;
  char j = 0;
  int checksum = 0;   
   static int rx1[5] ;                 
  while(Serial_CJ.available() > 0){                 //用While判断缓冲区是否有内容
    rx1[i] = Serial_CJ.read();
    if (rx1[0] != 0x5B) {
      i = 0;
    }else if(i == 1 && rx1[1] != 0x05){
      i = 0;
    }else if(i == 4 ){
    for (j = 0; j < 4; j++) {
      checksum += rx1[j];      //计算校验和
    }
    if (rx1[4] == checksum) {       
        switch(rx1[2]) 
        {
          case 0x01 : if(rx1[3])                       
                        SerialInterrupt = 2;                      
                      else                      
                        SerialInterrupt = 1;
                     break;
          case 0x02 :SerialInterrupt = 3; break;
          case 0x03 :SerialInterrupt = 4; 
                      if(rx1[3]>=0x01) {
                        SamplingDate   = rx1[3] + 1;
                        EEPROM.write(60, SamplingDate);delay(1);  
                        EEPROM.commit();delay(1) ;
                        Serial_CJ.print("已成功更改");///////////////////////////////////////////////////////////////////////////////
                        }break;
          case 0x04 :SerialInterrupt = 5; 
                     if(rx1[3]>=0x01) {
                        EscalationDate   = rx1[3] + 1; 
                        EEPROM.write(80, EscalationDate);delay(1);  
                        EEPROM.commit();delay(1) ;
                        Serial_CJ.print("已成功更改");
                        }break;
        } 
      }
      i=0;
    }else{
      i++;
    }
  } 
}
