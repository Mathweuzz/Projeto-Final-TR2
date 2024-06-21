#include <SPI.h>
#include <RH_RF95.h>

// global variables
const int reset_lora = 9;
const int trigger_port = 3;
const int echo_port = 4;
const float longest_distance = 100;

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
  char buffer[distance_cm.length() + 1]; // +1 para o caractere nulo '\0'
  distance_cm.toCharArray(buffer, sizeof(buffer));
  rf95.send((uint8_t*)buffer, distance_cm.length());
  rf95.waitPacketSent();
  Serial.println("Sent message");

  delay(100);
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
