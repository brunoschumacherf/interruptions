## 📌 Visão Geral

Este projeto tem como foco aprofundar o conhecimento sobre **interrupções** no microcontrolador **RP2040**, utilizando a placa de desenvolvimento **BitDogLab**. Durante sua implementação, são abordadas diversas técnicas essenciais, incluindo:

- **Interrupções via IRQ**
- **Debouncing** para botões
- Controle de **LEDs comuns** (incluindo LED RGB)
- Controle de **LEDs endereçáveis WS2812**
- Uso de **resistores pull-up internos**

O objetivo é integrar hardware e software para criar uma aplicação interativa e funcional.

---

## 🎯 Metas do Projeto

- **Entender e utilizar interrupções** em microcontroladores.
- **Implementar debouncing** para garantir leituras precisas dos botões.
- **Gerenciar LEDs comuns e endereçáveis** para exibição de informações.
- **Explorar resistores pull-up internos** para botões.
- **Criar uma aplicação funcional**, unindo entradas (botões) e saídas (LEDs e matriz de exibição).

---

## 🔩 Componentes Utilizados

- **Matriz 5x5 de LEDs WS2812 (endereçáveis):**
  - **GPIO:** Conectada à GPIO 7.
  - **Função:** Exibição de números de 0 a 9 com efeitos visuais.
- **LED RGB Comum:**
  - **GPIOs:** Conectado às GPIOs 11 (verde), 12 (azul) e 13 (vermelho).
  - **Função:** O LED vermelho pisca continuamente 5 vezes por segundo.
- **Botão A:**
  - **GPIO:** Conectado à GPIO 5.
  - **Função:** Aumenta o número exibido na matriz ao ser pressionado.
- **Botão B:**
  - **GPIO:** Conectado à GPIO 6.
  - **Função:** Diminui o número exibido na matriz ao ser pressionado.

---

## 📂 Estrutura do Projeto

```plaintext
Interruptions/
├── .gitignore
├── CMakeLists.txt
├── LICENSE
├── main.c
├── pico_sdk_import.cmake
└── README.md
```

---

## 🚀 Funcionalidades Implementadas

### 1. LED RGB – Piscar Vermelho

- O LED vermelho pisca continuamente com frequência de 5 Hz, servindo como indicação visual do funcionamento.

### 2. Interrupções e Debouncing para Botões

- **Botão A:** Aumenta o valor exibido na matriz de LEDs ao ser pressionado.
- **Botão B:** Diminui o valor exibido na matriz de LEDs ao ser pressionado.
- **Debouncing:** Implementado via software para garantir leituras confiáveis.

### 3. Exibição de Números na Matriz WS2812

- A matriz de LEDs exibe números de 0 a 9 com efeitos visuais.
- O formato pode seguir um estilo digital fixo ou ser mais criativo, desde que o número seja identificável.

## 🔧 Requisitos Técnicos

- **Interrupções:** Uso de IRQs para tratar as entradas dos botões.
- **Debouncing:** Implementação via software para leituras confiáveis.
- **Controle de LEDs:** Gestão eficiente de LEDs comuns e endereçáveis.
- **Código bem estruturado e comentado**, visando facilidade de manutenção.

---

## ⚙️ Instalação e Execução

### 1. Configuração do Ambiente

- Certifique-se de que o [Pico SDK](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) está instalado e configurado corretamente.
- Verifique se todas as dependências necessárias estão instaladas.

### 2. Clonando o Repositório

Para obter o código do projeto, utilize:

```bash
git clone https://github.com/brunoschumacherf/interruptions
```

### 3. Compilação e Upload do Código

No VS Code com o ambiente do Pico SDK configurado, execute:

```bash
mkdir build
cd build
cmake ..
make
```

Após a compilação, copie o arquivo `.uf2` gerado para o Raspberry Pi Pico (certifique-se de que a placa esteja no modo bootloader).

---

## 📁 Entregáveis

- Código-fonte disponível no repositório.
- Vídeo demonstrativo: [Vídeo](https://drive.google.com/file/d/1xheecxIxhoxraP3RAJNPTQlgTh7RUu12/view?usp=sharing)

_Desenvolvido por Bruno Schumacher_

