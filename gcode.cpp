#include  "ame.h"

String  inParams[MAX_COMMAND_PARAMS];
GParam  GParams[GCODE_BUFFER];
char    c;
int     paramCount = 0,
        reading = 1,
        i = 0,
        index = 0;    //  Used to find subG

unsigned  long  tmp;

extern int bufferSize, bufferIndex;
extern float feedrate, currentPos[3];

float  conversionFactor = 1.0;    //  1.0 is for mm, which is default. When units of inches are selected with G20, this value will be set to INCH_TO_MM. Motion.cpp will ALWAYS receive units in mm.

int started = 0;

void readCommand()
{
  while (Serial.available() > 0 && bufferSize < GCODE_BUFFER)
  {
    if(started == 0)
    {
      started = 1;
      tmp = micros();
    }
    c = Serial.read();
    if(c == ' ' && reading == 1)
    {
      reading = 0;
    }
    else if(c == '\n')
    {
      paramCount++;
      interpretCommand();
      Serial.println("ok");
      Serial.println(micros() - tmp);
      started = 0;
      break;
    }
    else if((c == 'G' || c == 'g') & reading == 0)
    {
      Serial.println('g');
      paramCount++;
      interpretCommand();
      Serial.println("ok");
      Serial.println(micros() - tmp);
      
      inParams[paramCount] += c;
    }
    else if(reading != ' ')
    {
      if(reading == 0)
      {
        paramCount++;
        reading = 1;
      }
      inParams[paramCount] += c;
    }
  }
}

void interpretCommand()
{
  if(inParams[0][0] == 'G' || inParams[0][0] == 'g')
  {
    getParams();
    switch(GParams[bufferIndex].GValue)
    {
      case 0:
        G00i();
        fifoAdd();
      break;
      case 1:
        G01i();
        fifoAdd();
      break;
      case 4:
        G04i();
        fifoAdd();
      case 10:
        if(G10i())
        {
          fifoAdd();
        }
      break;
      case 17: case 18: case 19:
        G17to19i();
        fifoAdd();
      break;
      case 20: case 21:
        fifoAdd();
      break;
      case 28: case 30:
        fifoAdd();
      break;
      case 54: case 55: case 56: case 57: case 58: case 59:
        G54to59i();
        fifoAdd();
      break;
      default:
        GParams[bufferIndex] = {};
      break; 
    }
  }
  
  for(i = 0; i < paramCount; i++)
  {
    inParams[i] = "";
  }
  
  paramCount = 0;
  reading = 1;
  i = 0;
}

