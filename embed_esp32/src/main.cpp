#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <M5Stack.h>
#include <time.h>

const char* ssid = "2.4ghz_meo_wifi";
const char* password = "adminadmin";

const char* BASE_URL = "http://meo.local";


class Sensor {
public:
    String name;
    String building_id;
    String facility_id;
    String sensor_id;

    Sensor() {
        name = httpGet(String(BASE_URL) + "/get_target_name/");
        building_id = httpGet(String(BASE_URL) + "/get_target_building/");
        facility_id = httpGet(String(BASE_URL) + "/get_target_facility/");
        
        String url = String(BASE_URL) + "/sensor/";
        String payload = "{\"name\":\"" + name + "\", \"building_id\":\"" + building_id + "\"}";
        
        String response = httpPost(url, payload);
        if (response.indexOf("id") != -1) {
            sensor_id = parseJson(response, "id");
        } else {
            sensor_id = "";
        }
    }

String getCurrentDatetime() {
    // Configure NTP
    configTime(0, 0, "pool.ntp.org");  // UTC

    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retries = 10;
    while(timeinfo.tm_year < (2016 - 1900) && retries--) {
        delay(1000);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    char datetime[64];
    strftime(datetime, sizeof(datetime), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);

    return String(datetime);
}

private:

    String httpGet(const String& url) {
        HTTPClient http;
        http.begin(url);
        int httpCode = http.GET();
        if (httpCode > 0) {
            return http.getString();
        } else {
            return "";
        }
        http.end();
    }

    String httpPost(const String& url, const String& payload) {
        HTTPClient http;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        int httpCode = http.POST(payload);
        if (httpCode > 0) {
            return http.getString();
        } else {
            return "";
        }
        http.end();
    }

    String parseJson(const String& jsonData, const String& key) {
        // Simplified JSON parsing, consider using a library for complex cases
        String searchStr = "\"" + key + "\":\"";
        int startPos = jsonData.indexOf(searchStr) + searchStr.length();
        int endPos = jsonData.indexOf("\"", startPos);
        return jsonData.substring(startPos, endPos);
    }
};

enum SensorType {
    TEMPERATURE,
    HUMIDITY
};

struct SensorValue {
    SensorType type;
    float max_value;
    float low_value;
    float value;
    String datetime;
    String sensor_id;
};

Sensor global_sensor;

void setup() {
    M5.begin();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
    M5.Lcd.clear();
    M5.Lcd.println("Starting application");
}

void loop() {
    post_temp_humidity_data();
    M5.update();
    delay(1000);
}

void post_temp_humidity_data() {
    // Placeholder for temperature and humidity readings
    float temperature = 25.0; 
    float humidity = 60.0;

    M5.Lcd.printf("Temp: %f\nHum: %f", temperature, humidity);
    
    // Create SensorValue instances for temperature and humidity
    SensorValue tempValue;
    tempValue.type = TEMPERATURE;
    tempValue.value = temperature;
    tempValue.max_value = 30.0; // Placeholder max value
    tempValue.low_value = 20.0; // Placeholder low value
    tempValue.datetime = getCurrentDatetime(); // Placeholder datetime, consider using a real-time clock or NTP
    tempValue.sensor_id = global_sensor.sensor_id;

    SensorValue humValue;
    humValue.type = HUMIDITY;
    humValue.value = humidity;
    humValue.max_value = 100.0; // Placeholder max value
    humValue.low_value = 0.0;   // Placeholder low value
    humValue.datetime = getCurrentDatetime();  // Placeholder datetime
    humValue.sensor_id = global_sensor.sensor_id;

    // Convert SensorValue to JSON and send using HTTP POST
    sendSensorData(tempValue);
    sendSensorData(humValue);
}

void sendSensorData(const SensorValue& data) {
    String typeStr = (data.type == TEMPERATURE) ? "TEMPERATURE" : "HUMIDITY";
    String payload = "{\"type\":\"" + typeStr + "\",";
    payload += "\"value\":" + String(data.value) + ",";
    payload += "\"max_value\":" + String(data.max_value) + ",";
    payload += "\"low_value\":" + String(data.low_value) + ",";
    payload += "\"datetime\":\"" + data.datetime + "\",";
    payload += "\"sensor_id\":\"" + data.sensor_id + "\"}";

    HTTPClient http;
    http.begin(String(BASE_URL) + "/send_sensor_data/"); // Assuming this endpoint
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(payload);
    if (httpCode > 0) {
        String response = http.getString();
        // Optionally handle the response, e.g., check for success
    }
    http.end();
}