#include <SoftwareSerial.h>
#include "Obloq.h"
#include "Button.h"

//MQTT连接相关参数
const String ssid = "DFRobot-guest";
const String password = "dfrobot@2017";
const String iot_id = "r1qHJFJ4Z";
const String iot_pwd = "SylqH1Y1VZ";
const String topic = "SyZ6l-mBG";

 //led小灯引脚
int buttonPin = 2;
bool sendPingFlag = true; 
String buttonMessage = "1";                        

Button button;
SoftwareSerial softSerial(10, 11);         // RX, TX
Obloq olq(softSerial, ssid, password);

void handleRaw(String& data)
{
    Serial.println(data);   //串口打印返回的数据
}

void setup()
{
    Serial.begin(9600);
    button.init(buttonPin);
    softSerial.begin(9600);
    olq.setHandleRaw(handleRaw);
}

void loop()
{
    olq.update();
    button.update();
    if(sendPingFlag && olq.getWifiState()== OBLOQWIFICONNECT)
    {
      sendPingFlag = false;
      olq.connect(iot_id,iot_pwd);
    }
    if(button.click() && olq.getMqttConnectState())
    {
        olq.publish(topic,buttonMessage);
        if(buttonMessage == "1")
            buttonMessage = "0";
        else
            buttonMessage = "1";
    }
}

