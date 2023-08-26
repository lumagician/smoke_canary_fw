#include "canary.h"
#include <SD.h>
#include <WiFi.h>
#include <SPI.h>

const int SDchipSelect = 21; // Set the chip select pin for your SD module

// Function to retrieve WiFi configuration from the SD card
bool canary_getWifiConfig(WiFiConfig *conf);

void canary_wifiConnect(void)
{
    WiFiConfig config;

#ifdef LOAD_CONFIG
    // Get WiFi configuration from SD card
    if (!canary_getWifiConfig(&config))
    {
        Serial.println("Error while getting Wifi configuration");
        // Handle the error condition here
    }
#else
    Serial.println("Using hardcoded Credentials");
    strncpy(config.ssid, "TP-Link_246C", sizeof(config.ssid));
    strncpy(config.password, "49738766", sizeof(config.password));
#endif

    // Connect to WiFi using retrieved configuration
    Serial.println("Connecting to WiFi...");
    WiFi.begin(config.ssid, config.password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

// Function to read WiFi configuration from SD card and populate WiFiConfig struct
bool canary_getWifiConfig(WiFiConfig *conf)
{
    Serial.println("Loading Config from SD");

    // Initialize SD card
    if (!SD.begin(SDchipSelect))
    {
        Serial.println("SD card initialization failed.");
        return false;
    }
    Serial.println("SD card initialized.");

    // Read configuration file
    File configFile = SD.open("/config.txt");

    if (configFile)
    {
        String line = configFile.readStringUntil('\n');
        int delimiterIndex = line.indexOf(';');

        if (delimiterIndex != -1)
        {
            // Extract SSID and password from the read line
            String ssid = line.substring(0, delimiterIndex);
            String password = line.substring(delimiterIndex + 1);

            // Convert and store SSID and password in WiFiConfig struct
            ssid.toCharArray(conf->ssid, sizeof(conf->ssid));
            password.toCharArray(conf->password, sizeof(conf->password));
            configFile.close();
            return true; // Configuration read successfully
        }
        else
        {
            Serial.println("Invalid input format.");
            configFile.close();
            return false; // Invalid format
        }
    }
    else
    {
        Serial.println("Error opening config file.");
        return false; // Error opening file
    }
}