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
 * Spanish
 *
 * LCD Menu Messages
 * See also documentation/LCDLanguageFont.md
 *
 */
#ifndef LANGUAGE_ES_H
#define LANGUAGE_ES_H

#define MAPPER_NON                  // For direct asci codes
#define DISPLAY_CHARSET_ISO10646_1  // use the better font on full graphic displays.


#define WELCOME_MSG                         MACHINE_NAME " lista."
#define MSG_SD                              "Tarjeta"
#define MSG_SD_INSERTED                     MSG_SD " colocada"
#define MSG_SD_REMOVED                      MSG_SD " retirada"
#define MSG_MAIN                            "Volver"
#define MSG_AUTOSTART                       "Autostart"
#define MSG_VER_SERIE                       "Serie:"
#define MSG_VER_MODELO                      " Modelo:"
#define MSG_VER_VERSION                     "Version:"
#define MSG_VER_IND_ARG                     "INDUSTRIA ARGENTINA"

//----- Idioma de pantalla Kuttercraft
#define MSG_TARJETA                         "Tarjeta Sd"
#define MSG_TARJETA_NO                      "Recargar Tarjeta Sd"
#define MSG_CALIBRAR                        "Calibrar"//11 == 31
#define MSG_CALIBRAR_BASE                   "Calibrar Cama"//13 == 25
#define MSG_CALIBRAR_BASE_                  "Ajustar Cama"//12==28
#define MSG_CALIBRAR_MAPA_                  "Mapa De Cama"//12==28
#define MSG_CANCELANDO_IMPRESION            "Cancelando Impresion"
#define MSG_PAUSANDO_IMPRESION            "Pausando Impresion"//18 == 10
#define MSG_PUEDE_TARDAR_UNOS               "Puede Tardar Unos"
#define MSG_SEGUNDOS                        "--Segundos--"
#define MSG_CALIBRAR_HOME                   "Realizando Home..."//18 MSG_CALIBRAR
#define MSG_INICIO                          "Inicio"
#define MSG_FILAMENTO                       "Ajuste de Filamento"
#define MSG_VOLVER                          "Volver"//6 == 46
#define MSG_CONTINUAR                       "Continuar"//9 == 37
#define MSG_RETIRAR                         "Retirar Filamento"//17==13
#define MSG_CARGAR                          "Cargar Filamento"//16==16
#define MSG_TEMPERATURE                     "Temperatura"//11 ===  31
#define MSG_PRE                             "Predeterminado"
#define MSG_ESPERA                          "Por favor espere"//16 == 16
#define MSG_CALENTADO                       "Calentando..."
#define MSG_MENSAJE_01                      "Seleccione"
#define MSG_MENSAJE_02                      "la temperatura"
#define MSG_MENSAJE_03                      "Extrusor:"
#define MSG_MENSAJE_04                      "la velocidad"
#define MSG_MENSAJE_FLOW_01                 "Flujo"//9 == 49
#define MSG_MANUAL                          "Manual"
#define MSG_MANUAL_AUTO                     "Calibracion Precisa"//19 == 7
#define MSG_AUTO                            "Automatica"//10==34
#define MGS_ALINEAR_EJE_Z                   "Alinear Eje Z"//14==22
#define MSG_PREHEAT                         "Predeterminado"

#define MSG_LUZ_01                          "Corte de luz!"
#define MSG_LUZ_02                          "Desea continuar"
#define MSG_LUZ_03                          "Con la Impresion"
#define MSG_LUZ_04                          "La Impresion sigue"
#define MSG_LUZ_05                          "pegada a la cama?"
#define MSG_LUZ_06                          "NO-Cont desde base"
#define MSG_LUZ_07                          "SI-Cont Ult Posicion"

