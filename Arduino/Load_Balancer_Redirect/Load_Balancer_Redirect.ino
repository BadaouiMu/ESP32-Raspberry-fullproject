#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "Groupe_MMYH";
const char *password = "+rlsdQrHudg)|(zqd-Y]@};Fd";

  ////// supp ///////////////////////////
unsigned long previousMillis = 0;
const long interval = 5000;
  ////// supp ///////////////////////////

IPAddress staticIP (192, 168, 4, 250);
IPAddress gateway (192, 168, 4, 1);
IPAddress subnet (255, 255, 255, 0);
IPAddress dns (192, 168, 4, 1);

AsyncWebServer server(80);


const char *mqtt_server = "192.168.4.254";
const int mqtt_port = 1883;
const char *mqtt_user = "admin";
const char *mqtt_password = "xhc9QmmISs";

WiFiClient espClient;
PubSubClient client_mqtt(espClient);

const char *server1IP = "192.168.4.252";
const char *server2IP = "192.168.4.253";
const char *topicChargeServer1 = "serveur1/charge";
const char *topicChargeServer2 = "serveur2/charge";

int chargeServeur1 = 0;
int chargeServeur2 = 0;

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");

  String payloadStr = "";
  for (int i = 0; i < length; i++) {
    payloadStr += (char)payload[i];
  }

  DynamicJsonDocument doc(256); 
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
  client_mqtt.setServer(mqtt_server, 1883);
  Serial.print("Tentative de connexion MQTT...");
    if (client_mqtt.connect("ESP32Client_lb", mqtt_user, mqtt_password)) {
      Serial.println("Connecté");
      client_mqtt.subscribe(topicChargeServer1);
      client_mqtt.subscribe(topicChargeServer2);
      client_mqtt.setCallback(callback);
    } else {
      Serial.println(" [!] Failed to connect to MQTT broker. ");
    }
  }

void setup() {
  Serial.begin(115200);
  setup_wifi();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (!client_mqtt.connected()) {
    reconnect();
    }
    Serial.println("Charge Serveur 1: " + String(chargeServeur1));
    Serial.println("Charge Serveur 2: " + String(chargeServeur2));
    if (chargeServeur1 <= chargeServeur2) {
      request->redirect("http://" + String(server1IP));
    } else {
      request->redirect("http://" + String(server2IP));
    }
  });

  server.begin();
}

void loop() {
  client_mqtt.loop();

  ////// supp ///////////////////////////
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
    ////// supp ///////////////////////////

}
