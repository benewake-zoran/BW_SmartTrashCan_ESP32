// #include <Arduino.h>
// #include "date.h"
// #include "main.h"
/*
   雷达数据接收和解析函数   
*/


void getLidarData( TF* Lidar)  //串口获取一帧数据，并计算距离，信号强度和记录成功计算的标志。
{ 
  //59 59 03 00 E9 09 68 09 18  一帧数据
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static int rx[9] ;//= {0x59 ,0x59 ,0x2F ,0x00 ,0xC9 ,0x0B ,0x70 ,0x09 ,0x2E};
   while (Serial.available() > 0) {
     rx[i] = Serial.read();
    if (rx[0] != 0x59) {
      i = 0;
    } else if (i == 1 && rx[1] != 0x59) {
      i = 0;
    } else if (i == 8) {
      for (j = 0; j < 8; j++) {
        checksum += rx[j];      //计算校验和
      }
      if (rx[8] == (checksum % 256)) {
        Lidar->distance = rx[2] + rx[3] * 256;  //距离
        Lidar->strength = rx[4] + rx[5] * 256;  //信号强度
        Lidar->receiveComplete = true;          //接收完成
      }
      i = 0;
    } else {
      i++;
    }
   } 
}



// void ContinuationDistance() //状态1
// { 
//   TempData = Lidar.distance;
// }

// void SingleDistance() //状态2
// {
// }

// void ContinuationStrength() //状态3
// {
// }

// void SingleStrength() //状态4
// {
// }


// void RestoreFactory() //状态5
// {
// }

// void GetVERSION() //状态6
// {
// }






















// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(115200);
//    // struct TF lidarData;
//     //getLidarData(&Lidar);
// }

// void loop() {
//   // put your main code here, to run repeatedly:
//   getLidarData(&Lidar);
//   Serial.print("长度为: ");
//   Serial.println(Lidar.distance);
// }






// void getLidarData( TF* Lidar)
// { 
//   //59 59 03 00 E9 09 68 09 18  一帧数据
//   static char i = 0;
//   char j = 0;
//   int checksum = 0;
//   static int rx[9] ;
//   while (Serial.available() > 0) {
//     rx[i] = Serial.read();
//     if (rx[0] != 0x59) {
//       i = 0;
//     } else if (i == 1 && rx[1] != 0x59) {
//       i = 0;
//     } else if (i == 8) {
//       for (j = 0; j < 8; j++) {
//         checksum += rx[j];      //计算校验和
//       }
//       if (rx[8] == (checksum % 256)) {
//         Lidar->distance = rx[2] + rx[3] * 256;  //距离
//         Lidar->strength = rx[4] + rx[5] * 256;  //信号强度
//         Lidar->receiveComplete = true;          //接收完成
//       }
//       i = 0;
//     } else {
//       i++;
//     }
//   }
// }
