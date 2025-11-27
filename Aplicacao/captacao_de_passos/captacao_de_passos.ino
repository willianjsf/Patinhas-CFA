#include <Wire.h> 
#include "MPU9250.h"
#include "math.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>

// (teste) Led embutida
const int PIN_LED = 8;
unsigned long momentoLedAceso = 0;
bool ledEstaAceso = false;

// ========= CONFIGURAÇÕES DE REDE =========
WiFiUDP udp;
// ATENÇÃO: Troque pelo seu WiFi real
const char* ssid = "Rede_Wifi"; 
const char* password = "senha_do_wifi";

const int broadcastPort = 50000;

// ========= VARIÁVEIS DO SERVIDOR =========
bool serverEncontrado = false;
char serverIP[32] = ""; 
const int serverPort = 8080;

// ========= VARIÁVEIS DE ENVIO (Faltavam estas!) =========
unsigned long lastPost = 0;
int acumuladorPassos = 0;
// Envia se passar 30 segundos OU acumular 30 passos
const unsigned long timerSendPost = 30000; 
const int limiteAcumulador = 30;

// ========= CONFIGURAÇÕES DO SENSOR =========
MPU9250 mpu; // Declarado apenas uma vez agora
float rawAccX, rawAccY, rawAccZ;
float A; 

// ========= ALGORITMO DE PASSOS =========

// 1. Filtro de Média Móvel 
const int TAMANHO_FILTRO = 5;
float leiturasFiltro[TAMANHO_FILTRO];
int indiceFiltro = 0;
float A_filtrado;

// 2. Máquina de Estados
bool esperandoPico = true;      
float impactoDoPasso = 1.10;    // Ajuste conforme sensibilidade desejada
float impactoDoRepouso = 0.90;  

// 3. Controle de Tempo
unsigned long momentoDoPassoAnterior = 0;  
int tempoEntrePassos = 200;    // Minimo tempo entre passos (ms)
int passos = 0;  

// 4. Detecção de Rotação (Ignorar movimentos bruscos do pet brincando)
float gyroMagnitude;   
float limiarDeRotacao = 100.0;  // graus/segundo
unsigned long ultimoMomentoDeRotacao = 0;  
const int COOLDOWN_APOS_ROTACAO = 500; 

// 5. Timeout
unsigned long momentoDoPico = 0;   
const int tempoMaximoParaRepouso = 500; 

void setup() {
  Serial.begin(115200);

  // CONFIGURAÇÃO DOS PINOS DO ESP32-C3
  Wire.begin(5, 6); //5 = SDA

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  delay(2000);

  // Calibração do MPU
  MPU9250Setting config;
  config.accel_fs_sel = ACCEL_FS_SEL::A2G;
  config.gyro_fs_sel  = GYRO_FS_SEL::G250DPS;
  mpu.setup(0x68, config); //Inicializa o sensor no endereço 0x68, aplicando as configurações escolhidas
  
  Serial.println("Calibrando sensor (deixe parado)...");
  mpu.calibrateAccelGyro();
  
  // Correção dos viéses do acelerômetro: Aplica os valores de calibração para "zerar" os sensores
  mpu.setAccBias(mpu.getAccBiasX(), mpu.getAccBiasY(), mpu.getAccBiasZ());
  mpu.setGyroBias(mpu.getGyroBiasX(), mpu.getGyroBiasY(), mpu.getGyroBiasZ());
  // Inicializa o filtro com valor de gravidade (1.0)
  for (int i = 0; i < TAMANHO_FILTRO; i++) {
    leiturasFiltro[i] = 1.0;
  }
 
  Serial.println("Configuração concluída!");

  lastPost = millis();
  udp.begin(broadcastPort);
  broadcastProcuraServidor();

  // (teste) Configurando o pino do led (para termos um feedback visual do passo)
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH); // Começa apagado (se for lógica invertida) ou aceso.
  // Fazendo o led piscar 3 vezes rápido no final do setup para avisar que ligou
  for(int i=0; i<3; i++) {
    digitalWrite(PIN_LED, LOW); // Liga (ou desliga)
    delay(100);
    digitalWrite(PIN_LED, HIGH); // Desliga (ou liga)
    delay(100);
  }

}

