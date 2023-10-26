#ifndef __COM_H
#define __COM_H
/*
  |  存放全局变量，雷达数据、标志位等
*/

//雷达数据结构体
typedef struct {
  int distance = 0;        //距离
  int strength = 0;        //信号强度
  int ID   = 0;            //雷达 ID

  long int BaudRate = 0;

  boolean LidarFlag = false;
  boolean receiveComplete = false;
  boolean IO = 0;
} TF;
TF Lidar;


char    Keydate = '0';
int  EmptyDate = 0, FullDate = 0, SamplingDate = 2, EscalationDate = 2 ,WarningValue = 80;//  空\满状态的雷达数据、采样间隔、上报间隔 单位秒 (n-1)S,最小为2 对应一秒
uint8_t state = 1 ;   //状态编号
uint8_t KeyFlag = 0,SerialInterrupt = 0; //按键信号  KeyFlag
uint8_t modez = 1,state1_KM=0,Errornum = 0;    //暂时存储KeyFlag的状态，等待按键松开后发来的‘S’
float TrashData = 0; //储存垃圾桶已使用的百分比
#endif  



//获取版本号    5A 04 01 00
//单次触发指令  5A 04 04 00
//保存设置      5A 04 11 00
//恢复出厂设置  5A 04 10 00
//设置输出频率 0hz  [5A 06 03 00 00 00]     10Hz [5A 06 03 0A 00 00]   250Hz [5A 06 03 FA 00 00]

// uint8_t GetVersion[] ={0x5A, 0x04, 0x01 ,0x00} ; //Serial.write(GetDatacmd, 5);
// uint8_t GetData[]    ={0x5A, 0x04, 0x04 ,0x00} ;  
// uint8_t save[]       ={0x5A, 0x04, 0x11 ,0x00} ; 
// uint8_t Restore[]    ={0x5A, 0x04, 0x10 ,0x00} ; 
// uint8_t SetOne[]     ={0x5A, 0x06, 0x03 ,0x00, 0x00, 0x00}  ;
// uint8_t SetHundred[] ={0x5A, 0x06, 0x03 ,0x64, 0x00, 0x00}  ;
