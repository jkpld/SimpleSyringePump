/*
Linear Drive Class declaration.

A linear drive will be a stepper motor with a lead screw.
This class will contain the step/distance calibration as well as methods for
moving the stepper motor specific distances.
*/

#ifndef LeadScrew_h
#define LeadScrew_h

class LeadScrew {
public:
    // Constructor
    // @param distance_per_step: the distance traveled per step
    // @param motor_pin_1-4: pin numbers for the motor.
    LeadScrew(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4);
    LeadScrew(float distance_per_step, int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4);

    // Method to set the motor speed in distance/time
    void setSpeed(float speed_in);
    // Method to set the motor distance per step
    void setDistancePerStep(float distance_per_step);

    // Methods for moving the motor
    // @param distance_to_move: the (signed) distance to move (This is just a
    // @param steps_to_move: the (signed) number of steps to move
    void move_distance(float distance_to_move);
    void move_step(int steps_to_move);
    void disable();
    int compute_num_steps(float distance);

    // Nothing
    int version();
    unsigned long step_delay;
    unsigned long last_step_time;

private:
    void init(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4);
    void setMotorPins(int motor_pin_1, int motor_pin_2, int motor_pin_3, int motor_pin_4);
    void stepMotor();
    void increment_state(int plus_minus_one);

    float distance_per_step;
    float speed;
    unsigned long dt;
    
    int motor_state;

    int motPin1;
    int motPin2;
    int motPin3;
    int motPin4;
};


#endif
