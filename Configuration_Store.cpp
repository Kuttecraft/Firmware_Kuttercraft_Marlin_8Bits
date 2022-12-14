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
 * configuration_store.cpp
 *
 * Configuration and EEPROM storage
 *
 * IMPORTANT:  Whenever there are changes made to the variables stored in EEPROM
 * in the functions below, also increment the version number. This makes sure that
 * the default values are used whenever there is a change to the data, to prevent
 * wrong data being written to the variables.
 *
 * ALSO: Variables in the Store and Retrieve sections must be in the same order.
 *       If a feature is disabled, some data must still be written that, when read,
 *       either sets a Sane Default, or results in No Change to the existing value.
 *
 */

#include "base.h"

#define EEPROM_VERSION "MKV28"
#define EEPROM_OFFSET 100


uint16_t eeprom_checksum;
const char version[6] = EEPROM_VERSION;

#if (TACTIL)
  bool autolevel_on_off = true;
#endif

void _EEPROM_writeData(int& pos, uint8_t* value, uint8_t size) {
  uint8_t c;
  while(size--) {
    eeprom_write_byte((unsigned char*)pos, *value);
    c = eeprom_read_byte((unsigned char*)pos);
    if (c != *value) {
      SERIAL_LM(ER, "Error writing to EEPROM!");
    }
    eeprom_checksum += c;
    pos++;
    value++;
  };
}

void _EEPROM_readData(int& pos, uint8_t* value, uint8_t size) {
  do {
    byte c = eeprom_read_byte((unsigned char*)pos);
    *value = c;
    eeprom_checksum += c;
    pos++;
    value++;
  } while (--size);
}

/**
 * Post-process after Retrieve or Reset
 */
void Config_Postprocess() {
  // steps per s2 needs to be updated to agree with units per s2
  planner.reset_acceleration_rates();

  // Make sure delta kinematics are updated before refreshing the
  // planner position so the stepper counts will be set correctly.
  #if MECH(DELTA)
    set_delta_constants();
  #endif

  // Refresh steps_to_mm with the reciprocal of axis_steps_per_mm
  // and init stepper.count[], planner.position[] with current_position
  planner.refresh_positioning();

  #if ENABLED(PIDTEMP)
    updatePID();
  #endif

  calculate_volumetric_multipliers();

  // Software endstops depend on home_offset
  LOOP_XYZ(i) update_software_endstops((AxisEnum)i);
}

#if ENABLED(EEPROM_SETTINGS)

#define EEPROM_START() int eeprom_index = EEPROM_OFFSET
#define EEPROM_SKIP(VAR) eeprom_index += sizeof(VAR)
#define EEPROM_WRITE(VAR) _EEPROM_writeData(eeprom_index, (uint8_t*)&VAR, sizeof(VAR))
#define EEPROM_READ(VAR) _EEPROM_readData(eeprom_index, (uint8_t*)&VAR, sizeof(VAR))

/**
 * M500 - Store Configuration
 */
