#include <I2S.h>
#include <canary.h>

const int chipSelect = 21;                          // Set the chip select pin for your SD module
const int sampleRate = 16000;                       // Sample rate in Hz
const float sampleDuration = 0.5;                   // Sample duration in seconds
const int numSamples = sampleRate * sampleDuration; // Number of samples in the array
int16_t samples[numSamples];                        // Array to store the samples

void setup()
{
  delay(1000); // Delay to ensure stable startup
  Serial.println("Device Up");

  Serial.begin(115200); // Start serial communication
  while (!Serial)
    ; // Wait for the serial port to open

  canary_wifiConnect(); // Connect to WiFi

  // Start I2S at 16 kHz with 16-bits per sample
  I2S.setAllPins(-1, 42, 41, -1, -1);
  if (!I2S.begin(PDM_MONO_MODE, sampleRate, 16))
  {
    Serial.println("Failed to initialize I2S!");
    while (1)
      ; // Stay in an infinite loop if I2S initialization fails
  }
}

void loop()
{
  // uint64_t start = esp_timer_get_time();
  for (int i = 0; i < numSamples; i++)
  {
    samples[i] = I2S.read();                 // Read a sample from I2S and store it in the array
  }

  // uint64_t end = esp_timer_get_time();

  // printf("took %llu milliseconds\n", (end - start) / 1000);

  for (int i = 0; i < numSamples; i++)
  {
    Serial.print(">Audio:");
    Serial.println(samples[i]);
  }

  delay(1000); // Delay before taking the next sample

  // // Read a sample from I2S
  // int sample = I2S.read();

  // if (sample && sample != -1 && sample != 1)
  // {
  //   Serial.print(">Audio:");
  //   Serial.println(sample); // Print the sampled audio data
  // }
  // delayMicroseconds(100); // Delay to control the loop frequency
}