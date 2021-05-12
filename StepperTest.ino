///*
//#include "SmoothStepper.h"
#include "LinearStepper.h"
#include "DRV8825.h"
#include <stdlib.h>

#define pin_dir 13
#define pin_step 12
#define pin_enable 0
#define pin_reset 0
#define pin_sleep 11
#define pin_error 0
#define pin_mode0 0
#define pin_mode1 0
#define pin_mode2 0

t_LinearAct *linact;

t_DRV8825 *stp;

int isrunning;

uint32_t delay_;

char buff[256];
uint8_t bufflen = 0;
char *tok;

float disp, spd, accel, deccel;

uint16_t steps;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_step, OUTPUT);
  pinMode(pin_enable, OUTPUT);

  stp = DRV8825_New(pin_dir,pin_step, pin_enable, pin_reset, pin_sleep, pin_error, FULLSTEP, pin_mode0, pin_mode1, pin_mode2);
  linact = linearActNew(200,1,125,RHT); 
  
  isrunning = 0;

  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  if(Serial.available()){
//    while(Serial.available()){
//        if(bufflen < 255) {
//          bufflen++;
//          buff[bufflen] = Serial.getc();
//          buff[bufflen + 1] = '\0';
//        }
    bufflen = Serial.readBytes(buff,255);
    buff[bufflen] = '\0';

    disp = 0.1;
    spd = 0.1;
    accel = 0.1;
    deccel = 0.1;
    steps = 0;

    tok = strtok(buff, " ");
    if(tok != NULL) {
      disp = atof(tok);
    }
    tok = strtok(NULL, " ");
    if(tok != NULL) {
      spd = atof(tok);
    }
    tok = strtok(NULL, " ");
    if(tok != NULL) {
      accel = atof(tok);
    }
    tok = strtok(NULL, " ");
    if(tok != NULL) {
      deccel = atof(tok);
    }

    if(disp > 0.1) {
      disp = 0.1;
      }

    //Serial.print(bufflen);
    //Serial.print(buff);
    Serial.print("\nRotating:\n");
    Serial.print("\tdisplacement: ");
    Serial.print(disp);
    Serial.print("\n\tspeed: ");
    Serial.print(spd);
    Serial.print("\n\tacceleration: ");
    Serial.print(accel);
    Serial.print("\n\tdecceleration: ");
    Serial.print(deccel);

    DRV8825_Wake(stp);
    
    linearActSetDisplacement (linact, disp, spd, accel, deccel);

    stepperDump (linact->stepper);

    isrunning = 1;
  }

  do {
    delay_ = linearActRun (linact, micros(), &stepperStep);
    
    //delayMicroseconds(100);
  } while(linact->stepper->plan.mode != IDDLE);

  if(isrunning == 1) {
    Serial.print("\ndone ");
    Serial.print(steps);
    Serial.print(" steps :D\n");
    DRV8825_Sleep(stp);
    
  isrunning = 0;
  }  
  
}

void stepperStep (enum e_StepDirection dir) {

  DRV8825_Step(stp, (dir == CW ? DRV8825_CW : DRV8825_CCW));
  steps++;
}


void stepperDump (t_Stepper *stepper) {

    Serial.print("\nDumping stepper");
    if(stepper != NULL) {

        Serial.print("\n\tstate.stepAngle \t");
        Serial.print(stepper->def.stepAngle);
        Serial.print("\n\tstate.dir \t\t");
        Serial.print((stepper->state.dir == CW ? "CW" : "CCW"));
        Serial.print("\n\tstate.targSpeed \t");
        Serial.print(stepper->state.targSpeed);
        Serial.print("\n\tstate.startSpeed \t");
        Serial.print(stepper->state.startSpeed);
        Serial.print("\n\tstate.angSpeed \t\t");
        Serial.print(stepper->state.angSpeed);
        Serial.print("\n\tstate.angAccel \t\t");
        Serial.print(stepper->state.angAccel);
        Serial.print("\n\tstate.angDeccel \t");
        Serial.print(stepper->state.angDeccel);
        Serial.print("\n\tstate.steps \t\t");
        Serial.print(stepper->state.steps);
        Serial.print("\n\tstate.timeLastStep \t");
        Serial.print(stepper->state.timeLastStep);
        Serial.print("\n\tstate.delayNextStep \t");
        Serial.print(stepper->state.delayNextStep);
    
        Serial.print("\n\tstate.mode \t\t");
        Serial.print(stepper->plan.mode);
        Serial.print("\n\tstate.stepsAccel \t");
        Serial.print(stepper->plan.stepsAccel);
        Serial.print("\n\tstate.stepsConst \t");
        Serial.print(stepper->plan.stepsConst);
        Serial.print("\n\tstate.stepsDeccel \t");
        Serial.print(stepper->plan.stepsDeccel);
  }
} //stepperDump
//*/


