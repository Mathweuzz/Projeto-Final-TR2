#include <SPI.h>
#include <RH_RF95.h>

// global variables
RH_RF95 rf95;
int reset_lora = 9;
String dataString = "";
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

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
  if (rf95.available()) {
    Serial.println("Get new message"); 

    if (rf95.recv(buf, &len)) {

      // Add null terminator to the buffer
      if (len < sizeof(buf)) {
        buf[len] = '\0';
      } 
      Serial.print("got message: ");
      Serial.println((char*)buf);

      // Send a reply to client as ACK
      sendAckReply();
      Serial.println("Sent a reply");
    } else {
      Serial.println("recv failed");
    }
  }
}

void sendAckReply(void) {
  uint8_t data[] = "200 OK";
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
}