# 📝 Relatório de aprendizado

Aqui vamos relatar nossas descobertas ao longo do semestre.

## 📒 Pesquisas individuais
- [Anotações do Willian](https://docs.google.com/document/d/1L0mPsEgcZPG29M-nSPczp4rMd2Pr4x98OYEAvyfv0Eg/edit?usp=sharing)
- [Anotações da Stefanie](https://github.com/stepalmeira)
- [Anotações do Arthur - doc. display do ESP32c3](https://docs.google.com/document/d/17O6CZThYMCn8GwaPOuoTraIOa0sEShhfARCrMZcj9_8/edit?usp=sharing)
- [Anotações do Gabriel](https://docs.google.com/document/d/1JIy1R8I9HfKGUwXwEe_GVcWvw83TuC-qe8YNm9TS3fU/edit?usp=sharing)

## 🔧 Componentes

####  1. ESP32-C3 Super Mini OLED Display de 0.42''
<img src="/imagens/esp32c3supermini.png" alt="ESP32-C3 Super Mini" width="50%"/>

##### 1.1. Encontrando uma biblioteca
##### 1.2. Enviando mensagens para o display



#### 2. MPU-9250/6500
<img src="/imagens/mpu9250.png" alt="ESP32-C3 Super Mini" width="25%"/>

Este módulo contém um sensor acelerômetro de 3 eixos, um giroscópio de 3 eixos e um magnetômetro de 3 eixos

##### 2.1. Encontrando uma biblioteca
##### 2.2. Recebendo dados crus do acelerômetro

#### 3. Fonte de energia (bateria)

## 🪛 Conexões físicas
<img src="/imagens/conexao_esp32c3.png" alt="ESP32-C3 Super Mini" width="60%"/>

## 🛜 Conexão sem fio

## 📱 Aplicativo
O aplicativo foi desenvolvido utilizando React Native com Expo, a partir da seguinte documentação: https://reactnative.dev/
Link para o repositório contendo a versão final do app: https://github.com/arthurHernandess/CFA-patinhas-app

## ⚙️ Backend
O Backend foi feito em Python com o uso da biblioteca Flask, permitindo criação de endpoint HTTP. O ESP32 se comunica com o backend enviando os passos do animal a cada 30s, e o aplicativo se comunica também com esse backend, recebendo os passos diários do animal e gerando gráficos e informações úteis ao dono do pet.

## 🔋 Bateria

### LilyPad Power Supply 
<img src="/imagens/lilypadpowersupply.png" alt="ESP32-C3 Super Mini" width="50%"/>

O LilyPad Power Supply possui a capacidade de armazenar uma pilha AAA comum e amplificar sua tensão para até 5V, podendo ter uma tensão de entrada variável de 1,2 até 5V. ([Fonte](https://www.eletrogate.com/fonte-de-alimentacao-para-lilypad?srsltid=AfmBOorD8TQTNCVx9LB4w-kpmaVvcd5Fv9Nq4aKqoRTktFCqAG9Ks1Cy))


## 📑 Links e referências
- [Use of pedometers to measure physical activity in dogs](https://avmajournals.avma.org/view/journals/javma/226/12/javma.2005.226.2010.xml?tab_body=pdf)
- [ESP32-C3 0.42 OLED - Kevin's Blog](https://emalliab.wordpress.com/2025/02/12/esp32-c3-0-42-oled/)
