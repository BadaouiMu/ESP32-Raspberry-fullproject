#include <WiFi.h>
#include "esp_wifi.h"
#include <PubSubClient.h>

const char *ssid = "Groupe_MMYH";
const char *password = "+rlsdQrHudg)|(zqd-Y]@};Fd";
const int   channel        = 10;
const bool  hide_SSID      = false;
const int   max_connection = 10;


const char *mqtt_server = "192.168.4.254";
const int mqtt_port = 1883;
const char *mqtt_user = "admin";
const char *mqtt_password = "xhc9QmmISs";
const char *mqtt_topic = "donnees";

unsigned long previousMillis = 0;
const long interval = 5000;

WiFiClient espClient;
PubSubClient client(espClient);

void display_connected_devices()
{
    wifi_sta_list_t wifi_sta_list;
    tcpip_adapter_sta_list_t adapter_sta_list;
    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);

    if (adapter_sta_list.num > 0)
      Serial.println("-----------");
      String payload = "{\"ip\":\" 192.168.4.1 ""\",\"mac\":\"" + String(WiFi.macAddress()) + "\"}";
      client.publish(mqtt_topic, payload.c_str());
      
    for (uint8_t i = 0; i < adapter_sta_list.num; i++)
    {
        tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
    char Mac[18];
    sprintf(Mac, "%02X:%02X:%02X:%02X:%02X:%02X", station.mac[0], station.mac[1], 
    station.mac[2], station.mac[3], station.mac[4], station.mac[5]);
    char IP[16]; 
    strcpy(IP, "a"); 

if (strcasecmp(Mac, "E4:5F:01:26:29:77") == 0)
    strcpy(IP, "192.168.4.254");
else if (strcasecmp(Mac, "E0:5A:1A:A6:1C:7C") == 0)
    strcpy(IP, "192.168.4.253");
else if (strcasecmp(Mac, "C0:49:EF:CA:AE:34") == 0)
    strcpy(IP, "192.168.4.250");
else if (strcasecmp(Mac, "08:3A:8A:2F:1C:AC") == 0)
    strcpy(IP, "192.168.4.252");
else  
    strncpy(IP, ip4addr_ntoa((ip4_addr_t*)(&(station.ip))), sizeof(IP) - 1);
        Serial.print("[+] Device ");
        Serial.print(i);
        Serial.print(" | MAC : ");
        Serial.print(Mac);
        Serial.print(" | IP ");
        Serial.println(IP);
        String payload = "{\"ip\":\"" + String(IP) + "\",\"mac\":\"" + String(Mac) + "\"}";
        client.publish(mqtt_topic, payload.c_str());
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\n[*] Creating AP");

    WiFi.mode(WIFI_AP);
    if (WiFi.softAP(ssid, password, channel, hide_SSID, max_connection))
    {
      Serial.print("[+] AP Created");
      Serial.println(WiFi.softAPIP());
    }
    else
    {
      Serial.println("[!] Failed to create AP");
    }

    client.setServer(mqtt_server, mqtt_port);
    client.setClient(espClient);
    if (client.connect("ESP32Client", mqtt_user, mqtt_password))
      {
          Serial.println("[+] Connected to MQTT broker");
      }
    else
      {
    Serial.print("[!] Failed to connect to MQTT broker. State: ");
      }
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    if (!client.connected())
    {
        Serial.println("[!] MQTT Disconnected. Reconnecting...");
        if (client.connect("ESP32Client", mqtt_user, mqtt_password))
        {
          Serial.println("[+] Reconnected to MQTT broker");
        }
        else
        {
          Serial.print("[!] Failed to connect to MQTT broker. State: ");
      }
    }
    display_connected_devices(); 
    previousMillis = currentMillis;
  }
}