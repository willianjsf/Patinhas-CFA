  #include <Wire.h> 
  #include "MPU9250.h"
  #include <U8g2lib.h>
  #include "math.h"
  #include <WiFi.h>
  #include <HTTPClient.h>
  #include <WiFiUdp.h>


  // ========= CONFIGURAÇÕES DE REDE =========
  WiFiUDP udp;
  const char* ssid = "_esp"; 
  const char* password = "esp32c3mini";

  const int broadcastPort = 50000;

  // ========= VARIÁVEIS DO SERVIDOR =========
  bool serverEncontrado = false;
  char serverIP[32] = ""; 
  const int serverPort = 8080;

  // ========= VARIÁVEIS DE ENVIO =========
  unsigned long lastPost = 0;
  // int acumuladorPassos = 0;
  // // Envia se passar 30 segundos OU acumular 30 passos
  // const unsigned long timerSendPost = 30000; 
  // const int limiteAcumulador = 30;

  // ========= CONFIGURAÇÕES DO SENSOR =========
  MPU9250 mpu; // Declarado apenas uma vez agora
  float rawAccX, rawAccY, rawAccZ;
  float A; 

  // ========= CONFIGURAÇÕES INICIAIS DO DISPLAY DO ESP32C3 =========
  #define OLED_SCL 6  // Pino SCL
  #define OLED_SDA 5  // Pino SDA
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, OLED_SCL, OLED_SDA);
  const int X_OFFSET = 30;
  const int Y_OFFSET = 24;

  // ========= ALGORITMO DE PASSOS =========

  // 1. Filtro de Média Móvel 
  const int TAMANHO_FILTRO = 3;
  float leiturasFiltro[TAMANHO_FILTRO];
  int indiceFiltro = 0;
  float A_filtrado;

  // 2. Máquina de Estados
  bool esperandoPico = true;      
  float impactoDoPasso = 1.2;    // Ajuste conforme sensibilidade desejada (1.5 inicialmente)
  float impactoDoRepouso = 0.95;  // Ajuste conforme sensibilidade desejada (0.9 inicialmente)

  // 3. Controle de Tempo
  unsigned long momentoDoPassoAnterior = 0;  
  int tempoEntrePassos = 100;    // Minimo tempo entre passos (ms) (200 inicialmente)
  int passos = 0;  

  // 4. Loop Não-Bloqueante 
  unsigned long ultimoUpdateDisplay = 0;  // armazena o tempo da última atualização
  const int INTERVALO_DISPLAY = 100;   // em ms

  // 5. Detecção de Rotação (Ignorar movimentos bruscos do pet brincando)
  float gyroMagnitude;   
  float limiarDeRotacao = 100.0;  // graus/segundo
  unsigned long ultimoMomentoDeRotacao = 0;  
  const int COOLDOWN_APOS_ROTACAO = 500; 

  // 5. Timeout
  unsigned long momentoDoPico = 0;   
  const int tempoMaximoParaRepouso = 400; 

  void setup() {
    Serial.begin(115200);

    // CONFIGURAÇÃO DOS PINOS DO ESP32-C3
    Wire.begin(5, 6); //5 = SDA, 6 = SCL

    // Inicializa display
    u8g2.begin();
    u8g2.setContrast(255);
    u8g2.setFont(u8g2_font_spleen12x24_me);

    //Printar no display essas mensagens do serial
    


    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.println("Conectando ao WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
      //Printar no display essas mensagens do serial
      u8g2.clearBuffer();
      u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 20);
      u8g2.setFont(u8g2_font_bauhaus2015_tr);
      u8g2.print("Buscando ");
      u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 40);
      u8g2.setFont(u8g2_font_bauhaus2015_tr);
      u8g2.print("Wifi...");
    }

    Serial.println("\nConectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    //Printar no display essas mensagens do serial
    u8g2.clearBuffer();
    u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 20);
    u8g2.setFont(u8g2_font_bauhaus2015_tr);
    u8g2.print("Wifi ");
    u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 40);
    u8g2.setFont(u8g2_font_bauhaus2015_tr);
    u8g2.print("OK!");

    delay(2000);

    //Printar no display essas mensagens do serial
    u8g2.clearBuffer();
    u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 20);
    u8g2.setFont(u8g2_font_bauhaus2015_tr);
    u8g2.print("De um");
    u8g2.setCursor(X_OFFSET + 5, Y_OFFSET + 40);
    u8g2.setFont(u8g2_font_bauhaus2015_tr);
    u8g2.print("passo");

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


  }

  void broadcastProcuraServidor(){
    Serial.println("Procurando servidor na rede...");

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
      delay(1000);
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
      // Serial.println(A);  //Só para debug

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
      // Serial.println(A_filtrado);  //Só para debug


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

            // Escreve a quantidade de passos no display
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
        // Timeout (falso positivo)
        else if ((millis() - momentoDoPico) > tempoMaximoParaRepouso) {
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
