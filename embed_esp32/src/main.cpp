#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <M5Stack.h>
#include <time.h>
#include "M5_ENV.h"

// offsets for the lcd screen
#define X_OFFSET 140
#define Y_OFFSET 10
#define X_LOCAL 40
#define Y_LOCAL 30
#define FRONT 2


const char* ssid = "2.4ghz_meo_wifi";
const char* password = "adminadmin";

const char* BASE_URL = "http://meo.local";

SHT3X sht30;

float temp = 0.0;
float humd = 0.0;

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

void TempHumRead(void) {
    if (sht30.get() == 0) {  // Obtain the data of shT30.  获取sht30的数据
        temp = sht30.cTemp;  // Store the temperature obtained from shT30.
                             // 将sht30获取到的温度存储
        humd = sht30.humidity;  // Store the humidity obtained from the SHT30.
                                // 将sht30获取到的湿度存储
    } else {
        temp = 0, humd = 0;
    }

    String temp_re = httpGet(String(BASE_URL) + "/get_temp/");
    String humid_re = httpGet(String(BASE_URL) + "/get_humid/");

    M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 8, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET * 8, FRONT);
    M5.Lcd.print(temp_re);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 8, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET * 8, FRONT);
    M5.Lcd.print(humid_re);

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET+13 * 8, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET+13 * 8, FRONT);
    M5.Lcd.print("Real T M P:");
    M5.Lcd.print(temp);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET+13 * 8, FRONT);
    M5.Lcd.print("                     ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET+13 * 8, FRONT);
    M5.Lcd.print("Real H M D:");
    M5.Lcd.print(humd);
}


void temp_humidity_data() {
    String temp = httpGet(String(BASE_URL) + "/get_temp/");
    String humid = httpGet(String(BASE_URL) + "/get_humid/");

       
    M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Lcd.setCursor(10, 10 + 10 * 8, 10);
    M5.Lcd.println(temp + "TEEEEMP");
    M5.Lcd.println(humid + "HMIIID");


    TempHumRead();
}


void setup() {
    M5.begin();
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        M5.Lcd.println(".");
    }
    M5.Lcd.clear();
    M5.Lcd.println("Connected to WiFi");
    M5.Lcd.println("Starting application");
    M5.Lcd.println("-----------------");
}

void loop() {
    int sensor_id = 1; // Replace with your actual sensor ID
    fetchTemperature(sensor_id);
    fetchHumidity(sensor_id);
    M5.update();
    delay(10000); // Delay for 10 seconds between updates
}

void fetchTemperature(int sensor_id) {
    if(WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(base_url) + "/get_temp_data/" + String(sensor_id);
        http.begin(url);
        int httpResponseCode = http.GET();
        
        if(httpResponseCode == 200) {
            String response = http.getString();
            M5.Lcd.println("Temperature Data: " + response);
            // Further processing can be done here
        } else {
            M5.Lcd.println("Error on HTTP request");
        }
        
        http.end();
    } else {
        M5.Lcd.println("WiFi Disconnected");
    }
}

void fetchHumidity(int sensor_id) {
    if(WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(base_url) + "/get_humid_data/" + String(sensor_id);
        http.begin(url);
        int httpResponseCode = http.GET();
        
        if(httpResponseCode == 200) {
            String response = http.getString();
            M5.Lcd.println("Humidity Data: " + response);
            // Further processing can be done here
        } else {
            M5.Lcd.println("Error on HTTP request");
        }
        
        http.end();
    } else {
        M5.Lcd.println("WiFi Disconnected");
    }
}






