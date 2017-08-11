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

//按键连接引脚
int  buttonPin = 2;
int  sendPingFlag = true;

//按键防抖动的相关参数
int normalVoltage = 0;                     
bool isPress = false;
bool isClick = false;
bool hasPress = false;
bool currentState = false;
unsigned long currentTime = 0;
bool keyFlag  = true;

SoftwareSerial softSerial(10, 11);         // RX, TX
Obloq olq(softSerial, ssid, password);

void handleRaw(String& data)
{
    //Serial.println(data);   //串口打印返回的数据
}
void handleJson(JsonObject& data)
{
}

void setup()
{
    //Serial.begin(9600);
    softSerial.begin(9600);
    pinMode(buttonPin,INPUT);
    //olq.setHandleRaw(handleRaw);
    //olq.setHandleJson(handleJson);
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
    keyScan();
    if(isClick)
    {
        if(keyFlag){
        olq.publish(topic, "1");    
        keyFlag = false;
        }
        else{
        olq.publish(topic, "0");             
        keyFlag = true;
        }    
    }
}

//按键防抖扫描
void keyScan()
{
   if((digitalRead(buttonPin)!=normalVoltage)&&!isPress)
    {
        isPress = true;
        currentTime = millis();
    }
    else if (isPress)
    {
        if (millis()-currentTime>=10)
        {
            if(digitalRead(buttonPin)!=normalVoltage)
            {
                currentState = true;
            }
            else
            {
                currentState = false;
                isPress = false;
            }
        }
    }
    isClick = false;
    if (currentState) {
        hasPress = true;
    }
    else if(hasPress) {
        isClick = true;
        hasPress = false;
    }
}
