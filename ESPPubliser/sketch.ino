#include <WiFi.h>           // Biblioteca para conectar o ESP32 à rede Wi-Fi
#include <PubSubClient.h>    // Biblioteca para usar o protocolo MQTT (Publicar/Assinar)
#include "DHTesp.h"          // Biblioteca para lidar com o sensor DHT (para temperatura e umidade)

// Definição das credenciais de conexão Wi-Fi
const char* ssid = "Wokwi-GUEST";  // Nome da rede Wi-Fi
const char* password = "";         // Senha da rede Wi-Fi (nesse caso, não há senha)

// Definição do endereço do Broker MQTT
const char* mqtt_server = "test.mosquitto.org";  // Endereço do servidor MQTT
const char* mqtt_topic = "temperature";          // Tópico onde vamos publicar as informações de temperatura

// Objetos necessários para a conexão Wi-Fi e MQTT
WiFiClient espClient;           // Cliente Wi-Fi para conectar ao roteador
PubSubClient client(espClient); // Cliente MQTT para conectar ao Broker

// Pino onde o sensor DHT22 está conectado
const int dhtPin = 15;  
DHTesp dhtSensor;              // Objeto para lidar com o sensor DHT22

// Função de configuração inicial
void setup() {
  Serial.begin(115200);                   // Inicializa a comunicação serial com velocidade de 115200 bps
  dhtSensor.setup(dhtPin, DHTesp::DHT22); // Configura o sensor DHT22 no pino 15

  setup_wifi();                           // Chama a função para configurar o Wi-Fi
  
  client.setServer(mqtt_server, 1883);    // Configura o cliente MQTT para usar o Broker "test.mosquitto.org" na porta 1883
}

// Função para configurar a conexão Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);  // Imprime o nome da rede que estamos tentando conectar

  WiFi.begin(ssid, password);  // Inicia o processo de conexão com o Wi-Fi

  // Enquanto não estiver conectado, a função fica em loop, esperando a conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);            // Aguarda 500 ms antes de tentar novamente
    Serial.print(".");     // Imprime um ponto para indicar que ainda está tentando
  }

  // Quando conectado, imprime a confirmação e o endereço IP do ESP32
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());  // Exibe o endereço IP atribuído pelo roteador
}

// Função principal que roda em loop
void loop() {
  // Verifica se a conexão com o Broker MQTT está ativa, se não estiver, reconecta
  if (!client.connected()) {
    client.connect("ESP32ClientPublisher");  // Conecta o cliente MQTT com o nome "ESP32ClientPublisher"
  }
  client.loop();  // Mantém a conexão ativa com o Broker

  // Lê os dados de temperatura e umidade do sensor DHT22
  TempAndHumidity data = dhtSensor.getTempAndHumidity();  
  String temperature = String(data.temperature, 2);  // Converte a temperatura lida em uma string com 2 casas decimais
  
  // Exibe no monitor serial a temperatura que está sendo publicada
  Serial.println("Publicando Temperatura: " + temperature);
  
  // Publica a temperatura no tópico "temperature" do Broker MQTT
  client.publish(mqtt_topic, temperature.c_str());

  delay(2000);  // Aguarda 2 segundos antes de repetir o processo
}
