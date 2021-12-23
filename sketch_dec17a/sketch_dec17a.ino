/*
* 17.12.2021
* Новиков С. В.
* nsv47nsv36@yandex.ru
* программа для управления лазерным источником типа Raycus RFL-P30Q
* необходимо установить 
*/

//#include <SPI.h>
//#include <UIPEthernet.h>

#include <SoftwareSerial.h>

//#define SERV_PORT 2000 // порт сервера

//18296 1412

//Порт подключенный к ST_CP 74HC595
uint8_t latchPin = 2;
//Порт подключенный к SH_CP  74HC595
uint8_t clockPin = 3;
//Порт подключенный к DS 74HC595
uint8_t dataPin = 6;

uint8_t portLatch_9 = 18;
uint8_t portEnableEmission_18 = 7;
uint8_t portEnableModulation_19 = 8;
uint8_t portPulseRepetitiveRateSignal_20 = 9;
uint8_t portRed_22 = 15;
uint8_t portEmStop_23 = 16;

uint8_t portCool = 14;
uint8_t portPowerSupply = 17;

// 11 12 16 21

//uint8_t portAlarmPin11 = 8;
//uint8_t portAlarmPin12 = 9;
//uint8_t portAlarmPin16 = 10;
//uint8_t portAlarmPin21 = 11;

unsigned long timerSwitchWhile = 0;
unsigned long previousMillis_1 = 0;
unsigned long previousMillis_2 = 0;

// 18304 1412

bool statePortLatch_9 = LOW;
bool statePortEnableEmission_18 = LOW;
bool statePortEnableModulation_19 = LOW;
bool statePortRed_22 = LOW;
bool statePortEmStop_23 = HIGH;

bool statePortCool = LOW;
bool statePortPowerSupply = LOW; 
 
// определяем конфигурацию сети
byte mac[] = {0xAE, 0xB2, 0x26, 0xE4, 0x4A, 0x5C}; // MAC-адрес
byte ip[] = {192, 168, 1, 100}; // IP-адрес

char receivingBuffer[100]; // приемный буфер Udp-пакета
//EthernetUDP udp; // создаем экземпляр Udp

const uint8_t pinRX = 5;
const uint8_t pinTX = 4;

SoftwareSerial softSerial(pinRX,pinTX);

uint8_t powerLaser = 10;
uint8_t frequencyLaser = 30;
uint32_t durationOfActivationLaser = 1000; 

uint32_t reqfreq=30000; //переменная запроса частоты

/*
void setting_the_pulse_repetition_frequency(){
	uint32_t ocr=OCR1A;  uint16_t divider=1;  float freq; 
	//reqfreq = Serial.parseInt(); 
	if (reqfreq==0 || reqfreq>F_CPU/2) {return;}
	ocr = (F_CPU / reqfreq /2 /divider); 
	byte shifts[] = {3,3,2,2};
	for(byte i = 0; i < 4; i++){
		if (ocr > 65536) { divider <<= shifts[i];
			ocr = F_CPU / reqfreq /2 /divider; }
		else { TCCR1B = (i+1)|(1<<WGM12);  break; }  } //Mode4 (CTC)
	OCR1A=ocr-1; TCCR1A=1<<COM1A0;
    freq= (float) F_CPU/2 / (OCR1A+1) /divider;
	if (freq <10000) { 
		//Serial.print(freq,1);
		//Serial.println(" Hz "); 
	}
	if (freq >=10000) { 
		//Serial.print(freq/1000,3);
		//Serial.println(" kHz");
	}
}
*/

void setting_the_pump_level_code(){
	//for (int j = 0; j < 256; j++) {
    //устанавливаем LOW на latchPin пока не окончена передача байта
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, powerLaser);  
    //устанавливаем HIGH на latchPin, чтобы проинформировать регистр, что передача окончена.
    digitalWrite(latchPin, HIGH);
    delay(10);
	digitalWrite(portLatch_9, !statePortLatch_9);
	delay(10);
	digitalWrite(portLatch_9, statePortLatch_9);
}