#define MSG_PREHEAT_FLEX                    MSG_PREHEAT " FLEX"
#define MSG_PREHEAT_NYLON                   MSG_PREHEAT " NYLON"
#define MSG_PREHEAT_PLA                     MSG_PREHEAT " PLA"
#define MSG_PREHEAT_ABS                     MSG_PREHEAT " ABS"
#define MSG_OFF_MOTOR                       "Apagar Motores"
#define MSG_LED_ON                          "Encender Led"//12==28
#define MSG_LED_OFF                         "Apagar Led"//10==34
#define MSG_PREPARE                         "Mover Ejes"//10
#define MSG_X                               /*MSG_HOME_MENU*/ "X"
#define MSG_Y                               /*MSG_HOME_MENU*/ "Y"
#define MSG_Z                               /*MSG_HOME_MENU*/ "Z"
#define MSG_X_HOME                          MSG_HOME_MENU " X"
#define MSG_Y_HOME                          MSG_HOME_MENU " Y"
#define MSG_Z_HOME                          MSG_HOME_MENU " Z"
#define MSG_X_EJE                           MSG_EJE " X"//5
#define MSG_Y_EJE                           MSG_EJE " Y"
#define MSG_Z_EJE                           MSG_EJE " Z"
#define MSG_01MM                            MSG_MOVE " 0,1mm"//11
#define MSG_1MM                             MSG_MOVE " 1mm"//9
#define MSG_5MM                             MSG_MOVE " 5mm"
#define MSG_EJE                             "Eje"//3
#define MSG_HOME_MENU                       "Llevar al origen"//16
#define MSG_NOZZLE                          "Boquilla"
#define MSG_BED                             "Cama caliente"
#define MSG_FAN_SPEED                       "Fan de capa"
#define MSG_MOVE                            "Mover"//5
#define MSG_CALI_01                         "Movimiento..."//11 == 31
#define MSG_CALI_02                         "Posicion "//9
#define MSG_CALI_03                         "1 de 4"//6 ||6+9= 15 == 19
#define MSG_CALI_04                         "2 de 4"
#define MSG_CALI_05                         "3 de 4"
#define MSG_CALI_06                         "4 de 4"
#define MSG_MIDIENDO                        "Calibrando..."//13 == 25


#define MSG_FILAMENT_CHANGE_EXTRUDE_2       "Cargando Filamento"//18 == 10
#define MSG_FILAMENT_CHANGE_LOAD_2          "Retirando Filamento"//19 == 7


////
#define MSG_LOGROS_FILAMENTO                 "En Filamento"
#define MSG_LOGROS_RESPUESTOS                "En Respuestos"
#define MSG_LOGROS_INSUMOS                   "En Insumos"
#define MSG_LOGROS_MAQUINAS                  "En Maquinas"
#define MSG_LOGROS_TODO                      "En Nuestra Tienda"
#define MSG_LOGROS_SERVI                     "Servicio Tecnico"



//comandos de calibracion
#if(PK2_PLUS_PLUS)
  #define MSG_PRIMER_PUNTO                    "G1 Z10\nG1 X40 Y40 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_SEGUNDO                  "G1 Z10\nG1 X170 Y250 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_TERCERO                  "G1 Z10\nG1 X40 Y250 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_CUARTO                   "G1 Z10\nG1 X170 Y40 F2000\nG28 Z\nG1 Z0"
#elif(PK3_PLUS_PLUS || PK3_EXTEN)
  #define MSG_PRIMER_PUNTO                    "G1 Z10\nG1 X40 Y40 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_SEGUNDO                  "G1 Z10\nG1 X170 Y250 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_TERCERO                  "G1 Z10\nG1 X40 Y250 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_CUARTO                   "G1 Z10\nG1 X170 Y40 F2000\nG28 Z\nG1 Z0"
#elif(PK3)
  #define MSG_PRIMER_PUNTO                    "G1 Z10\nG1 X40 Y40 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_SEGUNDO                  "G1 Z10\nG1 X170 Y150 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_TERCERO                  "G1 Z10\nG1 X40 Y150 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_CUARTO                   "G1 Z10\nG1 X170 Y40 F2000\nG28 Z\nG1 Z0"
#else
  #define MSG_PRIMER_PUNTO                    "G1 Z10\nG1 X40 Y40 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_SEGUNDO                  "G1 Z10\nG1 X170 Y150 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_TERCERO                  "G1 Z10\nG1 X40 Y150 F2000\nG28 Z\nG1 Z0"
  #define MSG_PRIMER_CUARTO                   "G1 Z10\nG1 X170 Y40 F2000\nG28 Z\nG1 Z0"
#endif

#define MSG_COOLDOWN                        "Enfriar"
#define MSG_SWITCH_PS_ON                    "Encender"
#define MSG_SWITCH_PS_OFF                   "Apagar"
#define MSG_CONTROL                         "Ajustes"
#define MSG_VELOCIDAD                       "Velocidad"//9 === 37
#define MSG_PAUSE_PRINT                     "Pausar Impresion"//16 == 16
#define MSG_RESUME_PRINT                    "Reanudar Impresion"//18 == 10
#define MSG_FILAMENT_CHANGE                 "Cambiar Filamento"//17 == 13
#define MSG_STOP_PRINT                      "Reiniciar Impresora"//19 == 7
#define MSG_STOP_PRINT_01                   "Cancelar Impresion"//18 == 10
#define MSG_AJUSTAR_FLOW                    "Modificar Flujo"//15 == 19