void Config_StoreSettings() {
  float dummy = 0.0f;
  char ver[6] = "00000";

  EEPROM_START();

  EEPROM_WRITE(ver);     // invalidate data first
  EEPROM_SKIP(eeprom_checksum); // Skip the checksum slot

  eeprom_checksum = 0; // clear before first "real data"

  EEPROM_WRITE(planner.axis_steps_per_mm);
  EEPROM_WRITE(planner.max_feedrate_mm_s);
  EEPROM_WRITE(planner.max_acceleration_mm_per_s2);
  EEPROM_WRITE(planner.acceleration);
  EEPROM_WRITE(planner.retract_acceleration);
  EEPROM_WRITE(planner.travel_acceleration);
  EEPROM_WRITE(planner.min_feedrate_mm_s);
  EEPROM_WRITE(planner.min_travel_feedrate_mm_s);
  EEPROM_WRITE(planner.min_segment_time);
  EEPROM_WRITE(planner.max_xy_jerk);
  EEPROM_WRITE(planner.max_z_jerk);
  EEPROM_WRITE(planner.max_e_jerk);
  EEPROM_WRITE(home_offset);
  EEPROM_WRITE(hotend_offset);
  EEPROM_WRITE(save_on_off);
  EEPROM_WRITE(on_off_sensor_de_filamento);
  EEPROM_WRITE(card.estabaImprimiendo);
  #if HAS(BED_PROBE) && NOMECH(DELTA)
    EEPROM_WRITE(autolevel_on_off);
    EEPROM_WRITE(g77_offset);
  #endif
  EEPROM_WRITE(se_estaba_imprimiendo);
  //EEPROM_WRITE(dir_encoder);

  #if(LCD || OLED || TACTIL)
    //EEPROM_WRITE(ver_cordenadas);
  #endif

  #if ENABLED(MESH_BED_LEVELING)
    // Compile time test that sizeof(mbl.z_values) is as expected
    typedef char c_assert[(sizeof(mbl.z_values) == (MESH_NUM_X_POINTS) * (MESH_NUM_Y_POINTS) * sizeof(dummy)) ? 1 : -1];
    uint8_t mesh_num_x  = MESH_NUM_X_POINTS,
            mesh_num_y  = MESH_NUM_Y_POINTS,
            dummy_uint8 = mbl.status & _BV(MBL_STATUS_HAS_MESH_BIT);
    EEPROM_WRITE(dummy_uint8);
    EEPROM_WRITE(mbl.z_offset);
    EEPROM_WRITE(mesh_num_x);
    EEPROM_WRITE(mesh_num_y);
    EEPROM_WRITE(mbl.z_values);
    //EEPROM_WRITE(offset_mesh_valor);
  #endif

  #if HEATER_USES_AD595
    EEPROM_WRITE(ad595_offset);
    EEPROM_WRITE(ad595_gain);
  #endif

  #if MECH(DELTA)
    EEPROM_WRITE(endstop_adj);
    EEPROM_WRITE(delta_radius);
    EEPROM_WRITE(delta_diagonal_rod);
    EEPROM_WRITE(delta_segments_per_second);
    EEPROM_WRITE(soft_endstop_max);
    EEPROM_WRITE(tower_adj);
    EEPROM_WRITE(diagrod_adj);
  #elif ENABLED(Z_DUAL_ENDSTOPS)
    EEPROM_WRITE(z_endstop_adj);
  #endif

  #if HASNT(BED_PROBE)
    float zprobe_zoffset = 0;
  #endif
  EEPROM_WRITE(zprobe_zoffset);

  #if DISABLED(ULTIPANEL)
    int plaPreheatHotendTemp = PLA_PREHEAT_HOTEND_TEMP, plaPreheatHPBTemp = PLA_PREHEAT_HPB_TEMP, plaPreheatFanSpeed = PLA_PREHEAT_FAN_SPEED,
        absPreheatHotendTemp = ABS_PREHEAT_HOTEND_TEMP, absPreheatHPBTemp = ABS_PREHEAT_HPB_TEMP, absPreheatFanSpeed = ABS_PREHEAT_FAN_SPEED,
        gumPreheatHotendTemp = GUM_PREHEAT_HOTEND_TEMP, gumPreheatHPBTemp = GUM_PREHEAT_HPB_TEMP, gumPreheatFanSpeed = GUM_PREHEAT_FAN_SPEED;
  #endif

  EEPROM_WRITE(plaPreheatHotendTemp);
  EEPROM_WRITE(plaPreheatHPBTemp);
  EEPROM_WRITE(plaPreheatFanSpeed);
  EEPROM_WRITE(absPreheatHotendTemp);
  EEPROM_WRITE(absPreheatHPBTemp);
  EEPROM_WRITE(absPreheatFanSpeed);
  EEPROM_WRITE(gumPreheatHotendTemp);
  EEPROM_WRITE(gumPreheatHPBTemp);
  EEPROM_WRITE(gumPreheatFanSpeed);
  #if(LCD || OLED || TACTIL)
    EEPROM_WRITE(ver_cordenadas);
  #endif
  #if (LCD) || (OLED)
    EEPROM_WRITE(dir_encoder);
  #endif
  #if ENABLED(PIDTEMP)
    for (int h = 0; h < HOTENDS; h++) {
      EEPROM_WRITE(PID_PARAM(Kp, h));
      EEPROM_WRITE(PID_PARAM(Ki, h));
      EEPROM_WRITE(PID_PARAM(Kd, h));
      EEPROM_WRITE(PID_PARAM(Kc, h));
    }
  #endif

  #if DISABLED(PID_ADD_EXTRUSION_RATE)
    int lpq_len = 20;
  #endif
  EEPROM_WRITE(lpq_len);

  #if ENABLED(PIDTEMPBED)
    EEPROM_WRITE(bedKp);
    EEPROM_WRITE(bedKi);
    EEPROM_WRITE(bedKd);
  #endif

  #if ENABLED(PIDTEMPCHAMBER)
    EEPROM_WRITE(chamberKp);
    EEPROM_WRITE(chamberKi);
    EEPROM_WRITE(chamberKd);
  #endif

  #if ENABLED(PIDTEMPCOOLER)
    EEPROM_WRITE(coolerKp);
    EEPROM_WRITE(coolerKi);
    EEPROM_WRITE(coolerKd);
  #endif

  #if HASNT(LCD_CONTRAST)
    const int lcd_contrast = 32;
  #endif
  EEPROM_WRITE(lcd_contrast);

  #if MECH(SCARA)
    EEPROM_WRITE(axis_scaling); // 3 floats
  #endif

  #if ENABLED(FWRETRACT)
    EEPROM_WRITE(autoretract_enabled);
    EEPROM_WRITE(retract_length);
    #if EXTRUDERS > 1
      EEPROM_WRITE(retract_length_swap);
    #else
      dummy = 0.0f;
      EEPROM_WRITE(dummy);
    #endif
    EEPROM_WRITE(retract_feedrate);
    EEPROM_WRITE(retract_zlift);
    EEPROM_WRITE(retract_recover_length);
    #if EXTRUDERS > 1
      EEPROM_WRITE(retract_recover_length_swap);
    #else
      dummy = 0.0f;
      EEPROM_WRITE(dummy);
    #endif
    EEPROM_WRITE(retract_recover_feedrate);
  #endif // FWRETRACT

  EEPROM_WRITE(volumetric_enabled);

  // Save filament sizes
  for (int e = 0; e < EXTRUDERS; e++)
    EEPROM_WRITE(filament_size[e]);

  #if ENABLED(IDLE_OOZING_PREVENT)
    EEPROM_WRITE(IDLE_OOZING_enabled);
  #endif

  #if MB(ALLIGATOR)
    EEPROM_WRITE(motor_current);
  #endif
  EEPROM_WRITE(print_job_counter.data.numberPrints);
  EEPROM_WRITE(print_job_counter.data.completePrints);
  EEPROM_WRITE(print_job_counter.data.printTime);
  EEPROM_WRITE(print_job_counter.data.printer_usage_seconds);
  EEPROM_WRITE(print_job_counter.data.filamentUsed);
  EEPROM_WRITE(numero_de_serie);
  EEPROM_WRITE(numero_de_logro);
  EEPROM_WRITE(numero_de_logro_);
  EEPROM_WRITE(ultimo_numeros_de_kilometros);
  EEPROM_WRITE(numero_ventana_mantenimiento);
  EEPROM_WRITE(confi_actualizacion);//confi_actualizacion
  EEPROM_WRITE(on_off_sonido_final);
  EEPROM_WRITE(estatus_guardado);




  uint16_t  final_checksum = eeprom_checksum,
            eeprom_size = eeprom_index;

  eeprom_index = EEPROM_OFFSET;
  EEPROM_WRITE(version);
  EEPROM_WRITE(final_checksum);

  // uint16_t numberPrints;          // Number of prints
  // uint16_t completePrints;        // Number of complete prints
  // millis_t printTime;             // Accumulated printing time
  // millis_t printer_usage_seconds; // Longest successfull print job
  // double filamentUsed;            // Accumulated filament consumed in mm


  // Report storage size
  SERIAL_MV("Settings Stored (", eeprom_size);
  SERIAL_EM(" bytes)");
}
/**
 * M667 - Guarda el offset
 */
 /*
 void guardarOffset(){
   EEPROM_WRITE(zprobe_zoffset);
 }
 */
