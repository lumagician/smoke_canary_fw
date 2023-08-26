struct WiFiConfig
{
    char ssid[33];     // Max length of SSID
    char password[64]; // Max length of password
};

bool canary_getWifiConfig(WiFiConfig *conf);
void canary_wifiConnect(void);