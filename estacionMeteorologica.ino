#include <Adafruit_PCD8544.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <EEPROM.h>
#include <Servo.h>
#include "LowPower.h"

// Defines

#define THINGSBOARD_SERVER "demo.thingsboard.io"
#define XAXISPOSITION 0
#define YAXISPOSITION 1
#define DELTA 2
#define RANDOMPOS 10
#define TEMPERATURE_TOKEN "a9Ggq3euSFkiqrHOaraF"

// LED pins and definition

// Axis 
const int yAxisInput = 7;
const int xAxisInput = 6;

// LEDS

const int redLED = 53;
const int blueLED = 2;

// Misc

const int communication = 12;
const int rainSensor = 11;
const int screenPower = 22;


// LCD variables

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 5, 6, 4, 8);

// EEPROM ADDRESS

int eempromAddress = 0;

// S-H parameters for aproximating the temperature for the thermistor, same 
// procedure as the previous lab

// Thermistor resistance

float R1 = 100000;
float logR2, R2, TEMP;
// obtained from  https://www.thinksrs.com/downloads/programs/Therm%20Calc/NTCCalibrator/NTCcalculator.html
float c1 = 0.8586139205e-03, c2 = 2.059709585e-04, c3 = 0.8130635267e-07;

// Variables for the temperature

int thermistorValue;
int displayTemperature;

// Variables for the humidity

int displayHumidity;
int humidityVale;
float normalizedHumidity;

// Variables for the wind

int windValue;
int displayWind;
float windNormalized;
float windVoltage;
float windSpeed;


// Variables for the rain

float rainNormalized;
int rainValue;
int displayRain;

// Variables for the battery

float batteryNormalized;
int batteryValue;
int displayBattery;
float batteryVoltage;

// Variables for the light 

int lightValue;
float lightNormalized;
int displayLight;
// Values in K ohms
const long darknessResistance = 500;
const int resistanceToLight = 12.5;
const int calibrationResistance = 10;
const int LDRPin = A1;
int LDRPinRead;
int ilimunationVoltage;

// Variables for keeping track of the time

unsigned long startingTime;
unsigned long currentTime;
unsigned long globalCurrentTime;
unsigned long globalStartingTime;
const unsigned long tenMinutes = 6000; // Value in ms
unsigned int startTimerEnabled = 1;

// Variables to configure the servos

Servo yAxis;
Servo xAxis;
int xInputServo;
int yInputServo;

// Misc variables

int globalCounter_0 = 0;
int globalCounter_1 = 0;
int flashes = 0;
int globalFlag = 0;

// S-H equation

void hartEquation()
{
    R2 = R1 * (1023.0 / (float)thermistorValue - 1.0);
    logR2 = log(R2);
    TEMP = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
    TEMP = TEMP - 273.15; // Convert to celsius
}

// Blink LEDs

void blink()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(300);
    digitalWrite(LED_BUILTIN, LOW);
    delay(300);
}

// Verify battery level

void verifyBatteryLevel()
{
    if (displayBattery <= 25) // 25% of battery
    {
          LowPower.idle(SLEEP_8S, ADC_OFF, TIMER5_OFF, TIMER4_OFF,
                TIMER3_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF,
                USART3_OFF, USART2_OFF, USART1_OFF, USART0_OFF, TWI_OFF);
    
    if(globalFlag < 3)
    {
        globalFlag++;
    }

    else 
        {
            globalFlag = 0;
            digitalWrite(redLED, !digitalRead(redLED));
        }
    }
    else{
        digitalWrite(redLED, LOW);
    }
}

// Function to refresh the display
// only works  if the switch is ON, to save energy
void refresh_display()
{
    if(digitalRead(screenPower) == LOW){
        display.setCursor(0,0);
        display.print("Batt: ");
        display.print(displayBattery);
        display.println("%");
        display.print("Temp: ");
        display.print(displayTemperature);
        display.println("C");
        display.print("Hum: ");
        display.print(displayHumidity);
        display.println("%");
        display.print("Wind: ");
        display.print(displayWind);
        display.println("m/s");
        display.print("Rain: ");
        if(displayRain == 1)
        {
            display.println("Rain");
        }
        else
        {
            display.println("No rain");
        }
        display.print("Light: ");
        display.print(ilimunationVoltage);
        display.println("Lux");

    }
    display.display();
    display.clearDisplay();
}

// Refreshes the serial comm if the switch is OFF, prints to serial

