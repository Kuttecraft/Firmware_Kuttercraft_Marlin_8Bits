/**
 * Kuttercraft 3D Printer Firmware
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
  temperature.cpp - temperature control
  Part of Marlin

 Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "../../base.h"

//===========================================================================
//================================== macros =================================
//===========================================================================

#if ENABLED(K1) // Defined in Configuration_base.h in the PID settings
  #define K2 (1.0 - K1)
#endif


//===========================================================================
//============================= public variables ============================
//===========================================================================

int target_temperature[4] = { 0 };
float current_temperature[4] = { 0.0 };
int current_temperature_raw[4] = { 0 };

int target_temperature_bed = 0;
float current_temperature_bed = 0.0;
int current_temperature_bed_raw = 0;

int target_temperature_chamber = 0;
int current_temperature_chamber_raw = 0;
float current_temperature_chamber = 0.0;

int target_temperature_cooler = 0;
int current_temperature_cooler_raw = 0;
float current_temperature_cooler = 0.0;

float elapsedTime, Time, timePrev;

#if ENABLED(TEMP_SENSOR_1_AS_REDUNDANT)
  int redundant_temperature_raw = 0;
  float redundant_temperature = 0.0;
#endif

#if ENABLED(PIDTEMPBED)
  float bedKp = DEFAULT_bedKp;
  float bedKi = ((DEFAULT_bedKi) * (PID_dT));
  float bedKd = ((DEFAULT_bedKd) / (PID_dT));
#endif

#if ENABLED(PIDTEMPCHAMBER)
  float chamberKp = DEFAULT_chamberKp;
  float chamberKi = ((DEFAULT_chamberKi) * (PID_dT));
  float chamberKd = ((DEFAULT_chamberKd) / (PID_dT));
#endif

#if ENABLED(PIDTEMPCOOLER)
  float coolerKp = DEFAULT_coolerKp;
  float coolerKi = ((DEFAULT_coolerKi) * (PID_dT));
  float coolerKd = ((DEFAULT_coolerKd) / (PID_dT));
#endif

#if ENABLED(FAN_SOFT_PWM)
  unsigned char fanSpeedSoftPwm = 0;
  #if HAS(AUTO_FAN)
    unsigned char fanSpeedSoftPwm_auto = EXTRUDER_AUTO_FAN_MIN_SPEED;
  #endif
  #if HAS(CONTROLLERFAN)
    unsigned char fanSpeedSoftPwm_controller = CONTROLLERFAN_MIN_SPEED;
  #endif
#endif

unsigned char soft_pwm_bed;
unsigned char soft_pwm_chamber;
unsigned char soft_pwm_cooler;

#if ENABLED(FAST_PWM_COOLER)
  unsigned char fast_pwm_cooler;
#endif

void setPwmCooler(unsigned char pwm);

#if ENABLED(BABYSTEPPING)
  volatile int babystepsTodo[3] = { 0 };
#endif

#if ENABLED(FILAMENT_SENSOR)
  int current_raw_filwidth = 0;  //Holds measured filament diameter - one extruder only
#endif

#if ENABLED(THERMAL_PROTECTION_HOTENDS) || ENABLED(THERMAL_PROTECTION_BED) || ENABLED(THERMAL_PROTECTION_COOLER)
  enum TRState { TRInactive, TRFirstRunning, TRStable, TRRunaway };
  void thermal_runaway_protection(TRState* state, millis_t* timer, float temperature, float target_temperature, int temp_controller_id, int period_seconds, int hysteresis_degc);

  #if ENABLED(THERMAL_PROTECTION_HOTENDS)
    static TRState thermal_runaway_state_machine[HOTENDS] = { TRInactive };
    static millis_t thermal_runaway_timer[HOTENDS] = { 0 };
  #endif
  #if ENABLED(THERMAL_PROTECTION_BED) && TEMP_SENSOR_BED != 0
    static TRState thermal_runaway_bed_state_machine = TRInactive;
    static millis_t thermal_runaway_bed_timer;
  #endif
  #if ENABLED(THERMAL_PROTECTION_COOLER) && TEMP_SENSOR_COOLER != 0
    static TRState thermal_runaway_cooler_state_machine = TRReset;
    static millis_t thermal_runaway_cooler_timer;
  #endif
#endif

#if HAS(POWER_CONSUMPTION_SENSOR)
  int current_raw_powconsumption = 0;  //Holds measured power consumption
  static unsigned long raw_powconsumption_value = 0;
#endif

//===========================================================================
//============================ private variables ============================
//===========================================================================

static volatile bool temp_meas_ready = false;

#if ENABLED(PIDTEMP)
  //static cannot be external:
  static float temp_iState[HOTENDS] = { 0 };
  static float temp_dState[HOTENDS] = { 0 };
  static float pTerm[HOTENDS];
  static float iTerm[HOTENDS];
  static float dTerm[HOTENDS];
  #if ENABLED(PID_ADD_EXTRUSION_RATE)
    static float cTerm[HOTENDS];
    static long last_e_position;
    static long lpq[LPQ_MAX_LEN];
    static int lpq_ptr = 0;
  #endif
  //int output;
  static float pid_error[HOTENDS];
  static float previous_error[HOTENDS];
  static float temp_iState_min[HOTENDS];
  static float temp_iState_max[HOTENDS];
  static bool pid_reset[HOTENDS];
  static bool primer_calentamiento = false;
  static bool solo_una_vez_ki = false;
#endif //PIDTEMP
#if ENABLED(PIDTEMPBED)
  //static cannot be external:
  static float temp_iState_bed = { 0 };
  static float temp_dState_bed = { 0 };
  static float pTerm_bed;
  static float iTerm_bed;
  static float dTerm_bed;
  //int output;
  static float pid_error_bed;
  static float temp_iState_min_bed;
  static float temp_iState_max_bed;
#else // PIDTEMPBED
  static millis_t next_bed_check_ms;
#endif // !PIDTEMPBED
#if ENABLED(PIDTEMPCHAMBER)
  //static cannot be external:
  static float temp_iState_chamber = { 0 };
  static float temp_dState_chamber = { 0 };
  static float pTerm_chamber;
  static float iTerm_chamber;
  static float dTerm_chamber;
  //int output;
  static float pid_error_chamber;
  static float temp_iState_min_chamber;
  static float temp_iState_max_chamber;
#else // PIDTEMPCHAMBER
  static millis_t next_chamber_check_ms;
#endif // !PIDTEMPCHAMBER
#if ENABLED(PIDTEMPCOOLER)
  //static cannot be external:
  static float temp_iState_cooler = { 0 };
  static float temp_dState_cooler = { 0 };
  static float pTerm_cooler;
  static float iTerm_cooler;
  static float dTerm_cooler;
  //int output;
  static float pid_error_cooler;
  static float temp_iState_min_cooler;
  static float temp_iState_max_cooler;
#else // PIDTEMPCOOLER
  static millis_t next_cooler_check_ms;
#endif // !PIDTEMPCOOLER

static unsigned char soft_pwm[HOTENDS];

#if ENABLED(FAN_SOFT_PWM)
  static unsigned char soft_pwm_fan;
#endif

#if HAS(AUTO_FAN)
  static millis_t next_auto_fan_check_ms = 0;
#endif

#if ENABLED(PIDTEMP)
  float Kp[HOTENDS], Ki[HOTENDS], Kd[HOTENDS], Kc[HOTENDS];
#endif //PIDTEMP

// Init min and max temp with extreme values to prevent false errors during startup
static int minttemp_raw[HOTENDS] = ARRAY_BY_HOTENDS_N( HEATER_0_RAW_LO_TEMP , HEATER_1_RAW_LO_TEMP , HEATER_2_RAW_LO_TEMP, HEATER_3_RAW_LO_TEMP);
static int maxttemp_raw[HOTENDS] = ARRAY_BY_HOTENDS_N( HEATER_0_RAW_HI_TEMP , HEATER_1_RAW_HI_TEMP , HEATER_2_RAW_HI_TEMP, HEATER_3_RAW_HI_TEMP);
static int minttemp[HOTENDS] = { 0 };
static int maxttemp[HOTENDS] = ARRAY_BY_HOTENDS(16383);

static unsigned long raw_temp_value[4] = { 0 };
static unsigned long raw_temp_bed_value = 0;
static unsigned long raw_temp_chamber_value = 0;
static unsigned long raw_temp_cooler_value = 0;

#if ENABLED(BED_MINTEMP)
  static int bed_minttemp_raw = HEATER_BED_RAW_LO_TEMP;
#endif
#if ENABLED(BED_MAXTEMP)
  static int bed_maxttemp_raw = HEATER_BED_RAW_HI_TEMP;
#endif
#if ENABLED(CHAMBER_MINTEMP)
  static int chamber_minttemp_raw = HEATER_CHAMBER_RAW_LO_TEMP;
#endif
#if ENABLED(CHAMBER_MAXTEMP)
  static int chamber_maxttemp_raw = HEATER_CHAMBER_RAW_HI_TEMP;
#endif
#if ENABLED(COOLER_MINTEMP)
  static int cooler_minttemp_raw = COOLER_RAW_LO_TEMP;
#endif
#if ENABLED(COOLER_MAXTEMP)
  static int cooler_maxttemp_raw = COOLER_RAW_HI_TEMP;
#endif

#ifdef __SAM3X8E__
  // MEDIAN COUNT
  // For Smoother temperature
  // ONLY FOR DUE
  #define MEDIAN_COUNT 10

  #define CORRECTION_FOR_RAW_TEMP 4
  #define RAW_MIN_TEMP_DEFAULT 123000
  #define RAW_MEDIAN_TEMP_DEFAULT 3600 * OVERSAMPLENR

  static int max_temp[7];
  static int min_temp[7];
  static unsigned long raw_median_temp[7][MEDIAN_COUNT];
  static uint8_t median_counter;
  static unsigned long sum;
#endif

#if ENABLED(TEMP_SENSOR_1_AS_REDUNDANT)
  static void* heater_ttbl_map[2] = {(void*)HEATER_0_TEMPTABLE, (void*)HEATER_1_TEMPTABLE };
  static uint8_t heater_ttbllen_map[2] = { HEATER_0_TEMPTABLE_LEN, HEATER_1_TEMPTABLE_LEN };
#else
  static void* heater_ttbl_map[HOTENDS] = ARRAY_BY_HOTENDS_N( (void*)HEATER_0_TEMPTABLE, (void*)HEATER_1_TEMPTABLE, (void*)HEATER_2_TEMPTABLE, (void*)HEATER_3_TEMPTABLE );
  static uint8_t heater_ttbllen_map[HOTENDS] = ARRAY_BY_HOTENDS_N( HEATER_0_TEMPTABLE_LEN, HEATER_1_TEMPTABLE_LEN, HEATER_2_TEMPTABLE_LEN, HEATER_3_TEMPTABLE_LEN );
#endif

static float analog2temp(int raw, uint8_t e);
static float analog2tempBed(int raw);
static float analog2tempChamber(int raw);
static float analog2tempCooler(int raw);
static void updateTemperaturesFromRawValues();

#if ENABLED(THERMAL_PROTECTION_HOTENDS)
  int watch_target_temp[HOTENDS] = { 0 };
  millis_t watch_heater_next_ms[HOTENDS] = { 0 };
#endif

#if ENABLED(THERMAL_PROTECTION_BED)
  int watch_target_bed_temp = 0;
  millis_t watch_bed_next_ms = 0;
#endif

#if ENABLED(PREVENT_COLD_EXTRUSION)
  float extrude_min_temp = EXTRUDE_MINTEMP;
  bool allow_cold_extrude = false;
#endif

#if ENABLED(THERMAL_PROTECTION_CHAMBER)
  int watch_target_temp_chamber = 0;
  millis_t watch_chamber_next_ms = 0;
#endif

#if ENABLED(THERMAL_PROTECTION_COOLER)
  int watch_target_temp_cooler = 0;
  millis_t watch_cooler_next_ms = 0;
#endif

#if DISABLED(SOFT_PWM_SCALE)
  #define SOFT_PWM_SCALE 0
#endif

#if ENABLED(FILAMENT_SENSOR)
  static int meas_shift_index;  // used to point to a delayed sample in buffer for filament width sensor
#endif

#if ENABLED(HEATER_0_USES_MAX6675)
  static int16_t read_max6675();
#endif

//===========================================================================
//================================ Functions ================================
//===========================================================================

void setPwmCooler(unsigned char pwm) {
  soft_pwm_cooler = pwm >> 1;
  #if ENABLED(FAST_PWM_COOLER)
    fast_pwm_cooler = pwm;
    analogWrite(COOLER_PIN, pwm);
  #endif
}

unsigned char getPwmCooler(bool soft = true) {
  if(soft)
    return soft_pwm_cooler;
  #if ENABLED(FAST_PWM_COOLER)
    return fast_pwm_cooler;
  #else
    return soft_pwm_cooler * 2;
  #endif
}

void autotempShutdown() {
  #if ENABLED(AUTOTEMP)
    if (planner.autotemp_enabled) {
      planner.autotemp_enabled = false;
      if (degTargetHotend(active_extruder) > planner.autotemp_min)
        setTargetHotend(0, active_extruder);
    }
  #endif
}

#if  HAS(PID_HEATING) || HAS(PID_COOLING)
  void PID_autotune(float temp, int temp_controller, int ncycles, bool set_result/*=false*/) {
    float input = 0.0;
    int cycles = 0;
    bool running = true;

    millis_t temp_ms = millis(), t1 = temp_ms, t2 = temp_ms;
    long t_high = 0, t_low = 0;

    long bias = 0, d = 0;
    float Ku = 0, Tu = 0;
    float workKp = 0, workKi = 0, workKd = 0;
    float max = 0, min = 10000;

    #if HAS(AUTO_FAN)
      millis_t next_auto_fan_check_ms = temp_ms + 2500UL;
    #endif

    if (temp_controller >= HOTENDS
      #if HASNT(TEMP_BED) && HASNT(TEMP_COOLER)
        || temp_controller < 0
      #elif HASNT(TEMP_BED)
        || (temp_controller == -1 && temp_controller < -3)
      #elif HASNT(TEMP_COOLER)
        || temp_controller < -2
      #endif
    ) {
      SERIAL_LM(ER, MSG_PID_BAD_TEMP_CONTROLLER_NUM);
      return;
    }

    SERIAL_EM(MSG_PID_AUTOTUNE_START);
    if (temp_controller == -1) {
      SERIAL_M("BED");
    }
    else if(temp_controller == -2) {
      SERIAL_M("CHAMBER");
    }
    else if(temp_controller == -3) {
      SERIAL_M("COOLER");
    }
    else {
      SERIAL_MV("Hotend: ", temp_controller);
    }
    SERIAL_MV(" Temp: ", temp);
    SERIAL_MV(" Cycles: ", ncycles);
    if (set_result)
      SERIAL_EM(" Apply result");
    else
      SERIAL_E;

    disable_all_heaters(); // switch off all heaters.
    disable_all_coolers(); // switch off all coolers.

    if (temp_controller == -1)
      soft_pwm_bed = bias = d = MAX_BED_POWER / 2;
    else if (temp_controller == -3) {
      bias = d = MAX_COOLER_POWER / 2;
      setPwmCooler(MAX_COOLER_POWER);
    }
    else
      soft_pwm[temp_controller] = bias = d = PID_MAX / 2;

    // PID Tuning loop
    for (;;) {

      millis_t ms = millis();

      if (temp_meas_ready) { // temp sample ready
        updateTemperaturesFromRawValues();

        if (temp_controller == -1)
          input = current_temperature_bed;
        else if (temp_controller == -3)
          input = current_temperature_cooler;
        else
          input = current_temperature[temp_controller];

        max = max(max, input);
        min = min(min, input);

        #if HAS(AUTO_FAN)
          if (ELAPSED(ms, next_auto_fan_check_ms)) {
            checkExtruderAutoFans();
            next_auto_fan_check_ms = ms + 2500UL;
          }
        #endif

        if (running && ((input > temp && temp_controller >= -1) || (input < temp && temp_controller < -1))) {
          if (ms > t2 + 5000UL) {
            running = false;

            if (temp_controller == -3)
              setPwmCooler((bias - d));
            else if (temp_controller == -1)
              soft_pwm_bed = (bias - d) >> 1;
            else
              soft_pwm[temp_controller] = (bias - d) >> 1;

            t1 = ms;
            t_high = t1 - t2;

            if (temp_controller == -3)
              min = temp;
            else
              max = temp;
          }
        }

        if (!running && ((input < temp && temp_controller >= -1) || (input > temp && temp_controller < -1))) {
          if (ms > t1 + 5000UL) {
            running = true;
            t2 = ms;
            t_low = t2 - t1;
            if (cycles > 0) {
              long max_pow;

              if (temp_controller == -3)
                max_pow = MAX_COOLER_POWER;
              else
                max_pow = temp_controller < 0 ? MAX_BED_POWER : PID_MAX;

              bias += (d * (t_high - t_low)) / (t_low + t_high);
              bias = constrain(bias, 20, max_pow - 20);
              d = (bias > max_pow / 2) ? max_pow - 1 - bias : bias;

              SERIAL_MV(MSG_BIAS, bias);
              SERIAL_MV(MSG_D, d);
              SERIAL_MV(MSG_T_MIN, min);
              SERIAL_MV(MSG_T_MAX, max);
              if (cycles > 2) {
                Ku = (4.0 * d) / (3.14159265 * (max - min) / 2.0);
                Tu = ((float)(t_low + t_high) / 1000.0);
                SERIAL_MV(MSG_KU, Ku);
                SERIAL_EMV(MSG_TU, Tu);
                workKp = 0.6 * Ku;
                workKi = 2 * workKp / Tu;
                workKd = workKp * Tu / 8;

                SERIAL_EM(MSG_CLASSIC_PID);
                SERIAL_MV(MSG_KP, workKp);
                SERIAL_MV(MSG_KI, workKi);
                SERIAL_EMV(MSG_KD, workKd);
              }
              else {
                SERIAL_E;
              }
            }

            #if ENABLED(PIDTEMP)
              if (temp_controller >= 0)
                soft_pwm[temp_controller] = (bias + d) >> 1;
            #endif

            #if ENABLED(PIDTEMPBED)
              if (temp_controller == -1)
                soft_pwm_bed = (bias + d) >> 1;
            #endif

            #if ENABLED(PIDTEMPCOOLER)
              if (temp_controller == -3)
                setPwmCooler((bias + d));
            #endif

            cycles++;

            if(temp_controller == -3)
              max = temp;
            else
              min = temp;
          }
        }
      }
      if (input > temp + MAX_OVERSHOOT_PID_AUTOTUNE && temp_controller >= -1) {
        SERIAL_LM(ER, MSG_PID_TEMP_TOO_HIGH);
        return;
      }
      else if (input < temp + MAX_OVERSHOOT_PID_AUTOTUNE && temp_controller < -1) {
		    SERIAL_LM(ER, MSG_PID_TEMP_TOO_LOW);
		    return;
		  }
      // Every 2 seconds...
      if (ELAPSED(ms, temp_ms + 2000UL)) {
        #if HAS(TEMP_0) || HAS(TEMP_BED) || ENABLED(HEATER_0_USES_MAX6675)
          print_heaterstates();
          SERIAL_E;
        #endif
        #if HAS(TEMP_CHAMBER)
          print_chamberstate();
          SERIAL_E;
        #endif
        #if HAS(TEMP_COOLER)
          print_coolerstate();
          SERIAL_E;
        #endif

        temp_ms = ms;
      } // every 2 seconds

      // Over 2 minutes?
      if (((ms - t1) + (ms - t2)) > (10L*60L*1000L*2L)) {
        SERIAL_LM(ER, MSG_PID_TIMEOUT);
        return;
      }
      if (cycles > ncycles) {
        SERIAL_EM(MSG_PID_AUTOTUNE_FINISHED);

        #if ENABLED(PIDTEMP)
          if (temp_controller >= 0) {
            SERIAL_MV(MSG_KP, workKp);
            SERIAL_MV(MSG_KI, workKi);
            SERIAL_EMV(MSG_KD, workKd);
            if (set_result) {
              PID_PARAM(Kp, temp_controller) = workKp;
              PID_PARAM(Ki, temp_controller) = scalePID_i(workKi);
              PID_PARAM(Kd, temp_controller) = scalePID_d(workKd);
              updatePID();
            }
          }
        #endif

        #if ENABLED(PIDTEMPBED)
          if (temp_controller == -1) {
            SERIAL_EMV("#define DEFAULT_bedKp ", workKp);
            SERIAL_EMV("#define DEFAULT_bedKi ", workKi);
            SERIAL_EMV("#define DEFAULT_bedKd ", workKd);
            if (set_result) {
              bedKp = workKp;
              bedKi = scalePID_i(workKi);
              bedKd = scalePID_d(workKd);
              updatePID();
            }
          }
        #endif

        #if ENABLED(PIDTEMPCOOLER)
          if (temp_controller == -3) {
            SERIAL_EMV("#define DEFAULT_coolerKp ", workKp);
            SERIAL_EMV("#define DEFAULT_coolerKi ", workKi);
            SERIAL_EMV("#define DEFAULT_coolerKd ", workKd);
            if (set_result) {
              coolerKp = workKp;
              coolerKi = scalePID_i(workKi);
              coolerKd = scalePID_d(workKd);
              updatePID();
            }
          }
        #endif

        return;
      }
      lcd_update();
    }
  }
