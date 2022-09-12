/**
 * Kuttercraft 3D Printer Firmware
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 - 2016 Alberto Cotronei @MagoKimbra
 * Copyri0ght (C) 2017 Kuttercraft Kuttercraft@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * Configuration_Cartesian.h
 *
 * This configuration file contains mechanism settings for cartesian printer.
 *
 * - Machine name
 * - Endstop pullup resistors
 * - Endstops logic
 * - Z probe Options
 * - Endstops min or max
 * - Min Z height for homing
 * - Stepper enable logic
 * - Stepper step logic
 * - Stepper direction
 * - Disables axis
 * - Travel limits
 * - Axis relative mode
 * - Mesh Bed Leveling (MBL)
 * - Auto Bed Leveling (ABL)
 * - Safe Z homing
 * - Manual home positions
 * - Axis steps per unit
 * - Axis feedrate
 * - Axis accelleration
 * - Homing feedrate
 * - Hotend offset
 * - Cartesian Correction
 *
 * Basic-settings can be found in Configuration_Basic.h
 * Temperature-settings can be found in Configuration_Temperature.h
 * Feature-settings can be found in Configuration_Feature.h
 * Pins-settings can be found in "Configuration_Pins.h"
 */

#ifndef CONFIGURATION_MECHANISM
#define CONFIGURATION_MECHANISM

#define KNOWN_MECH

/*****************************************************************************************
 *********************************** Nombre de maquina ***********************************
 *****************************************************************************************
 *                                                                                       *
 * This to set a custom name for your generic Mendel.                                    *
 * Displayed in the LCD "Ready" message.                                                 *
 *                                                                                       *
 *****************************************************************************************/
#define CUSTOM_MACHINE_NAME "Kuttercraft"
/*****************************************************************************************/

/*****************************************************************************************
 *********************************** Selector de filamento *************************************
 *****************************************************************************************/
 #define KUTTERCRAFT_MULTIFILAMENT      false




/*****************************************************************************************
 *********************************** tipo de maquina *************************************
 *****************************************************************************************
 *                                                                                       *
 * Elegir el tipo de maquina (Solo uno puede estar activo)                               *
 *                                                                                       *
 *****************************************************************************************/
#define PK3             false
#define PK3_PLUS_PLUS   false
#define PK3_EXTEN       false
#define PK2             false
#define PK2_PLUS_PLUS   false
#define PK1             false
#define MK_TITAN         true

#define ESPESIAL        false

#define LCD             true
#define TACTIL          false
#define OLED            false

#define SENSOR_INDUCTIVO_NUEVO  true






















///////////////////////////////////
#define TIENE_SENSOR_INDUCTIVO true
//esto es para unir verciones



#if(PK3)
  #define SI_ES_UNA_PK3 true
#endif


#define SENSOR_DE_FILAMENTO
/*****************************************************************************************
 ******************************** Tiene Sensor Inductivo *********************************
 *****************************************************************************************
 *                                                                                       *
 * Elegir el tipo de pantalla (Solo uno puede estar activo)                               *
 *                                                                                       *
 *****************************************************************************************/
#if(TIENE_SENSOR_INDUCTIVO)

  #define CON_SENSOR_INDUCTIVO  true

  #define EXPERIMENTAL          true

#else

  #define CON_SENSOR_INDUCTIVO  false

  #define EXPERIMENTAL          false

#endif

#define GUARDAR               true

#define AUTO_TEMP_K           true

#define AVISO_DE_CORTE_DE_LUZ true

//#define INVERT_ROTARY_SWITCH
#define SENSOR_FILAMENT

/*****************************************************************************************
 *********************************** tipo de pantalla ************************************
 *****************************************************************************************
 *                                                                                       *
 * Elegir el tipo de pantalla (Solo uno puede estar activo)                               *
 *                                                                                       *
 *****************************************************************************************/



/*****************************************************************************************/

