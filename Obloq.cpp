/*
 * @Author: Jason 
 * @Date: 2018-02-09 09:54:28 
 * @Last Modified by: Jason
 * @Last Modified time: 2018-02-09 17:56:19
 */

#include "Obloq.h"

Obloq::Obloq(Stream *serial, const String& ssid, const String& pwd, const String& url,const String& port, const String& iotId, const String& iotPwd)
{
    this->_serial = serial;
    this->_ssid   = ssid;
    this->_pwd    = pwd;
    this->_url    = url;
    this->_port   = port;
    this->_iotId  = iotId;
    this->_iotPwd = iotPwd;
}

Obloq::Obloq(Stream *serial, const String& ssid, const String& pwd, const String& iotId, const String& iotPwd)
{
    this->_serial = serial;
    this->_ssid   = ssid;
    this->_pwd    = pwd;
    this->_iotId  = iotId;
    this->_iotPwd = iotPwd;
}

Obloq::~Obloq()
{}

void Obloq::setRawHandle(RawHandle handle)
{
    this->_rawHandle = handle;
}

void Obloq::setMsgHandle(MsgHandle handle)
{
    this->_msgHandle = handle;
}

void Obloq::update()
{
    switch(this->_currentState)
    {
        case State::ping : ping(); break;
        case State::wifiConnecting: connectWifi(); break;
        case State::mqttConnecting: connectMqtt(); break;
        default:break;
    }
    if (this->_serial && this->_serial->available() > 0)
	{
		String data = this->_serial->readStringUntil('\r');
        //Serial.print("Rece: ");
        //Serial.println(data);
		while (true)
		{
			int length = data.length();
			int index = data.indexOf('\r');
			String subData = data.substring(0, index);
			this->receiveData(subData);
            
			if (index == -1 || length - index <= 1)
			{
				break;
			}
			else
			{
				subData = data.substring(index + 1, length);
				data = String(subData);
			}
		}
	}
    //如果mqtt连接成功，则去监听所有记录的topic
    if(enable())
    {
        subscribeTopicArray();
    }
}

void Obloq::receiveData(const String& data)
{
	if (data == F("|1|1|")) //检测到敲门成功
	{
		this->_currentState = State::wifiConnecting;
        this->_time = millis() - this->_wifiConnectInterval; //将当前时间减去wifi连接的间隔时间，保证在第一次进入connectWifi函数的时候能够立刻连接wifi
        if(this->_rawHandle)
            this->_rawHandle(data);
		return;
	}

	splitString(this->_receiveStringIndex,data,"|");

	if(this->_receiveStringIndex[wifiProtocol::wifiType] == WIFITYPE) //wifi消息
	{
        //wifi异常断开
        if(this->_receiveStringIndex[wifiProtocol::wifiCode] == WIFIDISCONNECT && this->_wifiState == WIFICONNECTED)
        {
            this->_wifiState = WIFIDISCONNECT;
            this->_currentState = State::wifiConnecting;
            if(this->_enable)
            {
                this->_enable = false;
                this->_reconnectMqtt = true;
            }
            if(this->_rawHandle)
                this->_rawHandle(data);
        }
		else if(this->_receiveStringIndex[wifiProtocol::wifiCode] == WIFICONNECTED)
        {
			this->_ip = this->_receiveStringIndex[wifiProtocol::wifiMessage];
            this->_time = millis() - this->_mqttConnectInterval; //将当前时间减去mqtt连接的间隔时,这样在第一次进入mqtt连接
            this->_wifiState = WIFICONNECTED;
            this->_currentState = State::mqttConnecting;
            // this->_currentState = State::none;
        }
		return;
	}
    else if(this->_receiveStringIndex[mqttProtocol::mqttType] == MQTTTYPE)//mqtt消息 
    {
        if(this->_receiveStringIndex[mqttProtocol::mqttFunction] == MQTTCONNECT) //连接mqtt返回的消息
        {
            if(this->_receiveStringIndex[mqttProtocol::mqttCode] == FAILED) //mqtt连接失败
            {
                this->_currentState = State::mqttConnecting; //重新设置当前状态
                if(this->_enable)  //之前已经成功连接过mqtt，再次连接需要用“重新连接mqtt”协议，不能使用“连接mqtt"协议，因为第一次连接的时候需要new出来一个新的mqtt句柄      
                {
                    this->_enable = false;
                    this->_reconnectMqtt = true;              
                    this->_time = millis() - this->_mqttConnectInterval; //将当前时间减去mqtt连接的间隔时,这样第一次进入函数connectMqtt()的时候能够立即执行发送消息的指令
                }
                if(this->_rawHandle)
                {
                    this->_rawHandle(data);
                }
            }
            else //mqtt连接成功
            {
                this->_enable = true;
                this->_currentState = State::none;
            }
        }
        else if(this->_receiveStringIndex[mqttProtocol::mqttFunction] == MQTTPUBLISH) //发送消息
        {
            if(this->_receiveStringIndex[mqttProtocol::mqttCode] == FAILED && this->_rawHandle) //发送消息失败
            {
                this->_rawHandle(data);
            }
        }
        else if(this->_receiveStringIndex[mqttProtocol::mqttFunction] == MQTTSUBSCRIBE) //订阅Topic
        {
            if(this->_receiveStringIndex[mqttProtocol::mqttCode] == FAILED) //订阅Topic失败
            {
                this->_subscribeState = FAILED; 
                if(this->_rawHandle)
                    this->_rawHandle(data);
            }
            else //订阅Topic成功
            {
                this->_subscribeState = SUCCESSED;
            }
        }
        else if(this->_receiveStringIndex[mqttProtocol::mqttFunction] == MQTTRECEIVEMSG) //监听的Topic返回消息
        {
            if(this->_msgHandle)
            {
                this->_msgHandle(this->_receiveStringIndex[mqttProtocol::mqttTopic],this->_receiveStringIndex[mqttProtocol::mqttMessage]);
            }
        }
    }

}

