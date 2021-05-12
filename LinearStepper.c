#include "LinearStepper.h"
#include <stdlib.h>

//#define errorCheck(exp) if(!(exp)) fprintf(stderr,"Assertion error on %s #%d",__FILE__,__LINE__);
#define errorCheck(exp)

//#define DEBUG

#define PI 3.12159

float linearAct_convert (uint8_t rotationsPerMeter, float value);

t_LinearAct *linearActNew (uint8_t stepsPerRev, uint8_t stepMultiplier, uint8_t rotationsPerMeter, enum e_handedness handedness) {

	//Allocates memory for the Stepper object
	t_LinearAct *linearAct = NULL;
	linearAct = malloc(sizeof(t_LinearAct));
	if(linearAct == NULL) {
		return NULL;
	}

	//Allocates memory for the Stepper object
	linearAct->stepper = stepperNew (stepsPerRev, stepMultiplier);
	if(linearAct->stepper == NULL) {
		linearActKill (linearAct);
		return NULL;
	}

	//check for programming error as rotationsPerMeter sould be greater than zero
	errorCheck(rotationsPerMeter > 0);

	//sets the physical stepper motor parameter and the stepping mode multiplier
	linearAct->rotationsPerMeter = rotationsPerMeter;
	linearAct->handedness = handedness;

	return linearAct;
} //linearActNew


void linearActSetSpeed (t_LinearAct *linearAct, float linSpeed, float linAccel) {

	float angSpeed;
	float angAccel;

	if(linearAct != NULL) {

		//convert the linear values to angular values
		angSpeed = linearAct_convert (linearAct->rotationsPerMeter, linSpeed);
		angAccel = linearAct_convert (linearAct->rotationsPerMeter, linAccel);
		
		//if the thread is right hand threaded, the speed (linear)(move away from motor if > 0) will be sent to the SmoothStepper without change
		//if the thread is left hand threaded, the speed (linear)(move away from motor if > 0) will be sent to the SmoothStepper with the signal inverted
		//the SmoothStepper will rotate CW for positive speeds or CCW for negative speeds
		//RHT + CW = MOVE AWAY; RHT + CCW = MOVE CLOSER; LHT + CW = MOVE CLOSER; LHT + CCW = MOVE AWAY;
		if(linearAct->handedness == LHT) {
			angSpeed = -angSpeed;
		}

		stepperSetSpeed (linearAct->stepper, angSpeed, angAccel);
	}
} //linearActSetSpeed


void linearActSetDisplacement (t_LinearAct *linearAct, float linDisp, float linSpeed, float linAccel, float linDeccel) {

	float angDisp;
	float angSpeed;
	float angAccel;
	float angDeccel;

	if(linearAct != NULL) {

		//convert the linear values to angular values
		angDisp = linearAct_convert (linearAct->rotationsPerMeter, linDisp);
		angSpeed = linearAct_convert (linearAct->rotationsPerMeter, linSpeed);
		angAccel = linearAct_convert (linearAct->rotationsPerMeter, linAccel);
		angDeccel = linearAct_convert (linearAct->rotationsPerMeter, linDeccel);

		//if the thread is right hand threaded, the speed (linear)(move away from motor if > 0) will be sent to the SmoothStepper without change
		//if the thread is left hand threaded, the speed (linear)(move away from motor if > 0) will be sent to the SmoothStepper with the signal inverted
		//the SmoothStepper will rotate CW for positive speeds or CCW for negative speeds
		//RHT + CW = MOVE AWAY; RHT + CCW = MOVE CLOSER; LHT + CW = MOVE CLOSER; LHT + CCW = MOVE AWAY; (speed > 0)
		if(linearAct->handedness == LHT) {
			angDisp = -angDisp;
			angSpeed = -angSpeed;
		}
		
		stepperSetDisplacement (linearAct->stepper, angDisp, angSpeed, angAccel, angDeccel);
	}

} //linearActSetDisplacement


uint32_t linearActRun (t_LinearAct *linearAct, uint32_t time, void (*stepFunc) (enum e_StepDirection dir)) {

	uint32_t delay = 0;

	if(linearAct != NULL) {
		
		delay = stepperRun (linearAct->stepper, time, stepFunc);
	}
	
    return delay;
} //linearActRun

uint8_t linearActIsActive(t_LinearAct *linearAct) {
	
	if(linearAct != NULL) {
		return stepperIsActive(linearAct->stepper);
	}
	
	return 0;
} //linearActIsActive

void linearActStop (t_LinearAct *linearAct) {

	if(linearAct != NULL) {
		stepperStop(linearAct->stepper);
	}
} //linearActStop


void linearActKill (t_LinearAct *linearAct) {

	if(linearAct != NULL) {
		
		stepperKill (linearAct->stepper);
		free(linearAct);
	}

} //linearActKill

float linearAct_convert (uint8_t rotationsPerMeter, float value) {
	
	float result = 0.0;
	
	//to convert from m to rad, we multiply the value in m by the value of rotation/m and by rad/rotation
	result = value * (float) rotationsPerMeter * PI;
	
	return result;
}