/*****************************************************************************************
 ************************* Endstop pullup resistors ***** *********************************
 *****************************************************************************************
 *                                                                                       *
 * Comment this out (using // at the start of the line) to                               *
 * disable the endstop pullup resistors                                                  *
 *                                                                                       *
 *****************************************************************************************/
 #define ENDSTOPPULLUPS
#if DISABLED(ENDSTOPPULLUPS)
// fine endstop settings: Individual pullups. will be ignored if ENDSTOPPULLUPS is defined
//#define ENDSTOPPULLUP_XMIN
//#define ENDSTOPPULLUP_YMIN
//#define ENDSTOPPULLUP_ZMIN
//#define ENDSTOPPULLUP_Z2MIN
//#define ENDSTOPPULLUP_XMAX
//#define ENDSTOPPULLUP_YMAX
//#define ENDSTOPPULLUP_ZMAX
//#define ENDSTOPPULLUP_Z2MAX
//#define ENDSTOPPULLUP_ZPROBE
#define ENDSTOPPULLUP_EMIN
#endif
/*****************************************************************************************/


/*****************************************************************************************
 ************************************ Endstops logic *************************************
 *****************************************************************************************
 *                                                                                       *
 * Mechanical endstop with COM to ground and NC to Signal                                *
 * uses "false" here (most common setup).                                                *
 *                                                                                       *
 *****************************************************************************************/
#define X_MIN_ENDSTOP_LOGIC   true   // set to true to invert the logic of the endstop.
#define Y_MIN_ENDSTOP_LOGIC   true   // set to true to invert the logic of the endstop.
#define Z_MIN_ENDSTOP_LOGIC   true   // set to true to invert the logic of the endstop.
#define Z2_MIN_ENDSTOP_LOGIC  true   // set to true to invert the logic of the endstop.
#define X_MAX_ENDSTOP_LOGIC   true   // set to true to invert the logic of the endstop.
#define Y_MAX_ENDSTOP_LOGIC   true   // set to true to invert the logic of the endstop.
#define Z_MAX_ENDSTOP_LOGIC   true   // set to true to invert the logic of the endstop.
#define Z2_MAX_ENDSTOP_LOGIC  true   // set to true to invert the logic of the endstop.
#define Z_PROBE_ENDSTOP_LOGIC true   // set to true to invert the logic of the endstop.
#define E_MIN_ENDSTOP_LOGIC   true   // set to true to invert the logic of the endstop.
/*****************************************************************************************/


/*****************************************************************************************
 ******************************* Z probe Options *****************************************
 *****************************************************************************************
 *                                                                                       *
 * Probes are sensors/switches that need to be activated before they can be used         *
 * and deactivated after their use.                                                      *
 * Servo Probes, Z Sled Probe, Fix mounted Probe, etc.                                   *
 * You must activate one of these to use AUTO BED LEVELING FEATURE below.                *
 *                                                                                       *
 * If you want to still use the Z min endstop for homing,                                *
 * disable Z SAFE HOMING.                                                                *
 * Eg: to park the head outside the bed area when homing with G28.                       *
 *                                                                                       *
 * WARNING: The Z MIN endstop will need to set properly as it would                      *
 * without a Z PROBE to prevent head crashes and premature stopping                      *
 * during a print.                                                                       *
 * To use a separte Z PROBE endstop, you must have a Z PROBE PIN                         *
 * defined in the Configuration_Pins.h file for your control board.                      *
 *                                                                                       *
 * Use M666 P to set the Z probe vertical offset from the nozzle. Store with M500.       *
 * WARNING: Setting the wrong pin may have unexpected and potentially                    *
 * disastrous outcomes. Use with caution and do your homework.                           *
 *                                                                                       *
 *****************************************************************************************/
// Z Servo Endstop
// Remember active servos in Configuration_Feature.h
// Define nr servo for endstop -1 not define. Servo index start 0
//#define Z_ENDSTOP_SERVO_NR -1
//#define Z_ENDSTOP_SERVO_ANGLES {90,0} // Z Servo Deploy and Stow angles

// A Fix-Mounted Probe either doesn't deploy or needs manual deployment.
// For example an inductive probe, or a setup that uses the nozzle to probe.
// An inductive probe must be deactivated to go below
// its trigger-point if hardware endstops are active.