//Estaditicas MSG
#define MSG_EST_IMPRECION                   "Impresiones"
#define MSG_EST_TOTAL                       "Total:"
#define MSG_EST_TERMINADAS                  "Terminadas:"
#define MSG_EST_INCOMPLETAS                 "Incompletas:"


#define MSG_RETIRAR_FILAMENTO               "G91\nG1 E15 F150\nG90\nG91\nG1 E-100 F300\nG90\nM84"
#define MSG_CARGAR_FILAMENTO                "G91\nG1 E100 F150\nG90\nM84"

#define MSG_CAPAS                           "Capas:"

#define MSG_PID01                        "Ajustando PID"
#define MSG_PID02                        "Calculando..."
#define MSG_PID03                        "Puede tardar unos minutos"

#define MSG_MENUS_AUTOLEVEL_00           "Ajustes de cama"//15==19
#define MSG_MENUS_AUTOLEVEL_01           "Calibrar | Offset"//17==13
#define MSG_MENUS_AUTOLEVEL_02           "Modificar Offset"//16==16
#define MSG_MENUS_AUTOLEVEL_03           "Menu Offset"//11==31
#define MSG_MENUS_AUTOLEVEL_04           "Calibrar Offset"//15==19 Calibración Rápida
#define MSG_MENUS_AUTOLEVEL_05           "Automatico"//10==34
#define MSG_MENUS_AUTOLEVEL_06           "Manual"//6==46
#define MSG_MENUS_FIN                    "Base calibrada"//6==46

#define MSG_MENUS_AUTOLEVEL_07           "Presione Clik Para"//18==10
//-- Fin
#define MSG_LEVEL_BED_NEXT_POINT         "Posicion"


#define MSG_ALINIACION_Z_ERROR_01         "ESTA OPCION ES SOLO" //19 == 7
#define MSG_ALINIACION_Z_ERROR_02         "PARA LOS MODELOS PK3"  //20 == 3

#define MSG_ALINIACION_Z_ADV_01           "Advertencia el Eje Z" //20 == 3
#define MSG_ALINIACION_Z_ADV_02           "va  a colisionar con"  //20 == 3
#define MSG_ALINIACION_Z_ADV_03           "la parte superion"  //17 == 13
#define MSG_ALINIACION_Z_ADV_04           "los motores vibraran"//20==3


#define MSG_ADV                           "Advertencia" //11 = 31
#define MSG_ADV_T1_CAMA                   "Advertencia T1 Cama" //19==7
#define MSG_ADV_T0_HOTEND                 "Advertencia T0 Hotend" //21==1
#define MSG_TEM_ERROR_02                  "Proteccion Termica"//18 == 10

#define MSG_TEM_ERROR_07                  "Variacion detectada"//19 == 7
#define MSG_TEM_ERROR_08                  "No hay Calentamiento"//20 == 3

#define MSG_TEM_ERROR_09                  "Maximo o Minimo"//15 == 19

#define MSG_TEM_ERROR_05                  "ERROR: T0 Hotend"//9==37
#define MSG_TEM_ERROR_06                  "ERROR: T1 Cama Caliente"//9==37

#define MSG_WEB                           "kuttercraft.com"
#define MSG_WWW                           "WWW."
#define MSG_STORE                         "/store"
#define MSG_STORE_M                       "Kuttercraft Store"//17==13


#define MSG_BED_Z                           "Bed Z"
#define MSG_FLOW                            "Flujo"

#define MSG_STATS                           "Statistics"
#define MSG_FIX_LOSE_STEPS                  "Fix axis steps"
#define MSG_MIN                             LCD_STR_THERMOMETER " Min"
#define MSG_MAX                             LCD_STR_THERMOMETER " Max"
#define MSG_FACTOR                          LCD_STR_THERMOMETER " Fact"
#define MSG_IDLEOOZING                      "Anti oozing"
#define MSG_AUTOTEMP                        "Autotemp"
#define MSG_ON                              "ON "
#define MSG_OFF                             "OFF"
#define MSG_PID_P                           "PID-P"
#define MSG_PID_I                           "PID-I"
#define MSG_PID_D                           "PID-D"
#define MSG_PID_C                           "PID-C"
#define MSG_H1                              " H1"
#define MSG_H2                              " H2"
#define MSG_H3                              " H3"
#define MSG_ACC                             "Accel"
#define MSG_VXY_JERK                        "Vxy-jerk"
#define MSG_VZ_JERK                         "Vz-jerk"
#define MSG_VE_JERK                         "Ve-jerk"
#define MSG_VMAX                            "Vmax "
#define MSG_E                               "E"

