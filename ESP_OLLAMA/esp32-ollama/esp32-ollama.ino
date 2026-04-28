
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourceHTTPStream.h>
#include <AudioFileSourceBuffer.h>
#include <AudioFileSourcePROGMEM.h>
#include <driver/i2s.h>
#include <math.h>

const char* WIFI_SSID     = "ssid";
const char* WIFI_PASSWORD = "password";


const char* OLLAMA_IP    = "ip";
const int   OLLAMA_PORT  = 11434;
const char* OLLAMA_MODEL = "gemma:2b";// can change accordingly


// Voice options: Brian, Amy, Emma, Joanna, Salli, Kendra, Kimberly, Joey, Matthew, Justin, Ivy
const char* SE_VOICE    = "Brian";


#define I2S_DOUT  25
#define I2S_BCLK  26
#define I2S_LRC   27


#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


#define BLUE_LED  2
#define BOOT_BTN  0


static const unsigned char PROGMEM image_DolphinMafia_bits[] = {
  0x00,0x00,0x00,0x00,0x0f,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x70,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x07,0x80,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x7f,0xfe,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x0f,0xaa,0x80,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x75,0x55,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x01,0xaa,0xaa,0xa8,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x55,0x54,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x02,0xaa,0xaa,0xaa,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x05,0x55,0x54,0x00,0x00,0x10,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x06,0xaa,0xaa,0xaa,0x00,0x1f,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x05,0x55,0x54,0x00,0x00,0x08,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x06,0xaa,0xaa,0xaa,0x00,0x08,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x05,0x55,0x54,0x00,0x00,0x04,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x06,0xab,0xff,0xff,0xfc,0x04,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x05,0x5f,0xff,0xff,0xff,0xff,0xc0,0x1c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x06,0xff,0xff,0xff,0xff,0xff,0xff,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x07,0xff,0xff,0xfd,0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0f,0xff,0xff,0xea,0xaa,0xaa,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x3f,0xff,0xff,0x55,0x40,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xff,0xff,0xfa,0xa8,0x00,0x00,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x01,0xff,0xff,0xf5,0x40,0x1f,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0xff,0xff,0xaa,0x80,0xff,0xf8,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x07,0xff,0xff,0x54,0x03,0xf0,0x1f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0f,0xff,0xfa,0xa8,0x0f,0xf0,0x1f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x02,0x1f,0xff,0xf5,0x40,0x1f,0xf0,0x38,0x70,0xf8,0x00,0x00,0x00,0x00,0x00,
  0x06,0x3f,0xff,0xea,0xa8,0x1f,0xf8,0x78,0x7f,0x06,0x00,0x00,0x00,0x6b,0x00,
  0x0a,0x7f,0xff,0xd5,0xff,0xff,0xff,0xf0,0xf0,0x01,0x00,0x00,0x01,0x80,0x13,
  0x72,0x7f,0xff,0xaf,0xe8,0x0f,0xff,0xf3,0x00,0x00,0x80,0x00,0x02,0x00,0x00,
  0x04,0xff,0xff,0xdf,0x00,0x07,0xff,0xe0,0x00,0x06,0x80,0x00,0x04,0x00,0x00,
  0x18,0xff,0xff,0xba,0xa0,0x03,0xff,0xc0,0x00,0x0e,0x87,0xe0,0x05,0xc5,0x0d,
  0x60,0xff,0xff,0x55,0x00,0x01,0xff,0x80,0x00,0x3f,0x80,0x00,0x06,0x00,0x00,
  0x00,0xff,0xff,0xba,0xa0,0x03,0xfe,0x00,0x00,0xcd,0x00,0x00,0x04,0x00,0x00,
  0x00,0x7f,0xff,0x75,0x00,0x04,0x06,0x00,0x03,0x01,0x00,0xf8,0x00,0x00,0x00,
  0x00,0x3f,0xf9,0xaa,0xa0,0x08,0x02,0x00,0x0c,0x01,0x00,0x00,0x70,0x00,0x00,
  0x00,0x00,0x01,0x55,0x00,0x08,0x00,0x00,0x30,0x01,0x00,0x00,0x88,0x00,0x00,
  0x00,0x00,0x01,0xaa,0xa0,0x00,0x00,0x00,0xc0,0x02,0x00,0x01,0x08,0x00,0x00,
  0x00,0x00,0x01,0x55,0x00,0x00,0x40,0x01,0x00,0x04,0x00,0x02,0x08,0x00,0x00,
  0x00,0x00,0x00,0xaa,0xa0,0x00,0x40,0x06,0x00,0x08,0x00,0x02,0x08,0x00,0x00,
  0x00,0x00,0x00,0xd5,0x00,0x00,0x30,0x18,0x00,0x30,0x00,0x04,0x10,0x00,0x00,
  0x00,0x00,0x00,0xaa,0xa8,0x00,0x0f,0xe0,0x00,0xc0,0x03,0x88,0x10,0x00,0x00,
  0x00,0x00,0x00,0xd5,0x00,0x00,0x00,0x00,0x03,0x00,0x04,0x68,0x78,0x00,0x00,
  0x00,0x00,0x00,0xaa,0xa8,0x00,0x00,0x00,0x0c,0x00,0x04,0x10,0x84,0x00,0x00,
  0x00,0x00,0x01,0xd5,0x00,0x00,0x00,0x00,0x30,0x00,0x04,0x01,0x02,0x00,0x00,
  0x00,0x00,0x02,0xba,0xa8,0x00,0x00,0x00,0xc0,0x00,0x02,0x01,0x01,0x00,0x00,
  0x00,0x00,0x03,0x57,0x40,0x00,0x00,0x03,0xe0,0x00,0x02,0x02,0x00,0x80,0x00,
  0x00,0x00,0x02,0xaa,0xea,0x00,0x00,0x1f,0xe0,0x00,0x01,0x02,0x00,0x80,0x00,
  0x00,0x00,0x03,0x55,0x5c,0x00,0x00,0x7f,0xa0,0x00,0x01,0x02,0x00,0x80,0x00,
  0x00,0x00,0x07,0xaa,0xab,0x80,0x00,0x3f,0xa0,0x00,0x01,0x01,0x01,0x00,0x00,
  0x00,0x00,0x0f,0xd5,0x50,0x78,0x00,0x0e,0x30,0x00,0x01,0x00,0x81,0x00,0x00,
  0x00,0x00,0x1b,0xea,0xaa,0x87,0x80,0x00,0x28,0x00,0x01,0x00,0x42,0x00,0x00,
  0x00,0x00,0x37,0xf5,0x50,0x00,0x7f,0xe0,0x28,0x00,0x07,0x00,0x3e,0x00,0x00,
  0x00,0x00,0x4f,0xfe,0xaa,0xa0,0x00,0x18,0x24,0x00,0x08,0x80,0x04,0x00,0x00,
  0x00,0x00,0xd7,0xff,0x50,0x00,0x00,0x04,0x44,0x00,0x10,0x40,0x04,0x00,0x00,
  0x00,0x00,0xaf,0xff,0xaa,0xa0,0x00,0x12,0x44,0x00,0x20,0x20,0x08,0x00,0x00,
  0x00,0x01,0x5f,0xff,0xf0,0x00,0x00,0x31,0x42,0x00,0x60,0x10,0x10,0x00,0x00,
  0x00,0x02,0xaf,0xff,0xfa,0xa8,0x00,0x60,0xd2,0x01,0x90,0x0c,0x20,0x00,0x00,
  0x00,0x03,0x5f,0xff,0x57,0x00,0x00,0xe0,0x4a,0x02,0x08,0x03,0x60,0x00,0x00,
  0x00,0x06,0xaf,0xfe,0xaa,0xe8,0x01,0xf0,0x4e,0x0c,0x04,0x00,0xe0,0x00,0x00,
  0x00,0x0d,0x5f,0xfd,0x55,0x1c,0x03,0xf8,0x2f,0x35,0x42,0x00,0x20,0x00,0x00,
  0x00,0x0a,0xaf,0xfa,0xaa,0xab,0x87,0xfc,0x2e,0xea,0x81,0x80,0x40,0x00,0x00,
  0x00,0x15,0x5f,0xf5,0x55,0x01,0xcf,0xfe,0x1f,0x55,0x50,0x60,0x80,0x00,0x00,
  0x00
};
AudioOutputI2S*            i2sOut     = nullptr;
AudioGeneratorMP3*         mp3        = nullptr;
AudioFileSourceHTTPStream* httpSource = nullptr;
AudioFileSourceBuffer*     bufSource  = nullptr;
bool    waitingForInput  = true;
bool    audioPlaying     = false;
bool    aiConnected      = false;
bool    waitingForBoot   = false;
unsigned long audioStartTime = 0; 
AudioFileSourcePROGMEM* progSource = nullptr;
String  currentAIText   = "";
int     scrollOffset    = 0;
int     totalLines      = 0;
int     barPhase        = 0;
WebServer webServer(80);
volatile bool webQuestionReady = false;
String       webQuestion       = "";
String       webLastAnswer     = "";
#define MAX_HISTORY 20
struct ChatMsg { String role; String text; };
ChatMsg chatHistory[MAX_HISTORY];
int chatCount = 0;