#if(CON_SENSOR_INDUCTIVO)
  #define Z_PROBE_FIX_MOUNTED// Activar sensor
#endif


// The BLTouch probe emulates a servo probe.
//#define BLTOUCH

// Enable if you have a Z probe mounted on a sled like those designed by Charles Bell.
//#define Z_PROBE_SLED
// The extra distance the X axis must travel to pick up the sled.
// 0 should be fine but you can push it further if you'd like.
//#define SLED_DOCKING_OFFSET 0

// Offsets to the probe relative to the nozzle tip (Nozzle - Probe)
// X and Y offsets MUST be INTEGERS
//
//    +-- BACK ---+
//    |           |
//  L |    (+) P  | R <-- probe (10,10)
//  E |           | I
//  F | (-) N (+) | G <-- nozzle (0,0)
//  T |           | H
//    |  P (-)    | T <-- probe (-10,-10)
//    |           |
//    O-- FRONT --+
//  (0,0)
#if(MK_TITAN)
  #define X_PROBE_OFFSET_FROM_NOZZLE  29     // X offset: -left  [of the nozzle] +right
  #define Y_PROBE_OFFSET_FROM_NOZZLE  -49     // Y offset: -front [of the nozzle] +behind
  #define Z_PROBE_OFFSET_FROM_NOZZLE  0     //-2,7 Z offset: -below [of the nozzle] (always negative!)
#else
  #define X_PROBE_OFFSET_FROM_NOZZLE  44     // X offset: -left  [of the nozzle] +right
  #define Y_PROBE_OFFSET_FROM_NOZZLE  -41     // Y offset: -front [of the nozzle] +behind
  #define Z_PROBE_OFFSET_FROM_NOZZLE  0     //-2,7 Z offset: -below [of the nozzle] (always negative!)
#endif
// X and Y axis travel speed between probes, in mm/min
#define XY_PROBE_SPEED            3000

//
// Probe Raise options provide clearance for the probe to deploy, stow, and travel.
//
#define Z_RAISE_PROBE_DEPLOY_STOW 5  // Raise to make room for the probe to deploy / stow
#define Z_RAISE_BETWEEN_PROBINGS   5  // Raise between probing points.

//
// For M666 give a range for adjusting the Z probe offset
//
#define Z_PROBE_OFFSET_RANGE_MIN -50
#define Z_PROBE_OFFSET_RANGE_MAX  50
/*****************************************************************************************/


/*****************************************************************************************
 ********************************** Endstops min or max **********************************
 *****************************************************************************************
 *                                                                                       *
 * Sets direction of endstop when homing; 1=MAX, -1=MIN                                  *
 *                                                                                       *
 *****************************************************************************************/
#define X_HOME_DIR -1
#define Y_HOME_DIR 1
#define Z_HOME_DIR -1
#define E_HOME_DIR -1
/*****************************************************************************************/


/*****************************************************************************************
 ***************************** MIN Z HEIGHT FOR HOMING **********************************
 *****************************************************************************************
 *                                                                                       *
 * (in mm) Minimal z height before homing (G28) for Z clearance above the bed, clamps,   *
 * Be sure you have this distance over your Z_MAX_POS in case.                           *
 *                                                                                       *
 *****************************************************************************************/
#define MIN_Z_HEIGHT_FOR_HOMING   5
/*****************************************************************************************/


/*****************************************************************************************
 ********************************* Stepper enable logic **********************************
 *****************************************************************************************
 *                                                                                       *
 * For Inverting Stepper Enable Pins                                                     *
 * (Active Low) use 0                                                                    *
 * Non Inverting (Active High) use 1                                                     *
 *                                                                                       *
 *****************************************************************************************/
#define X_ENABLE_ON 0
#define Y_ENABLE_ON 0
#define Z_ENABLE_ON 0
#define E_ENABLE_ON 0      // For all extruder
/*****************************************************************************************/


