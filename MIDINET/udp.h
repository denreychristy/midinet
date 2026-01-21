#ifndef UDP_H
#define UDP_H

#include <ArduinoJson.h>
#include <WiFiUdp.h>

#define MAX_BUFFER_SIZE 1023

class UDPManager {
  private:
    WiFiUDP _udp;
    int _port = 5150;
    char _incomingPacket[MAX_BUFFER_SIZE];
    bool _newPacketAvailable = false;

  public:
    int channel;
    const char* type;
    int value;

    UDPManager() {}

    void begin() {
      _udp.begin(_port);
      Serial.print("UDP Listening on port: ");
      Serial.println(_port);
    }

    void update() {
      int packetSize = _udp.parsePacket();
      if (packetSize) {
        int len = _udp.read(_incomingPacket, MAX_BUFFER_SIZE - 1);
        if (len > 0) {
          _incomingPacket[len] = 0;
          StaticJsonDocument<200> doc;
          DeserializationError error = deserializeJson(doc, _incomingPacket);

          if (!error) {
            channel = doc["ch"];
            type = doc["type"];
            value = doc["val"];

            Serial.printf("MIDI Recv -> Ch: %d, Type: %s, Val: %d\n", channel, type, value);
          } else {
            Serial.print("JSON Parse Failed: ");
            Serial.println(error.f_str());
          }
        }
      }
    }

    bool hasNewCommand() {
      if (_newPacketAvailable) {
        _newPacketAvailable = false;
        return true;
      }
      return false;
    }
};

#endif