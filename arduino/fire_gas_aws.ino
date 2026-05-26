#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>


const char* ssid     = "Verma4G";       
const char* password = "8009597191";   

// ==================== AWS Endpoint ====================
const char* awsEndpoint = "a8dqyvr54htxt-ats.iot.ap-south-1.amazonaws.com"; 
const char* mqttTopic   = "fire-gas-sensor/data";



const char* rootCA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char* deviceCert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUf0DWmHtVdseZIqKRJGUNB9isrOMwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDQxNzIxNTYz
MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOJGuX5+omhPZkmqj/cd
bPSeZUXux7LVVMkiS/JTVi7M9AA1HjIe3hLBt0n+tDjceGQILgsPuGyyCHoAQgRB
X7ZZV2P3BjWBmdS33LwrWyqPAcaE/5B9MMOjl3i2rgTCiAyBShvSQ10iR2QIgWvw
7ixuOTFkiShejhqcGKWv7jHKTHN0avz7ovrVsedfDupDbwpNK4dkE+YyMZK17+Cy
ashpaEZ2yqUBv5+314D63Olt38kDKDFLSQGWYU+YnYavV1HCnt94Alggr79TcBJs
NinVhXxxsa6QhPcFGcb4b9AM+XBHCq/q7igR8S9esbwmCsUJ8A5Qx2n8L5imK99y
XA0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUp8QANYVZ1+n3Pr5YsizIgs3RdQAwHQYD
VR0OBBYEFPwgauKoLmcKqz4HaM3r9phX/gRNMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQB0ZlXEfZylnEDOJP0ZZXuhHJe/
JmV9fNE3jZpEML/FTss3/7CJXsQfXT8uhK7Q+CBlHWm+cxs74aAJfxR7JcUr+2xH
rEhhVDT6dWm5svPGSeItuqes6g9UVzpixaeBzFSzydYoOTy6jWlpzeegOPS7cWLj
NjvUFGr925vUKIECBlRNFTAJEzIDbb9X2zU1Gh/gYFLC56pFgAvI+8+UpxBfu1WK
PeAefhulzemVfZTzEUX9SylvwSAzd85IIo2Dc46jP0ag6xddNXNkpoRDGeB4rg2p
ZNQXWtwp/YYEMG7ZbeC1SkRdWfmZPnmaqtAdweal50davKB0HVPs/A1eFerL
-----END CERTIFICATE-----
)EOF";

const char* privateKey = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA4ka5fn6iaE9mSaqP9x1s9J5lRe7HstVUySJL8lNWLsz0ADUe
Mh7eEsG3Sf60ONx4ZAguCw+4bLIIegBCBEFftllXY/cGNYGZ1LfcvCtbKo8BxoT/
kH0ww6OXeLauBMKIDIFKG9JDXSJHZAiBa/DuLG45MWSJKF6OGpwYpa/uMcpMc3Rq
/Pui+tWx518O6kNvCk0rh2QT5jIxkrXv4LJqyGloRnbKpQG/n7fXgPrc6W3fyQMo
MUtJAZZhT5idhq9XUcKe33gCWCCvv1NwEmw2KdWFfHGxrpCE9wUZxvhv0Az5cEcK
r+ruKBHxL16xvCYKxQnwDlDHafwvmKYr33JcDQIDAQABAoIBAFM+TZN5punU+/5x
3xC8JI9BjmRR2emyjcSI94OooPYKtGE3KWqq7GveFD4Gk+qvQSMb10R+fgyO2ot3
eNHz8vA0oe/GuuQIT71v/qbDFIDtp2uPFi3jwILYtirUmeRqZH6lOcxOaDXlHHpy
nppFQnnZ+vRu1f6h+olTj8W0mV57Ko89mXnAGg+5IORnttkOH58ZLArsz7fTKvTq
iD3/TDsAopCtPCmTsn3Zjgm9D8SZLQPaseFaPesYqvySjprERheIpbZJSX69cpaC
asUc1ErjiO5yKgX9rRlVsMglNbvaLTHPrhnV5bHEByQI+aEhpc1snuw6AkQbZZNE
7PuB4EkCgYEA/vihqdNEZ4RSG3+o519CNNWRDefdh8aE0cKKvh/53ilkLFrrSDCp
/vEnhsaME7udxtc19T3NpVThRjGEnYmohQ4Ksw7PvP27s7/3WdUc79MxvSdGTIFN
KxRGciws7MjM81HVBbmOiX6acxlAxaeD1PdfkIYkI1GJv8pxm6OBrTsCgYEA4zBz
/e0R+cgnX8/z38yy2/14ZznhiWuF2ISIEX1dfKYE5hzbaWiZy8Aq+nQxkVT2SKwQ
y8lARmnD7YK4avYYKXuaj00W9IzBmglJmRRZhE7ehfL73TN3mfGqTzNwfuBDAc7i
03H0V3/N/5U2jzxU1mZefkXNTq7dfaTwPpktp1cCgYEAxp0Q+YQ79dMqTodsIAeh
QBzY6Jckhv6KOgesIhwwfPGLOc39F3fpBFIAM0l4lmqApgLA4m+pQM/Ux+QM5Nb1
sefVR9QS2KpiS8wOtyh+aQb/6NOuPCxA23zov0lQGYb1t9iX8uSvdsVrNJuITC+I
Tx3/kEod3nFGt2LQiqecfT8CgYB3AtvQSg8flP0jUNFFDHspiLc5A/YkwaGk0ZBh
8RU3BdUNIlLB/hyKwokBGDsbEbHOUeyehDdaa29KjS7EQVRY7roBlXWP9+g3I+Rc
/28BQzuIWcgckSh3CUSZJq4omJwhZ1olP82qxHWKDIU/NXCPu6KGbVRmgm3esrdz
Y1SqEwKBgQDKwMPSAKq51TRoiyBlvYaOU/lU0FAHOuKVZPP+5PiQ87itDVH9SNPQ
9+SdizqOa8L4BHkjsLZjq5m49bkdNip9Hy0l2s8KCynA3xvLPzahG1Bj3DQZubnE
fqvZre9PbUtpwy+zBP+V+mOdz4SwwQqmu+Km3yb3K2Kv7ou9DwPxJw==
-----END RSA PRIVATE KEY-----
)EOF";