#define MSG_MOVE_AXIS                       MSG_MOVE " Ejes"
#define MSG_MOVE_X                          MSG_MOVE " " MSG_X
#define MSG_MOVE_Y                          MSG_MOVE " " MSG_Y
#define MSG_MOVE_Z                          MSG_MOVE " " MSG_Z
#define MSG_MOVE_Z_OFFSET                   "Offset:"
#define MSG_MOVE_01MM                       MSG_MOVE " 0.1mm"
#define MSG_MOVE_1MM                        MSG_MOVE " 1mm"
#define MSG_MOVE_10MM                       MSG_MOVE " 10mm"
#define MSG_MOVE_E                          "Extrusor"
#define MSG_VMIN                            "Vmin"
#define MSG_VTRAV_MIN                       "Vvacio min"
#define MSG_AMAX                            "Amax"
#define MSG_A_RETRACT                       "A-retrac."
#define MSG_A_TRAVEL                        "A-travel"
#define MSG_XSTEPS                          MSG_X " steps/mm"
#define MSG_YSTEPS                          MSG_Y " steps/mm"
#define MSG_ZSTEPS                          MSG_Z " steps/mm"
#define MSG_E0STEPS                         MSG_E "0 steps/mm"
#define MSG_E1STEPS                         MSG_E "1 steps/mm"
#define MSG_E2STEPS                         MSG_E "2 steps/mm"
#define MSG_E3STEPS                         MSG_E "3 steps/mm"
#define MSG_TMPERATURE                      "Temperatura"
#define MSG_MOTION                          "Movimiento"
#define MSG_VOLUMETRIC                      "Filament"
#define MSG_VOLUMETRIC_ENABLED              MSG_E " in mm3"
#define MSG_FILAMENT_SIZE_EXTRUDER          "Fil. Dia."
#define MSG_CONTRAST                        "Contraste"
#define MSG_STORE_EPROM                     "Guardar en memoria"
#define MSG_LOAD_EPROM                      "Cargar memoria"
#define MSG_RESTORE_FAILSAFE                "Rest. de emergen."
#define MSG_REFRESH                         "Volver a cargar"
#define MSG_VOL_CARP                        "Volver.."
#define MSG_WATCH                           "Menu Principal"
#define MSG_KUTTERCRAFT_MENU                "Munu Kuttercraft"
#define MSG_TUNE                            "Modificar"

#define MSG_RESUME_PRINT                    "Reanudar Impres."

#define MSG_STOP_SAVE_PRINT                 "Stop and Save"
#define MSG_CARD_MENU                       "Menu de SD"
#define MSG_NO_CARD                         "No hay tarjeta SD"
#define MSG_DWELL                           "Reposo..."
#define MSG_USERWAIT                        "Esperando ordenes"
#define MSG_RESUMING                        "Resumiendo Impre."
#define MSG_PRINT_ABORTED                   "Impresion Abortada"
#define MSG_NO_MOVE                         "Sin movimiento"
#define MSG_KILLED                          "PARADA DE EMERG."
#define MSG_STOPPED                         "PARADA"
#define MSG_CONTROL_RETRACT                 "Retraer mm"
#define MSG_CONTROL_RETRACT_SWAP            "Interc. Retraer mm"
#define MSG_CONTROL_RETRACTF                "Retraer  V"
#define MSG_CONTROL_RETRACT_ZLIFT           "Levantar mm"
#define MSG_CONTROL_RETRACT_RECOVER         "DesRet +mm"
#define MSG_CONTROL_RETRACT_RECOVER_SWAP    "Interc. DesRet +mm"
#define MSG_CONTROL_RETRACT_RECOVERF        "DesRet V"
#define MSG_AUTORETRACT                     "AutoRetr."

