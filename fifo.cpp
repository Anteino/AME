#include  "ame.h"

int  bufferSize = 0,
     execIndex = 0,
     bufferIndex = 0;

extern  GParam  GParams[GCODE_BUFFER];

void fifoAdd()
{
  bufferIndex++;
  if(bufferIndex >= GCODE_BUFFER)
  {
    bufferIndex = 0; 
  }
  bufferSize++;
}

void fifoDelete()
{
  GParams[execIndex] = {};
  execIndex++;
  if(execIndex >= GCODE_BUFFER)
  {
    execIndex = 0; 
  }
  bufferSize--;
}
