#ifndef  motion_h
  #define  motion_h
  
  void updateMovement();
  
  void G00();
  void G01();
  void G04();
  void G10();
  
  void checkEndStops();
  void linearMotion();
  void wait();
  void relativeOffsetCS();  //  Offsets the coordinate system CS[GParams[execIndex].PValue] with the values of dOffset[]
  void selectCoordinateSystem();  //  When using multiple tools this will select the coordinate system corresponding to the tool specified in PValue, which is in turn extracted from G54 - G59
  void selectPlane();
  void setConversionFactor();
  void predefinedPos(int i);
  
  void initiatializeLinearMotion(float inputFeedrate);
  void calculateDistanceVector();
  void startLinearMotion(float inputFeedrate);
  void initializeWaiting();
  void deInitialize();
  
  void pulseX(int dir);
  void pulseY(int dir);
  void pulseZ(int dir);
  
  void moveX();
  void moveY();
  void moveZ();
  
  typedef struct coordinateSystem
  {
    float XValue = 0.0;
    float YValue = 0.0;
    float ZValue = 0.0;
  } CoordinateSystem;
  
  typedef enum planes
  {
    XY,
    ZX,
    YZ
  } Planes;
#endif
