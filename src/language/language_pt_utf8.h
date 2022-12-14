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
 * Portuguese
 *
 * LCD Menu Messages
 * See also documentation/LCDLanguageFont.md
 *
 */
#ifndef LANGUAGE_PT_UTF_H
#define LANGUAGE_PT_UTF_H

#define MAPPER_NON
// Define SIMULATE_ROMFONT to see what is seen on the character based display defined in Configuration.h
//#define SIMULATE_ROMFONT
#define DISPLAY_CHARSET_ISO10646_1

#define WELCOME_MSG                         MACHINE_NAME " pronto."
#define MSG_SD_INSERTED                     "Cartão inserido"
#define MSG_SD_REMOVED                      "Cartão removido"
#define MSG_MAIN                            "Menu principal"
#define MSG_AUTOSTART                       "Autostart"
#define MSG_DISABLE_STEPPERS                "Desactivar motores"
#define MSG_AUTO_HOME                       "Ir para origem"
#define MSG_LEVEL_BED_HOMING                "Indo para origem"
#define MSG_LEVEL_BED_WAITING               "Click to Begin"
#define MSG_LEVEL_BED_DONE                  "Leveling Done!"
#define MSG_LEVEL_BED_CANCEL                "Cancel"
#define MSG_SET_HOME_OFFSETS                "Definir desvio"
#define MSG_HOME_OFFSETS_APPLIED            "Offsets applied"
#define MSG_SET_ORIGIN                      "Definir origem"
#define MSG_PREHEAT_PLA                     "Pre-aquecer PLA"
#define MSG_PREHEAT_PLA_N                   "Pre-aquecer PLA"
#define MSG_PREHEAT_PLA_ALL                 "Pre-aq. PLA Tudo"
#define MSG_PREHEAT_PLA_BEDONLY             "Pre-aq. PLA " LCD_STR_THERMOMETER "Base"
#define MSG_PREHEAT_PLA_SETTINGS            "Definições PLA"
#define MSG_PREHEAT_ABS                     "Pre-aquecer ABS"
#define MSG_PREHEAT_ABS_N                   "Pre-aquecer ABS "
#define MSG_PREHEAT_ABS_ALL                 "Pre-aq. ABS Tudo"
#define MSG_PREHEAT_ABS_BEDONLY             "Pre-aq. ABS " LCD_STR_THERMOMETER "Base"
#define MSG_PREHEAT_ABS_SETTINGS            "Definições ABS"
#define MSG_COOLDOWN                        "Arrefecer"
#define MSG_SWITCH_PS_ON                    "Ligar"
#define MSG_SWITCH_PS_OFF                   "Desligar"
#define MSG_EXTRUDE                         "Extrudir"
#define MSG_RETRACT                         "Retrair"
#define MSG_MOVE_AXIS                       "Mover eixo"
#define MSG_MOVE_X                          "Mover X"
#define MSG_MOVE_Y                          "Mover Y"
#define MSG_MOVE_Z                          "Mover Z"
#define MSG_MOVE_E                          "Mover Extrusor"
#define MSG_MOVE_01MM                       "Mover 0.1mm"
#define MSG_MOVE_1MM                        "Mover 1mm"
#define MSG_MOVE_10MM                       "Mover 10mm"
#define MSG_SPEED                           "Velocidade"
#define MSG_BED_Z                           "Base Z"
#define MSG_NOZZLE                          LCD_STR_THERMOMETER " Bico"
#define MSG_BED                             LCD_STR_THERMOMETER " Base"
#define MSG_FAN_SPEED                       "Vel. ventoinha"
#define MSG_FLOW                            "Fluxo"
#define MSG_CONTROL                         "Controlo"
#define MSG_MIN                             LCD_STR_THERMOMETER " Min"
#define MSG_MAX                             LCD_STR_THERMOMETER " Max"
#define MSG_FACTOR                          LCD_STR_THERMOMETER " Fact"
#define MSG_AUTOTEMP                        "Temp. Automática"
#define MSG_ON                              "On "
#define MSG_OFF                             "Off"
#define MSG_PID_P                           "PID-P"
#define MSG_PID_I                           "PID-I"
#define MSG_PID_D                           "PID-D"
#define MSG_PID_C                           "PID-C"
#define MSG_E1                              "E1"
#define MSG_E2                              "E2"
#define MSG_E3                              "E3"
#define MSG_E4                              "E4"
#define MSG_ACC                             "Acc"
#define MSG_VXY_JERK                        "Vxy-jerk"
#define MSG_VZ_JERK                         "Vz-jerk"
#define MSG_VE_JERK                         "Ve-jerk"
#define MSG_VMAX                            " Vmax "
#define MSG_X                               "X"
#define MSG_Y                               "Y"
#define MSG_Z                               "Z"
#define MSG_E                               "E"
#define MSG_VMIN                            "Vmin"
#define MSG_VTRAV_MIN                       "VTrav min"
#define MSG_AMAX                            "Amax "
#define MSG_A_RETRACT                       "A-retracção"
#define MSG_A_TRAVEL                        "A-movimento"
#define MSG_XSTEPS                          "X passo/mm"
#define MSG_YSTEPS                          "Y passo/mm"
#define MSG_ZSTEPS                          "Z passo/mm"
#define MSG_ESTEPS                          "E passo/mm"
#define MSG_TEMPERATURE                     "Temperatura"
#define MSG_MOTION                          "Movimento"
#define MSG_VOLUMETRIC                      "Filamento"
#define MSG_VOLUMETRIC_ENABLED              "E em mm3"
#define MSG_FILAMENT_DIAM                   "Fil. Diam."
#define MSG_CONTRAST                        "Contraste"
#define MSG_STORE_EPROM                     "Guardar na memoria"
#define MSG_LOAD_EPROM                      "Carregar da memoria"
#define MSG_RESTORE_FAILSAFE                "Rest. de emergen."
#define MSG_REFRESH                         LCD_STR_REFRESH " Recarregar"
#define MSG_WATCH                           "Monitorizar"
#define MSG_PREPARE                         "Preparar"
#define MSG_TUNE                            "Afinar"
#define MSG_PAUSE_PRINT                     "Pausar impressão"
#define MSG_RESUME_PRINT                    "Retomar impressão"
#define MSG_STOP_PRINT                      "Parar impressão"
#define MSG_CARD_MENU                       "Imprimir do SD"
#define MSG_NO_CARD                         "Sem cartão SD"
#define MSG_DWELL                           "Em espera..."
#define MSG_USERWAIT                        "Á espera de ordem"
#define MSG_RESUMING                        "Retomando impressão"
#define MSG_PRINT_ABORTED                   "Impressão cancelada"
#define MSG_NO_MOVE                         "Sem movimento"
#define MSG_KILLED                          "EMERGÊNCIA. "
#define MSG_STOPPED                         "PARADO. "
#define MSG_CONTROL_RETRACT                 " Retrair mm"
#define MSG_CONTROL_RETRACT_SWAP            "Troca Retrair mm"
#define MSG_CONTROL_RETRACTF                " Retrair  V"
#define MSG_CONTROL_RETRACT_ZLIFT           " Levantar mm"
#define MSG_CONTROL_RETRACT_RECOVER         " DesRet +mm"
#define MSG_CONTROL_RETRACT_RECOVER_SWAP    "Troca DesRet +mm"
#define MSG_CONTROL_RETRACT_RECOVERF        " DesRet  V"
#define MSG_AUTORETRACT                     " AutoRetr."
#define MSG_FILAMENTCHANGE                  "Trocar filamento"
#define MSG_INIT_SDCARD                     "Inici. cartão SD"
#define MSG_CNG_SDCARD                      "Trocar cartão SD"
#define MSG_ZPROBE_OUT                      "Sensor fora/base"
#define MSG_YX_UNHOMED                      "XY antes de Z"
#define MSG_ZPROBE_ZOFFSET                  "Desvio Z"
#define MSG_BABYSTEP_X                      "Babystep X"
#define MSG_BABYSTEP_Y                      "Babystep Y"
#define MSG_BABYSTEP_Z                      "Babystep Z"
#define MSG_ENDSTOP_ABORT                   "Fim de curso"
#define MSG_HEATING_FAILED_LCD              "Aquecimento falhou"
#define MSG_ERR_REDUNDANT_TEMP              "Err: REDUNDANT TEMP"
#define MSG_THERMAL_RUNAWAY                 "THERMAL RUNAWAY"
#define MSG_ERR_MAXTEMP                     "Err: T Máxima"
#define MSG_ERR_MINTEMP                     "Err: T Mínima"
#define MSG_ERR_MAXTEMP_BED                 "Err: T Base Máxima"
#define MSG_ERR_MINTEMP_BED                 "Err: T Base Mínima"
#define MSG_ERR_MAXTEMP_COOLER              "MAXTEMP COOLER ERROR"
#define MSG_ERR_MINTEMP_COOLER              "MINTEMP COOLER ERROR"
#define MSG_END_HOUR                        "horas"
#define MSG_END_MINUTE                      "minutos"
#define MSG_HEATING                         "Aquecendo..."
#define MSG_HEATING_COMPLETE                "Aquecida."
#define MSG_BED_HEATING                     "Aquecendo base.."
#define MSG_BED_DONE                        "Base aquecida."
#define MSG_COOLER_COOLING                  "Cooling..."
#define MSG_COOLER_DONE                     "Cooling done."


#define MSG_DELTA_CALIBRATE                 "Calibração Delta"
#define MSG_DELTA_CALIBRATE_X               "Calibrar X"
#define MSG_DELTA_CALIBRATE_Y               "Calibrar Y"
#define MSG_DELTA_CALIBRATE_Z               "Calibrar Z"
#define MSG_DELTA_CALIBRATE_CENTER          "Calibrar Centro"

#endif // LANGUAGE_PT_UTF_H
