#include  "ame.h"

float currentPos[3],
      newPos[3],
      dCurrentPos[3],
      dCurrentPosAbs,
      dCurrentPosVector[3],
      feedrate,
      dOffset[3],
      predefinedPositions[2][3];

CoordinateSystem CS[MAX_CS];
int selectedCS = 0;
int selectedPlane = XY;    //  Default is the XY plane

int stepperCounter[3];
int executing = 0;

unsigned  long  time, timerX = 0, timerY = 0, timerZ = 0;

int  minStepMsX = 0,
     minStepMsY = 0,
     minStepMsZ = 0;

extern  int execIndex;
extern  GParam GParams[GCODE_BUFFER];
extern  int bufferSize;
extern  int execIndex;
extern float conversionFactor;

int  conditions[3];

void updateMovement()
{
  if(bufferSize > 0)
  {
    switch(GParams[execIndex].GValue)
    {
      case 0:
        G00();
      break;
      case 1:
        G01();
      break;
      case 4:
        G04();
      break;
      case 10:
        G10();
      break;
      case 17: case 18: case 19:
        selectPlane();
      break;
      case 20: case 21:
        setConversionFactor();
      break;
      case 28:
        predefinedPos(0);
      break;
      case 30:
        predefinedPos(1);
      break;
      case 54: case 55: case 56: case 57: case 58: case 59:
        selectCoordinateSystem();
      break;
      default:
      break; 
    }
  }
}

void G00()
{
  initiatializeLinearMotion(MAX_FEEDRATE);
  checkEndStops();
  linearMotion();
}

void G01()
{
  initiatializeLinearMotion(feedrate);
  checkEndStops();
  linearMotion();
}

void G04()
{
  initializeWaiting();
  wait();
}

void G10()
{
  switch(GParams[execIndex].LValue)
  {
    case 2:     //  Relative offset, yes that is correct. The current offset is moved by the given values.
      if(GParams[execIndex].XDefined)
      {
        dOffset[0] = GParams[execIndex].XValue;
      }
      else
      {
        dOffset[0] = 0.0;
      }
      if(GParams[execIndex].YDefined)
      {
        dOffset[1] = GParams[execIndex].YValue;
      }
      else
      {
        dOffset[1] = 0.0;
      }
      if(GParams[execIndex].ZDefined)
      {
        dOffset[2] = GParams[execIndex].ZValue;
      }
      else
      {
        dOffset[2] = 0.0;
      }
    break;
    case 20:    //  Absolute offset
      if(GParams[execIndex].XDefined)
      {
        dOffset[0] = GParams[execIndex].XValue - CS[GParams[execIndex].PValue].XValue;
      }
      else
      {
        dOffset[0] = 0.0;
      }
      if(GParams[execIndex].YDefined)
      {
        dOffset[1] = GParams[execIndex].YValue - CS[GParams[execIndex].PValue].YValue;
      }
      else
      {
        dOffset[1] = 0.0;
      }
      if(GParams[execIndex].ZDefined)
      {
        dOffset[2] = GParams[execIndex].ZValue - CS[GParams[execIndex].PValue].ZValue;
      }
      else
      {
        dOffset[2] = 0.0;
      }      
    break;
    default:    //  Other cases than 2 and 20 are filtered out in gcode.cpp and can never occur here
    break;
  }
  Serial.println(dOffset[0]);
  Serial.println(dOffset[1]);
  Serial.println(dOffset[2]);
  relativeOffsetCS();
  deInitialize();
}

void checkEndStops()
{
  switch(GParams[execIndex].SValue != 0)
  {
    case 0:
    break;
    case 1: case 2:
      if(digitalRead(ENDSTOP_X) == HIGH | digitalRead(ENDSTOP_Y) == HIGH | digitalRead(ENDSTOP_Z) == HIGH)
      {
        Serial.println("Tooling was interrupted, endstop was hit. Reset arduino.");
        for( ; ; );  //  Lets the program crash
      }
    break;
    default:
    break; 
  }
}

void linearMotion()
{
  time = micros();
  conditions[0] = abs(GParams[execIndex].XValue - currentPos[0]) > X_TOLERANCE;
  conditions[1] = abs(GParams[execIndex].YValue - currentPos[1]) > Y_TOLERANCE;
  conditions[2] = abs(GParams[execIndex].ZValue - currentPos[2]) > Z_TOLERANCE;
  
  if(conditions[0])
  {
    if(time - timerX >= minStepMsX)
    {
      if(GParams[execIndex].XValue < currentPos[0])
      {
        pulseX(-1);
      }
      else
      {
        pulseX(1);
      }
      timerX += minStepMsX;
    } 
  }
  if(conditions[1])
  {
    if(time - timerY >= minStepMsY)
    {
      if(GParams[execIndex].YValue < currentPos[1])
      {
        pulseY(-1); 
      }
      else
      {
        pulseY(1); 
      }
      timerY += minStepMsY;
    }
  }
  if(conditions[2])
  {
    if(time - timerZ >= minStepMsZ)
    {
      if(GParams[execIndex].ZValue < currentPos[2])
      {
        pulseZ(-1); 
      }
      else
      {
        pulseZ(1); 
      }
      timerZ += minStepMsZ;
    }
  }
  if(!conditions[0] && !conditions[1] && !conditions[2])
  {
    deInitialize();
  } 
}

