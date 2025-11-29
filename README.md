# 🐾 Projeto Patinhas

Repositório público para apresentação do projeto desenvolvido por alunos do curso de Sistemas de Informação na disciplina Computação Física e Aplicações (2025) na EACH-USP com orientação do professor doutor [Fábio Nakano](https://github.com/FNakano/CFA).

## ✏️ Resumo do projeto

Acessório de coleira que conta os passos do pet e envia essas informações para um app mobile. O tutor acompanha a atividade física diária do animal, com a contagem sendo reiniciada a cada dia. Os dados ficam armazenados em uma base para geração de gráficos e histórico de atividade.


![Capa](Relatorios/imagens/capa.png)


## 🔧 Componentes

- Placa microcontroladora ESP32-C3 Super Mini OLED Display de 0.42''
- Módulo MPU-9250/6500: Este módulo contém um sensor acelerômetro de 3 eixos, um giroscópio de 3 eixos e um magnetômetro de 3 eixos
- Módulo para carregamento de bateria 
- Bateria Li-po 3.7V 300mAh
- Case de proteção

<sup>Obs.: A bateria, o módulo de carregamento e a case foram reciclados de uma caixinha carregadora de fones de ouvido bluetooth</sup>

![Circuito](Relatorios/imagens/circuito_resultado.png)


##  🐕‍🦺 Como o algoritmo detecta passos

1. ESP32 faz a leitura de sensores
    - Lê aceleração nos 3 eixos e calcula sua magnitude
    - Lê também o giroscópio para detectar rotações bruscas

2. A magnitude da aceleração passa por um filtro de média movel que suaviza ruídos e vibrações rápidas
   
3. Detecção de rotação: Se o pet faz uma movimentação muito brusca (rotação alta), o algoritmo entra em cooldown e ignora possíveis “falsos passos”

4. Máquina de estados para detectar passos:
    - O algoritmo procura primeiro um pico de aceleração (indicando o início do passo)
    - Depois espera a volta ao nível de repouso (vale), confirmando que o passo foi completo
    - Também verifica tempo mínimo entre passos e timeout para evitar falsos positivos

5. Quando um passo é confirmado, incrementa o contador e envia um POST para o servidor com o valor 1

6. O número total de passos é atualizado no display do ESP32

## 📱 Aplicativo mobile
O app (React) consome uma API fornecida por um backend em Python, que funciona como ponte entre o ESP32 e o aplicativo. O ESP32 detecta passos, encontra o servidor automaticamente via UDP Broadcast e envia os dados em POSTs periódicos. O backend (Flask) recebe esses passos, armazena o histórico e disponibiliza os valores via HTTP para o app, que atualiza as informações do pet em quase tempo real.
Para usar o sistema, basta rodar o server.py localmente (na mesma rede do ESP32 e do celular) e abrir o app do repositório.


## 📦 Dependências de software

#### Dispositivo físico

- Arduino IDE
- Bibliotecas externas:
  -  [MPU9250 por hideakitai](https://github.com/hideakitai/MPU9250) (Comunicação com MPU9250)
  -  [U8g2lib por olikraus](https://github.com/olikraus/u8g2) (Comunicação com display)

#### Backend
- Python
- Flask 
- Zeroconf
  
#### Frontend
- Node.js
- Expo
- React Native
- TypeScript

## 🚀 Demonstração
![Imagem da tela inicial do app](Relatorios/imagens/teste_real.gif)

 
## 🗣️ Dúvidas e Ajuda

Ficou interessado pelo projeto e está com alguma dúvida? Ficou perdido ou confuso? Ou quer sugerir alguma melhoria ao projeto?

Incentivamos que você nos procure. **Abra uma Issue** na página de Issues contando seu problema/sugestão.

## 👨‍💻 Autores
 
- [Arthur Hernandes](https://github.com/arthurHernandess)
- [Gabriel Kennuy](https://github.com/fosfro)
- [Stefanie Palmeira](https://github.com/stepalmeira)
- [Willian Farias](https://github.com/willianjsf)


