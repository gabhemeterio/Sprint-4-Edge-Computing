# Passa a Bola – Desafio de Desempenho entre Jogadoras (Arquitetura IoT)

## 1. Integrantes

- Alana Vieira Batista | RM 563796 
- Felippe Nascimento Silva | RM 562123
- Gabriel S. Hemeterio | RM 566243
- Kawan Oliveira Amorim | RM 562197
- Matheus Hideki Doroszewski Yoshimura | RM 564970

> Projeto desenvolvido para a **Sprint 4 – Aplicação Prática da Arquitetura IoT**  
> Curso: (Engenharia de Software - ESPZ1)

---

## 2. Visão geral do projeto

O **Passa a Bola IoT** é uma prova de conceito que simula um **desafio de desempenho entre jogadoras de futebol** usando uma arquitetura IoT completa:

- **Dispositivos IoT** (ESP32 simulados no Wokwi) representando as jogadoras.  
- **Comunicação via MQTT** usando o broker público HiveMQ.  
- **Processamento e visualização em tempo real** com Node-RED Dashboard, em um painel com layout inspirado no app **Passa a Bola**.

Cada jogadora envia, em tempo real, métricas de desempenho como:

- **BPM** (frequência cardíaca)  
- **Velocidade** (km/h)  
- **Kcal** gastas  
- **Distância** percorrida (km)  
- **Tempo** de partida (segundos → minutos)  

A plataforma calcula uma **pontuação de desempenho** para cada jogadora e monta um **ranking ao vivo** da partida.

---

## 3. Objetivo da Sprint

Atender aos critérios da Sprint 4:

- Demonstrar comunicação entre dispositivos IoT e plataforma usando **MQTT**.  
- Implementar **publicação e subscrição** dos dados em tempo real.  
- Integrar com uma plataforma de visualização (Node-RED Dashboard).  
- Garantir **reprodutibilidade** através de código versionado e instruções passo a passo.

---

## 4. Arquitetura IoT

A arquitetura geral do projeto é mostrada na figura abaixo:

> ![Arquitetura IoT – Passa a Bola](assets/Arquitetura%20do%20Projeto.png)

A arquitetura segue o fluxo de dados completo:
os **ESP32 das jogadoras** enviam dados de desempenho via **Wi-Fi e MQTT (HiveMQ)**, o **Node-RED** processa e calcula as pontuações, e o **dashboard Passa a Bola** exibe tudo em tempo real com layout inspirado no aplicativo.

---

## 5. Camadas da Arquitetura

### 5.1. Dispositivos IoT – Jogadoras (ESP32 / Wokwi)
- **Jogadora 1 – ESP32**
  - Gera e envia valores simulados de:
    - BPM, Velocidade, Kcal, Distância percorrida e Tempo de partida.  
  - Publica no tópico MQTT: `passabola/jogadora1`

- **Jogadora 2 – ESP32**
  - Mesmo conjunto de métricas, com valores independentes.  
  - Publica no tópico MQTT: `passabola/jogadora2`

Ambos se conectam via Wi-Fi (no Wokwi, SSID `Wokwi-GUEST`).

---

### 5.2. Comunicação – Protocolo MQTT

- **Broker público HiveMQ**
  - Endereço: `broker.hivemq.com`
  - Porta: `1883`
  - Protocolo: MQTT 3.1.1

Tópicos usados:
- `passabola/jogadora1` – dados da Jogadora 1  
- `passabola/jogadora2` – dados da Jogadora 2  

Formato das mensagens (JSON):

```json
{
  "velocidade": 15.2,
  "bpm": 140,
  "kcal": 120.5,
  "distancia_km": 1.8,
  "tempo_s": 600
}
```

---

### 5.3. Plataforma – Node-RED

**Nós principais:**
- `mqtt in Jogadora 1` → assina `passabola/jogadora1`  
- `mqtt in Jogadora 2` → assina `passabola/jogadora2`  

Para cada jogadora:
- `function Calcular J1` / `function Calcular J2`
  - Converte tempo para minutos.  
  - Normaliza BPM, velocidade, kcal e distância.  
  - Calcula a pontuação final.