void broadcastProcuraServidor(){
  Serial.println("Procurando servidor na rede...");

  // Fazendo o led piscar 4 vezes rápido quando encontrar servidor para avisar que encontrou
  for(int i=0; i<4; i++) {
    digitalWrite(PIN_LED, LOW); // Liga (ou desliga)
    delay(100);
    digitalWrite(PIN_LED, HIGH); // Desliga (ou liga)
    delay(100);
  }

  while (!serverEncontrado) {
    udp.beginPacket(IPAddress(255,255,255,255), broadcastPort);
    udp.print("DISCOVER_SERVER");
    udp.endPacket();

    int len = udp.parsePacket();
    if (len > 0) {
      char buf[64];
      udp.read(buf, len);
      buf[len] = 0;

      if (strncmp(buf, "SERVER_IP:", 10) == 0) {
        strncpy(serverIP, buf + 10, sizeof(serverIP));
        serverEncontrado = true;
        Serial.print("Servidor encontrado: ");
        Serial.println(serverIP);
      }
    }
    delay(2000);
  }
  
}

void sendPost(int mensagem) {
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    char URL[100];
    sprintf(URL, "http://%s:%d/", serverIP, serverPort);
    
    Serial.print("Enviando dados para: ");
    Serial.println(URL);
    
    http.begin(URL);
    http.addHeader("Content-Type", "text/plain"); // Define explicitamente como texto
    
    String payload = String(mensagem);
    int httpResponseCode = http.POST(payload);
    
    if(httpResponseCode > 0){
      Serial.print("Sucesso! Código: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Erro no envio: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi desconectado!");
  }
}

void loop() {
  if (mpu.update()) {
    // Leitura Acelerômetro
    rawAccX = mpu.getAccX();
    rawAccY = mpu.getAccY();
    rawAccZ = mpu.getAccZ();
    A = sqrt(pow(rawAccX, 2) + pow(rawAccY, 2) + pow(rawAccZ, 2));
    // Serial.println(A);  

    // Leitura Giroscópio (Rotação)
    float gyroX = mpu.getGyroX();
    float gyroY = mpu.getGyroY();
    float gyroZ = mpu.getGyroZ();
    gyroMagnitude = sqrt(pow(gyroX, 2) + pow(gyroY, 2) + pow(gyroZ, 2));

    if (gyroMagnitude > limiarDeRotacao) {
      ultimoMomentoDeRotacao = millis();
    }

    // Filtro Média Móvel
    float soma = 0;
    leiturasFiltro[indiceFiltro] = A;
    indiceFiltro = (indiceFiltro + 1) % TAMANHO_FILTRO;
    for (int i = 0; i < TAMANHO_FILTRO; i++) {
      soma += leiturasFiltro[i];
    }
    A_filtrado = soma / TAMANHO_FILTRO;

    // Máquina de Estados (Algoritmo de passos)
    if (esperandoPico &&
        (A_filtrado > impactoDoPasso) && 
        (gyroMagnitude < limiarDeRotacao) && 
        ((millis() - ultimoMomentoDeRotacao) > COOLDOWN_APOS_ROTACAO)) 
    {    
      esperandoPico = false;
      momentoDoPico = millis();
    }
    else if (!esperandoPico) {
      // Se detectou o vale (passo completado)
      if (A_filtrado < impactoDoRepouso) { 
        if ((millis() - momentoDoPassoAnterior) > tempoEntrePassos) {   
          passos++;
          sendPost(1);
          // acumuladorPassos++;           
          Serial.print("Passo Detectado! Total: ");
          Serial.println(passos);
          momentoDoPassoAnterior = millis();

          // (teste) Piscar led embutido
          digitalWrite(PIN_LED, LOW); // Liga o LED (Nota: No C3 SuperMini, LOW costuma LIGAR)
          ledEstaAceso = true;
          momentoLedAceso = millis();
        }
        esperandoPico = true;
      }
      // Timeout (falso positivo)
      else if ((millis() - momentoDoPico) > tempoMaximoParaRepouso) {
        esperandoPico = true;
      }
    }
  }


  // (teste) Lógica para apagar o LED depois de 100ms (sem usar delay)
  if (ledEstaAceso && (millis() - momentoLedAceso > 100)) {
    digitalWrite(PIN_LED, HIGH); // Apaga o LED
    ledEstaAceso = false;
  }
  
}