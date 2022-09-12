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
 * endstops.cpp - A singleton object to manage endstops
 */

#include "../../base.h"

// TEST_ENDSTOP: test the old and the current status of an endstop
#define TEST_ENDSTOP(ENDSTOP) (TEST(current_endstop_bits & old_endstop_bits, ENDSTOP))

Endstops endstops;

// public:

bool  Endstops::enabled = true,
      Endstops::enabled_globally =
        #if ENABLED(ENDSTOPS_ONLY_FOR_HOMING)
          (false)
        #else
          (true)
        #endif
      ;
volatile char Endstops::endstop_hit_bits; // use X_MIN, Y_MIN, Z_MIN and Z_MIN_PROBE as BIT value

#if ENABLED(Z_DUAL_ENDSTOPS)
  uint16_t
#else
  byte
#endif
    Endstops::current_endstop_bits = 0,
    Endstops::old_endstop_bits = 0;

#if HAS(BED_PROBE)
  volatile bool Endstops::z_probe_enabled = false;
#endif

/**
 * Class and Instance Methods
 */

void Endstops::init() {

  #if HAS(X_MIN)
    SET_INPUT(X_MIN_PIN);
    #if ENABLED(ENDSTOPPULLUP_XMIN)
      PULLUP(X_MIN_PIN, HIGH);
    #endif
  #endif

  #if HAS(Y_MIN)
    SET_INPUT(Y_MIN_PIN);
    #if ENABLED(ENDSTOPPULLUP_YMIN)
      PULLUP(Y_MIN_PIN, HIGH);
    #endif
  #endif

  #if HAS(Z_MIN)
    SET_INPUT(Z_MIN_PIN);
    #if ENABLED(ENDSTOPPULLUP_ZMIN)
      PULLUP(Z_MIN_PIN, HIGH);
    #endif
  #endif

  #if HAS(Z2_MIN)
    SET_INPUT(Z2_MIN_PIN);
    #if ENABLED(ENDSTOPPULLUP_Z2MIN)
      PULLUP(Z2_MIN_PIN, HIGH);
    #endif
  #endif

  #if HAS(E_MIN)
    SET_INPUT(E_MIN_PIN);
    #if ENABLED(ENDSTOPPULLUP_EMIN)
      PULLUP(E_MIN_PIN, HIGH);
    #endif
  #endif

  #if HAS(X_MAX)
    SET_INPUT(X_MAX_PIN);
    #if ENABLED(ENDSTOPPULLUP_XMAX)
      PULLUP(X_MAX_PIN, HIGH);
    #endif
  #endif

  #if HAS(Y_MAX)
    SET_INPUT(Y_MAX_PIN);
    #if ENABLED(ENDSTOPPULLUP_YMAX)
      PULLUP(Y_MAX_PIN, HIGH);
    #endif
  #endif

  #if HAS(Z_MAX)
    SET_INPUT(Z_MAX_PIN);
    #if ENABLED(ENDSTOPPULLUP_ZMAX)
      PULLUP(Z_MAX_PIN, HIGH);
    #endif
  #endif

  #if HAS(Z2_MAX)
    SET_INPUT(Z2_MAX_PIN);
    #if ENABLED(ENDSTOPPULLUP_Z2MAX)
      PULLUP(Z2_MAX_PIN, HIGH);
    #endif
  #endif

  #if HAS(Z_PROBE_PIN) // Check for Z_PROBE_ENDSTOP so we don't pull a pin high unless it's to be used.
    SET_INPUT(Z_PROBE_PIN);
    #if ENABLED(ENDSTOPPULLUP_ZPROBE)
      PULLUP(Z_PROBE_PIN, HIGH);
    #endif
  #endif

} // Endstops::init

