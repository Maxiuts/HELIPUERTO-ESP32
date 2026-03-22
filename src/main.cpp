#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* ssid = "ASUS";
const char* password = "Max100806";

#define LED_PIN 2
#define PIR_PIN 23
#define LED_OK 18
#define LED_ERROR 19
#define DHT_PIN 4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);
WebServer server(80);

unsigned long tiempoParpadeo = 0;
const unsigned long intervaloParpadeo = 500;
bool estadoParpadeo = false;

// NOTA: El HTML completo está en src/pagina.html
// Para actualizar, abre pagina.html en el navegador directamente
const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>LED Control</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:Arial,sans-serif;background:#0a0e27;min-height:100vh;display:flex;align-items:center;justify-content:center;color:#fff}
.container{text-align:center;background:linear-gradient(135deg,#1e293b,#0f172a);padding:50px;border-radius:20px;box-shadow:0 10px 40px rgba(0,0,0,.5);border:1px solid rgba(99,102,241,.3)}
h1{font-size:32px;margin-bottom:10px;color:#6366f1}
p{color:#94a3b8;margin-bottom:30px}
.led-status{width:100px;height:100px;margin:20px auto;border-radius:50%;background:#ef4444;box-shadow:0 0 30px rgba(239,68,68,.5);transition:all .3s}
.led-status.on{background:#22c55e;box-shadow:0 0 30px rgba(34,197,94,.8)}
.buttons{display:flex;gap:15px;justify-content:center}
button{padding:15px 40px;border:none;border-radius:10px;font-size:16px;font-weight:600;cursor:pointer;transition:all .3s;text-transform:uppercase}
.btn-on{background:linear-gradient(135deg,#22c55e,#16a34a);color:#fff}
.btn-off{background:linear-gradient(135deg,#ef4444,#dc2626);color:#fff}
button:hover{transform:translateY(-2px);box-shadow:0 8px 20px rgba(0,0,0,.3)}
</style>
</head>
<body>
<div class="container">
<h1>💡 LED Control</h1>
<p>ESP32 Simple Control</p>
<div class="led-status" id="led"></div>
<div class="buttons">
<button class="btn-on" onclick="control('on')">ON</button>
<button class="btn-off" onclick="control('off')">OFF</button>
</div>
</div>
<script>
function control(action){
fetch('/'+action).then(r=>r.text()).then(d=>{
console.log(d);
if(action==='on'){
document.getElementById('led').classList.add('on');
}else{
document.getElementById('led').classList.remove('on');
}
});
}
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", html);
}

void handleOn() {
  digitalWrite(LED_PIN, HIGH);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "LED ON");
}

void handleOff() {
  digitalWrite(LED_PIN, LOW);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "LED OFF");
}

void handleData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int pir = digitalRead(PIR_PIN);
  bool dhtOK = !(isnan(temp) || isnan(hum));
  
  String json = "{\"temp\":" + String(dhtOK ? temp : 0) + 
                ",\"hum\":" + String(dhtOK ? hum : 0) + 
                ",\"pir\":" + String(pir) + 
                ",\"light\":" + String(random(0, 1000)) + 
                ",\"rain\":0" +
                ",\"dhtOK\":" + String(dhtOK ? "true" : "false") + "}";
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== ESP32 INICIANDO ===");
  
  // Configurar pines
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Iniciar DHT11
  dht.begin();
  Serial.println("DHT11 iniciado");
  
  Serial.println("Conectando a WiFi...");

  // Forzar modo estación y limpiar estado previo (ayuda con algunas redes/routers)
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);
  delay(200);

  Serial.print("SSID: ");
  Serial.println(ssid);

  // Opcional: nombre de host para identificar en el router
  WiFi.setHostname("HELIPUERTO-ESP32");

  // Mostrar MAC (útil si el router filtra por MAC)
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  // Iniciar conexión
  WiFi.begin(ssid, password);

  // Esperar con diagnóstico
  const int maxIntentos = 40; // ~20s
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < maxIntentos) {
    delay(500);
    Serial.print(".");
    intentos++;

    // Cada ~2.5s imprimir estado
    if (intentos % 5 == 0) {
      Serial.print("\nWiFi.status(): ");
      Serial.println((int)WiFi.status());
    }
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n*** CONECTADO A WIFI ***");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("Abrir en navegador: http://");
    Serial.println(WiFi.localIP());
    Serial.println("========================\n");
  } else {
    Serial.println("\nERROR: No se pudo conectar al WiFi");

    // Diagnóstico rápido de causas comunes:
    // - SSID/Password incorrectos (WL_CONNECT_FAILED / 4)
    // - Red en 5GHz (ESP32 solo 2.4GHz)
    // - Canal 12/13 con región mal configurada en algunos casos
    // - Filtrado MAC / WPA Enterprise / portal cautivo
    Serial.print("WiFi.status(): ");
    Serial.println((int)WiFi.status());

    Serial.println("Tips:");
    Serial.println("- Asegura que la red sea 2.4GHz (no 5GHz).");
    Serial.println("- Evita caracteres raros/espacios al inicio o fin en SSID/Password.");
    Serial.println("- Si el router usa canal 12/13, prueba canal 1-11.");
    Serial.println("- Revisa filtrado por MAC en el router.");
  }
  
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/led/on", handleOn);
  server.on("/led/off", handleOff);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient();
  
  // Leer DHT11
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  bool sistemaOK = !(isnan(temp) || isnan(hum));
  
  // Control LED OK/ERROR
  if (sistemaOK) {
    digitalWrite(LED_ERROR, LOW);
    // Parpadeo LED OK
    if (millis() - tiempoParpadeo >= intervaloParpadeo) {
      tiempoParpadeo = millis();
      estadoParpadeo = !estadoParpadeo;
      digitalWrite(LED_OK, estadoParpadeo);
    }
  } else {
    digitalWrite(LED_OK, LOW);
    digitalWrite(LED_ERROR, HIGH);
  }
}
