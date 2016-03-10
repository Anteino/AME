#ifndef  gcode_h
  #define  gcode_h
  
  void readCommand();
  void interpretCommand();
  void getParams();
  
  void G00i();  //  The i is for interpret, because I need G00() in motion.cpp
  void G01i();
  void G04i();
  bool G10i();
  void G17to19i();  //  Handles G17, G18 and G19
  void G54to59i();  //  Handles G54, G55, G56, G57, G58 and G59
  
  void adjustFeedrate();
  void getWaitingTime();
  void adjustForUnits();    //  Makes sure the numbers are correctly converted to mm to be used in motion.cpp
  
  typedef struct gparam
  {
    //  [Input]  These parameters are read from the gcode input over serial
    int GValue;
    int subG;    //  If a command is in the form of GXX.Y, the value of Y is stored in subG
    int MValue;
    int TValue;
    int SValue;
    unsigned long PValue;
    int LValue;
    float XValue;
    float YValue;
    float ZValue;
    float IValue;
    float JValue;
    float DValue;
    float HValue;
    float FValue;  //  This is the feedrate, provided in mm/min. Before motion.cpp handles it, it's converted to mm/ms.
    float RValue;
    int QValue;
    float EValue;
    int NValue;
    float nValue;
    
    //  [Calculated]  These parameters are calculated in the code according to the values of the previous variables
    bool XDefined = false;  //  When an entry is reset with GParams[i] = {}, false is automatically set
    bool YDefined = false;
    bool ZDefined = false;
  } GParam;
#endif
