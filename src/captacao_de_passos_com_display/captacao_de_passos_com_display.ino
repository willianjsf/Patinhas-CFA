#include <Wire.h>
#include <U8g2lib.h>
#include "MPU9250.h"
#include "math.h"


// ========= CONFIGURAÇÕES INICIAIS DO DISPLAY DO ESP32C3 =========
#define OLED_SCL 6 // Pino SCL
#define OLED_SDA 5 // Pino SDA

// Inicializa o display com a biblioteca U8g2
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, OLED_SCL, OLED_SDA);

// Constantes para o deslocamento do display
const int X_OFFSET = 30; // Deslocamento horizontal de 30 pixels
const int Y_OFFSET = 24; // Deslocamento vertical de 12 pixels
const int VIEW_WIDTH = 70;    // Largura visível
const int VIEW_HEIGHT = 40;   // Altura visível


// ========= CONFIGURAÇÕES INICIAIS DO ACELERÔMETRO =========
MPU9250 mpu;

// Variáveis para armazenar os vieses calculados
float calibratedAccBiasX, calibratedAccBiasY, calibratedAccBiasZ;
float calibratedGyroBiasX, calibratedGyroBiasY, calibratedGyroBiasZ;

// Variáveis para calcular e armazenar a aceleração linear nos 3 eixos e a aceleração normalizada
float linearAccX, linearAccY, linearAccZ; 
float A; //Aceleração normalizada, variável mais importate!!!

// Variáveis para armazenar informações sobre passos registrados
unsigned long momentoDoPassoAnterior = 0; // Guarda a última vez que um evento ocorreu
float impactoDoPasso = 0.40; // Medido em g
int tempoEntrePassos = 250; // Medido em milissegundo
int passos = 0;






void setup() {
  //Inicializa a porta serial entre o ESP e o PC a uma taxa de transmissão de 115200 baud
  Serial.begin(115200);
  
  //Inicializa o barramento I2C
  // Wire.begin();
  Wire.begin(5, 6);

  //Inicializa o display
  u8g2.begin();
  u8g2.setContrast(255);
  u8g2.setBusClock(400000); //400KHz I2C
  u8g2.setFont(u8g2_font_spleen12x24_me);

  // Inicializa acelerômetro
  delay(2000); // Pausa de 2 segundos para estabilizar a inicialização do acelerômetro 
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

  pinMode(4, OUTPUT); //Led
  pinMode(3, OUTPUT); //Buzzer 
}






void loop() {
  //Problema a resolver: A aceleração muda se mexermos na angulação do sensor

  if (mpu.update()) {
    linearAccX = mpu.getLinearAccX();
    linearAccY = mpu.getLinearAccY();
    linearAccZ = mpu.getLinearAccZ();

    A = sqrt(pow(linearAccX, 2) + pow(linearAccY, 2) + pow(linearAccZ, 2));
    // Serial.println(A); 
    

    if (A > impactoDoPasso && (millis() - momentoDoPassoAnterior) > tempoEntrePassos) {

      // Acende o LED ao dar um passo
      digitalWrite(4, HIGH);
      tone(3, 1000, 250);
      delay(250);
      digitalWrite(4, LOW);

      passos++;
      Serial.print("Total de passos: ");
      Serial.println(passos);
      momentoDoPassoAnterior = millis(); //Marca o momento em que o passo foi dado

      u8g2.clearBuffer();
  
      //Escreve quantidade de passos
      u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 20);
      u8g2.setFont(u8g2_font_spleen12x24_me);
      u8g2.print(passos);

      //Escreve "passos" no display
      u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 40);
      u8g2.setFont(u8g2_font_bauhaus2015_tr);
      u8g2.print("Passos");

      //Opcional: Imprime ícone de bateria no display
      // u8g2.setCursor(X_OFFSET + 60, Y_OFFSET + 22);
      // u8g2.setFont(u8g2_font_battery19_tn);
      // u8g2.print(6);
    }

    u8g2.sendBuffer(); //Transfere a memória interna para o display
    delay(100);
  }
  
}