void refresh_serial()
{
    if(digitalRead(communication) == LOW)
    {
        if(startTimerEnabled == 1)
        {
            startingTime = millis();
            startTimerEnabled = 0;
        }
        currentTime = millis();
        if(currentTime - startingTime >= tenMinutes)
        {
            Serial.print(displayBattery);
            Serial.print(",");
            Serial.print(displayTemperature);
            Serial.print(",");
            Serial.print(displayHumidity);
            Serial.print(",");
            Serial.print(displayWind);
            Serial.print(",");
            if(displayRain == 1)
            {
                Serial.print("Yes");
            }
            else
            {
                Serial.print("No rain");
            }
            Serial.print(",");
            Serial.println(ilimunationVoltage);
            startTimerEnabled = 1;
        }
    }
}

void adjustSolarPanel(){
  xInputServo = analogRead(xAxisInput);
  yInputServo = analogRead(yAxisInput);

  xInputServo = map(xInputServo, 0, 1023, 0, 180);
  yInputServo = map(yInputServo, 0, 1023, 0, 180);

  xAxis.write(round(xInputServo));
  yAxis.write(round(yInputServo));
}

// Function to check if there is rain present

void checkIfRaining()
{
    if(digitalRead(rainSensor) == LOW)
    {
        displayRain = 1;
    }
    else
    {
        displayRain = 0;
    }
}

// Function to verify EEPROM memory

void verifyMemory()
{
    if (eempromAddress == EEPROM.length())
    {
        eempromAddress = 0;
        for( int i = 0; i < EEPROM.length(); i++)
        {
            EEPROM.write(i, 0);
        }
    }
}

void writeToMemory()
{
    globalCurrentTime = millis();
    if(globalCurrentTime - globalStartingTime >= tenMinutes/2)
    {
        verifyMemory();
        EEPROM.write(eempromAddress, displayTemperature);
        eempromAddress++;

        verifyMemory();
        EEPROM.write(eempromAddress, displayHumidity);
        eempromAddress++;

        verifyMemory();
        EEPROM.write(eempromAddress, displayLight);
        eempromAddress++;

        verifyMemory();
        EEPROM.write(eempromAddress, displayWind);
        eempromAddress++;

        verifyMemory();
        EEPROM.write(eempromAddress, displayRain);
        eempromAddress++;

        globalStartingTime = globalCurrentTime;
    }
}

// Setup function

void setup()
{
    Serial.begin(9600);
    display.begin();
    display.setContrast(75);
    display.display();
    delay(500);
    display.clearDisplay();   
    display.setTextSize(1);
    display.setTextColor(BLACK);

    // SET specific pins as I/O

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(blueLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    pinMode(communication, INPUT);
    pinMode(screenPower, INPUT);
    pinMode(rainSensor, INPUT);

    // Configure the servos

    xAxis.attach(8, 1000, 2000);
    yAxis.attach(9, 1000, 2000);

    // Start execution timer
    globalStartingTime = millis();

    // Set all EEPROM memory to 0
    for (int i = 0 ; i < EEPROM.length() ; i++) {
        EEPROM.write(i, 0);
      }

}

// Loop function

void loop()
{
    if (digitalRead(communication) == HIGH)
    {
        digitalWrite(blueLED, LOW);
        flashes = 0;
        globalCounter_0 = 0;
        globalCounter_1 = 0;

    }
    if(digitalRead(communication) == LOW)
    {
        if(flashes < 10)
        {
            if(globalCounter_0 < 10)
            {
                globalCounter_0 = globalCounter_0 + 1;
            }
            else
            {
                digitalWrite(blueLED, !digitalRead(blueLED));
                flashes = flashes + 1;
                globalCounter_0 = 0;
            }
        }
        else
        {
            if(globalCounter_1 < 90)
            {
                globalCounter_1 = globalCounter_1 + 1;
            }
            else
            {
                globalCounter_1 = 0;
                flashes = 0;
            }
        }
    }
    LDRPinRead = analogRead(LDRPin);
    ilimunationVoltage = ((long)LDRPinRead*darknessResistance*10)/((long)resistanceToLight*calibrationResistance*(1024-LDRPinRead));
    // Read values for the temp sensor
    thermistorValue = analogRead(A0);
    hartEquation();
    displayTemperature = round(TEMP);

    // Read values for the humidity sensor
    humidityVale = analogRead(A15);
    normalizedHumidity = humidityVale/10.23;
    displayHumidity = round(normalizedHumidity);

    // Read values for the Wind sensor
    windValue = analogRead(A14);
    windNormalized = windValue/10.23;
    windVoltage = windValue/204.6;
    windSpeed = (windVoltage)*1000/(1.525*32.4);
    displayWind = windSpeed;

    // Read values for the battery;
    batteryValue = analogRead(A13)-926;
    batteryNormalized = batteryValue/0.96;

    if(batteryNormalized >= 0)
    {
        displayBattery = round(batteryNormalized);
    }
    else
    {
        displayBattery = 0;
    }

    verifyBatteryLevel();
    checkIfRaining();
    writeToMemory();
    adjustSolarPanel();
    refresh_serial();
    refresh_display();




}