#define MSG_INIT_SDCARD                     "Cargar tarjeta"
#define MSG_CNG_SDCARD                      "Cambiar tarjeta"
#define MSG_ZPROBE_OUT                      "Sonda Z fuera"
#define MSG_HOME                            "Home"
#define MSG_FIRST                           "first"
#define MSG_ZPROBE_ZOFFSET                  "Offset Z"
#define MSG_ZPROBE_GUARDAR                  " Guardar"
#define MSG_ACEPTAR                         "Aceptar"//7 = 43
#define MSG_ZPROBE_CANCELAR                 " Cancelar"
#define MSG_BABYSTEP                        "Babystep"
#define MSG_BABYSTEP_X                      MSG_BABYSTEP " " MSG_X
#define MSG_BABYSTEP_Y                      MSG_BABYSTEP " " MSG_Y
#define MSG_BABYSTEP_Z                      MSG_BABYSTEP " " MSG_Z
#define MSG_ENDSTOP_ABORT                   "Endstop abort"
#define MSG_HEATING_FAILED_LCD              "Heating failed"
#define MSG_ERR_REDUNDANT_TEMP              "REDUNDANT TEMP ERROR"
//#define MSG_THERMAL_RUNAWAY                 "THERMAL RUNAWAY"
#define MSG_AD595                           "AD595 Offset & Gain"
//#define MSG_ERR_MAXTEMP                     "MAXTEMP ERROR"
//#define MSG_ERR_MINTEMP                     "MINTEMP ERROR"
//#define MSG_ERR_MAXTEMP_BED                 "MAXTEMP BED ERROR"
//#define MSG_ERR_MINTEMP_BED                 "MINTEMP BED ERROR"
//#define MSG_ERR_MAXTEMP_CHAMBER             "MAXTEMP CHAMBER ERROR"
///#define MSG_ERR_MINTEMP_CHAMBER             "MINTEMP CHAMBER ERROR"
//#define MSG_ERR_MAXTEMP_COOLER              "MAXTEMP COOLER ERROR"
//#define MSG_ERR_MINTEMP_COOLER              "MINTEMP COOLER ERROR"
#define MSG_END_DAY                         "days"
#define MSG_END_HOUR                        "hours"
#define MSG_END_MINUTE                      "minutes"

#define MSG_ENDSTOPS_HIT                    "endstops hit: "
#define MSG_BABYSTEPPING                    "Babystepping"
#define MSG_BABYSTEPPING_X                  MSG_BABYSTEPPING " " MSG_X
#define MSG_BABYSTEPPING_Y                  MSG_BABYSTEPPING " " MSG_Y
#define MSG_BABYSTEPPING_Z                  MSG_BABYSTEPPING " " MSG_Z

#define MSG_ENDSTOP_XS                      MSG_X
#define MSG_ENDSTOP_YS                      MSG_Y
#define MSG_ENDSTOP_ZS                      MSG_Z
#define MSG_ENDSTOP_ZPS                     MSG_Z "P"
#define MSG_ENDSTOP_ES                      MSG_E

// // Calibrate Delta
// #if MECH(DELTA)
//   #define MSG_DELTA_CALIBRATE               "Delta Calibration"
//   #define MSG_DELTA_CALIBRATE_X             "Calibrate " MSG_X
//   #define MSG_DELTA_CALIBRATE_Y             "Calibrate " MSG_Y
//   #define MSG_DELTA_CALIBRATE_Z             "Calibrate " MSG_Z
//   #define MSG_DELTA_CALIBRATE_CENTER        "Calibrate Center"
// #endif // DELTA

// FILAMENT_CHANGE_FEATURE
#define MSG_FILAMENT_CHANGE_HEADER          "CAMBIO DE FILAMENTO"//"CAMBIAR FILAMENTO"
#define MSG_FILAMENT_CHANGE_INIT_1          "Espere a que la"
#define MSG_FILAMENT_CHANGE_INIT_2          "Impresora se"
#define MSG_FILAMENT_CHANGE_INIT_3          "detenga"
#define MSG_FILAMENT_CHANGE_UNLOAD_1        "Espere"
#define MSG_FILAMENT_CHANGE_UNLOAD_2        "Limpiando boquilla"
#define MSG_FILAMENT_CHANGE_INSERT_1        "Presione el boton"
#define MSG_FILAMENT_CHANGE_INSERT_2        "y continue"
#define MSG_FILAMENT_CHANGE_LOAD_1          "Espere"
#define MSG_FILAMENT_CHANGE_EXTRUDE_1       "Espere"

#define MSG_FILAMENT_CHANGE_OPTION_HEADER   "Paso siguiente"
#define MSG_FILAMENT_CHANGE_OPTION_EXTRUDE  "Cargar filamento"
#define MSG_FILAMENT_CHANGE_OPTION_RESUME   "Reanudar Impresion"
#define MSG_FILAMENT_CHANGE_RESUME_1        "Espere"
#define MSG_FILAMENT_CHANGE_RESUME_2        "Reanudando Impresion"

