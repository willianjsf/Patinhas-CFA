#include "MPU9250.h"
#include "math.h"

MPU9250 mpu;

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


void setup() {
  //Inicializa a porta serial entre o ESP e o PC a uma taxa de transmissão de 115200 baud
  Serial.begin(115200);
  
  //Inicializa o barramento I2C
  Wire.begin();

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

  

  // // Ponteiro para arquivo
  // FILE *fptr;

  // // Cria o arquivo
  // fptr = fopen("passos.txt", "w");

  // // Fecha o arquivo
  // fclose(fptr);
}

void loop() {

  // Descomentar se precisar ver os valores de aceleração captados em cada um dos eixos
  // if (mpu.update()) {
  //   Serial.print(mpu.getLinearAccX());
  //   Serial.print(", ");
  //   Serial.print(mpu.getLinearAccY());
  //   Serial.print(", ");
  //   Serial.println(mpu.getLinearAccZ());
  //   delay(50);
  // }

  // PROBLEMA: A aceleração muda se mexermos na angulação do sensor

  if (mpu.update()) {
    linearAccX = mpu.getLinearAccX();
    linearAccY = mpu.getLinearAccY();
    linearAccZ = mpu.getLinearAccZ();

    A = sqrt(pow(linearAccX, 2) + pow(linearAccY, 2) + pow(linearAccZ, 2));
    Serial.println(A);
    

    if (A > impactoDoPasso && (millis() - momentoDoPassoAnterior) > tempoEntrePassos) {
      passos++;
      Serial.print("Total de passos: ");
      Serial.println(passos);
      momentoDoPassoAnterior = millis(); //Marca o momento em que o passo foi dado
    }

    delay(100);
  }
  
}