/*****************************************************************************************
 ********************************* Stepper step logic **********************************
 *****************************************************************************************
 *                                                                                       *
 * By default pololu step drivers require an active high signal.                         *
 * However, some high power drivers require an active low signal as step.                *
 *                                                                                       *
 *****************************************************************************************/
#define INVERT_X_STEP_PIN false
#define INVERT_Y_STEP_PIN false
#define INVERT_Z_STEP_PIN false
#define INVERT_E_STEP_PIN false
/*****************************************************************************************/


/*****************************************************************************************
 ********************************** Stepper direction ************************************
 *****************************************************************************************
 *                                                                                       *
 * Invert the stepper direction.                                                         *
 * Change (or reverse the motor connector) if an axis goes the wrong way.                *
 *                                                                                       *
 *****************************************************************************************/
#define INVERT_X_DIR false

#if(PK3 || PK3_PLUS_PLUS || PK3_EXTEN)
  #define INVERT_Y_DIR true
#else
  #define INVERT_Y_DIR false
#endif

#if(MK_TITAN)
  #define INVERT_Z_DIR false
#else
  #define INVERT_Z_DIR true
#endif
#define INVERT_E0_DIR false
#define INVERT_E1_DIR false
#define INVERT_E2_DIR false
#define INVERT_E3_DIR false
#define INVERT_E4_DIR false
#define INVERT_E5_DIR false
/*****************************************************************************************/


/*****************************************************************************************
 ************************************* Disables axis *************************************
 *****************************************************************************************
 *                                                                                       *
 * Disables axis when it's not being used.                                               *
 *                                                                                       *
 *****************************************************************************************/
#define DISABLE_X false
#define DISABLE_Y false
#define DISABLE_Z false
#define DISABLE_E false      // For all extruder
// Disable only inactive extruder and keep active extruder enabled
#define DISABLE_INACTIVE_EXTRUDER false
/*****************************************************************************************/


/*****************************************************************************************
 ************************************ Travel limits **************************************
 *****************************************************************************************
 *                                                                                       *
 * Travel limits after homing (units are in mm)                                          *
 *                                                                                       *
 /*****************************************************************************************/

/*****************************************************************************************/
#if(PK2_PLUS_PLUS)
  #define X_MAX_POS 210
  #define X_MIN_POS 0
  #define Y_MAX_POS 310
  #define Y_MIN_POS 0
  #define Z_MAX_POS 200
  #define Z_MIN_POS 0
  #define E_MIN_POS 0

#elif(MK_TITAN)
  #define X_MAX_POS 300
  #define X_MIN_POS 0
  #define Y_MAX_POS 230
  #define Y_MIN_POS 0
  #define Z_MAX_POS 250
  #define Z_MIN_POS 0
  #define E_MIN_POS 0
/*****************************************************************************************/
#elif(PK3_PLUS_PLUS)
  #define X_MAX_POS 240
  #define X_MIN_POS 0
  #define Y_MAX_POS 320
  #define Y_MIN_POS 0
  #define Z_MAX_POS 210
  #define Z_MIN_POS 0
  #define E_MIN_POS 0
/*****************************************************************************************/
#elif(PK3_EXTEN)
  #define X_MAX_POS 240
  #define X_MIN_POS 0
  #define Y_MAX_POS 320
  #define Y_MIN_POS 0
  #define Z_MAX_POS 410
  #define Z_MIN_POS 0
  #define E_MIN_POS 0
/*****************************************************************************************/
#elif(PK3)
  #define X_MAX_POS 240 // Visar
  #define X_MIN_POS 0
  #define Y_MAX_POS 225
  #define Y_MIN_POS 0
  #define Z_MAX_POS 210
  #define Z_MIN_POS 0
  #define E_MIN_POS 0
/*****************************************************************************************/
#elif(PK2)
  #define X_MAX_POS 210
  #define X_MIN_POS 0
	#define Y_MAX_POS 210
  #define Y_MIN_POS 0
  #define Z_MAX_POS 200
  #define Z_MIN_POS 0
  #define E_MIN_POS 0