void addToHistory(const String& role, const String& text) {
  if (chatCount < MAX_HISTORY) {
    chatHistory[chatCount].role = role;
    chatHistory[chatCount].text = text;
    chatCount++;
  } else {
    for (int i = 0; i < MAX_HISTORY - 1; i++) chatHistory[i] = chatHistory[i+1];
    chatHistory[MAX_HISTORY-1].role = role;
    chatHistory[MAX_HISTORY-1].text = text;
  }
}

TaskHandle_t   blinkHandle = NULL;
volatile bool  blinkActive = false;

void blinkTask(void*) {
  for (;;) {
    if (blinkActive) {
      digitalWrite(BLUE_LED, HIGH); vTaskDelay(200 / portTICK_PERIOD_MS);
      digitalWrite(BLUE_LED, LOW);  vTaskDelay(200 / portTICK_PERIOD_MS);
    } else {
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  }
}
void startBlinking()               { blinkActive = true; }
void stopBlinking(bool on = false) { blinkActive = false; delay(10); digitalWrite(BLUE_LED, on); }

void ensureI2SOutput() {
  if (!i2sOut) {
    rawUninstall();
    i2sOut = new AudioOutputI2S();
    i2sOut->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    i2sOut->SetGain(0.25f);
  }
}

void stopAudioSources() {
  if (mp3)        { if (mp3->isRunning()) mp3->stop(); delete mp3; mp3 = nullptr; }
  if (bufSource)  { bufSource->close(); delete bufSource; bufSource = nullptr; }
  if (httpSource) { httpSource->close(); delete httpSource; httpSource = nullptr; }
  if (progSource) { delete progSource; progSource = nullptr; }
  if (i2sOut)     { delete i2sOut; i2sOut = nullptr; }
  audioPlaying = false;
}


static bool rawInstalled = false;

void rawInstall() {
  if (rawInstalled) return;
  i2s_config_t cfg = {};
  cfg.mode                 = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  cfg.sample_rate          = 22050;
  cfg.bits_per_sample      = I2S_BITS_PER_SAMPLE_16BIT;
  cfg.channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT;
  cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  cfg.intr_alloc_flags     = ESP_INTR_FLAG_LEVEL1;
  cfg.dma_buf_count        = 8;
  cfg.dma_buf_len          = 64;
  cfg.tx_desc_auto_clear   = true;
  i2s_pin_config_t pins = {};
  pins.bck_io_num   = I2S_BCLK;
  pins.ws_io_num    = I2S_LRC;
  pins.data_out_num = I2S_DOUT;
  pins.data_in_num  = I2S_PIN_NO_CHANGE;
  i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);
  rawInstalled = true;
}

void rawUninstall() {
  if (!rawInstalled) return;
  i2s_driver_uninstall(I2S_NUM_0);
  rawInstalled = false;
}

void playRawTone(int freq, int ms, float gain = 0.20f) {
  const int SR = 22050;
  int16_t buf[128];
  int total = SR * ms / 1000;
  float phase = 0, inc = freq > 0 ? (2.f * M_PI * freq / SR) : 0;
  int16_t amp = (int16_t)(32767 * gain);
  for (int i = 0; i < total; i += 64) {
    int chunk = min(64, total - i);
    for (int s = 0; s < chunk; s++) {
      int16_t v = freq > 0 ? (int16_t)(amp * sinf(phase)) : 0;
      buf[s*2] = buf[s*2+1] = v;
      phase += inc;
      if (phase > 2.f * M_PI) phase -= 2.f * M_PI;
    }
    size_t bw = 0;
    i2s_write(I2S_NUM_0, buf, chunk * 4, &bw, portMAX_DELAY);
  }
}

void playSeq(const int* f, const int* d, int n, float g = 0.20f) {
  if (i2sOut) { delete i2sOut; i2sOut = nullptr; }
  rawInstall();
  for (int i = 0; i < n; i++) { playRawTone(f[i], d[i], g); delay(8); }
  rawUninstall();
  ensureI2SOutput();
}
//sound
void playDroneStartup() {
  const int f1[] = {220,262,330,392,440,523,587,659};
  const int d1[] = {80, 80, 80, 80, 90, 90, 100,120};
  const int f2[] = {440,550,660,880,660,550,440,0  };
  const int d2[] = {80, 80, 80, 100,80, 80, 80, 60 };
  const int f3[] = {523,659,784,880,784,659,523,440,330};
  const int d3[] = {80, 80, 80, 120,80, 80, 80, 80, 100};
  playSeq(f1, d1, 8, 0.22f);
  playSeq(f2, d2, 8, 0.22f);
  playSeq(f3, d3, 9, 0.20f);
}

void playQuestionBeep() {
  const int f[] = {600, 800, 1000};
  const int d[] = {55,  55,  75};
  playSeq(f, d, 3, 0.18f);
}

void playResponseBeep() {
  const int f[] = {880, 0, 880};
  const int d[] = {75, 55, 75};
  playSeq(f, d, 3, 0.18f);
}

void playConfirmBeep() {
  const int f[] = {1100, 1400};
  const int d[] = {50,   80};
  playSeq(f, d, 2, 0.16f);
}


int wordWrap(String text, String* lines, int maxL, int maxC) {
  int n = 0;
  while (text.length() && n < maxL) {
    if ((int)text.length() <= maxC) { lines[n++] = text; break; }
    int cut = maxC;
    for (int i = maxC; i > 0; i--) if (text[i] == ' ') { cut = i; break; }
    lines[n++] = text.substring(0, cut);
    text = (cut < (int)text.length()) ? text.substring(cut + 1) : "";
  }
  return n;
}


void showReadyScreen() {
  display.clearDisplay();
  display.drawBitmap(0, 1, image_DolphinMafia_bits, 119, 62, SSD1306_WHITE);
  display.fillRect(0, 54, 128, 10, SSD1306_BLACK);
  display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 55);  display.print("ASK AI...");
  display.display();
}