const int FLAME_SENSOR_PIN    = D0;
const int MQ2_ANALOG_PIN      = A0;
const int MQ2_DIGITAL_PIN     = D2;
const int BUZZER_PIN          = D3;
const int LED_PIN             = D6;
const int RED_LED_PIN         = D7;
const int RELAY_PIN           = D5;


const int GAS_ANALOG_THRESHOLD = 650;

unsigned long lastAlertTime = 0;
const unsigned long ALERT_DELAY = 5000;

int flameDetected  = 0;
int gasDetected    = 0;
int gasAnalogValue = 0;

String alertMessage = "";
bool systemArmed    = true;


BearSSL::WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
unsigned long    lastPublishTime = 0;
const unsigned long PUBLISH_INTERVAL = 3000; // send to AWS every 3 seconds


void connectWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 20) {
    delay(500);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected: " + WiFi.localIP().toString());
  } else {
    Serial.println("\n✗ WiFi Failed - running in offline mode");
  }
}


void connectAWS() {
  if (WiFi.status() != WL_CONNECTED) return;

 
  BearSSL::X509List   *rootCACert  = new BearSSL::X509List(rootCA);
  BearSSL::X509List   *clientCert  = new BearSSL::X509List(deviceCert);
  BearSSL::PrivateKey *clientKey   = new BearSSL::PrivateKey(privateKey);

  wifiClient.setTrustAnchors(rootCACert);
  wifiClient.setClientRSACert(clientCert, clientKey);


  Serial.println("Syncing time...");
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  int timeTries = 0;
  while (now < 1000000000 && timeTries < 20) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    timeTries++;
  }
  Serial.println("\nTime synced: " + String(now));

  mqttClient.setServer(awsEndpoint, 8883);
  mqttClient.setKeepAlive(60);

  Serial.print("Connecting to AWS IoT");
  int tries = 0;
  while (!mqttClient.connected() && tries < 5) {
    if (mqttClient.connect("FireGasDetector")) {
      Serial.println("\n✓ AWS IoT Connected!");
    } else {
      Serial.print(" code:");
      Serial.print(mqttClient.state());
      delay(3000);
      tries++;
    }
  }

  if (!mqttClient.connected()) {
    Serial.println("\n✗ AWS connect failed");
  }
}


