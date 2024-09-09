#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>

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
const float Kd = 2.5;

int const max_speed = 400;
int const min_speed = -400;
int const base_speed = 300;

int rspeed;
int lspeed;
int smoothed_rspeed = 0;
int smoothed_lspeed = 0;

bool run = true;

// Glättungsfaktor (zwischen 0 und 1)
const float smoothing_factor = 0.95;

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

void setup() {
    sensors.initFiveSensors();
    Serial.begin(115200);
}

void loop() {
    pid_calc();
    motor_drive();
    no_line_detect();
}
