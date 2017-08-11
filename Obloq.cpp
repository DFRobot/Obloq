#include <Arduino.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>
#include "Obloq.h"

///JSON解析存储数据的数组大小
#define BufferSize 200

Obloq::Obloq(HardwareSerial & hardSerial, String ssid, String pwd, String mac)
{

	this->_hardSerial = &hardSerial;
	this->_ssid = ssid;
	this->_pwd = pwd;
	this->_mac = mac;
}

Obloq::Obloq(SoftwareSerial & softSerial, String ssid, String pwd, String mac)
{
	this->_softSerial = &softSerial;
	this->_ssid = ssid;
	this->_pwd = pwd;
	this->_mac = mac;
}


Obloq::~Obloq()
{
}

/** 
 * @brief 设置处理Obloq返回数据的回掉函数
 * @param   handle:回掉函数
 * @return  无
 */
void Obloq::setHandleJson(HandleJson handle)
{
	this->_handleJson = handle;
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
 * @return  -1: wifi断开连接
 * @return  1:  wifi连接中
 * @return  2:  wifi连接成功，已获取Obloq的IP
 * @return  3:  wifi处于未连接状态
 */
int Obloq::getWifiState()
{
	return this->_wifiState;
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
		this->_hardSerial->print(F("{\"type\":\"system\",\"message\":\"PING!\"}\r"));
	}
	else if (_softSerial)
	{
		this->_softSerial->print(F("{\"type\":\"system\",\"message\":\"PING!\"}\r"));
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

/** 
 * @brief 处理接收的消息数据
 * @param   data:接收的消息数据
 * @return  无
 */
void Obloq::receiveData(String data)
{
	if (this->_handleRaw)
	{ 
		this->_handleRaw(data);
	}
	if (data == F("{\"type\":\"system\",\"message\":\"PONG!\"}"))
	{
		this->_isSerialReady = true;
		if (this->_wifiState == 2)
		{
			return;
		}
		if (this->_mac == "")
		{
			this->sendMsg("{\"type\":\"system\",\"SSID\":\"" + this->_ssid + "\",\"PWD\":\"" + this->_pwd + "\"}");
		}
		else
		{
			this->sendMsg("{\"type\":\"system\",\"SSID\":\"" + this->_ssid + "\",\"PWD\":\"" + this->_pwd + "\",\"MAC\":\"" + this->_mac + "\"}");
		}
		return;
	}

	if (data == F("{\"type\":\"system\",\"message\":\"Heartbeat!\"}"))
	{
		return;
	}
	const char* jsonString = data.c_str();
	StaticJsonBuffer<BufferSize> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(jsonString);
	if (!root.success())
	{
		return;
	}

	String type = root["type"];
    if(type == "wifi")
        this->_wifiState = root["wifiState"];

	if (this->_handleJson)
	{
		this->_handleJson(root);
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
 * @brief 连接MQTT，默认：url = "iot.dfrobot.com.cn"，port= "1883"
 * @param   client_id: 物联网client_id，填写唯一的英文标识符即可
 * @param   iot_id: 物联网iot_id，从物联网网址上直接复制
 * @param   iot_pwd: 物联网iot_pwd，从物联网网址上直接复制
 * @return  无
 */
void Obloq::connect(const String& client_id,const String& iot_id,const String& iot_pwd)
{
	this->connect(client_id,iot_id,iot_pwd,this->_url,this->_port);
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
void Obloq::connect(const String& client_id,const String& iot_id,const String& iot_pwd,const String& url,const String& port)
{
	String connect_msg = "{\"type\":\"mqtt\",\"method\":\"connect\",\"ClientId\":\"" + client_id + "\",\"Iot_id\":\"" + iot_id + "\",\"Iot_pwd\":\""+ iot_pwd + "\",\"url\":\"" + url + "\",\"port\":\"" + port + "\"}";
	this->sendMsg(connect_msg);
}

/** 
 * @brief 重新连接MQTT
 * @param   无
 * @return  无 
 */
void Obloq::reconnect()
{
	String reconnect_msg = F("{\"type\":\"mqtt\",\"method\":\"reconnect\"}");
	this->sendMsg(reconnect_msg);
}

/** 
 * @brief 注册设备
 * @param   topic:设备ID
 * @return  无
 */
void Obloq::subscribe(const String& topic)
{
	String subscribe_msg = "{\"type\":\"mqtt\",\"method\":\"subscribe\",\"topic\":\"" + topic + "\"}";
	this->sendMsg(subscribe_msg);
}
/** 
 * @brief 向特定设备发送消息数据
 * @param   topic:设备ID
 * @return  message：发送的消息内容
 */
void Obloq::publish(const String& topic, const String &message)
{
	String publish_msg = "{\"type\":\"mqtt\",\"method\":\"publish\",\"topic\":\"" + topic + "\",\"message\":\"" + message + "\"}";
	this->sendMsg(publish_msg);
}

/** 
 * @brief 断开MQTT
 * @param   无
 * @return  无 
 */
void Obloq::disconnect()
{	
	String disconnect_msg = F("{\"type\":\"mqtt\",\"method\":\"disconnect\"}");
	this->sendMsg(disconnect_msg);	
}
