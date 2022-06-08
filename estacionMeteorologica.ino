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
const int yAxisInput = 6;
const int xAxisInput = 7;

// LEDS

const int redLED = 53;
const int blueLED = 2;

// Misc

const int communication = 12;
const int rainSensor = 11;
const int screenPower = 22;


// LCD variables

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 5, 6, 4, 8);

// EEMPROM ADDRESS

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
const int pinLDR = A1;
int lightSourceVoltage;
int ilimunationVoltage;

// Variables for keeping track of the time

unsigned long startingTime;
unsigned long currentTime;
unsigned long generalCurrentTime;
unsigned long generalStartingTime;
const unsigned long tenMinutes = 600000; // Value in ms
unsigned int startTimerEnabled = 1;

// Variables to configure the servos

Servo yAxis;
Servo xAxis;
int xInputServo;
int yInputServo;

// Misc variables

int generalCounter_0 = 0;
int generalCounter_1 = 0;
int flashes = 0;
int generalFlag = 0;

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
    }
}