/*****************************************************************************************/
#elif(PK1)
  #define X_MAX_POS 205
  #define X_MIN_POS 0
	#define Y_MAX_POS 205
  #define Y_MIN_POS 0
  #define Z_MAX_POS 180
  #define Z_MIN_POS 0
  #define E_MIN_POS 0

#elif(ESPESIAL)
  #define X_MAX_POS 210
  #define X_MIN_POS 0
	#define Y_MAX_POS 210
  #define Y_MIN_POS 0
  #define Z_MAX_POS 200
  #define Z_MIN_POS 0
  #define E_MIN_POS 0

#endif


/*****************************************************************************************/


/*****************************************************************************************
 ********************************** Axis relative mode ***********************************
 *****************************************************************************************/
 //RESERVE EL G92
#define AXIS_RELATIVE_MODES {false, false, false, false}
/*****************************************************************************************/


/*****************************************************************************************
 *********************************** Safe Z homing ***************************************
 *****************************************************************************************
 *                                                                                       *
 * If you have enabled the auto bed levelling feature or are using                       *
 * Z Probe for Z Homing, it is highly recommended you let                                *
 * this Z_SAFE_HOMING enabled!!!                                                         *
 *                                                                                       *
 * X point for Z homing when homing all axis (G28)                                       *
 * Y point for Z homing when homing all axis (G28)                                       *
 *                                                                                       *
 * Uncomment Z_SAFE_HOMING to enable                                                     *
 *                                                                                       *
 *****************************************************************************************/
 #if(CON_SENSOR_INDUCTIVO)
   #define Z_SAFE_HOMING //Activar sensor
 #endif
//#define Z_SAFE_HOMING //Activar sensor
#if(MK_TITAN)
  #define Z_SAFE_HOMING_X_POINT ((X_MIN_POS + X_MAX_POS) / 2)
  #define Z_SAFE_HOMING_Y_POINT ((Y_MIN_POS + Y_MAX_POS) / 2)
#else
  #define Z_SAFE_HOMING_X_POINT ((X_MIN_POS + X_MAX_POS) / 2)
  #define Z_SAFE_HOMING_Y_POINT ((Y_MIN_POS + Y_MAX_POS) / 2)
#endif
/*****************************************************************************************/


/*****************************************************************************************
 ******************************* Mesh Bed Leveling ***************************************
 *****************************************************************************************/
#if(EXPERIMENTAL)
  #define MESH_BED_LEVELING
#endif
#define MESH_INSET         60   // Mesh inset margin on print area
#define MESH_NUM_X_POINTS   3   // Don't use more than 7 points per axis, implementation limited.
#define MESH_NUM_Y_POINTS   3
#define MESH_HOME_SEARCH_Z  5   // Z after Home, bed somewhere below but above 0.0.

// After homing all axes ('G28' or 'G28 XYZ') rest at origin [0,0,0]
#define MESH_G28_REST_ORIGIN

// Add display menu option for bed leveling.
#if(EXPERIMENTAL)
  #define MANUAL_BED_LEVELING
#endif
// Step size while manually probing Z axis.
#define MBL_Z_STEP 0.025
#define LCD_PROBE_Z_RANGE 8 // Z Range centered on Z_MIN_POS for LCD Z adjustment
/*****************************************************************************************/


/*****************************************************************************************
 ******************************* Auto Bed Leveling ***************************************
 *****************************************************************************************
 *                                                                                       *
 * There are 2 different ways to specify probing locations                               *
 *                                                                                       *
 * - "grid" mode                                                                         *
 *   Probe several points in a rectangular grid.                                         *
 *   You specify the rectangle and the density of sample points.                         *
 *   This mode is preferred because there are more measurements.                         *
 *                                                                                       *
 * - "3-point" mode                                                                      *
 *   Probe 3 arbitrary points on the bed (that aren't colinear)                          *
 *   You specify the XY coordinates of all 3 points.                                     *
 *                                                                                       *
 * Remember you must define type of probe                                                *
 * Uncomment AUTO BED LEVELING FEATURE to enable                                         *
 *                                                                                       *
 *****************************************************************************************/
 #if(CON_SENSOR_INDUCTIVO)
   //#define AUTO_BED_LEVELING_FEATURE //Activar sensor
 #endif