#endif

void updatePID() {
  #if ENABLED(PIDTEMP)
    #if ENABLED(PID_ADD_EXTRUSION_RATE)
      last_e_position = 0;
    #endif
    for (uint8_t h = 0; h < HOTENDS; h++)
      temp_iState_max[h] = PID_INTEGRAL_DRIVE_MAX / PID_PARAM(Ki, h);
  #endif
  #if ENABLED(PIDTEMPBED)
    temp_iState_max_bed = PID_BED_INTEGRAL_DRIVE_MAX / bedKi;
  #endif
  #if ENABLED(PIDTEMPCHAMBER)
	 temp_iState_max_chamber = PID_CHAMBER_INTEGRAL_DRIVE_MAX / chamberKi;
  #endif
  #if ENABLED(PIDTEMPCOOLER)
	 temp_iState_max_cooler = PID_COOLER_INTEGRAL_DRIVE_MAX / coolerKi;
  #endif
}

int getHeaterPower(int heater) {
  return soft_pwm[heater];
}

int getBedPower() {
  return soft_pwm_bed;
}

int getChamberPower() {
  return soft_pwm_chamber;
}

int getCoolerPower() {
  #if ENABLED(FAST_PWM_COOLER)
    return fast_pwm_cooler;
  #else
    return soft_pwm_cooler;
  #endif
}

#if HAS(AUTO_FAN)
  void checkExtruderAutoFans() {
    const int8_t fanPin[] = { EXTRUDER_0_AUTO_FAN_PIN, EXTRUDER_1_AUTO_FAN_PIN, EXTRUDER_2_AUTO_FAN_PIN, EXTRUDER_3_AUTO_FAN_PIN };
    const int fanBit[] = {
                    0,
      AUTO_1_IS_0 ? 0 :               1,
      AUTO_2_IS_0 ? 0 : AUTO_2_IS_1 ? 1 :               2,
      AUTO_3_IS_0 ? 0 : AUTO_3_IS_1 ? 1 : AUTO_3_IS_2 ? 2 : 3
    };
    uint8_t fanState = 0;

    for (uint8_t h = 0; h < HOTENDS; h++) {
      if (current_temperature[h] > EXTRUDER_AUTO_FAN_TEMPERATURE) {
        SBI(fanState, fanBit[h]);
      }
    }

    uint8_t fanDone = 0;
    for (int8_t f = 0; f < COUNT(fanPin); f++) {
      int8_t pin = fanPin[f];
      if (pin >= 0 && !TEST(fanDone, fanBit[f])) {
        unsigned char newFanSpeed = TEST(fanState, fanBit[f]) ? EXTRUDER_AUTO_FAN_SPEED : 0;
        // this idiom allows both digital and PWM fan outputs (see M42 handling).
        digitalWrite(pin, newFanSpeed);
        analogWrite(pin, newFanSpeed);
        SBI(fanDone, fanBit[f]);
      }
    }
  }
#endif // HAS(AUTO_FAN)

//
// Temperature Error Handlers
//
inline void _temp_error(int tc, const char* serial_msg, const char* lcd_msg) {
  static bool killed = false;
  if (IsRunning()) {
    SERIAL_ST(ER, serial_msg);
    if (tc >= 0) {
      SERIAL_M(MSG_STOPPED_HEATER);
      SERIAL_EV((int)tc);
    }
    else if (tc == -1) {
      SERIAL_EM(MSG_STOPPED_BED);
    }
    else if (tc == -2) {
      SERIAL_EM(MSG_STOPPED_CHAMBER);
    }
    else
      SERIAL_EM(MSG_STOPPED_COOLER);

    #if ENABLED(ULTRA_LCD)
      lcd_setalertstatuspgm(lcd_msg);
    #endif
  }

  #if DISABLED(BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE)
    if (!killed) {
      Running = false;
      killed = true;
      kill(lcd_msg);
    }
    else {
      disable_all_heaters(); // paranoia
      disable_all_coolers();
    }
  #endif
}

void max_temp_error(int8_t h) {
  //_temp_error(h, PSTR(MSG_T_MAXTEMP), PSTR(MSG_ERR_MAXTEMP));
}
void min_temp_error(int8_t h) {
  _temp_error(h, PSTR(MSG_T_MINTEMP), PSTR(MSG_ERR_MINTEMP));
}

