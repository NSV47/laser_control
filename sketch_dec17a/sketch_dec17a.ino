/*
* 17.12.2021
* Новиков С. В.
* nsv47nsv36@yandex.ru
* программа для управления лазерным источником типа Raycus RFL-P30Q
* необходимо установить 
* 21.12.21
* /*
 * создано 13.12.21
 * За основу взят материал из папки ENC28j60 на Yandex.Disc
 * не получалось поддключить плату ENC28j60 пока не запитал ее от 5 вольт.
 * тестировалось с tcp/ip builder. Данные в обе стороны, но при отправке из UART не чистится буфер?
 * занимает много места. Удалить лишний код из библиотеки.
 * при отправке из tcp/ip builder единицы поднимается вторая ножка, при отправке 0 отпускается.
 * не работает чтение принятых данных.
*/


#include <SPI.h>
#include <UIPEthernet.h>

//#include <SoftwareSerial.h>

#define SERV_PORT 51000 // порт сервера

//18296 1412

//Порт подключенный к ST_CP 74HC595
uint8_t latchPin = 2;
//Порт подключенный к SH_CP  74HC595
uint8_t clockPin = 3;
//Порт подключенный к DS 74HC595
uint8_t dataPin = 4;
 
uint8_t portRed = 5;

uint8_t portEmission = 6;

uint8_t portCool = 7;

// 11 12 16 21

uint8_t portAlarmPin11 = 8;
uint8_t portAlarmPin12 = 9;
//uint8_t portAlarmPin16 = 10;
//uint8_t portAlarmPin21 = 11;

unsigned long timerSwitchWhile = 0;
unsigned long previousMillis_1 = 0;
unsigned long previousMillis_2 = 0;

// 18304 1412

bool statePortRed = false;
bool statePortEmission = false;
bool statePortCool = false;
 
// определяем конфигурацию сети
byte mac[] = {0xAE, 0xB2, 0x26, 0xE4, 0x4A, 0x5C}; // MAC-адрес
byte ip[] = {192, 168, 1, 10}; // IP-адрес
byte ipServ[] = {192, 168, 1, 2}; // IP-адрес сервера

char receivingBuffer[100]; // приемный буфер Udp-пакета
char transmitBuffer[100];
EthernetUDP udp; // создаем экземпляр Udp

const uint8_t pinRX = 5;
const uint8_t pinTX = 4;

//SoftwareSerial softSerial(pinRX,pinTX);

uint8_t powerLaser = 10;
uint8_t frequencyLaser = 30;
uint32_t durationOfActivationLaser = 1000; 