//#define Z_PROBE_REPEATABILITY_TEST  // If not commented out, Z-Probe Repeatability test will be included if Auto Bed Leveling is Enabled.

// Enable this to sample the bed in a grid (least squares solution)
// Note: this feature generates 10KB extra code size
//
#define AUTO_BED_LEVELING_GRID


/** START yes AUTO BED LEVELING GRID **/
#define LEFT_PROBE_BED_POSITION 44     //0
#define RIGHT_PROBE_BED_POSITION 204  //160
#define FRONT_PROBE_BED_POSITION 16   //60
#define BACK_PROBE_BED_POSITION 250   //300

// The Z probe minimum square sides can be no smaller than this.
#define MIN_PROBE_EDGE 5

// Set the number of grid points per dimension
// You probably don't need more than 3 (squared=9)
#define AUTO_BED_LEVELING_GRID_POINTS 3
/** END yes AUTO BED LEVELING GRID **/


/** START no AUTO BED LEVELING GRID **/
// Arbitrary points to probe. A simple cross-product
// is used to estimate the plane of the bed.
#define ABL_PROBE_PT_1_X 40 //40
#define ABL_PROBE_PT_1_Y 70 //
#define ABL_PROBE_PT_2_X 40
#define ABL_PROBE_PT_2_Y 250
#define ABL_PROBE_PT_3_X 160
#define ABL_PROBE_PT_3_Y 120
/** END no AUTO BED LEVELING GRID **/

// These commands will be executed in the end of G29 routine.
// Useful to retract a deployable Z probe.
#define Z_PROBE_END_SCRIPT "G1 X5 Y5 F3000"
/*****************************************************************************************/


/*****************************************************************************************
 ******************************** Manual home positions **********************************
 *****************************************************************************************/
// The position of the homing switches
//#define MANUAL_HOME_POSITIONS   // If defined, MANUAL_*_HOME_POS below will be used
//#define BED_CENTER_AT_0_0       // If defined, the center of the bed is at (X=0, Y=0)

//Manual homing switch locations:
#define MANUAL_X_HOME_POS 0
#define MANUAL_Y_HOME_POS 0
#define MANUAL_Z_HOME_POS 0
/*****************************************************************************************/


/*****************************************************************************************
 ******************************* Axis steps per unit *************************************
 *****************************************************************************************/
// Default steps per unit               X  ,  Y ,  Z  ,   E0   ,...(per extruder)


#if(PK2_PLUS_PLUS)
  #define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,400,95,95,95,95}

#elif(PK3_PLUS_PLUS)
  #define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,400,95,95,95,95}

#elif(PK3_EXTEN)
  #define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,400,95,95,95,95}

#elif(PK3)
  #define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,400,95,95,95,95}

#elif(MK_TITAN)
  #define DEFAULT_AXIS_STEPS_PER_UNIT   {80,80,1600,95,95,95,95}

#elif(PK2)
  #define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,400,95,95,95,95}

#elif(PK1)
  #define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,4047,95,95,95,95}

#elif(ESPESIAL)
  #define DEFAULT_AXIS_STEPS_PER_UNIT   {100,100,400,100,100,100,100}

#endif

/*****************************************************************************************/


/*****************************************************************************************
 ********************************** Axis feedrate ****************************************
 *****************************************************************************************/
//                                       X,   Y, Z,  E0...(per extruder). (mm/sec)
#if(PK2_PLUS_PLUS)
  #define DEFAULT_MAX_FEEDRATE          {160, 160, 15, 25, 15, 25, 25}
  #define MANUAL_FEEDRATE               {500, 500, 1000, 10000}  // Feedrates for manual moves along X, Y, Z, E from panel
  #define DEFAULT_MAX_ACCELERATION              {500, 500, 1000, 10000, 1000, 10000, 10000}

