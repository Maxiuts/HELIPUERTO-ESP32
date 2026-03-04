# 🏛️ Helipuerto Dome Control Station

Sistema de control automatizado para helipuerto con monitoreo ambiental y secuencias de vuelo.

## 🚀 Características

- **Secuencias de Vuelo Automatizadas**: TAKEOFF y LANDING con verificaciones de seguridad
- **Monitoreo en Tiempo Real**: Temperatura, humedad, luz, movimiento y lluvia
- **Panel de Mantenimiento**: Control manual protegido por contraseña (código: 1234)
- **Dashboard Moderno**: Interfaz dark theme con gráficos en tiempo real
- **Confirmaciones Modales**: Sistema de doble confirmación para operaciones críticas

## 📋 Hardware Requerido

- ESP32 DevKit
- Sensor DHT11 (temperatura/humedad)
- Sensor de luz (LDR o similar)
- Sensor PIR (movimiento)
- Sensor de lluvia
- LED indicador (GPIO 2)
- Servos/motores para control del domo

## 🔧 Instalación

1. Clona el repositorio
2. Abre el proyecto en PlatformIO
3. Configura tu WiFi en `src/main.cpp`:
   ```cpp
   const char* ssid = "TU_WIFI";
   const char* password = "TU_PASSWORD";
   ```
4. Sube el código al ESP32
5. Abre la IP del ESP32 en tu navegador

## 🌐 GitHub Pages

La interfaz web está disponible en: `https://[tu-usuario].github.io/[repo-name]/src/pagina.html`

## 📱 Uso

### Secuencias de Vuelo
1. **TAKEOFF**: Verifica clima → Verifica movimiento → Enciende luces si es necesario → Abre domo → Espera confirmación de despegue → Cierra domo
2. **LANDING**: Verifica clima → Verifica movimiento → Abre domo → Espera confirmación de aterrizaje → Cierra domo

### Panel de Mantenimiento
- Accede desde el menú hamburguesa
- Código: `1234`
- Control manual de LED y domo

## 🛡️ Seguridad

- Detección automática de lluvia (suspende operaciones)
- Verificación de movimiento antes de operaciones
- Verificación de niveles de luz
- Sistema de doble confirmación
- Cierre automático del domo en caso de lluvia

## 📊 Sensores

- **Temperatura**: Rango 20-30°C
- **Humedad**: Rango 50-80%
- **Luz**: 0-1000 lux
- **PIR**: Detección de movimiento
- **Lluvia**: Detección binaria

## 🎨 Tecnologías

- ESP32 (Arduino Framework)
- HTML5 + CSS3 + JavaScript
- Chart.js para gráficos
- PlatformIO

## 📝 Licencia

MIT

## 👨‍💻 Autor

Desarrollado con ❤️ para control de helipuertos
