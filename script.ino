#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAX_SAMPLES 100

// Pins
const int PIN_SENSOR = 34; // AD8317 Output
const int PIN_BUZZER = 27;
const int LED_PINS[] = {32, 25, 33, 26};
const int LED_GREEN = 2;

// AD8317 Calibration
const float SLOPE = -0.022;
const float INTERCEPT = 0.55;

// Signal variables absolute
int rawValue = 0;
float voltage = 0;
float dBm = 0;

// Signal variable median
float dBm_median;

// Signal variable standard deviation
float std_dev = 0;

// Signal variable peak to average
float par = 0;

float baseline = -45;
float n = 8;

// Samples
float samples[MAX_SAMPLES];
int sampleCount = 0;

// OLED variables
unsigned long previousDisplayMillis = 0;
const long DISPLAY_INTERVAL = 300;

// LED Thresholds
const float DB_THRESHOLDS[] = {baseline + n, baseline + 2 * n, baseline + 3 * n, baseline + 4 * n};

// Alert
boolean alert = false;

void setup()
{
    Serial.begin(115200);

    // Initialize LEDs
    for (int i = 0; i < 4; i++)
    {
        pinMode(LED_PINS[i], OUTPUT);
        digitalWrite(LED_PINS[i], HIGH);
    }

    ledcAttach(LED_GREEN, 5000, 8);
    ledcWrite(LED_GREEN, 10);

    // Initialize Buzzer
    ledcAttach(PIN_BUZZER, 2000, 8);

    // Initialize OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("RF Detector Init...");
    display.display();
    delay(1000);

    // LEDs to low
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(LED_PINS[i], LOW);
    }

    Serial.println("Signal_dBm,Signal_dBm_median");
}

void loop()
{
    unsigned long currentMillis = millis();

    // Collecting data
    rawValue = analogRead(PIN_SENSOR);
    voltage = rawValue * (3.3 / 4095);
    dBm = (voltage - INTERCEPT) / SLOPE;

    if (sampleCount < MAX_SAMPLES)
    {
        samples[sampleCount] = dBm;
        sampleCount++;
    }
    else
    {
        for (int i = 0; i < MAX_SAMPLES - 1; i++)
        {
            samples[i] = samples[i + 1];
        }
        samples[MAX_SAMPLES - 1] = dBm;
    }

    // Calculate average
    double sum = 0;
    for (int i = 0; i < sampleCount; i++)
    {
        sum += samples[i];
    }
    dBm_median = sum / sampleCount;

    // Standard deviation
    float sumSqrDif = 0;
    for (int i = 0; i < sampleCount; i++)
    {
        float diff = samples[i] - dBm_median;
        sumSqrDif += diff * diff;
    }

    std_dev = sqrt(sumSqrDif / sampleCount);

    // Peak to average
    float maxVal = samples[0];

    for (int i = 0; i < sampleCount; i++)
    {
        if (maxVal < samples[i])
        {
            maxVal = samples[i];
        }
    }
    par = maxVal - dBm_median;

    // Check if drone nearby
    if ((par > 30 && std_dev > 15) || dBm_median > DB_THRESHOLDS[1])
    {
        alert = true;
    }
    else
    {
        alert = false;
    }

    Serial.print(dBm);
    Serial.print(",");
    Serial.print(dBm_median);
    Serial.print(",");
    Serial.print(DB_THRESHOLDS[0]);
    Serial.print(",");
    Serial.print(DB_THRESHOLDS[1]);
    Serial.print(",");
    Serial.print(DB_THRESHOLDS[2]);
    Serial.print(",");
    Serial.println(DB_THRESHOLDS[3]);

    if (currentMillis - previousDisplayMillis >= DISPLAY_INTERVAL)
    {
        previousDisplayMillis = currentMillis;
        // Update display
        display.clearDisplay();

        // Title
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("RF Signal Monitor");

        // dBm Value
        display.setTextSize(2);
        display.setCursor(0, 20);
        display.print(dBm_median, 1);
        display.setTextSize(1);
        display.print(" dBm");

        // Drone alert
        if (alert == true)
        {
            display.setTextSize(2);
            display.setCursor(0, 50);
            display.print("DRONE NEARBY");
        }

        display.display();
    }
    // Update LEDs
    for (int i = 0; i < 4; i++)
    {
        if (dBm_median >= DB_THRESHOLDS[i])
        {
            digitalWrite(LED_PINS[i], HIGH);
        }
        else
        {
            digitalWrite(LED_PINS[i], LOW);
        }
    }

    // Update Buzzer
    float THRESHOLD = 7;
    float TONE = 2000;
    if (dBm >= dBm_median + 9)
    {
        ledcWriteTone(PIN_BUZZER, TONE);
    }
    else
    {
        ledcWriteTone(PIN_BUZZER, 0);
    }
}