#include <Adafruit_PCD8544.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <EEPROM.h>
#include <Servo.h>
#include "LowPower.h"



#define THINGSBOARD_SERVER "demo.thingsboard.io"


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



