//Xpress Lane
//Senior Project 2 (1/2018)
//Assumption University
//Pratik Kumar Basu (5718156) & Niyam Poudel ()

#include <SoftwareSerial.h> 
#include "SparkFun_UHF_RFID_Reader.h" 
#define BUZZER1 9
#define BUZZER2 10



SoftwareSerial softSerial(2, 3);
RFID nano; 

void setup()
{
  int var = 0;
  Serial.begin(115200);

  pinMode(BUZZER1, OUTPUT);
  pinMode(BUZZER2, OUTPUT);
  digitalWrite(BUZZER2, LOW); 

  while (!Serial);
  //Serial.println();
  //Serial.println("Initializing...");

  if (setupNano(38400) == false) 
  {
    //Serial.println("Module failed to respond. Please check wiring.");
    while (1); 
  }

  nano.setRegion(REGION_CHINA); 
  nano.setReadPower(1500); 

int i = 0;
int starttime = millis();
int endtime = starttime;
while((endtime - starttime) <= 60000)
{ 
  String x[1]; 
  
  byte myEPC[2]; 
  byte myEPClength;
  byte responseType = 0;

  //while (responseType != RESPONSE_SUCCESS)
  //{
    myEPClength = sizeof(myEPC); 

    responseType = nano.readTagEPC(myEPC, myEPClength, 500); 
    //Serial.println(F("Searching for tag"));
  //}
if (responseType == RESPONSE_SUCCESS)
   {
  tone(BUZZER1, 2093, 150); //C
  delay(150);

  //Print EPC
  Serial.print(F(""));
  for (byte x = 0 ; x < myEPClength ; x++)
  {
    if (myEPC[x] < 0x10) Serial.print(F("0"));
    
    Serial.print(myEPC[x], HEX);
    
  }
  Serial.println(F(""));
  i++;
    }

  endtime = millis();
 }  

 if ((endtime - starttime) == 59999){

  Serial.end();
 }
}

void loop()
{
}


boolean setupNano(long baudRate)
{
  nano.begin(softSerial); 

  softSerial.begin(baudRate); 
  while(!softSerial);

  while(softSerial.available()) softSerial.read();
  
  nano.getVersion();

  if (nano.msg[0] == ERROR_WRONG_OPCODE_RESPONSE)
  {
   
    nano.stopReading();

    //Serial.println(F("Module continuously reading. Asking it to stop..."));

    delay(1500);
  }
  else
  {
 
    softSerial.begin(115200); 

    nano.setBaud(baudRate); 

    softSerial.begin(baudRate); 
  }


  nano.getVersion();
  if (nano.msg[0] != ALL_GOOD) return (false); 


  nano.setTagProtocol(); 

  nano.setAntennaPort(); 

  return (true); 
}