/**
 * M501 - Retrieve Configuration
 */
void Config_RetrieveSettings() {
  char stored_ver[6];
  uint16_t stored_checksum;

  EEPROM_START();
  EEPROM_READ(stored_ver);
  EEPROM_READ(stored_checksum);

  if (DEBUGGING(INFO)) {
    SERIAL_SMV(INFO, "Version: [", version);
    SERIAL_MV("] Stored version: [", stored_ver);
  }

  if (strncmp(version, stored_ver, 5) != 0) {
    Config_ResetDefault();
  }
  else {
    float dummy = 0;

    eeprom_checksum = 0; // clear before reading first "real data"

    // version number match
    EEPROM_READ(planner.axis_steps_per_mm);
    EEPROM_READ(planner.max_feedrate_mm_s);
    EEPROM_READ(planner.max_acceleration_mm_per_s2);

    EEPROM_READ(planner.acceleration);
    EEPROM_READ(planner.retract_acceleration);
    EEPROM_READ(planner.travel_acceleration);
    EEPROM_READ(planner.min_feedrate_mm_s);
    EEPROM_READ(planner.min_travel_feedrate_mm_s);
    EEPROM_READ(planner.min_segment_time);
    EEPROM_READ(planner.max_xy_jerk);
    EEPROM_READ(planner.max_z_jerk);
    EEPROM_READ(planner.max_e_jerk);
    EEPROM_READ(home_offset);
    EEPROM_READ(hotend_offset);
    EEPROM_READ(save_on_off);
    EEPROM_READ(on_off_sensor_de_filamento);
    EEPROM_READ(card.estabaImprimiendo);
    #if HAS(BED_PROBE) && NOMECH(DELTA)
      EEPROM_READ(autolevel_on_off);
      EEPROM_READ(g77_offset);
    #endif
    EEPROM_READ(se_estaba_imprimiendo);
    //EEPROM_READ(dir_encoder);

    #if ENABLED(MESH_BED_LEVELING)
      uint8_t mesh_num_x = 0, mesh_num_y = 0;
      EEPROM_READ(mbl.status);
      EEPROM_READ(mbl.z_offset);
      EEPROM_READ(mesh_num_x);
      EEPROM_READ(mesh_num_y);
      EEPROM_READ(mbl.z_values);
      //EEPROM_READ(offset_mesh_valor);
    #endif

    #if HEATER_USES_AD595
      EEPROM_READ(ad595_offset);
      EEPROM_READ(ad595_gain);
      for (int8_t h = 0; h < HOTENDS; h++)
        if (ad595_gain[h] == 0) ad595_gain[h] == TEMP_SENSOR_AD595_GAIN;
    #endif

    #if MECH(DELTA)
      EEPROM_READ(endstop_adj);
      EEPROM_READ(delta_radius);
      EEPROM_READ(delta_diagonal_rod);
      EEPROM_READ(delta_segments_per_second);
      EEPROM_READ(soft_endstop_max);
      EEPROM_READ(tower_adj);
      EEPROM_READ(diagrod_adj);
    #endif //DELTA

    #if HASNT(BED_PROBE)
      float zprobe_zoffset = 0;
    #endif
    EEPROM_READ(zprobe_zoffset);

    #if DISABLED(ULTIPANEL)
      int plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed,
          absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed,
          gumPreheatHotendTemp, gumPreheatHPBTemp, gumPreheatFanSpeed;
    #endif

    EEPROM_READ(plaPreheatHotendTemp);
    EEPROM_READ(plaPreheatHPBTemp);
    EEPROM_READ(plaPreheatFanSpeed);
    EEPROM_READ(absPreheatHotendTemp);
    EEPROM_READ(absPreheatHPBTemp);
    EEPROM_READ(absPreheatFanSpeed);
    EEPROM_READ(gumPreheatHotendTemp);
    EEPROM_READ(gumPreheatHPBTemp);
    EEPROM_READ(gumPreheatFanSpeed);
    #if(LCD || OLED || TACTIL)
      EEPROM_READ(ver_cordenadas);
    #endif
    #if (LCD) || (OLED)
      EEPROM_READ(dir_encoder);
    #endif
    /*
    #if(GUARDAR)
      EEPROM_READ(save_Temp);
      EEPROM_READ(save_CoordZ);
      EEPROM_READ(save_Linia);
    #endif
    */
    #if ENABLED(PIDTEMP)
      for (int8_t h = 0; h < HOTENDS; h++) {
        EEPROM_READ(PID_PARAM(Kp, h));
        EEPROM_READ(PID_PARAM(Ki, h));
        EEPROM_READ(PID_PARAM(Kd, h));
        EEPROM_READ(PID_PARAM(Kc, h));
      }
    #endif // PIDTEMP

    #if DISABLED(PID_ADD_EXTRUSION_RATE)
      int lpq_len;
    #endif
    EEPROM_READ(lpq_len);

    #if ENABLED(PIDTEMPBED)
      EEPROM_READ(bedKp);
      EEPROM_READ(bedKi);
      EEPROM_READ(bedKd);
    #endif

    #if ENABLED(PIDTEMPCHAMBER)
      EEPROM_READ(chamberKp);
      EEPROM_READ(chamberKi);
      EEPROM_READ(chamberKd);
    #endif

    #if ENABLED(PIDTEMPCOOLER)
      EEPROM_READ(coolerKp);
      EEPROM_READ(coolerKi);
      EEPROM_READ(coolerKd);
    #endif

    #if HASNT(LCD_CONTRAST)
      int lcd_contrast;
    #endif
    EEPROM_READ(lcd_contrast);

    #if MECH(SCARA)
      EEPROM_READ(axis_scaling);  // 3 floats
    #endif

    #if ENABLED(FWRETRACT)
      EEPROM_READ(autoretract_enabled);
      EEPROM_READ(retract_length);
      #if EXTRUDERS > 1
        EEPROM_READ(retract_length_swap);
      #else
        EEPROM_READ(dummy);
      #endif
      EEPROM_READ(retract_feedrate);
      EEPROM_READ(retract_zlift);
      EEPROM_READ(retract_recover_length);
      #if EXTRUDERS > 1
        EEPROM_READ(retract_recover_length_swap);
      #else
        EEPROM_READ(dummy);
      #endif
      EEPROM_READ(retract_recover_feedrate);
    #endif // FWRETRACT

    EEPROM_READ(volumetric_enabled);

    for (int8_t e = 0; e < EXTRUDERS; e++)
      EEPROM_READ(filament_size[e]);

    #if ENABLED(IDLE_OOZING_PREVENT)
      EEPROM_READ(IDLE_OOZING_enabled);
    #endif

    #if MB(ALLIGATOR)
      EEPROM_READ(motor_current);
    #endif

    EEPROM_READ(print_job_counter.data.numberPrints);
    EEPROM_READ(print_job_counter.data.completePrints);
    EEPROM_READ(print_job_counter.data.printTime);
    EEPROM_READ(print_job_counter.data.printer_usage_seconds);
    EEPROM_READ(print_job_counter.data.filamentUsed);
    EEPROM_READ(numero_de_serie);
    EEPROM_READ(numero_de_logro);
    EEPROM_READ(numero_de_logro_);
    EEPROM_READ(ultimo_numeros_de_kilometros);
    EEPROM_READ(numero_ventana_mantenimiento);
    EEPROM_READ(confi_actualizacion);//confi_actualizacion
    EEPROM_READ(on_off_sonido_final);
    EEPROM_READ(estatus_guardado);




    if (eeprom_checksum == stored_checksum) {
      //SERIAL_EM(ESPACIADO);
      Config_Postprocess();
      SERIAL_MV("                Stored settings retrieved (", eeprom_index);
      SERIAL_EM(" bytes)");
      SERIAL_EM(ESPACIADO);
    }
    else {
      SERIAL_LM(ER, "EEPROM checksum mismatch");
      Config_ResetDefault();
    }
  }

  #if ENABLED(EEPROM_CHITCHAT)
    Config_PrintSettings();
  #endif

}

