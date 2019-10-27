
// Import required libraries
#include <ArducamSSD1306.h>    // Modification of Adafruit_SSD1306 for ESP8266 compatibility
#include <Adafruit_GFX.h>   // Needs a little change in original Adafruit library (See README.txt file)
#include <Wire.h>           // For I2C comm, but needed for not getting compile error

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Pin definitions
#define OLED_RESET  16  // Pin 15 -RESET digital signal

#define MAX_LINE 256 

BluetoothSerial SerialBT;

ArducamSSD1306 display(OLED_RESET); // FOR I2C
bool has_data = false;
bool dirty = false;
static char buf[MAX_LINE]; // an array to store the received data

void setup(void)
{
    // set up text buffer
    clearBuffer();

    // Start Serial
    Serial.begin(115200);
    SerialBT.begin("ESP32test"); //Bluetooth device name
    Serial.println("Try connecting to ESP32test!");

    // SSD1306 Init
    display.begin();  // Switch OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setTextWrap(true);
    display.stopscroll();
    printHeader(/*connected =*/true);
}

void loop() {
    if (dirty) {
        printHeader(SerialBT.connected());
    }
    recvWithEndMarker();
    showNewData();
}

void recvWithEndMarker() {
    byte ndx = 0;
    
    while (SerialBT.available() > 0 && !has_data) {
        char rc = SerialBT.read();
       
        if (rc != '\n') {
            buf[ndx++] = rc;
            if (ndx >= MAX_LINE) {
                ndx = MAX_LINE - 1;
            }
        } else {
            buf[ndx] = '\0'; // terminate the string
            has_data = true;
        }
    }
}

void showNewData() {
    if (has_data) {
        displayText();
    }
}

void printHeader(bool connected) {
    display.clearDisplay();
    if (connected) {
        display.setCursor(5,2);
        display.println("Connecting...");
    } else {
       display.setCursor(5,2); // 16 px tall, 
       display.println("Translating...");
    }
    display.startscrollright(0, 1);
    dirty = false;

    display.display();

}
void clearBuffer() {
    memset(buf, 0, sizeof(buf));
}

void displayText() {
    display.clearDisplay();
    printHeader(false);
    
    // display input each word by word for a cool effect
    display.setCursor(5, 20);
    int idx = 0;
    char ch = buf[idx];
    while (ch) {
        display.print(ch);
        if (idx % 10 == 0) { // only re-print every 10 chars, so its faster
            display.display();
        }
        ch = buf[++idx];
    }
    display.display();
    clearBuffer();
    has_data = false;
    dirty = false;
}

