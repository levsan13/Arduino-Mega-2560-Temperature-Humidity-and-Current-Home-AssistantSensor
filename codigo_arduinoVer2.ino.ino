//====================================================================================================
//                  Software Sensor de temperatura/Umidade DHT11 e Sensor de corrente
//                  Autor: Sergio Levy Nobre dos Santos
//                  Departamento de Física - Universidade Federal do Ceará - UFC
//====================================================================================================

//********************************************************************************
//************************** INCLUDES ********************************************
//********************************************************************************


#include <UIPEthernet.h>
#include <DHT.h>
#include <Wire.h>
#include <NTPClient.h>
#include <SPI.h> //INCLUSÃO DE BIBLIOTECA
#include <EthernetUdp.h>
#include <TimeLib.h>
#include <Wire.h> // Biblioteca utilizada para fazer a comunicação com o I2C
#include <LiquidCrystal_I2C.h> // Biblioteca utilizada para fazer a comunicação com o display 20x4 
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <EmonLib.h> // Sensores de corrente

//********************************************************************************
//************************** DEFINES AND GLOBAL VARIABLES ************************
//********************************************************************************


// Configuração Rede
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; 
EthernetClient client;
// Configuração MQTT
const char* mqtt_server = "10.2.120.241";
int port = 1883;
const char* mqtt_user = "mqtt_user";
const char* mqtt_password = "mqtt_user";
const char* topicTemp = "sensores/ArduinoDHT11AC/Temp";
const char* topicUmid = "sensores/ArduinoDHT11AC/Umid";
const char* topicAC = "sensores/ArduinoDHT11AC/AC";
//const char* topic_warnings = "sensores/ArduinoDHT11AC/warnings";

// Pinos dos sensores DHT11
#define sensor1 A0
#define sensor2 A1
#define sensor3 A2
#define sensor4 A3
#define sensor5 A4
#define sensor6 A5
#define sensor7 A6
#define sensor8 A7
#define sensor9 A8

// definição dos Ar-condicionados 

//Pino do sensor SCT
int pino_ar1 = A13; // ar 2  
int pino_ar2 = A14; // ar 1
int pino_ar3 = A15; // ar 3


// Configuração dos sensores
#define DHTTYPE DHT11
DHT dht1(sensor1, DHTTYPE);
DHT dht2(sensor2, DHTTYPE);
DHT dht3(sensor3, DHTTYPE);
DHT dht4(sensor4, DHTTYPE);
DHT dht5(sensor5, DHTTYPE);
DHT dht6(sensor6, DHTTYPE);
DHT dht7(sensor7, DHTTYPE);
DHT dht8(sensor8, DHTTYPE);
DHT dht9(sensor9, DHTTYPE);

float temp_mean_now = 30.0, umid_mean_now = 90.0;
float temp1, temp2, temp3 = 30.1, temp4 = 30.1, temp5 = 30.1, temp6 = 30.1, temp7 = 30.1, temp8 = 30.1, temp9 = 30.1;
float umid1, umid2, umid3, umid4 = 30.1, umid5 = 30.1, umid6 = 30.1, umid7 = 30.1, umid8 = 30.1, umid9 = 30.1;
EnergyMonitor Iar1, Iar2, Iar3; // definição das correntes dos Ar-condicionados 
int hours, minutes, seconds, WeekDay_, Month_, Day_, Year_;
int tela=0;

