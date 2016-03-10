//    Motor pins
#define  MOTORX1             10
#define  MOTORX2             11
#define  MOTORX3             12
#define  MOTORX4             13

#define  MOTORY1             6
#define  MOTORY2             7
#define  MOTORY3             8
#define  MOTORY4             9

#define  MOTORZ1             2
#define  MOTORZ2             3
#define  MOTORZ3             4
#define  MOTORZ4             5

//      Sensor pins
#define  ENDSTOP_X           14
#define  ENDSTOP_Y           15
#define  ENDSTOP_Z           16

//	Constants
#define  BAUD_RATE           115200

#define  MAX_COMMAND_PARAMS  19        //  Change this when a parameter is added to gparam in gcode.h before the [calculated] label
#define  GCODE_BUFFER        20

#define  STEPS_TO_MM_X       102.4
#define  STEPS_TO_MM_Y       102.4
#define  STEPS_TO_MM_Z       102.4

#define  MM_TO_STEPS_X       1.0 / STEPS_TO_MM_X
#define  MM_TO_STEPS_Y       1.0 / STEPS_TO_MM_Y
#define  MM_TO_STEPS_Z       1.0 / STEPS_TO_MM_Z

#define  INCH_TO_MM          25.4

#define  X_TOLERANCE         0.02
#define  Y_TOLERANCE         0.02
#define  Z_TOLERANCE         0.02

#define  DEFAULT_FEEDRATE   4.5 * 60.0 / 60000000.0
#define  MAX_FEEDRATE       5.0 * 60.0 / 60000000.0
#define  MAX_FEEDRATE_MIN   60000000.0 * MAX_FEEDRATE

#define  MAX_VALUE_MICROS   4294967296

#define  MAX_CS             6      //  Amount of coordinate systems
