#include "Arduino.h"
#include "SoftwareSerial.h"
#include "Obloq.h"

SoftwareSerial softSerial(10,11);
//生成OBLOQ对象，参数：串口指针，wifiSsid,WifiPwd,iotId,iotPwd
Obloq olq(&softSerial,"DFRobot-guest","dfrobot@2017");
static unsigned long currentTime = 0; 

void httpMsgHandle(const String& code,const String& message)
{
    Serial.println("Code: " + code + " , " + "Message : " + message);
}

void setup()
{
    softSerial.begin(9600);
    Serial.begin(115200);
    setHttpMsgHandle(httpMsgHandle);
}
void loop()
{
    olq.update();
    if(millis() - currentTime > 2000)
    {
        currentTime =  millis();
        olq.get("http://192.168.7.232:8080/input?id=1&val=2");
        // olq.post("http://192.168.7.232:8080/input?id=1&val=2","test");
    }
}