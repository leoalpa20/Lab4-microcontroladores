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