#endif // EEPROM_SETTINGS

/**
 * M502 - Reset Configuration
 */
void Config_ResetDefault() {
  float tmp1[] = DEFAULT_AXIS_STEPS_PER_UNIT;
  float tmp2[] = DEFAULT_MAX_FEEDRATE;
  float tmp3[] = DEFAULT_MAX_ACCELERATION;
  float tmp4[] = DEFAULT_RETRACT_ACCELERATION;
  float tmp5[] = DEFAULT_EJERK;
  float tmp6[] = DEFAULT_Kp;
  float tmp7[] = DEFAULT_Ki;
  float tmp8[] = DEFAULT_Kd;
  float tmp9[] = DEFAULT_Kc;

  #if ENABLED(HOTEND_OFFSET_X) && ENABLED(HOTEND_OFFSET_Y) && ENABLED(HOTEND_OFFSET_Z)
    float tmp10[] = HOTEND_OFFSET_X;
    float tmp11[] = HOTEND_OFFSET_Y;
    float tmp12[] = HOTEND_OFFSET_Z;
  #endif

  #if MB(ALLIGATOR)
    float tmp13[] = MOTOR_CURRENT;
    for (int8_t i = 0; i < 3 + DRIVER_EXTRUDERS; i++)
      motor_current[i] = tmp13[i];
  #endif

  for (int8_t i = 0; i < 3 + EXTRUDERS; i++) {
    planner.axis_steps_per_mm[i] = tmp1[i];
    planner.max_feedrate_mm_s[i] = tmp2[i];
    planner.max_acceleration_mm_per_s2[i] = tmp3[i];
  }

  for (int8_t i = 0; i < EXTRUDERS; i++) {
    planner.retract_acceleration[i] = tmp4[i];
    planner.max_e_jerk[i] = tmp5[i];
  }

  for (int8_t i = 0; i < HOTENDS; i++) {
    #if ENABLED(HOTEND_OFFSET_X) && ENABLED(HOTEND_OFFSET_Y) && ENABLED(HOTEND_OFFSET_Z)
      hotend_offset[X_AXIS][i] = tmp10[i];
      hotend_offset[Y_AXIS][i] = tmp11[i];
      hotend_offset[Z_AXIS][i] = tmp12[i];
    #else
      hotend_offset[X_AXIS][i] = 0;
      hotend_offset[Y_AXIS][i] = 0;
      hotend_offset[Z_AXIS][i] = 0;
    #endif
  }

  #if MECH(SCARA)
    LOOP_XYZE(i) {
      if (i < COUNT(axis_scaling))
        axis_scaling[i] = 1;
    }
  #endif

  planner.acceleration = DEFAULT_ACCELERATION;
  planner.travel_acceleration = DEFAULT_TRAVEL_ACCELERATION;
  planner.min_feedrate_mm_s = DEFAULT_MINIMUMFEEDRATE;
  planner.min_segment_time = DEFAULT_MINSEGMENTTIME;
  planner.min_travel_feedrate_mm_s = DEFAULT_MINTRAVELFEEDRATE;
  planner.max_xy_jerk = DEFAULT_XYJERK;
  planner.max_z_jerk = DEFAULT_ZJERK;
  home_offset[X_AXIS] = home_offset[Y_AXIS] = home_offset[Z_AXIS] = 0;

  #if ENABLED(SENSOR_DE_FILAMENTO)
    on_off_sensor_de_filamento = false;
    save_on_off = true;
  #endif

  #if ENABLED(MESH_BED_LEVELING)
    mbl.reset();
  #endif

  #if HAS(BED_PROBE)
    zprobe_zoffset = Z_PROBE_OFFSET_FROM_NOZZLE;
  #endif

  #if MECH(DELTA)
    delta_radius = DEFAULT_DELTA_RADIUS;
    delta_diagonal_rod = DELTA_DIAGONAL_ROD;
    delta_segments_per_second =  DELTA_SEGMENTS_PER_SECOND;
    soft_endstop_max[0] = X_MAX_POS;
    soft_endstop_max[1] = Y_MAX_POS;
    soft_endstop_max[2] = Z_MAX_POS;
    endstop_adj[0] = TOWER_A_ENDSTOP_ADJ;
    endstop_adj[1] = TOWER_B_ENDSTOP_ADJ;
    endstop_adj[2] = TOWER_C_ENDSTOP_ADJ;
    tower_adj[0] = TOWER_A_POSITION_ADJ;
    tower_adj[1] = TOWER_B_POSITION_ADJ;
    tower_adj[2] = TOWER_C_POSITION_ADJ;
    tower_adj[3] = TOWER_A_RADIUS_ADJ;
    tower_adj[4] = TOWER_B_RADIUS_ADJ;
    tower_adj[5] = TOWER_C_RADIUS_ADJ;
    diagrod_adj[0] = TOWER_A_DIAGROD_ADJ;
    diagrod_adj[1] = TOWER_B_DIAGROD_ADJ;
    diagrod_adj[2] = TOWER_C_DIAGROD_ADJ;
  #endif

  #if ENABLED(ULTIPANEL)
    plaPreheatHotendTemp = PLA_PREHEAT_HOTEND_TEMP;
    plaPreheatHPBTemp = PLA_PREHEAT_HPB_TEMP;
    plaPreheatFanSpeed = PLA_PREHEAT_FAN_SPEED;
    absPreheatHotendTemp = ABS_PREHEAT_HOTEND_TEMP;
    absPreheatHPBTemp = ABS_PREHEAT_HPB_TEMP;
    absPreheatFanSpeed = ABS_PREHEAT_FAN_SPEED;
    gumPreheatHotendTemp = GUM_PREHEAT_HOTEND_TEMP;
    gumPreheatHPBTemp = GUM_PREHEAT_HPB_TEMP;
    gumPreheatFanSpeed = GUM_PREHEAT_FAN_SPEED;
    #if(LCD || OLED || TACTIL)
      ver_cordenadas = false;
    #endif
    #if (LCD) || (OLED)
      dir_encoder = -1;
    #endif
    #if (TACTIL)
      autolevel_on_off = true;
    #endif
    se_estaba_imprimiendo = false;
  #endif
  /*
  #if(GUARDAR)
    save_Temp;
    save_CoordZ;
    save_Linia;
  #endif
  */
  on_off_sonido_final = true;
  estatus_guardado = true;
  #if HAS(LCD_CONTRAST)
    lcd_contrast = DEFAULT_LCD_CONTRAST;
  #endif

  #if ENABLED(PIDTEMP)
    for (int8_t h = 0; h < HOTENDS; h++) {
      Kp[h] = tmp6[h];
      Ki[h] = scalePID_i(tmp7[h]);
      Kd[h] = scalePID_d(tmp8[h]);
      Kc[h] = tmp9[h];
    }
    #if ENABLED(PID_ADD_EXTRUSION_RATE)
      lpq_len = 20; // default last-position-queue size
    #endif
  #endif // PIDTEMP

  #if ENABLED(PIDTEMPBED)
    bedKp = DEFAULT_bedKp;
    bedKi = scalePID_i(DEFAULT_bedKi);
    bedKd = scalePID_d(DEFAULT_bedKd);
  #endif

  #if ENABLED(PIDTEMPCHAMBER)
    chamberKp = DEFAULT_chamberKp;
    chamberKi = scalePID_i(DEFAULT_chamberKi);
    chamberKd = scalePID_d(DEFAULT_chamberKd);
  #endif

  #if ENABLED(PIDTEMPCOOLER)
    coolerKp = DEFAULT_coolerKp;
    coolerKi = scalePID_i(DEFAULT_coolerKi);
    coolerKd = scalePID_d(DEFAULT_coolerKd);
  #endif

  #if ENABLED(FWRETRACT)
    autoretract_enabled = false;
    retract_length = RETRACT_LENGTH;
    #if EXTRUDERS > 1
      retract_length_swap = RETRACT_LENGTH_SWAP;
    #endif
    retract_feedrate = RETRACT_FEEDRATE;
    retract_zlift = RETRACT_ZLIFT;
    retract_recover_length = RETRACT_RECOVER_LENGTH;
    #if EXTRUDERS > 1
      retract_recover_length_swap = RETRACT_RECOVER_LENGTH_SWAP;
    #endif
    retract_recover_feedrate = RETRACT_RECOVER_FEEDRATE;
  #endif

  volumetric_enabled = false;

  #if ENABLED(IDLE_OOZING_PREVENT)
    IDLE_OOZING_enabled = true;
  #endif

  Config_Postprocess();

  //SERIAL_EM("Hardcoded Default Settings Loaded");
}

