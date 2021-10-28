#include <stdio.h>
#include <SPI.h>
#include "LoRa.h"
#include "DHT.h"

/********** Descreption ******************** 
 *  This node is composed of : 
 *  --> MKRWAN 1300 board (SAMD21 Microcontroller & LoRa module).
 *  --> DHT11 sensor (temperature & humidity) connected to Digital pin 2.
 ********************************************/

/* Ids */
uint16_t id_node            = 236; // to change

uint16_t id_temp            = 556; // to change
uint16_t id_humid           = 557; // to change
uint16_t id_luminosity      = 558; // to change

const int number_of_sensors = 2;
uint16_t id_sensor [number_of_sensors]={id_temp,id_humid}; //id_luminosity
float value_f[number_of_sensors];

/*Senors*/
#define DHTPIN 6     // Digital pin connected to the DHT senso
#define DHTTYPE DHT11   // DHT 11

/* LoRa module */ 
unsigned int freq = 865500000;//HZ

/*network */ 
#define WITH_APPKEY
unsigned int nCycle = 120;// in second
unsigned short id_frame = 0;
#ifdef WITH_APPKEY
uint8_t my_appKey[4]={5, 6, 7, 8};
#endif


unsigned int cpt=1;
char message[100];


void setup(){
	Serial.begin(38400);
  delay(2000);
  
  Serial.println("Set LoRa modulation\r\n");
  
  if (!LoRa.begin(freq)) {
    Serial.println("Starting LoRa failed!");
    while (1); 
  }
  
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.enableCrc();
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
}





void loop()
{
  
	/* Update sensor values*/
	value_f[0]= Temperature(); // sensor's identifier id_sensor[0] --> Temp (°C)
	value_f[1]= Humidity(); // sensor's identifier id_sensor[1] --> Humid (%)

  Serial.print("\nTemp:");
  Serial.println(value_f[0]);
  Serial.print("Hum:");
  Serial.println(value_f[1]);
  
  Serial.print("Frame Number : ");
  Serial.println(cpt);
  cpt++;
    
	/*Send Data via LoRa Module*/
     send_data(value_f);

	 /* wait for #idlePeriodInMin Minute */
     for (int j=0;j<nCycle;j++) {delay(1000);}
}








void send_data(float * value){
   
  uint8_t app_key_offset = 0;
  int e;
  #ifdef WITH_APPKEY
      app_key_offset = sizeof(my_appKey);
      memcpy(message,my_appKey,app_key_offset);
  #endif
      uint8_t r_size;
      char final_str[80] = "\\";
      char aux[10] = "";
      char id[1] = "";
      sprintf(final_str, "%s!%i!%hd", final_str,id_node, id_frame++);
      for (int i=0; i<number_of_sensors; i++) {
              sprintf(aux,"%4.2f", value[i]);
              sprintf(final_str, "%s#%d/%s", final_str, id_sensor[i], aux);
      }

      r_size=sprintf(message+app_key_offset,final_str);
    
      Serial.print(message);
      Serial.println(r_size);
      
      LoRa.beginPacket();
      LoRa.print(message);
      LoRa.endPacket(); 
      
     Serial.println("DATA sent correctly !");
  
      for(uint8_t j=0;j<number_of_sensors;j++){
        value[j]=0.0;
      }
}







float Temperature(){
  DHT dht(DHTPIN, DHTTYPE);
  dht.begin();
  delay(2000);
  return dht.readTemperature();
}
float Humidity(){
  DHT dht(DHTPIN, DHTTYPE);
  dht.begin();
  delay(2000);
  return dht.readHumidity();
}
