# 📡 ESP32 Rogue Access Point (Evil Twin) Detector

A small, fun weekend project: an ESP32 that scans nearby WiFi networks and sounds an alarm when it spots a **fake network copying the name of your trusted WiFi**.

If a network uses your WiFi's name (SSID) but comes from a MAC address (BSSID) that is **not on your trusted list**, the LED flashes and the buzzer goes off.

> Built in about 2 hours with an ESP32, an LED, and a buzzer. Tested only on my own network.

---

## 🧠 What is an Evil Twin attack?

An **evil twin** is a fake WiFi hotspot set up with the same name as a network you trust (for example, a cafe's free WiFi). Your phone or laptop may connect to it automatically, and the attacker can then sit in the middle of your traffic.

**How it can harm you:**

- 🔴 Steal passwords and login details through fake login pages
- 🔴 Capture banking and UPI details
- 🔴 Watch your browsing activity and private messages
- 🔴 Push malware through fake downloads or updates

A WiFi **name** proves nothing, because anyone can copy it. The hardware address (MAC/BSSID) of the access point is a much better clue, and that is what this project checks.

---

## ⚙️ How it works

1. The ESP32 runs in WiFi station mode and scans for nearby networks every few seconds.
2. For every network found, it reads the SSID, BSSID (MAC), channel, signal strength, and encryption type.
3. If the SSID matches your trusted WiFi name **and** the BSSID is **not** in your trusted list, it is flagged as a **rogue AP**.
4. The LED blinks and the buzzer beeps, and the details are printed to the Serial Monitor.

The code has two modes:

| Mode | `SETUP_MODE` | What it does |
|------|--------------|--------------|
| Setup | `true` | Only prints nearby networks. Use this to find your router's MAC. No alarm. |
| Detect | `false` | Alarm is active. |

---

## 🧰 Hardware required

| Part | Quantity |
|------|----------|
| ESP32 development board (ESP32-WROOM-32 / "ESP32 Dev Module") | 1 |
| Active buzzer | 1 |
| LED (any colour) | 1 |
| 220Ω resistor | 1 |
| Breadboard | 1 |
| Jumper wires | a few |
| USB data cable | 1 |

---

## 🔌 Wiring

Power off the board before wiring.

| Component | ESP32 pin |
|-----------|-----------|
| LED long leg (+) → 220Ω resistor | GPIO 26 |
| LED short leg (-) | GND |
| Active buzzer (+) | GPIO 27 |
| Active buzzer (-) | GND |

> Use an **active** buzzer. A passive buzzer will not make sound with this code.

> These pins are for the classic ESP32. On ESP32-S2, S3, or C6 boards, GPIO 26 and 27 can conflict with flash memory, so change `LED_PIN` and `BUZZER_PIN` in the code (for example to 4 and 5).

---

## 🛠️ Software setup

1. Install the **Arduino IDE**.
2. Open **File → Preferences** and add this to "Additional boards manager URLs":
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. Open **Tools → Board → Boards Manager**, search for `esp32`, and install **esp32 by Espressif Systems**.
4. Select **Tools → Board → ESP32 Dev Module**.
5. Plug in the ESP32 and select its port under **Tools → Port**.
   - If no port appears, install the **CP210x** or **CH340** USB driver (check the chip near the USB connector) and try a data-capable USB cable.

---

## 🚀 Usage

### Step 1: Configure the code

Open `rogue_ap_detector.ino` and edit the settings at the top:

```cpp
const char* TRUSTED_SSID = "YourWiFiName";   // case-sensitive

const char* TRUSTED_BSSIDS[] = {
  "AA:BB:CC:DD:EE:FF"
  // , "11:22:33:44:55:66"   // add more for extenders / mesh nodes
};

const bool SETUP_MODE = true;   // true first, then false
```

### Step 2: Find your router's real MAC (BSSID)

1. Keep `SETUP_MODE = true` and upload the code.
2. Open the **Serial Monitor** at **115200 baud**.
3. Find your WiFi name in the list and copy its MAC.

Example output:

```
Pradeep 4g     B4:A7:C6:6F:65:0F  ch10  -83dBm  WPA2
```

> The MAC printed on the router's sticker is often **not** the WiFi BSSID, so trust the Serial Monitor.
> If you use an extender or a mesh system, add each device's MAC to the list.

### Step 3: Turn on alarm mode

1. Paste your MAC(s) into `TRUSTED_BSSIDS`.
2. Set `SETUP_MODE = false`.
3. Upload again. You should hear one short beep and see one blink (startup self-test).

---

## 🧪 Testing (your own network only)

**Option A: Fake the trusted MAC.** Temporarily put a wrong MAC (like `00:11:22:33:44:55`) in `TRUSTED_BSSIDS`. Your real router is now "unknown", so the alarm fires. Restore the correct MAC afterwards.

**Option B: Second hotspot.** Turn on a hotspot on another phone, rename it to exactly your WiFi name, and set it to **2.4GHz**. The ESP32 should flag it within about 5-10 seconds.

Expected Serial Monitor output:

```
>>> ROGUE AP DETECTED! MAC: B4:3D:08:02:B0:60  ch2  WPA/WPA2
Networks: 7 | Trusted APs seen: 0
```

---

## 🩺 Troubleshooting

| Problem | Fix |
|---------|-----|
| No port shows in Arduino IDE | Install the CP210x or CH340 driver and use a data USB cable. |
| Upload stuck on "Connecting......" | Hold the **BOOT** button on the ESP32 until upload starts. |
| Serial Monitor shows garbage | Set the baud rate to **115200**. |
| False alarm on your own WiFi | Add the MAC of your extender or second band to `TRUSTED_BSSIDS`. |
| Network not detected | The ESP32 only sees **2.4GHz**. Make sure your WiFi (or test hotspot) is on 2.4GHz. |
| Buzzer is silent | Check polarity and confirm it is an **active** buzzer. |

---

## ⚠️ Limitations

- A determined attacker can **spoof a MAC address**, so this catches the common, careless evil twin, not every attack.
- Only **2.4GHz** networks are detected (the ESP32 has no 5GHz radio).
- It is a learning and demo project, not a replacement for professional wireless intrusion detection.

---

## 🔮 Future improvements

- 📩 SMS alerts using a SIM800L module
- 🔋 Battery-powered pocket version (Li-ion + TP4056 charger + switch)
- 🔍 Extra checks such as encryption-type mismatch
- 📱 Phone notifications over WiFi or Bluetooth

---

## ⚖️ Ethical use

Use this project only on networks you **own or are authorized to monitor**. It is a defensive, detection-only tool and does not attack, jam, or connect to any network.

---

## 📁 Project structure

```
rogue_ap_detector/
├── rogue_ap_detector.ino   # main Arduino sketch
└── README.md
```

> The Arduino sketch must live in a folder with the same name as the `.ino` file.

---

## 📜 License

This project is released under the MIT License. Feel free to use, modify, and share it.

---

## 🙋 Author

Made by **Pradeep** for fun on a bored weekend.

Feedback and ideas are welcome. Open an issue or connect with me on LinkedIn.
