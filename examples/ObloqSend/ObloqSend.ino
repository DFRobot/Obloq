#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Obloq.h"

//MQTT连接相关参数
const String ssid = "DFRobot-guest";
const String password = "dfrobot@2017";
const String iot_id = "r1qHJFJ4Z";
const String iot_pwd = "SylqH1Y1VZ";
const String topic = "SyZ6l-mBG";

bool sendPingFlag = true;
unsigned long currentTime = 0;

SoftwareSerial softSerial(10, 11);         // RX, TX
Obloq olq(softSerial, ssid, password);

//回调函数，打印OBLOQ返回的原始数据，需要用setHandleRaw()来设置
void handleRaw(String& data)
{
    Serial.println(data);   //串口打印返回的数据
}
    
//获得LM35测得的温度
float getTemp()
{
    uint16_t val;
    float dat;
    val=analogRead(A0);//Connect LM35 on Analog 0
    dat = (float) val * (5/10.24);
    return dat;
}

void setup()
{
    Serial.begin(9600);
    softSerial.begin(9600);
    //注册回调函数，查看OBLOQ返回的原始数据
    olq.setHandleRaw(handleRaw);

}
void loop()
{
    olq.update();
    if(sendPingFlag && olq.getWifiState() == OBLOQWIFICONNECT)
    {
      sendPingFlag = false;
      //连接物联网
      olq.connect(iot_id,iot_pwd);
    }
    if(olq.getMqttConnectState() && millis() - currentTime > 10000)
    {
        currentTime = millis();
        //获取温度数据
        float temperature = getTemp();
        //发送数据到物联网
        olq.publish(topic,(String)temperature); 
    }
}

