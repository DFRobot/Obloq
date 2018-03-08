#include <SoftwareSerial.h>
#include "Obloq.h"

SoftwareSerial softSerial(10,11);
//生成OBLOQ对象，参数：串口指针，wifiSsid,WifiPwd,iotId,iotPwd
Obloq olq(&softSerial,"DFRobot-guest","dfrobot@2017","SJG02MoBSf","Sk7AnMsBrG");
const String devTopic = "HkfVZ9BBz";
int ledPin = 13;
//设备的消息回调函数，监听的设备检测到消息会调用这个函数，可以在这个函数里面对接收的消息做判断和相应处理，需要用setMsgHandle()来设置这个回掉函数
void msgHandle(const String& topic,const String& message)
{
    if(devTopic == topic)
    {
        if(message == "1")
          digitalWrite(ledPin,HIGH);
        else if(message == "2")
          digitalWrite(ledPin,LOW);
    }
}

void setup()
{
    softSerial.begin(9600);
    pinMode(ledPin,OUTPUT);
    olq.setMsgHandle(msgHandle);//注册消息回掉函数
    olq.subscribe(devTopic); //监听设备,设备Topic:SyZ6l-mBG
}
void loop()
{
    olq.update();
}