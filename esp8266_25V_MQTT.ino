#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "SSID" //your SSID
#define wifi_password "PASSWORD" //your PW

#define mqtt_server "192.168.*.*" //your MQTT Server IP Address
#define solar_spannung "sensor/solar_spannung" //topic

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
float vcc = 0.0;
int analogInput = A0;
float vout = 0.0;
float vin = 0.0;
float R1 = 3002; // Change to your resistor value
float R2 = 1501; // Change to your resistor value
int value = 0;

void setup() {
  pinMode(analogInput, INPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
     if (client.connect("ESP8266Client")) {
    //if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
   // read the value at analog input
   value = analogRead(analogInput);
   vout = (value * 5.0) / 1024.0 * 1.036;
   vin = vout / (R2/(R1+R2)); 
   
   Serial.print("INPUT V= ");
   Serial.println(vin,3);
   client.publish(solar_spannung, String(vin).c_str(), true);
   delay(300000); //change for interval
      
}