void getParams()
{  
  for(i = 0; i < paramCount; i++)
  {
    switch(inParams[i][0])
    {
      case '*':
        if(inParams[i][1] == 'N' | inParams[i][1] == 'n')
        {
          GParams[bufferIndex].nValue = inParams[i].substring(2).toFloat();
        }
      break;
      case 'N': case 'n':
        GParams[bufferIndex].NValue = inParams[i].substring(1).toInt();
      break;
      case 'E': case 'e':
        GParams[bufferIndex].EValue = inParams[i].substring(1).toFloat();
      break;
      case 'Q': case 'q':
        GParams[bufferIndex].QValue = inParams[i].substring(1).toInt();
      break;
      case 'R': case 'r':
        GParams[bufferIndex].RValue = inParams[i].substring(1).toFloat();
      break;
      case 'F': case 'f':
        GParams[bufferIndex].FValue = inParams[i].substring(1).toFloat();
      break;
      case 'H': case 'h':
        GParams[bufferIndex].HValue = inParams[i].substring(1).toFloat();
      break;
      case 'D': case 'd':
        GParams[bufferIndex].DValue = inParams[i].substring(1).toFloat();
      break;
      case 'J': case 'j':
        GParams[bufferIndex].JValue = inParams[i].substring(1).toFloat();
      break;
      case 'I': case 'i':
        GParams[bufferIndex].IValue = inParams[i].substring(1).toFloat();
      break;    
      case 'Z': case 'z':
        GParams[bufferIndex].ZValue = inParams[i].substring(1).toFloat();
        GParams[bufferIndex].ZDefined = true;
      break;
      case 'Y': case 'y':
        GParams[bufferIndex].YValue = inParams[i].substring(1).toFloat();
        GParams[bufferIndex].YDefined = true;
      break;
      case 'X': case 'x':
        GParams[bufferIndex].XValue = inParams[i].substring(1).toFloat();
        GParams[bufferIndex].XDefined = true;
      break;
      case 'L': case 'l':
        GParams[bufferIndex].LValue = inParams[i].substring(1).toInt();
      break;
      case 'P': case 'p':
        GParams[bufferIndex].PValue = inParams[i].substring(1).toInt();
      break;
      case 'S': case 's':
        GParams[bufferIndex].SValue = inParams[i].substring(1).toInt();
      break;
      case 'T': case 't':    
        GParams[bufferIndex].TValue = inParams[i].substring(1).toInt();
      break;
      case 'M': case 'm':    
        GParams[bufferIndex].MValue = inParams[i].substring(1).toInt();
      break;
      case 'G': case 'g':
        GParams[bufferIndex].GValue = inParams[i].substring(1).toInt();
        index = inParams[i].indexOf('.');
        if(index > 0)
        {
          GParams[bufferIndex].subG = inParams[i].substring(index + 1).toInt();
        }
      break;
      default:
      break;
    }
  }
}

void G00i()
{
  adjustFeedrate();
  adjustForUnits();
}

void G01i()
{
  adjustFeedrate();
  adjustForUnits();
}

void G04i()
{
  getWaitingTime();
}

bool G10i()
{
  switch(GParams[bufferIndex].LValue)
  {
    case 2: case 20:    //  If no tool/coordinate system is (PValue) is given in the gcode, PValue will default to 0
      adjustForUnits();
      return true;
    break;
    default:
      GParams[bufferIndex] = {};
      return false;
    break;
  }
}

void G17to19i()
{
  switch(GParams[bufferIndex].GValue)
  {
    case 17:
      GParams[bufferIndex].PValue = XY;
    break;
    case 18:
      GParams[bufferIndex].PValue = ZX;
    break;
    case 19:
      GParams[bufferIndex].PValue = YZ;
    break;
    default:
    break; 
  }
}

void G54to59i()    //  Choose coordinate system (the only difference between them all is the offsets in xyz, they're all cartesian)
{
  GParams[bufferIndex].PValue = GParams[bufferIndex].GValue - 54;  //  This way G54 corresponds to coordinate system 0, G55 to 1, etcetera
}

void adjustFeedrate()
{
  if(GParams[bufferIndex].FValue == 0.0)
  {
    if(feedrate == 0.0)
    {
      feedrate = DEFAULT_FEEDRATE; 
    }
    return;
  }
  else if(GParams[bufferIndex].FValue > MAX_FEEDRATE_MIN * conversionFactor)
  {
    feedrate = MAX_FEEDRATE * conversionFactor;    //  In the case where units of inches were chosen, the feedrate is converted from [inch per minute] to [mm per minute]
    return;
  }
  feedrate = GParams[bufferIndex].FValue / 60000000.0;  //  Converted from [mm per minute] to [mm per microsecond]
}

void getWaitingTime()
{
  if(GParams[bufferIndex].PValue > 0)
  {
    GParams[bufferIndex].PValue *= 1000;  // Converting to microseconds
  }
  else
  {
     GParams[bufferIndex].PValue = GParams[bufferIndex].SValue * 1000000;
  }
  Serial.println(GParams[bufferIndex].PValue);
}

void adjustForUnits()
{
  GParams[bufferIndex].XValue *= conversionFactor;
  GParams[bufferIndex].YValue *= conversionFactor;
  GParams[bufferIndex].ZValue *= conversionFactor;
}