void publishToAWS() {
  
  if (WiFi.status() != WL_CONNECTED || !mqttClient.connected()) return;

  
  if (millis() - lastPublishTime < PUBLISH_INTERVAL) return;
  lastPublishTime = millis();

  
  StaticJsonDocument<256> doc;
  doc["device_id"]    = "FireGasDetector";
  doc["flame"]        = flameDetected;
  doc["gas_analog"]   = gasAnalogValue;
  doc["gas_detected"] = gasDetected;
  doc["alert"]        = alertMessage;
  doc["location"]     = "Room";

  char buffer[256];
  serializeJson(doc, buffer);

  bool ok = mqttClient.publish(mqttTopic, buffer);
  if (ok) {
    Serial.println("✓ Sent to AWS");
  } else {
    Serial.println("✗ AWS send failed - will retry");
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // YOUR ORIGINAL pin setup
  pinMode(FLAME_SENSOR_PIN, INPUT);
  pinMode(MQ2_DIGITAL_PIN,  INPUT);
  pinMode(BUZZER_PIN,       OUTPUT);
  pinMode(LED_PIN,          OUTPUT);
  pinMode(RELAY_PIN,        OUTPUT);
  pinMode(RED_LED_PIN,      OUTPUT);

  // YOUR ORIGINAL initial states
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BUZZER_PIN,  LOW);
  digitalWrite(LED_PIN,     HIGH);
  digitalWrite(RELAY_PIN,   HIGH);

  Serial.println("System Ready");

  // YOUR ORIGINAL warm-up blink
  for(int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, LOW);
    delay(500);
    digitalWrite(LED_PIN, HIGH);
    delay(500);
  }

  
  connectWiFi();
  connectAWS();
}


void loop() {

  
  if (mqttClient.connected()) {
    mqttClient.loop();
  }

  
  if (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
    Serial.println("MQTT disconnected - reconnecting...");
    connectAWS();
  }

  
  readSensors();
  analyzeData();
  triggerAlerts();

  publishToAWS();

  delay(2000);
}


void readSensors() {

  flameDetected  = digitalRead(FLAME_SENSOR_PIN);
  gasAnalogValue = analogRead(MQ2_ANALOG_PIN);
  int mq2Digital = digitalRead(MQ2_DIGITAL_PIN);

  gasDetected = (gasAnalogValue > GAS_ANALOG_THRESHOLD) ? 1 : 0;

  Serial.println("\n--- Sensor Readings ---");
  Serial.print("Flame: ");
  Serial.println(flameDetected == 1 ? "SAFE" : "FIRE!");
  Serial.print("Gas Analog: ");
  Serial.println(gasAnalogValue);
  Serial.print("Gas: ");
  Serial.println(gasDetected ? "DETECTED" : "NORMAL");
}


void analyzeData() {

  alertMessage = "";

  if(flameDetected == 0) {
    alertMessage += "FIRE DETECTED! ";
  }

  if(gasDetected == 1 || gasAnalogValue > GAS_ANALOG_THRESHOLD) {
    alertMessage += "GAS LEAKAGE! ";
  }

  if(alertMessage.length() > 0) {
    Serial.println("ALERT: " + alertMessage);
  }
}


void triggerAlerts() {

  bool fire = (flameDetected == 0);
  bool gas  = (gasAnalogValue > GAS_ANALOG_THRESHOLD);

  // Control GREEN LED
  if(fire || gas) {
    digitalWrite(LED_PIN, LOW);    // OFF during alert
  } else {
    digitalWrite(LED_PIN, HIGH);   // ON in normal
  }

  // NO ALERT CONDITION
  if(!fire && !gas) {
    digitalWrite(BUZZER_PIN,  HIGH);  // OFF
    digitalWrite(RED_LED_PIN, LOW);   // OFF
    digitalWrite(RELAY_PIN,   LOW);   // OFF
    return;
  }

  // ALERT ACTIVE → buzzer ON
  digitalWrite(BUZZER_PIN, LOW);

  // BOTH fire + gas → blink
  if(fire && gas) {
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(RED_LED_PIN, HIGH);
    delay(200);
    digitalWrite(RED_LED_PIN, LOW);
    delay(200);
    Serial.println("⚠️ FIRE + GAS ALERT!");
  }

  // ONLY GAS → solid ON
  else if(gas) {
    digitalWrite(RELAY_PIN,   HIGH);
    digitalWrite(RED_LED_PIN, HIGH);
    Serial.println("⚠️ GAS ALERT!");
  }

  // ONLY FIRE → fast blink
  else if(fire) {
    digitalWrite(RELAY_PIN,   HIGH);
    digitalWrite(RED_LED_PIN, HIGH);
    delay(100);
    digitalWrite(RED_LED_PIN, LOW);
    delay(100);
    Serial.println("⚠️ FIRE ALERT!");
  }

  logEvent("Alert triggered");
}


void logEvent(String event) {
  Serial.print("LOG: ");
  Serial.print(millis());
  Serial.print(" -> ");
  Serial.println(event);
}
