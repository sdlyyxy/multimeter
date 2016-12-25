#include "U8glib.h"
#include <SoftwareSerial.h>
#include <IRremote.h>
#include "./ESP8266.h"
//感谢的Kaionji Seimei
//我原来试图用esp8266Serial.println()向WIFI模块发送指令，但各种问题，于是想用ESP8266类。系统自带的ESP8266.h貌似不能用于软串口，多亏了Kaionji Seimei的改写的头文件。
//Kaionji Seimei http://139.199.29.58/
//https://github.com/Zirconium2159/Station/
#define esp8266Serial mySerial   // 定义WIFI模块通信串口
#define setFont_LL u8g.setFont(u8g_font_osr18)
#define setFont_SS u8g.setFont(u8g_font_6x10)
#define readPIN A0 
#define RECV_PIN 12
#define INCREASE 0x1FEF807    //遥控器按键编码定义:增加+
#define DECREASE 0x1FE708F   //减少-
#define NUM_0 0x1FED827      //数字0
#define NUM_1 0x1FE807F      //数字1
#define NUM_2 0x1FE40BF      //数字2
#define NUM_3 0x1FEC03F      //数字3
#define NUM_4 0x1FE20DF      //数字4
#define NUM_5 0x1FEA05F      //数字5
#define NUM_6 0x1FE609F      //数字6
#define NUM_7 0x1FEE01F      //数字7
#define NUM_8 0x1FE10EF      //数字8
#define NUM_9 0x1FE906F      //数字9
#define SSID "EVA-AL10"
#define PASSWORD "*******" // :)
#define HOST_NAME "**.**.**.**" // :)
//我无意中看了下apache的日志，各种扫描简直可怕
#define HOST_PORT 80

IRrecv irrecv(RECV_PIN);
decode_results results;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE); 
SoftwareSerial mySerial(2, 3);  // 对于Core必须使用软串口进行WIFI模块通信
ESP8266 wifi(mySerial);

