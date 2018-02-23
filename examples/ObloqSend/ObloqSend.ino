#include "Arduino.h"
#include "SoftwareSerial.h"
#include "Obloq.h"

SoftwareSerial softSerial(10,11);
//生成OBLOQ对象，参数：串口指针，wifiSsid,WifiPwd,iotId,iotPwd
Obloq olq(&softSerial,"DFRobot-guest","dfrobot@2017","SJG02MoBSf","Sk7AnMsBrG");

void setup()
{
    softSerial.begin(9600);
}
void loop()
{
    olq.update();
    olq.publish("S1TOmpqUG", "Test"); //向设备发送消息，设备Topic:S1TOmpqUG,消息内容：Test
}