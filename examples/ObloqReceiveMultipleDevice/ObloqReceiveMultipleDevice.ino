#include "Arduino.h"
#include "SoftwareSerial.h"
#include "Obloq.h"

SoftwareSerial softSerial(10,11);
//生成OBLOQ对象，参数：串口指针，wifiSsid,WifiPwd,iotId,iotPwd
Obloq olq(&softSerial,"DFRobot-guest","dfrobot@2017","SJG02MoBSf","Sk7AnMsBrG");

//设备的消息回调函数，监听的设备检测到消息会调用这个函数，可以在这个函数里面对接收的消息做判断和相应处理，需要用setMsgHandle()来设置这个回掉函数
void msgHandle(const String& topic,const String& message)
{
    Serial.println("Topic : " + topic + " , " + "Message : " + message);
}

void setup()
{
    Serial.begin(115200);
    softSerial.begin(9600);
    olq.setMsgHandle(msgHandle);//注册消息回掉函数
    olq.subscribe("H15OXaqUG");//监听多个设备
    olq.subscribe("HkoO7acIM");
    olq.subscribe("S1TOmpqUG");
    olq.subscribe("B1-t76c8f");
    olq.subscribe("r1GKmT9UG");
}
void loop()
{
    olq.update();
}