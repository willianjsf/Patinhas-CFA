// Código simples para conexão ao wifi eduroam para controlar a led do ESP32 por meio de um site a partir do número IP.

#include <WiFi.h>
#include "esp_eap_client.h" // Biblioteca ATUALIZADA para autenticação Enterprise

// --- Configurações da Rede Eduroam (ALTERE AQUI) ---
const char* ssid = "eduroam"; // Nome da rede
const char* EAP_IDENTITY = "seunumerousp@usp.br"; // Geralmente seu e-mail institucional completo
const char* EAP_PASSWORD = "senha"; // Sua senha da rede

// O pino do LED que você quer controlar.
const int ledPin = 2;

// Cria um objeto de servidor na porta 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); // Define o pino do LED como saída

  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  // Define o modo Wi-Fi para estação (cliente)
  WiFi.mode(WIFI_STA);

  // --- Bloco de conexão ATUALIZADO para a nova biblioteca ---
  esp_eap_client_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY)); // Define a identidade EAP
  // Para a maioria das redes eduroam, o "usuário anônimo" ou "username" é o mesmo que a identidade.
  esp_eap_client_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_eap_client_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
 
  // Habilita a conexão enterprise
  esp_wifi_sta_enterprise_enable();
  // -----------------------------------------------------------

  // Inicia a conexão Wi-Fi
  WiFi.begin(ssid);

  // Espera a conexão ser estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado com sucesso!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicia o servidor web
  server.begin();
}

void loop() {
  // O código do loop continua exatamente o mesmo
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Novo Cliente.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE html><html>");
            client.println("<head><title>Controle do LED - ESP32</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
            client.println("<body><h1>Controle do LED no pino 5</h1>");
            client.println("<p><a href=\"/H\"><button style=\"font-size:24px;\">LIGAR</button></a></p>");
            client.println("<p><a href=\"/L\"><button style=\"font-size:24px;\">DESLIGAR</button></a></p>");
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /H")) {
          digitalWrite(ledPin, HIGH);
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(ledPin, LOW);
        }
      }
    }
    client.stop();
    Serial.println("Cliente Desconectado.");
  }
}