#elif(PK3_PLUS_PLUS)
  #define DEFAULT_MAX_FEEDRATE          {160, 160, 15, 25, 25, 25, 25}
  #define MANUAL_FEEDRATE               {500, 500, 1000, 10000}  // Feedrates for manual moves along X, Y, Z, E from panel
  #define DEFAULT_MAX_ACCELERATION              {500, 500, 1000, 10000, 10000, 10000, 10000}

#elif(PK3_EXTEN)
  #define DEFAULT_MAX_FEEDRATE          {160, 160, 15, 25, 25, 25, 25}
  #define MANUAL_FEEDRATE               {500, 500, 1000, 10000}  // Feedrates for manual moves along X, Y, Z, E from panel
  #define DEFAULT_MAX_ACCELERATION              {500, 500, 1000, 10000, 10000, 10000, 10000}

#elif(MK_TITAN)
  #define DEFAULT_MAX_FEEDRATE          {160, 160, 5, 25, 25, 25, 25}
  #define MANUAL_FEEDRATE               {500, 500, 250, 10000}  // Feedrates for manual moves along X, Y, Z, E from panel
  #define DEFAULT_MAX_ACCELERATION              {500, 500, 250, 10000, 10000, 10000, 10000}

#elif(PK3)
  #define DEFAULT_MAX_FEEDRATE          {350, 350, 15, 25, 25, 25, 25}
  #define MANUAL_FEEDRATE               {1500, 1500, 1000, 10000}  // Feedrates for manual moves along X, Y, Z, E from panel
  #define DEFAULT_MAX_ACCELERATION              {1000, 1000, 1000, 10000, 10000, 10000, 10000}

#elif(PK2)
  #define DEFAULT_MAX_FEEDRATE          {350, 350, 15, 25, 25, 25, 25}
  #define MANUAL_FEEDRATE               {1500, 1500, 1000, 10000}  // Feedrates for manual moves along X, Y, Z, E from panel
  #define DEFAULT_MAX_ACCELERATION              {1000, 1000, 1000, 10000, 10000, 10000, 10000}

#elif(PK1)
  #define DEFAULT_MAX_FEEDRATE          {350, 350, 1.5, 25, 25, 25, 25}
  #define MANUAL_FEEDRATE               {1500, 1500, 100, 10000}  // Feedrates for manual moves along X, Y, Z, E from panel
  #define DEFAULT_MAX_ACCELERATION              {1000, 1000, 100, 10000, 10000, 10000, 10000}

#elif(ESPESIAL)
  #define DEFAULT_MAX_FEEDRATE          {160, 160, 5, 25, 25, 25, 25}
  #define MANUAL_FEEDRATE               {1500, 1500, 100, 500}  // Feedrates for manual moves along X, Y, Z, E from panel
  #define DEFAULT_MAX_ACCELERATION              {500, 500, 100, 500, 500, 500, 500}

#endif


#define DEFAULT_MINIMUMFEEDRATE       0.0                       // minimum feedrate
#define DEFAULT_MINTRAVELFEEDRATE     0.0
// Minimum planner junction speed. Sets the default minimum speed the planner plans for at the end
// of the buffer and all stops. This should not be much greater than zero and should only be changed
// if unwanted behavior is observed on a user's machine when running at very slow speeds.
#define MINIMUM_PLANNER_SPEED         0.05                      // (mm/sec)
/*****************************************************************************************/


/*****************************************************************************************
 ******************************** Axis accelleration *************************************
 *****************************************************************************************/
//  Maximum start speed for accelerated moves.    X,    Y,  Z,   E0...(per extruder)
//  Maximum acceleration in mm/s^2 for retracts   E0... (per extruder)
#define DEFAULT_RETRACT_ACCELERATION          {1000, 1000, 1000, 1000}
//  X, Y, Z and E* maximum acceleration in mm/s^2 for printing moves
#define DEFAULT_ACCELERATION          1000
//  X, Y, Z acceleration in mm/s^2 for travel (non printing) moves
#define DEFAULT_TRAVEL_ACCELERATION   1000
/*****************************************************************************************/


