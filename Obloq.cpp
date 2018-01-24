#include <Arduino.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include "Obloq.h"

Obloq::Obloq(HardwareSerial & hardSerial, String ssid, String pwd, String mac)
{

	this->_hardSerial = &hardSerial;
	this->_ssid = ssid;
	this->_pwd = pwd;
}

Obloq::Obloq(SoftwareSerial & softSerial, String ssid, String pwd, String mac)
{
	this->_softSerial = &softSerial;
	this->_ssid = ssid;
	this->_pwd = pwd;
}

Obloq::~Obloq()
{
}

/** 
 * @brief 设置回显原始数据的回掉函数
 * @param   handle:回掉函数
 * @return  无
 */
void Obloq::setHandleRaw(HandleRaw handle)
{
	this->_handleRaw = handle;
}

void Obloq::setReceiveCallBak(ReceiveMessageCallbak handle)
{
	this->_receiveMessageCallbak = handle;
}

/** 
 * @brief 监测串口是否准备好
 * @param   无
 * @return  true:表示串口准备好，false:表示串口木有准备好
 */
bool Obloq::isSerialReady()
{
	return this->_isSerialReady;
}

/** 
 * @brief 获取wifi连接状态
 * @param   无
 * @return  "1":  wifi断开连接
 * @return  "2":  wifi连接中
 * @return  "3":  wifi连接成功，已获取Obloq的IP
 * @return  "4":  wifi处于未连接状态
 */
String Obloq::getWifiState()
{
	return this->_wifiState;
}

/** 
 * @brief 获取mqtt连接状态
 * @param   无
 * @return  true: mqtt连接成功，false:mqtt连接失败
 */
bool Obloq::getMqttConnectState()
{
	return this->_mqttConnectReady;
}

/** 
 * @brief 获取设备监听状态
 * @param   无
 * @return  true: 监听设备成功，false:监听设备失败
 */
bool Obloq::getSubscribeState()
{
	return this->_subscribeReady;
	this->_subscribeReady = false;
}
/** 
 * @brief 获取设备发送消息状态
 * @param   无
 * @return  true: 发送消息成功，false:发送消息失败
 */
bool Obloq::getPublishState()
{
	return this->_publishReady;
	this->_publishReady = false;
}

/** 
 * @brief 获取取消监听设备的状态
 * @param   无
 * @return  true: 取消监听成功，false:取消监听失败
 */
bool Obloq::getUnsubscribeState()
{
	return this->_unsubscribeReady;
	this->_unsubscribeReady = false;
}

/** 
 * @brief 获取设备监听状态
 * @param   无
 * @return  true: 监听设备成功，false:监听设备失败
 */
String Obloq::getIp()
{
	return this->_ip;
}

/** 
 * @brief 发送串口敲门指令
 * @param   无
 * @return  无
 */
void Obloq::ping()
{
	this->_isSerialReady = false;
	if (_hardSerial)
	{
		this->_hardSerial->print(F("|1|1|\r"));
	}
	else if (_softSerial)
	{
		this->_softSerial->print(F("|1|1|\r"));
	}
}

/** 
 * @brief 发送消息数据
 * @param   msg:发送的消息内容
 * @return  true:表示发送成功，false:表示发送失败
 */
