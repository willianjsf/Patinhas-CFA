// Código criado para mandar uma mensagem via wifi (através de um site) e aparecer no ESP32C3 super mini, lembre-se de baixar as bibliotecas necessárias e utilizar a placa XIAO_ESP32C3.
// *** AS CONSTANTES PARA A MENSAGEM APARCER NO ENQUADRAMENTO CORRETO AINDA PRECISAM SER AJUSTADAS ***

#include <WiFi.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "esp_eap_client.h"

// --- CONFIGURAÇÕES DA REDE WI-FI EDUROAM ---
const char* ssid = "eduroam";
#define EAP_IDENTITY "xxxxxx@usp.br" // SEU USUÁRIO
#define EAP_PASSWORD "xxxxx#"         // SUA SENHA

// --- CONFIGURAÇÕES DO DISPLAY PARA XIAO ESP32-C3 ---
#define OLED_SCL 6 // Pino SCL correto para a XIAO
#define OLED_SDA 5 // Pino SDA correto para a XIAO

// --- CONSTANTES PARA O DESLOCAMENTO DO DISPLAY ---
const int X_OFFSET = 30; // Deslocamento horizontal de 30 pixels
const int Y_OFFSET = 12; // Deslocamento vertical de 12 pixels
const int VIEW_WIDTH = 72;    // Largura visível
const int VIEW_HEIGHT = 40;   // Altura visível

// Inicializa o display com a biblioteca U8g2
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, OLED_SCL, OLED_SDA);

// --- VARIÁVEIS GLOBAIS ---
String textoNoDisplay = "Iniciando...";
WiFiServer server(80);

// --- FUNÇÃO DE ATUALIZAÇÃO DO DISPLAY (COM OFFSET) ---
void atualizarDisplay() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr); // Fonte um pouco menor para caber melhor

  // Opcional: Desenha uma moldura para visualizar a área útil.
  u8g2.drawFrame(X_OFFSET, Y_OFFSET, VIEW_WIDTH, VIEW_HEIGHT);
 
  String linha1 = textoNoDisplay;
  String linha2 = "";
 
  // Lógica para quebrar texto em duas linhas (ajustado para a largura menor)
  if (textoNoDisplay.length() > 12) {
    for (int i=12; i > 0; i--) {
        if (isspace(textoNoDisplay.charAt(i))) {
            linha1 = textoNoDisplay.substring(0, i);
            linha2 = textoNoDisplay.substring(i + 1);
            break;
        }
    }
  }

  // Define o cursor DENTRO da área visível usando os offsets
  u8g2.setCursor(X_OFFSET + 15, Y_OFFSET + 22);
  u8g2.print(linha1);    
 
  if (linha2 != "") {
    u8g2.setCursor(X_OFFSET + 15, Y_OFFSET + 35);
    u8g2.print(linha2);
  }
 
  u8g2.sendBuffer();
}


void setup() {
  Serial.begin(115200);
  u8g2.begin();
 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.setCursor(X_OFFSET + 3, Y_OFFSET + 15); // Posição corrigida
  u8g2.print("Conectando");
  u8g2.setCursor(X_OFFSET + 3, Y_OFFSET + 32); // Posição corrigida
  u8g2.print("a Eduroam...");
  u8g2.sendBuffer();

  Serial.println("Conectando a Eduroam...");

  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

  esp_eap_client_set_identity((const uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_eap_client_set_username((const uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_eap_client_set_password((const uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_enterprise_enable();

  WiFi.begin(ssid);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(tentativas++ > 40) {
        Serial.println("\nFalha ao conectar!");
        textoNoDisplay = "Falha na conexao!";
        atualizarDisplay();
        while(true);
    }
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());

  server.begin();

  textoNoDisplay = "IP:" + WiFi.localIP().toString();
  atualizarDisplay();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Novo cliente conectado!");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          // A linha da requisição HTTP terminou

          // 1. Verificação para ignorar o favicon.ico
          if (currentLine.indexOf("GET /favicon.ico") >= 0) {
            Serial.println("Requisicao do favicon.ico ignorada.");
            break;
          }

          // 2. Verificação para a requisição de texto
          if (currentLine.indexOf("GET /get?texto=") >= 0) {
            int startIndex = currentLine.indexOf('=') + 1;
            int endIndex = currentLine.indexOf(" HTTP");
            String tempTexto = currentLine.substring(startIndex, endIndex);
            tempTexto.replace("+", " ");
           
            // Decodificação de URL (código original)
            String decodedText = "";
            char temp[] = "0x00";
            for (unsigned int i = 0; i < tempTexto.length(); i++) {
                if (tempTexto[i] == '%') {
                    temp[2] = tempTexto[i+1];
                    temp[3] = tempTexto[i+2];
                    decodedText += (char) strtol(temp, NULL, 16);
                    i += 2;
                } else {
                    decodedText += tempTexto[i];
                }
            }
            textoNoDisplay = decodedText;

            Serial.print("Texto recebido para exibir: ");
            Serial.println(textoNoDisplay);
           
            atualizarDisplay();
           
            // Responde e redireciona
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=UTF-8");
            client.println();
            client.println("<!DOCTYPE html><html><head><title>Sucesso!</title><meta http-equiv=\"refresh\" content=\"2;url=/\" /></head><body>");
            client.println("<h2>Texto atualizado! Redirecionando...</h2></body></html>");
            break;
          }

          // 3. Se a linha estiver vazia, é o fim dos headers, então envia a página principal
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=UTF-8");
            client.println();
            client.println("<!DOCTYPE html><html><head><title>ESP32 Display</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>");
            client.println("<style>body{font-family: Arial, sans-serif; text-align: center; margin-top: 50px;} form{display: inline-block;}</style>");
            client.println("<h1>Digite o texto para o display:</h1>");
            client.println("<form action=\"/get\">");
            client.println("<input type=\"text\" name=\"texto\" size=\"30\" autofocus>");
            client.println("<input type=\"submit\" value=\"Enviar\">");
            client.println("</form></body></html>");
            break;
          }
         
          // Limpa a linha para a próxima
          currentLine = "";

        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}
