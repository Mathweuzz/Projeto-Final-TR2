#include <SPI.h>
#include <LoRa.h>

// Define os pinos para a comunicação Lora
#define SS 18
#define RST 14
#define DI0 26

// Define o endereço do receptor Lora
#define RECEIVER_ADDRESS 0x01

// Define o pino do sensor de nível de combustível
#define FUEL_SENSOR_PIN A0

// Define a frequência de operação do Lora
#define LORA_FREQUENCY 915E6

// Variável para armazenar o nível de combustível
int fuelLevel = 0;

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
  Serial.print("LoRa Sender: ");
  Serial.println(LoRa.getVersion());
  Serial.print("Frequency: ");
  Serial.print(LORA_FREQUENCY);
  Serial.println(" Hz");
}

void loop() {
  // Lê o nível de combustível do sensor
  fuelLevel = analogRead(FUEL_SENSOR_PIN);

  // Se o nível de combustível caiu abaixo de um determinado limite, envia os dados para o receptor
  if (fuelLevel < 500) {
    // Cria um pacote de dados
    String message = "Fuel Level: " + String(fuelLevel);

    // Envia o pacote de dados para o receptor
    LoRa.beginPacket();
    LoRa.write(message.c_str(), message.length());
    LoRa.endPacket();
    Serial.println("Data sent: " + message);
  }

  // Entra em modo de baixo consumo de energia
  LoRa.sleep();

  // Ativa o Arduino em intervalos fixos para verificar o nível de combustível
  delay(10000); // Verifica o nível de combustível a cada 10 segundos
}