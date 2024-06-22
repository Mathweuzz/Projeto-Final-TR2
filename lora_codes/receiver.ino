
#include <SPI.h>
#include <RH_RF95.h>
#include <sqlite3.h>
#include <time.h> // Inclua a biblioteca TimeLib (opcional)

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
  sqlite3 *db;
  int rc = sqlite3_open("fuel_tank_data.db", &db); // open bd

  if (rc != SQLITE_OK) {
    Serial.println("Cannot open database");
  } else {
    Serial.println("Opened database successfully");

    char *err;
    String sql = "INSERT INTO fuel_data (datetime, level) VALUES (?, ?)";
    sqlite3_stmt *stmt;

    // statement
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, &err);
    if (rc != SQLITE_OK) {
      Serial.println("Error preparing statement:");
      Serial.println(err);
      sqlite3_free(err);
    } else {
      // Bind the values
      String now = getCurrentDateTime(); // ver se vai dá certo
      sqlite3_bind_text(stmt, 1, now.c_str(), -1, SQLITE_STATIC);
      sqlite3_bind_double(stmt, 2, level);

      // Step the statement
      rc = sqlite3_step(stmt);
      if (rc == SQLITE_DONE) {
        Serial.println("Data inserted successfully");
      } else {
        Serial.println("Error inserting data");
      }

      // Finalize the statement
      sqlite3_finalize(stmt);
    }

    sqlite3_close(db); // close bd
  }
}


String getCurrentDateTime() {
  struct tm timeinfo;
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);

  char buffer[80];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}