void ethernet_control(){
	int size = udp.parsePacket(); // считываем размер принятого пакета
  
  if (size) {
    // есть новый пакет
    udp.read(receivingBuffer, size); // чтение Udp-пакета
	
	 // есть пакет Udp, выводим информацию о пакете
	  Serial.print("Received packet from ");
	  IPAddress ipFrom = udp.remoteIP();
	  Serial.println(ipFrom);
	  Serial.print("Size ");
	  Serial.print(size);
	  Serial.print(", port ");
	  Serial.println(udp.remotePort());
	
	//-----Отправляю в ответ время----------------------
	udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.println(millis());
    udp.endPacket();
	//--------------------------------------------------
	
	/*
    // управление светодиодом
    if(receivingBuffer[0] == '0') digitalWrite(4, LOW); // если пришел 0, гасим светодиод
    if(receivingBuffer[0] == '1') {
      digitalWrite(4, HIGH); // если пришла 1, зажигаем светодиод
      timer = 5;
    }
    receivingBuffer[0]=0;

    // посылка клиенту в ответ времени
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.println(millis());
    udp.endPacket();
	*/
	
	for(uint8_t j=0; j<size; j++){
			Serial.print(receivingBuffer[j]);
		}
		Serial.println();
		
	for(uint8_t i=0; i<size; i++){
		
			if(memcmp(&receivingBuffer[i],"n0" , 2)==0){ // Если в строке str начиная с символа i находится текст "movingUp",  значит кнопка дисплея была включена
				i+=1; 
				
				powerLaser = receivingBuffer[i-3];
				Serial.print(F("powerLaser: "));
				Serial.println(powerLaser);
			}else 
			if(memcmp(&receivingBuffer[i],"n1", 2)==0){
				i+=1; 
				
				frequencyLaser = receivingBuffer[i-3];
				Serial.print(F("frequencyLaser: "));
				Serial.print(frequencyLaser);
			}
	}
  }
}
/*
void controlFromTheDisplay(){
	if(softSerial.available()>0){         // Если есть данные принятые от дисплея, то ...
		
		uint8_t cmd[15];
		
		uint8_t pos = 0;
		while(softSerial.available()){
			cmd[pos++] = softSerial.read();
			delay(10);
		}
		cmd[pos] = char(0);
		                
		for(uint8_t j=0; j<pos; j++){
			Serial.print(cmd[j]);
		}
		Serial.println();
		for(uint8_t i=0; i<pos; i++){
		
			if(memcmp(&cmd[i],"n0" , 2)==0){ // Если в строке str начиная с символа i находится текст "movingUp",  значит кнопка дисплея была включена
				i+=1; 
				
				powerLaser = cmd[i-3];
				Serial.print(F("powerLaser: "));
				Serial.println(powerLaser);
			}else 
			if(memcmp(&cmd[i],"n1", 2)==0){
				i+=1; 
				
				frequencyLaser = cmd[i-3];
				Serial.print(F("frequencyLaser: "));
				Serial.print(frequencyLaser);
			}else 
			if(memcmp(&cmd[i],"x0", 2)==0){
				i+=1; 
				
				durationOfActivationLaser = cmd[i-3];
				Serial.print(F("durationOfActivationLaser: "));
				Serial.print(durationOfActivationLaser);
			}else 
			if(memcmp(&cmd[i],"red", 3)==0){
				i+=2; 
				
				statePortRed = !statePortRed;
				digitalWrite(portRed, statePortRed);
			}else
			if(memcmp(&cmd[i],"cool", 4)==0){
				i+=6; 
				
				statePortCool = !statePortCool;
				digitalWrite(portCool, statePortCool);
			}else	
			if(memcmp(&cmd[i],"emission_ON", 11)==0){
				i+=10; 
				
				statePortEmission = true;
				digitalWrite(portEmission, statePortEmission);
			}else	
			if(memcmp(&cmd[i],"emission_OFF", 12)==0){
				i+=11; 
				
				statePortEmission = false;
				digitalWrite(portEmission, statePortEmission);
			}else	
			if(memcmp(&cmd[i],"switchWhile", 11)==0){
				i+=10; 
				
				statePortEmission = true;
				digitalWrite(portEmission, statePortEmission);
				timerSwitchWhile = durationOfActivationLaser;
			}
		}
	}
}
*/
	
// 63488 красный
// 2024 зеленый
// softSerial.print((String)"t0.txt=\""+analogRead(pinR)+"\""+char(255)+char(255)+char(255));
// 0 перегрев
// 001 нормальное состояние
// 010 отражение
// 011 генератор
// 100 питание
// 101
// 110
// 111

/*
void setIndicationAlarm(uint8_t alarm){
	if(alarm==0){
		softSerial.print((String)"r0.pco=63488"+char(255)+char(255)+char(255));
		softSerial.print((String)"t3.txt=\"temper\""+char(255)+char(255)+char(255));
	}else
	if(alarm==1){
		softSerial.print((String)"r0.pco=2024"+char(255)+char(255)+char(255));
		softSerial.print((String)"t3.txt=\"normal\""+char(255)+char(255)+char(255));
	}else
	if(alarm==10){
		softSerial.print((String)"r0.pco=63488"+char(255)+char(255)+char(255));
		softSerial.print((String)"t3.txt=\"mirror\""+char(255)+char(255)+char(255));
	}else
	if(alarm==11){
		softSerial.print((String)"r0.pco=63488"+char(255)+char(255)+char(255));
		softSerial.print((String)"t3.txt=\"gener\""+char(255)+char(255)+char(255));
	}else
	if(alarm==100){
		softSerial.print((String)"r0.pco=63488"+char(255)+char(255)+char(255));
		softSerial.print((String)"t3.txt=\"voltage\""+char(255)+char(255)+char(255));
	}else
	if(alarm==101){
		softSerial.print((String)"r0.pco=0"+char(255)+char(255)+char(255));
		softSerial.print((String)"t3.txt=\"reserve\""+char(255)+char(255)+char(255));
	}else
	if(alarm==110){
		softSerial.print((String)"r0.pco=0"+char(255)+char(255)+char(255));
		softSerial.print((String)"t3.txt=\"reserve\""+char(255)+char(255)+char(255));
	}else
	if(alarm==111){
		softSerial.print((String)"r0.pco=0"+char(255)+char(255)+char(255));
		softSerial.print((String)"t3.txt=\"reserve\""+char(255)+char(255)+char(255));
	}
}
*/

