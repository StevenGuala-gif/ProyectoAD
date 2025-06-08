#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

// ==== CONFIGURACIÓN WIFI ====
const char* ssid = "Netlife-Jhon";
const char* password = "JJEGF2040@";

// ==== CONFIGURACIÓN TELEGRAM ====
const String botToken = "7943482705:AAEoGvwPDeJzs9C_V54oVSvIEUY2UAoae_A";
const String chatId = "7525355456";

// ==== PINS ====
#define TRIG_PIN D5
#define ECHO_PIN D6

// ===== CLIENTES PARA CADA TIPO DE CONEXIÓN =====
WiFiClient clientHttp;           // Para InfluxDB (HTTP)
WiFiClientSecure clientHttps;    // Para Telegram (HTTPS)

// ==== FUNCIÓN MEJORADA DE CODIFICACIÓN URL ====
String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}

// ==== SETUP ====
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2500);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("WiFi conectado.");

  clientHttps.setInsecure(); 
}

// ==== FUNCIONES PRINCIPALES ====
float leerUltrasonido() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duracion = pulseIn(ECHO_PIN, HIGH);
  float distancia = (duracion * 0.034 / 2.0) / 100;
  return distancia;
}

int leerGas() {
  return analogRead(A0);
}

int cuantizarAdaptativamente(int valor) {
  if (valor < 300) return valor / 4;
  else if (valor < 700) return valor / 2;
  else return valor;
}

void enviarInflux(float gas, float distancia, int gasCuant) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(clientHttp, "http://192.168.100.11:8086/write?db=sensores");
    http.addHeader("Content-Type", "text/plain");

    String payload = "mediciones sensorHUMO=" + String(gas) +
                     ",sensorHUMOcuantizado=" + String(gasCuant) +
                     ",sensorDISTANCIA=" + String(distancia);

    int httpCode = http.POST(payload);
    if (httpCode > 0) {
      Serial.println("Datos enviados a InfluxDB");
    } else {
      Serial.println("Error al enviar a InfluxDB");
    }
    http.end();
  }
}

void enviarAlertaTelegram(String mensaje) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.telegram.org/bot" + botToken +
                 "/sendMessage?chat_id=" + chatId +
                 "&text=" + urlencode(mensaje);

    http.begin(clientHttps, url);  // USA HTTPS aquí
    int httpCode = http.GET();
    if (httpCode == 200) {
      Serial.println("Mensaje enviado a Telegram");
      
    } else {
      Serial.println("Error al enviar mensaje");
      
    }
    http.end();
  }
}

// ==== LOOP PRINCIPAL ====
void loop() {
  float distancia = leerUltrasonido();
  int gas = leerGas();
  int gasCuant = cuantizarAdaptativamente(gas);

  Serial.print("Gas: "); Serial.print(gas);
  Serial.print(" | Cuantizado: "); Serial.print(gasCuant);
  Serial.print(" | Distancia: "); Serial.println(distancia);

  enviarInflux(gas, distancia, gasCuant);

  // ==== ALERTAS PERSONALIZADAS ====
  if (gas > 200 && distancia < 0.50) {
    enviarAlertaTelegram("⚠️ ALERTA: gas elevado y objeto muy cerca.");
  } else if (gas > 200) {
    enviarAlertaTelegram("⚠️ ALERTA: Presencia de HUMO detectada.");
  } else if (distancia < 0.50) {
    enviarAlertaTelegram("⚠️ ALERTA: Objeto muy CERCA detectado.");
  }

  delay(2000);  // Esperar 2 segundos entre lecturas
}
