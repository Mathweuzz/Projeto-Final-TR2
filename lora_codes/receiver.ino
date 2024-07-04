#include <SPI.h>
#include <RH_RF95.h>

// global variables
RH_RF95 rf95;
int reset_lora = 9;
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);
unsigned long previousMillis = 0;  // Store the last time an action was performed
const long interval = 5000;        // Interval in milliseconds (5 seconds)
bool rangeIds[256];
String lastReceivedMessage, firstPart, secondPart;

void setup() 
{
  Serial.begin(9600);
  pinMode(reset_lora, OUTPUT);     

  // reset lora module first. to make sure it will works properly
  digitalWrite(reset_lora, LOW);   
  delay(1000);
  digitalWrite(reset_lora, HIGH); 

  // iniatiate radio transmission module
  if (!rf95.init())
    Serial.println("init failed");
  else 
    rf95.setFrequency(915.0);
}

void loop() 
{
  if (rf95.available()) 
  {
    if (readWasSuccesfull()) 
    {
      split();
      if (firstPart == "HS") 
      {
        String hsReply = makeHandshakeReply();
        sendLoraMessage(hsReply);
        previousMillis = currentMillis;
      } 
      else 
      {
        String dataReply = makeDataReply();
        sendLoraMessage(dataReply);
        Serial.println(lastReceivedMessage);
        previousMillis = currentMillis;
      }
  
    } 
    else {
      Serial.println("recv failed");
    }
    if (currentMillis - previousMillis >= interval) {
      // 5 seconds have passed since last received message.
      // entering sleep mode for 1 hour
      rf95.sleep();
      delay(3600000);
      dataReply = makeWakeReply();
      sendLoraMessage(dataReply);
    }
  }
}

int getOfficialId(int id) {
  while(rangeIds[id]) {
    id++;
    if (id > 255)
      id = 0;
  }
  
  rangeIds[id] = true;
  return id;
}

bool readWasSuccesfull(void) {
  if (rf95.recv(buf, &len)) {
    if (len < sizeof(buf)) 
        buf[len] = '\0';
    lastReceivedMessage = (char*)buf;
    return true;
  }
  return false;
}

void split(void) {
  int index = lastReceivedMessage.indexOf("|");
  firstPart = lastReceivedMessage.substring(0, index);
  secondPart = lastReceivedMessage.substring(index + 1);
}

String makeHandshakeReply(void) {
  int officialId = getOfficialId(secondPart.toInt());
  return "HSACK|" + secondPart + "|" + String(officialId);
}

String makeDataReply(void) {
  return "ACK|" + secondPart + "|";
}

String makeWakeReply(void) {
  return "WAKE|" + secondPart + "|";
}

void sendLoraMessage(String message) {
  char buffer[message.length() + 1];
  message.toCharArray(buffer, message.length() + 1);
  rf95.send((uint8_t*)buffer, message.length());
  rf95.waitPacketSent();
}
