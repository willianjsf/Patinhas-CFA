#include <Servo.h>

// Cria os objetos servo para cada perna
Servo frontLeftLeg;
Servo frontRightLeg;
Servo backLeftLeg;
Servo backRightLeg;

// Define as conexões dos pinos
const int frontRightPin = 8;
const int frontLeftPin = 9; //
const int backLeftPin = 13;
const int backRightPin = 12; //

// --- CALIBRAÇÃO (TRIM) ---
// Coloque aqui os números que você encontrar no teste!
const int frontLeftOffset = 15;   // (Ex: -4)
const int frontRightOffset = 36;  // (Ex: 2)
const int backLeftOffset = 75;    // (Ex: 3)
const int backRightOffset = 10;   // (Ex: -1)


// Define os ângulos de movimento
int forwardAngle = 80;   // Perna para a frente
int backwardAngle = 110; // Perna para trás (dando o "passo")
int restAngle = 90;      // Posição de descanso (perpendicular)

// Ângulos invertidos para as pernas direitas (para lógica de espelho)
// Corrigindo os comentários para refletir os ângulos 30/120
int rightForwardAngle = backwardAngle; // Será 120
int rightBackwardAngle = forwardAngle; // Será 30

// --- MELHORIA: Variáveis de tempo ---
int stepTime = 600;     // Tempo total de cada passo (em ms)
int staggerDelay = 100;  // Pequena pausa para dividir o pico de energia

void setup() {
  // Anexa os servos aos pinos (SOMENTE AQUI)
  frontLeftLeg.attach(frontLeftPin);
  frontRightLeg.attach(frontRightPin);
  backLeftLeg.attach(backLeftPin);
  backRightLeg.attach(backRightPin);

  // "Zera" (inicia) todas as pernas em 90° (COM OFFSET APLICADO)
  frontLeftLeg.write(restAngle + frontLeftOffset);
  frontRightLeg.write(restAngle + frontRightOffset);
  backLeftLeg.write(restAngle + backLeftOffset);
  backRightLeg.write(restAngle + backRightOffset);

  delay(1000); // Espera os servos se estabilizarem
}

void loop() {
  // --- PASSO DE TROTE 1 (Escalonado) ---
  
  // O Par 1 (Frente-Esquerda e Trás-Direita) move-se PARA TRÁS (empurrando)
  backRightLeg.write(rightBackwardAngle + backRightOffset);  // Empurra para trás (30)
  frontLeftLeg.write(backwardAngle + frontLeftOffset); // Empurra para trás (120)
//frontLeftLeg.write(backwardAngle + frontLeftOffset); // Empurra para trás (120)
  // Pequena pausa para dividir o pico de energia
  delay(staggerDelay); 

  // O Par 2 (Frente-Direita e Trás-Esquerda) move-se PARA FRENTE (se preparando)
  backLeftLeg.write(forwardAngle + backLeftOffset);   // Prepara para frente (30)
  frontRightLeg.write(rightForwardAngle + frontRightOffset); // Prepara para frente (120)

  // Espera o resto do tempo do passo
  delay(stepTime - staggerDelay); 

  // --- PASSO DE TROTE 2 (Escalonado) ---
  
  // O Par 1 (Frente-Esquerda e Trás-Direita) move-se PARA FRENTE (se preparando)
  backRightLeg.write(rightForwardAngle + backRightOffset);  // Prepara para frente (120)
  frontLeftLeg.write(forwardAngle + frontLeftOffset);   // Prepara para frente (30)
  //frontLeftLeg.write(forwardAngle + frontLeftOffset);   // Prepara para frente (30)

  // Pequena pausa
  delay(staggerDelay);

  // O Par 2 (Frente-Direita e Trás-Esquerda) move-se PARA TRÁS (empurrando)
  backLeftLeg.write(backwardAngle + backLeftOffset); // Empurra para trás (120)
  frontRightLeg.write(rightBackwardAngle + frontRightOffset); // Empurra para trás (30)

  // Espera o resto do tempo do passo
  delay(stepTime - staggerDelay); 
}
