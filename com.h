#ifndef __COM_H
#define __COM_H


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

#endif  
