#include  "ame.h"

extern  float   currentPos[3],
                newPos[3],
                dCurrentPos[3];

extern  int  stepperCounter[3];

void ameSetup()
{
  Serial.begin(BAUD_RATE);
  
  pinMode(MOTORX1, OUTPUT);
  pinMode(MOTORX2, OUTPUT);
  pinMode(MOTORX3, OUTPUT);
  pinMode(MOTORX4, OUTPUT);
  
  pinMode(MOTORY1, OUTPUT);
  pinMode(MOTORY2, OUTPUT);
  pinMode(MOTORY3, OUTPUT);
  pinMode(MOTORY4, OUTPUT);
  
  pinMode(MOTORZ1, OUTPUT);
  pinMode(MOTORZ2, OUTPUT);
  pinMode(MOTORZ3, OUTPUT);
  pinMode(MOTORZ4, OUTPUT);
  
  pinMode(ENDSTOP_X, INPUT);
  pinMode(ENDSTOP_Y, INPUT);
  pinMode(ENDSTOP_Z, INPUT);
  
  currentPos[0] = 0.0;
  currentPos[1] = 0.0;
  currentPos[2] = 0.0;
  
  dCurrentPos[0] = 0.0;
  dCurrentPos[1] = 0.0;
  dCurrentPos[2] = 0.0;
  
  newPos[0] = 0.0;
  newPos[1] = 0.0;
  newPos[2] = 0.0;
  
  stepperCounter[0] = 0;
  stepperCounter[1] = 0;
  stepperCounter[2] = 0;
}
