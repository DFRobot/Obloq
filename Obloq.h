/*
 * @Author: Jason 
 * @Date: 2018-02-09 09:54:51 
 * @Last Modified by: Jason
 * @Last Modified time: 2018-03-21 
 */
#ifndef _OBLOQ_H_
#define _OBLOQ_H_
#include "Arduino.h"

//最多能够监听的Topic个数
#define MAXTOPICNUMBER 5

//返回消息的code码,用来判断OBLOQ发送的消息是否得到响应
#define SUCCESSED "1"
#define FAILED    "2"

//返回消息的类型
#define SYSTEMTYPE "1"
#define WIFITYPE   "2"
#define HTTPTYPE   "3"
#define MQTTTYPE   "4"

//系统返回的消息状态
#define SYSTEMPING      "1"
#define SYSTEMVERSION   "2"
#define SYSTEMHEARTBEAT "3"

//wifi连接状态下的各个子状态
#define WIFIDISCONNECT    "1"
#define WIFICONNECTING    "2"
#define WIFICONNECTED     "3"
#define WIFICONNECTFAILED "4"

//mqtt状态下的各个子状态
#define MQTTCONNECT    "1"
#define MQTTSUBSCRIBE  "2"
#define MQTTPUBLISH    "3"
#define MQTTRECEIVEMSG "5" 

//Obloq当前的执行状态
enum State
{
    none,
    ping,
    getVersion,
    wifiConnecting,
    mqttConnecting,
    mqttConnected
};

//系统消息协议字段的位置
class systemProtocol
{
public:
    static const uint8_t systemType    = 0;
    static const uint8_t systemCode    = 1;
    static const uint8_t systemMessage = 2;
};

//wifi协议各字段的位置
class wifiProtocol
{
public:
    static const uint8_t wifiType    = 0;
    static const uint8_t wifiCode    = 1;
    static const uint8_t wifiMessage = 2;
};

//http协议各字段位置
class httpProtocol
{
public:
    static const uint8_t httpType    = 0;
    static const uint8_t httpCode    = 1;
    static const uint8_t httpMessage = 2;
};

//mqtt协议各字段的位置
class mqttProtocol
{
public:
    static const uint8_t mqttType      = 0;
    static const uint8_t mqttMethod    = 1;
    static const uint8_t mqttFunction  = 2;
    static const uint8_t mqttCode      = 3;
    static const uint8_t mqttMessage   = 4;
    static const uint8_t mqttTopic     = 3;
};

class Obloq
{
public:
    //回调函数的函数指针
    typedef void (*RawHandle)(const String& message);
    typedef void (*MsgHandle)(const String& topic, const String& message);
    typedef void (*HttpMsgHandle)(const String& code, const String& message);

public:
    /** 
     * @brief   构造函数,连接指定url的Iot
     * @param   serial:设备ID
     * @param   ssid:wifi账号
     * @param:  pwd:wifi密码
     * @param:  url:连接的iot网址
     * @param:  port:连接iot的端口
     * @param:  iotID:连接iot的用户ID,连接Easy Iot可在工作间里面获取这个ID
     * @param:  iotPwd:连接iot的密码，连接Easy Iot可在工作间里面获取这个ID
     * @return: 无
     */
    Obloq(Stream *serial, const String& ssid, const String& pwd, const String& url,const String& port, const String& iotId, const String& iotPwd);

    /** 
     * @brief   构造函数,连接默认的Easy Iot
     * @param   serial:设备ID
     * @param   ssid:wifi账号
     * @param:  pwd:wifi密码
     * @param:  iotID:连接iot的用户ID,连接Easy Iot可在工作间里面获取这个ID
     * @param:  iotPwd:连接iot的密码，连接Easy Iot可在工作间里面获取这个ID
     * @return: 无
     */
    Obloq(Stream *serial, const String& ssid, const String& pwd, const String& iotId, const String& iotPwd);
    
    /** 
     * @brief   构造函数,连接http的时候使用这个这个构造函数来生成实例
     * @param   serial:设备ID
     * @param   ssid:wifi账号
     * @param:  pwd:wifi密码
     * @param:  iotID:连接iot的用户ID,连接Easy Iot可在工作间里面获取这个ID
     * @param:  iotPwd:连接iot的密码，连接Easy Iot可在工作间里面获取这个ID
     * @return: 无
     */
    Obloq(Stream *serial, const String& ssid, const String& pwd);

    ~Obloq();


    //******************************************************MQTT API********************************************
    /** 
     * @brief 设置原始数据回调函数,当有错误发生时会调用设置的回调函数来返回错误信息
     * @param   handle:回调函数
     * @return: 无
     */
    void setRawHandle(RawHandle handle);

    /** 
     * @brief   设置Iot设备接收消息回调函数,返回接收消息的tpoic和消息内容
     * @param   handle:回调函数
     * @return: 无
     */
    void setMsgHandle(MsgHandle handle);

    /** 
     * @brief   设置http返回消息回调函数
     * @param   handle:回调函数
     * @return: 无
     */
    void setHttpMsgHandle(HttpMsgHandle handle);

