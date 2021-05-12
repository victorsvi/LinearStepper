/*
	****************************************************************************
	** LinearStepper - Stepper motor for linear actuators with speed profiles **
	****************************************************************************
	
	By Victor Henrique Salvi (victorsvi@gmail.com)
	
	Functionality:
		Its a interface to use linear actuator driven by stepper motors in open loop configuration.
		The library supports two kinds of movement:
			(A) Accelerate to a given speed and keep going. It will accelerate to the desired speed and then will keep moving at constant speed.
				This mode is intended to applications which the motor will rotate for a given time and the displacement done isn't relevant, for example,
				a linear actuator where the stop signal is determined by a sensor. 
				This mode supports real time speed profiles: you can change the speed while the motor is rotating and it will account the current
				speed in order to reach the new target speed.
			(B) Move a given displacement smoothly. It will start stopped and accelerate to the desired speed then move at constant speed than deccelerate ultil stop. 
				The full movement (stopped - at targed speed - stopped) will displace a given ammount. Sometimes the relation between the acceleration and the
				given displacement don't allow the motor to reach the desired speed so it will deccelerate before reaching the targed speed.
				This mode is intended to applications which the motor needs to do precise finite movements.
			    The angle displacement is known before the movement is plannned. The movement is planned and can't be changed in real time. If the motor
				is running and a new displacement is set, the motor will stop immediately.
		This interface is not intended to handle specific stepper drivers. Its reponssible to generate speed profiles and abstracts the fact that the
		stepper can just move at discrete steps.
		Please notice that besides the fact that this library specifies the variables as real numbers, the motor might not be able to rotate at
		certain speeds or rotate a certain amount. The problem resides in the discretization of the stepper movement. Problems can occur when:
			- Trying to do an agular displacement smaller then the step angle of the motor. The motor might not rotate or rotate more than specified.
			- Trying to rotate at a very slow speed. The inter-step delay might be too big so the rotation is done in small increments in rotation angle.
			- Trying to rotate at a very high speed. The motor/driver might not support such step frequecy or the program might take longer to loop than the time to do the next step.
			- Trying to accelerate at a very high rate. The motor plus the load have inerce. The motor torque might not be strong enough to speed up the system fast.
		Obs: Positive speeds will make the object go away from the motor. Negative speeds will make the object go closer to the motor.
	
	Changelog:
		15/05/2019:
			Frist version.

*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "SmoothStepper.h"

#ifndef H_LINEARSTEPPER
#define H_LINEARSTEPPER

/* Data Types */

	enum e_handedness {LHT = 0, RHT}; //RHT -> CW; LHT -> CCW; for positive speeds (away from motor)

	struct s_LinearAct {
		uint8_t rotationsPerMeter;
		enum e_handedness handedness;
		t_Stepper *stepper;
	};

	typedef struct s_LinearAct t_LinearAct;

/* Functions */

	/* Creates a new linear actuator 
	 * The stepsPerRev parameter is how many steps the motor needs to do in order to complete one full rotation. It must be greater than zero. 
	 * The stepMultiplier is related to the indexer used (full step = 1, half-step = 2, microstep 1/4 = 4, ...). It must be greater than zero. 
	 * The total steps to do one revolution is the two parameters multiplied.
	 * The rotationsPerMeter is how many rotations the threaded spindle needs to do in order to displace one meter.
	 * The handedness describes if the spindle is right-hand or left-hand threaded.
	 */
	t_LinearAct *linearActNew (uint8_t stepsPerRev, uint8_t stepMultiplier, uint8_t rotationsPerMeter, enum e_handedness handedness);

	/* Set to mode (A) (see library description) and accelerates the speed to the target value.
	 * The linSpeed is the speed that the motor is expected to reach in m/s. Its sign determines the direction of the movement.
	 * The linAccel is the acceleration rate to reach the target speed in m/s^2. It must be sent in absolute value and must be greater than zero, as the funcion will calculate the sign of the acceleration based on the difference between the current and the target speed.
	 */
	void linearActSetSpeed (t_LinearAct *linearAct, float linSpeed, float linAccel);

	/* Set to mode (B) (see library description) and displaces the given value.
	 * The linDisp is the linear displacement that the motor needs to do in m. Its sign determines the direction of of the movement.
	 * The linSpeed is the speed that the actuator is expected to reach in m/s. Its sign determines the direction of of the movement. Both the linDisp and linSpeed must have the same sign.
	 * The linAccel is the acceleration rate to reach the target speed from rest in m/s^2. It must be sent in absolute value and must be greater than zero, as the funcion will calculate the sign of the acceleration based on the target speed.
	 * The linDeccel is the acceleration rate to reach rest from the target speed in m/s^2. It must be sent in absolute value and must be greater than zero, as the funcion will calculate the sign of the acceleration based on the target speed.
	 */
	void linearActSetDisplacement (t_LinearAct *linearAct, float linDisp, float linSpeed, float linAccel, float linDeccel);

	/* Executes the linear actuator movement.
	 * Calculates if its time to step the stepper motor. If its true, then calls the stepFunc to do the step.
	 * Updates the movement plan and speed.
	 * The time parameter must the the current time of the system in microseconds.
	 * The funcion sent as parameter (stepFunc) should handle the stepper underlying details and do one step when called.
	 * It returns the time delay to the next step in microseconds.
	 * It should be called at small time intervals or right after the step delay is past.
	 */
	uint32_t linearActRun (t_LinearAct *linearAct, uint32_t time, void (*stepFunc) (enum e_StepDirection dir));
	
	/* Checks if the acturator is running.
	 * Returns 1 if its running, 0 otherwise
	 */
	uint8_t linearActIsActive(t_LinearAct *linearAct);

	/* Imediatly stops the linear actuator. The state and move plan will be reseted */
	void linearActStop (t_LinearAct *linearAct);

	/* dealocate a linear actuator */
	void linearActKill (t_LinearAct *linearAct);

#endif

#ifdef __cplusplus
}
#endif