#if DISABLED(DISABLE_M503)

#define CONFIG_MSG_START(str) do{ if (!forReplay) SERIAL_S(CFG); SERIAL_EM(str); }while(0)

/**
 * M503 - Print Configuration
 */
void Config_PrintSettings(bool forReplay) {
  // Always have this function, even with EEPROM_SETTINGS disabled, the current values will be shown
  SERIAL_EM(ESPACIADO);
  SERIAL_MV("Pantalla: ", MSG_PANTALLA);
  SERIAL_MV("Modelo: ", MSG_MODELO);
  SERIAL_MV(MSG_VER_SERIE, numero_de_serie);
  SERIAL_E;
  SERIAL_EM(ESPACIADO);
  SERIAL_EM("                           Configuration");
  SERIAL_EM(ESPACIADO);

  SERIAL_EM("                          |Steps per unit|");
  SERIAL_EM("                               (M92)");
  SERIAL_MV("           X", planner.axis_steps_per_mm[X_AXIS]);
  SERIAL_MV(" Y", planner.axis_steps_per_mm[Y_AXIS]);
  SERIAL_MV(" Z", planner.axis_steps_per_mm[Z_AXIS]);
  SERIAL_EMV(" E", planner.axis_steps_per_mm[E_AXIS]);
  #if EXTRUDERS > 1
    for (short i = 1; i < EXTRUDERS; i++) {
      SERIAL_SMV(CFG, "  M92 T", i);
      SERIAL_EMV(" E", planner.axis_steps_per_mm[E_AXIS + i]);
    }
  #endif //EXTRUDERS > 1
  /*
  CONFIG_MSG_START("La verdad:");
  #if(LCD || OLED || TACTIL)
    SERIAL_EMV(" E", ver_cordenadas);
  #endif
  */
  SERIAL_EM("");
  SERIAL_EM("                     |Maximum feedrates (mm/s)|");
  SERIAL_EM("                               (M203)");
  SERIAL_MV("           X", planner.max_feedrate_mm_s[X_AXIS]);
  SERIAL_MV(" Y", planner.max_feedrate_mm_s[Y_AXIS] );
  SERIAL_MV(" Z", planner.max_feedrate_mm_s[Z_AXIS] );
  SERIAL_EMV(" E", planner.max_feedrate_mm_s[E_AXIS]);
  SERIAL_EM("");
  #if EXTRUDERS > 1
    for (short i = 1; i < EXTRUDERS; i++) {
      SERIAL_SMV(CFG, "  M203 T", i);
      SERIAL_EMV(" E", planner.max_acceleration_mm_per_s2[E_AXIS + i]);
    }
  #endif //EXTRUDERS > 1

  SERIAL_EM("                   |Maximum Acceleration (mm/s2)|");
  SERIAL_EM("                               (M201)");
  SERIAL_MV("                      X", planner.max_acceleration_mm_per_s2[X_AXIS] );
  SERIAL_MV(" Y", planner.max_acceleration_mm_per_s2[Y_AXIS] );
  SERIAL_MV(" Z", planner.max_acceleration_mm_per_s2[Z_AXIS] );
  SERIAL_EMV(" E", planner.max_acceleration_mm_per_s2[E_AXIS]);
  #if EXTRUDERS > 1
    for (int8_t i = 1; i < EXTRUDERS; i++) {
      SERIAL_SMV(CFG, "  M201 T", i);
      SERIAL_EMV(" E", planner.max_acceleration_mm_per_s2[E_AXIS + i]);
    }
  #endif //EXTRUDERS > 1

  SERIAL_EM("");
  SERIAL_EM("                          |Accelerations|");
  SERIAL_EM("            (M204 P=printing, V=travel and T* R=retract)");
  SERIAL_MV("                     P", planner.acceleration);
  SERIAL_MV(" V", planner.travel_acceleration);
  #if EXTRUDERS > 0
    for (int8_t i = 0; i < EXTRUDERS; i++) {
      SERIAL_EM("");
      SERIAL_MV("                          T", i);
      SERIAL_EMV(" R", planner.retract_acceleration[i]);
    }
  #endif

  SERIAL_EM(ESPACIADO);
  SERIAL_EMV("                 Sensor de filamento: ", on_off_sensor_de_filamento );
  SERIAL_EM(ESPACIADO);
  SERIAL_EM(ESPACIADO);
  SERIAL_EM("                        Configuration offset");
  SERIAL_EM(ESPACIADO);
  SERIAL_EM("                         |Home offset (mm)|");
  SERIAL_EM("                               (M206)");
  SERIAL_MV("                   X", home_offset[X_AXIS] );
  SERIAL_MV(" Y", home_offset[Y_AXIS] );
  SERIAL_EMV(" Z", home_offset[Z_AXIS] );
  SERIAL_EM("");
  SERIAL_EM("                        |Hotend offset (mm)|");
  SERIAL_EM("                               (M218)");
  SERIAL_EM("                        |Hotend offset (mm)|");
  //SERIAL_EMV(" Z", g77_offset );
  for (int8_t h = 0; h < HOTENDS; h++) {
    SERIAL_MV("                  T", h);
    SERIAL_MV(" X", hotend_offset[X_AXIS][h]);
    SERIAL_MV(" Y", hotend_offset[Y_AXIS][h]);
    SERIAL_EMV(" Z", hotend_offset[Z_AXIS][h]);
  }
  //SERIAL_EM(ESPACIADO);

  #if HAS(LCD_CONTRAST)
    CONFIG_MSG_START("LCD Contrast:");
    SERIAL_LMV(CFG, "  M250 C", lcd_contrast);
  #endif

  #if ENABLED(MESH_BED_LEVELING)
    CONFIG_MSG_START("Mesh bed leveling:");
    SERIAL_SMV(CFG, "  M420 S", mbl.has_mesh() ? 1 : 0);
    SERIAL_MV(" X", MESH_NUM_X_POINTS);
    SERIAL_MV(" Y", MESH_NUM_Y_POINTS);
    SERIAL_E;

    for (uint8_t py = 1; py <= MESH_NUM_Y_POINTS; py++) {
      for (uint8_t px = 1; px <= MESH_NUM_X_POINTS; px++) {
        SERIAL_SMV(CFG, "  G29 S3 X", px);
        SERIAL_MV(" Y", py);
        SERIAL_EMV(" Z", mbl.z_values[py-1][px-1], 5);
      }
    }
  #endif

  #if HEATER_USES_AD595
    CONFIG_MSG_START("AD595 Offset and Gain:");
    for (int8_t h = 0; h < HOTENDS; h++) {
      SERIAL_SMV(CFG, "  M595 T", h);
      SERIAL_MV(" O", ad595_offset[h]);
      SERIAL_EMV(", S", ad595_gain[h]);
    }
  #endif // HEATER_USES_AD595

  #if MECH(DELTA)
    CONFIG_MSG_START("Delta Geometry adjustment:");
    SERIAL_SMV(CFG, "  M666 A", tower_adj[0], 3);
    SERIAL_MV(" B", tower_adj[1], 3);
    SERIAL_MV(" C", tower_adj[2], 3);
    SERIAL_MV(" I", tower_adj[3], 3);
    SERIAL_MV(" J", tower_adj[4], 3);
    SERIAL_MV(" K", tower_adj[5], 3);
    SERIAL_MV(" U", diagrod_adj[0], 3);
    SERIAL_MV(" V", diagrod_adj[1], 3);
    SERIAL_MV(" W", diagrod_adj[2], 3);
    SERIAL_MV(" R", delta_radius);
    SERIAL_MV(" D", delta_diagonal_rod);
    SERIAL_EMV(" H", soft_endstop_max[2]);

    CONFIG_MSG_START("Endstop Offsets:");
    SERIAL_SMV(CFG, "  M666 X", endstop_adj[X_AXIS]);
    SERIAL_MV(" Y", endstop_adj[Y_AXIS]);
    SERIAL_EMV(" Z", endstop_adj[Z_AXIS]);

  #elif ENABLED(Z_DUAL_ENDSTOPS)
    CONFIG_MSG_START("Z2 Endstop adjustement (mm):");
    SERIAL_LMV(CFG, "  M666 Z", z_endstop_adj );

  #endif // DELTA

  /**
   * Auto Bed Leveling
   */
  #if HAS(BED_PROBE)
    CONFIG_MSG_START("Z Probe offset (mm):");
    SERIAL_LMV(CFG, "  M666 P", zprobe_zoffset);
    //SERIAL_LMV(CFG, "  Offset_K Z", offset_mesh_valor );
  #endif


  #if ENABLED(ULTIPANEL)
  #endif // ULTIPANEL

  #if ENABLED(PIDTEMP) || ENABLED(PIDTEMPBED) || ENABLED(PIDTEMPCHAMBER) || ENABLED(PIDTEMPCOOLER)
    SERIAL_EM(ESPACIADO);
    SERIAL_EM("                         Configuration PID");
    SERIAL_EM(ESPACIADO);
    SERIAL_EM("                           |PID settings|");
    SERIAL_EM("                               (M301)");
    #if ENABLED(PIDTEMP)
      for (int8_t h = 0; h < HOTENDS; h++) {
        SERIAL_MV("                H", h);
        SERIAL_MV(" P", PID_PARAM(Kp, h));
        SERIAL_MV(" I", unscalePID_i(PID_PARAM(Ki, h)));
        SERIAL_MV(" D", unscalePID_d(PID_PARAM(Kd, h)));
        #if ENABLED(PID_ADD_EXTRUSION_RATE)
          SERIAL_MV(" C", PID_PARAM(Kc, h));
        #endif
        SERIAL_E;
      }
      #if ENABLED(PID_ADD_EXTRUSION_RATE)

        SERIAL_SMV(CFG, "  M301 L", lpq_len);

      #endif
    #endif
    #if ENABLED(PIDTEMPBED)
      SERIAL_SMV(CFG, "  M304 P", bedKp);
      SERIAL_MV(" I", unscalePID_i(bedKi));
      SERIAL_EMV(" D", unscalePID_d(bedKd));
    #endif
    #if ENABLED(PIDTEMPCHAMBER)
      SERIAL_SMV(CFG, "  M305 P", chamberKp);
      SERIAL_MV(" I", unscalePID_i(chamberKi));
      SERIAL_EMV(" D", unscalePID_d(chamberKd));
    #endif
    #if ENABLED(PIDTEMPCOOLER)
      SERIAL_SMV(CFG, "  M306 P", coolerKp);
      SERIAL_MV(" I", unscalePID_i(coolerKi));
      SERIAL_EMV(" D", unscalePID_d(coolerKd));
    #endif
  #endif

  #if ENABLED(FWRETRACT)
    CONFIG_MSG_START("Retract: S=Length (mm) F:Speed (mm/m) Z: ZLift (mm)");
    SERIAL_SMV(CFG, "  M207 S", retract_length);
    #if EXTRUDERS > 1
      SERIAL_MV(" W", retract_length_swap);
    #endif
    SERIAL_MV(" F", retract_feedrate * 60);
    SERIAL_EMV(" Z", retract_zlift);

    CONFIG_MSG_START("Recover: S=Extra length (mm) F:Speed (mm/m)");
    SERIAL_SMV(CFG, "  M208 S", retract_recover_length);
    #if EXTRUDERS > 1
      SERIAL_MV(" W", retract_recover_length_swap);
    #endif
    SERIAL_MV(" F", retract_recover_feedrate * 60);

    CONFIG_MSG_START("Auto-Retract: S=0 to disable, 1 to interpret extrude-only moves as retracts or recoveries");
    SERIAL_LMV(CFG, "  M209 S", autoretract_enabled ? 1 : 0);
  #endif // FWRETRACT

  if (volumetric_enabled) {
    CONFIG_MSG_START("Filament settings:");
    SERIAL_LMV(CFG, "  M200 D", filament_size[0]);

    #if EXTRUDERS > 1
      SERIAL_LMV(CFG, "  M200 T1 D", filament_size[1]);
      #if EXTRUDERS > 2
        SERIAL_LMV(CFG, "  M200 T2 D", filament_size[2]);
        #if EXTRUDERS > 3
          SERIAL_LMV(CFG, "  M200 T3 D", filament_size[3]);
        #endif
      #endif
    #endif

  }
  else
    //CONFIG_MSG_START("  M200 D0");

  #if MB(ALLIGATOR)
    CONFIG_MSG_START("Motor current:");
    SERIAL_SMV(CFG, "  M906 X", motor_current[X_AXIS]);
    SERIAL_MV(" Y", motor_current[Y_AXIS]);
    SERIAL_MV(" Z", motor_current[Z_AXIS]);
    SERIAL_EMV(" E", motor_current[E_AXIS]);
    #if DRIVER_EXTRUDERS > 1
      for (uint8_t i = 1; i < DRIVER_EXTRUDERS; i++) {
        SERIAL_SMV(CFG, "  M906 T", i);
        SERIAL_EMV(" E", motor_current[E_AXIS + i]);
      }
    #endif // DRIVER_EXTRUDERS > 1
  #endif // ALLIGATOR

  //Configuration Advanced

  SERIAL_EM(ESPACIADO);
  SERIAL_EM("                       Configuration Advanced");
  SERIAL_EM(ESPACIADO);
  SERIAL_EM("                             |Advanced|");
  SERIAL_EM("                               (M205)");
  SERIAL_EM("       (S=Min feedrate (mm/s), V=Min travel feedrate (mm/s))");
  SERIAL_EM("      (B=minimum segment time (ms), X=maximum XY jerk (mm/s))");
  SERIAL_EM("        (Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s))");
  SERIAL_EM("");
  SERIAL_MV("     S", planner.min_feedrate_mm_s );
  SERIAL_MV(" V", planner.min_travel_feedrate_mm_s );
  SERIAL_MV(" B", planner.min_segment_time );
  SERIAL_MV(" X", planner.max_xy_jerk );
  SERIAL_MV(" Z", planner.max_z_jerk);
  SERIAL_EMV(" E", planner.max_e_jerk[0]);
  #if (EXTRUDERS > 1)
    for(int8_t i = 1; i < EXTRUDERS; i++) {
      SERIAL_SMV(CFG, "  M205 T", i);
      SERIAL_EMV(" E" , planner.max_e_jerk[i]);
    }
  #endif
  SERIAL_EM(ESPACIADO);
  SERIAL_EM(ESPACIADO);
  SERIAL_EM(ESPACIADO);
  ConfigSD_PrintSettings(forReplay);

}

