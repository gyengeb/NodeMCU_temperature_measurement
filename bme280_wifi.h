#ifndef BME280_WIFI_H
#define BME280_WIFI_H

#define FUNCTION_OFF 0x00
#define FUNCTION_ON  0x01

// Compiler switch
#define SERIAL_MONITOR FUNCTION_OFF

#define SEALEVELPRESSURE_HPA (1013.25)

// LED pin on NodeMCU
const uint8_t boardLedPin = 2;

// Offset to the timezone in ms
const long utcOffsetInSeconds = 3600;

// Delay in ms
const unsigned long delayTime = 6000;

// Stores the value of millis()
unsigned long currentMillis = 0;

// Store the last time value of BME280 sensor update
unsigned long previousBme280Millis = 0;

// Store the last time value of sending data
unsigned long previousSendMillis = 0;

// Store the last time value of ntp client update
unsigned long previousNtpClientMillis = 0;

// Store the state of the LED
byte ledState = LOW;

// BME280 sensor data
float m_temperature = 0;

// NTP client data
uint8_t m_hour = 0;

typedef enum ENodeMcuStatus
{
    INIT                    = 0,
    I2C_CONNECTION_PROBLEM  = 1,
    NOT_CONNECTED_TO_WIFI   = 2,
    MEASUREMENT_IS_ONGOING  = 3
};

#endif // BME280_WIFI_H
