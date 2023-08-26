#include <I2S.h>
#include <canary.h>

const int chipSelect = 21; // Set the chip select pin for your SD module

void setup()
{
  delay(1000); // Delay to ensure stable startup

  Serial.begin(115200); // Start serial communication
  while (!Serial); // Wait for the serial port to open

  canary_wifiConnect(); // Connect to WiFi using the configuration from your custom library

  // Start I2S at 16 kHz with 16-bits per sample
  I2S.setAllPins(-1, 42, 41, -1, -1);
  if (!I2S.begin(PDM_MONO_MODE, 16000, 16))
  {
    Serial.println("Failed to initialize I2S!");
    while (1)
      ; // Stay in an infinite loop if I2S initialization fails
  }
}

void loop()
{
  // Read a sample from I2S
  int sample = I2S.read();

  if (sample && sample != -1 && sample != 1)
  {
    Serial.print(">Audio:");
    Serial.println(sample); // Print the sampled audio data
  }
  delayMicroseconds(100); // Delay to control the loop frequency
}
