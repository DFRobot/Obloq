#include <SoftwareSerial.h>
#include "Obloq.h"

//MQTT连接相关参数
const String ssid = "DFRobot-guest";
const String password = "dfrobot@2017";
const String iot_id = "r1qHJFJ4Z";
const String iot_pwd = "SylqH1Y1VZ";
const String topic = "SyZ6l-mBG";

 //led小灯引脚
int ledPin = 13;
bool sendPingFlag = true; 
bool subscribeFlag = true; 
unsigned long currentTime = 0;                          

SoftwareSerial softSerial(10, 11);         // RX, TX
Obloq olq(softSerial, ssid, password);

void handleRaw(String& data)
{
    Serial.println(data);   //串口打印返回的数据
}

void subscribeMessageHandle(const String topicStr,const String message)
{
    if(topicStr == topic)
    {
        if(message == "1")
          digitalWrite(ledPin,HIGH);
        else if(message == "2")
          digitalWrite(ledPin,LOW);
    }
}

void setup()
{
    Serial.begin(9600);
    softSerial.begin(9600);
    pinMode(ledPin,OUTPUT);
    olq.setHandleRaw(handleRaw);
    olq.setReceiveCallBak(subscribeMessageHandle);
}
void loop()
{
    olq.update();
    if(sendPingFlag && olq.getWifiState()== OBLOQWIFICONNECT)
    {
      sendPingFlag = false;
      olq.connect(iot_id,iot_pwd);
    }
    if(subscribeFlag && olq.getMqttConnectState())
    {
      subscribeFlag = false;
      //监听Topic
      olq.subscribe(topic);
    }
}