void ConfigSD_PrintSettings(bool forReplay) {
  // Always have this function, even with SD_SETTINGS disabled, the current values will be shown

  #if HAS(POWER_CONSUMPTION_SENSOR)
    CONFIG_MSG_START("Watt/h consumed:");
    SERIAL_LMV(INFO, power_consumption_hour," Wh");
  #endif

  //print_job_counter.showStats();
}

#endif // !DISABLE_M503

/**
* Configuration on SD card
*
* Author: Simone Primarosa
*
*/
void ConfigSD_ResetDefault() {
  #if HAS(POWER_CONSUMPTION_SENSOR)
    power_consumption_hour = 0;
  #endif
  print_job_counter.initStats();
  SERIAL_EM("               Ok|Hardcoded SD Default Settings Loaded");
}

#if ENABLED(SDSUPPORT) && ENABLED(SD_SETTINGS)

static const char *cfgSD_KEY[] = { // Keep this in lexicographical order for better search performance(O(Nlog2(N)) insted of O(N*N)) (if you don't keep this sorted, the algorithm for find the key index won't work, keep attention.)
  "CPR",  // Number of complete prints
  "FIL",  // Filament Usage
  "NPR",  // Number of prints
#if HAS(POWER_CONSUMPTION_SENSOR)
  "PWR",  // Power Consumption
#endif
  "TME",  // Longest print job
  "TPR"   // Total printing time
};

