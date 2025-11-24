#include <Wire.h>      
#include <U8g2lib.h>
#include "MPU9250.h"
#include "math.h"

// ========= CONFIGURAÇÕES INICIAIS DO DISPLAY DO ESP32C3 =========
#define OLED_SCL 6  // Pino SCL
#define OLED_SDA 5  // Pino SDA
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, OLED_SCL, OLED_SDA);
const int X_OFFSET = 30;
const int Y_OFFSET = 24;

// ========= CONFIGURAÇÕES INICIAIS DO ACELERÔMETRO =========
MPU9250 mpu;
float rawAccX, rawAccY, rawAccZ;
float A; 

// ========= VARIÁVEIS E CONSTANTES PARA O ALGORITMO =========

// 1. Filtro de Média Móvel 
const int TAMANHO_FILTRO = 5;
float leiturasFiltro[TAMANHO_FILTRO];
int indiceFiltro = 0;
float A_filtrado;

// 2. Máquina de Estados
bool esperandoPico = true;     // flag para definir o estado atual
float impactoDoPasso = 1.15;   // pico (impacto) medido em g
float impactoDoRepouso = 0.90;  // vale (repouso) em g

// 3. Controle de Tempo
unsigned long momentoDoPassoAnterior = 0;  // armazena o tempo em que o último passo foi contado
int tempoEntrePassos = 200;   // em ms
int passos = 0;  

// 4. Loop Não-Bloqueante 
unsigned long ultimoUpdateDisplay = 0;  // armazena o tempo da última atualização
const int INTERVALO_DISPLAY = 250;   // em ms

// 5. Filtro de Rotações
float gyroMagnitude;   // armazena a magnitude total da rotação (velocidade angular)
float limiarDeRotacao = 100.0;  // em graus/segundo

// 6. Timeout de Estado
unsigned long momentoDoPico = 0;   // armazena o tempo em que o pico foi detectado
const int tempoMaximoParaRepouso = 500;  // se detectar um pico e em até 0,5 segundo não detectar o vale -> alarme falso

// 7. Cooldown Após Rotação
unsigned long ultimoMomentoDeRotacao = 0;  // para armazenar o último momento de rotação abrupta
const int COOLDOWN_APOS_ROTACAO = 500; // tempo (ms) de silêncio após uma rotação intensa.


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
  // calibração do gyro
  mpu.calibrateAccelGyro();
  // aplica os valores de calibração para "zerar" os sensores
  mpu.setAccBias(mpu.getAccBiasX(), mpu.getAccBiasY(), mpu.getAccBiasZ());
  mpu.setGyroBias(mpu.getGyroBiasX(), mpu.getGyroBiasY(), mpu.getGyroBiasZ());
  // preenche o array com valores de 1.0g (valor esperado para repouso)
  for (int i = 0; i < TAMANHO_FILTRO; i++) {
    leiturasFiltro[i] = 1.0;
  }
 
  Serial.println("Configuração e Calibração concluída!");


}

void loop() {
  if (mpu.update()) {
    // leituras dos sensores

    rawAccX = mpu.getAccX();
    rawAccY = mpu.getAccY();
    rawAccZ = mpu.getAccZ();
  
    A = sqrt(pow(rawAccX, 2) + pow(rawAccY, 2) + pow(rawAccZ, 2));  // calcula a magnitude total da aceleração

    float gyroX = mpu.getGyroX();
    float gyroY = mpu.getGyroY();
    float gyroZ = mpu.getGyroZ();
    gyroMagnitude = sqrt(pow(gyroX, 2) + pow(gyroY, 2) + pow(gyroZ, 2)); // calcula a magnitude total da rotação

    // Atualiza o registro de cooldown da rotação
    if (gyroMagnitude > limiarDeRotacao) {
      ultimoMomentoDeRotacao = millis();
    }

    // Aplica o filtro de média móvel
    float soma = 0;
    leiturasFiltro[indiceFiltro] = A;
    indiceFiltro = (indiceFiltro + 1) % TAMANHO_FILTRO;
    for (int i = 0; i < TAMANHO_FILTRO; i++) {
      soma += leiturasFiltro[i];
    }
    A_filtrado = soma / TAMANHO_FILTRO;



    // Implementação da máquina de estados para detecção de passos
    // ESTADO 1: Procurando por um pico de aceleração 
    if (esperandoPico &&
        (A_filtrado > impactoDoPasso) &&                                  // acc filtrada é maior que o impacto do passo (g)?
        (gyroMagnitude < limiarDeRotacao) &&                              // a rotação é menor que o valor definido em limiarDeRotacao?
        ((millis() - ultimoMomentoDeRotacao) > COOLDOWN_APOS_ROTACAO))    // já passou o tempo de cooldown após rotação?
    {    
      esperandoPico = false;
      momentoDoPico = millis();
    }
    // ESTADO 2: Pico já ocorreu, agora esperando o vale 
    else if (!esperandoPico) {
      // CONDIÇÃO 1: O vale foi detectado
      if (A_filtrado < impactoDoRepouso) {                              // acc filtrada é menor que o impacto do repouso?
        if ((millis() - momentoDoPassoAnterior) > tempoEntrePassos) {   // checagem para saber se passou o tempo mínimo entre passos
          passos++;
          momentoDoPassoAnterior = millis();
          
          Serial.print("Total de passos: ");
          Serial.println(passos);
          
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

  // Atualização do display regularmente, independentemente da detecção de passos
  if (millis() - ultimoUpdateDisplay > INTERVALO_DISPLAY) {
    u8g2.sendBuffer();
    ultimoUpdateDisplay = millis();
  }
}