void Endstops::report_state() {
  if (endstop_hit_bits) {
    #if ENABLED(ULTRA_LCD)
      char chrX = ' ', chrY = ' ', chrZ = ' ', chrP = ' ';
      #define _SET_STOP_CHAR(A,C) (chr## A = C)
    #else
      #define _SET_STOP_CHAR(A,C) ;
    #endif

    #define _ENDSTOP_HIT_ECHO(A,C) do{ \
      SERIAL_MV(" " STRINGIFY(A) ":", stepper.triggered_position_mm(A ##_AXIS)); \
      _SET_STOP_CHAR(A,C); }while(0)

    #define _ENDSTOP_HIT_TEST(A,C) \
      if (TEST(endstop_hit_bits, A ##_MIN) || TEST(endstop_hit_bits, A ##_MAX)) \
        _ENDSTOP_HIT_ECHO(A,C)

    SERIAL_SM(ER, MSG_ENDSTOPS_HIT);
    _ENDSTOP_HIT_TEST(X, 'X');
    _ENDSTOP_HIT_TEST(Y, 'Y');
    _ENDSTOP_HIT_TEST(Z, 'Z');

    #if ENABLED(Z_PROBE_ENDSTOP)
      #define P_AXIS Z_AXIS
      if (TEST(endstop_hit_bits, Z_PROBE)) _ENDSTOP_HIT_ECHO(P, 'P');
    #endif
    SERIAL_E;

    #if ENABLED(ULTRA_LCD)
      char msg[3 * strlen(MSG_ENDSTOPS_HIT) + 8 + 1]; // Room for a UTF 8 string
      sprintf_P(msg, PSTR(MSG_ENDSTOPS_HIT " %c %c %c %c"), chrX, chrY, chrZ, chrP);
      lcd_setstatus(msg);
    #endif

    hit_on_purpose();

    #if ENABLED(ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED) && ENABLED(SDSUPPORT)
      if (abort_on_endstop_hit) {
        card.sdprinting = false;
        card.closeFile();
        quickstop_stepper();
        disable_all_heaters(); // switch off all heaters.
        disable_all_coolers();
      }
    #endif
  }
} // Endstops::report_state
//#if ENABLED(SENSOR_FILAMENT)
#if ENABLED(SENSOR_DE_FILAMENTO)
/*
bool Endstops::se_pulso_el_sensor_de_filamento =  READ(2);
bool recuendo_de_pulso_filamento =  READ(2);

  bool Endstops::sensor_de_filamento(){

    recuendo_de_pulso_filamento = READ(2);
    //EJEMPLO DE EJECUSIONs
    //SERIAL_MV("Recuerdo:", recuendo_de_pulso_filamento);
    //SERIAL_EM("");
    if(recuendo_de_pulso_filamento != se_pulso_el_sensor_de_filamento && se_pulso_el_sensor_de_filamento == false){
      //mensaje de verificasion
      SERIAL_MV("Alguien pulso el sensor de filamento: ", se_pulso_el_sensor_de_filamento);
      SERIAL_EM("");
      se_pulso_el_sensor_de_filamento = recuendo_de_pulso_filamento;
      return  true;
    }
    se_pulso_el_sensor_de_filamento = recuendo_de_pulso_filamento;
    return  false;
  }
*/
#endif

//intento de un sensor de filamento que pueda detectar que se trabo el filamento
#if ENABLED(SENSOR_FILAMENT)
  int aux_x_pun=0;

  int maximo=0;
  int minimo=6000;
  int promedio=0;
  //int valor=0;
  unsigned long tiempo_vista=0;

  bool x_antes =false;
  bool x_ahora =false;

  void Endstops::sensor_de_filamento(){
  }
#endif
void Endstops::M119() {
  SERIAL_EM(MSG_M119_REPORT);
  #if HAS(X_MIN)
    SERIAL_EMT(MSG_X_MIN, ((READ(X_MIN_PIN)^X_MIN_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
  #if HAS(X_MAX)
    SERIAL_EMT(MSG_X_MAX, ((READ(X_MAX_PIN)^X_MAX_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
  #if HAS(Y_MIN)
    SERIAL_EMT(MSG_Y_MIN, ((READ(Y_MIN_PIN)^Y_MIN_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
  #if HAS(Y_MAX)
    SERIAL_EMT(MSG_Y_MAX, ((READ(Y_MAX_PIN)^Y_MAX_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
  #if HAS(Z_MIN)
    SERIAL_EMT(MSG_Z_MIN, ((READ(Z_MIN_PIN)^Z_MIN_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
  #if HAS(Z_MAX)
    SERIAL_EMT(MSG_Z_MAX, ((READ(Z_MAX_PIN)^Z_MAX_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
  #if HAS(Z2_MAX)
    SERIAL_EMT(MSG_Z2_MAX, ((READ(Z2_MAX_PIN)^Z2_MAX_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
  #if HAS(Z_PROBE_PIN)
    SERIAL_EMT(MSG_Z_PROBE, ((READ(Z_PROBE_PIN)^Z_PROBE_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
  #if HAS(E_MIN)
    SERIAL_EMT(MSG_E_MIN, ((READ(E_MIN_PIN)^E_MIN_ENDSTOP_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
  #if HAS(FILRUNOUT)
    SERIAL_EMT(MSG_FILRUNOUT_PIN, ((READ(FILRUNOUT_PIN)^FILRUNOUT_PIN_INVERTING)?MSG_ENDSTOP_HIT:MSG_ENDSTOP_OPEN));
  #endif
} // Endstops::M119

#if ENABLED(Z_DUAL_ENDSTOPS)

  // Pass the result of the endstop test
  void Endstops::test_dual_z_endstops(EndstopEnum es1, EndstopEnum es2) {
    byte z_test = TEST_ENDSTOP(es1) | (TEST_ENDSTOP(es2) << 1); // bit 0 for Z, bit 1 for Z2
    if (stepper.current_block->steps[Z_AXIS] > 0) {
      stepper.endstop_triggered(Z_AXIS);
      SBI(endstop_hit_bits, Z_MIN);
      if (!performing_homing || (z_test == 0x3))  //if not performing home or if both endstops were trigged during homing...
        stepper.kill_current_block();
    }
  }

#endif

// Check endstops - Called from ISR!

void Endstops::update() {

  #define _ENDSTOP_PIN(AXIS, MINMAX) AXIS ##_## MINMAX ##_PIN
  #define _ENDSTOP_INVERTING(AXIS, MINMAX) AXIS ##_## MINMAX ##_ENDSTOP_INVERTING
  #define _ENDSTOP_HIT(AXIS) SBI(endstop_hit_bits, _ENDSTOP(AXIS, MIN))
  #define _ENDSTOP(AXIS, MINMAX) AXIS ##_## MINMAX
  #if (KUTTERCRAFT_MULTIFILAMENT)
    final_de_carrera_e = true;
  #endif
  // UPDATE_ENDSTOP_BIT: set the current endstop bits for an endstop to its status
  #define UPDATE_ENDSTOP_BIT(AXIS, MINMAX) SET_BIT(current_endstop_bits, _ENDSTOP(AXIS, MINMAX), (READ(_ENDSTOP_PIN(AXIS, MINMAX)) != _ENDSTOP_INVERTING(AXIS, MINMAX)))
  // COPY_BIT: copy the value of COPY_BIT to BIT in bits
  #define COPY_BIT(bits, COPY_BIT, BIT) SET_BIT(bits, BIT, TEST(bits, COPY_BIT))

  #define UPDATE_ENDSTOP(AXIS,MINMAX) do { \
      UPDATE_ENDSTOP_BIT(AXIS, MINMAX); \
      if (TEST_ENDSTOP(_ENDSTOP(AXIS, MINMAX)) && stepper.current_block->steps[_AXIS(AXIS)] > 0) { \
        _ENDSTOP_HIT(AXIS); \
        stepper.endstop_triggered(_AXIS(AXIS)); \
      } \
    } while(0)

  #if MECH(COREXY) || MECH(COREYX)|| MECH(COREXZ) || MECH(COREZX)
    // Head direction in -X axis for CoreXY and CoreXZ bots.
    // If DeltaA == -DeltaB, the movement is only in Y or Z axis
    if ((stepper.current_block->steps[CORE_AXIS_1] != stepper.current_block->steps[CORE_AXIS_2]) || (stepper.motor_direction(CORE_AXIS_1) == stepper.motor_direction(CORE_AXIS_2))) {
      if (stepper.motor_direction(X_HEAD))
  #else
    if (stepper.motor_direction(X_AXIS))   // stepping along -X axis (regular Cartesian bot)
  #endif
      { // -direction
        #if ENABLED(DUAL_X_CARRIAGE)
          // with 2 x-carriages, endstops are only checked in the homing direction for the active extruder
          if ((stepper.current_block->active_driver == 0 && X_HOME_DIR == -1) || (stepper.current_block->active_driver != 0 && X2_HOME_DIR == -1))
        #endif
          {
            #if HAS(X_MIN)
              UPDATE_ENDSTOP(X, MIN);


            #endif
          }
      }
      else { // +direction
        #if ENABLED(DUAL_X_CARRIAGE)
          // with 2 x-carriages, endstops are only checked in the homing direction for the active extruder
          if ((stepper.current_block->active_driver == 0 && X_HOME_DIR == 1) || (stepper.current_block->active_driver != 0 && X2_HOME_DIR == 1))
        #endif
          {
            #if HAS(X_MAX)
              UPDATE_ENDSTOP(X, MAX);
            #endif
          }
      }
  #if MECH(COREXY) || MECH(COREYX) || MECH(COREXZ) || MECH(COREZX)
    }
  #endif

  #if MECH(COREXY) || MECH(COREYX)
    // Head direction in -Y axis for CoreXY bots.
    // If DeltaA == DeltaB, the movement is only in X axis
    if ((stepper.current_block->steps[CORE_AXIS_1] != stepper.current_block->steps[CORE_AXIS_2]) || (stepper.motor_direction(CORE_AXIS_1) != stepper.motor_direction(CORE_AXIS_2))) {
      if (stepper.motor_direction(Y_HEAD))
  #else
      if (stepper.motor_direction(Y_AXIS))   // -direction
  #endif
      { // -direction
        #if HAS(Y_MIN)
          UPDATE_ENDSTOP(Y, MIN);
        #endif
      }
      else { // +direction
        #if HAS(Y_MAX)
          UPDATE_ENDSTOP(Y, MAX);
        #endif
      }
  #if MECH(COREXY) || MECH(COREYX)
    }
  #endif

  #if MECH(COREXZ) || MECH(COREZX)
    // Head direction in -Z axis for CoreXZ bots.
    // If DeltaA == DeltaB, the movement is only in X axis
    if ((stepper.current_block->steps[CORE_AXIS_1] != stepper.current_block->steps[CORE_AXIS_2]) || (stepper.motor_direction(CORE_AXIS_1) !) != stepper.motor_direction(CORE_AXIS_2))) {
      if (stepper.motor_direction(Z_HEAD))
  #else
      if (stepper.motor_direction(Z_AXIS))
  #endif
      { // z -direction
        #if HAS(Z_MIN)

          #if ENABLED(Z_DUAL_ENDSTOPS)
            UPDATE_ENDSTOP_BIT(Z, MIN);
            #if HAS_Z2_MIN
              UPDATE_ENDSTOP_BIT(Z2, MIN);
            #else
              COPY_BIT(current_endstop_bits, Z_MIN, Z2_MIN);
            #endif

            test_dual_z_endstops(Z_MIN, Z2_MIN);

          #else // !Z_DUAL_ENDSTOPS

          #if HAS(BED_PROBE) && HASNT(Z_PROBE_PIN)
            if (z_probe_enabled) UPDATE_ENDSTOP(Z, MIN);
          #else
            #if(!EXPERIMENTAL)
              UPDATE_ENDSTOP(Z, MIN);
            #endif
          #endif

          #endif // !Z_DUAL_ENDSTOPS
        #endif // HAS_Z_MIN

        #if HAS(BED_PROBE) && HAS(Z_PROBE_PIN)
          if(autolevel_on_off){
              UPDATE_ENDSTOP(Z, MIN);
            //if (true) {
              UPDATE_ENDSTOP(Z, PROBE);
              //if (TEST_ENDSTOP(Z_PROBE)) SBI(endstop_hit_bits, Z_PROBE);
            //}

          }else{
            UPDATE_ENDSTOP(Z, MIN);
            //UPDATE_ENDSTOP(Z, PROBE);
            /*
            if (z_probe_enabled) {
              UPDATE_ENDSTOP(Z, PROBE);
              if (TEST_ENDSTOP(Z_PROBE)) SBI(endstop_hit_bits, Z_PROBE);
            }
            */
          }

        #endif

      }
      else { // z +direction
        #if HAS(Z_MAX)

          #if ENABLED(Z_DUAL_ENDSTOPS)

            UPDATE_ENDSTOP_BIT(Z, MAX);
            #if HAS_Z2_MAX
              UPDATE_ENDSTOP_BIT(Z2, MAX);
            #else
              COPY_BIT(current_endstop_bits, Z_MAX, Z2_MAX);
            #endif

            test_dual_z_endstops(Z_MAX, Z2_MAX);

          #else // !Z_DUAL_ENDSTOPS

            UPDATE_ENDSTOP(Z, MAX);

          #endif // !Z_DUAL_ENDSTOPS
        #endif // Z_MAX_PIN
      }
  #if MECH(COREXZ) || MECH(COREZX)
    }
  #endif

  #define E_MIN_PIN 2
  #if (KUTTERCRAFT_MULTIFILAMENT)
    UPDATE_ENDSTOP(E, MIN);

    if(!READ(2) && m750){
      //UPDATE_ENDSTOP(E, MIN);
      //_ENDSTOP_HIT(E_AXIS)
      //stepper.endstop_triggered(E_AXIS);
      //stepper.kill_current_block();
      //stepper.synchronize();
      final_de_carrera_e = false;
    }
  #endif
  #if ENABLED(NPR2)
    UPDATE_ENDSTOP(E, MIN);
  #endif

  old_endstop_bits = current_endstop_bits;

} // Endstops::update()
