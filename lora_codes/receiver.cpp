#include <SPI.h>
#include <LoRa.h>

// Define o endereço do receptor Lora
#define RECEIVER_ADDRESS 0x01

// Define a frequência de operação do Lora
#define LORA_FREQUENCY 915E6

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Imprime a configuração Lora
  Serial.print("LoRa Receiver: ");
  Serial.println(LoRa.getVersion());
  Serial.print("Frequency: ");
  Serial.print(LORA_FREQUENCY);
  Serial.println(" Hz");

}

void loop() {
  // Try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Received a packet
    Serial.print("Received packet '");

    // Read packet
    while (LoRa.available()) {
      String receivedData = LoRa.readString();
      Serial.print(receivedData);

      // Extract distance value from the received string
      if (receivedData.startsWith("Distance: ")) {
        receivedDistance = receivedData.substring(10).toFloat();
      }
    }

    // Print the received distance
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}