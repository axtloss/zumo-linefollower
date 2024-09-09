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
int lp;
int correction;

const float Kp = 0;
const float Ki = 0;
const float Kd = 0;

int const max_speed;
int const base_speed;

int rspeed;
int lspeed;

bool run;

void setup(){

}

void loop(){

    pid_calc();
    motor_drive();

}



void pid_calc(){

    sensors.read(readings);

    error = readings[1] - readings[3];

    p = error;
    i += error;
    d = p - lp;
    lp = p;

    correction = int(Kp*p + Ki*i + Kd*d);

    rspeed = constrain(base_speed + correction,0,max_speed);
    lspeed = constrain(base_speed - correction,0,max_speed);

}

void motor_drive(int l,int r){
    if(run) motors.setSpeeds(l,r);
}

void fall_detect(){

}

void no_line_detect(){

}

void finish_detect(){

}

void object_detect(){
    
}

void timer(){

}