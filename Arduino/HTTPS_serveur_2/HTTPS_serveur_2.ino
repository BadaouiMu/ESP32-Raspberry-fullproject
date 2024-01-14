#include "cert.h"
#include "private_key.h"
#include <WiFi.h>
#include <HTTPSServer.hpp>
#include <HTTPServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>

// ce que nous dois changer pour le serveur 2 /////////////
IPAddress staticIP (192, 168, 4, 253); // 1-252 / 2-253
std::string redirectToStr = "https://192.168.4.253/";
const char *mqtt_topic1 = "serveur2/charge";
const char *mqtt_topic2 = "serveur2/trame";
const char *mqtt_client = "ESP32Client_server2"; 
//////////////////////////////////////////////////////////

#define WIFI_SSID "Groupe_MMYH"
#define WIFI_PSK  "+rlsdQrHudg)|(zqd-Y]@};Fd"

IPAddress gateway (192, 168, 4, 1);
IPAddress subnet (255, 255, 255, 0);
IPAddress dns (192, 168, 4, 1);

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

int charge = 0;
int charge_actualise = 0; 

WiFiClient espClient;
PubSubClient client_mqtt(espClient);

using namespace httpsserver;

SSLCert cert = SSLCert(example_crt_DER, example_crt_DER_len, example_key_DER, example_key_DER_len);

HTTPSServer secureServer = HTTPSServer(&cert);
HTTPServer insecureServer = HTTPServer();

void handleRoot(HTTPRequest *req, HTTPResponse *res);

void handle404(HTTPRequest *req, HTTPResponse *res);

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  Serial.println("Setting up WiFi");
  WiFi.mode(WIFI_STA);

  if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false) {
    Serial.println("Configuration failed."); }
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
    Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
  ResourceNode *nodeRoot = new ResourceNode("/", "GET", &handleRoot);
  ResourceNode *node404 = new ResourceNode("", "GET", &handle404);

  
  secureServer.registerNode(nodeRoot);
  insecureServer.registerNode(nodeRoot);

  secureServer.setDefaultNode(node404);
  insecureServer.setDefaultNode(node404);


  Serial.println("Starting HTTPS server...");
  secureServer.start();
  Serial.println("Starting HTTP server...");
  insecureServer.start();

  if (secureServer.isRunning() && insecureServer.isRunning()) {
    Serial.println("Servers ready.");
  }


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

void loop() {
  secureServer.loop();
  insecureServer.loop();
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
  delay(1);
}

void handleRoot(HTTPRequest *req, HTTPResponse *res) {
  if (req->isSecure()) {
    res->setHeader("Content-Type", "text/html");
    res->println("<!DOCTYPE html>");
    res->println("<html><head><style>");
    res->println("body {font-family: Arial, sans-serif; text-align: center;}");
    res->println("h1 {color: #3333cc;}");
    res->println("</style></head><body>");
    res->print("<h1>Hello from ESP32!</h1>");
    res->println("</body>");
    res->println("</html>");
    
    charge++ ;
    charge_actualise++ ;

    DynamicJsonDocument doc1(256);
    doc1["IP"] = "ipAddress" ; 

    doc1["charge"] = charge_actualise;
    char buffer1[256];
    serializeJson(doc1, buffer1);
    client_mqtt.publish(mqtt_topic1, buffer1);

  } else {
    res->setStatusCode(302);
    res->setHeader("Location", redirectToStr);
  }
}

void handle404(HTTPRequest *req, HTTPResponse *res) {
  req->discardRequestBody();
  res->setStatusCode(404);
  res->setStatusText("Not Found");
  res->setHeader("Content-Type", "text/html");
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Not Found</title></head>");
  res->println("<body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body>");
  res->println("</html>");
}
