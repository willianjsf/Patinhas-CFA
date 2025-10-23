#include <Wire.h>
#include <U8g2lib.h>
#include "MPU9250.h"
#include "math.h"

// ========= CONFIGURAÇÕES INICIAIS DO DISPLAY DO ESP32C3 =========
#define OLED_SCL 6 // Pino SCL
#define OLED_SDA 5 // Pino SDA
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, OLED_SCL, OLED_SDA);
const int X_OFFSET = 30;
const int Y_OFFSET = 24;

// ========= CONFIGURAÇÕES INICIAIS DO ACELERÔMETRO =========
MPU9250 mpu;
float rawAccX, rawAccY, rawAccZ;
float A; // Aceleração normalizada instantânea

// ========= VARIÁVEIS E CONSTANTES PARA O ALGORITMO =========

// --- 1. Filtro de Média Móvel ---
const int TAMANHO_FILTRO = 5;
float leiturasFiltro[TAMANHO_FILTRO];
int indiceFiltro = 0;
float A_filtrado;

// --- 2. Máquina de Estados ---
bool esperandoPico = true;
float impactoDoPasso = 1.15;   // medido em g
float impactoDoRepouso = 0.90;  

// --- 3. Controle de Tempo ---
unsigned long momentoDoPassoAnterior = 0;
int tempoEntrePassos = 200;   // em milissegundos
int passos = 0;

// --- 4. Loop Não-Bloqueante ---
unsigned long ultimoUpdateDisplay = 0;
const int INTERVALO_DISPLAY = 250;   // em ms

// --- 5. Filtro de Rotações ---
float gyroMagnitude;
float limiarDeRotacao = 100.0;

// --- 6. Timeout de Estado ---
unsigned long momentoDoPico = 0;
const int tempoMaximoParaRepouso = 500;

// --- 7. Cooldown Após Rotação (NOVA SOLUÇÃO) ---
unsigned long ultimoMomentoDeRotacao = 0;
const int COOLDOWN_APOS_ROTACAO = 500; // Tempo (ms) de "silêncio" após uma rotação intensa.


void setup() {
  Serial.begin(115200);
  Wire.begin(5, 6);


  u8g2.begin();
  u8g2.setContrast(255);
  u8g2.setFont(u8g2_font_spleen12x24_me);


  delay(2000);
  MPU9250Setting config;
  config.accel_fs_sel = ACCEL_FS_SEL::A2G;
  config.gyro_fs_sel  = GYRO_FS_SEL::G250DPS;
  mpu.setup(0x68, config);


  mpu.calibrateAccelGyro();
  mpu.setAccBias(mpu.getAccBiasX(), mpu.getAccBiasY(), mpu.getAccBiasZ());
  mpu.setGyroBias(mpu.getGyroBiasX(), mpu.getGyroBiasY(), mpu.getGyroBiasZ());


  for (int i = 0; i < TAMANHO_FILTRO; i++) {
    leiturasFiltro[i] = 1.0;
  }
 
  Serial.println("Configuração e Calibração concluída!");
  pinMode(4, OUTPUT); //Led
  pinMode(3, OUTPUT); //Buzzer
}

void loop() {
  if (mpu.update()) {
    // 1. Obtém leituras dos sensores
    rawAccX = mpu.getAccX();
    rawAccY = mpu.getAccY();
    rawAccZ = mpu.getAccZ();
    // O cálculo de 'A' agora inclui a gravidade.
    // Se o sensor estiver parado, A será ~1.0
    A = sqrt(pow(rawAccX, 2) + pow(rawAccY, 2) + pow(rawAccZ, 2));

    float gyroX = mpu.getGyroX();
    float gyroY = mpu.getGyroY();
    float gyroZ = mpu.getGyroZ();
    gyroMagnitude = sqrt(pow(gyroX, 2) + pow(gyroY, 2) + pow(gyroZ, 2));

    // 1.1 ATUALIZA O TIMESTAMP DE COOLDOWN DA ROTAÇÃO
    if (gyroMagnitude > limiarDeRotacao) {
      ultimoMomentoDeRotacao = millis();
    }

    // 2. Aplica o Filtro de Média Móvel
    float soma = 0;
    leiturasFiltro[indiceFiltro] = A;
    indiceFiltro = (indiceFiltro + 1) % TAMANHO_FILTRO;
    for (int i = 0; i < TAMANHO_FILTRO; i++) {
      soma += leiturasFiltro[i];
    }
    A_filtrado = soma / TAMANHO_FILTRO;

    Serial.print("Acc:");
    Serial.print(A_filtrado); // <-- Monitore este valor! Parado deve ser ~1.0
    Serial.print(",Gyro:");
    Serial.println(gyroMagnitude);

    // 3. Implementa a Máquina de Estados para Detecção de Passos
    
    // ESTADO 1: Procurando por um pico de aceleração (AGORA > 1.0)
    if (esperandoPico &&
        (A_filtrado > impactoDoPasso) && // <-- Lógica idêntica, mas 'impactoDoPasso' é > 1.0
        (gyroMagnitude < limiarDeRotacao) &&
        ((millis() - ultimoMomentoDeRotacao) > COOLDOWN_APOS_ROTACAO)) {
      
      esperandoPico = false;
      momentoDoPico = millis();
    }
    // ESTADO 2: Pico já ocorreu, agora esperando o vale (AGORA < 1.0)
    else if (!esperandoPico) {
      // CONDIÇÃO 1: O vale foi detectado
      if (A_filtrado < impactoDoRepouso) { // <-- Lógica idêntica, mas 'impactoDoRepouso' é < 1.0
        if ((millis() - momentoDoPassoAnterior) > tempoEntrePassos) {
          passos++;
          momentoDoPassoAnterior = millis();
          
          Serial.print("Total de passos (vale detectado): ");
          Serial.println(passos);
          
          digitalWrite(4, HIGH);
          tone(3, 1000, 250);
          delay(250);
          digitalWrite(4, LOW);
          u8g2.clearBuffer();
          u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 20);
          u8g2.setFont(u8g2_font_spleen12x24_me);
          u8g2.print(passos);
          u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 40);
          u8g2.setFont(u8g2_font_bauhaus2015_tr);
          u8g2.print("Passos");
        }
        esperandoPico = true;
      }
      // CONDIÇÃO 2: O timeout do estado foi atingido
      else if ((millis() - momentoDoPico) > tempoMaximoParaRepouso) {
        Serial.println("Timeout! Resetando estado para procurar novo pico.");
        esperandoPico = true;
      }
    }
  }

  // 4. Atualização do Display de forma Não-Bloqueante
  if (millis() - ultimoUpdateDisplay > INTERVALO_DISPLAY) {
    u8g2.sendBuffer();
    ultimoUpdateDisplay = millis();
  }
}
