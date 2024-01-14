#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "Groupe_MMYH";
const char *password = "+rlsdQrHudg)|(zqd-Y]@};Fd";
const char *mqtt_server = "192.168.4.254";
const int mqtt_port = 1883;
const char *mqtt_user = "admin";
const char *mqtt_password = "xhc9QmmISs";

unsigned long previousMillis = 0;
const long interval = 5000;

IPAddress staticIP (192, 168, 4, 250);
IPAddress gateway (192, 168, 4, 1);
IPAddress subnet (255, 255, 255, 0);
IPAddress dns (192, 168, 4, 1);


WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

const char *server1IP = "192.168.4.252";
const char *server2IP = "192.168.4.253";
const char *topicChargeServer1 = "serveur1/charge";
const char *topicChargeServer2 = "serveur2/charge";

int chargeServeur1 = 0;
int chargeServeur2 = 0;

void forwardRequest(String serverIP, String uri) {
  WiFiClient client;
  if (client.connect(serverIP.c_str(), 80)) {
    client.print("GET " + uri + " HTTP/1.1\r\n" +
                 "Host: " + serverIP + "\r\n" +
                 "Connection: close\r\n\r\n");

    while (client.connected() && !client.available())
      delay(1);
    while (client.available()) {
      server.client().write(client.read());
    }
    client.stop();
  } else {
    Serial.println("Erreur de connexion au serveur : " + serverIP);
    String otherServer = (serverIP == server1IP) ? server2IP : server1IP;
    forwardRequest(otherServer, uri);
  }
}


void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }

  DynamicJsonDocument doc(1024); 
  deserializeJson(doc, payloadStr);

  int chargeValue = doc["charge"];

  if (strcmp(topic, topicChargeServer1) == 0) {
    chargeServeur1 = chargeValue;
  } else if (strcmp(topic, topicChargeServer2) == 0) {
    chargeServeur2 = chargeValue;
  }
}


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connexion à ");
  Serial.println(ssid);

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed."); }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.println("Adresse IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  client.setServer(mqtt_server, 1883);
  Serial.print("Tentative de connexion MQTT...");
    if (client.connect("ESP32Client_lb", mqtt_user, mqtt_password)) {
      Serial.println("Connecté");
      client.subscribe(topicChargeServer1);
      client.subscribe(topicChargeServer2);
      client.setCallback(callback);
    } else {
      Serial.println(" [!] Failed to connect to MQTT broker. ");
    }
  }


void handleRoot() {
  if (!client.connected()) {
    reconnect();
  }

  Serial.println("Charge Serveur 1: " + String(chargeServeur1));
  Serial.println("Charge Serveur 2: " + String(chargeServeur2));

  if (chargeServeur1 <= chargeServeur2) {
    Serial.println(server1IP); 
    forwardRequest(server1IP, "/");
  } else {
    Serial.println(server2IP); 
    forwardRequest(server2IP, "/");
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  server.on("/", HTTP_GET, []() {
    handleRoot();
  });

  server.onNotFound([]() {
    handleNotFound();
  });

  server.begin();
}

void loop() {
  server.handleClient();
  client.loop();
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t heapSize = ESP.getHeapSize();
    uint8_t memoryPercentage = ((heapSize - freeHeap) * 100) / heapSize;

    Serial.println("Adresse IP : " + WiFi.localIP().toString());
    Serial.println("Mémoire utilise : " + String(memoryPercentage) + " %");
    Serial.println("Mémoire free : " + String(freeHeap) + " octet");
    Serial.println("Mémoire total : " + String(heapSize) + " octet");
    previousMillis = currentMillis;

  }
}
