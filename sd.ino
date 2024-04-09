#include <SPI.h>

class sd {
  public:
    void powerUp();
    void send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc);
    uint8_t response_1(uint8_t buf);
    void response_3and7(uint8_t *buf);
};

void sd::powerUp(){
  // Begin SPI Configuration
  SPI.begin();
  SPI.beginTransaction(SPISettings(250000, MSBFIRST, SPI_MODE0));
  //waiting for 1 ms and sending 80 clock cycle to sd card to power up
  // and synchronise 
  delay(1);
  for(int i = 0; i < 10; i++){
    SPI.transfer(0xFF);
  }
}

void sd::send_cmd(uint8_t cmd, uint32_t arg, uint8_t crc){
  SPI.transfer(cmd | 0x40);
  SPI.transfer(arg >> 24);
  SPI.transfer(arg >> 16);
  SPI.transfer(arg >> 8);
  SPI.transfer(arg);
  SPI.transfer(crc);
  SPI.transfer(0xFF);
}

uint8_t sd::response_1(uint8_t buf){
  return buf = SPI.transfer(0xFF);
}

void sd::response_3and7(uint8_t *buf){
  buf[0] = SPI.transfer(0xFF);
  buf[1] = SPI.transfer(0xFF);
  buf[2] = SPI.transfer(0xFF);
  buf[3] = SPI.transfer(0xFF);
  buf[4] = SPI.transfer(0xFF);
}

uint8_t res_7 [5];
uint8_t res_3 [5];
uint8_t res512 [512];
uint8_t r1 = 0;
sd mySD;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  digitalWrite(10,HIGH);
  mySD.powerUp();
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(10,LOW);
  // CMD0
  while(r1 != 0x01){
    mySD.send_cmd(0, 0, 0x95);
    r1 = mySD.response_1(r1);
  }
  // CMD8
  mySD.send_cmd(8, 0x01AA, 0x86);
  mySD.response_3and7(res_7);
  Serial.println(res_7[0],HEX);
  Serial.println(res_7[1],HEX);
  Serial.println(res_7[2],HEX);
  Serial.println(res_7[3],HEX);
  Serial.println(res_7[4],HEX);
  // CMD58
  mySD.send_cmd(58, 0, 0);
  mySD.response_3and7(res_3);
  Serial.println(res_3[0],HEX);
  Serial.println(res_3[1],HEX);
  Serial.println(res_3[2],HEX);
  Serial.println(res_3[3],HEX);
  Serial.println(res_3[4],HEX);

  r1 = 0xFF;
  while(r1 & B00000001){
    // CMD55
    mySD.send_cmd(55, 0, 0);
    mySD.response_1(r1);
    // ACMD41
    mySD.send_cmd(0x41, 0x40000000, 0);
    r1 = mySD.response_1(r1);
  }

  // CMD58
  mySD.send_cmd(58, 0, 0);
  mySD.response_3and7(res_3);
  Serial.println(res_3[0],HEX);
  Serial.println(res_3[1],HEX);
  Serial.println(res_3[2],HEX);
  Serial.println(res_3[3],HEX);
  Serial.println(res_3[4],HEX);

  // CMD24 write 1 block
  mySD.send_cmd(24, 0, 0);
  r1 = mySD.response_1(r1);
  while(r1 != 0){
  r1 = mySD.response_1(r1);
  }
  SPI.transfer(0xFE);
  for(int i = 0; i< 512; i++){
    SPI.transfer(0xAA);
  }
  
  while((r1 = mySD.response_1(r1)) == 0xFF){}
  Serial.println(r1);
  while((r1 = mySD.response_1(r1)) == 0){}
  
  // CMD13 send status
  /*SPI.transfer(0x4D);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  SPI.transfer(0xFF);
  res_2[0] = SPI.transfer(0xFF);
  Serial.println(res_2[0],HEX);
  res_2[1] = SPI.transfer(0xFF);
  Serial.println(res_2[1],HEX);*/

  // CMD17 Read 1 block
  mySD.send_cmd(17, 0, 0);
  r1 = mySD.response_1(r1);
  delay(100);
  while(r1 != 0xFE){
  r1 = mySD.response_1(r1);
  }
  for(int i = 0; i< 512; i++){
    res512[i] = SPI.transfer(0xFF);
  }
  uint8_t crc1 = SPI.transfer(0xFF);
  uint8_t crc2 = SPI.transfer(0xFF);
  for(int i = 0; i< 512; i++){
    Serial.println(res512[i]);
  }
  delay(5000);
}

 
