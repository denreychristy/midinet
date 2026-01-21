// MIDINET - WiFi

// ============================================================================================== //

#ifndef WIFI_H
#define WIFI_H

// ============================================================================================== //
// Include

#include <ESP8266WiFi.h>

#include "display.h"

// ============================================================================================== //
// Constants

// ============================================================================================== //
// WiFi Manager Class

class WiFiManager {
  // ================================================== //
  private:
    int _networksFound = 0;
    std::vector<String> _networkSSIDs;

  // ================================================== //
  public:
    // ================================================== //
    WiFiManager() {

    }

    // ================================================== //
    void begin() {
      WiFi.persistent(true);
      WiFi.mode(WIFI_STA);
      WiFi.setAutoReconnect(true);
      WiFi.setAutoConnect(true);
      WiFi.begin();
    }

    // ================================================== //
    bool isConnected() {
      return (WiFi.status() == WL_CONNECTED);
    }

    // ================================================== //
    String getNetworkName() { return WiFi.SSID(); }

    // ================================================== //
    String getIP() { return WiFi.localIP().toString(); }

    // ================================================== //
    String getSignalStrength() {
      if (!isConnected()) return "0%";

      int rssi = WiFi.RSSI();
      if (rssi <= -100) return "0%";
      if (rssi >= -50)  return "100%";
      return String(2 * (rssi + 100)) + "%";
    }

    // ================================================== //
    void scan(Adafruit_SSD1306 &display) {
      // Display
      display.clearDisplay();
      headerText("WiFi Scan");
      writeToLine(2, "Scanning...");
      display.display();

      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);

      _networkSSIDs.clear();
      _networksFound = 0;

      int n = WiFi.scanNetworks(false, false);

      if (n > 0) {
        // Limit to 20 networks to prevent memory bloat
        int limit = (n > 20) ? 20 : n; 
        for (int i = 0; i < limit; i++) {
          _networkSSIDs.push_back(WiFi.SSID(i));
        }
        _networksFound = limit;
      }

      WiFi.scanDelete();
    }

    // ================================================== //
    bool connect(String ssid, String password, Adafruit_SSD1306 &display) {
      display.clearDisplay();
      headerText("Connecting...");
      display.display();

      // Convert String to C-style strings (char*) which the library expects
      WiFi.begin(ssid.c_str(), password.c_str());

      // Set a timeout (e.g., 10 seconds) so it doesn't loop forever
      unsigned long startAttemptTime = millis();
      const unsigned long timeout = 10000; 

      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
        display.clearDisplay();
        headerText("Connecting");
        
        // Simple animation
        writeToLine(2, ssid);
        String dots = "";
        for(int i=0; i < (millis()/500)%4; i++) dots += ".";
        writeToLine(3, dots);
        
        display.display();
        yield(); // Keep the ESP8266 background tasks alive
      }

      if (WiFi.status() == WL_CONNECTED) {
        display.clearDisplay();
        headerText("Connected!");
        writeToLine(2, "IP: " + WiFi.localIP().toString());
        display.display();
        delay(2000);
        return true;
      } else {
        display.clearDisplay();
        headerText("Failed");
        writeToLine(2, "Check Password");
        display.display();
        delay(2000);
        return false;
      }
    }

    // ================================================== //
    int getCount() { return _networksFound; }
    
    // ================================================== //
    std::vector<String> getNetworkList() { return _networkSSIDs; }
};

// ============================================================================================== //

#endif