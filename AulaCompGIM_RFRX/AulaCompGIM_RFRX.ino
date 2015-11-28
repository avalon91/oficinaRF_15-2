#include <SPI.h>
#include "RF24.h"
#include "nRF24L01.h"
#include "printf.h"

RF24 radio(9,10);

const uint64_t pipes[2] = {0x78787878A3LL, 0x78787878E1LL};

boolean sucesso = false;
int array[4];
int canal = 120;
char papel = 'R'; //T = TX; R = RX

void setup() {
  Serial.begin(57600);
  printf_begin();
  radio.begin();
  radio.setChannel(canal);
  radio.setRetries(15,15);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.setPayloadSize(16);
  if(papel == 'T'){
    radio.openReadingPipe(1,pipes[0]);
    radio.openWritingPipe(pipes[1]);
  }
  if(papel == 'R'){
    radio.openReadingPipe(1,pipes[1]);
    radio.openWritingPipe(pipes[0]);
  }
  radio.startListening();  
  radio.printDetails();
}

void loop() {
  recebe();
}

void recebe(){
  char c = 'b';
  if(radio.available()){
    boolean concl = false;
    while(!concl){
      concl = radio.read(&array, sizeof(array));
      for(int i=0; i<4; i++){
        Serial.print(array[i]);
        Serial.println();
      }
      Serial.println();
    }
    radio.stopListening();
    radio.write(&c, sizeof(char));
    radio.startListening();
  }
}

void envio(){
  radio.stopListening();
  sucesso = radio.write(&array, sizeof(array));
  if(sucesso == true){
    Serial.print("Enviado com sucesso!");
  } else{
    Serial.print("Falha no envio.");
  }
  radio.startListening();
  
  unsigned long esperando = millis();
  boolean timeout = false;
  while(!radio.available() && !timeout){
    if(millis() - esperando > 250){
      timeout = true;
    }
  }
  if(timeout){
    Serial.println(" Falha no retorno.");
  } else{
    Serial.println(" Sucesso no retorno!");
    char peguei;
    radio.read(&peguei, sizeof(char));
    //Serial.println(peguei);
  }  
}
