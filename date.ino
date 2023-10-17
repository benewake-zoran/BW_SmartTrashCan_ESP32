/*
   存放雷达数据接收和解析函数   
*/

/* void getLidarData( TF* Lidar) 
   串口获取一帧数据，并计算距离，信号强度和记录成功计算的标志。
*/
void getLidarData( TF* Lidar)  
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
/* void Printdist(int date)
   功能：将一个多位的整型数据通过蓝牙发送出去
   date ：任意整型数据

*/
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

/* void EmptyAndFull( )
   功能：状态1标记垃圾桶的空和满
*/

void EmptyAndFull( ) //状态1标记空和满
{ 
  getLidarData(&Lidar) ;
   if(TIM%2 == 0) //采样频率
   {
      if(state1_KM == 0)
    {
    pTxCharacteristic->setValue("标记空状态"); 
    pTxCharacteristic->notify();
    Printdist(Lidar.distance);
    if(KeyFlag == 5)    //摁下确定键
    {
      KeyFlag = 0;
      getLidarData(&Lidar);
      EmptyDate = Lidar.distance;
      EEPROM.write(40, EmptyDate);delay(1);  
      EEPROM.commit();delay(1);  //更改垃圾桶空置状态的雷达数据
      state1_KM = 1;
    }
  }

  if(state1_KM == 1)
  {
     pTxCharacteristic->setValue("标记满状态 "); // 
     pTxCharacteristic->notify();
     Printdist(Lidar.distance);
     if(KeyFlag == 5)    //确定键
     {
      KeyFlag = 0;   
      getLidarData(&Lidar);
      FullDate = Lidar.distance; 
      EEPROM.write(20, FullDate) ;delay(1);  
      EEPROM.commit();delay(1);  //更改垃圾桶满溢状态的雷达数据
      EEPROM.write(1, 1) ;delay(1);  
      EEPROM.commit();delay(1);  //上电检测标志位，判断上电前有没有存有上次标记的数据
      state = 4;
     }       
   }  
   TIM++;
  } 
}


/* void SetSampling()
   功能：状态2设置采样间隔
*/
void SetSampling() 
{ 
    if(KeyFlag == 2) 
      KeyFlag = 0;
    if(TIM%2 == 0) //采样频率
   {
    pTxCharacteristic->setValue("设置 采样间隔: "); // 
    pTxCharacteristic->notify();
    Printdist(SamplingDate-1);
    pTxCharacteristic->setValue(" s"); // 
    pTxCharacteristic->notify();
     TIM++;
   }
   switch(KeyFlag)
   {
    case 6 : SamplingDate++;KeyFlag=0;break;
    case 7 : SamplingDate--;KeyFlag=0;break;
    case 5 : state = 4;;KeyFlag=0;
             EEPROM.write(60, SamplingDate);delay(1);  
             EEPROM.commit();delay(1) ;break;//保存采样间隔的数据
   }
   if(SamplingDate < 2)
    SamplingDate = 2;
}

/* void SetEscalation() 
   功能：状态3设置上报间隔
*/
void SetEscalation() 
{
   if(KeyFlag == 3) 
      KeyFlag = 0;
   if(TIM%2 == 0) //采样频率
   {
    pTxCharacteristic->setValue("设置 上报间隔: "); // 
    pTxCharacteristic->notify();
    Printdist(EscalationDate-1);
    pTxCharacteristic->setValue(" s"); // 
    pTxCharacteristic->notify();
     TIM++;
   }
   switch(KeyFlag)
   {
    case 6 : EscalationDate++;KeyFlag=0;break;
    case 7 : EscalationDate--;KeyFlag=0;break;
    case 5 : state = 4;;KeyFlag=0;
             EEPROM.write(80, EscalationDate);delay(1);  
             EEPROM.commit();delay(1);break;  //保存上报间隔的数据
   }
   if(EscalationDate < 2)
    EscalationDate = 2;   
}


/* void SetEscalation() 
   功能：状态4  检测状态 并上报和警报
*/
void Escalation() 
{
   float date;
   if(TIM%SamplingDate == 0) //采样频率
   {
       getLidarData(&Lidar);
       TIM++;
   }
  date = ( ( (float)EmptyDate - Lidar.distance ) / ( (float)EmptyDate - FullDate  ) )*100; //转换成百分比
  if(TIM1%EscalationDate == 0) //上报频率
  {
    if(date<80)
    {
       Printdist(date);
       pTxCharacteristic->setValue("%"); // 
       pTxCharacteristic->notify();      
     }
   else
   {
    pTxCharacteristic->setValue("垃圾桶已满!"); // 
    pTxCharacteristic->notify();    
   }
   TIM1++;
  }
}
