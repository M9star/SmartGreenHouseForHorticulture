
#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <ArduinoJson.h>

// Pin definitions
#define DHTPIN 2
#define DHTTYPE DHT11
#define GREEN_LED_dht11 3
#define RED_LED_dht11 4
#define LDR_PIN A0
#define LDr_LED_PIN 5
#define soilMoisturePin A1
#define ledPin_g_soilM 6
#define ledPin_r_soilM 7
#define waterSupply 8
#define alkaline 9
#define acidic 10

// Thresholds
#define MIN_TEMP 15
#define MAX_TEMP 35
#define MIN_HUMIDITY 30
#define MAX_HUMIDITY 80
int dark_threshold = 800;
int light_threshold = 200;
float min_pH = 5.5;
float max_pH = 7.5;
int minThresholdSoilM = 30;
int maxThresholdSoilM = 70;

// Objects
DHT dht(DHTPIN, DHTTYPE);
Servo myservo;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
StaticJsonDocument<512> doc;

// Variables
float temperature, humidity, pH, altitude;
int soilMoistureValue, ldrValue;

void setup() {
  Serial.begin(9600);
  dht.begin();
  myservo.attach(11); // Servo motor pin
  if(!bmp.begin()){
    Serial.println("BMP180 not detected!");
    while(1);
  }

  pinMode(GREEN_LED_dht11, OUTPUT);
  pinMode(RED_LED_dht11, OUTPUT);
  pinMode(LDr_LED_PIN, OUTPUT);
  pinMode(ledPin_g_soilM, OUTPUT);
  pinMode(ledPin_r_soilM, OUTPUT);
  pinMode(waterSupply, OUTPUT);
  pinMode(alkaline, OUTPUT);
  pinMode(acidic, OUTPUT);
}

void loop() {
  // Temperature and Humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT sensor!");
    return;
  }

  if ((temperature > MIN_TEMP && temperature < MAX_TEMP) && 
      (humidity > MIN_HUMIDITY && humidity < MAX_HUMIDITY)) {
    digitalWrite(GREEN_LED_dht11, HIGH);
    digitalWrite(RED_LED_dht11, LOW);
  } else {
    digitalWrite(GREEN_LED_dht11, LOW);
    digitalWrite(RED_LED_dht11, HIGH);
    // Fogger operation (servo motor)
    for(int i=0;i<5;i++){
      for (int pos = 0; pos <= 180; pos++) {
        myservo.write(pos);
        delay(5);
      }
      for (int pos = 180; pos >= 0; pos--) {
        myservo.write(pos);
        delay(5);
      }
    }
  }

  // Pressure and Altitude
  sensors_event_t event;
  bmp.getEvent(&event);
  if (event.pressure) {
    altitude = bmp.pressureToAltitude(1013.25, event.pressure, temperature);
    Serial.print("Altitude: ");
    Serial.println(altitude);
  }

  // LDR sensor
  ldrValue = analogRead(LDR_PIN);
  if (ldrValue > dark_threshold) {
    digitalWrite(LDr_LED_PIN, HIGH);
  } else {
    digitalWrite(LDr_LED_PIN, LOW);
  }
  Serial.print("Light: ");
  Serial.println(ldrValue);

  // Soil moisture
  soilMoistureValue = analogRead(soilMoisturePin);
  int soilMoisturePercentage = map(soilMoistureValue, 0, 1023, 0, 100);

  if (soilMoisturePercentage < minThresholdSoilM) {
    digitalWrite(ledPin_g_soilM, LOW);
    digitalWrite(ledPin_r_soilM, HIGH);
    digitalWrite(waterSupply, HIGH);
  } else {
    digitalWrite(ledPin_g_soilM, HIGH);
    digitalWrite(ledPin_r_soilM, LOW);
    digitalWrite(waterSupply, LOW);
  }
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisturePercentage);

  // Placeholder pH sensor reading (mock value)
  pH = 6.5; 
  if (pH > min_pH && pH < max_pH) {
    digitalWrite(alkaline, LOW);
    digitalWrite(acidic, LOW);
  } else if (pH < min_pH) {
    digitalWrite(alkaline, HIGH);
    digitalWrite(acidic, LOW);
  } else {
    digitalWrite(alkaline, LOW);
    digitalWrite(acidic, HIGH);
  }
  Serial.print("pH: ");
  Serial.println(pH);

  // Send JSON Data
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["altitude"] = altitude;
  doc["ldrvalue"] = ldrValue;
  doc["pH"] = pH;
  doc["soilMoistureValue"] = soilMoistureValue;

  serializeJson(doc, Serial);
  Serial.println();

  delay(2000);
}