void Obloq::connectMqtt()
{
    if(millis() - this->_time >=this->_mqttConnectInterval)
    {
        this->_time= millis();
        if(this->_reconnectMqtt)
        {
            String reconnectMsg = F("|4|1|5|");
            this->sendMsg(reconnectMsg);
        }
        else
        {
            String connectMsg = "|4|1|1|" + _url + _separator + _port + _separator + _iotId + _separator + _iotPwd + _separator;
            this->sendMsg(connectMsg);
        }
    }
}

void Obloq::subscribe(String topic)
{
    //检查topic是否已经达到最大值5个
    if(this->_topicCount >= MAXTOPICNUMBER)
        return;
    
    //检测topic是否已经记录过
    for(int i = 0; i < this->_topicCount; i++)
    {
        if(this->_topicArray[i] == topic)
        {
            return;
        }
    }
    //记录需要监听的topic
    this->_topicArray[this->_topicCount++] = topic;
    
}

void Obloq::subscribeTopicArray()
{
    static uint8_t num = 0;
    if(this->_topicCount != 0 && num < this->_topicCount) //订阅所有记录的topic
    {
        if(subscribeSingleTopic(this->_topicArray[num]))
        {
            num++;
            this->_firstSubscribe = true;
            // Serial.print("num: ");
            // Serial.println(num);
        }
    }
}

bool Obloq::subscribeSingleTopic(const String& topic)
{
    if(this->_enable)
    {
        if(this->_firstSubscribe || this->isSubscribeFailed())
        {
            this->_firstSubscribe = false;
            String subscribeMsg = "|4|1|2|" + topic + _separator;
            this->sendMsg(subscribeMsg);
        }
        if(this->isSubscribeSuccessed()) //注册成功了一个topic
        {
            //Serial.println("subscribe topic success");
            return true;
        }
        return false;
    }
}

void Obloq::publish(const String& topic, const String& message)
{
    static unsigned long publishCurrentTime = 0;
    if(this->_enable)
    {
        if(publishInterval == 0)
        {
            String publishMsg = "|4|1|3|" + topic + _separator + message + _separator;
            this->sendMsg(publishMsg); 
        }
        else if(millis() - publishCurrentTime > publishInterval)
        {
            publishCurrentTime = millis();
            String publishMsg = "|4|1|3|" + topic + _separator + message + _separator;
            this->sendMsg(publishMsg);
        }  
    }
}

void Obloq::ping()
{
    if ((millis() - this->_time) >= this->_pingInterval)
    {
        this->_time = millis();
        this->_serial->print(F("|1|1|\r"));
    }
}

void Obloq::connectWifi()
{
    if ((millis() - this->_time) >= this->_wifiConnectInterval)
    {
        this->_time = millis();
        String connectWifiMsg = "|2|1|" + _ssid + "," + _pwd + _separator;
        this->sendMsg(connectWifiMsg);
    }
}

bool Obloq::isWifiConnected()
{
    if(this->_wifiState == WIFICONNECTED)
        return true;
    else 
        return false;
}

void Obloq::sendMsg(const String & msg)
{
	if (this->_serial)
	{
        this->_serial->print(msg + "\r");
        // Serial.println("Send: " + msg);
	}
}

int Obloq::splitString(String data[],String str,const char* delimiters)
{
    char *s = (char *)(str.c_str());
    int count = 0;
    data[count] = strtok(s, delimiters);
    while(data[count])
    {
        data[++count] = strtok(NULL, delimiters);
    }
    return count;
}


bool Obloq::isSubscribeSuccessed()
{
    if(this->_subscribeState == SUCCESSED)
    {
        this->_subscribeState = ""; 
        return true;
    }
    return false;
}

bool Obloq::isSubscribeFailed()
{
    if(this->_subscribeState == FAILED)
    {
        this->_subscribeState = "";
        return true;
    }
    return false;
}