void turning_on_the_radiation(){
	setting_the_pump_level_code();
	
	digitalWrite(portEnableEmission_18, HIGH);
	delay(10);
	digitalWrite(portEnableModulation_19, HIGH);
}

void turning_off_the_radiation(){
	digitalWrite(portEnableModulation_19, LOW);
	digitalWrite(portEnableEmission_18, LOW);
}

/*
void ethernet_control(){
	int size = udp.parsePacket(); // считываем размер принятого пакета
  
  if (size) {
    // есть новый пакет
    udp.read(receivingBuffer, size); // чтение Udp-пакета
	
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
*/

void controlFromTheDisplay_debug(){
	if(Serial.available()>0){         // Если есть данные принятые от дисплея, то ...
		
		uint8_t cmd[20];
		
		uint8_t pos = 0;
		while(Serial.available()){
			cmd[pos++] = Serial.read();
			delay(10);
		}
		cmd[pos] = char(0);
		                
		for(uint8_t j=0; j<pos; j++){
			Serial.print(char(cmd[j]));
		}
		Serial.println();
		for(uint8_t i=0; i<pos; i++){
		
			if(memcmp(&cmd[i],"n0" , 2)==0){ 
				i+=5; 
				
				powerLaser = cmd[i-3];
				Serial.print(F("powerLaser: "));
				Serial.println(powerLaser);
			}else 
			if(memcmp(&cmd[i],"n1", 2)==0){
				i+=5; 
				
				frequencyLaser = cmd[i-3];
				reqfreq = frequencyLaser*1000L;
 				Serial.print(F("frequencyLaser: "));
				Serial.println(reqfreq);
			}else 
			if(memcmp(&cmd[i],"n2", 2)==0){
				i+=5; 
				
				durationOfActivationLaser = cmd[i-3];
				Serial.print(F("durationOfActivationLaser: "));
				Serial.print(durationOfActivationLaser);
			}else 
			if(memcmp(&cmd[i],"enableRed", 9)==0){
				i+=8; 
				
				digitalWrite(portRed_22, HIGH);
			}else
			if(memcmp(&cmd[i],"disableRed", 10)==0){
				i+=9; 
				
				digitalWrite(portRed_22, LOW);
			}else	
			if(memcmp(&cmd[i],"cool", 4)==0){
				i+=6; 
				
				statePortCool = !statePortCool;
				digitalWrite(portCool, statePortCool);
			}else	
			if(memcmp(&cmd[i],"enableRadiation", 15)==0){
				i+=14; 
				
				turning_on_the_radiation();
			}else	
			if(memcmp(&cmd[i],"disableRadiation", 16)==0){
				i+=15; 
				
				turning_off_the_radiation();
			}else	
			if(memcmp(&cmd[i],"switchWhile", 11)==0){
				i+=10; 
				
				timerSwitchWhile = durationOfActivationLaser*1000;
				turning_on_the_radiation();
			}else	
			if(memcmp(&cmd[i],"powerSupply", 11)==0){
				i+=10; 
				
				statePortPowerSupply = !statePortPowerSupply;
				digitalWrite(portPowerSupply, statePortPowerSupply);
			}
		}
	}
}

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
		
			if(memcmp(&cmd[i],"n0" , 2)==0){ 
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
				
				//statePortRed_22 = !statePortRed_22;
				//digitalWrite(portRed_22, statePortRed_22);
			}else
			if(memcmp(&cmd[i],"cool", 4)==0){
				i+=6; 
				
				statePortCool = !statePortCool;
				digitalWrite(portCool, statePortCool);
			}else	
			if(memcmp(&cmd[i],"enableLaser", 11)==0){
				i+=10; 
				
				
			}else	
			if(memcmp(&cmd[i],"disableLaser", 12)==0){
				i+=11; 
				
				//statePortEnableEmission_18 = false;
				//digitalWrite(portEnableEmission_18, statePortEnableEmission_18);
			}else	
			if(memcmp(&cmd[i],"switchWhile", 11)==0){
				i+=10; 
				
				//statePortEnableEmission_18 = true;
				//digitalWrite(portEnableEmission_18, statePortEnableEmission_18);
				timerSwitchWhile = durationOfActivationLaser;
			}else	
			if(memcmp(&cmd[i],"powerSupply", 11)==0){
				i+=10; 
				
				statePortPowerSupply = !statePortPowerSupply;
				digitalWrite(portPowerSupply, statePortPowerSupply);
			}
		}
	}
}	
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

