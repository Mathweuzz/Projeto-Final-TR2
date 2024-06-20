#include <SPI.h>
#include <LoRa.h>
#include <NewPing.h>

// Define o necessário para o sensor
#define TRIGGER_PIN  3
#define ECHO_PIN     4
#define MAX_DISTANCE 200

// Define a frequência de operação do Lora
#define LORA_FREQUENCY 915E6

// Variável para armazenar o nível de combustível
int fuelLevel = 0;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Imprime a configuração Lora
  Serial.print("LoRa Sender: ");
  Serial.println(LoRa.getVersion());
  Serial.print("Frequency: ");
  Serial.print(LORA_FREQUENCY);
  Serial.println(" Hz");
}

void loop() {
  // Lê o nível de combustível do sensor
  fuelLevel = sonar.ping_cm();

  // "Acorda" o LoRa
  LoRa.idle();

  // Envia o pacote de dados para o receptor
  LoRa.beginPacket();
  LoRa.print("Distance: ");
  LoRa.print(fuelLevel);
  LoRa.println(" cm");
  LoRa.endPacket();
  Serial.println("Data sent: " + fuelLevel);

  // Entra em modo de baixo consumo de energia
  LoRa.sleep();

  delay(10000); // Verifica o nível de combustível a cada 10 segundos
}