#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Obloq.h"

//MQTT连接相关参数
const String ssid = "DFRobot-guest";
const String password = "dfrobot@2017";
const String iot_id = "r1qHJFJ4Z";
const String iot_pwd = "SylqH1Y1VZ";
const String topic1 = "SyZ6l-mBG";
const String topic2 = "H1xdUtPZG";
const String topic3 = "rkkdLtv-z";
const String topic4 = "B1Aw8Fv-z";
const String topic5 = "S1hwUtvZf";

bool sendPingFlag = true;    
unsigned long currentTime = 0;                       

SoftwareSerial softSerial(10, 11);         // RX, TX
Obloq olq(softSerial, ssid, password);

//回调函数，打印OBLOQ返回的原始数据，需要用setHandleRaw()来设置
void handleRaw(String& data)
{
    Serial.println(data);   //串口打印返回的数据
}
//已监听设备的消息回调函数，可以在这个函数里面对接收的消息做判断和相应处理，需要用setReceiveCallBak()来设置
void subscribeMessageHandle(const String topic,const String message)
{
    Serial.print("Topic : " + topic);
    Serial.print(" , ");
    Serial.println("Message : " + message);
}


void setup()
{
    Serial.begin(9600);
    softSerial.begin(9600);
    //设置回调函数，查看OBLOQ返回的原始数据
    olq.setHandleRaw(handleRaw);
    olq.setReceiveCallBak(subscribeMessageHandle);
}

bool subscribeFlag = true;
void loop()
{
    olq.update();
    if(sendPingFlag && olq.getWifiState() == OBLOQWIFICONNECT)
    {
      sendPingFlag = false;
      //连接物联网
      olq.connect(iot_id,iot_pwd);
    }
    if(subscribeFlag && olq.getMqttConnectState())
    {
      //监听Topic
      subscribeFlag = false;
      olq.subscribe(topic1);
      delay(100);
      olq.subscribe(topic2);
      delay(100);
      olq.subscribe(topic3);
      delay(100);
      olq.subscribe(topic4);
      delay(100);
      olq.subscribe(topic5);
    }
}
