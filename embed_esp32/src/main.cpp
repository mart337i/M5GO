#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <M5Stack.h>
#include <WiFi.h>

const char* base_url = "http://meo.local";

class Sensor {
public:
  String name;
  String building_id;
  String facility_id;
  int sensor_id;

  Sensor(const char* name = "", int building_id = 1, int facility_id = 1, int sensor_id = -1) {
    HTTPClient http;

    http.begin(String(base_url) + "/get_target_name/");
    int httpCode = http.GET();
    if(httpCode == 200) {
      this->name = http.getString(); // Use this-> to refer to the class member, not the parameter
    }
    http.end();

  // For building_id
    http.begin(String(base_url) + "/get_target_building/");
    int httpCode = http.GET();
    if(httpCode == 200) {
      this->building_id = http.getString(); // Use this-> to refer to the class member
    }
    http.end();

    // For facility_id
    http.begin(String(base_url) + "/get_target_facility/");
    httpCode = http.GET();
    if(httpCode == 200) {
      this->facility_id = http.getString(); // Use this-> to refer to the class member
    }
    http.end();
  }

    StaticJsonDocument<200> doc;
    doc["name"] = name;
    doc["building_id"] = building_id;

    String sensor_data;
    serializeJson(doc, sensor_data);
    
    http.begin(String(base_url) + "/sensor/");
    http.addHeader("Content-Type", "application/json");
    httpCode = http.POST(sensor_data);
    if(httpCode == 200) {
      String payload = http.getString();
      deserializeJson(doc, payload);
      sensor_id = doc["id"];
    } else {
      sensor_id = -1;
    }
    http.end();
  }
};

class SensorValue {
public:
  int id;
  String type;
  float max_value;
  float low_value;
  float value;
  String datetime;
  int sensor_id;
};

Sensor global_sensor;

void send_alarm(String message) {
  HTTPClient http;
  StaticJsonDocument<200> doc;
  doc["type"] = "failure";
  doc["message"] = message;
  doc["sensor_id"] = global_sensor.sensor_id;
  
  String data;
  serializeJson(doc, data);
  
  http.begin(String(base_url) + "/alarm/");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(data);
  if(httpCode != 200) {
    M5.Lcd.printf("Alarm sent: %d\n", httpCode);
  } else {
    M5.Lcd.printf("Alarm sent: %d\n", httpCode);
  }
  http.end();
}

void setup() {
  // Initialize the M5Stack object
  M5.begin();

  // Connect to WiFi
  // WiFi.begin(ssid, password);

  // Wait for connection
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }

  // Initialize global_sensor here
  // global_sensor = ...

  // Clear the screen and set up the initial display
  M5.Lcd.clear();
  M5.Lcd.println("Starting application");
}

void loop() {
  // Main application loop
  if (M5.BtnA.wasPressed()) {
    // Call function to post temperature and humidity data
    // post_temp_humidity_data();
  }

  // Update screen with sensor data
  // ...

  // Delay between readings
  delay(1000);
}

int main() {
  setup();
  while (true) {
    loop();
  }
  return 0;
}