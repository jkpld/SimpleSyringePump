/*
LeadScrew.cpp - - Library for stepper motor with lead screw
*/

#include "Arduino.h"
#include "LeadScrew.h"

// Initializers
LeadScrew::LeadScrew(int mPin1, int mPin2, int mPin3, int mPin4)
{
    distance_per_step = 1;
    init(mPin1, mPin2, mPin3, mPin4);
};

LeadScrew::LeadScrew(float distance_per_step_in, int mPin1, int mPin2, int mPin3, int mPin4)
{
    distance_per_step = distance_per_step_in;
    init(mPin1, mPin2, mPin3, mPin4);
};

//Initialize parameters
void LeadScrew::init(int mPin1, int mPin2, int mPin3, int mPin4)
{
    motor_state = 0;
    speed = 0;
    last_step_time = 0;
    setMotorPins(mPin1, mPin2, mPin3, mPin4);
}

// Set motor pins
void LeadScrew::setMotorPins(int mPin1, int mPin2, int mPin3, int mPin4)
{
    motPin1 = mPin1;
    motPin2 = mPin2;
    motPin3 = mPin3;
    motPin4 = mPin4;

    pinMode(motPin1, OUTPUT);
    pinMode(motPin2, OUTPUT);
    pinMode(motPin3, OUTPUT);
    pinMode(motPin4, OUTPUT);
}

// Speed in distance per minute
void LeadScrew::setSpeed(float distance_per_minute)
{
    // (ms/step) = (dist/step) * (us/min) * (min/dist)
    step_delay = distance_per_step * 60000000.0 / distance_per_minute;
}

// Increment the current step number of the motor
// -- The motor is always in one of 4 possible steps.
//    -- If the program is modified to use half or microstepping then
//       this function should be modified to use the correct number of steps.
void LeadScrew::increment_state(int pm_one)
{
    motor_state += pm_one;
    if (motor_state < 0) {
      motor_state += 4;
    }
    motor_state = motor_state % 4;
}

void LeadScrew::stepMotor()
{
    switch (motor_state) {
      case 0:    // 1010
      digitalWrite(motPin1, HIGH);
      digitalWrite(motPin2, LOW);
      digitalWrite(motPin3, HIGH);
      digitalWrite(motPin4, LOW);
      break;
      case 1:    // 0110
      digitalWrite(motPin1, LOW);
      digitalWrite(motPin2, HIGH);
      digitalWrite(motPin3, HIGH);
      digitalWrite(motPin4, LOW);
      break;
      case 2:    //0101
      digitalWrite(motPin1, LOW);
      digitalWrite(motPin2, HIGH);
      digitalWrite(motPin3, LOW);
      digitalWrite(motPin4, HIGH);
      break;
      case 3:    //1001
      digitalWrite(motPin1, HIGH);
      digitalWrite(motPin2, LOW);
      digitalWrite(motPin3, LOW);
      digitalWrite(motPin4, HIGH);
      break;
    }
}

void LeadScrew::move_step(int steps_to_move)
{
    int direction = (steps_to_move > 0) ? 1 : -1;
    steps_to_move = abs(steps_to_move);  // how many steps to take
    

    // decrement the number of steps, moving one step each time:
    while(steps_to_move > 0) {
        // move only if the appropriate delay has passed:
        unsigned long current_time = micros();
        dt = current_time - last_step_time;
        
        if (dt >= step_delay) {
            // get the timeStamp of when you stepped:
            last_step_time = current_time;
            increment_state(direction);
            stepMotor();
            --steps_to_move;
        }
    }
}

void LeadScrew::move_distance(float distance)
{
    // calculate number of steps
    int steps = compute_num_steps(distance);

    // move specified number of steps.
    move_step(steps);
}

void LeadScrew::disable()
{
    motor_state = 0;
    digitalWrite(motPin1, LOW);
    digitalWrite(motPin2, LOW);
    digitalWrite(motPin3, LOW);
    digitalWrite(motPin4, LOW);
}

int LeadScrew::compute_num_steps(float distance)
{
    // calculate number of steps
    int steps = (int)(distance/distance_per_step + 0.5);
    return steps;
}

int version()
{
    return 1;
}
