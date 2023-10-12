/*
   创建一个BLE服务器，一旦我们收到连接，将会周期性发送通知
   使用步骤：
   1. 创建一个 BLE Server
   2. 创建一个 BLE Service
   3. 创建一个 BLE Characteristic
   4. 创建一个 BLE Descriptor
   5. 开始服务
   6. 开始广播
*/


uint8_t txValue[] = "Dist:";                         //后面需要发送的值
BLEServer *pServer = NULL;                   //BLEServer指针 pServer
BLECharacteristic *pTxCharacteristic = NULL; //BLECharacteristic指针 pTxCharacteristic
bool deviceConnected = false;                //本次连接状态
bool oldDeviceConnected = false;             //上次连接状态d

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

        if (rxValue.length() > 0)
        { //向串口输出收到的值
            Serial.println("*********");
            Serial.print("Received Value: ");
            for (int i = 0; i < rxValue.length(); i++)
              Serial.print(rxValue[i]);
            Serial.println();
            Serial.println("*********");
        }
    }
};

void BLE_init()
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

void BLE_Serve()
{
        // 已连接
    if (deviceConnected)
    {  
      //  for(int i=0 ;i<5;i++)
      //  {
        int TempData= Lidar.distance;
        int numCopy = TempData;
        int digitCount = 0;
         while (numCopy != 0) {
            numCopy /= 10;
            digitCount++;
         }       
          uint8_t digits[3] = {0}; // 用于存放每一位数字的数组
          for (int i = 0; i < digitCount; i++) {
            digits[i] = (char)(TempData % 10 + '0');
            TempData /= 10;
           }
          // Serial.print("长度000为: ");
       for (int i = digitCount - 1; i >= 0; i--) 
       {
        pTxCharacteristic->setValue(&digits[i], 1); // 设置要发送的值为1
        pTxCharacteristic->notify();              // 广播
        //txValue++;                                // 数值自加1
         
          //Serial.print(digits[i]);
        delay(1);
       }
        delay(200);                              // 如果有太多包要发送，蓝牙会堵塞
        //Serial.println(" ??????\n ");
    }
    //   断开连接
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // 留时间给蓝牙缓冲
        pServer->startAdvertising(); // 重新广播
        Serial.println(" 开始广播 ");
        oldDeviceConnected = deviceConnected;
    }
    //  正在连接
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
