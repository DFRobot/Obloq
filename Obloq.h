#pragma once
#define INTERVAL 1000

class HardwareSerial;
class SoftwareSerial;

typedef void(*HandleJson)(JsonObject& data);
typedef void(*HandleRaw)(const String& data);
class Obloq
{
private:
	HardwareSerial* _hardSerial = NULL;
	SoftwareSerial* _softSerial = NULL;
	HandleJson _handleJson = NULL;
	HandleRaw _handleRaw = NULL;
	int _wifiState = -1;
	int _interval = INTERVAL;
	bool _isSerialReady = false;
	unsigned long _time = 0;
	String _ssid = "";
	String _pwd = "";
	String _mac = "";
	String _url = "iot.dfrobot.com.cn";
	String _port= "1883";
	
public:
	Obloq(HardwareSerial & hardSerial, String ssid, String pwd = "", String mac = "");
	Obloq(SoftwareSerial & softSerial, String ssid, String pwd = "", String mac = "");

	~Obloq();

public:
	///设置处理Obloq返回数据的回掉函数
	void setHandleJson(HandleJson);
	///设置回显原始数据的回掉函数
	void setHandleRaw(HandleRaw);
	///监测串口是否准备好
	bool isSerialReady();
	///获取wifi连接状态
	int	 getWifiState();
	///发送串口敲门指令
	void ping();
	///Obloq循环监测和处理串口数据
	void update();
	///连接MQTT，默认：url = "iot.dfrobot.com.cn"，port= "1883"
	void connect(const String& client_id,const String& iot_id,const String& iot_pwd);
	///连接MQTT
	void connect(const String& client_id,const String& iot_id,const String& iot_pwd,const String& url,const String& port);
	///重新连接MQTT
	void reconnect();
	///注册设备
	void subscribe(const String& topic);
	///向特定设备发送消息数据
	void publish(const String& topic, const String &message);
	///断开MQTT
	void disconnect();
private:
	void receiveData(String);
	bool sendMsg(const String&);
};

