#include "wifi.h"
#include "bme280_wifi.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Creating a BME280 sensor object
Adafruit_BME280 bme;

String readString;

const char* host = "script.google.com";
const int httpsPort = 443;

// Define client for wifi conenction
WiFiClientSecure client;

// Define NTP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";

ENodeMcuStatus m_nodeMcuStatus;

void setup()
{
  m_nodeMcuStatus = INIT;
  
  #if (SERIAL_MONITOR == FUNCTION_ON)
  Serial.begin(9600);
  #endif // SERIAL_MONITOR
  
  connect_to_wifi();

  timeClient.begin();

  if (!bme.begin(0x76))
  {
    m_nodeMcuStatus = I2C_CONNECTION_PROBLEM;
  }
}

void loop()
{
  // Store the latest value of millis()
  currentMillis = millis();

  time_client_update();

  if ((m_hour > 8) && (m_hour < 16))
  {
    m_nodeMcuStatus = MEASUREMENT_IS_ONGOING;

    get_bme_sensor_data();
    sendData(m_temperature, m_nodeMcuStatus);
  }

  if (currentMillis >= 4294967295)
  {
    ESP.restart();
  }

  #if (SERIAL_MONITOR == FUNCTION_ON)
  status_to_serial_port(m_nodeMcuStatus);
  #endif // SERIAL_MONITOR
}

void connect_to_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    m_nodeMcuStatus = NOT_CONNECTED_TO_WIFI;
    delay(500);
  }
}

void sendData(float x, ENodeMcuStatus y)
{
  // Wait for delayTime to expire
  if ((currentMillis - previousSendMillis) >= delayTime)
  {
    client.setInsecure();

    client.connect(host, httpsPort);

    String string_x     =  String(x, 2);
    String string_y     =  String(y);
    String url = "/macros/s/" + GAS_ID + "/exec?value1=" + string_x + "&value2=" + string_y;

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
          "Host: " + host + "\r\n" +
          "User-Agent: BuildFailureDetectorESP8266\r\n" +
          "Connection: close\r\n\r\n");

    // Save the time
    previousSendMillis += delayTime;
  }
}

void get_bme_sensor_data()
{
  // Wait for delayTime to expire
  if ((currentMillis - previousBme280Millis) >= delayTime)
  {
    // Read temperature [C]
    m_temperature = bme.readTemperature();

    // Read pressue [hPa]
    //m_pressure = (bme.readPressure() / 100.0);

    // Read humidity [%]
    //m_humidity = bme.readHumidity();

    // Save the time
    previousBme280Millis += delayTime;
  }
}

void time_client_update()
{
  // Wait for delayTime to expire
  if ((currentMillis - previousNtpClientMillis) >= delayTime)
  {
    timeClient.update();

    m_hour = timeClient.getHours();

    // Save the time
    previousNtpClientMillis += delayTime;
  }
}

#if (SERIAL_MONITOR == FUNCTION_ON)
void status_to_serial_port(ENodeMcuStatus m_status)
{
  switch(m_status)
  {
    case I2C_CONNECTION_PROBLEM:
      Serial.print("Couldn't connect to sensor\n");
      break;

    case NOT_CONNECTED_TO_WIFI:
      Serial.print("Not able to connect\n");
      break;

    case MEASUREMENT_IS_ONGOING:
      Serial.print("Measurement is ongoing\n");
      Serial.print(m_temperature);
      Serial.print("\n");
      break;

    default:
      Serial.print("Default\n");
  }
}
#endif // SERIAL_MONITOR
