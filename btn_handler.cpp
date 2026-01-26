#include "btn_handler.h"
#include "config.h"
#include "ota_manager.h"
#include <Arduino.h>

static bool lastState = HIGH;
static uint32_t pressStart = 0;
static bool otaTriggered = false;
static bool warning5sTriggered = false;

void btnInit() {
  pinMode(BTN_FORCE_OTA, BTN_ACTIVE_LOW ? INPUT_PULLUP : INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); 
}

void btnLoop() {
  if (otaInProgress) return;
  bool raw = digitalRead(BTN_FORCE_OTA);
  bool pressed = BTN_ACTIVE_LOW ? (raw == LOW) : (raw == HIGH);

  if (pressed && !lastState) {
    pressStart = millis();
    otaTriggered = false;
    warning5sTriggered = false;
  }
  if (pressed && lastState) {
    uint32_t held = millis() - pressStart;
    
    // 3 วินาที: เสียงสั้น 1 ครั้ง + เริ่ม OTA
    if (held >= 3000 && !otaTriggered) {
      digitalWrite(BUZZER_PIN, HIGH); delay(150); digitalWrite(BUZZER_PIN, LOW);
      Serial.println("[BTN] >>> 3s HOLD DETECTED! Starting OTA... <<<");
      otaRequestNormalCheck();
      otaTriggered = true;
    }
    // 5 วินาที: เสียงยาว 1 ครั้ง เพื่อเตือน
    if (held >= 5000 && !warning5sTriggered) {
      digitalWrite(BUZZER_PIN, HIGH); delay(600); digitalWrite(BUZZER_PIN, LOW);
      Serial.println("[BTN] >>> 5s WARNING! PLEASE RELEASE BUTTON <<<");
      warning5sTriggered = true;
    }
  }
  lastState = pressed;
}
