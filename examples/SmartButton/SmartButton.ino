#include <SoftwareSerial.h>
#include "Obloq.h"
#include "Button.h"

Button button;
SoftwareSerial softSerial(10,11);
//生成OBLOQ对象，参数：串口指针，wifiSsid,WifiPwd,iotId,iotPwd
Obloq olq(&softSerial,"DFRobot-guest","dfrobot@2017","SJG02MoBSf","Sk7AnMsBrG");
//Iot设备Topic:HkfVZ9BBz
const String devTopic = "HkfVZ9BBz";
String buttonMessage = "";

void setup()
{
    button.init(2);
    softSerial.begin(9600);
}

void loop()
{
    olq.update();
    button.update();
    if(button.click())
    {
        olq.publish(devTopic,buttonMessage); //向Iot设备发送消息
        if(buttonMessage == "1")
            buttonMessage = "0";
        else
            buttonMessage = "1";
    }
}