Depois:
- `function Combinar jogadoras`
  - Junta os dados de ambas.  
  - Gera texto de ranking.  
  - Envia payload consolidado para o dashboard (`ui_template`).

---

### 5.4. Visualização – Dashboard Passa a Bola

- Construído em um único `ui_template` (HTML + CSS) no Node-RED Dashboard.  
- Layout inspirado no app **Passa a Bola**, com menu lateral e cards de desempenho.

Cada card de jogadora exibe:
- BPM, velocidade, kcal, distância (km) e tempo (min).  
- Pontuação calculada.  
- **Gauges circulares** (BPM, velocidade, energia, distância e tempo).  
- Card de **ranking** atualizando automaticamente.

---

## 6. Cálculo de Pontos de Desempenho

1. **Conversão de tempo:**
   ```
   tempo_min = tempo_s / 60
   ```

2. **Normalização de métricas (0 a 100%)**
   - BPM: 90–190 bpm  
   - Velocidade: 4–20 km/h  
   - Kcal: até 500 kcal  
   - Distância: até 5 km  

3. **Fórmula da pontuação final:**
   ```
   pontos = 0.3*BPM_norm + 0.25*Vel_norm + 0.25*Kcal_norm + 0.2*Dist_norm
   ```

4. O nó final cria o objeto:
   ```json
   {
     "jogadora1": {...},
     "jogadora2": {...},
     "ranking_texto": "1º Jogadora 2 - 55.35 pts\n2º Jogadora 1 - 38.01 pts"
   }
   ```

---


## 7. Manual Passo a Passo

### 7.1. Pré-requisitos
- Node.js + Node-RED instalados  
- Acesso à internet  
- Navegador web  
- Conta Wokwi (para simular ESP32)

---

### 7.2. Clonar o repositório

```bash
git clone <URL_DO_REPOSITORIO>
cd passa-bola-iot
```

---

### 7.3. Importar o flow no Node-RED
1. Acesse `http://localhost:1880`  
2. Menu (☰) → Import  
3. Selecione `flows.json`  
4. Clique em **Import** e depois em **Deploy**

---

### 7.4. Conferir configuração MQTT
- Server: `broker.hivemq.com`  
- Port: `1883`  
- Tópicos: `passabola/jogadora1` e `passabola/jogadora2`

---

### 7.5. Executar simulação no Wokwi

1. Acesse **https://wokwi.com**  
2. Crie dois projetos ESP32:
   - `jogadora1.ino`
   - `jogadora2.ino`
3. Cole o código respectivo.  
4. Execute ambos (Start Simulation).  
5. Veja os dados JSON sendo publicados.

---

### 7.6. Visualizar o Dashboard

1. Acesse `http://localhost:1880/ui`  
2. Entre na aba **Passa a Bola**  
3. Acompanhe:
   - BPM, velocidade, kcal, distância, tempo  
   - Pontuação e ranking em tempo real  
   - Gauges circulares com dados atualizados

---

## 8. Resultados

- Comunicação entre os dois ESP32 simulados e o Node-RED foi bem-sucedida.  
- O painel exibe métricas e ranking dinâmico.  
- Atualizações ocorrem a cada 5 segundos.  
- Layout visual segue o estilo do app **Passa a Bola**, com design responsivo e limpo.

---

## 9. Melhorias Futuras

- Adicionar persistência de dados (MongoDB/InfluxDB).  
- Integrar API com o app Passa a Bola real.  
- Inserir mapa de calor de movimentação das jogadoras.  
- Criar opção de “nova partida” com histórico e resultados.

---

## 10. Conclusão

O projeto **Passa a Bola IoT** demonstra uma aplicação prática da arquitetura IoT,
desde a **coleta e envio de dados simulados (ESP32)** até o **processamento e visualização em tempo real (Node-RED)**.

A solução evidencia:
- Comunicação via MQTT  
- Cálculo automatizado de desempenho  
- Ranking dinâmico e interface intuitiva  

---
