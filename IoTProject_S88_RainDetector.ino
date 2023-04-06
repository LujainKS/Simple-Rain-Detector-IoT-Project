#include <ESP8266WiFi.h>

  const char* ssid     = "lujain";
  const char* password = "11111111";

  // ifttt.com access details
  const char* host = "maker.ifttt.com";
  const byte  port = 80;             // http standard port
  WiFiClient client;                 // wifi client object

  int sensorPrevState;

 void setup() {
  
    Serial.begin(9600);
    
    //red LED (it's raining) & Buzzer
    pinMode(12, OUTPUT);
    //blue LED (it's not raining)
    pinMode(13, OUTPUT);

    //red LED (to detected whether the nodeMCU connected to the WiFi or not)
    pinMode(14, OUTPUT);

    //intial state of LEDs
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);

    //WiFi connection
    // join wifi network
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi= ");
    Serial.print(ssid);
    Serial.println(" ...");

    while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(14, !(digitalRead(14)));  // toggle the red LED
    delay(500);
    Serial.print('.');
    } 
  }//end of setup method


  void loop() {

    int sensorCurrentState = analogRead(5);
 
    //to check whether the sensor state changed or not
    if(sensorCurrentState!=sensorPrevState)
    {
      if (!client.connect(host, port)) {           // connect to ifttt.com
        Serial.println("Connection failed.");
        return;
      }
      if (sensorCurrentState < 1023) {
        delay(1500);
        if (analogRead(5) < 1023)
        {
        Serial.println("It's RAINING");
        Serial.println(sensorCurrentState);
        digitalWrite(12, HIGH);
        digitalWrite(13, LOW);

        // request ifttt.com to trigger the front door access event email
        String url = "/trigger/RaindropsDetectionSystem/with/key/csgOluH3nowBZRjM-IcMny";
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                    "Host: " + host + "\r\n" +
                    "Connection: close\r\n\r\n");

        delay(100);  // without the delay, the request may not be completed

        while (client.available())
          {
            Serial.println(client.read());
          }
      
        Serial.println("\nDisconnecting from ifttt.com");
        client.stop();

        delay(5000);
        }
        else
        {
          sensorCurrentState=1023;
        }
      }
    else if (sensorCurrentState == 1023) 
    {
      Serial.println("It's Not RAINING");
      Serial.println(sensorCurrentState);
      digitalWrite(13, HIGH);
      digitalWrite(12, LOW);

      // request ifttt.com to trigger the front door access event email
      String url = "/trigger/NoRaindropsDetectionSystem/with/key/csgOluH3nowBZRjM-IcMny";
      client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                  "Host: " + host + "\r\n" +
                  "Connection: close\r\n\r\n");

      delay(100);  // without the delay, the request may not be completed
  
      while (client.available())
        {
          Serial.println(client.read());
        }
    
      Serial.println("\nDisconnecting from ifttt.com");
      client.stop();
    }

    }//end of the outer if block
    sensorPrevState=sensorCurrentState;
  }//end of the loop method
