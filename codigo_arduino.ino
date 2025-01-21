//====================================================================================================
//                  Software Sensor de temperatura/Umidade DHT11 e Sensor de corrente
//                  Autor: Sergio Levy Nobre dos Santos
//                             Departamento de Física - Universidade Federal do Ceará - UFC
//====================================================================================================

//********************************************************************************
//************************** INCLUDES ********************************************
//********************************************************************************

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include <Wire.h>


//********************************************************************************
//************************** DEFINES *********************************************
//********************************************************************************


boolean Esp_state=false;

// Defines the LCD's parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(9, 8, 5, 4, 3, 2);

byte symbol_grau[8] ={ B00001100,
B00010010,
B00010010,
B00001100,
B00000000,
B00000000,
B00000000,
B00000000};


// definição dos Ar-condicionados 

#define Ar1 48  
#define Ar2 50
#define Ar3 52 

// Sensores de temperatura/ umidade

#define sensor1 A0
#define sensor2 A1
#define sensor3 A2
#define sensor4 A3
#define sensor5 A4

#define TEMP_VAR 20.0
#define UMID_VAR 20.0
#define DHTTYPE DHT11 // DHT 11



DHT dht1(sensor1,DHTTYPE), dht2(sensor2,DHTTYPE), dht3(sensor3,DHTTYPE), dht4(sensor4,DHTTYPE), dht5(sensor5,DHTTYPE); //VARIÁVEL DO TIPO DHT

float temp_mean_now, temp_mean_prev, umid_prev, umid_now;
float var_temp, var_umid;


// Sirene

#define sirene 22



//********************************************************************************
//************************** SETUP ***********************************************
//********************************************************************************


String booleanToString(boolean var){
  if(var)
    return "ON";
  else
    return "OFF";
}

void setup(){
  Wire.begin(8);  //0x08 =8
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendEvent);
  Serial.begin(9600);

  lcd.begin(16, 4);
  lcd.clear();

  dht1.begin(); dht2.begin(); dht3.begin(); dht4.begin(); dht5.begin();
  // Set up the LCD's number of columns and rows
  lcd.createChar(1, symbol_grau);
  temp_mean_prev = dht1.readTemperature();//(float)(dht1.readTemperature()+dht2.readTemperature()+dht3.readTemperature()+dht4.readTemperature()+dht5.readTemperature())/5;
  umid_prev = dht1.readHumidity(); //(float)(dht1.readHumidity()+dht2.readHumidity()+dht3.readHumidity()+dht4.readHumidity()+dht5.readHumidity())/5;

  pinMode(Ar1, INPUT);    // Ar 1
  pinMode(Ar2, INPUT);    // Ar 2
  pinMode(Ar3, INPUT);    // Ar 3

  pinMode(sirene, OUTPUT);    // Ar 3


  lcd.setCursor(0,1);


  delay(3000);




}



//********************************************************************************
//************************** LOOP ************************************************
//********************************************************************************


void loop() {
  
  temp_mean_now= dht1.readTemperature(); //(float)(dht1.readTemperature()+dht2.readTemperature()+dht3.readTemperature()+dht4.readTemperature()+dht5.readTemperature())/5; //dht1.readTemperature();//(float)(dht1.readTemperature()+dht2.readTemperature()+dht3.readTemperature()+dht4.readTemperature()+dht5.readTemperature())/5;
  umid_now= dht1.readHumidity();//(float)(dht1.readHumidity()+dht2.readHumidity()+dht3.readHumidity()+dht4.readHumidity()+dht5.readHumidity())/5; //dht1.readHumidity();//(float)(dht1.readHumidity()+dht2.readHumidity()+dht3.readHumidity()+dht4.readHumidity()+dht5.readHumidity())/5;


delay(500);


}

//********************************************************************************
//************************** ENVIO E RECEBIMENTO *********************************
//********************************************************************************



void receiveEvent(int howMany){
  String recebido="";
  while (0 < Wire.available()) {
    char c = Wire.read();
    recebido += c;
  }

   
  if(recebido.indexOf("IP") > 0){

    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(recebido);
    

   }

   else if(recebido == "PRINTLCD"){
     // Calula temperatura e umidade 

    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Temp: ");
    lcd.print(temp_mean_now);
    lcd.print(" ");
    lcd.write(byte(1));
    lcd.print("C");
    lcd.setCursor(0,1);
    lcd.print("Umidade: ");
    lcd.print(umid_now);
    lcd.print(" ");
    lcd.print("%");

    lcd.setCursor(3,2);
    lcd.print("  AR 1 |");
    lcd.print(" AR 2 |");
    lcd.print(" AR 3 ");
    lcd.setCursor(3,3);
    if(digitalRead(Ar1)==HIGH){
      lcd.print("  ON   |");
    }
    else{
      lcd.print("  OFF  |");
    }

    if(digitalRead(Ar2)==HIGH){
      lcd.print("  ON  |");
    }
    else{
      lcd.print("  OFF |");
    }
    if(digitalRead(Ar3)==HIGH){
      lcd.print("  ON ");
    }
    else{
      lcd.print("  OFF ");
    }

    
  }


  else{

    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Erro de Conexao com o ESP!");
    delay(2000);


  }
  //DataConverted=DecodeStringArray(recibido);

  //Serial.print(recebido);
}

void sendEvent(int howmany){

    String recebido=String(temp_mean_now)+"|"+String(umid_now)+"|"+ digitalRead(Ar1)+"|"+ digitalRead(Ar2) +"|"+ digitalRead(Ar3);
  
Wire.println(recebido);


}