float get_pid_output(int h) {
  #if HOTENDS <= 1
    UNUSED(h);
    #define _HOTEND_TEST  true
  #else
    #define _HOTEND_TEST  h == active_extruder
  #endif

  float pid_output; //pwm la respuesta final

  // target_temperature[HOTEND_INDEX] //temperatura objetibo
  // current_temperature[HOTEND_INDEX] //temperatura objetibo

  pid_error[HOTEND_INDEX] = target_temperature[HOTEND_INDEX] - current_temperature[HOTEND_INDEX];
  //calculamos p
  //PID_PARAM(kx, HotEnd)// devuelve el valor de kx
  //Kp * la diferencia de temperatura
  pTerm[HOTEND_INDEX] = PID_PARAM(Kp, HOTEND_INDEX) * pid_error[HOTEND_INDEX];


  //For derivative we need real ti cme to calculate speed change rate
  timePrev = Time;                            // the previous time is stored before the actual time read
  Time = millis();                            // actual time read
  elapsedTime = (Time - timePrev) / 1000;
  //calculamos Ki temp_iState[HOTEND_INDEX](desconocido)
  //iTerm + (ki * la diferencia de temperatura)
  iTerm[HOTEND_INDEX] = iTerm[HOTEND_INDEX] + pid_error[HOTEND_INDEX] * (PID_PARAM(Ki, HOTEND_INDEX) * elapsedTime);

  //Now we can calculate the D calue
  dTerm[HOTEND_INDEX] = (pid_error[HOTEND_INDEX] - previous_error[HOTEND_INDEX]) * PID_PARAM(Kd, HOTEND_INDEX) / elapsedTime;

  //Final total PID value is the sum of P + I + D
  pid_output = pTerm[HOTEND_INDEX] + iTerm[HOTEND_INDEX] + dTerm[HOTEND_INDEX];


  //We define PWM range between 0 and 255
  if(pid_output < 0){
    pid_output = 0;
    pid_reset[HOTEND_INDEX] = true;
  }
  if(pid_output > 255){
    pid_output = 255;
    pid_reset[HOTEND_INDEX] = true;
  }

  if (pid_error[HOTEND_INDEX] > 20){
      pid_output = 255;
  }
  if(primer_calentamiento){
    if (pid_error[HOTEND_INDEX] >= 10){
        pid_output = 255;
    }else{
      primer_calentamiento = false;
    }
  }
  if(solo_una_vez_ki){
    //SERIAL_EM("-------------++++++++++++++++++++++");
    iTerm[HOTEND_INDEX] = 0;
    solo_una_vez_ki = false;
  }

  if (pid_error[HOTEND_INDEX] > 30){
    iTerm[HOTEND_INDEX] = 0;
  }
  // SERIAL_MV(" pTerm ", pTerm[HOTEND_INDEX]);
  // SERIAL_MV(" iTerm ", iTerm[HOTEND_INDEX]);
  // SERIAL_EMV(" dTerm ", dTerm[HOTEND_INDEX]);

  //guarda el anterior
  previous_error[HOTEND_INDEX] = pid_error[HOTEND_INDEX];

  return pid_output;
}

#if ENABLED(PIDTEMPBED)
  float get_pid_output_bed() {
    float pid_output;

    #if ENABLED(PID_OPENLOOP)
      pid_output = constrain(target_temperature_bed, 0, MAX_BED_POWER);
    #else
      pid_error_bed = target_temperature_bed - current_temperature_bed;
      pTerm_bed = bedKp * pid_error_bed;
      temp_iState_bed += pid_error_bed;
      temp_iState_bed = constrain(temp_iState_bed, temp_iState_min_bed, temp_iState_max_bed);
      iTerm_bed = bedKi * temp_iState_bed;

      dTerm_bed = K2 * bedKd * (current_temperature_bed - temp_dState_bed) + K1 * dTerm_bed;
      temp_dState_bed = current_temperature_bed;

      pid_output = pTerm_bed + iTerm_bed - dTerm_bed;
      if (pid_output > MAX_BED_POWER) {
        if (pid_error_bed > 0) temp_iState_bed -= pid_error_bed; // conditional un-integration
        pid_output = MAX_BED_POWER;
      }
      else if (pid_output < 0) {
        if (pid_error_bed < 0) temp_iState_bed -= pid_error_bed; // conditional un-integration
        pid_output = 0;
      }
    #endif // PID_OPENLOOP

    #if ENABLED(PID_BED_DEBUG)
      SERIAL_SM(DEB ," PID_BED_DEBUG ");
      SERIAL_MV(": Input ", current_temperature_bed);
      SERIAL_MV(" Output ", pid_output);
      SERIAL_MV(" pTerm ", pTerm_bed);
      SERIAL_MV(" iTerm ", iTerm_bed);
      SERIAL_EMV(" dTerm ", dTerm_bed);
    #endif // PID_BED_DEBUG

    return pid_output;
  }
#endif

#if ENABLED(PIDTEMPCHAMBER)
  float get_pid_output_chamber() {
    float pid_output;

    #if ENABLED(PID_OPENLOOP)
      pid_output = constrain(target_temperature_chamber, 0, MAX_CHAMBER_POWER);
    #else
      pid_error_chamber = target_temperature_chamber - current_temperature_chamber;
      pTerm_chamber = chamberKp * pid_error_chamber;
      temp_iState_chamber += pid_error_chamber;
      temp_iState_chamber = constrain(temp_iState_chamber, temp_iState_min_chamber, temp_iState_max_chamber);
      iTerm_chamber = chamberKi * temp_iState_chamber;

      dTerm_chamber = K2 * chamberKd * (current_temperature_chamber - temp_dState_chamber) + K1 * dTerm_chamber;
      temp_dState_chamber = current_temperature_chamber;

      pid_output = pTerm_chamber + iTerm_chamber - dTerm_chamber;
      if (pid_output > MAX_CHAMBER_POWER) {
        if (pid_error_chamber > 0) temp_iState_chamber -= pid_error_chamber; // conditional un-integration
        pid_output = MAX_CHAMBER_POWER;
      }
      else if (pid_output < 0) {
        if (pid_error_chamber < 0) temp_iState_chamber -= pid_error_chamber; // conditional un-integration
        pid_output = 0;
      }
    #endif // PID_OPENLOOP

    #if ENABLED(PID_CHAMBER_DEBUG)
      SERIAL_SM(DEB ," PID_CHAMBER_DEBUG -1");
      SERIAL_MV(": Input ", current_temperature_chamber);
      SERIAL_MV(" Output ", pid_output);
      SERIAL_MV(" pTerm ", pTerm_chamber);
      SERIAL_MV(" iTerm ", iTerm_chamber);
      SERIAL_EMV(" dTerm ", dTerm_chamber);
    #endif // PID_CHAMBER_DEBUG

    return pid_output;
  }
#endif

#if ENABLED(PIDTEMPCOOLER)
  float get_pid_output_cooler() {
    float pid_output;

    // We need this cause 0 is lower than our current temperature probably.
    if (target_temperature_cooler < COOLER_MINTEMP)
      return 0.0;

    #if ENABLED(PID_OPENLOOP)
      pid_output = constrain(target_temperature_cooler, 0, MAX_COOLER_POWER);
    #else
      //pid_error_cooler = target_temperature_cooler - current_temperature_cooler;
      pid_error_cooler = current_temperature_cooler - target_temperature_cooler;
      pTerm_cooler = coolerKp * pid_error_cooler;
      temp_iState_cooler += pid_error_cooler;
      temp_iState_cooler = constrain(temp_iState_cooler, temp_iState_min_cooler, temp_iState_max_cooler);
      iTerm_cooler = coolerKi * temp_iState_cooler;

      //dTerm_cooler = K2 * coolerKd * (current_temperature_cooler - temp_dState_cooler) + K1 * dTerm_cooler;
      dTerm_cooler = K2 * coolerKd * (temp_dState_cooler - current_temperature_cooler) + K1 * dTerm_cooler;
      temp_dState_cooler = current_temperature_cooler;

      pid_output = pTerm_cooler + iTerm_cooler - dTerm_cooler;
      if (pid_output > MAX_COOLER_POWER) {
        if (pid_error_cooler > 0) temp_iState_cooler -= pid_error_cooler; // conditional un-integration
        pid_output = MAX_COOLER_POWER;
      }
      else if (pid_output < 0) {
        if (pid_error_cooler < 0) temp_iState_cooler -= pid_error_cooler; // conditional un-integration
        pid_output = 0;
      }
    #endif // PID_OPENLOOP

    #if ENABLED(PID_COOLER_DEBUG)
      SERIAL_SM(DEB ," PID_COOLER_DEBUG ");
      SERIAL_MV(": Input ", current_temperature_cooler);
      SERIAL_MV(" Output ", pid_output);
      SERIAL_MV(" pTerm ", pTerm_cooler);
      SERIAL_MV(" iTerm ", iTerm_cooler);
      SERIAL_EMV(" dTerm ", dTerm_cooler);
    #endif //PID_COOLER_DEBUG

    return pid_output;
  }

#endif

/**
 * Manage heating activities for hotends, bed, chamber and cooler
 *  - Acquire updated temperature readings
 *  - Invoke thermal runaway protection
 *  - Manage extruder auto-fan
 *  - Apply filament width to the extrusion rate (may move)
 *  - Update the heated bed PID output value
 */
void manage_temp_controller() {

  if (!temp_meas_ready) return;

  updateTemperaturesFromRawValues();

  #if ENABLED(HEATER_0_USES_MAX6675)
    if (current_temperature[0] > min(HEATER_0_MAXTEMP, 1023)) max_temp_error(0);
    if (current_temperature[0] < max(HEATER_0_MINTEMP, 0.01)) min_temp_error(0);
  #endif

  #if ENABLED(THERMAL_PROTECTION_HOTENDS) || ENABLED(THERMAL_PROTECTION_BED) || ENABLED(THERMAL_PROTECTION_CHAMBER) || ENABLED(THERMAL_PROTECTION_COOLER) || DISABLED(PIDTEMPBED) || DISABLED(PIDTEMPCHAMBER) || DISABLED(PIDTEMPCOOLER) || HAS(AUTO_FAN)
    millis_t ms = millis();
  #endif

  // Loop through all hotends
  for (uint8_t h = 0; h < HOTENDS; h++) {

    #if ENABLED(THERMAL_PROTECTION_HOTENDS)
      thermal_runaway_protection(&thermal_runaway_state_machine[h], &thermal_runaway_timer[h], current_temperature[h], target_temperature[h], h, THERMAL_PROTECTION_PERIOD, THERMAL_PROTECTION_HYSTERESIS);
    #endif

    float pid_output = get_pid_output(h);

    // Check if temperature is within the correct range
    //soft_pwm[h] = current_temperature[h] > minttemp[h] && current_temperature[h] < maxttemp[h] ? (int)pid_output >> 1 : 0;
    soft_pwm[h] = pid_output;

    // Check if the temperature is failing to increase
    #if ENABLED(THERMAL_PROTECTION_HOTENDS)

      // Is it time to check this extruder's heater?
      if (watch_heater_next_ms[h] && ms > watch_heater_next_ms[h]) {
        // Has it failed to increase enough?
        if (degHotend(h) < watch_target_temp[h]) {
          // Stop!
          _temp_error(h, PSTR(MSG_T_HEATING_FAILED), PSTR(MSG_HEATING_FAILED_LCD));
        }
        else {
          // Start again if the target is still far off
          start_watching_heater(h);
        }
      }

    #endif // THERMAL_PROTECTION_HOTENDS

    // Check if the temperature is failing to increase
    #if ENABLED(THERMAL_PROTECTION_BED)

      // Is it time to check the bed?
      if (watch_bed_next_ms && ELAPSED(ms, watch_bed_next_ms)) {
        // Has it failed to increase enough?
        if (degBed() < watch_target_bed_temp) {
          // Stop!
          _temp_error(-1, PSTR(MSG_T_HEATING_FAILED), PSTR(MSG_HEATING_FAILED_LCD));
        }
        else {
          // Start again if the target is still far off
          start_watching_bed();
        }
      }

    #endif // THERMAL_PROTECTION_HOTENDS

    #if ENABLED(TEMP_SENSOR_1_AS_REDUNDANT)
      if (fabs(current_temperature[0] - redundant_temperature) > MAX_REDUNDANT_TEMP_SENSOR_DIFF) {
        _temp_error(0, PSTR(MSG_REDUNDANCY), PSTR(MSG_ERR_REDUNDANT_TEMP));
      }
    #endif

  } // Hotends Loop

  #if HAS(AUTO_FAN)
    if (ELAPSED(ms, next_auto_fan_check_ms)) { // only need to check fan state very infrequently
      checkExtruderAutoFans();
      next_auto_fan_check_ms = ms + 2500UL;
    }
  #endif

  // Control the extruder rate based on the width sensor
  #if ENABLED(FILAMENT_SENSOR)
    if (filament_sensor) {
      meas_shift_index = filwidth_delay_index1 - meas_delay_cm;
      if (meas_shift_index < 0) meas_shift_index += MAX_MEASUREMENT_DELAY + 1;  // loop around buffer if needed

      // Get the delayed info and add 100 to reconstitute to a percent of
      // the nominal filament diameter then square it to get an area
      meas_shift_index = constrain(meas_shift_index, 0, MAX_MEASUREMENT_DELAY);
      float vm = pow((measurement_delay[meas_shift_index] + 100.0) / 100.0, 2);
      NOLESS(vm, 0.01);
      volumetric_multiplier[FILAMENT_SENSOR_EXTRUDER_NUM] = vm;
    }
  #endif // FILAMENT_SENSOR

  #if HAS(TEMP_BED) && DISABLED(PIDTEMPBED)
    if (ms < next_bed_check_ms) return;
    next_bed_check_ms = ms + BED_CHECK_INTERVAL;
  #endif

  #if HAS(TEMP_CHAMBER) && DISABLED(PIDTEMPCHAMBER)
    if (ms < next_chamber_check_ms) return;
    next_chamber_check_ms = ms + CHAMBER_CHECK_INTERVAL;
  #endif

  #if HAS(TEMP_COOLER) && DISABLED(PIDTEMPCOOLER)
    if (ms < next_cooler_check_ms) return;
    next_cooler_check_ms = ms + COOLER_CHECK_INTERVAL;
  #endif

  #if HAS(TEMP_BED)
    #if ENABLED(THERMAL_PROTECTION_BED)
      thermal_runaway_protection(&thermal_runaway_bed_state_machine, &thermal_runaway_bed_timer, current_temperature_bed, target_temperature_bed, -1, THERMAL_PROTECTION_BED_PERIOD, THERMAL_PROTECTION_BED_HYSTERESIS);
    #endif

    #if ENABLED(PIDTEMPBED)
      float pid_output = get_pid_output_bed();

      soft_pwm_bed = current_temperature_bed > BED_MINTEMP && current_temperature_bed < BED_MAXTEMP ? (int)pid_output >> 1 : 0;

    #elif ENABLED(BED_LIMIT_SWITCHING)
      // Check if temperature is within the correct band
      if (current_temperature_bed > BED_MINTEMP && current_temperature_bed < BED_MAXTEMP) {
        if (current_temperature_bed >= target_temperature_bed + BED_HYSTERESIS)
          soft_pwm_bed = 0;
        else if (current_temperature_bed <= target_temperature_bed - BED_HYSTERESIS)
          soft_pwm_bed = MAX_BED_POWER >> 1;
      }
      else {
        soft_pwm_bed = 0;
        WRITE_HEATER_BED(LOW);
      }
    #else // !PIDTEMPBED && !BED_LIMIT_SWITCHING
      // Check if temperature is within the correct range
      if (current_temperature_bed > BED_MINTEMP && current_temperature_bed < BED_MAXTEMP) {
        soft_pwm_bed = current_temperature_bed < target_temperature_bed ? MAX_BED_POWER >> 1 : 0;
      }
      else {
        soft_pwm_bed = 0;
        WRITE_HEATER_BED(LOW);
      }
    #endif
  #endif // HAS(TEMP_BED)

  #if HAS(TEMP_CHAMBER)
    #if ENABLED(THERMAL_PROTECTION_CHAMBER)
      thermal_runaway_protection(&thermal_runaway_chamber_state_machine, &thermal_runaway_chamber_timer, current_temperature_chamber, target_temperature_chamber, -1, THERMAL_PROTECTION_CHAMBER_PERIOD, THERMAL_PROTECTION_CHAMBER_HYSTERESIS);
    #endif

    #if ENABLED(PIDTEMPCHAMBER)
      float pid_output = get_pid_output_chamber();

      soft_pwm_chamber = current_temperature_chamber > CHAMBER_MINTEMP && current_temperature_chamber < CHAMBER_MAXTEMP ? (int)pid_output >> 1 : 0;

    #elif ENABLED(CHAMBER_LIMIT_SWITCHING)
      // Check if temperature is within the correct band
      if (current_temperature_chamber > CHAMBER_MINTEMP && current_temperature_chamber < CHAMBER_MAXTEMP) {
        if (current_temperature_chamber >= target_temperature_chamber + CHAMBER_HYSTERESIS)
          soft_pwm_chamber = 0;
        else if (current_temperature_chamber <= target_temperature_chamber - CHAMBER_HYSTERESIS)
          soft_pwm_chamber = MAX_CHAMBER_POWER >> 1;
      }
      else {
        soft_pwm_chamber = 0;
        WRITE_HEATER_CHAMBER(LOW);
      }
    #else // !PIDTEMPCHAMBER && !CHAMBER_LIMIT_SWITCHING
      // Check if temperature is within the correct range
      if (current_temperature_chamber > CHAMBER_MINTEMP && current_temperature_chamber < CHAMBER_MAXTEMP) {
        soft_pwm_chamber = current_temperature_chamber < target_temperature_chamber ? MAX_CHAMBER_POWER >> 1 : 0;
      }
      else {
        soft_pwm_chamber = 0;
        WRITE_HEATER_CHAMBER(LOW);
      }
    #endif
  #endif // HAS(TEMP_CHAMBER)

  #if HAS(TEMP_COOLER)
    #if ENABLED(THERMAL_PROTECTION_COOLER)
      thermal_runaway_protection(&thermal_runaway_cooler_state_machine, &thermal_runaway_cooler_timer, current_temperature_cooler, target_temperature_cooler, -2, THERMAL_PROTECTION_COOLER_PERIOD, THERMAL_PROTECTION_COOLER_HYSTERESIS);
    #endif

    #if ENABLED(PIDTEMPCOOLER)
      float pid_output = get_pid_output_cooler();

      setPwmCooler(current_temperature_cooler > COOLER_MINTEMP && current_temperature_cooler < COOLER_MAXTEMP ? (int)pid_output : 0);

    #elif ENABLED(COOLER_LIMIT_SWITCHING)
      // Check if temperature is within the correct band
      if (current_temperature_cooler > COOLER_MINTEMP && current_temperature_cooler < COOLER_MAXTEMP) {
        if (current_temperature_cooler >= target_temperature_cooler + COOLER_HYSTERESIS)
          setPwmCooler(MAX_COOLER_POWER);
        else if (current_temperature_cooler <= target_temperature_cooler - COOLER_HYSTERESIS)
          setPwmCooler(0);
      }
      else {
        setPwmCooler(0);
        WRITE_COOLER(LOW);
      }
    #else // COOLER_LIMIT_SWITCHING
      // Check if temperature is within the correct range
      if (current_temperature_cooler > COOLER_MINTEMP && current_temperature_cooler < COOLER_MAXTEMP) {
        setPwmCooler(current_temperature_cooler > target_temperature_cooler ? MAX_COOLER_POWER  : 0);
      }
      else {
        setPwmCooler(0);
        WRITE_COOLER(LOW);
      }
    #endif
  #endif // HAS(TEMP_COOLER)
}