uint8_t mode=0,looptime=0;//档
const int R[3]={220,1000,10000};
float val=0,rx=0;
// int val=0,rx=0;
char serialbuffer[150];  //用于从串口监视器发送指令
String HTTP="GET /wx/b.php?content=";
char data_str[30];//电压转字符
static unsigned char ohmlogo[]=
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xF0,0x07,0x00,0x00,0x3C,0x1C,0x00,0x00,0x0E,0x38,0x00,
0x00,0x0E,0x78,0x00,0x00,0x07,0x70,0x00,0x00,0x07,0xF0,0x00,0x80,0x07,0xE0,0x00,
0x80,0x03,0xE0,0x00,0x80,0x03,0xE0,0x00,0x80,0x07,0xE0,0x00,0x00,0x07,0xF0,0x00,
0x00,0x07,0x70,0x00,0x00,0x0E,0x38,0x00,0x00,0x1C,0x1C,0x00,0x00,0x38,0x07,0x00,
0x80,0x31,0xC3,0x00,0x80,0x31,0x63,0x00,0x80,0x3F,0x7F,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//Ω符号的XBM信息
static unsigned char Alogo[]=
{0x00,0x00,0x00,0x08,0x08,0x18,0x14,0x14,0x24,0x3C,0x22,0x42,0x42,0xE7,0x00,0x00};
static unsigned char Blogo[]=
{0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x1E,0x22,0x42,0x42,0x42,0x22,0x1F,0x00,0x00};
static unsigned char Clogo[]=
{0x00,0x00,0x00,0x7C,0x42,0x42,0x01,0x01,0x01,0x01,0x01,0x42,0x22,0x1C,0x00,0x00};
static unsigned char Dlogo[]=
{0x00,0x00,0x00,0x1F,0x22,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x22,0x1F,0x00,0x00};
/*
inline void retrievalData(){    //  显示WIFI模块信息,直接输出到Serial
	String dataToRead;
	// Serial.println("receive");
	if (esp8266Serial.available() > 0) {
		// Serial.println("hehe");
		dataToRead = esp8266Serial.readStringUntil('\n');
	}
	if(dataToRead.length()>0) {
		Serial.println(dataToRead);
		dataToRead="";
	}
	return;
}*/
/*
inline void commondFromSerial(){  //  发送AT指令
    if (Serial.available() > 0){
		//read from serial until terminating character
		int len = Serial.readBytesUntil('\n', serialbuffer, sizeof(serialbuffer));
		//trim buffer to length of the actual message
		//String message = String(serialbuffer).substring(0,len-1);
		String message = String(serialbuffer).substring(0,len);
		Serial.println("message: " + message);
		esp8266Serial.println(message); 
	}
	return;
}*/
inline void esp8266Init() {
	/*use serial
	esp8266Serial.begin(9600);//connection to ESP8266
	esp8266Serial.println("AT+CIPMUX=1");
	*/
	/*use wifi object*/
	if (wifi.setOprToStationSoftAP())Serial.print("to station + softap ok\r\n");
	else Serial.print("to station + softap err\r\n");
	if (wifi.joinAP(SSID,PASSWORD)){      //加入无线网
		Serial.print("Join AP success\r\n");  
		Serial.print("IP: ");
		Serial.println(wifi.getLocalIP().c_str());
	}else Serial.print("Join AP failure\r\n");
	if(wifi.disableMUX())Serial.print("single ok\r\n");
	else Serial.print("single err\r\n");
	Serial.print("setup end\r\n");
	//把串口信息去掉应当能提高运行效率，我就省事不去了
}
inline void draw(){
	//setFont_SS;u8g.setPrintPos(0,15);
	// char s[30];
	// switch(mode){
		// case 0:sprintf(s,"Voltmeter,0-5V");break;
		// case 1:sprintf(s,"Ohmmeter,small range");break;
		// case 2:sprintf(s,"Ohmmeter,medium range");break;
		// case 3:sprintf(s,"Ohmmeter,wide range");break;
	// }
	// u8g.print(s);
	//用上面的小字体程序空间不太够，所以改为下面的XBM
	switch(mode){
		case 0:u8g.drawXBM(0,0,8,16,Alogo);break;
		case 1:u8g.drawXBM(0,0,8,16,Blogo);break;
		case 2:u8g.drawXBM(0,0,8,16,Clogo);break;
		case 3:u8g.drawXBM(0,0,8,16,Dlogo);break;
	}
	char s[40];
	u8g.setFontPosCenter();setFont_LL;u8g.setPrintPos(5,50);
	if(!mode){//电压表
		sprintf(s,"U=       V",val);
		u8g.print(s);
		u8g.setPrintPos(50,50);
		u8g.print(val);
	}else{
		sprintf(s,"R=   ",rx);
		u8g.print(s);
		//精度可喜，所以作以下处理
		//科学计数法保留一位有效数字，sprintf处理%e不大能行，所以手写，有点怪
		int tmp=(int)rx;
		int base=0,exp=0;
		while(tmp){
			base=tmp;
			exp++;
			tmp/=10;
		}
		if(exp)exp--;
		long hehe=1;
		// Serial.print("base=");Serial.println(base);
		for(int i=0;i<exp;i++)hehe*=10;
		if((rx-hehe)>(hehe/2))base++;
		if(base==10){base=1;exp++;}
		if(base<0)base=0;
		// Serial.print("base=");Serial.println(base);Serial.print("exp=");Serial.println(exp);
		u8g.setPrintPos(50,50);
		u8g.print(base);
		u8g.setPrintPos(65,50);
		u8g.print("e");
		u8g.setPrintPos(78,50);
		u8g.print(exp);
		u8g.drawXBM(90,34,30,30,ohmlogo);
	}
}
inline void default_screen(){
    u8g.firstPage();
    do draw();while(u8g.nextPage());
}
inline void sendCurrentData(){
	if(wifi.createTCP(HOST_NAME, HOST_PORT)){ //建立TCP连接，如果失败，不能发送该数据
		Serial.print("create tcp ok\r\n");
		HTTP="GET /wx/b.php?content=";
		dtostrf(val,10,4,data_str);
		uint8_t pos=0;
		while(data_str[pos]==' ')pos++;
		HTTP+=(data_str+pos);
		HTTP+="&mode=";
		HTTP+=('0'+mode);
		HTTP+=" HTTP/1.0\r\n\r\n";
		const char *postArray = HTTP.c_str();                 //将str转化为char数组
		Serial.println(postArray);
		wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
		Serial.println("send success"); 
		// if (wifi.releaseTCP())Serial.print("release tcp ok\r\n"); 
	}

	/*use Serial
	esp8266Serial.println("AT+CIPSTART=4,\"TCP\",\"123.206.93.234\",80");
	delay(300);	
	HTTP="GET /wx/b.php?content=";
	dtostrf(val,10,4,data_str);
	uint8_t pos=0;
	while(data_str[pos]==' ')pos++;
	HTTP+=(data_str+pos);
	HTTP+="&mode=";
	HTTP+=('0'+mode);
	HTTP+=" HTTP/1.0\r\n\r\n";
	char HTTPcommand[30];
	sprintf(HTTPcommand,"AT+CIPSEND=4,%d",HTTP.length());
	esp8266Serial.println(HTTPcommand);
	delay(300);		
	esp8266Serial.println(HTTP);
	delay(300);
	*/
}
inline void updatemode(){//实时性极差，建议使用外部中断
	Serial.println("updatingmode");
	if(irrecv.decode(&results)){
		Serial.println("in IF");
		switch(results.value){
			case NUM_1:mode=0;Serial.println("setmode to 0");break;
			case NUM_2:mode=1;Serial.println("setmode to 1");break;
			case NUM_3:mode=2;Serial.println("setmode to 2");break;
			case NUM_4:mode=3;Serial.println("setmode to 3");break;
		}
		Serial.println(mode);
	}
	irrecv.resume(); // 接收下一个值
	return;
}
void setup(){
	Serial.begin(9600);
	esp8266Init();//WIFI初始化
	irrecv.enableIRIn(); // 红外初始化
}
void loop(){
	looptime++;looptime%=5;//每读取五个数据发送一次
	updatemode();//根据红外接收更新模式
	int _read=analogRead(readPIN);
	val=(float)_read/1023*5;//A0处电压
	rx=val*R[mode-1]/(5-val);//计算电阻
	if(rx<0)rx=0;
	if(!looptime)sendCurrentData();
	default_screen();//LED显示
	delay(200);
}