/*****************************************************************************************
 ************************************* Axis jerk *****************************************
 *****************************************************************************************
 *                                                                                       *
 * The speed change that does not require acceleration.                                  *
 * (i.e. the software might assume it can be done instantaneously)                       *
 *                                                                                       *
 *****************************************************************************************/
#define DEFAULT_XYJERK                    20.0                 // (mm/sec)
#define DEFAULT_ZJERK                     0.4                 // (mm/sec)
//  max initial speed for retract moves   E0... (mm/sec) per extruder
#define DEFAULT_EJERK                   {5.0, 5.0, 5.0, 5.0}
/*****************************************************************************************/


/*****************************************************************************************
 ************************************ Homing feedrate ************************************
 *****************************************************************************************/
// Homing speeds (mm/m)
#define HOMING_FEEDRATE_X (1000)
#define HOMING_FEEDRATE_Y (1000)
#define HOMING_FEEDRATE_Z (1000)

// homing hits the endstop, then retracts by this distance, before it tries to slowly bump again:
#define X_HOME_BUMP_MM 5
#define Y_HOME_BUMP_MM 5
#define Z_HOME_BUMP_MM 2
#define HOMING_BUMP_DIVISOR {5, 5, 2}  // Re-Bump Speed Divisor (Divides the Homing Feedrate)
/*****************************************************************************************/


/*****************************************************************************************
 *********************************** Hotend offset ***************************************
 *****************************************************************************************
 *                                                                                       *
 * Offset of the hotends (uncomment if using more than one and relying on firmware       *
 * to position when changing).                                                           *
 * The offset has to be X=0, Y=0, Z=0 for the hotend 0 (default hotend).                 *
 * For the other hotends it is their distance from the hotend 0.                         *
 *                                                                                       *
 *****************************************************************************************/
#define HOTEND_OFFSET_X {0.0, 0.0, 0.0, 0.0} // (in mm) for each hotend, offset of the hotend on the X axis
#define HOTEND_OFFSET_Y {0.0, 0.0, 0.0, 0.0} // (in mm) for each hotend, offset of the hotend on the Y axis
#define HOTEND_OFFSET_Z {0.0, 0.0, 0.0, 0.0} // (in mm) for each hotend, offset of the hotend on the Z axis
/*****************************************************************************************/


/*****************************************************************************************
 ******************************** CARTESIAN CORRECTION ***********************************
 *****************************************************************************************
 *                                                                                       *
 * New functions, Hysteresis and Zwobble.                                                *
 *                                                                                       *
 * Hysteresis:                                                                           *
 * These are the extra distances that are performed when an axis changes direction       *
 * to compensate for any mechanical hysteresis your printer has.                         *
 * Set the parameters width M99 X<in mm> Y<in mm> Z<in mm> E<in mm>                      *
 *                                                                                       *
 * ZWobble:                                                                              *
 * How to use it:                                                                        *
 * Set the parameters with M97 A<Amplitude_in_mm> W<period_in_mm> P<phase_in_degrees>    *
 * KNOWN LIMITATION (by design): if you redefine the Z value during your print           *
 * (with a G92 for example), the correction *will* screw up                              *
 * How does it work?                                                                     *
 * This class compensates for a wobble of the Z axis that makes the translation          *
 * rod movement->bed (extruder) movement nonlinear.                                      *
 * Instead of assuming Zactual = Zrod, the function assumes that                         *
 * Zaxtual = Zrod + A*sin(w*Zrod + phase). Since the user wants to specify Zactual,      *
 * we need to invert the formula to obtain Zrod, which is the value that will serve      *
 * as the input of the motor.                                                            *
 *                                                                                       *
 *****************************************************************************************/
//#define HYSTERESIS
//#define ZWOBBLE

#define DEFAULT_HYSTERESIS_MM   0, 0, 0, 0  // X, Y, Z, E hysteresis in mm.
#define DEFAULT_ZWOBBLE         0, 0, 0     // A, W, P
/*****************************************************************************************/

#endif
