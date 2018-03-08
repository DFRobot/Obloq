#include <SoftwareSerial.h>
#include "Obloq.h"

SoftwareSerial softSerial(10,11);
//生成OBLOQ对象，参数：串口指针，wifiSsid, WifiPwd, iotId, iotPwd
//Generate OBLOQ object, parameters: serial pointer, wifiSsid, WifiPwd, iotId, iotPwd
Obloq olq(&softSerial,"DFRobot-guest","dfrobot@2017","SJG02MoBSf","Sk7AnMsBrG");
const String devTopic = "HkfVZ9BBz";
 //led小灯引脚 led pin
int ledPin = 2;

//已监听设备的消息回调函数，可以在这个函数里面对接收的消息做判断和相应处理，需要用setMsgHandle()来设置这个回调函数
//message callback function of the subscribed device. This function handle the message received. setMsgHandle() will change settings of this function.
void msgHandle(const String& topic,const String& message)
{
    if(devTopic == topic)
    {
        if(message == "1")
          digitalWrite(ledPin,LOW);
        else if(message == "2")
          digitalWrite(ledPin,HIGH);
    }
}

void setup()
{
    pinMode(ledPin,OUTPUT);
    softSerial.begin(9600);
    olq.setMsgHandle(msgHandle);//注册消息回调函数 register message callback function
    olq.subscribe(devTopic); //监听设备 Subscribe topic
}
void loop()
{
    olq.update();
}