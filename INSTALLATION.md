# 📦 Installation Guide — ESP32 AI Assistant

This guide walks you through everything needed to flash and run the ESP32 AI Assistant from scratch.

---

## Prerequisites

### Hardware Required

- ESP32 development board (e.g. ESP32 DevKit v1, WROOM-32)
- SSD1306 128×64 OLED display (I2C, address `0x3C`)
- MAX98357A I2S audio amplifier breakout
- Small 4Ω or 8Ω speaker (≤3W)
- Jumper wires + breadboard or PCB
- USB cable (data-capable, not charge-only)
- A PC/Mac/Linux machine running **Ollama** on the same WiFi network

---

## Step 1 — Install Arduino IDE

1. Download **Arduino IDE 2.x** from [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)
2. Install and launch it.

---

## Step 2 — Add ESP32 Board Support

1. Open **File → Preferences**
2. In *Additional boards manager URLs*, add:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Open **Tools → Board → Boards Manager**
4. Search for `esp32` (by Espressif Systems) and click **Install** (version 2.x recommended)

---

## Step 3 — Install Required Libraries

Open **Tools → Manage Libraries** and install each of the following (search by name):

| Library | Author | Notes |
|---|---|---|
| `Adafruit SSD1306` | Adafruit | OLED driver |
| `Adafruit GFX Library` | Adafruit | Graphics primitives (dependency of SSD1306) |
| `ArduinoJson` | Benoit Blanchon | JSON parsing for Ollama API |
| `ESP8266Audio` | Earle F. Philhower III | MP3 + I2S audio playback |

> **Note:** `ESP8266Audio` works on ESP32 despite the name. Search for it exactly as shown.

> The following libraries are included with the ESP32 Arduino core and do **not** need manual installation:
> `WiFi`, `WiFiClient`, `WiFiClientSecure`, `HTTPClient`, `WebServer`, `Wire`, `driver/i2s`

---

## Step 4 — Install and Run Ollama

Ollama must be running on a machine on the **same local WiFi network** as the ESP32.

### Install Ollama

- **Windows / macOS:** Download from [https://ollama.ai/download](https://ollama.ai/download)
- **Linux:**
  ```bash
  curl -fsSL https://ollama.ai/install.sh | sh
  ```

### Pull the AI model

```bash
ollama pull gemma:2b
```

> Other models work too (`llama3`, `phi3`, `mistral`, etc.) — just update `OLLAMA_MODEL` in the sketch.

### Start the Ollama server

```bash
ollama serve
```

By default it listens on port **11434**.

### Allow LAN access (if needed)

On some systems, Ollama only binds to `127.0.0.1` by default. To allow your ESP32 to reach it:

```bash
# Linux / macOS
OLLAMA_HOST=0.0.0.0 ollama serve

# Windows — set environment variable before running
set OLLAMA_HOST=0.0.0.0
ollama serve
```

Find your PC's local IP address:

- **Windows:** `ipconfig` → look for IPv4 under your WiFi adapter
- **macOS/Linux:** `ifconfig` or `ip addr` → look for `192.168.x.x`

---

## Step 5 — Configure the Sketch

Open `ESP32_AI_Assistant_v15stable_noaudio.ino` in Arduino IDE and edit these lines:

```cpp
const char* WIFI_SSID     = "YourSSID";
const char* WIFI_PASSWORD = "YourPassword";

const char* OLLAMA_IP    = "192.168.x.x";  // ← your PC's LAN IP
const int   OLLAMA_PORT  = 11434;
const char* OLLAMA_MODEL = "gemma:2b";

const char* SE_VOICE = "Brian";  // TTS voice (optional)
```

---

## Step 6 — Wire the Hardware

Refer to the wiring table below, then connect your components:

```
ESP32          MAX98357A
─────          ─────────
GPIO 25   →   DIN
GPIO 26   →   BCLK
GPIO 27   →   LRC
5V        →   VIN
GND       →   GND

ESP32          SSD1306 OLED
─────          ────────────
GPIO 21   →   SDA
GPIO 22   →   SCL
3.3V      →   VCC
GND       →   GND
```

---

## Step 7 — Select Board and Port

1. Connect the ESP32 to your PC via USB
2. In Arduino IDE, go to **Tools → Board → ESP32 Arduino** → select **ESP32 Dev Module** (or your specific variant)
3. Go to **Tools → Port** → select the COM/tty port that appeared when you plugged in the ESP32
4. Recommended settings:
   - Upload Speed: `921600`
   - Flash Size: `4MB (32Mb)`
   - Partition Scheme: `Default 4MB with spiffs`
   - CPU Frequency: `240MHz`

---

## Step 8 — Upload the Firmware

1. Click the **Upload** button (→ arrow) or press `Ctrl+U`
2. If upload fails, hold the **BOOT** button on the ESP32 while clicking Upload, then release after a few seconds
3. Wait for `Done uploading` in the console

---

## Step 9 — Monitor and Use

1. Open **Tools → Serial Monitor**, set baud rate to **115200**
2. The ESP32 will:
   - Play the startup animation on the OLED
   - Connect to WiFi (displayed on screen)
   - Test TCP and HTTP connectivity to Ollama
   - Print its IP address to Serial Monitor
3. Once ready, you will see `> ` in Serial Monitor and `ASK AI...` on the OLED

### Using Serial Monitor

Type your question and press **Enter**:
```
> What is the speed of light?
```

### Using the Web UI

Open a browser on any device on the same network:
```
http://<esp32-ip-address>/
```
The IP is shown on the OLED and in the Serial Monitor after boot.

---

## Updating WiFi Credentials or Model

Simply edit the constants in the sketch and re-upload. No other changes are needed.

---

## Common Issues

| Symptom | Fix |
|---|---|
| Port not visible in Arduino IDE | Install CP2102 or CH340 USB driver for your ESP32 board |
| Upload fails | Hold BOOT button during upload |
| OLED blank | Check SDA/SCL wiring; confirm I2C address is `0x3C` |
| "TCP FAILED" on screen | Start `ollama serve` on your PC; check `OLLAMA_IP` |
| No sound | Check MAX98357A wiring; confirm speaker is connected |
| Web UI loads but no answer | Check Serial Monitor for `[Ollama]` error lines |
