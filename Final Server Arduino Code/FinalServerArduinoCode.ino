#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <AltSoftSerial.h>

#define rxPin 3
#define txPin 2
SoftwareSerial sim800L(rxPin,txPin); 
AltSoftSerial neogps;
TinyGPSPlus gps;

//Connect GPS Rx pin to 9
//Connect GPS TX pin to  8

unsigned long previousMillis = 0;
long interval = 60000;

String URL;
 

void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM800L
  sim800L.begin(9600);

  //Begin serial communication between GPS with Arduino and SIM800L
  neogps.begin(9600);

  Serial.println("Initializing...");


  //This command is for checking the sim module
  sendATcommand("AT", "OK", 2000);
  sendATcommand("AT+CMGF=1", "OK", 2000);
 
  
}

void loop()
{
  while(sim800L.available()){
    Serial.println(sim800L.readString());
  }
  while(Serial.available())  {
    sim800L.println(Serial.readString());
  }

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
       previousMillis = currentMillis;
       sendGpsToServer();
    }
}

int sendGpsToServer()
{
    //Can take up to 60 seconds
    boolean newData = false;
    for (unsigned long start = millis(); millis() - start < 2000;){
      while (neogps.available()){
        if (gps.encode(neogps.read())){
          newData = true;
          break;
        }
      }
    }
  
    if(true){
      newData = false;
      
      String latitude, longitude;
      float altitude;
  
      latitude = String(gps.location.lat(), 6); 
      longitude = String(gps.location.lng(), 6);
      
      Serial.print("Latitude= "); 
      Serial.print(latitude);
      Serial.print(" Longitude= "); 
      Serial.println(longitude);
  
      
      
      String url, temp;
      url = "http://busmanagementsystem.rf.gd/gpsdata.php?lat=";
      url += latitude;
      url += "&lng="; 
      url += longitude;


      Serial.println(url);    

          
    sendATcommand("AT+CFUN=1", "OK", 2000);
    //AT+CGATT = 1 Connect modem is attached to GPRS to a network. AT+CGATT = 0, modem is not attached to GPRS to a network
    sendATcommand("AT+CGATT=1", "OK", 2000);
    //Connection type: GPRS - bearer profile 1
    sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 2000);
    //sets the APN settings for your network provider.
    sendATcommand("AT+SAPBR=3,1,\"APN\",\"web\"", "OK", 2000);
    //enable the GPRS - enable bearer 1
    sendATcommand("AT+SAPBR=1,1", "OK", 2000);
    //Init HTTP service
    sendATcommand("AT+HTTPINIT", "OK", 2000); 
    sendATcommand("AT+HTTPPARA=\"CID\",1", "OK", 2000);
    sim800L.println("AT+HTTPPARA=URL,"+ url);

    sendATcommand("\"", "OK", 1000);
    //Set up the HTTP action
    sendATcommand("AT+HTTPACTION=0", "0,200", 1000);
    //Terminate the HTTP service
    // sendATcommand("AT+HTTPTERM", "OK", 1000);
    //shuts down the GPRS connection. This returns "SHUT OK".
    // sendATcommand("AT+CIPSHUT", "SHUT OK", 1000);

  }
  return 1;    
}

int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    //Initialice the string
    memset(response, '\0', 100);
    delay(100);
    
    //Clean the input buffer
    while( sim800L.available() > 0) sim800L.read();
    
    if (ATcommand[0] != '\0'){
      //Send the AT command 
      sim800L.println(ATcommand);
    }

    x = 0;
    previous = millis();

    //this loop waits for the answer with time out
    do{
        //if there are data in the UART input buffer, reads it and checks for the asnwer
        if(sim800L.available() != 0){
            response[x] = sim800L.read();
            //Serial.print(response[x]);
            x++;
            // check if the desired answer (OK) is in the response of the module
            if(strstr(response, expected_answer) != NULL){
                answer = 1;
            }
        }
    }while((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}