/*
void alarm_signal(){
	char statePortAlarmPin11 = digitalRead(portAlarmPin11);
	char statePortAlarmPin12 = digitalRead(portAlarmPin12);
	char statePortAlarmPin16 = digitalRead(portAlarmPin16);
	char statePortAlarmPin21 = digitalRead(portAlarmPin21);
	
	uint8_t alarm = statePortAlarmPin11*1000+statePortAlarmPin12*100+statePortAlarmPin16*10+statePortAlarmPin21;
	
	switch(alarm){
		case 0:   setIndicationAlarm(0); break;
		case 1:   setIndicationAlarm(1); break;
		case 10:  setIndicationAlarm(10); break;
		case 11:  setIndicationAlarm(11); break;
		case 100: setIndicationAlarm(100); break;
		case 101: setIndicationAlarm(100); break;
		case 110: setIndicationAlarm(110); break;
		case 111: setIndicationAlarm(111); break;
	}
}
*/

void setup() {
  //устанавливаем режим порта выхода
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  Ethernet.begin(mac, ip); // инициализируем контроллер
  udp.begin(SERV_PORT); // включаем прослушивание порта
  
  pinMode(portRed, OUTPUT);
  pinMode(portEmission, OUTPUT);
  pinMode(portCool, OUTPUT);
  
  pinMode(portAlarmPin11, INPUT_PULLUP);
  pinMode(portAlarmPin12, INPUT_PULLUP);
  //pinMode(portAlarmPin16, INPUT_PULLUP);
  //pinMode(portAlarmPin21, INPUT_PULLUP);
  
  Serial.begin(9600);
}
 
void loop() {

/*
  for (int j = 0; j < 256; j++) {
    //устанавливаем LOW на latchPin пока не окончена передача байта
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, j);  
    //устанавливаем HIGH на latchPin, чтобы проинформировать регистр, что передача окончена.
    digitalWrite(latchPin, HIGH);
    delay(1000);
  }
*/
  
  if(previousMillis_1>=4294967295){
    previousMillis_1 = 0;
  }
  if(previousMillis_2>=4294967295){
    previousMillis_2 = 0;
  }
  
  //int size = udp.parsePacket(); // считываем размер принятого пакета
  /*
  if (size) {
    // есть новый пакет
    udp.read(receivingBuffer, size); // чтение Udp-пакета
  }
  */
  
  //controlFromTheDisplay();
  ethernet_control();
  
  if(timerSwitchWhile<=0){
	statePortEmission = false;
	digitalWrite(portEmission, statePortEmission);
  }
  
  if(millis()- previousMillis_1 >= 100){
	
	timerSwitchWhile -= 100;
	previousMillis_1 = millis();
  }
  
  if(millis()-previousMillis_2 >= 1000){
	//alarm_signal();
	previousMillis_2 = millis();
  }
  
  // собираем данные из UART в пакет
	if( Serial.available() > 0 ) {
		byte i = 0;
		while(Serial.available()){
			transmitBuffer[i++] = Serial.read();
		}
	  
	  //if( transmitBuffer[i] == 10 ) {
		//transmitBuffer[i+1]= 0;
		//i=0;
		// передача UDP пакета
		udp.beginPacket(ipServ, SERV_PORT);
		udp.write(transmitBuffer);
		udp.endPacket();
		// сообщение в последовательный порт
		Serial.print("UDP-packet transmission to ");
		Serial.print(udp.remoteIP());
		Serial.print(" Port: ");
		Serial.println(udp.remotePort());
	 // }
	  //else {
		//i++;
		//if(i >= 100) i=0;
	  //}
	}
}
