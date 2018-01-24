#pragma once
#include <Arduino.h>
#define INTERVAL 1000

#define OBLOQWIFIDISCONNECT "1"
#define OBLOQWIFICONNECTING "2"
#define OBLOQWIFICONNECT "3"
#define OBLOQWIFICONNECTFAILD "4"

class HardwareSerial;
class SoftwareSerial;

typedef void(*HandleRaw)(const String& data);
typedef void(*ReceiveMessageCallbak)(const String topic,const String message);

class Obloq
{
private:
	HardwareSerial* _hardSerial = NULL;
	SoftwareSerial* _softSerial = NULL;
	HandleRaw _handleRaw = NULL;
	ReceiveMessageCallbak _receiveMessageCallbak = NULL;
	String _wifiState = "-1";
	bool _mqttConnectReady = false;
	bool _subscribeReady = false;
	bool _publishReady = false;
	bool _disConnectReady = false;
	bool _unsubscribeReady = false;
	bool _isSerialReady = false;
	int _interval = INTERVAL;
	String _ip = "";
	unsigned long _time = 0;
	String _separator = "|";
	String _ssid = "";
	String _pwd = "";
	String _url = "iot.dfrobot.com.cn";
	String _port= "1883";
	String receiveStringIndex[10] = {};

private:
	void handleConnectFlag();
	void handleSubscribeFlag();
	void handlePublishFlag();
	void handleDisconnectFlag();
	void handleReceiveMessage();
	void handleUnsubscribeFlag();
	
public:
	Obloq(HardwareSerial & hardSerial, String ssid, String pwd = "", String mac = "");
	Obloq(SoftwareSerial & softSerial, String ssid, String pwd = "", String mac = "");

	~Obloq();

public:
	///设置回显原始数据的回掉函数
	void setHandleRaw(HandleRaw);
	///设置消息回调函数,当监听的设备有消息返回时会自动调用设置的回调函数
	void setReceiveCallBak(ReceiveMessageCallbak);
	///监测串口是否准备好
	bool isSerialReady();
	///获取wifi连接状态
	String getWifiState();
	///获取wifi 的ip地址
	String getIp();
    //获取mqtt连接状态
	bool getMqttConnectState();
	//获取mqtt监听状态
	bool getSubscribeState();
	//获取消息发送状态
	bool getPublishState();
	///取消监听Topic的状态
	bool getUnsubscribeState();
	///发送串口敲门指令
	void ping();
	///Obloq循环监测和处理串口数据
	void update();
	///连接wifi
	void connectWifi(const String& wifissid,const String& wifipwd);
	///断开wifi
	void disconnectWifi();
	///重新连接wifi
	void reconnectWifi();
	///连接MQTT，默认：url = "iot.dfrobot.com.cn"，port= "1883"
	void connect(const String& iot_id,const String& iot_pwd);
	///连接MQTT
	void connect(const String& url,const String& port,const String& iotid,const String& iotpwd);
	///重新连接MQTT
	void reconnect();
	///注册设备
	void subscribe(const String& topic);
	///取消已经注册的设备
	void resubscribe(const String& topic);
	///向特定设备发送消息数据
	void publish(const String& topic, const String &message);
	///断开MQTT
	void disconnect();
private:
	void receiveData(String);
	bool sendMsg(const String&);
};

