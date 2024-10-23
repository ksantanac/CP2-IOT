#include <WiFi.h>           // Biblioteca para conectar o ESP32 à rede Wi-Fi
#include <PubSubClient.h>    // Biblioteca para usar o protocolo MQTT (Publicar/Assinar)

// Definição das credenciais de conexão Wi-Fi
const char* ssid = "Wokwi-GUEST";  // Nome da rede Wi-Fi
const char* password = "";         // Senha da rede Wi-Fi (nesse caso, sem senha)

// Definição do endereço do Broker MQTT
const char* mqtt_server = "test.mosquitto.org";  // Endereço do servidor MQTT
const char* mqtt_topic = "temperature";          // Tópico onde a temperatura será recebida

// Objetos necessários para a conexão Wi-Fi e MQTT
WiFiClient espClient;           // Cliente Wi-Fi para conectar ao roteador
PubSubClient client(espClient); // Cliente MQTT para conectar ao Broker

// Pino onde o LED está conectado
const int ledPin = 2;  
float temperature = 0;  // Variável para armazenar a temperatura recebida

// Função de configuração inicial
void setup() {
  Serial.begin(115200);    // Inicializa a comunicação serial
  pinMode(ledPin, OUTPUT); // Configura o pino do LED como saída

  setup_wifi();            // Chama a função para configurar o Wi-Fi
  
  client.setServer(mqtt_server, 1883); // Configura o cliente MQTT para o Broker
  client.setCallback(callback);       // Define a função callback para lidar com as mensagens recebidas
  reconnect();                        // Tenta se conectar ao Broker MQTT
}

// Função para configurar a conexão Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.print("Conectando-se a ");
  Serial.println(ssid);  // Imprime o nome da rede que estamos tentando conectar

  WiFi.begin(ssid, password);  // Inicia o processo de conexão com o Wi-Fi

  // Enquanto não estiver conectado, a função fica em loop, esperando a conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);            // Aguarda 500 ms antes de tentar novamente
    Serial.print(".");     // Imprime um ponto para indicar que ainda está tentando
  }

  // Quando conectado, imprime a confirmação e o endereço IP do ESP32
  Serial.println("\nWiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());  // Exibe o endereço IP atribuído pelo roteador
}

// Função para reconectar ao Broker MQTT, caso a conexão se perca
void reconnect() {
  // Tenta reconectar até que consiga
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    if (client.connect("ESP32ClientReceiver")) {  // Tenta se conectar ao Broker MQTT com o nome "ESP32ClientReceiver"
      Serial.println("conectado");
      client.subscribe(mqtt_topic);  // Assina o tópico onde receberemos as temperaturas
    } else {
      // Caso não consiga se conectar, imprime o erro e tenta novamente após 5 segundos
      Serial.print("falha, rc=");
      Serial.print(client.state());  // Exibe o código do erro
      Serial.println(" tente novamente em 5 segundos");
      delay(5000);  // Aguarda 5 segundos antes de tentar reconectar
    }
  }
}

// Função callback que é chamada quando uma mensagem MQTT é recebida
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  // Converte o payload recebido (byte array) em uma String
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  // Converte a mensagem recebida em temperatura (float) e armazena na variável 'temperature'
  temperature = message.toFloat();
  Serial.println("Temperatura recebida: " + String(temperature) + "ºC");  // Exibe a temperatura recebida no monitor serial
}

// Função principal que roda em loop
void loop() {
  // Verifica se o cliente MQTT ainda está conectado, caso não esteja, tenta reconectar
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // Mantém a conexão ativa e verifica a chegada de novas mensagens

  // Controle do LED baseado na temperatura recebida
  if (temperature > 30) {             // Se a temperatura for maior que 30°C
    digitalWrite(ledPin, HIGH);       // Liga o LED
  } else {                            // Caso contrário
    digitalWrite(ledPin, LOW);        // Desliga o LED
  }

  delay(1000);  // Aguarda 1 segundo antes de repetir o processo
}