/*
#include "SmoothStepper.h"
#include "DRV8825.h"
#include <stdlib.h>

#define pin_dir 8
#define pin_step 9
#define pin_enable 10
#define pin_reset 0
#define pin_sleep 0
#define pin_error 0
#define pin_mode0 0
#define pin_mode1 0
#define pin_mode2 0

t_Stepper *stepper;

t_DRV8825 *stp;

int isrunning;

uint32_t delay_;

char buff[256];
uint8_t bufflen = 0;
char *tok;

float disp, spd, accel, deccel;

uint16_t steps;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_step, OUTPUT);
  pinMode(pin_enable, OUTPUT);

	stp = DRV8825_New(pin_dir,pin_step, pin_enable, pin_reset, pin_sleep, pin_error, FULLSTEP, pin_mode0, pin_mode1, pin_mode2);
	stepper = stepperNew(200,1); 
  
  isrunning = 0;

  Serial.begin(115200);
  
}

void loop() {
  // put your main code here, to run repeatedly:

  if(Serial.available()){
//    while(Serial.available()){
//        if(bufflen < 255) {
//          bufflen++;
//          buff[bufflen] = Serial.getc();
//          buff[bufflen + 1] = '\0';
//        }
    bufflen = Serial.readBytes(buff,255);
    buff[bufflen] = '\0';

    disp = 1;
    spd = 1;
    accel = 1;
    deccel = 1;
    steps = 0;

    tok = strtok(buff, " ");
    if(tok != NULL) {
      disp = atof(tok);
    }
    tok = strtok(NULL, " ");
    if(tok != NULL) {
      spd = atof(tok);
    }
    tok = strtok(NULL, " ");
    if(tok != NULL) {
      accel = atof(tok);
    }
    tok = strtok(NULL, " ");
    if(tok != NULL) {
      deccel = atof(tok);
    }

    //Serial.print(bufflen);
    //Serial.print(buff);
    Serial.print("\nRotating:\n");
    Serial.print("\tdisplacement: ");
    Serial.print(disp);
    Serial.print("\n\tspeed: ");
    Serial.print(spd);
    Serial.print("\n\tacceleration: ");
    Serial.print(accel);
    Serial.print("\n\tdecceleration: ");
    Serial.print(deccel);
    
    stepperSetDisplacement (stepper, disp, spd, accel, deccel);

    stepperDump (stepper);

    isrunning = 1;
  }

  do {
    delay_ = stepperRun (stepper, micros(), &stepperStep);

    //delayMicroseconds(100);
  } while(stepper->plan.mode != IDDLE);

  if(isrunning == 1) {
    Serial.print("\ndone ");
    Serial.print(steps);
    Serial.print(" steps :D\n");
    
	isrunning = 0;
  }  
  
}

 void prt(char *s) {
  Serial.print(s);
  }

void stepperStep (enum e_StepDirection dir) {

  DRV8825_Step(stp, (dir == CW ? DRV8825_CW : DRV8825_CCW));
  steps++;
  
}


void stepperDump (t_Stepper *stepper) {

    Serial.print("\nDumping stepper");
    if(stepper != NULL) {

        Serial.print("\n\tstate.stepAngle \t");
        Serial.print(stepper->def.stepAngle);
        Serial.print("\n\tstate.dir \t\t");
        Serial.print((stepper->state.dir == CW ? "CW" : "CCW"));
        Serial.print("\n\tstate.targSpeed \t");
        Serial.print(stepper->state.targSpeed);
        Serial.print("\n\tstate.startSpeed \t");
        Serial.print(stepper->state.startSpeed);
        Serial.print("\n\tstate.angSpeed \t\t");
        Serial.print(stepper->state.angSpeed);
        Serial.print("\n\tstate.angAccel \t\t");
        Serial.print(stepper->state.angAccel);
        Serial.print("\n\tstate.angDeccel \t");
        Serial.print(stepper->state.angDeccel);
        Serial.print("\n\tstate.steps \t\t");
        Serial.print(stepper->state.steps);
        Serial.print("\n\tstate.timeLastStep \t");
        Serial.print(stepper->state.timeLastStep);
        Serial.print("\n\tstate.delayNextStep \t");
        Serial.print(stepper->state.delayNextStep);
    
        Serial.print("\n\tstate.mode \t\t");
        Serial.print(stepper->plan.mode);
        Serial.print("\n\tstate.stepsAccel \t");
        Serial.print(stepper->plan.stepsAccel);
        Serial.print("\n\tstate.stepsConst \t");
        Serial.print(stepper->plan.stepsConst);
        Serial.print("\n\tstate.stepsDeccel \t");
        Serial.print(stepper->plan.stepsDeccel);
  }
} //stepperDump
*/