#define PGM_RD_W(x)   (short)pgm_read_word(&x)
// Derived from RepRap FiveD extruder::getTemperature()
// For hot end temperature measurement.
static float analog2temp(int raw, uint8_t h) {
  #if ENABLED(TEMP_SENSOR_1_AS_REDUNDANT)
    if (h > HOTENDS)
  #else
    if (h >= HOTENDS)
  #endif
    {
      SERIAL_SV(ER, h);
      SERIAL_EM(MSG_INVALID_EXTRUDER_NUM);
      kill(PSTR(MSG_KILLED));
      return 0.0;
    }

  #if ENABLED(HEATER_0_USES_MAX6675)
    if (h == 0) return 0.25 * raw;
  #endif

  if (heater_ttbl_map[h] != NULL) {
    float celsius = 0;
    uint8_t i;
    short(*tt)[][2] = (short(*)[][2])(heater_ttbl_map[h]);

    for (i = 1; i < heater_ttbllen_map[h]; i++) {
      if (PGM_RD_W((*tt)[i][0]) > raw) {
        celsius = PGM_RD_W((*tt)[i - 1][1]) +
                  (raw - PGM_RD_W((*tt)[i - 1][0])) *
                  (float)(PGM_RD_W((*tt)[i][1]) - PGM_RD_W((*tt)[i - 1][1])) /
                  (float)(PGM_RD_W((*tt)[i][0]) - PGM_RD_W((*tt)[i - 1][0]));
        break;
      }
    }

    // Overflow: Set to last value in the table
    if (i == heater_ttbllen_map[h]) celsius = PGM_RD_W((*tt)[i - 1][1]);

    return celsius;
  }

  #if HEATER_USES_AD595
    #ifdef __SAM3X8E__
      return ((raw * ((3.3 * 100.0) / 1024.0) / OVERSAMPLENR) * ad595_gain[h]) + ad595_offset[h];
    #else
      return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * ad595_gain[h]) + ad595_offset[h];
    #endif
  #else
    return 0;
  #endif
}

