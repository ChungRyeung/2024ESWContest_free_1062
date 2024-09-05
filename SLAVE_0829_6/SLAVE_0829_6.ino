#include <LedControl.h>
#include <Servo.h>
#include <DFRobot_DHT11.h>
#include <Wire.h>

DFRobot_DHT11 DHT;
Servo myservo;
LedControl lc=LedControl(5,7,6,1);

#define flash_pin 3 //digital D0
#define co_pin 3 //analog A
#define DHT11_PIN 4 //digital DATA
#define servo_pin 8 //digital PWM
#define laser_pin 9 //digital S
#define dot_din 5 //digital DIN
#define dot_c5 6 //digital C5
#define dot_clk 7 //digital CLK

#define slave_address 6 //scl sda address num

int flame_state = 0;
int gas_rate = 0;
int temp = 0;
int humi = 0;

int angle=180;

unsigned long info = 0;
byte buffer[4];


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(flash_pin,INPUT);

  pinMode(co_pin,INPUT);

  pinMode(laser_pin, OUTPUT);

  myservo.attach(servo_pin);

  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);

  Wire.begin(slave_address);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  flame_state = digitalRead(flash_pin);
  flame_state=1-flame_state;

  gas_rate = analogRead(co_pin);

  DHT.read(DHT11_PIN);
  temp=DHT.temperature;
  humi=DHT.humidity;

  if(flame_state==1 || angle!=10)
  {
  myservo.write(angle);
  digitalWrite(laser_pin,HIGH);
  writeNodeOnMatrix();
  }
  else
  {
  digitalWrite(laser_pin,LOW);
  off_Matrix();
  }

  Serial.print(flame_state);
  Serial.print(" ");
  Serial.print(gas_rate);
  Serial.print(" ");
  Serial.print(temp);
  Serial.print(" ");
  Serial.print(humi);
  Serial.println(" ");

  info = (unsigned long)gas_rate * 100000 + (unsigned long)flame_state * 10000 + (unsigned long)(temp * 100) + (unsigned long)humi;

  Serial.print("Info: ");
  Serial.println(info);

    for (int i = 0; i < 4; i++) {
    buffer[i] = (info >> (i * 8)) & 0xFF;
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  delay(1000);
}

void writeNodeOnMatrix() {
  byte one[] = {
    B00000000,
    B00011100,
    B00010000,
    B00010000,
    B00011100,
    B00010100,
    B00011100,
    B00000000};
  lc.setRow(0,0,one[0]);
  lc.setRow(0,1,one[1]);
  lc.setRow(0,2,one[2]);
  lc.setRow(0,3,one[3]);
  lc.setRow(0,4,one[4]);
  lc.setRow(0,5,one[5]);
  lc.setRow(0,6,one[6]);
  lc.setRow(0,7,one[7]);
}

void off_Matrix() {
  byte one[] = {
  B00000000,
};
  lc.setRow(0,0,one[0]);
  lc.setRow(0,1,one[0]);
  lc.setRow(0,2,one[0]);
  lc.setRow(0,3,one[0]);
  lc.setRow(0,4,one[0]);
  lc.setRow(0,5,one[0]);
  lc.setRow(0,6,one[0]);
  lc.setRow(0,7,one[0]);
}

void requestEvent() {
  Wire.write(buffer, sizeof(buffer)); // 바이트 배열 전송
}

void receiveEvent(int howMany) { //전송 데이터 읽기
  while (Wire.available()>0) { 
    angle = Wire.read(); 
    Serial.println(angle);         
  }      
}