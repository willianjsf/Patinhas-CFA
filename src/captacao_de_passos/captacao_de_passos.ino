#include "MPU9250.h"
#include "math.h"
#include <WiFi.h>
#include <HTTPClient.h>

/*
IMPORTANTE: pro erro A fatal error occurred: Failed to connect to ESP32: Wrong boot mode detected (0x13)! The chip needs to be in download mode.
faça: Press and hold Boot button, click EN button, click Upload, release Boot button only when "Connecting...." is displayed.
*/
MPU9250 mpu;

//Conexão WiFi
const char* ssid = "Redmi Note 13";
const char* password = "123456789";

//Servidor
const char* serverIP = "172.20.183.10"; // Mudar para o IP atual
const int serverPort = 8080;

// Variáveis para armazenar os vieses calculados
float calibratedAccBiasX, calibratedAccBiasY, calibratedAccBiasZ;
float calibratedGyroBiasX, calibratedGyroBiasY, calibratedGyroBiasZ;

// Variáveis para calcular e armazenar a aceleração linear nos 3 eixos e a aceleração normalizada
float linearAccX, linearAccY, linearAccZ; 
float A;

// Variáveis para armazenar informações sobre passos registrados
unsigned long momentoDoPassoAnterior = 0; // Guarda a última vez que um evento ocorreu
float impactoDoPasso = 0.15; // Medido em g
int tempoEntrePassos = 500; // Medido em milissegundo
int passos = 0;
int acumuladorPassos = 0;
unsigned long timerSendPost = 30000;
int limiteAcumulador = 30;
unsigned long lastPost = 0;

void setup() {
  //Inicializa a porta serial entre o ESP e o PC a uma taxa de transmissão de 115200 baud
  Serial.begin(115200);
  
  //Inicializa o barramento I2C
  Wire.begin();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.println(WiFi.status());
  }

  Serial.println("Connectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Pausa de 2 segundos para estabilizar a inicialização do sensor
  delay(2000); 

  // Definir configurações
  MPU9250Setting config;
  config.accel_fs_sel = ACCEL_FS_SEL::A2G;  // Faixa do acelerômetro ±2g, aumenta a resolução e é melhor para detectar passos
  config.gyro_fs_sel  = GYRO_FS_SEL::G250DPS; // Faixa do giroscópio ±250°/s, aumenta a precisão para passos
  mpu.setup(0x68, config);  //Inicializa o sensor no endereço 0x68, aplicando as configurações escolhidas

  // Calibração do acelerômetro + giroscópio (É só deixar o sensor parado)
  mpu.calibrateAccelGyro();

  // Correção dos viéses do acelerômetro
  calibratedAccBiasX = mpu.getAccBiasX();
  calibratedAccBiasY = mpu.getAccBiasY(); 
  calibratedAccBiasZ = mpu.getAccBiasZ(); 
  calibratedGyroBiasX = mpu.getGyroBiasX();
  calibratedGyroBiasY = mpu.getGyroBiasY();
  calibratedGyroBiasZ = mpu.getGyroBiasZ();
  mpu.setAccBias(calibratedAccBiasX, calibratedAccBiasY, calibratedAccBiasZ);
  mpu.setGyroBias(calibratedGyroBiasX, calibratedGyroBiasY, calibratedGyroBiasZ);
  
  Serial.println("Configuração e Calibração concluída!");
  lastPost = millis();
}

void sendPost(int mensagem) {
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    char URL[100];
    sprintf(URL, "http://%s:%d/", serverIP, serverPort);
    
    Serial.println(URL);
    http.begin(URL);
    http.addHeader("Content-Type", "text/plain");
    String payload = String(mensagem);
    int httpResponseCode = http.POST(payload);
    Serial.println(httpResponseCode);

    http.end();
  }
}

void loop() {
  if (mpu.update()) {
    linearAccX = mpu.getLinearAccX();
    linearAccY = mpu.getLinearAccY();
    linearAccZ = mpu.getLinearAccZ();

    A = sqrt(pow(linearAccX, 2) + pow(linearAccY, 2) + pow(linearAccZ, 2));
    
    if (A > impactoDoPasso && (millis() - momentoDoPassoAnterior) > tempoEntrePassos) {
      passos++;
      acumuladorPassos++;
      Serial.print("Total de passos: ");
      Serial.println(passos);
      momentoDoPassoAnterior = millis(); // Marca o momento em que o passo foi dado
      // sendPost(passos);
    }

    delay(100);
  }

  if( ((millis() - lastPost) > timerSendPost) || (acumuladorPassos >= limiteAcumulador) ){ // ve se ja passou de X segundos desde o ultimo post() ou se o acumulador já juntou Y passos
    sendPost(acumuladorPassos); // envia quantos passos dados nos ultimos X segundos
    acumuladorPassos = 0; // zera o acumulador (nao zera o passos total)
    delay(200);
    lastPost = millis();
  }
}