    /** 
     * @brief   获取OBLOQ连接Iot状态
     * @return: true:连接成功 ，false:连接失败
     */
    bool enable() const {return this->_enable;}

    /** 
     * @brief   获取wifi连接是否成功 
     * @return: true:wifi连接成功,wifi连接失败
     */
    bool isWifiConnected();

    /** 
     * @brief   获取设备IP,OBLOQ正常连接wifi后会获得一个特定的IP地址 
     * @return: 设备ip
     */
    String getIp() const {return this->_ip;}

    /** 
     * @brief  获取OBLOQ固件版本号
     * @return: 固件版本号，如果返回"xxx"，表示版本号没有获取成功
     */
    String getFirmwareVersion() const {return this->_firmwareVersion;}

    /** 
     * @brief   循环检测OBLOQ当前状态和解析串口数据，需要放在Arduino loop()函数里面 
     * @return: 无
     */
    void update();
    
    /** 
     * @brief   监听Iot设备,实际上是记录监听的topic
     * @param   topic:监听设备的Topic
     * @return: 无
     */
    void subscribe(String topic);

    /** 
     * @brief   Iot设备发送消息
     * @param   topic:发送消息的设备Topic 
     * @param   message:发送的消息内容
     * @return: 无
     */
    void publish(const String& topic, const String& message);


    //******************************************************Http API********************************************
    /** 
     * @brief   Http get请求
     * @param   url : get请求的网址 
     * @return: 无
     */
    void get(const String& url);

    /** 
     * @brief   Http post请求
     * @param   url: post请求的网址
     * @param   content: post的消息内容
     * @return: 无
     */
    void post(const String& url, const String& content);

private:

    Stream *_serial = NULL;
    String _receiveStringIndex[10] = {};
    String _separator = "|";
	String _ssid = "";
	String _pwd = "";
	String _url = "iot.dfrobot.com.cn";
	String _port= "1883";
    String _iotId = "";
    String _iotPwd = "";
    String _ip = "";

    bool _enable = false;
    bool _isSerialReady = false; 
    bool _reconnectMqtt = false;
    bool _firstSubscribe = true;
    String _wifiState = "";
    String _subscribeState = "";
    String _firmwareVersion = "xxx"; 

    RawHandle _rawHandle = NULL;
    MsgHandle _msgHandle = NULL;
    HttpMsgHandle _httpMsgHandle = NULL;

    enum State _currentState = State::ping;
    String _topicArray[MAXTOPICNUMBER];
    int _topicCount = 0;

    unsigned long _time = 0;
    //发送敲门指令的时间间隔
    unsigned long _pingInterval = 2000;
    //当wifi连接失败或者异常断开后自动每隔1分钟发送一次连接请求
    unsigned long _wifiConnectInterval = 60000;
    //后自动每隔1分钟发送一次连接请求
    unsigned long _mqttConnectInterval = 60000;
    //publish方法发送消息时限定每两秒钟发送一次数据
    unsigned long _publishInterval = 0;
    //获取固件版本的请求时间间隔100ms
    unsigned long _gerVersionInterval = 100;

private:
    /** 
     * @brief   串口发送数据
     * @param   msg:发送的数据内容
     * @return: 无
     */
    void sendMsg(const String & msg);

    /** 
     * @brief   发送敲门指令 
     * @return: 无
     */
    void ping();

    /** 
     * @brief 连接wifi,wifi账号和密码是在构造函数中以参数形式传递进来的
     * @return: 无
     */
    void connectWifi();

    /** 
     * @brief   处理串口接收的数据
     * @param   data:串口接收的数据
     * @return: 无
     */
    void receiveData(const String& data);

    /** 
     * @brief   以特定字符切割字符串 
     * @param   data:存放字符串切割后的每个字段
     * @param   str:切割的目标字符串
     * @param:  delimiters:特定的切割字符
     * @return: 切割得到的字段个数
     */
    int  splitString(String data[],String str,const char* delimiters);

    /** 
     * @brief   连接mqtt，mqtt的用户id和pwd等是在构造函数中以参数形式传递进来的
     * @return: 无
     */
    void connectMqtt();

    /** 
     * @brief   监听所有记录的topic
     * @return: 无
     */
    void subscribeTopicArray();

    /** 
     * @brief   监听单个Iot设备
     * @param   topic:监听设备的Topic
     * @return: true:监听成功,false:监听失败
     */
    bool subscribeSingleTopic(const String& topic);

    /** 
     * @brief   获取Iot设备监听是否成功
     * @return: true:监听成功，false:监听失败或者监听过程中
     */
    bool isSubscribeSuccessed(); 

    /**
     * @brief 获取Iot设备监听是否失败
     * @return: true:监听失败，false:监听成功或者监听过程中
     */
    bool isSubscribeFailed();

    /**
     * @brief 检查固件版本，发送消息到OBLOQ
     * @return: 无
     */
    void checkFirmwareVersion();
};

#endif