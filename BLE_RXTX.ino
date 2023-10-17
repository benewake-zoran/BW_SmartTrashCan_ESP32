/*
   蓝牙相关初始化和服务函数   
   创建一个BLE服务器，一旦我们收到连接，将会周期性发送通知
   使用步骤：
   1. 创建一个 BLE Server
   2. 创建一个 BLE Service
   3. 创建一个 BLE Characteristic
   4. 创建一个 BLE Descriptor
   5. 开始服务
   6. 开始广播
*/


uint8_t txValue1[] = "Dist:";                         //后面需要发送的字符串strength
uint8_t txValue2[] = "Strength:";  
BLEServer *pServer = NULL;                   //BLEServer指针 pServer
BLECharacteristic *pTxCharacteristic = NULL; //BLECharacteristic指针 pTxCharacteristic
bool deviceConnected = false;                //本次连接状态
bool oldDeviceConnected = false;             //上次连接状态
//设置UUID
#define SERVICE_UUID "12a59900-17cc-11ec-9621-0242ac130002" // UART service UUID
#define CHARACTERISTIC_UUID_RX "12a59e0a-17cc-11ec-9621-0242ac130002"
#define CHARACTERISTIC_UUID_TX "12a5a148-17cc-11ec-9621-0242ac130002"

//根据已连接和未连接的情况改变标志位
class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

//处理接收到的消息
class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue(); //接收信息

        if (rxValue.length() > 0) //如果收到了信息
        {    
            Keydate = rxValue[0];                 
            switch(Keydate)
            {
              case 'A' : modez = 1;break;
              case 'B' : modez = 2;break;
              case 'C' : modez = 3;break;
              case 'D' : modez = 4;break;
              case 'E' : modez = 5;break;
              case 'G' : modez = 6;break;
              case 'K' : modez = 7;break;
            }
            if(Keydate == 'S')     //如果松开按键
            {
                KeyFlag = modez;   //把按键信号存入标志位
            }
    /**********************************************************/
            if(KeyFlag == 1)       //进入采样频率设置                                  
                 state = 2;
            
            if(KeyFlag == 2)       //进入上报频率设置                           
                 state = 3;

             if(KeyFlag == 3)       //进入上报频率设置      
             {                   
                 state = 1;
                 state1_KM= 0;
             }

            if(KeyFlag == 4)       // 进入频率设置     
            {
                 EEPROM.write(1, 0);delay(1);  
                 EEPROM.commit();delay(1);  //在写好所有的更改之后，保存更改的数据
                 EEPROM.write(40, 0);delay(1);  
                 EEPROM.write(20, 0);delay(1);  
                 EEPROM.commit();delay(1);  //在写好所有的更改之后，保存更改的数据
                 EEPROM.write(40, 0);delay(1);  
                 EEPROM.commit();delay(1);  //在写好所有的更改之后，保存更改的数据
                 EEPROM.write(60, 2);delay(1);  
                 EEPROM.commit();delay(1);  //在写好所有的更改之后，保存更改的数据
                 EEPROM.write(80, 2);delay(1);  
                 EEPROM.commit();delay(1);  //在写好所有的更改之后，保存更改的数据
                 SamplingDate = 2; EscalationDate = 2;
                 state = 1;
                 state1_KM= 0; //标记  是空模式还是满模式
            }
/*  按键处理函数，判断按键信号标志位。  摁下后分别实现什么操作，改变什么状态标志位  */
          //   if(KeyFlag == 1)    //摁下第一个键切换连续和单次状态
          //   {
          //      Continuous_Single =! Continuous_Single;
          //     if(Continuous_Single)  //发送切换指令给雷达
          //     {
          //       Serial.write(SetOne, 6);
          //     }
          //     else 
          //     {
          //       Serial.write(SetHundred, 6);
          //     }
          //       KeyFlag = 0 ; //清除标记位
          //   }

          //  if(KeyFlag == 2)  //按键2 摁下切换发送的是 距离还是信号强度
          //  { 
          //    Distance_Strength =! Distance_Strength;
          //    KeyFlag = 0 ;
          //    Lidar.receiveComplete = true  ;
          //  }

          // if(KeyFlag == 3)  //按键4 恢复出厂设置
          //  { 
          //    Serial.write(Restore, 4); //
          //    Serial.write(save, 4);
          //    KeyFlag = 0 ;
          //  }
          //  if(KeyFlag == 7)  //摁下发送指令，让雷达传入一帧数据
          //  { 
          //    Serial.write(GetData, 6);
          //    KeyFlag = 0 ;
          //  }
/********************************************************/
       }
  } 
};

void BLE_init()  //蓝牙初始化
{
    // 创建一个 BLE 设备
    BLEDevice::init("ESP32_BLE");   //蓝牙名称
    // 创建一个 BLE 服务
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks()); //设置回调
    BLEService *pService = pServer->createService(SERVICE_UUID);
    // 创建一个 BLE 特征 设定UUID等
    pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());
    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new MyCallbacks()); //设置回调

    pService->start();                  // 开始服务
    pServer->getAdvertising()->start(); // 开始广播
    Serial.println(" 等待一个客户端连接，且发送通知... ");
}
/*
  雷达广播服务
  用来发送需要发送的数据
// */
// void BLE_Serve() //
// {
//         // 已连接
//  if (deviceConnected)
//   {    
//         //将雷达的距离或者信号强度数据转化成字符型存入数组
//         int TempData = 0;
//         if(!Distance_Strength) //根据状态选择发送的数据内容
//            TempData = Lidar.strength;
//           else
//            TempData = Lidar.distance;
//         //数据处理 将整型数据按位转化为字符型存入数组  
//         int numCopy = TempData;
//         int digitCount = 0;
//          while (numCopy != 0) {
//             numCopy /= 10;
//             digitCount++;
//          }       
//           uint8_t digits[4] = {0}; // 用于存放每一位数字的数组
//           for (int i = 0; i < digitCount; i++) {
//             digits[i] = (char)(TempData % 10 + '0');
//             TempData /= 10;
//            }
//            if( digitCount == 0){
//               digits[0] = '0';
//               digitCount = 1;
//            }
//       //////////////////////////////////
//       if(Distance_Strength)  //发送文本内容
    
//        {
//         pTxCharacteristic->setValue("距离："); // 发送 "Dist:"  
//         pTxCharacteristic->notify();              // 广播
//        }
//        else
          
//        {
//         pTxCharacteristic->setValue("信号强度："); // 发送 "Strength:"  
//         pTxCharacteristic->notify();              
//        }
       
//        for (int i = digitCount - 1; i >= 0; i--) 
//        {
//         pTxCharacteristic->setValue(&digits[i], 1); // 发送 雷达距离数据
//         pTxCharacteristic->notify();             
//         Lidar.receiveComplete = false;
//        }   
//     }

//     //   断开连接
//     if (!deviceConnected && oldDeviceConnected)
//     {  
//         Serial.println(" 已断开连接 ");
//         delay(1000);                  // 留时间给蓝牙缓冲
//         pServer->startAdvertising(); // 重新广播
//         Serial.println(" 开始广播 ");
//         oldDeviceConnected = deviceConnected;
//     }

//     //  正在连接
//     if (deviceConnected && !oldDeviceConnected)
//     {
//         oldDeviceConnected = deviceConnected;
//     }
// }
