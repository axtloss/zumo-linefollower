
#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
#include "font.h"

Zumo32U4LineSensors sensors;
Zumo32U4Motors motors;
Zumo32U4OLED display;

#define NUM 5
int readings[NUM];

int error;
int p;
int i;
int d;
int lp = 0;
int correction;

int light = 0;
const float Kp = 1;
const float Ki = 0.05;
const float Kd = 9;

int const max_speed = 400;
int const min_speed = -400;
int const base_speed = 400;

int rspeed;
int lspeed;
int smoothed_rspeed = 0;
int smoothed_lspeed = 0;

bool run = true;

int update_rate = 100;
int curr_rate = 100;

// Glättungsfaktor (zwischen 0 und 1)
const float smoothing_factor = 0.98;

void pid_calc() {
    sensors.read(readings);

    error = readings[1] - readings[3];

    p = error;
    i += error;
    d = p - lp;
    lp = p;

    correction = int(Kp * p + Ki * i + Kd * d);

    rspeed = constrain(base_speed + correction, min_speed, max_speed);
    lspeed = constrain(base_speed - correction, min_speed, max_speed);

    // Geschwindigkeit glätten
    smoothed_rspeed = smoothed_rspeed + smoothing_factor * (rspeed - smoothed_rspeed);
    smoothed_lspeed = smoothed_lspeed + smoothing_factor * (lspeed - smoothed_lspeed);

    Serial.print(smoothed_rspeed);
    Serial.print(" | ");
    Serial.print(smoothed_lspeed);
    Serial.print(" | ");
    Serial.print(correction);
    //curr_rate += 1;
}

void motor_drive() {
    Serial.print(" | ");
    Serial.print(run);
    Serial.print(" | ");
    if (run) motors.setSpeeds(smoothed_lspeed, smoothed_rspeed);
    else motors.setSpeeds(0, 0);
}

void fall_detect() {
    // Implementiere die Fall-Erkennung
}

void no_line_detect() {
    light = 0;
    for (int n = 0; n < 5; n++) {
        Serial.print(readings[n]);
        Serial.print(" | ");
        light += readings[n];
    }

    Serial.println();

    if (light < 1100) {
        run = false;
        light = 0;
    } else {
        run = true;
    }
}

void finish_detect() {
    // Implementiere die Ziel-Erkennung
}

void object_detect() {
    // Implementiere die Objekt-Erkennung
}

void timer() {
    // Implementiere den Timer
}


void loadCustomCharactersFrwd()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

void loadCustomCharactersBkwd()
{
  static const char levels[] PROGMEM = {
    63, 63, 63, 63, 63, 63, 63, 0, 0, 0, 0, 0, 0, 0
  };
  display.loadCustomCharacter(levels + 0, 6);  // 1 bar
  display.loadCustomCharacter(levels + 1, 5);  // 2 bars
  display.loadCustomCharacter(levels + 2, 4);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 2);  // 5 bars
  display.loadCustomCharacter(levels + 5, 1);  // 6 bars
  display.loadCustomCharacter(levels + 6, 0);  // 7 bars
}


void display_bar_frwd(int height, int x, int y) {
  const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};

  loadCustomCharactersFrwd();
  for (int i = 0; i <= 1; i++) {
    display.gotoXY(x, y-i);
    display.print(barChars[constrain(height-8*i, 0, 8)]);
  }
}

void display_bar_bkwd(int height, int x, int y) {
  const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};

  loadCustomCharactersBkwd();
  for (int i = 0; i <= 1; i++) {
    display.gotoXY(x, y+i);
    display.print(barChars[constrain(height-8*i, 0, 8)]);
  }
}

void display_direction() {
  if (smoothed_lspeed < 0 && smoothed_rspeed > 0) {
    display.gotoXY(4, 1);
    display.print("<-");
  } else if (smoothed_lspeed > 0 && smoothed_rspeed < 0) {
    display.gotoXY(4, 1);
    display.print("->");
  } else if (smoothed_lspeed > 0 && smoothed_rspeed > 0) {
    display.gotoXY(4, 1);
    display.print("/\\");
  } else if (smoothed_lspeed < 0 && smoothed_rspeed < 0) {
    display.gotoXY(4, 1);
    display.print("\\/");
  } else {
    display.gotoXY(4, 1);
    display.print("  ");
  }
}

void display_show() {
  if (smoothed_lspeed > 0)
    display_bar_frwd(run ? map(smoothed_lspeed, 0, max_speed, 0, 32) : 0, 0, 1);
  else
    display_bar_bkwd(run ? map(-smoothed_lspeed, 0, max_speed, 0, 32) : 0, 0, 2);

  if (smoothed_rspeed > 0)
    display_bar_frwd(run ? map(smoothed_rspeed, 0, max_speed, 0, 32) : 0, 10, 1);
  else
    display_bar_bkwd(run ? map(-smoothed_rspeed, 0, max_speed, 0, 32) : 0, 10, 2);

  display_direction();
}

void setup() {
    sensors.initFiveSensors();
    Serial.begin(115200);
    display.setLayout11x4();
    display.clear();
}

void loop() {
    pid_calc();
    motor_drive();
    no_line_detect();
    display_show();
}
