/**
 * @file streams-i2s_pdm-serial.ino
 * @author Phil Schatzmann
 * @brief see https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-stream/streams-i2s_pdm-serial/README.md
 *
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

#include <canary.h>
#include <HTTPClient.h>
#include "AudioTools.h"
#include "AudioLibs/AudioKit.h"
#include "AudioLibs/AudioRealFFT.h" // or AudioKissFFT

String serverName = "https://us-central1-canary-ec729.cloudfunctions.net/sendCanaryAlert";
HTTPClient http;
String payload = "{\"canaryId\": \"00-00-00-00-00-00\"}";

AudioInfo info(16000, 1, 16);
I2SStream i2sStream;               // Access I2S as stream
AudioRealFFT fft;                  // or AudioKissFFT
StreamCopy copier(fft, i2sStream); // copy i2sStream to csvStream

int channels = 1;
int samples_per_second = 16000;
int bits_per_sample = 16;
float value = 0;

unsigned long timestamp;
bool smokeFlag = false;


void sendAlarm(void)
{
  http.begin(serverName.c_str());
  http.addHeader("Content-Type", "application/json"); 
  // Send the POST request
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);
  }
  else
  {
    Serial.print("Error sending POST request. HTTP Response code: ");
    Serial.println(httpResponseCode);
  }

  // Close the connection
  http.end();

  Serial.print("Sent");
}

// display fft result
void fftResult(AudioFFTBase &fft)
{
  float diff;
  auto result = fft.result();

  if ((result.frequency > 2900) && (result.frequency < 3100))
  {
    if (smokeFlag == false)
    {
      // first time canary died
      Serial.print(result.frequency);
      Serial.println(" => canary died!!");
      sendAlarm();
      timestamp = micros();
    }
    else if ((micros() - timestamp) > (unsigned long)(1E6 * 60 * 3))
    {
      // canary die already
      Serial.print(result.frequency);
      Serial.println(" => canary died!!");
      sendAlarm();
      timestamp = micros();
    }

    smokeFlag = true;
  }
}


// Arduino Setup
void setup(void)
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);

  auto cfg = i2sStream.defaultConfig(RX_MODE);
  cfg.copyFrom(info);
  cfg.signal_type = PDM;
  cfg.use_apll = false;
  cfg.auto_clear = false;
  cfg.pin_bck = I2S_PIN_NO_CHANGE; // not used
  cfg.pin_data = 41;
  cfg.pin_ws = 42;

  // Setup FFT
  auto tcfg = fft.defaultConfig();
  tcfg.length = 8192;
  tcfg.channels = channels;
  tcfg.sample_rate = samples_per_second;
  tcfg.bits_per_sample = bits_per_sample;
  tcfg.callback = &fftResult;

  canary_wifiConnect(); // Connect to WiFi
  i2sStream.begin(cfg);
  fft.begin(tcfg);
  timestamp = micros();
}

// Arduino loop - copy data
void loop()
{
  copier.copy();
}