bool Obloq::sendMsg(const String & msg)
{
	//Serial.print(F("Arduino send - > "));
	//Serial.println(msg);
		if (this->_isSerialReady)
	{
		if (this->_hardSerial)
		{
			this->_hardSerial->print(msg + "\r");
		}
		else if (this->_softSerial)
		{
			this->_softSerial->print(msg + "\r");
		}
		else
		{
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

/********************************************************************************************
Function    : splitString      
Description : 剔除分隔符，逐一提取字符串     
Params      : data[] 提取的字符串的目标储存地址；str 源字符串；delimiters 分隔符
Return      : 共提取的字符串的个数 
********************************************************************************************/
int splitString(String data[],String str,const char* delimiters)
{
  char *s = (char *)(str.c_str());
  int count = 0;
  data[count] = strtok(s, delimiters);
  while(data[count]){
    data[++count] = strtok(NULL, delimiters);
  }
  return count;
}

/** 
 * @brief 处理接收的消息数据
 * @param   data:接收的消息数据
 * @return  无
 */
void Obloq::receiveData(String data)
{
	const char* obloqMessage = data.c_str();
	if (this->_handleRaw)
	{ 
		this->_handleRaw(data);
	}
	if (data == F("|1|1|")) //检测到敲门成功
	{
		this->_isSerialReady = true;
		if (this->_wifiState == 2)
		{
			return;
		}
		this->connectWifi(this->_ssid,this->_pwd);
		return;
	}

	if (data == F("|1|3|"))  //检测到心跳数据
	{
		return;
	}

	splitString(receiveStringIndex,data,"|");

	if(receiveStringIndex[0] == "2")
	{
		this->_wifiState =  receiveStringIndex[1];
		if(this->_wifiState == "3")
			this->_ip = receiveStringIndex[2];
		return;
	}

	if((receiveStringIndex[0] == "4") && (receiveStringIndex[1] == "1"))
	{
		switch(receiveStringIndex[2].toInt())
		{
			case 1: handleConnectFlag();break;
			case 2: handleSubscribeFlag();break;
			case 3: handlePublishFlag();break;
			case 4: handleDisconnectFlag();break;
			case 5: handleReceiveMessage();break;
			case 6: handleUnsubscribeFlag();break;
			default:break;
		}
	}
}

/** 
 * @brief 处理mqtt连接标志
 * @param   无
 * @return  无
 */
void Obloq::handleConnectFlag()
{
	(receiveStringIndex[3] == "1")?this->_mqttConnectReady = true:this->_mqttConnectReady = false;
}

/** 
 * @brief 处理注册设备的标志
 * @param   无
 * @return  无
 */
void Obloq::handleSubscribeFlag()
{
	(receiveStringIndex[3] == "1")?this->_subscribeReady = true:this->_subscribeReady = false;
}

/** 
 * @brief 处理发送消息的标志
 * @param   无
 * @return  无
 */
void Obloq::handlePublishFlag()
{
	(receiveStringIndex[3] == "1")?this->_publishReady = true:this->_publishReady = false;
}

/** 
 * @brief 处理断开mqtt连接的标志
 * @param   无
 * @return  无
 */
void Obloq::handleDisconnectFlag()
{
	if(receiveStringIndex[3] == "1")
		this->_mqttConnectReady = false;
}


/** 
 * @brief 处理取消已注册设备的标志
 * @param   无
 * @return  无
 */
void Obloq::handleUnsubscribeFlag()
{
	(receiveStringIndex[3] == "1")?this->_unsubscribeReady = true:this->_unsubscribeReady = false;
}

/** 
 * @brief Obloq循环监测和处理串口数据
 * @param   无
 * @return  无
 */
void Obloq::handleReceiveMessage()
{
	if(this->_receiveMessageCallbak)
	{
		this->_receiveMessageCallbak(receiveStringIndex[3],receiveStringIndex[4]);
	}
}

/** 
 * @brief Obloq循环监测和处理串口数据
 * @param   无
 * @return  无
 */
void Obloq::update()
{
	if ((millis() - this->_time) >= this->_interval)
	{
		this->_time = millis();
		if (!this->_isSerialReady)
		{
			this->ping();
		}
	}

	if (_softSerial && _softSerial->available() > 0)
	{
		String data = _softSerial->readStringUntil('\r');
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
	if (_hardSerial && _hardSerial->available() > 0)
	{
		String data = _hardSerial->readStringUntil('\r');
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
}

/** 
 * @brief 连接wifi
 * @param   wifissid: wifi账号
 * @param   wifipwd: wifi密码
 * @return  无
 */
void Obloq::connectWifi(const String& wifissid,const String& wifipwd)
{
	String connectWifi_msg = "|2|1|" + wifissid + "," + wifipwd + _separator;
	this->sendMsg(connectWifi_msg);
}

/** 
 * @brief 断开已经连接的wifi
 * @param   wifissid: wifi账号
 * @param   wifipwd: wifi密码
 * @return  无
 */
void Obloq::disconnectWifi()
{
	this->sendMsg("|2|2|");
}

/** 
 * @brief 重新连接wifi，前提是在此之前成功连接过wifi并断开了
 * @param   wifissid: wifi账号
 * @param   wifipwd: wifi密码
 * @return  无
 */
void Obloq::reconnectWifi()
{
	this->connectWifi(this->_ssid,this->_pwd);
}

/** 
 * @brief 连接MQTT，默认：url = "iot.dfrobot.com.cn"，port= "1883"
 * @param   iot_id: 物联网iot_id，从物联网网址上直接复制
 * @param   iot_pwd: 物联网iot_pwd，从物联网网址上直接复制
 * @return  无
 */
void Obloq::connect(const String& iot_id,const String& iot_pwd)
{
	this->connect(this->_url,this->_port,iot_id,iot_pwd);
}
/** 
 * @brief 连接MQTT
 * @param   client_id: 物联网client_id，填写唯一的英文标识符即可
 * @param   iot_id: 物联网iot_id，从物联网网址上直接复制
 * @param   iot_pwd: 物联网iot_pwd，从物联网网址上直接复制
 * @param   url： 连接的物联网url
 * @param   port: 连接的物联网端口
 * @return  无
 */
void Obloq::connect(const String& url,const String& port,const String& iotid,const String& iotpwd)
{
	String connect_msg = "|4|1|1|" + url + _separator + port + _separator + iotid + _separator + iotpwd + _separator;
	this->sendMsg(connect_msg);
}

/** 
 * @brief 重新连接MQTT
 * @param   无
 * @return  无 
 */
void Obloq::reconnect()
{
	String reconnect_msg = F("|4|1|5|");
	this->sendMsg(reconnect_msg);
}

/** 
 * @brief 注册设备
 * @param   topic:设备ID
 * @return  无
 */
void Obloq::subscribe(const String& topic)
{
	String subscribe_msg = "|4|1|2|" + topic + _separator;
	this->sendMsg(subscribe_msg);
}

/** 
 * @brief 取消已经注册的设备
 * @param   topic:设备ID
 * @return  无
 */
void Obloq::resubscribe(const String& topic)
{
	String subscribe_msg = "|4|1|6|" + topic + _separator;
	this->sendMsg(subscribe_msg);
}
/** 
 * @brief 向特定设备发送消息数据
 * @param   topic:设备ID
 * @return  message：发送的消息内容
 */
void Obloq::publish(const String& topic, const String &message)
{
	String publish_msg = "|4|1|3|" + topic + _separator + message + _separator;
	this->sendMsg(publish_msg);
}

/** 
 * @brief 断开MQTT
 * @param   无
 * @return  无 
 */
void Obloq::disconnect()
{	
	String disconnect_msg = F("|4|1|4|");
	this->sendMsg(disconnect_msg);	
}