String WeekDays[]={"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};

#define col 20 // Serve para definir o numero de colunas do display utilizado
#define lin  4 // Serve para definir o numero de linhas do display utilizado
#define ende  0x27 // Serve para definir o endereço do display.

LiquidCrystal_I2C lcd(ende,col,lin); // Chamada da funcação LiquidCrystal para ser usada com o I2C

byte symbol_grau[8] ={ B00001100,
B00010010,
B00010010,
B00001100,
B00000000,
B00000000,
B00000000,
B00000000};


///===================== JSON DEFINES ============================

JsonDocument dataTemp, dataUmid, dataAC;

//********************************************************************************
//************************** Reconnect MQTT*****************************
//********************************************************************************

void callback(char* topic, byte* payload, unsigned int length);

PubSubClient mqttClient(mqtt_server, 1883, callback, client);



void reconnect() {
  // Configuração MQTT
  lcd.clear(); // Serve para limpar a tela do display
  lcd.setCursor(0,1);
  lcd.print("Conectando ao broker MQTT...");
  delay(1000);
  while (!mqttClient.connected()) {
    Serial.println("Conectando ao Broker MQTT");
    mqttClient.connect("ArduinoClient",mqtt_user,mqtt_password);
    delay(3000);
  }
  Serial.println("MQTT conectado");
  lcd.clear(); // Serve para limpar a tela do display
  lcd.setCursor(0,1);
  lcd.print("MQTT conectado!");

}

//********************************************************************************
//************************** Reconnect MQTT*****************************
//********************************************************************************


void callback(char* topic, byte* payload, unsigned int length) {

  
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;

 Serial.print("CALLBACK IS WORKING: ");
 Serial.println(message);
  
  if(message == "TEMPMAX"){

  }
  if(message == "0"){
    mqttClient.publish("/home/openhab/DEMO_SWITCH", "RELAY is OFF");
  }
  return;
}




//********************************************************************************
//************************** Append json *****************************************
//********************************************************************************

EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br");

void appendJson(){

  timeClient.update();

  unsigned long epochTime = timeClient.getEpochTime();

  // Converte o epoch time para formato de data usando TimeLib.h
  setTime(epochTime);  // Define o tempo com base no epoch
  hours = hour();
  minutes = minute();
  seconds = second(); 
  WeekDay_ = weekday();
  Month_ = month();
  Day_ = day();
  Year_ = year();


  dataTemp["Temp1"] = temp1;
  dataTemp["Temp2"] = temp2;
  dataTemp["Temp3"] = temp3;
  dataTemp["Temp4"] = temp4;
  dataTemp["Temp5"] = temp5;
  dataTemp["Temp6"] = temp6;
  dataTemp["Temp7"] = temp7;
  dataTemp["Temp8"] = temp8;
  dataTemp["Temp9"] = temp9;
  dataTemp["MeanValueTemp"] = temp_mean_now;

  dataUmid["Hty1"] = umid1;
  dataUmid["Hty2"] = umid2;
  dataUmid["Hty3"] = umid3;
  dataUmid["Hty4"] = umid4;
  dataUmid["Hty5"] = umid5;
  dataUmid["Hty6"] = umid6;
  dataUmid["Hty7"] = umid7;
  dataUmid["Hty8"] = umid8;
  dataUmid["Hty9"] = umid9;
  dataUmid["MeanValueHumidity"] = umid_mean_now;

  dataAC["Ar1"] = Iar1.calcIrms(1480);
  dataAC["Ar2"] = Iar2.calcIrms(1480); //540 <= analogRead(Ar2_)<= 570? 0 : 1;
  dataAC["Ar3"] = Iar3.calcIrms(1480);

}

//********************************************************************************
//************************** PRINTANDO TELA LCD **********************************
//********************************************************************************


void PrintInfoLCD(int tela){
    String Hora = String(hours)+"h"+String(minutes)+":"+String(seconds);
    String data = "Data: " + WeekDays[WeekDay_-1]+ "," + String(Day_) + "/" + String(Month_) + "/" + String(Year_);
    String Ar1_status = Iar1.calcIrms(1480)<30? "OFF" : "ON ";
    String Ar2_status = Iar2.calcIrms(1480)<30? "OFF" : "ON ";
    String Ar3_status = Iar3.calcIrms(1480)<30? "OFF" : "ON ";
    String Ar_status =  Ar1_status + "  |  " + Ar2_status + "  |  " + Ar3_status;
    String ms = mqttClient.connected()? "ON" : "OFF";
    String Mqtt_status = "Status MQTT: " + ms;
    if(tela == 0){
      lcd.clear(); // Serve para limpar a tela do display
      lcd.setCursor(0,0);
      lcd.print("IP: ");
      lcd.setCursor(4,0);
      lcd.print(Ethernet.localIP());
      lcd.setCursor(0,1);
      lcd.print("Hora: ");
      lcd.setCursor(6,1);
      lcd.print(Hora);
      lcd.setCursor(0,2);
      lcd.print(data);
      lcd.setCursor(0,3);
      lcd.print(Mqtt_status);
    }
    if(tela == 1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Temp: ");
      lcd.print(temp_mean_now);
      lcd.write(byte(1));
      lcd.print("C");
      lcd.setCursor(0,1);
      lcd.print("Umid: ");
      lcd.print(umid_mean_now);
      lcd.print(" ");
      lcd.print("%");
      lcd.setCursor(0,2);
      lcd.print("AR1  |  AR2  |  AR3");
      lcd.setCursor(0,3);
      lcd.print(Ar_status);
    }
 
}

//********************************************************************************
//************************** SETUP ***********************************************
//********************************************************************************


void setup() {
  Ethernet.init(53);
  Serial.begin(9600);
  lcd.init(); // Serve para iniciar a comunicação com o display já conectado
  lcd.backlight(); // Serve para ligar a luz do display
  lcd.clear(); // Serve para limpar a tela do display
  lcd.createChar(1, symbol_grau);
  lcd.setCursor(0,0); 
  lcd.print("Inicializando"); // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.
  lcd.setCursor(0,1);
  lcd.print("Ethernet ....."); // Comando de saída com a mensagem que deve aparecer na coluna 2 e linha 1.
  Serial.println("Inicializando Ethernet .....");  
  Ethernet.begin(mac);
  while(Ethernet.linkStatus()!=LinkON){
    lcd.clear(); // Serve para limpar a tela do display
    lcd.setCursor(0,1); 
    lcd.print("Erro de conexao!");
    if(Ethernet.linkStatus()==LinkOFF){
      lcd.setCursor(0,2);
      lcd.print("Cabo desconectado!");
      Serial.println("Cabo desconectado!"); 
    }
    else{
      lcd.setCursor(0,2);
      lcd.print("Erro desconhecido!");
      Serial.println("Erro desconhecido!"); 
    }
    delay(500);

  }
  lcd.clear(); // Serve para limpar a tela do display
  lcd.setCursor(0,2);
  lcd.print("Conectado!");
  delay(1000);

  // Inicializar os sensores DHT11
  dht1.begin();
  dht2.begin();
  dht3.begin();
  dht4.begin();
  dht5.begin();
  dht6.begin();
  dht7.begin();
  dht8.begin();
  dht9.begin();

  // Inicializar os sensores de corrente ar Condicionado
  Iar1.current(pino_ar1, 60);
  Iar2.current(pino_ar2, 60);
  Iar3.current(pino_ar3, 60);

  if(!mqttClient.connected()){
    reconnect();
  }

    //Configura Sensores através do MQTT Discovery do Home Assistant
  String novosensorDH11 = "{\"name\": \"Sensores DHT11\", \"state_topic\": \"sensores/dht11\"}";
  mqttClient.publish("sensores/dht11/config",novosensorDH11.c_str(),false);

    String novosensorArCond = "{\"name\": \"Sensores ArCond\", \"state_topic\": \"sensores/ArCondicionado\"}";
  mqttClient.publish("sensores/ArCondicionado/config",novosensorArCond.c_str(),false);

  timeClient.begin();
  timeClient.setTimeOffset(-10800);
  delay(1000);
  
}

//********************************************************************************
//************************** LOOP ************************************************
//********************************************************************************


void loop() {

    // Leitura dos sensores
  umid1 = dht1.readHumidity();
  temp1 = dht1.readTemperature();
  
  umid2 = dht2.readHumidity();
  temp2 = dht2.readTemperature();
  
  umid3 = dht3.readHumidity();
  temp3 = dht3.readTemperature();
  
  umid4 = dht4.readHumidity();
  temp4 = dht4.readTemperature();
  
  umid5 = dht5.readHumidity();
  temp5 = dht5.readTemperature();
  
  umid6 = dht6.readHumidity();
  temp6 = dht6.readTemperature();
  
  umid7 = dht7.readHumidity();
  temp7 = dht7.readTemperature();
  
  umid8 = dht8.readHumidity();
  temp8 = dht8.readTemperature();
  
  umid9 = dht9.readHumidity();
  temp9 = dht9.readTemperature();

  // Calculo de Temperatura e Umidade relativa média

  temp_mean_now=0.0;
  umid_mean_now=0.0;
  int i,count=0;
  float dht_temp_vec[]={temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9};
  float dht_umid_vec[]={umid1,umid2,umid3,umid4,umid5,umid6,umid7,umid8,umid9};
  for(i=0;i<9;i++){
    if(!isnan(dht_temp_vec[i]) && !isnan(dht_umid_vec[i])){
      count++;
      temp_mean_now += dht_temp_vec[i];
      umid_mean_now += dht_umid_vec[i];
    }
  }
  if(count == 0){
    temp_mean_now=count*temp_mean_now;
    umid_mean_now=count*umid_mean_now;
  }
  else{
    temp_mean_now=((1/float(count))*temp_mean_now);
    umid_mean_now=((1/float(count))*umid_mean_now);
  }
  



  if(!mqttClient.connected()){
    reconnect();
  }
  mqttClient.loop();
  appendJson();
  PrintInfoLCD(tela);

  tela++;
  if(tela>1)
    tela=0;

  // Montar JSON para publicação do DHT 11


  char outTemp[500];
  int a =serializeJson(dataTemp, outTemp);
  char outUmid[500];
  int b =serializeJson(dataUmid, outUmid);
  char outAC[500];
  int c =serializeJson(dataAC, outAC);
  // Publicar no tópico MQTT
  mqttClient.publish(topicTemp, outTemp);
  mqttClient.publish(topicUmid, outUmid);
  mqttClient.publish(topicAC, outAC);

  Serial.println("Dados publicados: " + String(outTemp));
  Serial.println("Dados publicados: " + String(outUmid));
  Serial.println("Dados publicados: " + String(outAC));

  delay(1500); // Intervalo de 1 segundo entre leituras
}