/*
* uint8_t portEnableEmission_18 = 7;
* uint8_t portEnableModulation_19 = 8;
* uint8_t portPulseRepetitiveRateSignal_20 = 9;
* uint8_t portEmStop_23 = 16;
*/

void setup() {
		
	Serial.begin(9600);
	softSerial.begin(9600);
	
  //устанавливаем режим порта выхода
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  //Ethernet.begin(mac, ip); // инициализируем контроллер
  //udp.begin(SERV_PORT); // включаем прослушивание порта
  
  pinMode(portEnableEmission_18, OUTPUT);
  pinMode(portEnableModulation_19, OUTPUT);
  pinMode(portPulseRepetitiveRateSignal_20, OUTPUT); // выход генератора
  pinMode(portRed_22, OUTPUT);
  pinMode(portEmStop_23, OUTPUT);
  
  digitalWrite(portEnableEmission_18, LOW);
  digitalWrite(portEnableModulation_19, LOW);
  digitalWrite(portRed_22, LOW);
  digitalWrite(portEmStop_23, HIGH);
  
  pinMode(portCool, OUTPUT);
  //portPowerSupply
  pinMode(portPowerSupply, OUTPUT);
  //pinMode(portAlarmPin11, INPUT_PULLUP);
  //pinMode(portAlarmPin12, INPUT_PULLUP);
  //pinMode(portAlarmPin16, INPUT_PULLUP);
  //pinMode(portAlarmPin21, INPUT_PULLUP);
  
  TCCR1A=0;TCCR1B=0;
}
 
void loop() {
  
  if(previousMillis_1>=4294967295){
    previousMillis_1 = 0;
  }
  if(previousMillis_2>=4294967295){
    previousMillis_2 = 0;
  }
  /*
  int size = udp.parsePacket(); // считываем размер принятого пакета
  
  if (size) {
    // есть новый пакет
    udp.read(receivingBuffer, size); // чтение Udp-пакета
  }
  */
  controlFromTheDisplay();
  controlFromTheDisplay_debug();
  
  if(timerSwitchWhile<=0){
	turning_off_the_radiation();
  }
  
  if(millis()- previousMillis_1 >= 1000){
	
	timerSwitchWhile -= 1000;
	previousMillis_1 = millis();
  }
  
  if(millis()-previousMillis_2 >= 1000){
	//alarm_signal();
	previousMillis_2 = millis();
  }
  
	uint32_t ocr=OCR1A;  uint16_t divider=1;  float freq; 
	//reqfreq = Serial.parseInt(); 
	if (reqfreq==0 || reqfreq>F_CPU/2) {return;}
	ocr = (F_CPU / reqfreq /2 /divider); 
	byte shifts[] = {3,3,2,2};
	for(byte i = 0; i < 4; i++){
		if (ocr > 65536) { divider <<= shifts[i];
			ocr = F_CPU / reqfreq /2 /divider; }
		else { TCCR1B = (i+1)|(1<<WGM12);  break; }  } //Mode4 (CTC)
	OCR1A=ocr-1; TCCR1A=1<<COM1A0;
    freq= (float) F_CPU/2 / (OCR1A+1) /divider;
	if (freq <10000) { 
		//Serial.print(freq,1);
		//Serial.println(" Hz "); 
	}
	if (freq >=10000) { 
		//Serial.print(freq/1000,3);
		//Serial.println(" kHz");
	}
  
}
