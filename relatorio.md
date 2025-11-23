## Links para formatação do relatório <APAGAR NO FINAL>

- https://github.com/FNakano/CFA2025-SampleProject

# Projeto Patinhas: monitoramento de atividade para Pets de pequeno porte

## Introdução

No cenário imobiliário atual, a verticalização das cidades tornou a vida em apartamentos mais comum do que em casas com quintais. Nesse contexto, a adoção de animais de estimação de pequeno porte tornou-se a norma. Entretanto, a restrição de espaço físico pode levar esses animais a desenvolverem um estilo de vida sedentário, condição que muitas vezes não é observada pelos tutores até que cause problemas de saúde visíveis, como obesidade, problemas articulares ou cardiovasculares.

Através da coleta de dados de movimentação (passometria) do animal, é possível transformar a percepção subjetiva do dono em dados objetivos. O "Projeto Patinhas" visa preencher essa lacuna oferecendo uma solução para monitorar a saúde física do pet.

A solução propõe um design minimalista e robusto, acoplado a um sistema de software que processa o movimento em "passos" e centraliza as informações em uma interface web acessível.

## Objetivos

**Objetivo Geral**

- Desenvolver um sistema completo de IoT (Internet das Coisas) para monitoramento de atividade física de pets.

**Objetivos Específicos**

- Hardware: Prototipar um "pingente" resiliente para acoplamento em coleira, contendo os componentes necessários. Além disso, deservolver o interior de um cachorro de brinquedo para testes e apresentação.

- Firmware: Desenvolver algoritmo para detecção de passos baseado em acelerômetro, filtrando movimentos como balançar a cabeça rapidamente.

- Conectividade: Implementar comunicação Wi-Fi para envio de dados do dispositivo para o servidor local.

- Software/Backend: Criar um webserver (API) para recepção, validação e armazenamento dos dados.

- Interface: Disponibilizar os dados para o usuário final.

## Materiais e Métodos
PLACEHOLDER
Explicar como foi feita:
-escolha do esp32
-calculo dos passos
-escolha da bateria
-escolha do flask pro servidor
-escolha da placa q vamos usar

### Componentes utilizados

### Pingente

Para a construção do pingente, foram selecionados os seguintes componentes:

| Nome | Quantidade | Motivo da escolha |
| --- | --- | --- |
| Microcontrolador (ESP32-C3 Super Mini com display) | 1 | --- |
| Acelerômetro (MPU-9250/6500)| 1 |  --- |
| Bateria (xxxx) | 1 | --- |
| ... | 1 | --- |

+ explicação da passo a passo de criação pingente

### Interior do cachorro de teste

Para a construção do interior do cachorro de brinquedo, foram selecionados os seguintes componentes:

| Nome | Quantidade | Como está conectado com os demais componentes |
| --- | --- | --- |
| Arduino Uno com cabo | 1 | 
| Servos Motores | 4 | 
| Palitos de sorvete pequenos | 4 |
| Algodão | 4 bolinhas | 

+ explicação da passo a passo de criação do cahorro

### App

<colocar as tecnologias usadas e o motivo>

<explicação de forma geral como fez e conectou com o algoritmo do acelerometro>

<colocar como uma pessoa aleatoria pode rodar o servidor com o codigo desse repositorio>

## Resultados

--> Imagem dos compoenetes soldados na placa

--> imagem final do interior do cachorro

--> imagens reais do app

## Conclusões e Comentários

O Projeto Patinhas atingiu o objetivo de criar um sistema funcional de monitoramento pet.

### Comentários

<COLOCAR **BREVEMENTE** A EXPERIÊNCIA FAZENDO O PROJETO E O QUE FOI MAIS FÁCIL/DIFÍCIL DO QUE IMAGINOU QUE SERIA>

Como melhoria futura, sugere-se a implementação de ... <COLOCAR SUGESTÕES PARA MELHORIAS FUTURAS REFERENTE A CADA PARTE (SE HOUVER): PINGENTE, CACHORRO, APP>

## Integrantes
 
- [Arthur Hernandes](https://github.com/arthurHernandess)
- Gabriel Kennuy
- [Stefanie Palmeira](https://github.com/stepalmeira)
- [Willian Jefferson Sousa Farias](https://github.com/willianjsf)
