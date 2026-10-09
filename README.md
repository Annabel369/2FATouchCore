# 2FATouchCore

Biblioteca Core para o dispositivo **2FATouch (Creeper Auth)** rodando em ESP32 CYD (Cheap Yellow Display).

## Componentes Inclusos
- `2FATouchCore.h`: Cabeçalho principal da biblioteca
- `qrcode.h` e `qrcode.c`: Motor de geração de QR Code de alta performance
- `allfuc.h`: 65 funções completas do sistema (telas, clima, TOTP, touch, web server, etc.)

## Como Usar no seu Sketch
```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <XPT2046_Touchscreen.h>
#include <vector>

// ========================================================
// CONFIGURAÇÕES DO HARDWARE (Ajuste para a sua placa)
// ========================================================
#define TOUCH_CS 21
XPT2046_Touchscreen ts(TOUCH_CS);

// Calibração do Touch
#define TOUCH_SWAP_XY false
#define TOUCH_INVERT_X false
#define TOUCH_INVERT_Y false
#define TOUCH_MIN_RAW_X 300
#define TOUCH_MAX_RAW_X 3800
#define TOUCH_MIN_RAW_Y 300
#define TOUCH_MAX_RAW_Y 3800

// ========================================================
// OBJETOS GLOBAIS REQUERIDOS PELA BIBLIOTECA
// ========================================================
TFT_eSPI tft = TFT_eSPI(); 
WebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -10800, 60000);
File uploadFile;

// ========================================================
// VARIÁVEIS GLOBAIS DE ESTADO REQUERIDAS
// ========================================================
bool sessaoAtiva = false;
String cfgSSID = "";
String cfgPASS = "";
String inputWifiPass = "";
String selectedSSID = "";
int wifiSetupState = 0;
int wifiScanPage = 0;
bool forceRedraw = true;
int displayMode = 0;
bool shiftActive = false;

std::vector<String> scannedSSIDs;
std::vector<int> scannedRSSI;

// ========================================================
// INCLUSÃO DA BIBLIOTECA 2FATouchCore
// ========================================================
#include <2FATouchCore.h>

void setup() {
  Serial.begin(115200);
  
  // 1. CONFIGURAÇÃO DE IDIOMA / GLOBAL LANGUAGE
  // Escolha o idioma da interface: 
  // LANG_EN_US = Inglês / English
  // LANG_PT_BR = Português / Portuguese
  GlobalLanguage = LANG_EN_US; // Ativação em Inglês
  //GlobalLanguage = LANG_PT_BR; // Ativação em Português

  // 2. INICIALIZAÇÃO DE TELA E TOUCH
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  ts.begin();
  ts.setRotation(1);

  // 3. INICIALIZAÇÃO DO CARTÃO SD
  if(!SD.begin()){
    Serial.println("Erro ao inicializar o SD Card!");
  } else {
    Serial.println("SD Card inicializado com sucesso.");
  }

  // 4. ANIMAÇÃO DE CARREGAMENTO (Da biblioteca)
  drawLoadingScreen(10);
  delay(500);
  drawLoadingScreen(50);
  delay(500);
  drawLoadingScreen(100);
  delay(500);

  // 5. CONFIGURAÇÃO DO WEBSERVER COM AS ROTAS DA BIBLIOTECA
  server.on("/login.html", HTTP_GET, handleLoginRoute);
  server.on("/dologin", HTTP_POST, handleDoLogin);
  server.on("/logout", HTTP_GET, handleLogoutCustom);
  server.on("/list.html", HTTP_GET, handleListHTML);
  server.on("/edit", HTTP_GET, handleEditFile);
  server.on("/delete", HTTP_GET, handleDeleteFile);
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/plain", "Upload success");
  }, handleUpload);

  server.begin();
  Serial.println("Servidor Web iniciado!");

  // 6. INICIA O MODO DE SCAN DE WI-FI DA TELA
  // Para fins de demonstração, vamos direto para a tela de scan de redes
  displayMode = 0;
  iniciarScanWiFiTFT();
  drawWiFiScanScreen();
}

void loop() {
  // Lida com requisições HTTP
  server.handleClient();
  
  // Lida com o toque na tela
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    int tx, ty;
    
    // Converte a leitura crua (raw) para as coordenadas da tela TFT (da biblioteca)
    converterPontoTouch(p, tx, ty);
    
    // Passa o toque processado para o tratador de eventos de Wi-Fi da biblioteca
    if (displayMode == 0) {
      handleWiFiTouch(tx, ty);
    }
    
    // Um pequeno delay para atuar como debounce do toque
    delay(200); 
  }
}
```