void ConfigSD_StoreSettings() {
  if(!IS_SD_INSERTED || card.isFileOpen() || card.sdprinting) return;

  set_sd_dot();
  card.setroot(true);
  card.startWrite((char *)CFG_SD_FILE, false);
  char buff[CFG_SD_MAX_VALUE_LEN];
  ltoa(print_job_counter.data.completePrints, buff, 10);
  card.unparseKeyLine(cfgSD_KEY[SD_CFG_CPR], buff);
  ltoa(print_job_counter.data.filamentUsed, buff, 10);
  card.unparseKeyLine(cfgSD_KEY[SD_CFG_FIL], buff);
  ltoa(print_job_counter.data.numberPrints, buff, 10);
  card.unparseKeyLine(cfgSD_KEY[SD_CFG_NPR], buff);
  #if HAS(POWER_CONSUMPTION_SENSOR)
    ltoa(power_consumption_hour, buff, 10);
    card.unparseKeyLine(cfgSD_KEY[SD_CFG_PWR], buff);
  #endif
  ltoa(print_job_counter.data.printer_usage_seconds, buff, 10);
  card.unparseKeyLine(cfgSD_KEY[SD_CFG_TME], buff);
  ltoa(print_job_counter.data.printTime, buff, 10);
  card.unparseKeyLine(cfgSD_KEY[SD_CFG_TPR], buff);

  card.closeFile();
  card.setlast();
  unset_sd_dot();
}

