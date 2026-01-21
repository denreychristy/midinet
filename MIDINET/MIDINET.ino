// MIDINET

// ============================================================================================== //
// Imports

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <SPI.h>
#include <Wire.h>
#include <vector>

#include "display.h"
#include "math.h"
#include "rotary_encoder.h"
#include "udp.h"
#include "wifi.h"

// ============================================================================================== //
// Constants

// Display
extern Adafruit_SSD1306 display;

// Encoder
Encoder encoder(D5, D6, D3);

// WiFi Manager
WiFiManager wifi = WiFiManager();

// UDP Manager
UDPManager udp = UDPManager();

// ============================================================================================== //
// Setup

void setup() {
  Serial.begin(115200);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;);
  }
  display.setTextWrap(false);

  wifi.begin();
  unsigned long startAttempt = millis();
  while (!wifi.isConnected() && millis() - startAttempt < 8000) {
    display.clearDisplay();
    headerText("MIDINET");
    writeToLine(2, "Connecting to WiFi...");
    display.display();
    delay(100);
    yield();
  }
  if (wifi.isConnected()) {
    udp.begin();
  }

  encoder.begin();

  mainScreen();
}

// ============================================================================================== //
// Loop

void loop() {}

// ============================================================================================== //
// Main Screen

void mainScreen() {
  bool loop_flag = true;
  String midi_message;

  while (loop_flag) {
    // ================================================== //
    // Update
    encoder.update();
    udp.update();

    // ================================================== //
    // Display
    display.clearDisplay();
    if (wifi.isConnected()) {
      String name = wifi.getNetworkName();
      String strength = wifi.getSignalStrength();
      String address = wifi.getIP();
      headerText("MIDINET | " + name + " | " + strength + " | " + address);
    }
    else {

      headerText("MIDINET | WiFi not connected. Press encoder button for menu.");
    }
    writeToLine(1, "Channel: " + udp.channel);
    writeToLine(2, String("Type: ") + udp.type);
    writeToLine(3, "Value: " + udp.value);
    display.display();

    // ================================================== //
    // User Input
    if (encoder.wasQuickPressed) {
      menuScreen();
    }
  }
}

// ============================================================================================== //
// Menu Screen

void menuScreen() {
  std::vector<String> mainOptions = {
    "Connect to WiFi",
    "MIDI Settings",
    "System Info",
    "[ Exit Menu ]"
  };
  int totalOptions = mainOptions.size();
  VerticalListMenu mainMenu("Main Menu", mainOptions);
  
  bool loop_flag = true;
  while (loop_flag) {
    yield(); 

    // ================================================== //
    // Update
    encoder.update();
    int currentSelection = abs(encoder.value) % totalOptions;
    udp.update();

    // ================================================== //
    // Display
    display.clearDisplay();
    mainMenu.display(currentSelection);
    display.display();

    // ================================================== //
    // User Input
    if (encoder.wasQuickPressed) {
      switch (currentSelection) {
        case 0: // Connect to WiFi
          wifi.scan(display);
          selectWiFiNetworkScreen();
          break;

        case 1: // Placeholder 1
          Serial.println("MIDI Settings Selected");
          break;

        case 2: // Placeholder 2
          Serial.println("System Info Selected");
          break;

        case 3: // [ Exit Menu ]
          loop_flag = false;
          break;
      }
    }
    
    delay(10);
  }
}

// ============================================================================================== //
// Connecting to WiFi

void selectWiFiNetworkScreen() {
  bool loop_flag = true;
  encoder.reset();

  while (loop_flag) {
    int wifiCount = wifi.getCount();

    // If no WiFi networks are found
    if (wifiCount <= 0) {
      // Update
      encoder.update();

      // Display
      display.clearDisplay();
      headerText("No WiFi Networks Found!");
      writeToLine(2, "> Click to Retry");
      display.display();

      // User Input
      if (encoder.wasQuickPressed) {
        wifi.scan(display);
      }
      yield();
      continue;
    }

    std::vector<String> menuOptions = wifi.getNetworkList();
    menuOptions.push_back("[ Rescan ]");
    int totalOptions = menuOptions.size();

    VerticalListMenu menu = VerticalListMenu("Select WiFi", menuOptions);

    bool menuActive = true;
    while (menuActive) {
      yield();

      // Update
      encoder.update();
      int currentSelection = abs(encoder.value) % totalOptions;

      // Display
      display.clearDisplay();
      menu.display(currentSelection);
      display.display();

      // User Input
      if (encoder.wasQuickPressed) {
        encoder.wasQuickPressed = false;

        // If the user selects "[ Rescan ]"
        if (currentSelection == totalOptions - 1) {
          wifi.scan(display); // Trigger a new scan
          menuActive = false; // Break this inner loop to rebuild the menu
        }

        // If the user selects a network name
        else {
          String selectedSSID = menuOptions[currentSelection];
          enterWiFiPassword(selectedSSID);
          loop_flag = false;
          menuActive = false;
        }
      }
      delay(10);
    }
  }
}

void enterWiFiPassword(String ssid) {
  TextEntry passwordEntry("Enter the password for: " + ssid);
  bool entering = true;

  while (entering) {
    yield();

    // Update
    encoder.update();
    passwordEntry.update(encoder);

    // Display
    display.clearDisplay();
    passwordEntry.display();
    display.display();

    // User Input
    if (encoder.wasDoublePressed) { 
      
      String finalPassword = passwordEntry.getUserText();
      
      // Attempt to connect to the WiFi network
      bool success = wifi.connect(ssid, finalPassword, display);
      
      if (success) {
        entering = false; // Connection successful, exit to main app
        udp.begin();
      }
      else {
        // If it fails, the connect method already showed "Failed"
        // We stay in the loop so the user can fix the password
      }
    }
    delay(10);
  }
}