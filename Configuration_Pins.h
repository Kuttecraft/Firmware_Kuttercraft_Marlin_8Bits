/**
 * Marlin Kuttercraft 3D Printer Firmware
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 - 2016 Alberto Cotronei @MagoKimbra
 * Copyright (C) 2017 Kuttercraft Kuttercraft@gmail.com
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
 * Configuration_Pins.h
 *
 * This configuration file contains all Pins.
 *
 */

#ifndef CONFIGURATION_PINS_H
#define CONFIGURATION_PINS_H

//=================================== BASIC ==================================

// X axis pins
#define X_STEP_PIN      ORIG_X_STEP_PIN
#define X_DIR_PIN       ORIG_X_DIR_PIN
#define X_ENABLE_PIN    ORIG_X_ENABLE_PIN

// Y axis pins
#define Y_STEP_PIN      ORIG_Y_STEP_PIN
#define Y_DIR_PIN       ORIG_Y_DIR_PIN
#define Y_ENABLE_PIN    ORIG_Y_ENABLE_PIN

// Z axis pins
#define Z_STEP_PIN      ORIG_Z_STEP_PIN
#define Z_DIR_PIN       ORIG_Z_DIR_PIN
#define Z_ENABLE_PIN    ORIG_Z_ENABLE_PIN

// E axis pins
#if DRIVER_EXTRUDERS > 0
  #define E0_STEP_PIN   ORIG_E0_STEP_PIN
  #define E0_DIR_PIN    ORIG_E0_DIR_PIN
  #define E0_ENABLE_PIN ORIG_E0_ENABLE_PIN
#endif

#if DRIVER_EXTRUDERS > 1
  #define E1_STEP_PIN   ORIG_E1_STEP_PIN
  #define E1_DIR_PIN    ORIG_E1_DIR_PIN
  #define E1_ENABLE_PIN ORIG_E1_ENABLE_PIN
#endif

#if DRIVER_EXTRUDERS > 2
  #define E2_STEP_PIN   ORIG_E2_STEP_PIN
  #define E2_DIR_PIN    ORIG_E2_DIR_PIN
  #define E2_ENABLE_PIN ORIG_E2_ENABLE_PIN
#endif

#if DRIVER_EXTRUDERS > 3
  #define E3_STEP_PIN   ORIG_E3_STEP_PIN
  #define E3_DIR_PIN    ORIG_E3_DIR_PIN
  #define E3_ENABLE_PIN ORIG_E3_ENABLE_PIN
#endif

#if DRIVER_EXTRUDERS > 4
  #define E4_STEP_PIN   ORIG_E4_STEP_PIN
  #define E4_DIR_PIN    ORIG_E4_DIR_PIN
  #define E4_ENABLE_PIN ORIG_E4_ENABLE_PIN
#endif

#if DRIVER_EXTRUDERS > 5
  #define E5_STEP_PIN   ORIG_E5_STEP_PIN
  #define E5_DIR_PIN    ORIG_E5_DIR_PIN
  #define E5_ENABLE_PIN ORIG_E5_ENABLE_PIN
#endif

// ENDSTOP pin
#define X_MIN_PIN       ORIG_X_MIN_PIN
#define X_MAX_PIN       -1

#define Y_MIN_PIN       ORIG_Y_MIN_PIN
#define Y_MAX_PIN       ORIG_Y_MAX_PIN

//#define Z_MIN_PIN       ORIG_Z_MIN_PIN
//#define Z_PROBE_PIN     ORIG_Z_MAX_PIN//ORIG_Z_MIN_PIN

#if(SENSOR_INDUCTIVO_NUEVO)
  #define Z_MIN_PIN       ORIG_Z_MAX_PIN
  #define Z_PROBE_PIN     ORIG_Z_MIN_PIN//ORIG_Z_MIN_PIN
#else
  #define Z_MIN_PIN       ORIG_Z_MIN_PIN
  #define Z_PROBE_PIN     ORIG_Z_MAX_PIN//ORIG_Z_MIN_PIN
#endif

#define Z_MAX_PIN       -1//ORIG_Z_MAX_PIN
#define E_MIN_PIN       ORIG_X_MAX_PIN
//#define SERVO_K         11

// HEATER pin
#define HEATER_0_PIN        ORIG_HEATER_0_PIN
#define HEATER_1_PIN        ORIG_HEATER_1_PIN
#define HEATER_2_PIN        ORIG_HEATER_2_PIN
#define HEATER_3_PIN        ORIG_HEATER_3_PIN
#define HEATER_BED_PIN      ORIG_HEATER_BED_PIN
#define HEATER_CHAMBER_PIN  -1
#define COOLER_PIN          -1

// TEMP pin
#define TEMP_0_PIN        ORIG_TEMP_0_PIN
#define TEMP_1_PIN        ORIG_TEMP_1_PIN
#define TEMP_2_PIN        ORIG_TEMP_2_PIN
#define TEMP_3_PIN        ORIG_TEMP_3_PIN
#define TEMP_BED_PIN      ORIG_TEMP_BED_PIN
#define TEMP_CHAMBER_PIN  -1
#define TEMP_COOLER_PIN   -1

// FAN pin
#define FAN_PIN         ORIG_FAN_PIN

// PS ON pin
#define PS_ON_PIN       ORIG_PS_ON_PIN

// BEEPER pin
#define BEEPER_PIN      ORIG_BEEPER_PIN

//============================================================================

//================================= FEATURE ==================================

#if ENABLED(MKR4)
  #define E0E1_CHOICE_PIN -1
  #define E0E2_CHOICE_PIN -1
  #define E1E3_CHOICE_PIN -1
#elif ENABLED(MKR6)
  #define EX1_CHOICE_PIN  -1
  #define EX2_CHOICE_PIN  -1
#endif

#if ENABLED(LASERBEAM)
  #define LASER_PWR_PIN                   -1
  #define LASER_TTL_PIN                   -1
  #if ENABLED(LASER_PERIPHERALS)
    #define LASER_PERIPHERALS_PIN         -1
    #define LASER_PERIPHERALS_STATUS_PIN  -1
  #endif
#endif

#if ENABLED(FILAMENT_RUNOUT_SENSOR)
  #define FILRUNOUT_PIN -1
#endif

#if ENABLED(FILAMENT_SENSOR)
  #define FILWIDTH_PIN -1
#endif

#if ENABLED(FLOWMETER_SENSOR)
  #define FLOWMETER_PIN -1
#endif

#if ENABLED(POWER_CONSUMPTION)
  #define POWER_CONSUMPTION_PIN -1
#endif

#if ENABLED(PHOTOGRAPH)
  #define PHOTOGRAPH_PIN -1
#endif

#if ENABLED(CHDK)
  #define CHDK_PIN -1
#endif

#if ENABLED(CONTROLLERFAN)
  #define CONTROLLERFAN_PIN -1
#endif

#if ENABLED(EXTRUDER_AUTO_FAN)
  #define EXTRUDER_0_AUTO_FAN_PIN -1
  #define EXTRUDER_1_AUTO_FAN_PIN -1
  #define EXTRUDER_2_AUTO_FAN_PIN -1
  #define EXTRUDER_3_AUTO_FAN_PIN -1
#endif

#if ENABLED(X2_IS_TMC)
  #define X2_ENABLE_PIN -1
  #define X2_STEP_PIN   -1
  #define X2_DIR_PIN    -1
#endif

#if ENABLED(Z_PROBE_SLED)
  #define SLED_PIN -1
#endif

//============================================================================

#endif
