#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4OLED display;
Zumo32U4Motors motors;
Zumo32U4LineSensors sensors;
Zumo32U4IMU imu;

//parameter:

const float P_factor = 1;
const float I_factor = 0.05;
const float D_factor = 9;

//max speed (<=400):

int const max_speed = 400;
int const min_speed = -400;
int const base_speed = 400;

//smoothing factor (between 0 and 1)

int const smoothing_factor=1;

enum states {
  START,
  RUN,
  FINISH,
  RESET
};

int state = START;  //state that runs

#define NUM 5  //number of line sensors 
int values[NUM];

unsigned long timeStart; //time of start and finish
unsigned long timeFinish;


int diff1_3;  //calculation of correction
int P_Erg;
int I_Erg;
int D_Erg;
int lastP_Erg = 0;
int correction;


int r_speed;  //motorspeed
int l_speed;
int smoothed_r_speed = 0;
int smoothed_lspeed = 0;

bool run = true; //switch between drive and stop 




void setup() {

  Wire.begin();     //initialisation of outputs
  sensors.initFiveSensors();
  Serial.begin(115200);
  imu.init();
  imu.enableDefault();
  while (!imu.gyroDataReady()) {
    delay(5);
  }
}


int sumValues(int values[]) {  //funktion to sum up all values of sensors
  int sum = 0;

  for (int i = 0; i < 5; i++) {
    sum += values[i];
  }
  return sum;
}



void start() {  //state 0 funktion, start prosess

  int i = 0;

  display.setLayout21x8();
  display.clear();

  while (i < 300) {
    imu.readGyro();
    if (imu.g.x < 700) {
      i++;
    } else {
      i = 0;
    }
    delay(10);
    display.gotoXY(1, 1);
    display.println("Place on ground!");
  }

  display.clear();

  sensors.read(values);
  imu.readGyro();  //read value of gyrosensor
  while (sumValues(values) < 5000) {
    display.gotoXY(1, 1);
    display.print("Place at Start!");
    sensors.read(values);
  }

  display.clear();
  display.gotoXY(1, 1);
  display.println("Ready");
  delay(1500);
  display.gotoXY(1, 1);
  display.println("Press Button A");
  display.gotoXY(1, 3);
  display.println("to start.");

  while (buttonA.getSingleDebouncedPress() == 0) {  //wait for button press
  }
  for (int i = 3; i > 0; i--) {  //countdown
    display.clear();
    display.gotoXY(2, 3);
    display.println("Race starts in: ");
    display.gotoXY(9, 4);
    display.println(i);
    buzzer.playNote(NOTE_C(4), 500, 8);
    delay(1000);
  }
  buzzer.playNote(NOTE_G(5), 600, 11);


  display.clear();
  timeStart = millis();  //start of timing
  state = RUN;
}

void pid_calc() {

  sensors.read(values);

  diff1_3 = values[1] - values[3];

  P_Erg = diff1_3;
  I_Erg += diff1_3;
  D_Erg = diff1_3 - lastP_Erg;
  lastP_Erg = P_Erg;

  correction = int(P_factor * P_Erg + I_factor * I_Erg + D_factor * D_Erg);

  r_speed = constrain(base_speed + correction, min_speed, max_speed);
  l_speed = constrain(base_speed - correction, min_speed, max_speed);

  // smooth motorspeed

  if (smoothed_r_speed < r_speed) {
    smoothed_r_speed = smoothed_r_speed + smoothing_factor * abs(r_speed - smoothed_r_speed);
  } else if (smoothed_r_speed > r_speed) {
    smoothed_r_speed = smoothed_r_speed - smoothing_factor * abs(r_speed - smoothed_r_speed);
  }

  if (smoothed_lspeed < l_speed) {
    smoothed_lspeed = smoothed_lspeed + smoothing_factor * abs(l_speed - smoothed_lspeed);
  } else if (smoothed_lspeed > l_speed) {
    smoothed_lspeed = smoothed_lspeed - smoothing_factor * abs(l_speed - smoothed_lspeed);
  }
}

void motor_drive() {

  if (run)
    motors.setSpeeds(-smoothed_lspeed, smoothed_r_speed);  // minus, because we use two right motors
  else
    motors.setSpeeds(0, 0);
}


void no_line_detect() {

  int light = 0;
  for (int n = 0; n < 5; n++) {
    light += values[n];
  }

  if (light < 1100) {
    run = false;
    light = 0;
  } else {
    run = true;
  }
}



void finish() {

  double lapTime;

  motors.setSpeeds(0, 0);

  display.clear();
  timeFinish = millis();   //end of timing
  lapTime = (timeFinish - timeStart) / 1000.000;

  display.gotoXY(9, 4);
  display.print(lapTime);
  delay(10000);

  state = RESET;
}






void loop() {

  if (state == START) {
    start();
  } else if (state == RUN) {

    sensors.read(values);
    if (sumValues(values) > 5000 && (millis() - timeStart) > 1000) {  //check if finish is reached
      state = FINISH;
    } else {

      pid_calc();
      motor_drive();
      no_line_detect();
    }


  } else if (state == FINISH) {

    finish();

  } else if (state == RESET) {

    display.clear();
    display.gotoXY(3, 0);
    display.print("Press reset to restart");
    state = 4;
  }
}