// Derived from RepRap FiveD extruder::getTemperature()
// For bed temperature measurement.
static float analog2tempBed(int raw) {
  #if ENABLED(BED_USES_THERMISTOR)
    float celsius = 0;
    byte i;

    for (i = 1; i < BEDTEMPTABLE_LEN; i++) {
      if (PGM_RD_W(BEDTEMPTABLE[i][0]) > raw) {
        celsius  = PGM_RD_W(BEDTEMPTABLE[i - 1][1]) +
                   (raw - PGM_RD_W(BEDTEMPTABLE[i - 1][0])) *
                   (float)(PGM_RD_W(BEDTEMPTABLE[i][1]) - PGM_RD_W(BEDTEMPTABLE[i - 1][1])) /
                   (float)(PGM_RD_W(BEDTEMPTABLE[i][0]) - PGM_RD_W(BEDTEMPTABLE[i - 1][0]));
        break;
      }
    }

    // Overflow: Set to last value in the table
    if (i == BEDTEMPTABLE_LEN) celsius = PGM_RD_W(BEDTEMPTABLE[i - 1][1]);

    return celsius;

  #elif ENABLED(BED_USES_AD595)
    #ifdef __SAM3X8E__
      return ((raw * ((3.3 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
    #else
      return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
    #endif
  #else
    UNUSED(raw);
    return 0;

  #endif
}

static float analog2tempChamber(int raw) {
  #if ENABLED(CHAMBER_USES_THERMISTOR)
    float celsius = 0;
    byte i;

    for (i = 1; i < CHAMBERTEMPTABLE_LEN; i++) {
      if (PGM_RD_W(CHAMBERTEMPTABLE[i][0]) > raw) {
        celsius  = PGM_RD_W(CHAMBERTEMPTABLE[i - 1][1]) +
                   (raw - PGM_RD_W(CHAMBERTEMPTABLE[i - 1][0])) *
                   (float)(PGM_RD_W(CHAMBERTEMPTABLE[i][1]) - PGM_RD_W(CHAMBERTEMPTABLE[i - 1][1])) /
                   (float)(PGM_RD_W(CHAMBERTEMPTABLE[i][0]) - PGM_RD_W(CHAMBERTEMPTABLE[i - 1][0]));
        break;
      }
    }

    // Overflow: Set to last value in the table
    if (i == CHAMBERTEMPTABLE_LEN) celsius = PGM_RD_W(CHAMBERTEMPTABLE[i - 1][1]);

    return celsius;

  #elif ENABLED(CHAMBER_USES_AD595)
    #ifdef __SAM3X8E__
      return ((raw * ((3.3 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
    #else
      return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
    #endif
  #else
    UNUSED(raw);
    return 0;
  #endif
}

static float analog2tempCooler(int raw) {
  #if ENABLED(COOLER_USES_THERMISTOR)
    float celsius = 0;
    byte i;

    for (i = 1; i < COOLERTEMPTABLE_LEN; i++) {
      if (PGM_RD_W(COOLERTEMPTABLE[i][0]) > raw) {
        celsius  = PGM_RD_W(COOLERTEMPTABLE[i - 1][1]) +
                   (raw - PGM_RD_W(COOLERTEMPTABLE[i - 1][0])) *
                   (float)(PGM_RD_W(COOLERTEMPTABLE[i][1]) - PGM_RD_W(COOLERTEMPTABLE[i - 1][1])) /
                   (float)(PGM_RD_W(COOLERTEMPTABLE[i][0]) - PGM_RD_W(COOLERTEMPTABLE[i - 1][0]));
        break;
      }
    }

    // Overflow: Set to last value in the table
    if (i == COOLERTEMPTABLE_LEN) celsius = PGM_RD_W(COOLERTEMPTABLE[i - 1][1]);

    return celsius;

  #elif ENABLED(COOLER_USES_AD595)
    #ifdef __SAM3X8E__
      return ((raw * ((3.3 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
    #else
      return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
    #endif
  #else
    UNUSED(raw);
    return 0;
  #endif
}


/* Called to get the raw values into the the actual temperatures. The raw values are created in interrupt context,
    and this function is called from normal context as it is too slow to run in interrupts and will block the stepper routine otherwise */
static void updateTemperaturesFromRawValues() {

  #if ENABLED(HEATER_0_USES_MAX6675)
    current_temperature_raw[0] = read_max6675();
  #endif

  for (uint8_t h = 0; h < HOTENDS; h++)
    current_temperature[h] = analog2temp(current_temperature_raw[h], h);

  current_temperature_bed = analog2tempBed(current_temperature_bed_raw);

  current_temperature_chamber = analog2tempChamber(current_temperature_chamber_raw);

  current_temperature_cooler = analog2tempCooler(current_temperature_cooler_raw);

  #if ENABLED(TEMP_SENSOR_1_AS_REDUNDANT)
    redundant_temperature = analog2temp(redundant_temperature_raw, 1);
  #endif
  #if HAS(FILAMENT_SENSOR)
    filament_width_meas = analog2widthFil();
  #endif
  #if HAS(POWER_CONSUMPTION_SENSOR)
    static millis_t last_update = millis();
    millis_t temp_last_update = millis();
    millis_t from_last_update = temp_last_update - last_update;
    static float watt_overflow = 0.0;
    power_consumption_meas = analog2power();
    /*SERIAL_MV("raw:", raw_analog2voltage(), 5);
    SERIAL_MV(" - V:", analog2voltage(), 5);
    SERIAL_MV(" - I:", analog2current(), 5);
    SERIAL_EMV(" - P:", analog2power(), 5);*/
    watt_overflow += (power_consumption_meas * from_last_update) / 3600000.0;
    if (watt_overflow >= 1.0) {
      power_consumption_hour++;
      watt_overflow--;
    }
    last_update = temp_last_update;
  #endif

  #if ENABLED(USE_WATCHDOG)
    // Reset the watchdog after we know we have a temperature measurement.
    watchdog_reset();
  #endif

  CRITICAL_SECTION_START;
  temp_meas_ready = false;
  CRITICAL_SECTION_END;
}


#if ENABLED(FILAMENT_SENSOR)
  // Convert raw Filament Width to millimeters
  float analog2widthFil() {
    return current_raw_filwidth / 16383.0 * 5.0;
    // return current_raw_filwidth;
  }

  // Convert raw Filament Width to a ratio
  int widthFil_to_size_ratio() {
    float temp = filament_width_meas;
    if (temp < MEASURED_LOWER_LIMIT) temp = filament_width_nominal;  //assume sensor cut out
    else NOMORE(temp, MEASURED_UPPER_LIMIT);
    return filament_width_nominal / temp * 100;
  }
#endif

#if HAS(POWER_CONSUMPTION_SENSOR)
  // Convert raw Power Consumption to watt
  float raw_analog2voltage() {
    return (5.0 * current_raw_powconsumption) / (1023.0 * OVERSAMPLENR);
  }

  float analog2voltage() {
    float power_zero_raw = (POWER_ZERO * 1023.0 * OVERSAMPLENR) / 5.0;
    float rel_raw_power = (current_raw_powconsumption < power_zero_raw) ? (2 * power_zero_raw - current_raw_powconsumption) : (current_raw_powconsumption);
    return ((5.0 * rel_raw_power) / (1023.0 * OVERSAMPLENR)) - POWER_ZERO;
  }

  float analog2current() {
    float temp = analog2voltage() / POWER_SENSITIVITY;
    temp = (((100 - POWER_ERROR) / 100) * temp) - POWER_OFFSET;
    return temp > 0 ? temp : 0;
  }

  float analog2power() {
    return (analog2current() * POWER_VOLTAGE * 100) /  POWER_EFFICIENCY;
  }

  float analog2error(float current) {
    float temp1 = (analog2voltage() / POWER_SENSITIVITY - POWER_OFFSET) * POWER_VOLTAGE;
    if(temp1 <= 0) return 0.0;
    float temp2 = (current) * POWER_VOLTAGE;
    if(temp2 <= 0) return 0.0;
    return ((temp2/temp1) - 1) * 100;
  }

  float analog2efficiency(float watt) {
    return (analog2current() * POWER_VOLTAGE * 100) / watt;
  }
#endif

/**
 * Initialize the temperature manager
 * The manager is implemented by periodic calls to manage_temp_controller()
 */
void tp_init() {

  #if MB(RUMBA) && ((TEMP_SENSOR_0==-1)||(TEMP_SENSOR_1==-1)||(TEMP_SENSOR_2==-1)||(TEMP_SENSOR_BED==-1)||(TEMP_SENSOR_COOLER==-1))
    // disable RUMBA JTAG in case the thermocouple extension is plugged on top of JTAG connector
    MCUCR = _BV(JTD);
    MCUCR = _BV(JTD);
  #endif

  // Finish init of mult hotends arrays
  for (uint8_t h = 0; h < HOTENDS; h++) {
    // populate with the first value
    maxttemp[h] = maxttemp[0];
    #if ENABLED(PIDTEMP)
      temp_iState_min[h] = 0.0;
      temp_iState_max[h] = PID_INTEGRAL_DRIVE_MAX / PID_PARAM(Ki, h);
    #endif //PIDTEMP
  }

  #if ENABLED(PIDTEMPBED)
    temp_iState_min_bed = 0.0;
    temp_iState_max_bed = PID_BED_INTEGRAL_DRIVE_MAX / bedKi;
  #endif // PIDTEMPBED

  #if ENABLED(PIDTEMPCHAMBER)
    temp_iState_min_chamber = 0.0;
    temp_iState_max_chamber = PID_CHAMBER_INTEGRAL_DRIVE_MAX / chamberKi;
  #endif

  #if ENABLED(PIDTEMPCOOLER)
    temp_iState_min_cooler = 0.0;
    temp_iState_max_cooler = PID_COOLER_INTEGRAL_DRIVE_MAX / coolerKi;
  #endif

  #if ENABLED(PID_ADD_EXTRUSION_RATE)
    last_e_position = 0;
  #endif

  #if HAS(HEATER_0)
    SET_OUTPUT(HEATER_0_PIN);
  #endif
  #if HAS(HEATER_1)
    SET_OUTPUT(HEATER_1_PIN);
  #endif
  #if HAS(HEATER_2)
    SET_OUTPUT(HEATER_2_PIN);
  #endif
  #if HAS(HEATER_3)
    SET_OUTPUT(HEATER_3_PIN);
  #endif
  #if HAS(HEATER_BED)
    SET_OUTPUT(HEATER_BED_PIN);
  #endif
  #if HAS(HEATER_CHAMBER)
    SET_OUTPUT(HEATER_CHAMBER_PIN);
  #endif
  #if HAS(COOLER)
    SET_OUTPUT(COOLER_PIN);
    #if ENABLED(FAST_PWM_COOLER)
	    setPwmFrequency(COOLER_PIN, 2); // No prescaling. Pwm frequency = F_CPU/256/64
    #endif
  #endif
  #if HAS(FAN)
    SET_OUTPUT(FAN_PIN);
    #if ENABLED(FAST_PWM_FAN)
      setPwmFrequency(FAN_PIN, 1); // No prescaling. Pwm frequency = F_CPU/256/8
    #endif
    #if ENABLED(FAN_SOFT_PWM)
      soft_pwm_fan = fanSpeedSoftPwm / 2;
    #endif
  #endif

  #if ENABLED(HEATER_0_USES_MAX6675)

    #if DISABLED(SDSUPPORT)
      OUT_WRITE(SCK_PIN, LOW);
      OUT_WRITE(MOSI_PIN, HIGH);
      SET_INPUT(MISO_PIN);
      WRITE(MISO_PIN,HIGH);
    #else
      OUT_WRITE(SS_PIN, HIGH);
    #endif

    OUT_WRITE(MAX6675_SS, HIGH);

  #endif // HEATER_0_USES_MAX6675

  // Set analog inputs
  #ifdef __SAM3X8E__
    #define ANALOG_SELECT(pin) startAdcConversion(pinToAdcChannel(pin))
  #else
    #ifdef DIDR2
      #define ANALOG_SELECT(pin) do{ if (pin < 8) SBI(DIDR0, pin); else SBI(DIDR2, pin - 8); }while(0)
    #else
      #define ANALOG_SELECT(pin) do{ SBI(DIDR0, pin); }while(0)
    #endif
  #endif

  // Setup channels
  #ifdef __SAM3X8E__
    // ADC_MR_FREERUN_ON: Free Run Mode. It never waits for any trigger.
    ADC->ADC_MR |= ADC_MR_FREERUN_ON | ADC_MR_LOWRES_BITS_12;
  #else
    ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADIF) | 0x07;
    DIDR0 = 0;
    #ifdef DIDR2
      DIDR2 = 0;
    #endif
  #endif

  #if HAS(TEMP_0)
    ANALOG_SELECT(TEMP_0_PIN);
  #endif
  #if HAS(TEMP_1)
    ANALOG_SELECT(TEMP_1_PIN);
  #endif
  #if HAS(TEMP_2)
    ANALOG_SELECT(TEMP_2_PIN);
  #endif
  #if HAS(TEMP_3)
    ANALOG_SELECT(TEMP_3_PIN);
  #endif

  #if HAS(TEMP_BED)
    ANALOG_SELECT(TEMP_BED_PIN);
  #endif

  #if HAS(TEMP_CHAMBER)
    ANALOG_SELECT(TEMP_CHAMBER_PIN);
  #endif

  #if HAS(TEMP_COOLER)
    ANALOG_SELECT(TEMP_COOLER_PIN);
  #endif

  #if HAS(FILAMENT_SENSOR)
    ANALOG_SELECT(FILWIDTH_PIN);
  #endif

  #if HAS(POWER_CONSUMPTION_SENSOR)
    ANALOG_SELECT(POWER_CONSUMPTION_PIN);
  #endif

  #if HAS(AUTO_FAN_0)
    SET_OUTPUT(EXTRUDER_0_AUTO_FAN_PIN);
    #if ENABLED(FAST_PWM_FAN)
      setPwmFrequency(EXTRUDER_0_AUTO_FAN_PIN, 1); // No prescaling. Pwm frequency = F_CPU/256/8
    #endif
  #endif
  #if HAS(AUTO_FAN_1) && !AUTO_1_IS_0
    SET_OUTPUT(EXTRUDER_1_AUTO_FAN_PIN);
    #if ENABLED(FAST_PWM_FAN)
      setPwmFrequency(EXTRUDER_1_AUTO_FAN_PIN, 1); // No prescaling. Pwm frequency = F_CPU/256/8
    #endif
  #endif
  #if HAS(AUTO_FAN_2) && !AUTO_2_IS_0 && !AUTO_2_IS_1
    SET_OUTPUT(EXTRUDER_2_AUTO_FAN_PIN);
    #if ENABLED(FAST_PWM_FAN)
      setPwmFrequency(EXTRUDER_2_AUTO_FAN_PIN, 1); // No prescaling. Pwm frequency = F_CPU/256/8
    #endif
  #endif
  #if HAS(AUTO_FAN_3) && !AUTO_3_IS_0 && !AUTO_3_IS_1 && !AUTO_3_IS_2
    SET_OUTPUT(EXTRUDER_3_AUTO_FAN_PIN);
    #if ENABLED(FAST_PWM_FAN)
      setPwmFrequency(EXTRUDER_3_AUTO_FAN_PIN, 1); // No prescaling. Pwm frequency = F_CPU/256/8
    #endif
  #endif

  // Use timer0 for temperature measurement
  // Interleave temperature interrupt with millies interrupt
  #ifdef __SAM3X8E__
    HAL_temp_timer_start(TEMP_TIMER_NUM);
    HAL_timer_enable_interrupt (TEMP_TIMER_NUM);
  #else
    OCR0B = 128;
    SBI(TIMSK0, OCIE0B);
  #endif

  // Wait for temperature measurement to settle
  HAL::delayMilliseconds(250);

  #define TEMP_MIN_ROUTINE(NR) \
    minttemp[NR] = HEATER_ ## NR ## _MINTEMP; \
    while(analog2temp(minttemp_raw[NR], NR) < HEATER_ ## NR ## _MINTEMP) { \
      if (HEATER_ ## NR ## _RAW_LO_TEMP < HEATER_ ## NR ## _RAW_HI_TEMP) \
        minttemp_raw[NR] += OVERSAMPLENR; \
      else \
        minttemp_raw[NR] -= OVERSAMPLENR; \
    }
  #define TEMP_MAX_ROUTINE(NR) \
    maxttemp[NR] = HEATER_ ## NR ## _MAXTEMP; \
    while(analog2temp(maxttemp_raw[NR], NR) > HEATER_ ## NR ## _MAXTEMP) { \
      if (HEATER_ ## NR ## _RAW_LO_TEMP < HEATER_ ## NR ## _RAW_HI_TEMP) \
        maxttemp_raw[NR] -= OVERSAMPLENR; \
      else \
        maxttemp_raw[NR] += OVERSAMPLENR; \
    }

  #if ENABLED(HEATER_0_MINTEMP)
    TEMP_MIN_ROUTINE(0);
  #endif
  #if ENABLED(HEATER_0_MAXTEMP)
    TEMP_MAX_ROUTINE(0);
  #endif
  #if HOTENDS > 1
    #if ENABLED(HEATER_1_MINTEMP)
      TEMP_MIN_ROUTINE(1);
    #endif
    #if ENABLED(HEATER_1_MAXTEMP)
      TEMP_MAX_ROUTINE(1);
    #endif
    #if HOTENDS > 2
      #if ENABLED(HEATER_2_MINTEMP)
        TEMP_MIN_ROUTINE(2);
      #endif
      #if ENABLED(HEATER_2_MAXTEMP)
        TEMP_MAX_ROUTINE(2);
      #endif
      #if HOTENDS > 3
        #if ENABLED(HEATER_3_MINTEMP)
          TEMP_MIN_ROUTINE(3);
        #endif
        #if ENABLED(HEATER_3_MAXTEMP)
          TEMP_MAX_ROUTINE(3);
        #endif
      #endif // HOTENDS > 3
    #endif // HOTENDS > 2
  #endif // HOTENDS > 1

  #if ENABLED(BED_MINTEMP)
    while(analog2tempBed(bed_minttemp_raw) < BED_MINTEMP) {
      #if HEATER_BED_RAW_LO_TEMP < HEATER_BED_RAW_HI_TEMP
        bed_minttemp_raw += OVERSAMPLENR;
      #else
        bed_minttemp_raw -= OVERSAMPLENR;
      #endif
    }
  #endif //BED_MINTEMP
  #if ENABLED(BED_MAXTEMP)
    while(analog2tempBed(bed_maxttemp_raw) > BED_MAXTEMP) {
      #if HEATER_BED_RAW_LO_TEMP < HEATER_BED_RAW_HI_TEMP
        bed_maxttemp_raw -= OVERSAMPLENR;
      #else
        bed_maxttemp_raw += OVERSAMPLENR;
      #endif
    }
  #endif // BED_MAXTEMP

  #if ENABLED(CHAMBER_MINTEMP)
    while(analog2tempChamber(chamber_minttemp_raw) < CHAMBER_MINTEMP) {
      #if CHAMBER_RAW_LO_TEMP < HEATER_CHAMBER_RAW_HI_TEMP
        chamber_minttemp_raw += OVERSAMPLENR;
      #else
        chamber_minttemp_raw -= OVERSAMPLENR;
      #endif
    }
  #endif // CHAMBER_MINTEMP
  #if ENABLED(CHAMBER_MAXTEMP)
    while(analog2tempCooler(chamber_maxttemp_raw) > CHAMBER_MAXTEMP) {
      #if CHAMBER_RAW_LO_TEMP < CHAMBER_RAW_HI_TEMP
        chamber_maxttemp_raw -= OVERSAMPLENR;
      #else
        chamber_maxttemp_raw += OVERSAMPLENR;
      #endif
    }
  #endif // BED_MAXTEMP

  #if ENABLED(COOLER_MINTEMP)
    while(analog2tempCooler(cooler_minttemp_raw) < COOLER_MINTEMP) {
      #if COOLER_RAW_LO_TEMP < HEATER_COOLER_RAW_HI_TEMP
        cooler_minttemp_raw += OVERSAMPLENR;
      #else
        cooler_minttemp_raw -= OVERSAMPLENR;
      #endif
    }
  #endif // COOLER_MINTEMP
  #if ENABLED(COOLER_MAXTEMP)
    while(analog2tempCooler(cooler_maxttemp_raw) > COOLER_MAXTEMP) {
      #if COOLER_RAW_LO_TEMP < COOLER_RAW_HI_TEMP
        cooler_maxttemp_raw -= OVERSAMPLENR;
      #else
        cooler_maxttemp_raw += OVERSAMPLENR;
      #endif
    }
  #endif // COOLER_MAXTEMP
}

#if ENABLED(THERMAL_PROTECTION_HOTENDS)
  /**
   * Start Heating Sanity Check for hotends that are below
   * their target temperature by a configurable margin.
   * This is called when the temperature is set. (M104, M109)
   */
  void start_watching_heater(int h) {
    if (degHotend(h) < degTargetHotend(h) - (WATCH_TEMP_INCREASE + TEMP_HYSTERESIS + 1)) {
      watch_target_temp[h] = degHotend(h) + WATCH_TEMP_INCREASE;
      watch_heater_next_ms[h] = millis() + WATCH_TEMP_PERIOD * 1000UL;
    }
    else
      watch_heater_next_ms[h] = 0;
  }
#endif

#if ENABLED(THERMAL_PROTECTION_BED)
  /**
   * Start Heating Sanity Check for bed that are below
   * their target temperature by a configurable margin.
   * This is called when the temperature is set. (M140, M190)
   */
  void start_watching_bed() {
    if (degBed() < degTargetBed() - (WATCH_BED_TEMP_INCREASE + TEMP_BED_HYSTERESIS + 1)) {
      watch_target_bed_temp = degBed() + WATCH_BED_TEMP_INCREASE;
      watch_bed_next_ms = millis() + (WATCH_BED_TEMP_PERIOD) * 1000UL;
    }
    else
      watch_bed_next_ms = 0;
  }
#endif

#if ENABLED(THERMAL_PROTECTION_CHAMBER)
  /**
   * Start Cooling Sanity Check for chamber that are below
   * their target temperature by a configurable margin.
   * This is called when the temperature is set. (M141)
   */
  void start_watching_chamber() {
    if (degCooler() > degTargetCooler() - (WATCH_TEMP_CHAMBER_DECREASE - TEMP_CHAMBER_HYSTERESIS - 1)) {
      watch_target_temp_chamber = degChamber() - WATCH_CHAMBER_TEMP_DECREASE;
      watch_chamber_next_ms = millis() + WATCH_TEMP_CHAMBER_PERIOD * 1000UL;
    }
    else
      watch_chamber_next_ms = 0;
  }
#endif

#if ENABLED(THERMAL_PROTECTION_COOLER)
  /**
   * Start Cooling Sanity Check for hotends that are below
   * their target temperature by a configurable margin.
   * This is called when the temperature is set. (M142)
   */
  void start_watching_cooler() {
    if (degCooler() > degTargetCooler() - (WATCH_TEMP_COOLER_DECREASE - TEMP_COOLER_HYSTERESIS - 1)) {
      watch_target_temp_cooler = degCooler() - WATCH_COOLER_TEMP_DECREASE;
      watch_cooler_next_ms = millis() + WATCH_TEMP_COOLER_PERIOD * 1000UL;
    }
    else
      watch_cooler_next_ms = 0;
  }
#endif

#if ENABLED(THERMAL_PROTECTION_HOTENDS) || ENABLED(THERMAL_PROTECTION_BED) || ENABLED(THERMAL_PROTECTION_CHAMBER) || ENABLED(THERMAL_PROTECTION_COOLER)
  void thermal_runaway_protection(TRState* state, millis_t* timer, float temperature, float target_temperature, int temp_controller_id, int period_seconds, int hysteresis_degc) {

    static float tr_target_temperature[HOTENDS + 3] = { 0.0 };

    /*
        SERIAL_M("Thermal Thermal Runaway Running. Heater ID: ");
        if (heater_id < 0) SERIAL_M("bed"); else SERIAL_V(heater_id);
        SERIAL_MV(" ;  State:", *state);
        SERIAL_MV(" ;  Timer:", *timer);
        SERIAL_MV(" ;  Temperature:", temperature);
        SERIAL_EMV(" ;  Target Temp:", target_temperature);
    */
    int temp_controller_index;

    if(temp_controller_id >= 0)
      temp_controller_index = temp_controller_id;
    else if(temp_controller_id == -1)
      temp_controller_index = HOTENDS; // BED
    else if(temp_controller_id == -2)
      temp_controller_index = HOTENDS + 1; // CHAMBER
    else
      temp_controller_index = HOTENDS + 2; // COOLER

    // If the target temperature changes, restart
    if (tr_target_temperature[temp_controller_index] != target_temperature)
      tr_target_temperature[temp_controller_index] = target_temperature;

    *state = target_temperature > 0 ? TRFirstRunning : TRInactive;

    switch (*state) {
      // Inactive state waits for a target temperature to be set
      case TRInactive: break;
      // When first heating/cooling, wait for the temperature to be reached then go to Stable state
      case TRFirstRunning:
        if (temperature < tr_target_temperature[temp_controller_index] && temp_controller_index > HOTENDS) break;
        else if ((temperature > tr_target_temperature[temp_controller_index] && temp_controller_index <= HOTENDS)) break;
        *state = TRStable;
      // While the temperature is stable watch for a bad temperature
      case TRStable:
        if (temp_controller_index <= HOTENDS) { // HOTENDS
          if (temperature < tr_target_temperature[temp_controller_index] - hysteresis_degc && ELAPSED(millis(), *timer))
            *state = TRRunaway;
          else {
            *timer = millis() + period_seconds * 1000UL;
            break;
          }
        }
        else { // COOLERS
          if (temperature > tr_target_temperature[temp_controller_index] + hysteresis_degc && ELAPSED(millis(), *timer))
            *state = TRRunaway;
          else {
            *timer = millis() + period_seconds * 1000UL;
            break;
          }
        }
      case TRRunaway:
        _temp_error(temp_controller_id, PSTR(MSG_T_THERMAL_RUNAWAY), PSTR(MSG_THERMAL_RUNAWAY));
    }
  }
#endif // THERMAL_PROTECTION_HOTENDS || THERMAL_PROTECTION_BED || THERMAL_PROTECTION_CHAMBER || THERMAL_PROTECTION_COOLER

void disable_all_heaters() {
  for (uint8_t h = 0; h < HOTENDS; h++) setTargetHotend(0, h);
  setTargetBed(0);
  setTargetChamber(0);

  // If all heaters go down then for sure our print job has stopped
  print_job_counter.stop();

  #define DISABLE_HEATER(NR) { \
    setTargetHotend(0, NR); \
    soft_pwm[NR] = 0; \
    WRITE_HEATER_ ## NR (LOW); \
  }

  #if HAS(TEMP_0)
    target_temperature[0] = 0;
    soft_pwm[0] = 0;
    WRITE_HEATER_0P(LOW); // Should HEATERS_PARALLEL apply here? Then change to DISABLE_HEATER(0)
  #endif

  #if HOTENDS > 1 && HAS(TEMP_1)
    DISABLE_HEATER(1);
  #endif

  #if HOTENDS > 2 && HAS(TEMP_2)
    DISABLE_HEATER(2);
  #endif

  #if HOTENDS > 3 && HAS(TEMP_3)
    DISABLE_HEATER(3);
  #endif

  #if HAS(TEMP_BED)
    target_temperature_bed = 0;
    soft_pwm_bed = 0;
    #if HAS(HEATER_BED)
      WRITE_HEATER_BED(LOW);
    #endif
  #endif

  #if HAS(TEMP_CHAMBER)
    target_temperature_chamber = 0;
    soft_pwm_chamber = 0;
    #if HAS(HEATER_CHAMBER)
      WRITE_HEATER_CHAMBER(LOW);
    #endif
  #endif
}

void disable_all_coolers() {
  setTargetCooler(0);

  // if cooler go down the print job is stopped
  print_job_counter.stop();

  #if ENABLED(LASER)
    // No laser firing with no coolers running! (paranoia)
    laser_extinguish();
  #endif

  #if HAS(TEMP_COOLER)
    target_temperature_cooler = 0;
    setPwmCooler(0);
    #if HAS(COOLER) && !ENABLED(FAST_PWM_COOLER)
      WRITE_COOLER(LOW);
    #endif
  #endif
}

#if ENABLED(HEATER_0_USES_MAX6675)

  #define MAX6675_HEAT_INTERVAL 250u

  #if ENABLED(MAX6675_IS_MAX31855)
    uint32_t max6675_temp = 2000;
    #define MAX6675_ERROR_MASK 7
    #define MAX6675_DISCARD_BITS 18
  #else
    uint16_t max6675_temp = 2000;
    #define MAX6675_ERROR_MASK 4
    #define MAX6675_DISCARD_BITS 3
  #endif

  int read_max6675() {

    static millis_t next_max6675_ms = 0;

    millis_t ms = millis();

    if (PENDING(ms, next_max6675_ms)) return (int)max6675_temp;

    next_max6675_ms = ms + MAX6675_HEAT_INTERVAL;

    #ifdef __SAM3X8E__
      HAL::spiBegin();
      HAL::spiInit(2);
    #else
      CBI(
        #ifdef PRR
          PRR
        #elif defined(PRR0)
          PRR0
        #endif
          , PRSPI);
      SPCR = _BV(MSTR) | _BV(SPE) | _BV(SPR0);
    #endif

    HAL::digitalWrite(MAX6675_SS, 0);  // enable TT_MAX6675

    // ensure 100ns delay - a bit extra is fine
    #ifdef __SAM3X8E__
      HAL_delayMicroseconds(1);
    #else
      asm("nop");//50ns on 20Mhz, 62.5ns on 16Mhz
      asm("nop");//50ns on 20Mhz, 62.5ns on 16Mhz
    #endif

    // Read a big-endian temperature value
    max6675_temp = 0;
    for (uint8_t i = sizeof(max6675_temp); i--;) {
      #ifdef __SAM3X8E__
        max6675_temp = HAL::spiReceive();
      #else
        SPDR = 0;
        for (;!TEST(SPSR, SPIF););
        max6675_temp |= SPDR;
      #endif
      if (i > 0) max6675_temp <<= 8; // shift left if not the last byte
    }

    HAL::digitalWrite(MAX6675_SS, 1); // disable TT_MAX6675

    if (max6675_temp & MAX6675_ERROR_MASK)
      max6675_temp = 4000; // thermocouple open
    else
      max6675_temp >>= MAX6675_DISCARD_BITS;

    return (int)max6675_temp;
  }
#endif // HEATER_0_USES_MAX6675

/**
 * Get raw temperatures
 */
#ifdef __SAM3X8E__
  static int calc_raw_temp_value(uint8_t temp_id) {
    raw_median_temp[temp_id][median_counter] = (raw_temp_value[temp_id] - (min_temp[temp_id] + max_temp[temp_id]));
    sum = 0;
    for(int i = 0; i < MEDIAN_COUNT; i++) sum += raw_median_temp[temp_id][i];
    return (sum / MEDIAN_COUNT + CORRECTION_FOR_RAW_TEMP) >> 2;
  }

  static int calc_raw_temp_bed_value() {
    raw_median_temp[4][median_counter] = (raw_temp_bed_value - (min_temp[4] + max_temp[4]));
    sum = 0;
    for(int i = 0; i < MEDIAN_COUNT; i++) sum += raw_median_temp[4][i];
    return (sum / MEDIAN_COUNT + CORRECTION_FOR_RAW_TEMP) >> 2;
  }

  static int calc_raw_temp_chamber_value() {
    raw_median_temp[5][median_counter] = (raw_temp_chamber_value - (min_temp[5] + max_temp[5]));
    sum = 0;
    for(int i = 0; i < MEDIAN_COUNT; i++) sum += raw_median_temp[5][i];
    return (sum / MEDIAN_COUNT + CORRECTION_FOR_RAW_TEMP) >> 2;
  }

  static int calc_raw_temp_cooler_value() {
    raw_median_temp[6][median_counter] = (raw_temp_cooler_value - (min_temp[6] + max_temp[6]));
    sum = 0;
    for(int i = 0; i < MEDIAN_COUNT; i++) sum += raw_median_temp[6][i];
    return (sum / MEDIAN_COUNT + CORRECTION_FOR_RAW_TEMP) >> 2;
  }
#endif

static void set_current_temp_raw() {
  #if HAS(TEMP_0) && DISABLED(HEATER_0_USES_MAX6675)
    #ifdef __SAM3X8E__
      current_temperature_raw[0] = calc_raw_temp_value(0);
    #else
      current_temperature_raw[0] = raw_temp_value[0];
    #endif
  #endif
  #if HAS(TEMP_1)
    #if ENABLED(TEMP_SENSOR_1_AS_REDUNDANT)
      #ifdef __SAM3X8E__
        redundant_temperature_raw = calc_raw_temp_value(1);
      #else
        redundant_temperature_raw = raw_temp_value[1];
      #endif
    #else
      #ifdef __SAM3X8E__
        current_temperature_raw[1] = calc_raw_temp_value(1);
      #else
        current_temperature_raw[1] = raw_temp_value[1];
      #endif
    #endif
    #if HAS(TEMP_2)
      #ifdef __SAM3X8E__
        current_temperature_raw[2] = calc_raw_temp_value(2);
      #else
        current_temperature_raw[2] = raw_temp_value[2];
      #endif
      #if HAS(TEMP_3)
        #ifdef __SAM3X8E__
          current_temperature_raw[3] = calc_raw_temp_value(3);
        #else
          current_temperature_raw[3] = raw_temp_value[3];
        #endif
      #endif
    #endif
  #endif

  #ifdef __SAM3X8E__
    current_temperature_bed_raw = calc_raw_temp_bed_value();
    current_temperature_chamber_raw = calc_raw_temp_chamber_value();
    current_temperature_cooler_raw = calc_raw_temp_cooler_value();
  #else
    current_temperature_bed_raw = raw_temp_bed_value;
    current_temperature_chamber_raw = raw_temp_chamber_value;
    current_temperature_cooler_raw = raw_temp_cooler_value;
  #endif

  #if HAS(POWER_CONSUMPTION_SENSOR)
    #ifdef __SAM3X8E__
      current_raw_powconsumption = calc_raw_powconsumption_value();
    #else
      current_raw_powconsumption = raw_powconsumption_value;
    #endif
  #endif

  #ifdef __SAM3X8E__
    // Reset min/max-holder
    for (uint8_t i = 0; i < 7; i++) {
      min_temp[i] = RAW_MIN_TEMP_DEFAULT;
      max_temp[i] = 0;
    }

    median_counter++;
    if (median_counter >= MEDIAN_COUNT) median_counter = 0;
  #endif

  temp_meas_ready = true;
}

/**
 * Timer 0 is shared with millies
 *  - Manage PWM to all the heaters, coolers and fan
 *  - Update the raw temperature values
 *  - Check new temperature values for MIN/MAX errors
 *  - Step the babysteps value for each axis towards 0
 */
#ifdef __SAM3X8E__
  HAL_TEMP_TIMER_ISR {
#else
  ISR(TIMER0_COMPB_vect) {
#endif
  //these variables are only accesible from the ISR, but static, so they don't lose their value
  static unsigned char temp_count = 0;
  static TempState temp_state = StartupDelay;
  static unsigned char pwm_count = _BV(SOFT_PWM_SCALE);
  #ifdef __SAM3X8E__
    static int temp_read = 0;
    static bool first_start = true;
  #endif

  // Static members for each heater
  #if ENABLED(SLOW_PWM_HEATERS)
    static unsigned char slow_pwm_count = 0;
    #define ISR_STATICS(n) \
      static unsigned char soft_pwm_ ## n; \
      static unsigned char state_heater_ ## n = 0; \
      static unsigned char state_timer_heater_ ## n = 0
  #else
    #define ISR_STATICS(n) static unsigned char soft_pwm_ ## n
  #endif

  // Statics per heater
  ISR_STATICS(0);
  #if (HOTENDS > 1) || ENABLED(HEATERS_PARALLEL)
    ISR_STATICS(1);
    #if HOTENDS > 2
      ISR_STATICS(2);
      #if HOTENDS > 3
        ISR_STATICS(3);
      #endif
    #endif
  #endif
  #if HAS(HEATER_BED)
    ISR_STATICS(BED);
  #endif
  #if HAS(HEATER_CHAMBER)
    ISR_STATICS(CHAMBER);
  #endif
  #if HAS(COOLER) && !ENABLED(FAST_PWM_COOLER)
    ISR_STATICS(COOLER);
  #endif

  #if HAS(FILAMENT_SENSOR)
    static unsigned long raw_filwidth_value = 0;
  #endif

  #ifdef __SAM3X8E__
    // Initialize some variables only at start!
    if (first_start) {
 	    for (uint8_t i = 0; i < 7; i++) {
        for (int j = 0; j < MEDIAN_COUNT; j++) raw_median_temp[i][j] = RAW_MEDIAN_TEMP_DEFAULT;
        max_temp[i] = 0;
        min_temp[i] = RAW_MIN_TEMP_DEFAULT;
      }
      first_start = false;
      SERIAL_EM("First start for temperature finished.");
    }

    HAL_timer_isr_status (TEMP_TIMER_COUNTER, TEMP_TIMER_CHANNEL);
  #endif

  #if DISABLED(SLOW_PWM_HEATERS)
    /**
     * standard PWM modulation
     */
    if (pwm_count == 0) {
      soft_pwm_0 = soft_pwm[0];
      if (soft_pwm_0 > 0) {
        WRITE_HEATER_0(1);
      }
      else WRITE_HEATER_0P(0); // If HEATERS_PARALLEL should apply, change to WRITE_HEATER_0

      #if HOTENDS > 1
        soft_pwm_1 = soft_pwm[1];
        WRITE_HEATER_1(soft_pwm_1 > 0 ? 1 : 0);
        #if HOTENDS > 2
          soft_pwm_2 = soft_pwm[2];
          WRITE_HEATER_2(soft_pwm_2 > 0 ? 1 : 0);
          #if HOTENDS > 3
            soft_pwm_3 = soft_pwm[3];
            WRITE_HEATER_3(soft_pwm_3 > 0 ? 1 : 0);
          #endif
        #endif
      #endif

      #if HAS(HEATER_BED)
        soft_pwm_BED = soft_pwm_bed;
        WRITE_HEATER_BED(soft_pwm_BED > 0 ? 1 : 0);
      #endif

      #if HAS(HEATER_CHAMBER)
        soft_pwm_CHAMBER = soft_pwm_chamber;
        WRITE_HEATER_CHAMBER(soft_pwm_CHAMBER > 0 ? 1 : 0);
      #endif

      #if HAS(COOLER) && !ENABLED(FAST_PWM_COOLER)
        soft_pwm_COOLER = soft_pwm_cooler;
        WRITE_COOLER(soft_pwm_COOLER > 0 ? 1 : 0);
      #endif

      #if ENABLED(FAN_SOFT_PWM)
        soft_pwm_fan = fanSpeedSoftPwm / 2;
        WRITE_FAN(soft_pwm_fan > 0 ? 1 : 0);
      #endif
    }

    if (soft_pwm_0 < pwm_count) { WRITE_HEATER_0(0); }
    #if HOTENDS > 1
      if (soft_pwm_1 < pwm_count) WRITE_HEATER_1(0);
      #if HOTENDS > 2
        if (soft_pwm_2 < pwm_count) WRITE_HEATER_2(0);
        #if HOTENDS > 3
          if (soft_pwm_3 < pwm_count) WRITE_HEATER_3(0);
        #endif
      #endif
    #endif

    #if HAS(HEATER_BED)
      if (soft_pwm_BED < pwm_count) WRITE_HEATER_BED(0);
    #endif

    #if HAS(HEATER_CHAMBER)
      if (soft_pwm_CHAMBER < pwm_count) WRITE_HEATER_CHAMBER(0);
    #endif

    #if HAS(COOLER) && !ENABLED(FAST_PWM_COOLER)
      if (soft_pwm_COOLER < pwm_count ) WRITE_COOLER(0);
    #endif

    #if ENABLED(FAN_SOFT_PWM)
      if (soft_pwm_fan < pwm_count) WRITE_FAN(0);
    #endif

    // 488.28 Hz (or 1:976.56, 2:1953.12, 3:3906.25, 4:7812.5, 5:7812.5 6:15625, 6:15625 7:31250)
    pwm_count += _BV(SOFT_PWM_SCALE);
    pwm_count &= 0x7f;

  #else // SLOW_PWM_HEATERS

    /**
     * SLOW PWM HEATERS
     *
     * for heaters drived by relay
     */
    #if DISABLED(MIN_STATE_TIME)
      #define MIN_STATE_TIME 16 // MIN_STATE_TIME * 65.5 = time in milliseconds
    #endif

    // Macros for Slow PWM timer logic - HEATERS_PARALLEL applies
    #define _SLOW_PWM_ROUTINE(NR, src) \
      soft_pwm_ ## NR = src; \
      if (soft_pwm_ ## NR > 0) { \
        if (state_timer_heater_ ## NR == 0) { \
          if (state_heater_ ## NR == 0) state_timer_heater_ ## NR = MIN_STATE_TIME; \
          state_heater_ ## NR = 1; \
          WRITE_HEATER_ ## NR(1); \
        } \
      } \
      else { \
        if (state_timer_heater_ ## NR == 0) { \
          if (state_heater_ ## NR == 1) state_timer_heater_ ## NR = MIN_STATE_TIME; \
          state_heater_ ## NR = 0; \
          WRITE_HEATER_ ## NR(0); \
        } \
      }
    #define SLOW_PWM_ROUTINE(n) _SLOW_PWM_ROUTINE(n, soft_pwm[n])

    #define PWM_OFF_ROUTINE(NR) \
      if (soft_pwm_ ## NR < slow_pwm_count) { \
        if (state_timer_heater_ ## NR == 0) { \
          if (state_heater_ ## NR == 1) state_timer_heater_ ## NR = MIN_STATE_TIME; \
          state_heater_ ## NR = 0; \
          WRITE_HEATER_ ## NR (0); \
        } \
      }

    if (slow_pwm_count == 0) {

      SLOW_PWM_ROUTINE(0); // HOTEND 0
      #if HOTENDS > 1
        SLOW_PWM_ROUTINE(1); // HOTEND 1
        #if HOTENDS > 2
          SLOW_PWM_ROUTINE(2); // HOTEND 2
          #if HOTENDS > 3
            SLOW_PWM_ROUTINE(3); // HOTEND 3
          #endif
        #endif
      #endif

      #if HAS(HEATER_BED)
        _SLOW_PWM_ROUTINE(BED, soft_pwm_bed); // BED
      #endif

      #if HAS(HEATER_CHAMBER)
        _SLOW_PWM_ROUTINE(CHAMBER, soft_pwm_chamber); // CHAMBER
      #endif

      #if HAS(COOLER) && !ENABLED(FAST_PWM_COOLER)
        _SLOW_PWM_ROUTINE(COOLER, soft_pwm_cooler); // COOLER
      #endif

    } // slow_pwm_count == 0

    PWM_OFF_ROUTINE(0); // HOTEND 0
    #if HOTENDS > 1
      PWM_OFF_ROUTINE(1); // HOTEND 1
      #if HOTENDS > 2
        PWM_OFF_ROUTINE(2); // HOTEND 2
        #if HOTENDS > 3
          PWM_OFF_ROUTINE(3); // HOTEND 3
        #endif
      #endif
    #endif

    #if HAS(HEATER_BED)
      PWM_OFF_ROUTINE(BED); // BED
    #endif

    #if HAS(HEATER_CHAMBER)
      PWM_OFF_ROUTINE(CHAMBER); // CHAMBER
    #endif

    #if HAS(COOLER) && !ENABLED(FAST_PWM_COOLER)
      PWM_OFF_ROUTINE(COOLER); // COOLER
    #endif

    #if ENABLED(FAN_SOFT_PWM)
      if (pwm_count == 0) {
        soft_pwm_fan = fanSpeedSoftPwm / 2;
        WRITE_FAN(soft_pwm_fan > 0 ? 1 : 0);
      }
      if (soft_pwm_fan < pwm_count) WRITE_FAN(0);
    #endif // FAN_SOFT_PWM

    pwm_count += _BV(SOFT_PWM_SCALE);
    pwm_count &= 0x7f;

    // increment slow_pwm_count only every 64 pwm_count circa 65.5ms
    if ((pwm_count % 64) == 0) {
      slow_pwm_count++;
      slow_pwm_count &= 0x7f;

      // HOTEND 0
      if (state_timer_heater_0 > 0) state_timer_heater_0--;
      #if HOTENDS > 1    // HOTEND 1
        if (state_timer_heater_1 > 0) state_timer_heater_1--;
        #if HOTENDS > 2    // HOTEND 2
          if (state_timer_heater_2 > 0) state_timer_heater_2--;
          #if HOTENDS > 3    // HOTEND 3
            if (state_timer_heater_3 > 0) state_timer_heater_3--;
          #endif
        #endif
      #endif

      #if HAS(HEATER_BED)
        if (state_timer_heater_BED > 0) state_timer_heater_BED--;
      #endif

      #if HAS(HEATER_CHAMBER)
        if (state_timer_heater_CHAMBER > 0) state_timer_heater_CHAMBER--;
      #endif

      #if HAS(COOLER) && !ENABLED(FAST_PWM_COOLER)
        if(state_timer_heater_COOLER > 0) state_timer_heater_COOLER--;
      #endif
    } // (pwm_count % 64) == 0

  #endif // SLOW_PWM_HEATERS

  #ifdef __SAM3X8E__
    #define SET_RAW_TEMP_VALUE(temp_id) temp_read = getAdcFreerun(pinToAdcChannel(TEMP_## temp_id ##_PIN)); \
      raw_temp_value[temp_id] += temp_read; \
      max_temp[temp_id] = max(max_temp[temp_id], temp_read); \
      min_temp[temp_id] = min(min_temp[temp_id], temp_read)

    #define SET_RAW_TEMP_BED_VALUE() temp_read = getAdcFreerun(pinToAdcChannel(TEMP_BED_PIN)); \
      raw_temp_bed_value += temp_read; \
      max_temp[4] = max(max_temp[4], temp_read); \
      min_temp[4] = min(min_temp[4], temp_read)

    #define SET_RAW_TEMP_CHAMBER_VALUE() temp_read = getAdcFreerun(pinToAdcChannel(TEMP_CHAMBER_PIN)); \
      raw_temp_chamber_value += temp_read; \
      max_temp[5] = max(max_temp[5], temp_read); \
      min_temp[5] = min(min_temp[5], temp_read)

    #define SET_RAW_TEMP_COOLER_VALUE() temp_read = getAdcFreerun(pinToAdcChannel(TEMP_COOLER_PIN)); \
      raw_temp_cooler_value += temp_read; \
      max_temp[6] = max(max_temp[6], temp_read); \
      min_temp[6] = min(min_temp[6], temp_read)
  #else
    #define SET_ADMUX_ADCSRA(pin) ADMUX = _BV(REFS0) | (pin & 0x07); SBI(ADCSRA, ADSC)
    #ifdef MUX5
      #define START_ADC(pin) if (pin > 7) ADCSRB = _BV(MUX5); else ADCSRB = 0; SET_ADMUX_ADCSRA(pin)
    #else
      #define START_ADC(pin) ADCSRB = 0; SET_ADMUX_ADCSRA(pin)
    #endif
  #endif

  // Prepare or measure a sensor, each one every 14th frame
  switch (temp_state) {
    case PrepareTemp_0:
      #if HAS(TEMP_0)
        #ifdef __SAM3X8E__
          // nothing todo for Due
        #else
          START_ADC(TEMP_0_PIN);
        #endif
      #endif
      lcd_buttons_update();
      temp_state = MeasureTemp_0;
      break;
    case MeasureTemp_0:
      #if HAS(TEMP_0)
        #ifdef __SAM3X8E__
          SET_RAW_TEMP_VALUE(0);
        #else
          raw_temp_value[0] += ADC;
        #endif
      #endif
      temp_state = PrepareTemp_BED;
      break;

    case PrepareTemp_BED:
      #if HAS(TEMP_BED)
        #ifdef __SAM3X8E__
          // nothing todo for Due
        #else
          START_ADC(TEMP_BED_PIN);
        #endif
      #endif
      lcd_buttons_update();
      temp_state = MeasureTemp_BED;
      break;
    case MeasureTemp_BED:
      #if HAS(TEMP_BED)
        #ifdef __SAM3X8E__
          SET_RAW_TEMP_BED_VALUE();
        #else
          raw_temp_bed_value += ADC;
        #endif
      #endif
      temp_state = PrepareTemp_1;
      break;

    case PrepareTemp_1:
      #if HAS(TEMP_1)
        #ifdef __SAM3X8E__
          // nothing todo for Due
        #else
          START_ADC(TEMP_1_PIN);
        #endif
      #endif
      lcd_buttons_update();
      temp_state = MeasureTemp_1;
      break;
    case MeasureTemp_1:
      #if HAS(TEMP_1)
        #ifdef __SAM3X8E__
          SET_RAW_TEMP_VALUE(1);
        #else
          raw_temp_value[1] += ADC;
        #endif
      #endif
      temp_state = PrepareTemp_2;
      break;

    case PrepareTemp_2:
      #if HAS(TEMP_2)
        #ifdef __SAM3X8E__
          // nothing todo for Due
        #else
          START_ADC(TEMP_2_PIN);
        #endif
      #endif
      lcd_buttons_update();
      temp_state = MeasureTemp_2;
      break;
    case MeasureTemp_2:
      #if HAS(TEMP_2)
        #ifdef __SAM3X8E__
          SET_RAW_TEMP_VALUE(2);
        #else
          raw_temp_value[2] += ADC;
        #endif
      #endif
      temp_state = PrepareTemp_3;
      break;

    case PrepareTemp_3:
      #if HAS(TEMP_3)
        #ifdef __SAM3X8E__
          // nothing todo for Due
        #else
          START_ADC(TEMP_3_PIN);
        #endif
      #endif
      lcd_buttons_update();
      temp_state = MeasureTemp_3;
      break;
    case MeasureTemp_3:
      #if HAS(TEMP_3)
        #ifdef __SAM3X8E__
          SET_RAW_TEMP_VALUE(3);
        #else
          raw_temp_value[3] += ADC;
        #endif
      #endif
      temp_state = PrepareTemp_CHAMBER;
      break;

    case PrepareTemp_CHAMBER:
      #if HAS(TEMP_CHAMBER)
        #ifdef __SAM3X8E__
          // nothing todo for Due
        #else
          START_ADC(TEMP_CHAMBER_PIN);
        #endif
      #endif
      lcd_buttons_update();
      temp_state = MeasureTemp_CHAMBER;
      break;
    case MeasureTemp_CHAMBER:
      #if HAS(TEMP_CHAMBER)
        #ifdef __SAM3X8E__
          SET_RAW_TEMP_CHAMBER_VALUE();
        #else
          raw_temp_chamber_value += ADC;
        #endif
      #endif
      temp_state = PrepareTemp_COOLER;
      break;

    case PrepareTemp_COOLER:
      #if HAS(TEMP_COOLER)
        #ifdef __SAM3X8E__
          // nothing todo for Due
        #else
          START_ADC(TEMP_COOLER_PIN);
        #endif
      #endif
      lcd_buttons_update();
      temp_state = MeasureTemp_COOLER;
      break;
    case MeasureTemp_COOLER:
      #if HAS(TEMP_COOLER)
        #ifdef __SAM3X8E__
          SET_RAW_TEMP_COOLER_VALUE();
        #else
          raw_temp_cooler_value += ADC;
        #endif
      #endif
      temp_state = Prepare_FILWIDTH;
      break;

    case Prepare_FILWIDTH:
      #if HAS(FILAMENT_SENSOR)
        #ifdef __SAM3X8E__
          // nothing todo for Due
        #else
          START_ADC(FILWIDTH_PIN);
        #endif
      #endif
      lcd_buttons_update();
      temp_state = Measure_FILWIDTH;
      break;
    case Measure_FILWIDTH:
      #if HAS(FILAMENT_SENSOR)
        // raw_filwidth_value += ADC;  //remove to use an IIR filter approach
        if (ADC > 102) { //check that ADC is reading a voltage > 0.5 volts, otherwise don't take in the data.
          raw_filwidth_value -= (raw_filwidth_value >> 7); //multiply raw_filwidth_value by 127/128
          raw_filwidth_value += ((unsigned long)ADC << 7); //add new ADC reading
        }
      #endif
      temp_state = Prepare_POWCONSUMPTION;
      break;

    case Prepare_POWCONSUMPTION:
      #if HAS(POWER_CONSUMPTION_SENSOR)
        #ifdef __SAM3X8E__
          // nothing todo for Due
        #else
          START_ADC(POWER_CONSUMPTION_PIN);
        #endif
      #endif
      lcd_buttons_update();
      temp_state = Measure_POWCONSUMPTION;
      break;
    case Measure_POWCONSUMPTION:
      #if HAS(POWER_CONSUMPTION_SENSOR)
        #ifdef __SAM3X8E__
          raw_powconsumption_value = analogRead(POWER_CONSUMPTION_PIN);
        #else
          raw_powconsumption_value += ADC;
        #endif
      #endif
      temp_state = PrepareTemp_0;
      temp_count++;
      break;

    case StartupDelay:
      temp_state = PrepareTemp_0;
      break;

    // default:
    //  SERIAL_LM(ER, MSG_TEMP_READ_ERROR);
    //  break;
  } // switch(temp_state)

  #ifdef __SAM3X8E__
    if (temp_count >= OVERSAMPLENR + 2) { // 14 * 16 * 1/(16000000/64/256)  = 164ms.
  #else
    if (temp_count >= OVERSAMPLENR) { // 10 * 16 * 1/(16000000/64/256)  = 164ms.
  #endif

    // Update the raw values if they've been read. Else we could be updating them during reading.
    if (!temp_meas_ready) set_current_temp_raw();

    // Filament Sensor - can be read any time since IIR filtering is used
    #if HAS(FILAMENT_SENSOR)
      current_raw_filwidth = raw_filwidth_value >> 10;  // Divide to get to 0-16384 range since we used 1/128 IIR filter approach
    #endif

    temp_count = 0;
    for (int i = 0; i < 4; i++) raw_temp_value[i] = 0;
    raw_temp_bed_value = 0;
    raw_temp_chamber_value = 0;
    raw_temp_cooler_value = 0;

    #if HAS(POWER_CONSUMPTION_SENSOR)
      raw_powconsumption_value = 0;
    #endif

    #if HAS(TEMP_0) && DISABLED(HEATER_0_USES_MAX6675)
      #if HEATER_0_RAW_LO_TEMP > HEATER_0_RAW_HI_TEMP
        #define GE0 <=
      #else
        #define GE0 >=
      #endif
      if (current_temperature_raw[0] GE0 maxttemp_raw[0]) max_temp_error(0);
      if (minttemp_raw[0] GE0 current_temperature_raw[0]) min_temp_error(0);
    #endif

    #if HAS(TEMP_1) && HOTENDS > 1
      #if HEATER_1_RAW_LO_TEMP > HEATER_1_RAW_HI_TEMP
        #define GE1 <=
      #else
        #define GE1 >=
      #endif
      if (current_temperature_raw[1] GE1 maxttemp_raw[1]) max_temp_error(1);
      if (minttemp_raw[1] GE1 current_temperature_raw[1]) min_temp_error(1);
    #endif // TEMP_SENSOR_1

    #if HAS(TEMP_2) && HOTENDS > 2
      #if HEATER_2_RAW_LO_TEMP > HEATER_2_RAW_HI_TEMP
        #define GE2 <=
      #else
        #define GE2 >=
      #endif
      if (current_temperature_raw[2] GE2 maxttemp_raw[2]) max_temp_error(2);
      if (minttemp_raw[2] GE2 current_temperature_raw[2]) min_temp_error(2);
    #endif // TEMP_SENSOR_2

    #if HAS(TEMP_3) && HOTENDS > 3
      #if HEATER_3_RAW_LO_TEMP > HEATER_3_RAW_HI_TEMP
        #define GE3 <=
      #else
        #define GE3 >=
      #endif
      if (current_temperature_raw[3] GE3 maxttemp_raw[3]) max_temp_error(3);
      if (minttemp_raw[3] GE3 current_temperature_raw[3]) min_temp_error(3);
    #endif // TEMP_SENSOR_3

    #if HAS(TEMP_BED)
      #if HEATER_BED_RAW_LO_TEMP > HEATER_BED_RAW_HI_TEMP
        #define GEBED <=
      #else
        #define GEBED >=
      #endif
      if (current_temperature_bed_raw GEBED bed_maxttemp_raw) //_temp_error(-1, MSG_T_MAXTEMP, PSTR(MSG_ERR_MAXTEMP_BED));
      if (bed_minttemp_raw GEBED current_temperature_bed_raw && target_temperature_bed > 0.0f) _temp_error(-1, MSG_T_MINTEMP, PSTR(MSG_ERR_MINTEMP_BED));
    #endif

    #if HAS(TEMP_CHAMBER)
      #if HEATER_CHAMBER_RAW_LO_TEMP > HEATER_CHAMBER_RAW_HI_TEMP
        #define GECHAMBER <=
      #else
        #define GECHAMBER >=
      #endif
      if (current_temperature_chamber_raw GECHAMBER chamber_maxttemp_raw) _temp_error(-2, MSG_T_MAXTEMP, PSTR(MSG_ERR_MAXTEMP_CHAMBER));
      if (chamber_minttemp_raw GECHAMBER current_temperature_chamber_raw && target_temperature_chamber > 0.0f) _temp_error(-2, MSG_T_MINTEMP, PSTR(MSG_ERR_MINTEMP_CHAMBER));
    #endif

    #if HAS(TEMP_COOLER)
      #if COOLER_RAW_LO_TEMP > COOLER_RAW_HI_TEMP
        #define GECOOLER <=
      #else
        #define GECOOLER >=
      #endif
      if (current_temperature_cooler_raw GECOOLER cooler_maxttemp_raw) _temp_error(-3, MSG_T_MAXTEMP, PSTR(MSG_ERR_MAXTEMP_COOLER));
      if (cooler_minttemp_raw GECOOLER current_temperature_cooler_raw && target_temperature_cooler > 0.0f) _temp_error(-3, MSG_T_MINTEMP, PSTR(MSG_ERR_MINTEMP_COOLER));
    #endif

  } // temp_count >= OVERSAMPLENR

  #if ENABLED(BABYSTEPPING)
    for (uint8_t axis = X_AXIS; axis <= Z_AXIS; axis++) {
      int curTodo = babystepsTodo[axis]; //get rid of volatile for performance

      if (curTodo > 0) {
        stepper.babystep(axis,/*fwd*/true);
        babystepsTodo[axis]--; //fewer to do next time
      }
      else if (curTodo < 0) {
        stepper.babystep(axis,/*fwd*/false);
        babystepsTodo[axis]++; //fewer to do next time
      }
    }
  #endif //BABYSTEPPING
}

#if ENABLED(PIDTEMP) || ENABLED(PIDTEMPBED) || ENABLED(PIDTEMPCHAMBER) || ENABLED(PIDTEMPCOOLER)
  // Apply the scale factors to the PID values
  float scalePID_i(float i)   { return i * PID_dT; }
  float unscalePID_i(float i) { return i / PID_dT; }
  float scalePID_d(float d)   { return d / PID_dT; }
  float unscalePID_d(float d) { return d * PID_dT; }
#endif // ENABLED(PIDTEMP) || ENABLED(PIDTEMPBED) || ENABLED(PIDTEMPCHAMBER) || ENABLED(PIDTEMPCOOLER)
