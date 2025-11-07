#include <WiFi.h>
#include <PubSubClient.h>

// ---------- CONFIG Wi-Fi / MQTT ----------
const char* ssid        = "Wokwi-GUEST";
const char* password    = "";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

// ---------- Perfil médio da jogadora ----------
const float PESO_KG   = 62.0;   // peso médio
const float ALTURA_M  = 1.65;   // não usado no cálculo, mas mantido como referência
const int   IDADE_ANOS = 24;    // idem

// ---------- Variáveis de desempenho ----------
unsigned long inicioPartida = 0;
unsigned long ultimoEnvio   = 0;

float distancia_km = 0.0;   // distância total percorrida
float kcal_gastas  = 0.0;   // calorias totais gastas

// ---------- Funções auxiliares ----------
void setup_wifi() {
  Serial.print("Conectando-se ao WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("WokwiJogadora1")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

// Define o MET de acordo com a velocidade (km/h)
float calcularMET(float velocidade_kmh) {
  if (velocidade_kmh < 6.0)       return 4.0;  // caminhada leve
  else if (velocidade_kmh < 8.0)  return 7.0;  // trote leve
  else if (velocidade_kmh < 12.0) return 9.0;  // corrida moderada
  else                            return 11.0; // corrida intensa
}

// Calcula kcal gastas no intervalo (em minutos) dado velocidade e peso
float calcularKcal(float velocidade_kmh, float deltaMin) {
  float MET = calcularMET(velocidade_kmh);
  float kcal_por_min = (MET * 3.5 * PESO_KG) / 200.0; // fórmula padrão
  return kcal_por_min * deltaMin;
}

// Gera valores simulados de sinais vitais
float simularVelocidade() {
  // 4.0 a 20.0 km/h (jogadora alternando caminhada/corrida)
  return random(40, 200) / 10.0;
}

int simularBPM() {
  // 90 a 190 bpm
  return random(90, 190);
}

float simularTemperaturaCorporal() {
  // 36.0 a 38.5 ºC
  return random(360, 385) / 10.0;
}

// Envia os dados para o broker MQTT em JSON
void envia_dados() {
  unsigned long agora = millis();

  // Calcula tempo desde o último envio
  float deltaSeg = (agora - ultimoEnvio) / 1000.0;
  if (deltaSeg <= 0) deltaSeg = 1; // proteção

  float deltaMin = deltaSeg / 60.0;

  // Sinais atuais
  float velocidade = simularVelocidade();      // km/h
  int   bpm        = simularBPM();
  float temp       = simularTemperaturaCorporal();

  // Atualiza distância percorrida: v (km/h) * tempo (h)
  float deltaHoras = deltaSeg / 3600.0;
  distancia_km += velocidade * deltaHoras;

  // Atualiza kcal gastas
  kcal_gastas += calcularKcal(velocidade, deltaMin);

  // Tempo total de partida (segundos desde o início)
  unsigned long tempoExecSeg = (agora - inicioPartida) / 1000;

  // Monta o JSON
  String payload = "{";
  payload += "\"velocidade\":"   + String(velocidade, 1);
  payload += ",\"bpm\":"         + String(bpm);
  payload += ",\"temp\":"        + String(temp, 1);
  payload += ",\"kcal\":"        + String(kcal_gastas, 1);
  payload += ",\"distancia_km\":"+ String(distancia_km, 3);
  payload += ",\"tempo_s\":"     + String(tempoExecSeg);
  payload += "}";

  Serial.println("Enviando dados: " + payload);

  // Publica no tópico da Jogadora 1
  client.publish("passabola/jogadora1", payload.c_str());

  ultimoEnvio = agora;
}

// ---------- Setup / Loop ----------
void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  randomSeed(analogRead(0));

  inicioPartida = millis();
  ultimoEnvio   = millis();
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  envia_dados();
  delay(5000); // envia a cada 5 segundos
}