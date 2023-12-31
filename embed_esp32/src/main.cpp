#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <M5Stack.h>
#include <time.h>
#include "M5_ENV.h"
#include <Adafruit_NeoPixel.h>

#define LED_PIN 26  // The pin where the LED data line is connected
#define NUM_LEDS 1  // The number of LEDs you have

Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRBW + NEO_KHZ800);

const char* ssid = "2.4ghz_meo_wifi";
const char* password = "adminadmin";

const char* BASE_URL = "http://meo.local/api";

uint32_t greenColor = pixels.Color(0, 255, 0, 0); // RGBW: full green, no white
uint32_t yellow = pixels.Color(255,255,0, 0);   // RGBW: all off
uint32_t redColor = pixels.Color(255, 0, 0, 0);   // RGBW: full red, no white


SHT3X sht30;

void setLedColor(uint32_t color) {
    pixels.setPixelColor(0, color); // Set the color for the first pixel.
    pixels.show(); // This sends the updated color value to the LEDs.
}

String httpGet(const String& url) {
    /*
        @param: string url # This is the full url endpoint    
        @returns: http resualt
    */

    HTTPClient http;
    http.begin(url);
    // The http code is the status code returned
    int httpCode = http.GET();
    if (httpCode > 0) {
        return http.getString();
    }

    http.end();
}

void checkWorkingHours() {
    /*
        @params: void
        @returns: void
        @details: The mothod calls the api endpoint using the method httpGet
        It also sets the otputs the resualt to the screen
    
    */
    String url = String(BASE_URL) + "/control_lights";
    String response = httpGet(url);

    M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Lcd.setCursor(5, 1 + 2 * 8, 2);
    
    if (response == "true") {
        M5.Lcd.println("Greenhouse open");
        setLedColor(greenColor); // Green for open
    } else if (response == "false") {
        M5.Lcd.println("Greenhouse closed");
        setLedColor(yellow); // Off for closed
    } else {
        // Print the actual response from the API
        M5.Lcd.print("Response: ");
        M5.Lcd.println(response);
        setLedColor(redColor); // Red for unknown status
    }
}


void setup() {
    /*
        @params: void
        @returns: void
        @details: This sets up the program 
    */
    // This powers up the screen
    M5.begin(); 
    // Sets the text size
    M5.Lcd.setTextSize(2);
    // Configures pin for output. This defaults to some uknown inside the libbary 
    pixels.begin();
    // Loads the wifi config 
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        M5.Lcd.println(".");
        M5.Lcd.println(".");
    }
    M5.Lcd.clear();
    M5.Lcd.println("Connected to WiFi");
    M5.Lcd.println();
    M5.Lcd.println("-----------------");
    delay(1000);

}

void loop() {
    M5.Lcd.clear();
    checkWorkingHours();
    delay(1000);
}
