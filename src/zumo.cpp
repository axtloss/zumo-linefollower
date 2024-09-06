#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <Wire.h>
#include <Zumo32U4.h>
#include <stdio.h>

Zumo32U4OLED display;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;

#define NUM_SENSORS 5
uint16_t lineSensorValues[NUM_SENSORS];

int currentLeftSpeed = 0;
int currentRightSpeed = 0;

void setup()
{
  lineSensors.initFiveSensors();
  display.setLayout21x8();
}

void
moveForward ()
{
  motors.setSpeeds(0,0);
  currentLeftSpeed = 200;
  currentRightSpeed = 200;
  motors.setSpeeds(currentLeftSpeed, currentRightSpeed);
}

void
moveLeft ()
{
  int x = currentRightSpeed;
  motors.setSpeeds(currentLeftSpeed*-1, ((x)>0?(x):-(x)));
}

void
moveRight ()
{
  int x = currentLeftSpeed;
  motors.setSpeeds(((x)>0?(x):-(x)), currentRightSpeed*-1);
}

void printReadingsToDisplay()
{
  display.gotoXY(0, 0);
  display.print(lineSensorValues[1]);
  display.print(F("    "));
  display.print(lineSensorValues[2]);
  display.print(F("    "));
  display.print(lineSensorValues[3]);
  display.print(F("    "));

  display.gotoXY(0,2);
  if (lineSensorValues[2] >= 440) {
    display.print("/\\");
    moveForward();
  }
  else if (lineSensorValues[1] >= 440) {
    display.print("<-");
    moveLeft();
  }
  else if (lineSensorValues[3] >= 440) {
    display.print("->");
    moveRight();
  }
  else {
    motors.setSpeeds(0, 0);
  }
}


void loop()
{
  static uint16_t lastSampleTime = 0;

  if ((uint16_t)(millis() - lastSampleTime) >= 100)
  {
    lastSampleTime = millis();

    lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);

    printReadingsToDisplay();
  }
}
