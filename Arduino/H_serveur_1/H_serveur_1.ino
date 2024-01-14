#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <esp_task_wdt.h>
#include <PubSubClient.h>
#include <Time.h>
#include <ArduinoJson.h>

// ce que nous dois changer pour le serveur 2 /////////////
IPAddress staticIP (192, 168, 4, 252); // 1-252 / 2-253
const char *mqtt_topic1 = "serveur1/charge";
const char *mqtt_topic2 = "serveur1/trame";
const char *mqtt_client = "ESP32Client_server1"; 
//////////////////////////////////////////////////////////


const char *ssid = "Groupe_MMYH";
const char *password = "+rlsdQrHudg)|(zqd-Y]@};Fd";

IPAddress gateway (192, 168, 4, 1);
IPAddress subnet (255, 255, 255, 0);
IPAddress dns (192, 168, 4, 1);

unsigned long x = 0 ; 

const char *mqtt_server = "192.168.4.254";
const int mqtt_port = 1883;
const char *mqtt_user = "admin";
const char *mqtt_password = "xhc9QmmISs";

unsigned long previousMillis = 0;
const long interval = 5000;

unsigned long previousMillis_charge = 0;
const long interval_charge = 60000;

unsigned long previousMillis_charge_d = 0;
const long interval_charge_d = 60000*60*24;

WebServer server(80);

int charge = 0;
int charge_actualise = 0; 

WiFiClient espClient;
PubSubClient client_mqtt(espClient);



void handleRoot() {

  String html = "<html><head><style>";
  html += "body {font-family: Arial, sans-serif; text-align: center;}";
  html += "h1 {color: #3333cc;}";
  html += "</style></head><body>";
  html += "<h1>Hello from ESP32!</h1>";
  html += "</body></html>";

 
  server.send(200, "text/html", html);

  charge++ ;
  charge_actualise++ ; 

  DynamicJsonDocument doc1(256);
  doc1["IP"] = server.client().remoteIP();
  doc1["charge"] = charge_actualise;
  char buffer1[256];
  serializeJson(doc1, buffer1);
  client_mqtt.publish(mqtt_topic1, buffer1);

}


void handleNotFound() {
  String message = "Not Found :( \n\n"; 
  server.send(404, "text/plain", message); 
}

void setup(void) {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed."); }

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
     delay(1);
  } 
  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
    
  client_mqtt.setServer(mqtt_server, mqtt_port);
  client_mqtt.setClient(espClient);
  if (client_mqtt.connect(mqtt_client, mqtt_user, mqtt_password))
    {
        Serial.println("[+] Connected to MQTT broker");
    }
  else
    {
  Serial.print("[!] Failed to connect to MQTT broker.");
    }
}

void loop(void) {
  server.handleClient();
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis_charge >= interval_charge)
  {charge_actualise = 0; 
    previousMillis_charge = currentMillis;}

  if (currentMillis - previousMillis_charge_d >= interval_charge_d)
  {charge = 0; 
    previousMillis_charge_d = currentMillis;}

  if (currentMillis - previousMillis >= interval)
  {
    if (!client_mqtt.connected())
    {
        Serial.println("[!] MQTT Disconnected. Reconnecting...");
        if (client_mqtt.connect(mqtt_client, mqtt_user, mqtt_password)) {Serial.println("[+] Reconnected to MQTT broker");}
        else{ Serial.println("[!] Failed to connect to MQTT broker. ");}
    }
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t heapSize = ESP.getHeapSize();
    uint8_t memoryPercentage = ((heapSize - freeHeap) * 100) / heapSize;

    Serial.println("Adresse IP : " + WiFi.localIP().toString());
    Serial.println("Mémoire utilise : " + String(memoryPercentage) + " %");
    Serial.println("Mémoire free : " + String(freeHeap) + " octet");
    Serial.println("Mémoire total : " + String(heapSize) + " octet");

    DynamicJsonDocument doc(256);
    doc["IP"] = WiFi.localIP().toString();
    doc["charge"] = charge;
    doc["MemoireFree"] = freeHeap; 
    doc["MemoireTotal"] = heapSize; 
    doc["MemoireUtilise"] = memoryPercentage; 
    char buffer[256];
    serializeJson(doc, buffer);
    client_mqtt.publish(mqtt_topic2, buffer);
    previousMillis = currentMillis;
   }
}