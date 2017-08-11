#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "Obloq.h"

//MQTT连接相关参数
const String ssid = "wifissid";
const String password = "wifipwd";
const String client_id = "SkxprkFyE-";
const String iot_id = "r1qHJFJ4Z";
const String iot_pwd = "SylqH1Y1VZ";
const String topic = "BJpHJt1VW";

 //led小灯引脚
int ledPin = 2;

int  sendPingFlag = true;                           

SoftwareSerial softSerial(10, 11);         // RX, TX
Obloq olq(softSerial, ssid, password);

void handleRaw(String& data)
{
    //Serial.println(data);   //串口打印返回的数据
}
void handleJson(JsonObject& data)
{
    int message = 0;
    char *deviceTopic = topic.c_str();
    if(strcmp(data["topic"],deviceTopic) == 0)
    {
        message = data["message"];
        switch(message)
        {
            case 0: digitalWrite(ledPin,LOW);break;
            case 1: digitalWrite(ledPin,HIGH) ;break;
            default:break;
        }
    }  
}

void setup()
{
    //Serial.begin(9600);
    softSerial.begin(9600);
    pinMode(ledPin,OUTPUT);
    //olq.setHandleRaw(handleRaw);
    olq.setHandleJson(handleJson);
}
void loop()
{
    olq.update();
    if(sendPingFlag && olq.getWifiState()==2)
    {
      sendPingFlag = false;
      olq.connect(client_id,iot_id,iot_pwd);
      delay(1000);
      olq.subscribe(topic);
    }
}

