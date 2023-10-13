#ifndef __COM_H
#define __COM_H
/*
  |  存放全局变量，雷达数据、标志位等
*/

//状态机： 1连续检测距离  2单次检测距离  3连续检测强度  4单次检测强度  5恢复出厂设置 6读取版本号 

//雷达数据结构体
typedef struct {
  int distance = 0;
  int strength = 0;
  int ID   = 0;            //雷达 ID

  long int BaudRate = 0;

  boolean LidarFlag = false;
  boolean receiveComplete = false;
  boolean IO = 0;
} TF;
TF Lidar;

//获取版本号    5A 04 01 00
//单次触发指令  5A 04 04 00
//保存设置      5A 04 11 00
//恢复出厂设置  5A 04 10 00
//设置输出频率 0hz  [5A 06 03 00 00 00]     10Hz [5A 06 03 0A 00 00]   250Hz [5A 06 03 FA 00 00]

uint8_t GetVersion[] ={0x5A, 0x04, 0x01 ,0x00} ; //Serial.write(GetDatacmd, 5);
uint8_t GetData[]    ={0x5A, 0x04, 0x04 ,0x00} ;  
uint8_t save[]       ={0x5A, 0x04, 0x11 ,0x00} ; 
uint8_t Restore[]    ={0x5A, 0x04, 0x10 ,0x00} ; 
uint8_t SetOne[]     ={0x5A, 0x06, 0x03 ,0x00, 0x00, 0x00}  ;
uint8_t SetHundred[] ={0x5A, 0x06, 0x03 ,0x64, 0x00, 0x00}  ;




char    Keydate = '0';
uint8_t State = 1 ;   //状态编号
uint8_t KeyFlag = 0; //按键信号  KeyFlag
uint8_t modez = 1;    //暂时存储ModeFlag的状态，等待按键松开后发来的‘S’
uint8_t Continuous_Single = 0;  //储存 连续/单次 按键的状态
uint8_t Distance_Strength = 0;  //储存 距离/强度 按键的状态
#endif  
