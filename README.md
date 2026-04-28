# 🤖 ESP32 AI Assistant

A self-contained AI assistant running on an ESP32 microcontroller, powered by a local [Ollama](https://ollama.ai) LLM server. It features a glassmorphism web UI, a 128×64 OLED display, I2S audio with text-to-speech (StreamElements TTS), and real-time non-blocking HTTP polling — all in a single `.ino` file.

---

## ✨ Features

| Feature | Details |
|---|---|
| **Local AI (Ollama)** | Queries a self-hosted Ollama instance (default: `gemma:2b`) over your LAN |
| **Web UI** | Glassmorphism chat interface served directly from the ESP32 on port 80 |
| **OLED Display** | 128×64 SSD1306 shows startup animation, user prompts, AI responses, and scrolling text |
| **I2S Audio (TTS)** | StreamElements free TTS API — no API key required — played via MAX98357A amplifier |
| **Sound Effects** | Startup jingle, question beep, and response beep via raw I2S tone synthesis |
| **Serial Interface** | Type questions directly in the Arduino Serial Monitor |
| **Non-blocking Design** | Web `/ask` + `/result` polling pattern keeps the main loop free during AI inference |
| **WiFi Watchdog** | Auto-reconnects on network drop |
| **Chat History** | Up to 20 messages kept in RAM and displayed in the web UI |
| **Status API** | `/status` JSON endpoint exposes device state (model, IP, audio, idle) |

---

## 🔌 Hardware Wiring

### MAX98357A I2S Speaker Amplifier

| MAX98357A Pin | ESP32 GPIO |
|---|---|
| VIN | 5V |
| GND | GND |
| DIN | GPIO 25 |
| BCLK | GPIO 26 |
| LRC | GPIO 27 |

### SSD1306 OLED Display (I2C)

| SSD1306 Pin | ESP32 GPIO |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### Other

| Component | ESP32 GPIO |
|---|---|
| Blue Status LED | GPIO 2 (onboard) |
| Boot Button | GPIO 0 (onboard) |

---

## ⚙️ Configuration

Open `ESP32_AI_Assistant_v15stable_noaudio.ino` and update the constants at the top of the file:

```cpp
// WiFi credentials
const char* WIFI_SSID     = "YourNetworkName";
const char* WIFI_PASSWORD = "YourPassword";

// Ollama server — must be on the same LAN as the ESP32
const char* OLLAMA_IP    = "192.168.x.x";   // IP of the machine running Ollama
const int   OLLAMA_PORT  = 11434;
const char* OLLAMA_MODEL = "gemma:2b";       // Any model pulled in Ollama

// TTS voice (StreamElements — no API key needed)
// Options: Brian, Amy, Emma, Joanna, Salli, Kendra, Kimberly, Joey, Matthew, Justin, Ivy
const char* SE_VOICE = "Brian";
```

---

## 🌐 Web UI Endpoints

Once the ESP32 is running and connected, open a browser on the same network:

| Endpoint | Method | Description |
|---|---|---|
| `http://<device-ip>/` | GET | Glassmorphism chat interface |
| `http://<device-ip>/ask?q=<question>` | GET | Submit a question (returns immediately with `ok`) |
| `http://<device-ip>/result` | GET | Poll for AI answer (`202` = processing, `200` = ready, `204` = idle) |
| `http://<device-ip>/status` | GET | JSON status (`model`, `ip`, `audioPlaying`, `waitingForInput`) |

---

## 🔁 System Flow

```
User types question
    │
    ├── Serial Monitor  ──┐
    └── Web UI /ask    ──┤
                          ▼
                  Main loop picks up question
                          │
                          ▼
                  POST to Ollama /api/generate
                          │
                          ▼
                  Response displayed on OLED
                          │
                          ▼
                  TTS via StreamElements API
                          │
                          ▼
                  MP3 streamed → MAX98357A speaker
                          │
                          ▼
                  Auto-scroll text → ready for next question
```

---

## 📟 OLED Display Screens

| Screen | Trigger |
|---|---|
| **Startup Animation** | Power-on / reset |
| **WiFi Connecting** | During WiFi join |
| **TCP/HTTP Diagnostic** | After WiFi connects |
| **Ready / "ASK AI..."** | Waiting for input (animated dots) |
| **User Bubble** | Question received |
| **AI Thinking** | Waiting for Ollama response |
| **AI Response + Bars** | Displaying answer / playing audio |
| **Auto-scroll** | After audio ends, scrolls remaining text |
| **Disconnected** | WiFi or Ollama unreachable |

---

## 🛠️ Troubleshooting

**OLED shows "TCP FAILED!" / "AI DISCONNECTED"**
- Make sure Ollama is running: `ollama serve`
- Confirm the `OLLAMA_IP` in the sketch matches your PC's LAN IP
- Check that both devices are on the same WiFi network

**No audio / TTS silent**
- Verify MAX98357A wiring (DIN=25, BCLK=26, LRC=27)
- The StreamElements TTS API requires internet access; confirm ESP32 can reach `api.streamelements.com`
- Audio has a 30-second watchdog — very long responses may be truncated to 200 characters for TTS

**Browser times out on `/ask`**
- This was fixed in v11+ with the non-blocking queue + `/result` polling pattern; ensure you are on the latest version

**Ollama returns empty response**
- The model may still be loading; wait a moment and retry
- Try a smaller model (`gemma:2b` recommended for speed)
- Check Serial Monitor for `[Ollama] HTTP code:` output

---

## 📁 Project Structure

```
ESP32_AI_Assistant_v15stable_noaudio.ino   ← Main firmware (single file)
README.md                                  ← This file
INSTALLATION.md                            ← Step-by-step setup guide
requirements.txt                           ← Arduino library list
```

---

## 📜 License

This project is provided as-is for personal and educational use. StreamElements TTS is a free public API — please respect their terms of service. Ollama and its models have their own respective licenses.