// // Scara
// #if MECH(SCARA)
//   #define MSG_SCALE                         "Scale"
//   #define MSG_XSCALE                        MSG_X " " MSG_SCALE
//   #define MSG_YSCALE                        MSG_Y " " MSG_SCALE
// #endif

#define MSG_HEATING                         "Calentando..."
#define MSG_HEATING_COMPLETE                "Calentamiento Ok."
#define MSG_BED_HEATING                     "Calentando Cama."
#define MSG_BED_DONE                        "Cama Ok."
#define MSG_CHAMBER_HEATING                 "Calentando."
#define MSG_CHAMBER_DONE                    "Calentamiento Ok."
#define MSG_COOLER_COOLING                  "Enfriando..."
#define MSG_COOLER_DONE                     "Enfriamiento Ok."

// Extra
// #define MSG_LASER                           "Laser Preset"
// #define MSG_CONFIG                          "Configuration"
// #define MSG_E_BOWDEN_LENGTH                 MSG_EXTRUDE " " STRINGIFY(BOWDEN_LENGTH) "mm"
// #define MSG_R_BOWDEN_LENGTH                 MSG_RETRACT " " STRINGIFY(BOWDEN_LENGTH) "mm"
// #define MSG_PURGE_XMM                       MSG_PURGE " " STRINGIFY(LCD_PURGE_LENGTH) "mm"
// #define MSG_RETRACT_XMM                     MSG_RETRACT " " STRINGIFY(LCD_RETRACT_LENGTH) "mm"
// #define MSG_SAVED_POS                       "Saved position"
// #define MSG_RESTORING_POS                   "Restoring position"
// #define MSG_INVALID_POS_SLOT                "Invalid slot, total slots: "

// Rfid module
// #if ENABLED(RFID_MODULE)
//   #define MSG_RFID_SPOOL                    "Spool on E"
//   #define MSG_RFID_BRAND                    "Brand: "
//   #define MSG_RFID_TYPE                     "Type: "
//   #define MSG_RFID_COLOR                    "Color: "
//   #define MSG_RFID_SIZE                     "Size: "
//   #define MSG_RFID_TEMP_HOTEND              "Temperature Hotend: "
//   #define MSG_RFID_TEMP_BED                 "Temperature Bed: "
//   #define MSG_RFID_TEMP_USER_HOTEND         "User temperature Hotend: "
//   #define MSG_RFID_TEMP_USER_BED            "User temperatura Bed: "
//   #define MSG_RFID_DENSITY                  "Density: "
//   #define MSG_RFID_SPOOL_LENGHT             "Spool Lenght: "
// #endif

// Firmware Test
// #if ENABLED(FIRMWARE_TEST)
//   #define MSG_FWTEST_YES                    "Put the Y command to go next"
//   #define MSG_FWTEST_NO                     "Put the N command to go next"
//   #define MSG_FWTEST_YES_NO                 "Put the Y or N command to go next"
//   #define MSG_FWTEST_ENDSTOP_ERR            "ENDSTOP ERROR! Check wire and connection"
//   #define MSG_FWTEST_PRESS                  "Press and hold the endstop "
//   #define MSG_FWTEST_INVERT                 "Reverse value of "
//   #define MSG_FWTEST_XAXIS                  "Has the nozzle moved to the right?"
//   #define MSG_FWTEST_YAXIS                  "Has the nozzle moved forward?"
//   #define MSG_FWTEST_ZAXIS                  "Has the nozzle moved up?"
//   #define MSG_FWTEST_01                     "Manually move the axes X, Y and Z away from the endstop"
//   #define MSG_FWTEST_02                     "Do you want check ENDSTOP?"
//   #define MSG_FWTEST_03                     "Start check ENDSTOP"
//   #define MSG_FWTEST_04                     "Start check MOTOR"
//   #define MSG_FWTEST_ATTENTION              "ATTENTION! Check that the three axes are more than 5 mm from the endstop!"
//   #define MSG_FWTEST_END                    "Finish Test. Disable FIRMWARE_TEST and recompile."
//   #define MSG_FWTEST_INTO                   "into "
//   #define MSG_FWTEST_ERROR                  "ERROR"
//   #define MSG_FWTEST_OK                     "OK"
//   #define MSG_FWTEST_NDEF                   "not defined"
// #endif // FIRMWARE_TEST

#endif // LANGUAGE_ES_H
