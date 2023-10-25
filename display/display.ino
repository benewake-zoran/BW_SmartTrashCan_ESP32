#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <Wire.h>               
#include "HT_SSD1306Wire.h"

SSD1306Wire  Display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst  oled初始化参数

// LoRa 配置参数
#define RF_FREQUENCY                                915000000 // Hz                工作频率
#define TX_OUTPUT_POWER                             14        // dBm               发射功率
#define LORA_BANDWIDTH                              0         // [0: 125 kHz,      带宽
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]       
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30        // 定义缓冲区大小
//////////////////////////////////////////////////////
char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
static RadioEvents_t RadioEvents;

int16_t rssi,rxSize;
bool lora_idle = true;

void setup() {
    Serial.begin(115200);
    Display.init(); //oled 初始化
    Display.setFont(ArialMT_Plain_16);//设置字体
    rssi=0;
    //LoRa 初始化
    Mcu.begin();    
    RadioEvents.RxDone = OnRxDone;  //接收函数
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
}


void loop()
{
  if(lora_idle)
  {
    lora_idle = false;
    Radio.Rx(0);     //进入Rx模式
  }
  Radio.IrqProcess( );

}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    rssi=rssi;
    rxSize=size;
    memcpy(rxpacket, payload, size );
    rxpacket[size]='\0';
    Radio.Sleep( );
    Display.clear();
    if(  strcmp(rxpacket, "RadarError") == 0  )
    {
      Serial.printf(" 未发现雷达! \"%s\" with rssi %d \n",rxpacket,rssi);
      Display.drawString(0, 0, rxpacket);  //X,Y,内容
    }
    else
    if(size<6)
    {
    Serial.printf(" 垃圾桶已使用 \"%s\" with rssi %d \n",rxpacket,rssi);
    Display.drawString(0, 0, "Used");  //X,Y,内容
    Display.drawString(45, 0, rxpacket);  //X,Y,内容
    }
    else
    {
      Display.drawString(0, 0, "Trash can is full !");  //X,Y,内容
      Serial.printf(" 垃圾桶已满  with rssi %d \n",rssi);
    }
    Display.display();  //将缓冲区写入内存
    lora_idle = true;//成功接包标志位
}
