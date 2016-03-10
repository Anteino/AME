#include  "ame.h"

void setup()
{
  ameSetup();
}

void loop()
{
  readCommand();
  updateMovement();
}
