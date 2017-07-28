// Syringe pump firmware

#include "LeadScrew.h"
#include "float2s.h"

// Motor pins
byte motPin1 = 6;
byte motPin2 = 4;
byte motPin3 = 5;
byte motPin4 = 3;

// Lead screw parameters
int steps_per_rev = 2053; // [steps/rev]
float thread_pitch = 1.27; // [mm/rev] 1.27 is the pitch of a 1/4-20 thread in mm.
float distance_per_step = thread_pitch / (float)steps_per_rev; // [mm/step]

LeadScrew SyringePump(distance_per_step, motPin1, motPin2, motPin3, motPin4);

String cmdString = ""; // a string to hold incoming data
boolean moveContinuous = false;
int direction = 1;
boolean moveStep = false; // move certain number of steps;
int steps_to_move = 0; // number of steps to be moved
float driveSpeed = 3; // [mm/min] speed of motor
float maxDriveSpeed = 20; // [mm/min]

unsigned long start_time = 0;
unsigned long time_to_complete = 0;

void setup()
{
  Serial.begin(9600);
  delay(50);

  cmdString.reserve(25);
  SyringePump.setSpeed(driveSpeed);
}

void loop()
{
  if (Serial.available() > 0) {
    // get the enw bute and add to cmdString:
    char inChar = (char)Serial.read();
    cmdString += inChar;

    // If inChar is a newline, then process the line
    if (inChar == '\n')
    {
      if (cmdString.startsWith("version")) {
        /*
          Version 1.0: Control using rev/min and steps.
          Version 2.0: Control using mm/min and distance.
        */
        Serial.print("1\n");
        Serial.print("Syringe Pump Driver Version 2.0\n");

      } else if (cmdString.startsWith("connected?")) {
        Serial.print("0\n");

      } else if (cmdString.startsWith("fwd")) {
        // Start moving motor continuously forward
        moveContinuous = true;
        direction = 1;
        steps_to_move = 0;
        Serial.print("0\n");

      } else if (cmdString.startsWith("bkd")) {
        // Start moving motor continuously backward
        moveContinuous = true;
        direction = -1;
        steps_to_move = 0;
        Serial.print("0\n");

      } else if (cmdString.startsWith("mov ")) {
        // Get the distance to move
        char buffer[10];
        cmdString.substring(4).toCharArray(buffer, 10);
        float distance = atof(buffer);

        steps_to_move = SyringePump.compute_num_steps(distance);
        direction = (steps_to_move > 0) ? 1 : -1;
        steps_to_move = abs(steps_to_move);
        moveContinuous = false;

        start_time = millis();
        Serial.print("0\n");

      } else if (cmdString.startsWith("mov_step ")) {
        // Get the steps to move
        steps_to_move = cmdString.substring(9).toInt();
        direction = (steps_to_move > 0) ? 1 : -1;
        steps_to_move = abs(steps_to_move);
        moveContinuous = false;

        start_time = millis();
        Serial.print("0\n");

      } else if (cmdString.startsWith("speed ")) {
        // Get the new set speed
        char buffer[10];
        cmdString.substring(6).toCharArray(buffer, 10);
        driveSpeed = atof(buffer);

        if (driveSpeed < 0) {
          Serial.print("1\n");
          Serial.print("Error! Speed must be positive. Not changing speed.\n");
        } else if (driveSpeed > maxDriveSpeed) {
          Serial.print("1\n");
          Serial.print("Warning! Speed exceeds the maximum allowed, ");
          Serial.print(maxDriveSpeed);
          Serial.print(" mm/min. Setting speed to the maximum allowed value.\n");
          driveSpeed = maxDriveSpeed;
          SyringePump.setSpeed(driveSpeed);
        } else {
          SyringePump.setSpeed(driveSpeed);
          Serial.print("0\n");
        }

      } else if (cmdString.startsWith("stop")) {
        // stop motor motion, but do not disable power from the coils
        steps_to_move = 0;
        moveContinuous = false;
        Serial.print("0\n");

      } else if (cmdString.startsWith("disable")) {
        // stops the motor motion and disables all power from the coils
        SyringePump.disable();

        // Stop the motor motion
        steps_to_move = 0;
        moveContinuous = false;
        Serial.print("0\n");

      } else if (cmdString.startsWith("get maxSpeed")) {
        // return the maximum speed
        Serial.print("2\n");
        Serial.print(float2s(maxDriveSpeed,3));
        Serial.print("\n");
        
      } else if (cmdString.startsWith("get speed")) {
        // return the speed
        Serial.print("2\n");
        Serial.print(float2s(driveSpeed,6));
        Serial.print("\n");
        
      } else if (cmdString.startsWith("get distance_per_step")) {
        // return the distance per step
        Serial.print("2\n");
        Serial.print(float2s(distance_per_step,6));
        Serial.print("\n");
        
      } else if (cmdString.startsWith("set maxSpeed")) {
        // set the maximum speed
        char buffer[10];
        cmdString.substring(13).toCharArray(buffer, 10);
        maxDriveSpeed = atof(buffer);
        Serial.print("0\n");
        
      } else if (cmdString.startsWith("get time_to_complete")) {
        // return the distance per step
        Serial.print("2\n");
        Serial.print(time_to_complete);
        Serial.print("\n");
        
      } else {
        Serial.print("1\n");
        Serial.print("Error! Unknown command.\n");
      }

      cmdString = "";
    }
  }

  if (moveContinuous) {
    SyringePump.move_step(direction);
  } else if (steps_to_move > 0) {
    SyringePump.move_step(direction);
    --steps_to_move;
    if (steps_to_move == 0) {
      time_to_complete = millis() - start_time;
    }
  }
}
