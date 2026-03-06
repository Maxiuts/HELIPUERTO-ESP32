// Configuración del ESP32
// OPCIÓN 1: IP fija (actualiza cuando cambies de red)
const ESP32_IP_MANUAL = 'http://192.168.50.124';

// OPCIÓN 2: Detectar IP automáticamente (RECOMENDADO)
// La página pedirá la IP al cargar
const USE_AUTO_IP = true;

// No modificar
let API_BASE = ESP32_IP_MANUAL;

if (USE_AUTO_IP) {
  const savedIP = localStorage.getItem('esp32_ip');
  if (savedIP) {
    API_BASE = savedIP;
    console.log('✅ Using saved IP:', API_BASE);
  } else {
    // Pedir IP al usuario
    const userIP = prompt('🔧 Ingresa la IP del ESP32 (ejemplo: 192.168.1.100):', '192.168.1.100');
    if (userIP) {
      API_BASE = userIP.startsWith('http') ? userIP : 'http://' + userIP;
      localStorage.setItem('esp32_ip', API_BASE);
      console.log('✅ IP saved:', API_BASE);
    }
  }
}

// Función para cambiar la IP manualmente
function changeESP32IP() {
  const newIP = prompt('🔧 Nueva IP del ESP32:', API_BASE.replace('http://', ''));
  if (newIP) {
    API_BASE = newIP.startsWith('http') ? newIP : 'http://' + newIP;
    localStorage.setItem('esp32_ip', API_BASE);
    alert('✅ IP actualizada a: ' + API_BASE + '\n\nRecarga la página.');
    location.reload();
  }
}

console.log('🌐 ESP32 API Base:', API_BASE);
