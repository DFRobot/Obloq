#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "Obloq.h"

const String ssid = "DFRobot-guest";
const String password = "dfrobot@2017";
const String client_id = "SkxprkFyE-";
const String iot_id = "r1qHJFJ4Z";
const String iot_pwd = "SylqH1Y1VZ";
const String topic = "BJpHJt1VW";

SoftwareSerial softSerial(10, 11);         // RX, TX
bool sendFlag = true;
int actionMode = 0;
int disconnectCount = 0;

Obloq olq(softSerial, ssid, password);

void handleRaw(const String& data)
{
    Serial.println(data);   //串口打印返回的数据
}
void handleJson(JsonObject& data)
{
    if(strcmp(data["type"],"mqtt") == 0)
    {
       if(data["mqttState"] == 1)
        {      
            actionMode = 1;     
            return;
        }
        else if(data["mqttState"] == -1)
        {
            disconnectCount++;
            if(disconnectCount == 1)
                actionMode = 4; 
        }
        else if(data["subscribe"] == 1)
        {
            actionMode = 2;
            return;
        }
        else if(data["message"] != "")
        {
            actionMode = 3;
            return;
        }           
    }
    return;
}

void setup()
{
    softSerial.begin(9600);
    Serial.begin(9600);
    olq.setHandleRaw(handleRaw);
    olq.setHandleJson(handleJson);
}
void loop()
{
    olq.update();
    if(sendFlag && olq.getWifiState()==2)
    {
      sendFlag = false;
      olq.connect(client_id,iot_id,iot_pwd);
    }
    switch(actionMode)
    {
        case 1: olq.subscribe(topic); actionMode = 0; break;
        case 2: olq.publish(topic,"hello world"); actionMode = 0; break;
        case 3: olq.disconnect(); actionMode = 0; break;
        case 4: olq.reconnect(); actionMode = 0; break;
        default: actionMode = 0; break;
    }
}
