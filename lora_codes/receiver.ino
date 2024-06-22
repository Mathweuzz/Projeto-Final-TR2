
#include <SPI.h>
#include <RH_RF95.h>
#include <TimeLib.h>
#include <TinySQLite.h>

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


      // trying add data to bd 
      float level = atof((char*)buf);

      insertData(level);
      
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

void insertData(float level) {
  String now = getCurrentDateTime();
  String sql = "INSERT INTO fuel_data (datetime, level) VALUES (?, ?)";
  sqlite3_stmt *stmt;

  int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {
    Serial.println("Error preparing statement");
    return;
  }

  sqlite3_bind_text(stmt, 1, now.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_double(stmt, 2, level);

  rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    Serial.println("Data inserted successfully");
  } else {
    Serial.println("Error inserting data");
  }

  sqlite3_finalize(stmt);
}

String getCurrentDateTime() {
  char buffer[80];
  sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
  return String(buffer);
}