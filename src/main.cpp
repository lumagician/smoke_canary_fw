/**
 * @file streams-i2s_pdm-serial.ino
 * @author Phil Schatzmann
 * @brief see https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-stream/streams-i2s_pdm-serial/README.md
 *
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

#include "AudioTools.h"
#include "AudioLibs/AudioKit.h"
#include "AudioLibs/AudioRealFFT.h" // or AudioKissFFT

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

// display fft result
void fftResult(AudioFFTBase &fft)
{
    float diff;
    auto result = fft.result();

    if ((result.frequency > 2900) && (result.frequency < 3100))
    {
        if(smokeFlag == false)
        {
            // first time canary died
            Serial.print(result.frequency);
            Serial.println(" => canary died!!");
            timestamp = micros();
        }else if((micros() - timestamp) > (unsigned long)(1E6 * 60 * 3)){
            // canary die already
            Serial.print(result.frequency);
            Serial.println(" => canary died!!");
            timestamp = micros();
        }

        smokeFlag =true;
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
    i2sStream.begin(cfg);

    // Setup FFT
    auto tcfg = fft.defaultConfig();
    tcfg.length = 8192;
    tcfg.channels = channels;
    tcfg.sample_rate = samples_per_second;
    tcfg.bits_per_sample = bits_per_sample;
    tcfg.callback = &fftResult;
    fft.begin(tcfg);

    timestamp = micros();
}

// Arduino loop - copy data
void loop()
{
    copier.copy();
}