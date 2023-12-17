#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

#define PIN_SERVO 13
Servo myservo;
const int led = 2;
const int ledArvore = 4;
const int buzzer = 5;
const int pinoPIR = 27;

int state = 0;

const char* ssid = "iPhone de Adauto";
const char* password = "conectaai";
const char* mqtt_server = "broker.hivemq.com";
  

String my_payload;
String my_topic;

WiFiClient adauto; // Cliente de Rede WiFi
PubSubClient client(adauto); // Cria uma instancia de um cliente MQTT

void setup_wifi(){
  delay(10);
  Serial.print("Conectando com... ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connectado");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length){
  String conc_payload;
  for (int i = 0; i < length; i++){
    conc_payload += ((char)payload[i]);
  }
  my_topic = topic;
  my_payload = conc_payload;
}

void reconnect(){
  while(!client.connected()){
    Serial.print("MQTT Conectando...");

    if(client.connect("adauto")){
      Serial.println("connectado");
      client.subscribe("PDARECEBE");
    } else {
      Serial.print("Conexão falhada, rc=");
      Serial.print(client.state());
      Serial.println("Tentando novamente em 5seg...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  myservo.attach(PIN_SERVO);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(pinoPIR, INPUT);
  pinMode(ledArvore, OUTPUT);
}

void abrirPortao(){
  myservo.write(0);
  myservo.write(90);
}

void fecharPortao(){
  myservo.write(90);
  myservo.write(0);
}

void Controle(){
 if(my_payload == "Abreportao"){
  abrirPortao();
 }
 if(my_payload == "Fechaportao"){
  fecharPortao();
 }
 if(my_payload == "Ligaalarme"){
  state = digitalRead(pinoPIR);
  if(state == 1){
    digitalWrite(led, HIGH);
    tone(buzzer, HIGH);
    delay(5000);
  } else {
    tone(buzzer, LOW);
    digitalWrite(led, LOW);
  }
 }
 if(my_payload == "Desligaalarme"){
  digitalWrite(led, LOW);
  tone(buzzer, LOW);
 }
 if(my_payload == "arvoreOn"){
  digitalWrite(ledArvore, HIGH);
  Serial.println("opa, arvore?");
 }
 if(my_payload == "arvoreOff"){
  digitalWrite(ledArvore, LOW);
 }
}

void loop() {
  reconnect();
  Controle();
  client.loop();
  delay(1000);
}