void wait()
{
  if(micros() - time >= GParams[execIndex].PValue)
  {
    deInitialize(); 
  }
}

void relativeOffsetCS()
{
  CS[GParams[execIndex].PValue].XValue += dOffset[0];
  CS[GParams[execIndex].PValue].YValue += dOffset[1];
  CS[GParams[execIndex].PValue].ZValue += dOffset[2];
  if(selectedCS == GParams[execIndex].PValue)
  {
    currentPos[0] += dOffset[0];
    currentPos[1] += dOffset[1];
    currentPos[2] += dOffset[2];
  }
}

void selectCoordinateSystem()
{
  currentPos[0] += CS[GParams[execIndex].PValue].XValue - CS[selectedCS].XValue;
  currentPos[1] += CS[GParams[execIndex].PValue].YValue - CS[selectedCS].YValue;
  currentPos[2] += CS[GParams[execIndex].PValue].ZValue - CS[selectedCS].ZValue;
  selectedCS = GParams[execIndex].PValue;
  deInitialize();
}

void selectPlane()
{
  selectedPlane = GParams[execIndex].PValue;
  deInitialize();
}

void setConversionFactor()
{
  switch(GParams[execIndex].GValue)
  {
    case 20:
      conversionFactor = INCH_TO_MM;
    break;
    case 21:
      conversionFactor = 1.0;
    break;
    default:
    break; 
  }
  deInitialize();
}

void predefinedPos(int i)    //  Error if Cutter Compensation is turned on still has to be implemented
{
  if(GParams[execIndex].subG == 1)
  {
    predefinedPositions[i][0] = currentPos[0];
    predefinedPositions[i][1] = currentPos[1];
    predefinedPositions[i][2] = currentPos[2];
    deInitialize();
    return;
  }
  else
  {
    dCurrentPos[0] = predefinedPositions[i][0] - currentPos[0] + GParams[execIndex].XValue;
    dCurrentPos[1] = predefinedPositions[i][1] - currentPos[1] + GParams[execIndex].YValue;
    dCurrentPos[2] = predefinedPositions[i][2] - currentPos[2] + GParams[execIndex].ZValue;
    GParams[execIndex].XValue = predefinedPositions[i][0] + GParams[execIndex].XValue;      //  This could be done more efficiently by using dCurrentPos in linearMotion to count down from
    GParams[execIndex].YValue = predefinedPositions[i][1] + GParams[execIndex].YValue;
    GParams[execIndex].ZValue = predefinedPositions[i][2] + GParams[execIndex].ZValue;
    GParams[execIndex].GValue = 0;
    Serial.println(dCurrentPos[0]);
    startLinearMotion(MAX_FEEDRATE);
  }
}

void initiatializeLinearMotion(float inputFeedrate)
{
  if(executing == 0)
  {
    calculateDistanceVector();
    startLinearMotion(inputFeedrate);
  }
}

void calculateDistanceVector()
{
  Serial.println("initializing");
  if(GParams[execIndex].XDefined)
  {
    dCurrentPos[0] = GParams[execIndex].XValue - currentPos[0];
  }
  else
  {
     dCurrentPos[0] = 0.0;
  }
  if(GParams[execIndex].YDefined)
  {
    dCurrentPos[1] = GParams[execIndex].YValue - currentPos[1];
  }
  else
  {
     dCurrentPos[1] = 0.0;
  }
  if(GParams[execIndex].ZDefined)
  {
    dCurrentPos[2] = GParams[execIndex].ZValue - currentPos[2];
  }
  else
  {
     dCurrentPos[2] = 0.0;
  }
}

void startLinearMotion(float inputFeedrate)
{
  dCurrentPosAbs = sqrt(dCurrentPos[0] * dCurrentPos[0] + dCurrentPos[1] * dCurrentPos[1] + dCurrentPos[2] * dCurrentPos[2]);
  dCurrentPosVector[0] = abs(dCurrentPos[0]) / dCurrentPosAbs;
  dCurrentPosVector[1] = abs(dCurrentPos[1]) / dCurrentPosAbs;
  dCurrentPosVector[2] = abs(dCurrentPos[2]) / dCurrentPosAbs;
  minStepMsX = round(MM_TO_STEPS_X / (inputFeedrate * dCurrentPosVector[0]));
  minStepMsY = round(MM_TO_STEPS_Y / (inputFeedrate * dCurrentPosVector[1]));
  minStepMsZ = round(MM_TO_STEPS_Z / (inputFeedrate * dCurrentPosVector[2]));
  executing = 1;
  timerX = micros();
  timerY = micros();
  timerZ = micros(); 
}

