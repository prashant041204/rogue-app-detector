/*
  ESP32 Rogue Access Point (Evil Twin) Detector
  ----------------------------------------------
  Scans nearby WiFi networks. If any network uses your trusted WiFi name (SSID)
  but comes from a MAC address (BSSID) that is NOT on your trusted list,
  the LED flashes and the buzzer sounds.

  Wiring:
    LED (+ long leg) -> 220 ohm resistor -> GPIO 26 ; LED (- short leg) -> GND
    Active buzzer (+) -> GPIO 27 ; Active buzzer (-) -> GND

  Use only on networks you own or are authorized to monitor.
*/

#include <WiFi.h>

// ================= USER SETTINGS =================
const char* TRUSTED_SSID = "YOUR WIFI NAME";   // case-sensitive

// Add every MAC of your router / extenders / both bands (2.4GHz).
const char* TRUSTED_BSSIDS[] = {
  "MAC Address"
  // , "11:22:33:44:55:66"
};

// true  = SETUP MODE: only prints networks, no alarm (use to find your MAC)
// false = DETECT MODE: alarm is active
const bool SETUP_MODE = true;
// =================================================

const int LED_PIN    = 26;
const int BUZZER_PIN = 27;
const int NUM_BSSIDS = sizeof(TRUSTED_BSSIDS) / sizeof(TRUSTED_BSSIDS[0]);

const unsigned long SCAN_PAUSE_MS = 2000;  // pause between scans
const int ALARM_BLINKS = 6;

unsigned long scanCount = 0;

bool isTrusted(const String& bssid) {
  for (int i = 0; i < NUM_BSSIDS; i++) {
    if (bssid.equalsIgnoreCase(TRUSTED_BSSIDS[i])) return true;
  }
  return false;
}

const char* encName(wifi_auth_mode_t t) {
  switch (t) {
    case WIFI_AUTH_OPEN:         return "OPEN";
    case WIFI_AUTH_WEP:          return "WEP";
    case WIFI_AUTH_WPA_PSK:      return "WPA";
    case WIFI_AUTH_WPA2_PSK:     return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA3_PSK:     return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:return "WPA2/WPA3";
    default:                     return "OTHER";
  }
}

void beepOnce(int ms) {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(ms);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

void alarm() {
  for (int i = 0; i < ALARM_BLINKS; i++) {
    beepOnce(150);
    delay(150);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println();
  Serial.println("=== ESP32 Rogue AP Detector ===");
  Serial.printf("Trusted SSID : %s\n", TRUSTED_SSID);
  Serial.printf("Mode         : %s\n", SETUP_MODE ? "SETUP (no alarm)" : "DETECT");

  // Startup self-test: one short beep + blink
  beepOnce(200);
}

void loop() {
  scanCount++;
  Serial.printf("\n--- Scan #%lu ---\n", scanCount);

  int n = WiFi.scanNetworks();
  if (n < 0) {
    Serial.println("Scan failed, retrying...");
    delay(SCAN_PAUSE_MS);
    return;
  }

  bool rogueFound = false;
  int trustedSeen = 0;

  for (int i = 0; i < n; i++) {
    String ssid  = WiFi.SSID(i);
    String bssid = WiFi.BSSIDstr(i);
    int rssi     = WiFi.RSSI(i);
    int channel  = WiFi.channel(i);
    wifi_auth_mode_t enc = WiFi.encryptionType(i);

    Serial.printf("%-24s %s  ch%-2d  %4ddBm  %s\n",
                  ssid.c_str(), bssid.c_str(), channel, rssi, encName(enc));

    if (ssid == TRUSTED_SSID) {
      if (isTrusted(bssid)) {
        trustedSeen++;
      } else if (!SETUP_MODE) {
        rogueFound = true;
        Serial.printf(">>> ROGUE AP DETECTED! MAC: %s  ch%d  %s\n",
                      bssid.c_str(), channel, encName(enc));
      }
    }
  }

  Serial.printf("Networks: %d | Trusted APs seen: %d\n", n, trustedSeen);
  WiFi.scanDelete();

  if (rogueFound) alarm();

  delay(SCAN_PAUSE_MS);
}
