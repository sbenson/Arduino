#include <PubSubClient.h>         
#include <ESP8266WiFi.h>           
#include <ESP8266mDNS.h>         
#include <ArduinoOTA.h> 

#define USER_SSID                 "YOUR_SSID"
#define USER_PASSWORD             "YOURSSID_PW"
#define USER_MQTT_SERVER          "MQTTT_SERVER_IP"
#define USER_MQTT_PORT            1883
#define USER_MQTT_USERNAME        "MQTT_USER"
#define USER_MQTT_PASSWORD        "MQTT_PASS"
#define USER_MQTT_CLIENT_NAME     "MQTT_CLIENT"


const char* ssid = USER_SSID ; 
const char* password = USER_PASSWORD ;
const char* mqtt_server = USER_MQTT_SERVER ;
const int mqtt_port = USER_MQTT_PORT ;
const char *mqtt_user = USER_MQTT_USERNAME ;
const char *mqtt_pass = USER_MQTT_PASSWORD ;
const char *mqtt_client_name = USER_MQTT_CLIENT_NAME ; 


WiFiClient espClient;
PubSubClient client(espClient);

bool boot = true;
String tanklevelstring;
char tanklevelchar[50];

void setup_wifi() 
{
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.hostname(USER_MQTT_CLIENT_NAME);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() 
{
  // Loop until we're reconnected
  int retries = 0;
  while (!client.connected()) {
    if(retries < 150)
    {
        Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass)) 
      {
        Serial.println("connected");
        // Once connected, publish an announcement...
        if(boot == true)
        {
          client.publish(USER_MQTT_CLIENT_NAME"/checkIn","Rebooted");
          boot = false;
        }
        if(boot == false)
        {
          client.publish(USER_MQTT_CLIENT_NAME"/checkIn","Reconnected"); 
        }

      } 
      else 
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        retries++;
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    else
    {
      ESP.restart();
    }
  }
}


void setup() 
{
  Serial.begin(115200);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  ArduinoOTA.setHostname(USER_MQTT_CLIENT_NAME);
  ArduinoOTA.begin(); 
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  
  int val;
  
  if(Serial.available()>0){
    val = Serial.parseInt();
    if(val > 0){
      Serial.print("Received: ");
      Serial.println(val);
      tanklevelstring = String(val);
      client.publish(USER_MQTT_CLIENT_NAME"/level", (char*) tanklevelstring.c_str());
    }
  }

 ArduinoOTA.handle();

}