void initializeWaiting()
{
  if(executing == 0)
  {
    time = micros();
    executing = 1;
  }
}

void deInitialize()
{
  executing = 0;
  Serial.println("executed");
  fifoDelete();
}

void pulseX(int dir)
{
  stepperCounter[0] += dir;
  if(stepperCounter[0] < 0)
  {
    stepperCounter[0] = 3; 
  }
  else if(stepperCounter[0] > 3)
  {
    stepperCounter[0] = 0; 
  }
  
  switch(stepperCounter[0])
  {
    case 0:
      digitalWrite(MOTORX1, HIGH);
      digitalWrite(MOTORX2, HIGH);
      digitalWrite(MOTORX3, LOW);
      digitalWrite(MOTORX4, LOW);
    break;
    case 1:
      digitalWrite(MOTORX1, LOW);
      digitalWrite(MOTORX2, HIGH);
      digitalWrite(MOTORX3, HIGH);
      digitalWrite(MOTORX4, LOW);
    break;
    case 2:
      digitalWrite(MOTORX1, LOW);
      digitalWrite(MOTORX2, LOW);
      digitalWrite(MOTORX3, HIGH);
      digitalWrite(MOTORX4, HIGH);
    break;
    case 3:
      digitalWrite(MOTORX1, HIGH);
      digitalWrite(MOTORX2, LOW);
      digitalWrite(MOTORX3, LOW);
      digitalWrite(MOTORX4, HIGH);
    break;
    default:
    break; 
  }
  
  currentPos[0] += dir * MM_TO_STEPS_X;
}

void pulseY(int dir)
{
  stepperCounter[1] += dir;
  if(stepperCounter[1] < 0)
  {
    stepperCounter[1] = 3; 
  }
  else if(stepperCounter[1] > 3)
  {
    stepperCounter[1] = 0; 
  }
  
  switch(stepperCounter[1])
  {
    case 0:
      digitalWrite(MOTORY1, HIGH);
      digitalWrite(MOTORY2, HIGH);
      digitalWrite(MOTORY3, LOW);
      digitalWrite(MOTORY4, LOW);
    break;
    case 1:
      digitalWrite(MOTORY1, LOW);
      digitalWrite(MOTORY2, HIGH);
      digitalWrite(MOTORY3, HIGH);
      digitalWrite(MOTORY4, LOW);
    break;
    case 2:
      digitalWrite(MOTORY1, LOW);
      digitalWrite(MOTORY2, LOW);
      digitalWrite(MOTORY3, HIGH);
      digitalWrite(MOTORY4, HIGH);
    break;
    case 3:
      digitalWrite(MOTORY1, HIGH);
      digitalWrite(MOTORY2, LOW);
      digitalWrite(MOTORY3, LOW);
      digitalWrite(MOTORY4, HIGH);
    break;
    default:
    break; 
  }
  
  currentPos[1] += dir * MM_TO_STEPS_Y;
}

void pulseZ(int dir)
{
  stepperCounter[2] += dir;
  if(stepperCounter[2] < 0)
  {
    stepperCounter[2] = 3; 
  }
  else if(stepperCounter[2] > 3)
  {
    stepperCounter[2] = 0; 
  }
  
  switch(stepperCounter[2])
  {
    case 0:
      digitalWrite(MOTORZ1, HIGH);
      digitalWrite(MOTORZ2, HIGH);
      digitalWrite(MOTORZ3, LOW);
      digitalWrite(MOTORZ4, LOW);
    break;
    case 1:
      digitalWrite(MOTORZ1, LOW);
      digitalWrite(MOTORZ2, HIGH);
      digitalWrite(MOTORZ3, HIGH);
      digitalWrite(MOTORZ4, LOW);
    break;
    case 2:
      digitalWrite(MOTORZ1, LOW);
      digitalWrite(MOTORZ2, LOW);
      digitalWrite(MOTORZ3, HIGH);
      digitalWrite(MOTORZ4, HIGH);
    break;
    case 3:
      digitalWrite(MOTORZ1, HIGH);
      digitalWrite(MOTORZ2, LOW);
      digitalWrite(MOTORZ3, LOW);
      digitalWrite(MOTORZ4, HIGH);
    break;
    default:
    break; 
  }
  
  currentPos[2] += dir * MM_TO_STEPS_Z;
}
