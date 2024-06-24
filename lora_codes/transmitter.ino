#include <SPI.h>
#include <RH_RF95.h>

// global variables
const int reset_lora = 9;
const int trigger_port = 3;
const int echo_port = 4;
const float longest_distance = 100;
uint8_t indatabuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(indatabuf);
String id;
String lastMessageSent, receivedMessage, messageType, messageId, officialId;
enum ProtocolState {
  HANDSHAKE,
  ACTIVE_TRANSMITTER
} state;

// instanciation
RH_RF95 rf95;

void setup() 
{
  Serial.begin(9600);
  
  pinMode(reset_lora, OUTPUT);
  pinMode(echo_port,INPUT);
  pinMode(trigger_port,OUTPUT);    

  // reset lora module first. to make sure it will works properly
  digitalWrite(reset_lora, LOW);   
  delay(1000);
  digitalWrite(reset_lora, HIGH); 

  

  if (!rf95.init())
    Serial.println("init failed");
  else // Set Brazil Allowed Frequency 
    rf95.setFrequency(915.0);

  state = HANDSHAKE;
  id = generateRandomId();
}

void loop() {
  switch(state) {
    case HANDSHAKE:
      lastMessageSent = "HS|" + id;
      sendLoraMessage(lastMessageSent);
      Serial.println(lastMessageSent);
      if (rf95.waitAvailableTimeout(3000)) { 
        if (readWasSuccesfull()) {
          splitMessage();
          if (messageType == "HSACK" && messageId.equals(id)) {
            id = officialId.toInt();
            state = ACTIVE_TRANSMITTER;
          }
        } else {
          Serial.println("recv failed");
        }
      } else {
        Serial.print("Timeout, resending last message = ");
        Serial.println(lastMessageSent);
        sendLoraMessage(lastMessageSent);
        
      }
      delay(1000);
      break;

    case ACTIVE_TRANSMITTER:
      lastMessageSent = id + "|" + getLevelData();
      sendLoraMessage(lastMessageSent);
      Serial.println(lastMessageSent);
      if (rf95.waitAvailableTimeout(3000)) {   
        if (readWasSuccesfull()) {
          splitMessage();
          if (messageType == "ACK" && messageId.equals(id)) {
            Serial.println("ACK received");
          }
        } else {
          Serial.println("recv failed");
        }
      } else {
        Serial.print("Timeout, resending last message = ");
        Serial.println(lastMessageSent);
        sendLoraMessage(lastMessageSent);
      }
      delay(5000);
      break;
  }
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

String getLevelData(void) {
  makePulse();
  float duration = pulseIn(echo_port, HIGH);
  return String(microsecondsToCentimeters(duration), 2);
}

void sendLoraMessage(String message) {
  char buffer[message.length() + 1];
  message.toCharArray(buffer, message.length() + 1);
  rf95.send((uint8_t*)buffer, message.length());
  rf95.waitPacketSent();
}


String generateRandomId(void) {
  return String(random(0, 256)); // Generates a random number between 0 and 255
}

void splitMessage(void) {
  int firstPipe = receivedMessage.indexOf('|');
  int secondPipe = receivedMessage.indexOf('|', firstPipe + 1);

  messageType = receivedMessage.substring(0, firstPipe);
  messageId = receivedMessage.substring(firstPipe + 1, secondPipe);
  officialId = receivedMessage.substring(secondPipe + 1);
}

bool readWasSuccesfull(void) {
  if (rf95.recv(indatabuf, &len)) {
    if (len < sizeof(indatabuf)) 
        indatabuf[len] = '\0';
    receivedMessage = (char*)indatabuf;
    return true;
  }
  return false;
}