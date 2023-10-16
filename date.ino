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

void Printdist(int date)
{
//将雷达的距离或者信号强度数据转化成字符型存入数组
        int TempData = 0;
        TempData = date;
        //数据处理 将整型数据按位转化为字符型存入数组  
        int numCopy = TempData;
        int digitCount = 0;
         while (numCopy != 0) {
            numCopy /= 10;
            digitCount++;
         }       
          uint8_t digits[4] = {0}; // 用于存放每一位数字的数组
          for (int i = 0; i < digitCount; i++) {
            digits[i] = (char)(TempData % 10 + '0');
            TempData /= 10;
           }
           if( digitCount == 0){
              digits[0] = '0';
              digitCount = 1;
           }
      //////////////////////////////////
       for (int i = digitCount - 1; i >= 0; i--) 
       {
         pTxCharacteristic->setValue(&digits[i], 1); // 发送 雷达距离数据
         pTxCharacteristic->notify();             
         Lidar.receiveComplete = false;
       }   
}



void EmptyAndFull( ) //状态1标记空和满
{ 
  if(state1_KM == 0)
  {
    pTxCharacteristic->setValue("标记空状态"); // 
    pTxCharacteristic->notify();
    Printdist(Lidar.distance);
    if(KeyFlag == 7)    //摁下第一个键切换连续和单次状态
    {
      KeyFlag = 0;
      getLidarData(&Lidar);
      EmptyDate = Lidar.distance;
      state1_KM = 1;
    }

  }
  if(state1_KM == 1)
  {
    pTxCharacteristic->setValue("标记满状态 "); // 
    pTxCharacteristic->notify();
    Printdist(Lidar.distance);
    if(KeyFlag == 7)    //摁下第一个键切换连续和单次状态
    {
      KeyFlag = 0;   
      getLidarData(&Lidar);
      FullDate = Lidar.distance; 
      state = 4;
    }       
  }   

}

void SetSampling() //状态2设置采样间隔
{ //if()
// {
    pTxCharacteristic->setValue("设置采样间隔："); // 
    pTxCharacteristic->notify();
//}

}

void SetEscalation() //状态3设置上报间隔
{
//if()
// {
    pTxCharacteristic->setValue("设置上报间隔："); // 
    pTxCharacteristic->notify();
//}
}

void Escalation() //状态4上报  
{
   float date;
   getLidarData(&Lidar);
   date = ( ( (float)EmptyDate - Lidar.distance ) / ( (float)EmptyDate - FullDate  ) )*100;
  //  Serial.print(EmptyDate);
  //  Serial.print("  ");
  //  Serial.print(FullDate);
  //  Serial.print("  ");
  //  Serial.print(Lidar.distance);
  //  Serial.println("  ");

   //if(0<date<100)
   Printdist(date);
   pTxCharacteristic->setValue("%"); // 
   pTxCharacteristic->notify();
}
void  Earlywarning() //状态5预警
{

}




















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
