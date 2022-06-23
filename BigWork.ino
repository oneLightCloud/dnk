//引入ESP8266.h头文件
#include "ESP8266.h"
#include "dht11.h"
#include "SoftwareSerial.h"
#include <Keypad.h>  //引入矩阵键盘的头文件

//配置ESP8266WIFI设置
#define SSID "666666"    //2.4GHz的WIFI名称，
#define PASSWORD "12345418"//WIFI密码
#define HOST_NAME "api.heclouds.com"  //API主机名称，连接OneNET平台
#define DEVICE_ID "860653282"       //OneNet设备ID
#define HOST_PORT (80)                //API端口，连接OneNET平台
String APIKey = "uCA2FSqcnjbv6F4q9siFbtw0di4="; //设备绑定的APIKey

#define INTERVAL_SENSOR 5000 //定义传感器采样及发送时间间隔

//创建dht11

dht11 DHT11;

//定义DHT11接入Arduino的管脚
#define DHT11PIN 4

//定义ESP8266所连接的软串口

SoftwareSerial mySerial(3, 2);
ESP8266 wifi(mySerial);

//定义参数
unsigned int tempMax = 34 ;  //报警温度

//设置键盘参数
const byte ROWS = 4; //设置键盘个数
const byte COLS = 3; 

char keys[ROWS][COLS] = {  
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};  //对键盘赋值

byte rowPins[ROWS] = {5, 6, 7, 8}; //设置引脚
byte colPins[COLS] = {12, 10, 9};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup()
{
  mySerial.begin(115200); //初始化软串口
  Serial.begin(9600);     //初始化串口
  Serial.print("setup begin\r\n");
  analogReference(INTERNAL);  //调用板载1.1V基准源
  pinMode(11, OUTPUT);  //设置蜂鸣报警器
  digitalWrite(11, LOW);

  //ESP8266初始化
  Serial.print("FW Version: ");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStation()) {
    Serial.print("to station ok\r\n");
  } else {
    Serial.print("to station err\r\n");
  }

  //ESP8266接入WIFI
  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print("Join AP success\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  Serial.println("");
  Serial.print("DHT11 LIBRARY VERSION: ");
  Serial.println(DHT11LIB_VERSION);

  mySerial.println("AT+UART_CUR=9600,8,1,0,0");
  mySerial.begin(9600);
  Serial.println("setup end\r\n");
}

unsigned long net_time1 = millis(); //数据上传服务器时间
void loop(){
  char key = keypad.getKey();  //获取键盘输入数值
    if (key != NO_KEY){
    Serial.println(key);   
  }
  
  if (net_time1 > millis())
    net_time1 = millis();

  if (millis() - net_time1 > INTERVAL_SENSOR) //发送数据时间间隔
  {

    int chk = DHT11.read(DHT11PIN);

    Serial.print("Read sensor: ");  //判断是否读取传感器温湿度成功
    switch (chk) {
      case DHTLIB_OK:
        Serial.println("OK");
        break;
      case DHTLIB_ERROR_CHECKSUM:
        Serial.println("Checksum error");
        break;
      case DHTLIB_ERROR_TIMEOUT:
        Serial.println("Time out error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }

    float sensor_hum = (float)DHT11.humidity;
    float sensor_tem = (float)DHT11.temperature;
    Serial.print("Humidity (%): ");
    Serial.println(sensor_hum, 2);

    Serial.print("Temperature (oC): ");
    Serial.println(sensor_tem, 2);
    Serial.println("");

    if(sensor_tem >= tempMax){  //蜂鸣器：判断是否大于报警值
      analogWrite(11,200);
    }
    else{ 
      digitalWrite(11, LOW);
      }
      
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
      Serial.print("create tcp ok\r\n");
      char buf[10];
      //拼接发送data字段字符串
      String jsonToSend = "{\"Temperature\":";
      dtostrf(sensor_tem, 1, 2, buf);
      jsonToSend += "\"" + String(buf) + "\"";
      jsonToSend += ",\"Humidity\":";
      dtostrf(sensor_hum, 1, 2, buf);
      jsonToSend += "\"" + String(buf) + "\"";
      jsonToSend += "}";

      //拼接POST请求字符串
      String postString = "POST /devices/";
      postString += DEVICE_ID;
      postString += "/datapoints?type=3 HTTP/1.1";
      postString += "\r\n";
      postString += "api-key:";
      postString += APIKey;
      postString += "\r\n";
      postString += "Host:api.heclouds.com\r\n";
      postString += "Connection:close\r\n";
      postString += "Content-Length:";
      postString += jsonToSend.length();
      postString += "\r\n";
      postString += "\r\n";
      postString += jsonToSend;
      postString += "\r\n";
      postString += "\r\n";
      postString += "\r\n";

      const char *postArray = postString.c_str(); //将str转化为char数组
      Serial.println(postArray);
      wifi.send((const uint8_t *)postArray, strlen(postArray)); //send发送命令
      Serial.println("send success");
      if (wifi.releaseTCP()) { //释放TCP连接
        Serial.print("release tcp ok\r\n");
      } else {
        Serial.print("release tcp err\r\n");
      }
      postArray = NULL; //清空数组，等待下次传输数据
    } else {
      Serial.print("create tcp err\r\n");
    }

    Serial.println("");

    net_time1 = millis();
  }
}
