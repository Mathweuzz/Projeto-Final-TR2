#include <SPI.h>
#include <RH_RF95.h>

// global variables
const int reset_lora = 9;
const int trigger_port = 3;
const int echo_port = 4;
const float longest_distance = 100;
uint8_t indatabuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(indatabuf);

// instanciation
RH_RF95 rf95;

void setup() 
{
  pinMode(reset_lora, OUTPUT);
  pinMode(echo_port,INPUT);
  pinMode(trigger_port,OUTPUT);    

  // reset lora module first. to make sure it will works properly
  digitalWrite(reset_lora, LOW);   
  delay(1000);
  digitalWrite(reset_lora, HIGH); 

  Serial.begin(9600);

  if (!rf95.init())
    Serial.println("init failed");
  else // Set Brazil Allowed Frequency 
    rf95.setFrequency(915.0);  
}

void loop()
{
  makePulse();
  float duration = pulseIn(echo_port, HIGH);
  String distance_cm = String(microsecondsToCentimeters(duration), 2);
  sendLoraMessage(distance_cm);
  Serial.println(distance_cm);

  if (rf95.waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(indatabuf, &len))
   {
      // Serial print "got reply:" and the reply message from the server
       if (len < sizeof(indatabuf)) {
        indatabuf[len] = '\0';
      } 
      Serial.print("got reply: ");
      Serial.println((char*)indatabuf);
   }
     else
     {
      Serial.println("recv failed");
     }
  }
  else
  {
    // Serial print "No reply, is rf95_server running?" if don't get the reply .
    Serial.println("No reply, is rf95_server running? Sending the same message");
    sendLoraMessage(distance_cm);
  }
  delay(5000);
}

void makePulse(void) {
  digitalWrite(trigger_port, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_port, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigger_port, LOW);
}

float microsecondsToCentimeters(float microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object
  // we take half of the distance traveled.
  return microseconds / 29 / 2;
}

void sendLoraMessage(String message) {
  char buffer[message.length()];
  message.toCharArray(buffer, sizeof(buffer));
  rf95.send((uint8_t*)buffer, message.length());
  rf95.waitPacketSent();
}
