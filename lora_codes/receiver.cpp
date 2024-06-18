#include <SPI.h>
#include <LoRa.h>

// Define os pinos para a comunicação Lora
#define SS 18
#define RST 14
#define DI0 26

// Define o endereço do receptor Lora
#define RECEIVER_ADDRESS 0x01

// Define a frequência de operação do Lora
#define LORA_FREQUENCY 915E6

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Inicializa a comunicação Lora
  LoRa.setPins(SS, RST, DI0);
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

  // Define o endereço do receptor
  LoRa.onReceive(onReceive);
}

void loop() {
  // Entra em modo de baixo consumo de energia
  LoRa.sleep();
}

void onReceive(int packetSize) {
  // Lê os dados recebidos
  String receivedMessage = "";
  for (int i = 0; i < packetSize; i++) {
    receivedMessage += (char)LoRa.read();
  }

  // Imprime os dados recebidos
  Serial.println("Received message: " + receivedMessage);

  // Processa os dados recebidos (ex: salva em um arquivo, envia para um servidor)
}