/*
 * ============================================================
 * 2FATouchCore - Exemplo Basico (Atualizado para Multi-Idioma)
 * ============================================================
 *
 * Este exemplo demonstra as NOVAS funcionalidades adicionadas ao core:
 *   - Variavel global `GlobalLanguage` (selecao de idioma)
 *   - Funcao `tr(pt_BR, en_US)` para traducao de strings
 *   - Chaves do sistema i18n usadas nas paginas HTML do SD
 *   - Telas TFT com strings traduzidas (WiFi, Loading, Teclado, etc)
 *
 * Compativel com 2FATouchCore versao com lang.h + handleI18nJS
 * Obs.: O include "allconfigs.h" foi removido pois este arquivo
 *       pertence ao projeto principal e nao a biblioteca.
 */

// --- Dependencias obrigatorias do core (ESP32) ---
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>

// --- Dependencias da biblioteca 2FATouchCore ---
#include <2FATouchCore.h>

// ====================================================================
// PINOS PADRAO (fallback se allconfigs.h nao existir)
// ====================================================================
// A biblioteca 2FATouchCore assume que estes pinos estao definidos.
// Ajuste os valores abaixo de acordo com o SEU hardware.
// (Valores de exemplo usados no projeto 2FATouch original)
#ifndef TFT_BL
  #define TFT_BL     4    // Pino do Backlight do TFT
#endif
#ifndef PIN_RED
  #define PIN_RED   16    // LED RGB - Vermelho
#endif
#ifndef PIN_GREEN
  #define PIN_GREEN 17    // LED RGB - Verde
#endif
#ifndef PIN_BLUE
  #define PIN_BLUE   5    // LED RGB - Azul
#endif

// --- Variaveis globais QUE A BIBLIOTECA ASSUME EXISTIR ---
// (declare-as no seu sketch ANTES de incluir <2FATouchCore.h>
//  ou no comeco do sketch, como abaixo)
TFT_eSPI tft = TFT_eSPI();
WebServer server(80);

bool forceRedraw = false;
int  displayMode = 0;
int  wifiScanPage = 0;

// ====================================================================
// SETUP - AQUI VOCE SELECIONA O IDIOMA GLOBAL
// ====================================================================
void setup() {
  Serial.begin(115200);
  delay(200);

  // ----------------------------------------------------------
  // (1) Escolha o idioma do dispositivo (NOVA FUNCIONALIDADE)
  // ----------------------------------------------------------
  // Opcoes validas:
  //   LANG_PT_BR = Portugues do Brasil (padrao)
  //   LANG_EN_US = Ingles Americano
  //
  // Basta descomentar APENAS a linha do idioma desejado.
  // ----------------------------------------------------------
  GlobalLanguage = LANG_PT_BR;
  //GlobalLanguage = LANG_EN_US;

  Serial.println();
  Serial.println("===== 2FATouchCore Example =====");
  Serial.print  ("Idioma ativo: ");
  Serial.println((GlobalLanguage == LANG_EN_US) ? "EN-US (Ingles)" : "PT-BR (Portugues)");
  Serial.println("---------------------------------");

  // ----------------------------------------------------------
  // (2) Demonstracao da funcao tr(texto_pt, texto_en) (NOVO!)
  // ----------------------------------------------------------
  // Use esta funcao SEMPRE para envolver literais de texto
  // que voce queira que acompanhem o idioma global.
  //
  // Regra: 1o argumento  = string em PT-BR
  //        2o argumento  = string em EN-US
  // ----------------------------------------------------------
  Serial.println(tr(
    "Mensagem em Portugues (usando tr())",
    "Message in Portuguese (using tr())"
  ));

  Serial.println(tr(
    "Tudo certo com a traducao!",
    "Everything works with translation!"
  ));
  Serial.println();

  // --- Inicializacao basica do hardware (ajuste pinos ao seu PCB!) ---
  // TFT_eSPI depende da configuracao no seu User_Setup.h.
  // Descomente abaixo se voce tiver os pinos definidos:
  //
  // pinMode(TFT_BL, OUTPUT);
  // digitalWrite(TFT_BL, HIGH);
  // tft.init();
  // tft.setRotation(0);
  // tft.fillScreen(TFT_BLACK);
  //
  // tft.setTextColor(TFT_GREEN, TFT_BLACK);
  // tft.setTextSize(2);
  // tft.drawCentreString(tr("2FATouch Core", "2FATouch Core"), 120, 16, 4);
  // tft.drawCentreString(tr("Exemplo Ativado!", "Example Enabled!"), 120, 60, 2);

  // ----------------------------------------------------------
  // (3) Demonstracao: tela de loading com texto traduzido
  //     (usa a funcao drawLoadingScreen do core, que ja usa tr())
  // ----------------------------------------------------------
  // for (int p = 0; p <= 100; p += 20) {
  //   drawLoadingScreen(p);
  //   delay(400);
  // }

  // --- (4) Demonstracao opcional do servidor web com i18n ---
  // Se for usar as paginas HTML estaticas do SD (login, list, v, edit)
  // NAO ESQUECA de registrar a rota do dicionario de traducao:
  //
  // WiFi.begin("SEU_SSID", "SUA_SENHA");
  // while (WiFi.status() != WL_CONNECTED) { delay(250); Serial.print("."); }
  //
  // server.on("/i18n.js", handleI18nJS);   // <<< OBRIGATORIO!
  // server.on("/",       handleLoginRoute);
  // server.on("/login",  handleLoginRoute);
  // server.on("/doLogin", handleDoLogin);
  // server.on("/logout",  handleLogout);
  // server.on("/list.html", handleListHTML);
  // server.on("/v.html",    handleListHTML2);
  // server.on("/edit",      handleEditFile);
  // server.on("/delete",    handleDeleteFile);
  // server.on("/listJSON",  handleListJSON);
  // server.on("/upload",    HTTP_POST, handleUpload);
  // server.begin();
  //
  // Serial.print("Servidor web em: http://");
  // Serial.println(WiFi.localIP());
  //
  // Serial.println(tr(
  //   "Acesse /login.html e confira as paginas SD traduzidas!",
  //   "Open /login.html and check the translated SD pages!"
  // ));
}

// ====================================================================
// LOOP PRINCIPAL
// ====================================================================
void loop() {
  // --- Se tiver servidor web, descomente: ---
  // server.handleClient();

  // --- Se tiver telas TFT: ---
  // if (forceRedraw) { forceRedraw = false; redrawCurrentScreen(); }
  // handleTFTTouch();

  delay(20);

  // --- Exemplo de log que muda com o idioma (a cada 5 segundos) ---
  static unsigned long ultimoLog = 0;
  if (millis() - ultimoLog > 5000) {
    ultimoLog = millis();
    Serial.println(tr(
      "[PT] Loop ativo - 2FATouchCore funcionando.",
      "[EN] Loop alive - 2FATouchCore working."
    ));
  }
}
