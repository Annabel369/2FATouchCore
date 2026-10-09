# 2FATouchCore

Biblioteca Core para o dispositivo **2FATouch (Creeper Auth)** rodando em ESP32 CYD (Cheap Yellow Display).

## Componentes Inclusos
- `2FATouchCore.h`: Cabeçalho principal da biblioteca
- `qrcode.h` e `qrcode.c`: Motor de geração de QR Code de alta performance
- `allfuc.h`: 65 funções completas do sistema (telas, clima, TOTP, touch, web server, etc.)

## Como Usar no seu Sketch
```cpp
#include "allconfigs.h"
#include <2FATouchCore.h>

void setup() {
  // Inicialização
}

void loop() {
  // Ciclo principal
}
```