void ConfigSD_RetrieveSettings(bool addValue) {
  if(!IS_SD_INSERTED || card.isFileOpen() || card.sdprinting || !card.cardOK) return;

  set_sd_dot();
  char key[CFG_SD_MAX_KEY_LEN], value[CFG_SD_MAX_VALUE_LEN];
  int k_idx;
  int k_len, v_len;
  card.setroot(true);
  card.selectFile((char *)CFG_SD_FILE);

  while(true) {
    k_len = CFG_SD_MAX_KEY_LEN;
    v_len = CFG_SD_MAX_VALUE_LEN;
    card.parseKeyLine(key, value, k_len, v_len);

    if(k_len == 0 || v_len == 0) break; // no valid key or value founded

    k_idx = ConfigSD_KeyIndex(key);
    if(k_idx == -1) continue;    // unknow key ignore it

    switch(k_idx) {
      case SD_CFG_CPR: {
        if(addValue) print_job_counter.data.completePrints += (unsigned long)atol(value);
        else print_job_counter.data.completePrints = (unsigned long)atol(value);
      }
      break;
      case SD_CFG_FIL: {
        if(addValue) print_job_counter.data.filamentUsed += (unsigned long)atol(value);
        else print_job_counter.data.filamentUsed = (unsigned long)atol(value);
      }
      break;
      case SD_CFG_NPR: {
        if(addValue) print_job_counter.data.numberPrints += (unsigned long)atol(value);
        else print_job_counter.data.numberPrints = (unsigned long)atol(value);
      }
      break;
    #if HAS(POWER_CONSUMPTION_SENSOR)
      case SD_CFG_PWR: {
        if(addValue) power_consumption_hour += (unsigned long)atol(value);
        else power_consumption_hour = (unsigned long)atol(value);
      }
      break;
    #endif
      case SD_CFG_TME: {
        if(addValue) print_job_counter.data.printer_usage_seconds += (unsigned long)atol(value);
        else print_job_counter.data.printer_usage_seconds = (unsigned long)atol(value);
      }
      break;
      case SD_CFG_TPR: {
        if(addValue) print_job_counter.data.printTime += (unsigned long)atol(value);
        else print_job_counter.data.printTime = (unsigned long)atol(value);
      }
      break;
    }
  }

  print_job_counter.loaded = true;
  card.closeFile();
  card.setlast();
  unset_sd_dot();
}

int ConfigSD_KeyIndex(char *key) {  // At the moment a binary search algorithm is used for simplicity, if it will be necessary (Eg. tons of key), an hash search algorithm will be implemented.
  int begin = 0, end = SD_CFG_END - 1, middle, cond;

  while(begin <= end) {
    middle = (begin + end) / 2;
    cond = strcmp(cfgSD_KEY[middle], key);
    if(!cond) return middle;
    else if(cond < 0) begin = middle + 1;
    else end = middle - 1;
  }

  return -1;
}

#endif
