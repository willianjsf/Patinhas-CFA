// Código simples para conexão bluetooth do ESP32 -> imprime "Olá do ESP32!" a cada 2s

#include "BluetoothSerial.h"

// Verifica se o Bluetooth está devidamente habilitado na configuração do seu projeto
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


// Cria um objeto para a comunicação serial via Bluetooth
BluetoothSerial SerialBT;


void setup() {
  // Inicia a comunicação serial via USB para depuração
  Serial.begin(115200);


  // Inicia o Bluetooth com um nome. Este nome será visível para outros dispositivos.
  // Escolha um nome único para o seu projeto!
  SerialBT.begin("ESP32_Nano_BT");
 
  Serial.println("O dispositivo Bluetooth iniciou. Agora você pode parear com ele!");
}


void loop() {
  // Verifica se há algum dado chegando do dispositivo conectado (ex: celular)
  if (SerialBT.available()) {
    // Lê o caractere recebido e o envia para o Monitor Serial (USB)
    char dadoRecebido = SerialBT.read();
    Serial.print("Recebido via BT: ");
    Serial.write(dadoRecebido);
    Serial.println();
  }


  // Envia um dado para o dispositivo conectado a cada 2 segundos
  SerialBT.println("Olá do ESP32!");
  delay(2000);
}