void showReadyDots(int frame) {
  display.fillRect(0, 54, 128, 10, SSD1306_BLACK);
  display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 55);
  display.print("ASK AI");
  for (int i = 0; i < (frame % 4); i++) display.print(".");
  display.display();
}

void showUserBubble(String text) {
  display.clearDisplay();
  display.fillRect(0, 0, SCREEN_WIDTH, 12, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK); display.setTextSize(1);
  display.setCursor(2, 2); display.print("YOU:");
  display.setTextColor(SSD1306_WHITE);
  String lines[5]; int n = wordWrap(text, lines, 5, 21);
  for (int i = 0; i < n; i++) { display.setCursor(2, 16 + i*10); display.print(lines[i]); }
  display.display();
}

void showThinkingScreen() {
  display.clearDisplay();
  display.fillRect(0, 0, SCREEN_WIDTH, 12, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK); display.setTextSize(1);
  display.setCursor(2, 2); display.print("AI THINKING...");
  display.setTextColor(SSD1306_WHITE);
  display.drawRoundRect(50, 20, 28, 20, 4, SSD1306_WHITE);
  display.drawLine(64, 18, 64, 20, SSD1306_WHITE);
  display.setCursor(14, 42); display.print("Asking Ollama...");
  display.display();
}

void showDisconnected() {
  for (int i = 0; i < 3; i++) {
    display.clearDisplay();
    display.drawLine(20,20,108,44,SSD1306_WHITE);
    display.drawLine(108,20,20,44,SSD1306_WHITE);
    display.display(); delay(200);
    display.clearDisplay(); display.display(); delay(150);
  }
  display.clearDisplay(); display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(16,20); display.print("AI DISCONNECTED");
  display.setCursor(22,36); display.print("Check Ollama");
  display.setCursor(22,48); display.print("& WiFi...");
  display.display();
}

static String _wrLines[40];
static int    _wrCount = 0;

void cacheWrap(const String& text) {
  _wrCount = 0;

  String seg = text;
  int newlinePos;
  while ((newlinePos = seg.indexOf('\n')) != -1 && _wrCount < 38) {
    String line = seg.substring(0, newlinePos);
    line.trim();
    if (line.length() == 0) {
      if (_wrCount > 0) _wrLines[_wrCount++] = "";  
    } else {
      int added = wordWrap(line, _wrLines + _wrCount, 40 - _wrCount, 21);
      _wrCount += added;
    }
    seg = seg.substring(newlinePos + 1);
  }
  seg.trim();
  if (seg.length() > 0 && _wrCount < 40) {
    int added = wordWrap(seg, _wrLines + _wrCount, 40 - _wrCount, 21);
    _wrCount += added;
  }
  totalLines = _wrCount;
}

void showAIWithBars(int scroll) {
  display.clearDisplay();
  display.fillRect(0, 0, SCREEN_WIDTH, 2, SSD1306_WHITE);
  int dx = (barPhase * 14) % SCREEN_WIDTH;
  display.fillRect(dx, 0, 8, 2, SSD1306_BLACK);
  display.fillRect(0, SCREEN_HEIGHT-2, SCREEN_WIDTH, 2, SSD1306_WHITE);
  int dx2 = SCREEN_WIDTH - 8 - ((barPhase * 14) % SCREEN_WIDTH);
  if (dx2 < 0) dx2 = 0;
  display.fillRect(dx2, SCREEN_HEIGHT-2, 8, 2, SSD1306_BLACK);
  display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 4); display.print("AI:");
  for (int i = 0; i < 4; i++) {
    int li = scroll + i;
    if (li >= _wrCount) break;
    display.setCursor(2, 14 + i*10);
    display.print(_wrLines[li]);
  }
  if (scroll + 4 < _wrCount) {
    display.fillTriangle(120,58, 126,58, 123,61, SSD1306_WHITE);
  }
  display.display();
}

void showWaitBoot() {
  display.clearDisplay();
  display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 0); display.print("AI:");
  for (int i = 0; i < 5; i++) {
    int li = scrollOffset + i;
    if (li >= _wrCount) break;
    display.setCursor(2, 10 + i*10);
    display.print(_wrLines[li]);
  }

  if (scrollOffset + 5 < _wrCount) {
    display.fillTriangle(120,58, 126,58, 123,62, SSD1306_WHITE);
  }
  display.display();
}

String urlEncode(const String& s) {
  String out; out.reserve(s.length()*3);
  for (int i = 0; i < (int)s.length(); i++) {
    char c = s[i];
    if ((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||
        c=='-'||c=='_'||c=='.'||c=='~') out += c;
    else if (c==' ') out += '+';
    else { char b[4]; snprintf(b,4,"%%%02X",(unsigned char)c); out+=b; }
  }
  return out;
}

String htmlEscape(const String& s) {
  String out;
  for (int i = 0; i < (int)s.length(); i++) {
    char c = s[i];
    if      (c == '&')  out += "&amp;";
    else if (c == '<')  out += "&lt;";
    else if (c == '>')  out += "&gt;";
    else if (c == '"')  out += "&quot;";
    else if (c == '\'') out += "&#39;";
    else                out += c;
  }
  return out;
}


String askOllama(const String& prompt) {
  WiFiClient wc; HTTPClient http;
  String path = "/api/generate";
  Serial.println("[Ollama] POST http://"+String(OLLAMA_IP)+":"+String(OLLAMA_PORT)+path);
  http.begin(wc, OLLAMA_IP, OLLAMA_PORT, path);
  http.addHeader("Content-Type","application/json");
  http.addHeader("Accept","application/json");
  http.setConnectTimeout(10000); http.setTimeout(180000);

 
  StaticJsonDocument<2048> req;
  req["model"]  = OLLAMA_MODEL;
  req["prompt"] = prompt;
  req["system"] = "You are a concise AI assistant. Give clear, accurate answers. " //customize the prompt here
                  "Use bullet points or numbered lists when listing items. "
                  "Keep answers under 200 words.";
  req["stream"] = false;
  req.createNestedObject("options")["num_predict"] = 300;
  String body; serializeJson(req, body);
  Serial.println("[Ollama] Body: "+body);

  int code = http.POST(body);
  Serial.println("[Ollama] HTTP code: "+String(code));
  if (code != 200) { http.end(); return ""; }

  DynamicJsonDocument res(8192);
  String rb = http.getString(); http.end();
  Serial.println("[Ollama] Response length: "+String(rb.length()));
  if (deserializeJson(res, rb)) return "";

  String ans = res["response"].as<String>();
  ans.trim();


  {
  
    int sysStart = ans.indexOf("<<SYS>>");
    while (sysStart != -1) {
      int sysEnd = ans.indexOf("<</SYS>>", sysStart);
      if (sysEnd != -1) {
        ans = ans.substring(0, sysStart) + ans.substring(sysEnd + 8);
      } else {
        ans = ans.substring(0, sysStart);
      }
      sysStart = ans.indexOf("<<SYS>>");
    }
    
    ans.replace("[INST]",  "");
    ans.replace("[/INST]", "");
    ans.replace("<INST>",  "");
    ans.replace("</INST>", "");
  
    String fillers[] = {
      "Sure, here's the answer:",
      "Sure, here is the answer:",
      "Sure! Here's the answer:",
      "Certainly! Here's the answer:",
      "Of course! Here's the answer:",
      "Here's the answer:",
      "Here is the answer:",
      "Sure, here's",
      "Sure, here is"
    };
    for (int f = 0; f < 9; f++) {
      if (ans.startsWith(fillers[f])) {
        ans = ans.substring(fillers[f].length());
        ans.trim();
        break;
      }
    }
    ans.trim();
  }


  ans.replace("\r\n", "\n");
  ans.replace("\r",   "\n");
  while (ans.indexOf("\n\n\n") != -1) ans.replace("\n\n\n", "\n\n");

  // Truncate for OLED word-wrap cache (keep newlines intact)
  if (ans.length() > 1200) ans = ans.substring(0, 1197) + "...";
  return ans;
}

void speakText(const String& text) {
  stopAudioSources();

//streamElements could cause issues , you can change to google tts (no api)
  String safeText = text;
  if (safeText.length() > 200) safeText = safeText.substring(0, 200);
  String encodedText = urlEncode(safeText);
  String url = "http://api.streamelements.com/kappa/v2/speech?voice=" +
               String(SE_VOICE) + "&text=" + encodedText;

  Serial.println("[TTS] Requesting StreamElements: " + url);

  WiFiClientSecure wc;
  wc.setInsecure(); 
  HTTPClient http;
  

  if (url.startsWith("http://")) {
    url.replace("http://", "https://");
  }

  http.begin(wc, url);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setConnectTimeout(10000);
  http.setTimeout(15000);
  http.addHeader("Accept", "audio/mpeg");

  int code = http.GET();
  Serial.println("[TTS] HTTP code: " + String(code));
  if (code != 200) {
    Serial.println("[TTS] StreamElements error — skipping audio.");
    http.end();
    waitingForBoot = true;
    showWaitBoot();
    return;
  }


  WiFiClient* stream = http.getStreamPtr();
  const size_t CHUNK = 2048;
  size_t allocated = 64 * 1024;
  uint8_t* audioData = (uint8_t*)malloc(allocated);
  if (!audioData) {
    Serial.println("[TTS] malloc failed.");
    http.end(); waitingForBoot = true; showWaitBoot(); return;
  }
  size_t audioLen = 0;
  unsigned long dl_start = millis();
  while (millis() - dl_start < 15000) {
    int avail = stream->available();
    if (avail > 0) {
      if (audioLen + avail > allocated) {
        allocated += avail + CHUNK;
        uint8_t* tmp = (uint8_t*)realloc(audioData, allocated);
        if (!tmp) { Serial.println("[TTS] realloc failed."); break; }
        audioData = tmp;
      }
      int rd = stream->readBytes(audioData + audioLen, avail);
      audioLen += rd;
    } else if (!stream->connected()) break;
    else delay(5);
  }
  http.end();
  Serial.println("[TTS] Downloaded " + String(audioLen) + " bytes.");

  if (audioLen == 0) {
    free(audioData);
    waitingForBoot = true; showWaitBoot(); return;
  }

  ensureI2SOutput();
  progSource = new AudioFileSourcePROGMEM(audioData, audioLen);
  bufSource  = new AudioFileSourceBuffer(progSource, 4096);
  mp3        = new AudioGeneratorMP3();
  mp3->begin(bufSource, i2sOut);
  audioPlaying   = true;
  audioStartTime = millis();
  Serial.println("[TTS] Audio started.");

  
  static uint8_t* _ttsHeap = nullptr;
  if (_ttsHeap) { free(_ttsHeap); }
  _ttsHeap = audioData;
}


bool testTCP() {
  WiFiClient c;
  Serial.println("[Diag] Testing TCP to Ollama at "+String(OLLAMA_IP)+":"+String(OLLAMA_PORT)+" ...");
  display.clearDisplay(); display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(4,10); display.print("Testing TCP..."); display.display();
  if (c.connect(OLLAMA_IP, OLLAMA_PORT)) {
    c.stop(); Serial.println("[Diag] TCP OK");
    display.setCursor(4,26); display.print("TCP OK!"); display.display(); delay(800); return true;
  }
  Serial.println("[Diag] TCP FAILED");
  display.setCursor(4,26); display.print("TCP FAILED!");
  display.setCursor(4,40); display.print("Start Ollama on PC"); display.display(); delay(2000); return false;
}

void testHTTP() {
  Serial.println("[Diag] Testing HTTP GET http://"+String(OLLAMA_IP)+":"+String(OLLAMA_PORT)+"/ ...");
  WiFiClient wc; HTTPClient http;
  http.begin(wc, OLLAMA_IP, OLLAMA_PORT, "/");
  http.setConnectTimeout(5000); http.setTimeout(20000);
  int code = http.GET();
  Serial.print("[Diag] HTTP response code: "); Serial.println(code);
  display.clearDisplay(); display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(4,10); display.print("HTTP GET /");
  display.setCursor(4,26);
  if (code>0) { display.print("HTTP "); display.print(code); display.print(" OK"); }
  else        { display.print("HTTP ERR: "); display.print(code); }
  display.display(); http.end(); delay(1000);
}

void showStartupAnimation() {
  for (int r=0;r<=30;r+=3){display.clearDisplay();display.drawRoundRect(64-r,32-r/2,r*2,r,4,SSD1306_WHITE);display.display();delay(30);}
  for (int y=0;y<SCREEN_HEIGHT;y+=2){display.drawFastHLine(0,y,SCREEN_WIDTH,SSD1306_WHITE);display.display();delay(8);}
  delay(100);
  for (int x=SCREEN_WIDTH;x>=14;x-=8){display.clearDisplay();display.setTextSize(2);display.setTextColor(SSD1306_WHITE);display.setCursor(x,10);display.print("ESP32");display.display();delay(20);}
  delay(150);
  display.clearDisplay();
  display.drawRect(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,SSD1306_WHITE);
  display.setTextSize(2);display.setTextColor(SSD1306_WHITE);display.setCursor(14,10);display.print("ESP32");
  display.setTextSize(1);display.setCursor(22,32);display.print("AI  ASSISTANT");
  display.setCursor(28,46);display.print("Powered by");display.setCursor(28,55);display.print("Ollama LLM");
  display.display();delay(2000);
  for(int i=0;i<60;i++){display.drawPixel(random(SCREEN_WIDTH),random(SCREEN_HEIGHT),SSD1306_BLACK);display.display();}
  delay(200);display.clearDisplay();display.display();
}


void goToNextPrompt() {
  stopAudioSources();
  stopBlinking(false);
  waitingForBoot  = false;
  waitingForInput = true;
  currentAIText   = "";
  scrollOffset    = 0;
  showReadyScreen();
  Serial.println("> ");
}

String buildChatHistoryJSON() {
  String json = "[";
  for (int i = 0; i < chatCount; i++) {
    if (i > 0) json += ",";
    json += "{\"role\":\"" + chatHistory[i].role + "\",\"text\":\"" + htmlEscape(chatHistory[i].text) + "\"}";
  }
  json += "]";
  return json;
}
//webserver
void handleRoot() {
  String html = F("<!DOCTYPE html><html lang='en'><head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1.0'>"
    "<title>ESP32 AI Assistant</title>"
    "<style>"
    "@import url('https://fonts.googleapis.com/css2?family=Space+Grotesk:wght@300;400;500;600;700&family=JetBrains+Mono:wght@400;500&display=swap');"
    "*{margin:0;padding:0;box-sizing:border-box;font-family:'Space Grotesk',sans-serif;}"
    "::-webkit-scrollbar{width:6px}"
    "::-webkit-scrollbar-track{background:rgba(255,255,255,0.03)}"
    "::-webkit-scrollbar-thumb{background:rgba(249,115,22,0.4);border-radius:3px}"
    "@keyframes fadeSlideUp{from{opacity:0;transform:translateY(12px)}to{opacity:1;transform:translateY(0)}}"
    "@keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}"
    "@keyframes floatA{0%,100%{transform:translateY(0) rotate(0deg)}50%{transform:translateY(-18px) rotate(6deg)}}"
    "@keyframes floatB{0%,100%{transform:translateY(0) rotate(0deg)}50%{transform:translateY(-12px) rotate(-4deg)}}"
    "@keyframes spinRing{from{transform:rotate(0deg)}to{transform:rotate(360deg)}}"
    "@keyframes barAnim{0%{width:0%}100%{width:100%}}"
    "body{background:linear-gradient(135deg,#0f1117 0%,#151922 50%,#1a1f2e 100%);min-height:100vh;color:#e8eaf0;overflow:hidden;}"
    ".orb{position:fixed;border-radius:50%;filter:blur(80px);pointer-events:none;z-index:0;}"
    ".orb1{width:350px;height:350px;background:radial-gradient(circle,rgba(249,115,22,.18) 0%,transparent 70%);top:-80px;left:-80px;animation:floatA 12s ease-in-out infinite;}"
    ".orb2{width:280px;height:280px;background:radial-gradient(circle,rgba(234,88,12,.12) 0%,transparent 70%);bottom:-60px;right:-60px;animation:floatB 15s ease-in-out infinite;}"
    ".orb3{width:180px;height:180px;background:radial-gradient(circle,rgba(249,115,22,.08) 0%,transparent 70%);top:40%;left:40%;animation:floatA 20s ease-in-out infinite reverse;}"
    ".layout{display:flex;height:100vh;position:relative;z-index:1;}"
    ".sidebar{width:260px;background:rgba(255,255,255,0.03);backdrop-filter:blur(20px);border-right:1px solid rgba(255,255,255,0.08);display:flex;flex-direction:column;flex-shrink:0;}"
    ".logo-section{padding:20px;border-bottom:1px solid rgba(255,255,255,0.06);}"
    ".logo-row{display:flex;align-items:center;gap:12px;}"
    ".logo-icon{width:42px;height:42px;background:linear-gradient(135deg,#f97316,#ea580c);border-radius:12px;display:flex;align-items:center;justify-content:center;font-size:20px;flex-shrink:0;box-shadow:0 4px 16px rgba(249,115,22,.3);}"
    ".logo-text h1{font-size:1.1em;font-weight:700;letter-spacing:-.02em;}"
    ".logo-text p{font-size:.72em;color:rgba(255,255,255,.4);margin-top:1px;}"
    ".device-status{padding:12px 20px;border-bottom:1px solid rgba(255,255,255,0.06);}"
    ".status-pill{display:inline-flex;align-items:center;gap:6px;background:rgba(249,115,22,.1);border:1px solid rgba(249,115,22,.25);border-radius:20px;padding:5px 12px;font-size:.75em;color:#f97316;}"
    ".status-dot{width:7px;height:7px;border-radius:50%;background:#4ade80;animation:pulse 2s infinite;}"
    ".status-dot.offline{background:#f87171;animation:none;}"
    ".nav-section{padding:16px 12px;flex:1;overflow-y:auto;}"
    ".nav-label{font-size:.68em;color:rgba(255,255,255,.3);letter-spacing:.08em;text-transform:uppercase;padding:0 10px;margin-bottom:8px;margin-top:16px;}"
    ".nav-item{display:flex;align-items:center;gap:10px;padding:10px 12px;border-radius:10px;cursor:pointer;transition:all .2s;font-size:.85em;color:rgba(255,255,255,.55);border:1px solid transparent;}"
    ".nav-item:hover{background:rgba(255,255,255,.05);color:rgba(255,255,255,.85);}"
    ".nav-item.active{background:rgba(249,115,22,.12);border-color:rgba(249,115,22,.2);color:#f97316;}"
    ".nav-item .icon{font-size:1em;width:18px;text-align:center;}"
    ".recent-item{display:flex;align-items:center;gap:10px;padding:8px 12px;border-radius:8px;cursor:pointer;transition:all .2s;}"
    ".recent-item:hover{background:rgba(255,255,255,.04);}"
    ".recent-avatar{width:30px;height:30px;border-radius:8px;background:rgba(249,115,22,.15);display:flex;align-items:center;justify-content:center;font-size:.8em;flex-shrink:0;}"
    ".recent-info p{font-size:.8em;color:rgba(255,255,255,.7);white-space:nowrap;overflow:hidden;text-overflow:ellipsis;max-width:140px;}"
    ".recent-info span{font-size:.68em;color:rgba(255,255,255,.3);}"
    ".sidebar-footer{padding:14px;border-top:1px solid rgba(255,255,255,.06);}"
    ".new-chat-btn{width:100%;padding:11px;background:linear-gradient(135deg,#f97316,#ea580c);border:none;border-radius:10px;color:#fff;font-size:.88em;font-weight:600;cursor:pointer;transition:all .2s;letter-spacing:.01em;}"
    ".new-chat-btn:hover{opacity:.88;transform:translateY(-1px);box-shadow:0 6px 20px rgba(249,115,22,.35);}"
    ".main{flex:1;display:flex;flex-direction:column;min-width:0;}"
    ".topbar{padding:14px 22px;background:rgba(255,255,255,.02);backdrop-filter:blur(10px);border-bottom:1px solid rgba(255,255,255,.06);display:flex;align-items:center;justify-content:space-between;}"
    ".topbar-left{display:flex;align-items:center;gap:12px;}"
    ".topbar-left h2{font-size:.98em;font-weight:600;}"
    ".topbar-left p{font-size:.75em;color:rgba(255,255,255,.4);}"
    ".topbar-actions{display:flex;gap:8px;}"
    ".icon-btn{width:34px;height:34px;border-radius:8px;border:1px solid rgba(255,255,255,.08);background:rgba(255,255,255,.04);color:rgba(255,255,255,.5);cursor:pointer;display:flex;align-items:center;justify-content:center;font-size:.85em;transition:all .2s;}"
    ".icon-btn:hover{background:rgba(255,255,255,.08);color:#fff;}"
    ".chat-area{flex:1;overflow-y:auto;padding:24px 22px;display:flex;flex-direction:column;gap:18px;}"
    ".welcome{display:flex;flex-direction:column;align-items:center;justify-content:center;flex:1;padding:40px;text-align:center;gap:16px;}"
    ".welcome-logo{width:64px;height:64px;background:linear-gradient(135deg,#f97316,#ea580c);border-radius:20px;display:flex;align-items:center;justify-content:center;font-size:28px;box-shadow:0 8px 32px rgba(249,115,22,.3);margin-bottom:8px;}"
    ".welcome h3{font-size:1.4em;font-weight:700;}"
    ".welcome p{font-size:.88em;color:rgba(255,255,255,.45);max-width:340px;line-height:1.6;}"
    ".chips{display:flex;flex-wrap:wrap;gap:8px;justify-content:center;margin-top:6px;}"
    ".chip{padding:7px 14px;border:1px solid rgba(249,115,22,.2);border-radius:20px;font-size:.78em;color:rgba(249,115,22,.85);cursor:pointer;transition:all .2s;background:rgba(249,115,22,.05);}"
    ".chip:hover{background:rgba(249,115,22,.12);border-color:rgba(249,115,22,.4);}"
    ".msg-row{display:flex;gap:10px;animation:fadeSlideUp .28s ease-out;}"
    ".msg-row.user{flex-direction:row-reverse;}"
    ".msg-avatar{width:32px;height:32px;border-radius:10px;display:flex;align-items:center;justify-content:center;font-size:.85em;flex-shrink:0;margin-top:2px;}"
    ".msg-avatar.ai{background:linear-gradient(135deg,#f97316,#ea580c);box-shadow:0 3px 12px rgba(249,115,22,.25);}"
    ".msg-avatar.user{background:rgba(255,255,255,.1);border:1px solid rgba(255,255,255,.12);}"
    ".msg-bubble{max-width:72%;padding:12px 16px;border-radius:16px;font-size:.88em;line-height:1.6;}"
    ".msg-bubble.ai{background:rgba(255,255,255,.07);border:1px solid rgba(255,255,255,.1);border-radius:4px 16px 16px 16px;}"
    ".msg-bubble.user{background:linear-gradient(135deg,#f97316,#ea580c);border-radius:16px 4px 16px 16px;}"
    ".msg-meta{font-size:.68em;color:rgba(255,255,255,.3);margin-top:5px;}"
    ".typing{display:flex;gap:4px;padding:4px 0;}"
    ".typing-dot{width:7px;height:7px;border-radius:50%;background:rgba(255,255,255,.4);animation:pulse 1.4s infinite;}"
    ".typing-dot:nth-child(2){animation-delay:.2s;}"
    ".typing-dot:nth-child(3){animation-delay:.4s;}"
    ".input-bar{padding:16px 22px;border-top:1px solid rgba(255,255,255,.06);background:rgba(255,255,255,.02);}"
    ".input-wrap{max-width:820px;margin:0 auto;}"
    ".input-row{display:flex;align-items:flex-end;gap:10px;background:rgba(255,255,255,.05);border:1px solid rgba(255,255,255,.1);border-radius:14px;padding:10px 14px;transition:border-color .2s;}"
    ".input-row:focus-within{border-color:rgba(249,115,22,.4);box-shadow:0 0 0 3px rgba(249,115,22,.08);}"
    ".input-row textarea{flex:1;background:none;border:none;outline:none;color:#e8eaf0;font-size:.9em;resize:none;max-height:120px;min-height:24px;line-height:1.5;font-family:'Space Grotesk',sans-serif;}"
    ".input-row textarea::placeholder{color:rgba(255,255,255,.3);}"
    ".attach-btn{color:rgba(255,255,255,.35);cursor:pointer;transition:color .2s;font-size:.95em;padding:2px;}"
    ".attach-btn:hover{color:rgba(255,255,255,.7);}"
    ".send-btn{width:36px;height:36px;border-radius:10px;background:linear-gradient(135deg,#f97316,#ea580c);border:none;color:#fff;cursor:pointer;display:flex;align-items:center;justify-content:center;font-size:.9em;transition:all .2s;flex-shrink:0;}"
    ".send-btn:hover{opacity:.88;transform:scale(1.05);box-shadow:0 4px 14px rgba(249,115,22,.4);}"
    ".send-btn:disabled{opacity:.4;cursor:not-allowed;transform:none;}"
    ".input-hint{font-size:.7em;color:rgba(255,255,255,.25);margin-top:8px;text-align:center;}"
    "@media(max-width:640px){.sidebar{display:none}.chat-area{padding:16px 12px}.input-bar{padding:12px}.msg-bubble{max-width:88%}}"
    "</style></head><body>"
    "<div class='orb orb1'></div><div class='orb orb2'></div><div class='orb orb3'></div>"
    "<div class='layout'>"
    "<div class='sidebar'>"
    "<div class='logo-section'>"
    "<div class='logo-row'>"
    "<div class='logo-icon'>&#129302;</div>"
    "<div class='logo-text'><h1>ESP32 AI</h1><p>Powered by Ollama</p></div>"
    "</div></div>"
    "<div class='device-status'>"
    "<div class='status-pill'>"
    "<div class='status-dot' id='statusDot'></div>"
    "<span id='statusText'>Device Online</span>"
    "</div></div>"
    "<div class='nav-section'>"
    "<div class='nav-label'>Menu</div>"
    "<div class='nav-item active'><span class='icon'>&#128172;</span> Chat</div>"
    "<div class='nav-item'><span class='icon'>&#128202;</span> Analytics</div>"
    "<div class='nav-item'><span class='icon'>&#9881;</span> Settings</div>"
    "<div class='nav-label'>Recent Chats</div>"
    "<div class='recent-item'><div class='recent-avatar'>&#127760;</div><div class='recent-info'><p>General Q&amp;A</p><span>Just now</span></div></div>"
    "<div class='recent-item'><div class='recent-avatar'>&#128187;</div><div class='recent-info'><p>Code Help</p><span>1h ago</span></div></div>"
    "</div>"
    "<div class='sidebar-footer'>"
    "<button class='new-chat-btn' onclick='clearChat()'>&#43; New Chat</button>"
    "</div></div>"
    "<div class='main'>"
    "<div class='topbar'>"
    "<div class='topbar-left'>"
    "<div><h2>Chat with ESP32 AI</h2><p>ESP32 device assistant via Ollama</p></div>"
    "</div>"
    "<div class='topbar-actions'>"
    "<button class='icon-btn' title='Refresh status' onclick='checkStatus()'>&#8635;</button>"
    "<button class='icon-btn' title='Clear chat' onclick='clearChat()'>&#128465;</button>"
    "</div></div>"
    "<div class='chat-area' id='chatArea'>"
    "<div class='welcome' id='welcomeScreen'>"
    "<div class='welcome-logo'>&#129302;</div>"
    "<h3>Hello, I&apos;m ESP32 AI</h3>"
    "<p>Running on your ESP32 via Ollama. Ask me anything and I&apos;ll respond on screen and through the speaker!</p>"
    "<div class='chips'>"
    "<span class='chip' onclick='quickAsk(\"What can you do?\")'>What can you do?</span>"
    "<span class='chip' onclick='quickAsk(\"Tell me a fun fact\")'>Fun fact</span>"
    "<span class='chip' onclick='quickAsk(\"What time is it?\")'>What time is it?</span>"
    "<span class='chip' onclick='quickAsk(\"Tell me a joke\")'>Tell me a joke</span>"
    "</div></div>"
    "</div>"
    "<div class='input-bar'>"
    "<div class='input-wrap'>"
    "<div class='input-row'>"
    "<span class='attach-btn'>&#128206;</span>"
    "<textarea id='msgInput' placeholder='Ask anything...' rows='1' onkeydown='handleKey(event)' oninput='autoResize(this)'></textarea>"
    "<span class='attach-btn'>&#127908;</span>"
    "<button class='send-btn' id='sendBtn' onclick='sendMsg()'>&#10148;</button>"
    "</div>"
    "<div class='input-hint'>Press Enter to send &bull; Shift+Enter for new line &bull; Answer spoken on device</div>"
    "</div></div>"
    "</div></div>"
    "<script>"
    // ── STATE ──────────────────────────────────────────────────────────────────
    "var isAsking=false;"
    "var deviceIP=location.hostname;"
    // ── HELPERS ────────────────────────────────────────────────────────────────
    "function ts(){var d=new Date();return d.getHours().toString().padStart(2,'0')+':'+d.getMinutes().toString().padStart(2,'0');}"
    "function autoResize(t){t.style.height='auto';t.style.height=Math.min(t.scrollHeight,120)+'px';}"
    "function handleKey(e){if(e.key==='Enter'&&!e.shiftKey){e.preventDefault();sendMsg();}}"
    "function quickAsk(q){document.getElementById('msgInput').value=q;sendMsg();}"
    "function escHtml(s){var d=document.createElement('div');d.appendChild(document.createTextNode(s));return d.innerHTML;}"
    // ── formatText: lightweight markdown-lite for AI responses ────────────────
    "function formatText(raw,isAI){"
    "if(!isAI)return'<span>'+escHtml(raw)+'</span>';"
    "var lines=raw.split(/\\r?\\n/);"
    "var html='';"
    "var inList=false;"
    "for(var i=0;i<lines.length;i++){"
    "var l=lines[i];"
    // Bullet point: lines starting with "- " or "* " or "• "
    "if(/^[-*•]\\s/.test(l)){"
    "if(!inList){html+='<ul style=\"margin:.4em 0 .4em 1.2em;padding:0\">';inList=true;}"
    "html+='<li>'+escHtml(l.replace(/^[-*•]\\s+/,''))+'</li>';"
    // Numbered list
    "}else if(/^\\d+[.)\\s]/.test(l)){"
    "if(!inList){html+='<ol style=\"margin:.4em 0 .4em 1.2em;padding:0\">';inList=true;}"
    "html+='<li>'+escHtml(l.replace(/^\\d+[.)\\s]+/,''))+'</li>';"
    "}else{"
    "if(inList){html+=inList&&/^<ul/.test(html.slice(-200))?'</ul>':'</ol>';inList=false;}"
    // Heading: line ending with ":" and no trailing text (acts as bold label)
    "if(/^[A-Z][^:]{2,40}:$/.test(l.trim())){"
    "html+='<div style=\"font-weight:600;margin-top:.6em;color:#fba96a\">'+escHtml(l.trim())+'</div>';"
    "}else if(l.trim()===''){"
    "html+='<div style=\"height:.4em\"></div>';"  // blank line spacer
    "}else{"
    // Inline **bold** and *italic*
    "var t=escHtml(l);"
    "t=t.replace(/\\*\\*(.+?)\\*\\*/g,'<strong>$1</strong>');"
    "t=t.replace(/\\*(.+?)\\*/g,'<em>$1</em>');"
    "html+='<div>'+t+'</div>';"
    "}}"
    "}"
    "if(inList)html+='</ul>';"
    "return html;"
    "}"
    "function clearChat(){"
    "document.getElementById('chatArea').innerHTML=\"<div class='welcome' id='welcomeScreen'>"
    "<div class='welcome-logo'>&#129302;</div><h3>Hello, I'm ESP32 AI</h3>"
    "<p>Running on your ESP32 via Ollama. Ask me anything!</p>"
    "<div class='chips'>"
    "<span class='chip' onclick=\\\"quickAsk('What can you do?')\\\">What can you do?</span>"
    "<span class='chip' onclick=\\\"quickAsk('Tell me a fun fact')\\\">Fun fact</span>"
    "<span class='chip' onclick=\\\"quickAsk('Tell me a joke')\\\">Tell me a joke</span>"
    "</div></div>\";}"
    // ── STATUS ─────────────────────────────────────────────────────────────────
    "function checkStatus(){"
    "fetch('/status').then(r=>r.json()).then(d=>{"
    "var dot=document.getElementById('statusDot');"
    "var txt=document.getElementById('statusText');"
    "if(d.online){dot.className='status-dot';txt.textContent='Device Online';}"
    "else{dot.className='status-dot offline';txt.textContent='Device Offline';}"
    "}).catch(()=>{"
    "document.getElementById('statusDot').className='status-dot offline';"
    "document.getElementById('statusText').textContent='Unreachable';});}"
    // ── BUBBLES ────────────────────────────────────────────────────────────────
    "function addBubble(role,text,time){"
    "var w=document.getElementById('welcomeScreen');"
    "if(w)w.remove();"
    "var ca=document.getElementById('chatArea');"
    "var row=document.createElement('div');"
    "row.className='msg-row '+(role==='user'?'user':'');"
    "var avatarIcon=role==='user'?'&#128100;':'&#129302;';"
    "var avatarCls=role==='user'?'user':'ai';"
    "var bubbleCls=role==='user'?'user':'ai';"
    "var content=formatText(text,role==='ai');"
    "row.innerHTML='<div class=\"msg-avatar '+avatarCls+'\">'+avatarIcon+'</div>'"
    "+'<div><div class=\"msg-bubble '+bubbleCls+'\">'+content+'</div>'"
    "+'<div class=\"msg-meta\">'+(role==='user'?'You':'ESP32 AI')+' &bull; '+(time||ts())+'</div></div>';"
    "ca.appendChild(row);"
    "ca.scrollTop=ca.scrollHeight;"
    "}"
    "function addTyping(){"
    "var ca=document.getElementById('chatArea');"
    "var row=document.createElement('div');"
    "row.className='msg-row';row.id='typingRow';"
    "row.innerHTML='<div class=\"msg-avatar ai\">&#129302;</div>'"
    "+'<div class=\"msg-bubble ai\"><div class=\"typing\">"
    "<div class=\"typing-dot\"></div><div class=\"typing-dot\"></div><div class=\"typing-dot\"></div>"
    "</div></div>';"
    "ca.appendChild(row);"
    "ca.scrollTop=ca.scrollHeight;"
    "}"
    "function removeTyping(){var r=document.getElementById('typingRow');if(r)r.remove();}"
    // ── SEND — non-blocking: post question, then poll /result ──────────────────
    "function sendMsg(){"
    "if(isAsking)return;"
    "var inp=document.getElementById('msgInput');"
    "var q=inp.value.trim();"
    "if(!q)return;"
    "isAsking=true;"
    "var btn=document.getElementById('sendBtn');"
    "btn.disabled=true;"
    "addBubble('user',q,ts());"
    "inp.value='';inp.style.height='auto';"
    "addTyping();"
    "fetch('/ask?q='+encodeURIComponent(q))"
    ".then(r=>{"
    "  if(!r.ok)throw new Error('ask failed');"
    "  pollResult();"
    "}).catch(err=>{"
    "  removeTyping();"
    "  addBubble('ai','Error: Could not reach device. Check your WiFi connection.',ts());"
    "  isAsking=false;btn.disabled=false;"
    "});}"
    // ── POLL /result every 800 ms until answer arrives ─────────────────────────
    "function pollResult(){"
    "fetch('/result').then(r=>{"
    "  if(r.status===202){setTimeout(pollResult,800);return;}"  // still thinking
    "  return r.text().then(ans=>{"
    "    removeTyping();"
    "    if(ans&&ans.length>0)addBubble('ai',ans,ts());"
    "    isAsking=false;document.getElementById('sendBtn').disabled=false;"
    "    document.getElementById('msgInput').focus();"
    "  });"
    "}).catch(()=>{"
    "  removeTyping();"
    "  addBubble('ai','Error: lost connection while waiting for answer.',ts());"
    "  isAsking=false;document.getElementById('sendBtn').disabled=false;"
    "});}"
    // ── INIT ───────────────────────────────────────────────────────────────────
    "checkStatus();"
    "setInterval(checkStatus,15000);"
    "document.getElementById('msgInput').focus();"
    "</script></body></html>");

  webServer.send(200, "text/html; charset=utf-8", html);
}

//

void handleAsk() {
  if (!webServer.hasArg("q") || webServer.arg("q").length() == 0) {
    webServer.send(400, "text/plain", "Missing question");
    return;
  }
  String q = webServer.arg("q");
  q.trim();
  Serial.println("[WEB] Question: " + q);
  addToHistory("user", q);

  if (webQuestionReady) {
   
    webServer.send(503, "text/plain", "Busy");
    return;
  }

  webLastAnswer    = "";  
  webQuestion      = q;
  webQuestionReady = true;

  webServer.send(200, "text/plain", "ok");
}


void handleResult() {
  if (webLastAnswer.length() > 0) {
  
    String ans = webLastAnswer;
    webLastAnswer = "";
    addToHistory("ai", ans);
    webServer.send(200, "text/plain", ans);
  } else if (webQuestionReady) {
    
    webServer.send(202, "text/plain", "");
  } else {
   
    webServer.send(204, "text/plain", "");
  }
}


void handleStatus() {
  String json = "{\"online\":true,\"model\":\"";
  json += String(OLLAMA_MODEL);
  json += "\",\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\",\"audioPlaying\":";
  json += audioPlaying ? "true" : "false";
  json += ",\"waitingForInput\":";
  json += waitingForInput ? "true" : "false";
  json += "}";
  webServer.send(200, "application/json", json);
}
void setupWebServer() {
  webServer.on("/",       handleRoot);
  webServer.on("/ask",    handleAsk);
  webServer.on("/result", handleResult);  
  webServer.on("/status", handleStatus);
  webServer.begin();
  Serial.println("[Web] Server started on http://" + WiFi.localIP().toString());
}


void setup() {
  Serial.begin(115200); delay(500);
  pinMode(BLUE_LED, OUTPUT); digitalWrite(BLUE_LED, LOW);

  xTaskCreatePinnedToCore(blinkTask,"blink",1024,NULL,1,&blinkHandle,0);

  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!"); while(true) delay(100);
  }

  showStartupAnimation();

  display.clearDisplay(); display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(8,20); display.print("Connecting to WiFi"); display.display();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int att=0;
  while (WiFi.status()!=WL_CONNECTED) {
    delay(500); Serial.print("."); display.print("."); display.display();
    if(++att>40){ display.clearDisplay();display.setCursor(0,10);display.print("WiFi FAILED!");display.display();while(true)delay(1000);}
  }
  aiConnected=true;
  Serial.println("\n[WiFi] Connected: "+WiFi.localIP().toString());
  display.clearDisplay();
  for(int r=2;r<=20;r+=3){display.drawCircle(64,32,r,SSD1306_WHITE);display.display();delay(60);}
  delay(300);
  display.clearDisplay();display.setCursor(16,14);display.print("WiFi Connected!");
  display.setCursor(4,30);display.print(WiFi.localIP().toString());display.display();delay(1500);

  testTCP(); testHTTP();
  setupWebServer();
  playDroneStartup();

  Serial.println("\n======================================");
  Serial.println("  ESP32 AI Assistant");
  Serial.println("  Serial: type here + Enter");
  Serial.println("  Web UI: http://"+WiFi.localIP().toString()+"/");
  Serial.println("======================================");

  goToNextPrompt();
}


void loop() {


  webServer.handleClient();

  
  if (waitingForInput) {
    static unsigned long lastDot = 0;
    static int dotFrame = 0;
    if (millis()-lastDot > 500) {
      lastDot = millis();
      showReadyDots(dotFrame++);
    }

    String q = "";
    bool fromWeb = false;
    if (Serial.available()) {
      q = Serial.readStringUntil('\n'); q.trim();
    } else if (webQuestionReady) {
      q = webQuestion;
      webQuestionReady = false;
      fromWeb = true;
    }

    if (!q.length()) return;

    waitingForInput = false; scrollOffset = 0; dotFrame = 0;
    Serial.println("\n[You]: "+q);
    showUserBubble(q);
    playQuestionBeep();
    delay(400);

    startBlinking(); showThinkingScreen();

    if (WiFi.status()!=WL_CONNECTED) {
      stopBlinking(); aiConnected=false; showDisconnected();
      Serial.println("[ERROR] WiFi disconnected."); waitingForInput=true; return;
    }

    String ans = askOllama(q);
    stopBlinking(true);

    if (!ans.length()) {
      stopBlinking(false); aiConnected=false; showDisconnected();
      Serial.println("[ERROR] No response."); waitingForInput=true; return;
    }

    Serial.println("[AI]: "+ans+"\n");
    currentAIText = ans; scrollOffset = 0;
    cacheWrap(ans);
    playResponseBeep();

    // Make answer available to the /result polling endpoint
    if (fromWeb) webLastAnswer = ans;

    barPhase = 0;
    showAIWithBars(0);
    delay(200);
    speakText(ans);
    return;
  }

 
  if (audioPlaying && mp3) {
   
    const unsigned long AUDIO_TIMEOUT_MS = 30000UL;
    if (millis() - audioStartTime > AUDIO_TIMEOUT_MS) {
      Serial.println("[Audio] Watchdog timeout — force stopping.");
      stopAudioSources();
      waitingForBoot = true;
      showWaitBoot();
      return;
    }


    static uint32_t lastPos      = 0;
    static unsigned long lastMove = 0;
    const unsigned long STALL_MS = 2000UL;


    if (lastMove == 0) lastMove = audioStartTime;

    uint32_t curPos = bufSource ? (uint32_t)bufSource->getPos() : 0;
    if (curPos != lastPos) {
      lastPos  = curPos;
      lastMove = millis();
    } else if (millis() - lastMove > STALL_MS) {
      Serial.println("[Audio] Stream stalled — force stopping.");
      lastPos  = 0; lastMove = 0;
      stopAudioSources();
      waitingForBoot = true;
      showWaitBoot();
      return;
    }

 
    bool stillRunning = mp3->loop();
    if (!stillRunning) {
      Serial.println("[Audio] Finished.");
      lastPos  = 0; lastMove = 0;
      stopAudioSources();
      waitingForBoot = true;
      showWaitBoot();
    } else {
      barPhase = (barPhase + 1) % 9;
      showAIWithBars(scrollOffset);
    }
    return;
  }

  
  if (waitingForBoot) {
    static unsigned long lastScrollTick = 0;
    static unsigned long endReachedAt   = 0;
    const unsigned long SCROLL_INTERVAL_MS = 700UL; 
    const unsigned long END_PAUSE_MS       = 4000UL; 

    unsigned long now = millis();

    if (scrollOffset + 5 < _wrCount) {
      endReachedAt = 0; 
      if (now - lastScrollTick >= SCROLL_INTERVAL_MS) {
        lastScrollTick = now;
        scrollOffset++;
        showWaitBoot();
      }
    } else {
    
      if (endReachedAt == 0) endReachedAt = now;
      if (now - endReachedAt >= END_PAUSE_MS) {
        Serial.println("[Auto] End of text — going to next prompt.");
        endReachedAt   = 0;
        lastScrollTick = 0;
        goToNextPrompt();
      }
    }
    return;
  }


  if (aiConnected && WiFi.status()!=WL_CONNECTED) {
    aiConnected=false; stopBlinking(false);
    stopAudioSources(); showDisconnected();
    Serial.println("[WARN] WiFi lost. Reconnecting...");
    WiFi.reconnect(); delay(3000);
    if (WiFi.status()==WL_CONNECTED) {
      aiConnected=true; digitalWrite(BLUE_LED,HIGH);
      if (!waitingForBoot) { goToNextPrompt(); }
      Serial.println("[WiFi] Reconnected.");
    }
  }
}