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
#include "../../base.h"
#include "ultralcd.h"

#if ENABLED(ULTRA_LCD)

int plaPreheatHotendTemp;
int plaPreheatHPBTemp;
int plaPreheatFanSpeed;

int absPreheatHotendTemp;
int absPreheatHPBTemp;
int absPreheatFanSpeed;

int gumPreheatHotendTemp;
int gumPreheatHPBTemp;
int gumPreheatFanSpeed;

int posicion_anterior = 0;

int dir_encoder = -1;

char numero_de_serie[15] = "";
char confi_actualizacion[5];//4.0.0

int numero_de_logro = 0;
int numero_de_logro_ = 0;
int numero_de_ticket = 0;

bool se_permite_el_movimineto = false;
bool tipo_de_logro = false;
bool on_off_sonido_final = true;
// int esperar_tres_minutos = 0;
long ultimo_numeros_de_kilometros = 0;
int numero_ventana_mantenimiento = 0;
int numero_de_matenimiento = 0;
bool nuevo_serial = false;
bool se_pauso_el_autoguardado = false;



bool ver_cordenadas;

bool autolevel_on_off;
bool se_estaba_imprimiendo;
bool se_viene_de_un_corte_de_luz = false;
bool imprimir_desde_base = false;

float move_menu_scale;
int tipo_eje;
int un_contador_de_tiempo;

#if HAS(LCD_FILAMENT_SENSOR) || HAS(LCD_POWER_SENSOR)
  millis_t previous_lcd_status_ms = 0;
#endif
//
//millis_t ultimo_tiempo_control_temp = 5000;
int contidad_de_controles = 0;
int cada_cinco_segundos = 0;
int cada_x_segundos = 0;

int control_actual_temp = 0;
int control_actual_temp_bed = 0;

int ultimo_control_temp = 0;
int ultimo_control_temp_caida = 0;

int solo_una_vez_error_temp_ = 1;

int cuantas_veces_paso = 0;
int cuantas_veces_paso_dos = 0;

#if(CON_SENSOR_INDUCTIVO)
  static void salirDeMeshManual(uint8_t valor);
#endif


#if HAS(LCD_POWER_SENSOR)
  millis_t print_millis = 0;
#endif

uint8_t lcd_status_message_level;
char lcd_status_message[3 * (LCD_WIDTH) + 1] = WELCOME_MSG; // worst case is kana with up to 3*LCD_WIDTH+1

#if ENABLED(DOGLCD)
  #include "ultralcd_impl_DOGM.h"
#else
  #include "ultralcd_impl_HD44780.h"
#endif

#if ENABLED(LASERBEAM)
  static void lcd_laser_focus_menu();
  static void lcd_laser_menu();
  static void lcd_laser_test_fire_menu();
  static void laser_test_fire(uint8_t power, int dwell);
  static void laser_set_focus(float f_length);
  static void action_laser_focus_custom();
  static void action_laser_focus_1mm();
  static void action_laser_focus_2mm();
  static void action_laser_focus_3mm();
  static void action_laser_focus_4mm();
  static void action_laser_focus_5mm();
  static void action_laser_focus_6mm();
  static void action_laser_focus_7mm();
  static void action_laser_test_20_50ms();
  static void action_laser_test_20_100ms();
  static void action_laser_test_100_50ms();
  static void action_laser_test_100_100ms();
  static void action_laser_test_warm();
  static void action_laser_acc_on();
  static void action_laser_acc_off();
#endif

// The main status screen
static void lcd_status_screen();

millis_t next_lcd_update_ms;

uint8_t lcdDrawUpdate = LCDVIEW_CLEAR_CALL_REDRAW; // Set when the LCD needs to draw, decrements after every draw. Set to 2 in LCD routines so the LCD gets at least 1 full redraw (first redraw is partial)

#if ENABLED(ULTIPANEL)

  // place-holders for Ki and Kd edits
  float raw_Ki, raw_Kd;

  /**
   * INVERT_ROTARY_SWITCH
   *
   * To reverse the menu direction we need a general way to reverse
   * the direction of the encoder everywhere. So encoderDirection is
   * added to allow the encoder to go the other way.
   *
   * This behavior is limited to scrolling Menus and SD card listings,
   * and is disabled in other contexts.
   */
  #if ENABLED(INVERT_ROTARY_SWITCH)
    int8_t encoderDirection = 1;
    #define ENCODER_DIRECTION_NORMAL() (encoderDirection = 1)
    #define ENCODER_DIRECTION_MENUS() (encoderDirection = -1)
  #else
    #define ENCODER_DIRECTION_NORMAL() ;
    #define ENCODER_DIRECTION_MENUS() ;
  #endif

  int8_t encoderDiff; // updated from interrupt context and added to encoderPosition every LCD update

  millis_t manual_move_start_time = 0;
  int8_t manual_move_axis = (int8_t)NO_AXIS;
  #if EXTRUDERS > 1
    int8_t manual_move_e_index = 0;
  #else
    #define manual_move_e_index 0
  #endif

  bool encoderRateMultiplierEnabled;

  bool ledKuttercraft = false;
  int32_t lastEncoderMovementMillis;

  #if HAS(POWER_SWITCH)
    extern bool powersupply;
  #endif
  static float manual_feedrate[] = MANUAL_FEEDRATE;
  static void Kuttercraft_menu();
  static void Kuttercraft_menu_start();
  static void lcd_prepare_menu();

  static void eje_x();
  static void eje_y();
  static void eje_z();
  static void eje_e();
  static void auxiliar_bot_chico();
  static void auxiliar_bot_grande();

  static void lcd_move_menu_kuttercraft_a();
  static void lcd_move_menu_kuttercraft();
  static void calibrar_Offset();
  static char contenedor_str[22];

  #if DISABLED(LASERBEAM)
    static void lcd_control_temperature_menu();
  #endif
  static void lcd_control_motion_menu();


  #if ENABLED(FILAMENT_CHANGE_FEATURE)
    static void lcd_filament_change_option_menu();
    static void lcd_filament_change_init_message();
    static void lcd_filament_change_unload_message();
    static void lcd_filament_change_insert_message();
    static void lcd_filament_change_load_message();
    static void lcd_filament_change_extrude_message();
    static void lcd_filament_change_resume_message();
  #endif

  #if ENABLED(HAS_LCD_CONTRAST)
    static void lcd_set_contrast();
  #endif

  #if ENABLED(FWRETRACT)
    static void lcd_control_retract_menu();
  #endif

  // Function pointer to menu functions.
  typedef void (*screenFunc_t)();

  // Different types of actions that can be used in menu items.
  static void _lcd_move_xyz(const char* name, AxisEnum axis);
  static void menu_action_back();
  static void menu_action_submenu(screenFunc_t data);
  static void menu_action_gcode(const char* pgcode);
  static void menu_action_function(screenFunc_t data);
  static void menu_action_setting_edit_bool(const char* pstr, bool* ptr);
  static void menu_action_setting_edit_int3(const char* pstr, int* ptr, int minValue, int maxValue);
  static void menu_action_setting_edit_float3(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float32(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float43(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float5(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float51(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float52(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_long5(const char* pstr, unsigned long* ptr, unsigned long minValue, unsigned long maxValue);
  static void menu_action_setting_edit_callback_bool(const char* pstr, bool* ptr, screenFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_int3(const char* pstr, int* ptr, int minValue, int maxValue, screenFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float3(const char* pstr, float* ptr, float minValue, float maxValue, screenFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float32(const char* pstr, float* ptr, float minValue, float maxValue, screenFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float43(const char* pstr, float* ptr, float minValue, float maxValue, screenFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float5(const char* pstr, float* ptr, float minValue, float maxValue, screenFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float51(const char* pstr, float* ptr, float minValue, float maxValue, screenFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float52(const char* pstr, float* ptr, float minValue, float maxValue, screenFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_long5(const char* pstr, unsigned long* ptr, unsigned long minValue, unsigned long maxValue, screenFunc_t callbackFunc);

  #if ENABLED(SDSUPPORT)
    static void lcd_sdcard_menu();
    static void menu_action_sdfile(const char* longFilename);
    static void restart_gcode(const char* longFilename);
    static void menu_action_sddirectory(const char* longFilename);
  #endif

  #define ENCODER_FEEDRATE_DEADZONE 10

  #if DISABLED(LCD_I2C_VIKI)
    #if DISABLED(ENCODER_STEPS_PER_MENU_ITEM)
      #define ENCODER_STEPS_PER_MENU_ITEM 5
    #endif
    #if DISABLED(ENCODER_PULSES_PER_STEP)
      #define ENCODER_PULSES_PER_STEP 1
    #endif
  #else
    #if DISABLED(ENCODER_STEPS_PER_MENU_ITEM)
      #define ENCODER_STEPS_PER_MENU_ITEM 2 // VIKI LCD rotary encoder uses a different number of steps per rotation
    #endif
    #if DISABLED(ENCODER_PULSES_PER_STEP)
      #define ENCODER_PULSES_PER_STEP 1
    #endif
  #endif


  /* Helper macros for menus */

  /**
   * START_SCREEN generates the init code for a menu function
   *
   *   encoderLine is the position based on the encoder
   *   encoderTopLine is the top menu line to display
   *   _lcdLineNr is the index of the LCD line (e.g., 0-3)
   *   _menuLineNr is the menu item to draw and process
   *   _thisItemNr is the index of each MENU_ITEM or STATIC_ITEM
   */

void sonido_final(){
  //duracion y frecuencia
  buzz(150, 1396);
  buzz(150, 932);
  buzz(150, 2349);
  buzz(150, 2793);
  buzz(150, 1864);
  buzz(400, 0);
  buzz(120, 1864);
  buzz(60, 0);
  buzz(120, 1864);
  buzz(60, 0);
  buzz(120, 1864);
  buzz(60, 0);
  buzz(900, 4186);
}

  #if ENABLED(BTN_BACK) && BTN_BACK > 0
    #define _START_SCREEN(CODE, SKIP) \
      ENCODER_DIRECTION_MENUS(); \
      encoderRateMultiplierEnabled = false; \
      if (encoderPosition > 0x8000) encoderPosition = 0; \
      int8_t encoderLine = encoderPosition / ENCODER_STEPS_PER_MENU_ITEM; \
      NOMORE(encoderTopLine, encoderLine); \
      int8_t _menuLineNr = encoderTopLine, _thisItemNr; \
      bool _skipStatic = SKIP; \
      CODE; \
      bool wasBackClicked = LCD_BACK_CLICKED; \
      if (wasBackClicked) { \
        lcd_quick_feedback(); \
        menu_action_back(); \
        return; } \
      for (int8_t _lcdLineNr = 0; _lcdLineNr < LCD_HEIGHT; _lcdLineNr++, _menuLineNr++) { \
        _thisItemNr = 0;
  #else
    #define _START_SCREEN(CODE, SKIP) \
      ENCODER_DIRECTION_MENUS(); \
      encoderRateMultiplierEnabled = false; \
      if (encoderPosition > 0x8000) encoderPosition = 0; \
      int8_t encoderLine = encoderPosition / ENCODER_STEPS_PER_MENU_ITEM; \
      NOMORE(encoderTopLine, encoderLine); \
      int8_t _menuLineNr = encoderTopLine, _thisItemNr; \
      bool _skipStatic = SKIP; \
      CODE; \
      for (int8_t _lcdLineNr = 0; _lcdLineNr < LCD_HEIGHT; _lcdLineNr++, _menuLineNr++) { \
      _thisItemNr = 0;
  #endif

  #define START_SCREEN() _START_SCREEN(NOOP, false)

  /**
   * START_MENU generates the init code for a menu function
   *
   *   wasClicked indicates the controller was clicked
   */
  #define START_MENU() _START_SCREEN(bool wasClicked = LCD_CLICKED, true)

  /**
   * MENU_ITEM generates draw & handler code for a menu item, potentially calling:
   *
   *   lcd_implementation_drawmenu_[type](sel, row, label, arg3...)
   *   menu_action_[type](arg3...)
   *
   * Examples:
   *   MENU_ITEM(back, MSG_WATCH)
   *     lcd_implementation_drawmenu_back(sel, row, PSTR(MSG_WATCH))
   *     menu_action_back()
   *
   *   MENU_ITEM(function, MSG_PAUSE_PRINT, lcd_sdcard_pause)
   *     lcd_implementation_drawmenu_function(sel, row, PSTR(MSG_PAUSE_PRINT), lcd_sdcard_pause)
   *     menu_action_function(lcd_sdcard_pause)
   *
   *   MENU_ITEM_EDIT(int3, MSG_SPEED, &feedrate_percentage, 10, 999)
   *   MENU_ITEM(setting_edit_int3, MSG_SPEED, PSTR(MSG_SPEED), &feedrate_percentage, 10, 999)
   *     lcd_implementation_drawmenu_setting_edit_int3(sel, row, PSTR(MSG_SPEED), PSTR(MSG_SPEED), &feedrate_percentage, 10, 999)
   *     menu_action_setting_edit_int3(PSTR(MSG_SPEED), &feedrate_percentage, 10, 999)
   *
   */
  #define _MENU_ITEM_PART_1(TYPE, LABEL, ARGS...) \
    if (_menuLineNr == _thisItemNr) { \
      if (lcdDrawUpdate) \
        lcd_implementation_drawmenu_ ## TYPE(encoderLine == _thisItemNr, _lcdLineNr, PSTR(LABEL), ## ARGS); \
      if (wasClicked && encoderLine == _thisItemNr) { \
        lcd_quick_feedback()

  #define _MENU_ITEM_PART_2(TYPE, ARGS...) \
        menu_action_ ## TYPE(ARGS); \
        return; \
      } \
    } \
    _thisItemNr++

  #define MENU_ITEM(TYPE, LABEL, ARGS...) do { \
      _skipStatic = false; \
      _MENU_ITEM_PART_1(TYPE, LABEL, ## ARGS); \
      _MENU_ITEM_PART_2(TYPE, ## ARGS); \
    } while(0)

  // Used to print static text with no visible cursor.
  #define STATIC_ITEM(LABEL, ARGS...) \
    if (_menuLineNr == _thisItemNr) { \
      if (_skipStatic && encoderLine <= _thisItemNr) { \
        encoderPosition += ENCODER_STEPS_PER_MENU_ITEM; \
        lcdDrawUpdate = LCDVIEW_CALL_REDRAW_NEXT; \
      } \
      if (lcdDrawUpdate) \
        lcd_implementation_drawmenu_static(_lcdLineNr, PSTR(LABEL), ## ARGS); \
    } \
    _thisItemNr++

  /**
   *
   * END_SCREEN  Closing code for a screen having only static items.
   *             Do simplified scrolling of the entire screen.
   *
   * END_MENU    Closing code for a screen with menu items.
   *             Scroll as-needed to keep the selected line in view.
   *
   * At this point _thisItemNr equals the total number of items.
   *
   */

  // Simple-scroll by using encoderLine as encoderTopLine
  #define END_SCREEN() \
    } \
    NOMORE(encoderLine, _thisItemNr - LCD_HEIGHT); \
    NOLESS(encoderLine, 0); \
    encoderPosition = encoderLine * (ENCODER_STEPS_PER_MENU_ITEM); \
    if (encoderTopLine != encoderLine) { \
      encoderTopLine = encoderLine; \
      lcdDrawUpdate = LCDVIEW_CALL_REDRAW_NEXT; \
    }

  // Scroll through menu items, scrolling as-needed to stay in view
  #define END_MENU() \
    } \
    if (encoderLine >= _thisItemNr) { \
      encoderLine = _thisItemNr - 1; \
      encoderPosition = encoderLine * (ENCODER_STEPS_PER_MENU_ITEM); \
    } \
    if (encoderLine >= encoderTopLine + LCD_HEIGHT) { \
      encoderTopLine = encoderLine - (LCD_HEIGHT - 1); \
      lcdDrawUpdate = LCDVIEW_CALL_REDRAW_NEXT; \
    }

  #if ENABLED(ENCODER_RATE_MULTIPLIER)

    //#define ENCODER_RATE_MULTIPLIER_DEBUG  // If defined, output the encoder steps per second value

    /**
     * MENU_MULTIPLIER_ITEM generates drawing and handling code for a multiplier menu item
     */
    #define MENU_MULTIPLIER_ITEM(type, label, args...) do { \
        _MENU_ITEM_PART_1(type, label, ## args); \
        encoderRateMultiplierEnabled = true; \
        lastEncoderMovementMillis = 0; \
        _MENU_ITEM_PART_2(type, ## args); \
      } while(0)

  #endif // ENCODER_RATE_MULTIPLIER

  #define MENU_ITEM_DUMMY() do { _thisItemNr++; } while(0)
  #define MENU_ITEM_EDIT(type, label, args...) MENU_ITEM(setting_edit_ ## type, label, PSTR(label), ## args)
  #define MENU_ITEM_EDIT_CALLBACK(type, label, args...) MENU_ITEM(setting_edit_callback_ ## type, label, PSTR(label), ## args)
  #if ENABLED(ENCODER_RATE_MULTIPLIER)
    #define MENU_MULTIPLIER_ITEM_EDIT(type, label, args...) MENU_MULTIPLIER_ITEM(setting_edit_ ## type, label, PSTR(label), ## args)
    #define MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(type, label, args...) MENU_MULTIPLIER_ITEM(setting_edit_callback_ ## type, label, PSTR(label), ## args)
  #else // !ENCODER_RATE_MULTIPLIER
    #define MENU_MULTIPLIER_ITEM_EDIT(type, label, args...) MENU_ITEM(setting_edit_ ## type, label, PSTR(label), ## args)
    #define MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(type, label, args...) MENU_ITEM(setting_edit_callback_ ## type, label, PSTR(label), ## args)
  #endif // !ENCODER_RATE_MULTIPLIER

  /** Used variables to keep track of the menu */
  volatile uint8_t buttons;  //the last checked buttons in a bit array.
  #if ENABLED(REPRAPWORLD_KEYPAD)
    volatile uint8_t buttons_reprapworld_keypad; // to store the keypad shift register values
  #endif

  #if ENABLED(LCD_HAS_SLOW_BUTTONS)
    volatile uint8_t slow_buttons; // Bits of the pressed buttons.
  #endif
  int8_t encoderTopLine;              /* scroll offset in the current menu */
  millis_t next_button_update_ms;
  uint8_t lastEncoderBits;
  uint32_t encoderPosition;
  #if PIN_EXISTS(SD_DETECT)
    uint8_t lcd_sd_status;
  #endif

  typedef struct {
    screenFunc_t menu_function;
    uint32_t encoder_position;
  } menuPosition;

  screenFunc_t currentScreen = lcd_status_screen; // pointer to the currently active menu handler

  menuPosition screen_history[10];
  uint8_t screen_history_depth = 0;

  bool ignore_click = false;
  bool wait_for_unclick;
  bool defer_return_to_status = false;

  // Variables used when editing values.
  const char* editLabel;
  void* editValue;
  int32_t minEditValue, maxEditValue;
  screenFunc_t callbackFunc;              // call this after editing

  /**
   * General function to go directly to a menu
   * Remembers the previous position
   */

  static void lcd_goto_screen(screenFunc_t screen, const bool feedback = false, const uint32_t encoder = 0) {
    if (currentScreen != screen) {
      currentScreen = screen;
      lcdDrawUpdate = LCDVIEW_CLEAR_CALL_REDRAW;
      encoderPosition = encoder;
      if (feedback) lcd_quick_feedback();
      if (screen == lcd_status_screen) {
        defer_return_to_status = false;
        screen_history_depth = 0;
      }
      #if ENABLED(LCD_PROGRESS_BAR)
        // For LCD_PROGRESS_BAR re-initialize custom characters
        lcd_set_custom_characters(screen == lcd_status_screen);
      #endif
    }
  }
  static void lcd_return_to_status() { lcd_goto_screen(lcd_status_screen); }

  inline void lcd_save_previous_menu() {
    if (screen_history_depth < COUNT(screen_history)) {
      screen_history[screen_history_depth].menu_function = currentScreen;
      screen_history[screen_history_depth].encoder_position = encoderPosition;
      ++screen_history_depth;
    }
  }

  static void lcd_goto_previous_menu(bool feedback=false) {
    if (screen_history_depth > 0) {
      --screen_history_depth;
      lcd_goto_screen(
        screen_history[screen_history_depth].menu_function,
        feedback,
        screen_history[screen_history_depth].encoder_position
      );
    }
    else
      lcd_return_to_status();
  }

  void lcd_ignore_click(bool b) {
    ignore_click = b;
    wait_for_unclick = false;
  }

#endif // ULTIPANEL

/**
 *
 * "Info Screen"
 *
 * This is very display-dependent, so the lcd implementation draws this.
 */
static void lcd_status_screen() {

  #if ENABLED(ULTIPANEL)
    ENCODER_DIRECTION_NORMAL();
    encoderRateMultiplierEnabled = false;
  #endif

  #if ENABLED(LCD_PROGRESS_BAR)
    millis_t ms = millis();
    #if DISABLED(PROGRESS_MSG_ONCE)
      if (ELAPSED(ms, progress_bar_ms + PROGRESS_BAR_MSG_TIME + PROGRESS_BAR_BAR_TIME)) {
        progress_bar_ms = ms;
      }
    #endif
    #if PROGRESS_MSG_EXPIRE > 0
      // Handle message expire
      if (expire_status_ms > 0) {
        #if ENABLED(SDSUPPORT)
          if (card.isFileOpen()) {
            // Expire the message when printing is active
            if (IS_SD_PRINTING) {
              if (ELAPSED(ms, expire_status_ms)) {
                lcd_status_message[0] = '\0';
                expire_status_ms = 0;
              }
            }
            else {
              expire_status_ms += LCD_UPDATE_INTERVAL;
            }
          }
          else {
            expire_status_ms = 0;
          }
        #else
          expire_status_ms = 0;
        #endif // SDSUPPORT
      }
    #endif
  #endif // LCD_PROGRESS_BAR
  //muestra el status del inicio
  lcd_implementation_status_screen();

  #if HAS(LCD_POWER_SENSOR)
    if (ELAPSED(millis(), print_millis + 2000UL)) print_millis = millis();
  #endif

  #if HAS(LCD_FILAMENT_SENSOR) || HAS(LCD_POWER_SENSOR)
    #if HAS(LCD_FILAMENT_SENSOR) && HAS(LCD_POWER_SENSOR)
      if (ELAPSED(millis(), previous_lcd_status_ms + 15000UL))
    #else
      if (ELAPSED(millis(), previous_lcd_status_ms + 10000UL))
    #endif
    {
      previous_lcd_status_ms = millis();
    }
  #endif

  #if ENABLED(ULTIPANEL)

    bool current_click = LCD_CLICKED;
    if (ignore_click) {
      if (wait_for_unclick) {
        if (!current_click)
          ignore_click = wait_for_unclick = false;
        else
          current_click = false;
      }
      else if (current_click) {
        lcd_quick_feedback();
        wait_for_unclick = true;
        current_click = false;
      }
    }
    if (current_click) {
      lcd_goto_screen(Kuttercraft_menu_start, true);
      lcd_implementation_init( // to maybe revive the LCD if static electricity killed it.
        #if ENABLED(LCD_PROGRESS_BAR) && ENABLED(ULTIPANEL)
          currentScreen == lcd_status_screen
        #endif
      );
      #if HAS(LCD_FILAMENT_SENSOR) || HAS(LCD_POWER_SENSOR)
        previous_lcd_status_ms = millis();  // get status message to show up for a while
      #endif
    }

    #if ENABLED(ULTIPANEL_FEEDMULTIPLY)
      int new_frm = feedrate_percentage + (int32_t)encoderPosition;
      // Dead zone at 100% feedrate
      if ((feedrate_percentage < 100 && new_frm > 100) || (feedrate_percentage > 100 && new_frm < 100)) {
        feedrate_percentage = 100;
        encoderPosition = 0;
      }
      else if (feedrate_percentage == 100) {
        if ((int32_t)encoderPosition > ENCODER_FEEDRATE_DEADZONE) {
          feedrate_percentage += (int32_t)encoderPosition - (ENCODER_FEEDRATE_DEADZONE);
          encoderPosition = 0;
        }
        else if ((int32_t)encoderPosition < -(ENCODER_FEEDRATE_DEADZONE)) {
          feedrate_percentage += (int32_t)encoderPosition + ENCODER_FEEDRATE_DEADZONE;
          encoderPosition = 0;
        }
      }
      else {
        feedrate_percentage = new_frm;
        encoderPosition = 0;
      }
    #endif // ULTIPANEL_FEEDMULTIPLY

    feedrate_percentage = constrain(feedrate_percentage, 10, 999);

  #endif // ULTIPANEL
}

static int ver_error_de_tempe() {
  //guarda la temperatura actual
  control_actual_temp = int(degHotend(0));
  control_actual_temp_bed = int(degBed());

  //solo se ejecuta una sola vez
  //error por Thermistor parcialmente cortado
  //guarda una temperatura para comparar mas adelante
  if(contidad_de_controles <= 4){
    //valor de la temp a 4000ms solo se ejecuta una vez
    ultimo_control_temp = control_actual_temp;
    ultimo_control_temp_caida = control_actual_temp;
  }

  /////////////////////
  //primer control  /
  ///////////////////

  //este mensaje aparece solo una vez
  if(solo_una_vez_error_temp_ == 1){
    //para evitar errores se espera a que pase un poco de tiempo
    if(contidad_de_controles >= 5){
      //controla que no halla error de temperatura Minima o Maxima (hotend)
      if(control_actual_temp == 0 || control_actual_temp >= 300){
        solo_una_vez_error_temp_ = 0;
        return 1;

      //controla que no halla error de temperatura Minima o Maxima (bed)
      }else if(control_actual_temp_bed == 0 || control_actual_temp_bed >= 120){
        solo_una_vez_error_temp_ = 0;
        return 2;
      }

    }
  }//fin del solo_una_vez_error_temp
  //fin del primer control de temperatura

  /////////////////////
  //segundo control  /
  ///////////////////

  ///picos de temperatura
  //entra si se cumple el tiempo para comparar temperaturas
  if(cada_cinco_segundos >= 5){
    //reinicia el contador
    cada_cinco_segundos = 0;

    //guarda la diferencia de temperatura
    int aux_temp =  control_actual_temp - ultimo_control_temp;

    //busca una variacion de temperatura muy alta en poco tiempo
    //unos 5 segundos algo que puede ser muy malo
    if(!(degTargetHotend(0) == 0)){
      //SERIAL_EM("Es Menor A 100");
      if(aux_temp >= 25 || aux_temp <= -20){
        //futuro error
        return 3;
      }
    }

    //guarda el valor actual para que sea el viejo en el futuro
    ultimo_control_temp = control_actual_temp;
  }else{
    //sigue aumentando
    cada_cinco_segundos += 1;
  }

  /////////////////////
  //Trecero control  /
  ///////////////////

  // comprueba que el extrusor este calentando
  // de no ser asi esto causa un error
  //comprueba picos muy altos
  if(contidad_de_controles >= 5){
    if(cada_x_segundos == 3){
      if(isHeatingHotend(0)){

        //guarda la diferencia entre temp obetivo y actual
        int aux_comparacion_temp = degTargetHotend(0) - control_actual_temp;
        int aux_cuanto_es_la_caida = ultimo_control_temp_caida - control_actual_temp;
        if(aux_comparacion_temp >= 10){
          if(aux_cuanto_es_la_caida >= 1){
            cuantas_veces_paso += 1;
          }else{
            cuantas_veces_paso = 0;
          }
        }

        if(cuantas_veces_paso >= 9){
          return 4;
        }
        ////////////////////
        //Cuarto control  /
        //////////////////
        if(aux_comparacion_temp >= 20){
          if(aux_cuanto_es_la_caida >= 0){
            cuantas_veces_paso_dos += 1;
          }else{
            cuantas_veces_paso_dos = 0;
          }
        }else{
          cuantas_veces_paso_dos = 0;
        }
        if(cuantas_veces_paso_dos >= 5){
          return 4;
        }
      }

      //guarda el valor actual en el momento de ejecusion
      //este es el valor anterior
      ultimo_control_temp_caida = control_actual_temp;

      //restablece la cuenta
      cada_x_segundos = 0;
    }else{
      cada_x_segundos += 1;
    }
    return 0;
  }

  //reinicia contidad_de_controles debido a que el contador es un int
  if(contidad_de_controles >= 200){
    contidad_de_controles = 6;
  }
  //cuenta que termino un control
  contidad_de_controles += 1;

}

/**
 *
 * draw the kill screen
 *
 */
 void led_prende_apaga() {
   if(ledKuttercraft){
     analogWrite(LUZ_KUTTERCRAFT, 0);
     ledKuttercraft = false;
   }else{
     analogWrite(LUZ_KUTTERCRAFT, 255);
     ledKuttercraft = true;
   }
   //encoderPosition = 5;
   lcd_goto_screen(Kuttercraft_menu,true,5);

}
void Kuttercraft_menu_print();
void led_prende_apaga2() {
  if(ledKuttercraft){
    analogWrite(LUZ_KUTTERCRAFT, 0);
    ledKuttercraft = false;
  }else{
    analogWrite(LUZ_KUTTERCRAFT, 255);
    ledKuttercraft = true;
  }
  lcd_goto_screen(Kuttercraft_menu_print, true, 4);

}
void kill_screen(const char* lcd_msg) {
  lcd_init();
  lcd_setalertstatuspgm(lcd_msg);
  #if ENABLED(DOGLCD)
    u8g.firstPage();
    do {
      lcd_kill_screen();
    } while (u8g.nextPage());
  #else
    lcd_kill_screen();
  #endif
}

#if ENABLED(ULTIPANEL)

  inline void line_to_current(AxisEnum axis) {
    #if MECH(DELTA)
      inverse_kinematics(current_position);
      planner.buffer_line(delta[X_AXIS], delta[Y_AXIS], delta[Z_AXIS], current_position[E_AXIS], manual_feedrate[axis]/60, active_extruder, active_driver);
    #else // !DELTA
      planner.buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[axis]/60, active_extruder, active_driver);
    #endif // !DELTA
  }

  #if ENABLED(SDSUPPORT)

    static void lcd_sdcard_stop() {
      card.sdprinting = false;
      card.closeFile();
      clear_command_queue();
      quickstop_stepper();
      print_job_counter.stop();
      autotempShutdown();
      wait_for_heatup = false;
      lcd_setstatus(MSG_PRINT_ABORTED, true);
    }

    static void lcd_sdcard_stop_save() {
      card.sdprinting = false;
      print_job_counter.stop();
      quickstop_stepper();
      card.closeFile(true);
      autotempShutdown();
      wait_for_heatup = false;
    }

  #endif // SDSUPPORT

  /**
   *
   * "Main" menu
   *
   */
   //primer menu Kuttercraft

  #if ENABLED(BABYSTEPPING)

    int babysteps_done = 0;

    static void _lcd_babystep(const AxisEnum axis, const char* msg) {
      if (LCD_CLICKED) { lcd_goto_previous_menu(true); return; }
      ENCODER_DIRECTION_NORMAL();
      if (encoderPosition) {
        int distance = (int32_t)encoderPosition * BABYSTEP_MULTIPLICATOR;
        encoderPosition = 0;
        lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
        #if MECH(COREXY) || MECH(COREYX)|| MECH(COREXZ) || MECH(COREZX)
          #if ENABLED(BABYSTEP_XY)
            switch(axis) {
              case CORE_AXIS_1: // X on CoreXY, Core YX, CoreXZ and CoreZZ
                babystepsTodo[CORE_AXIS_1] += distance * 2;
                babystepsTodo[CORE_AXIS_2] += distance * 2;
                break;
              case CORE_AXIS_2: // Y on CoreXY and CoreYX, Z on CoreXZ and CoreZX
                babystepsTodo[CORE_AXIS_1] += distance * 2;
                babystepsTodo[CORE_AXIS_2] -= distance * 2;
                break;
              case NORMAL_AXIS: // Z on CoreXY and CoreYX, Y on CoreXZ and CoreZX
                babystepsTodo[NORMAL_AXIS] += distance;
                break;
            }
          #elif MECH(COREXZ) || MECH(COREZX)
            babystepsTodo[CORE_AXIS_1] += distance * 2;
            babystepsTodo[CORE_AXIS_2] -= distance * 2;
          #else
            babystepsTodo[Z_AXIS] += distance;
          #endif
        #else
          babystepsTodo[axis] += distance;
        #endif

        babysteps_done += distance;
      }
      if (lcdDrawUpdate)
        lcd_implementation_drawedit(msg, ftostr43sign(
          ((1000 * babysteps_done) / planner.axis_steps_per_mm[axis]) * 0.001f
        ));
    }

    #if ENABLED(BABYSTEP_XY)
      static void _lcd_babystep_x() { _lcd_babystep(X_AXIS, PSTR(MSG_BABYSTEPPING_X)); }
      static void _lcd_babystep_y() { _lcd_babystep(Y_AXIS, PSTR(MSG_BABYSTEPPING_Y)); }
      static void lcd_babystep_x() { babysteps_done = 0; lcd_goto_screen(_lcd_babystep_x); }
      static void lcd_babystep_y() { babysteps_done = 0; lcd_goto_screen(_lcd_babystep_y); }
    #endif
    static void _lcd_babystep_z() { _lcd_babystep(Z_AXIS, PSTR(MSG_BABYSTEPPING_Z)); }
    static void lcd_babystep_z() { babysteps_done = 0; lcd_goto_screen(_lcd_babystep_z); }

  #endif // BABYSTEPPING

  static void lcd_tune_fixstep() {
    #if MECH(DELTA)
      enqueue_and_echo_commands_P(PSTR("G28 B"));
    #else
      enqueue_and_echo_commands_P(PSTR("G28 X Y B"));
    #endif
  }

  /**
   * Watch temperature callbacks
   */
    #if ENABLED(THERMAL_PROTECTION_HOTENDS)
    #if TEMP_SENSOR_0 != 0
      void watch_temp_callback_E0() { start_watching_heater(0); }
    #endif
    #if HOTENDS > 1 && TEMP_SENSOR_1 != 0
      void watch_temp_callback_E1() { start_watching_heater(1); }
    #endif // HOTENDS > 1
    #if HOTENDS > 2 && TEMP_SENSOR_2 != 0
      void watch_temp_callback_E2() { start_watching_heater(2); }
    #endif // HOTENDS > 2
    #if HOTENDS > 3 && TEMP_SENSOR_3 != 0
      void watch_temp_callback_E3() { start_watching_heater(3); }
    #endif // HOTENDS > 3
  #else
    #if TEMP_SENSOR_0 != 0
      void watch_temp_callback_E0() {}
    #endif
    #if HOTENDS > 1 && TEMP_SENSOR_1 != 0
      void watch_temp_callback_E1() {}
    #endif // HOTENDS > 1
    #if HOTENDS > 2 && TEMP_SENSOR_2 != 0
      void watch_temp_callback_E2() {}
    #endif // HOTENDS > 2
    #if HOTENDS > 3 && TEMP_SENSOR_3 != 0
      void watch_temp_callback_E3() {}
    #endif // HOTENDS > 3
  #endif

  #if ENABLED(THERMAL_PROTECTION_BED)
    #if TEMP_SENSOR_BED != 0
      void watch_temp_callback_bed() { start_watching_bed(); }
    #endif
  #else
    #if TEMP_SENSOR_BED != 0
      void watch_temp_callback_bed() {}
    #endif
  #endif

  #if ENABLED(THERMAL_PROTECTION_CHAMBER)
    #if TEMP_SENSOR_CHAMBER != 0
      void watch_temp_callback_chamber() { start_watching_chamber(); }
    #endif
  #else
    #if TEMP_SENSOR_CHAMBER != 0
      void watch_temp_callback_chamber() {}
    #endif
  #endif

  #if ENABLED(THERMAL_PROTECTION_COOLER)
    #if TEMP_SENSOR_COOLER != 0
      void watch_temp_callback_cooler() { start_watching_cooler(); }
    #endif
  #else
    #if TEMP_SENSOR_COOLER != 0
      void watch_temp_callback_cooler() {}
    #endif
  #endif


  #if ENABLED(EASY_LOAD)
    static void lcd_extrude(float length, float feedrate) {
      current_position[E_AXIS] += length;
      #if MECH(DELTA)
        inverse_kinematics(current_position);
        planner.buffer_line(delta[X_AXIS], delta[Y_AXIS], delta[Z_AXIS], current_position[E_AXIS], feedrate, active_extruder, active_driver);
      #else
        planner.buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], feedrate, active_extruder, active_driver);
      #endif
    }
    static void lcd_purge() { lcd_extrude(LCD_PURGE_LENGTH, LCD_PURGE_FEEDRATE); }
    static void lcd_retract() { lcd_extrude(-LCD_RETRACT_LENGTH, LCD_RETRACT_FEEDRATE); }
    static void lcd_easy_load() {
      allow_lengthy_extrude_once = true;
      lcd_extrude(BOWDEN_LENGTH, LCD_LOAD_FEEDRATE);
      lcd_return_to_status();
    }
    static void lcd_easy_unload() {
      allow_lengthy_extrude_once = true;
      lcd_extrude(-BOWDEN_LENGTH, LCD_UNLOAD_FEEDRATE);
      lcd_return_to_status();
    }
  #endif // EASY_LOAD

  /**
   *
   * "Prepare" submenu items
   *
   */
  void _lcd_preheat(int endnum, const float temph, const float tempb, const int fan) {
    if (temph > 0) setTargetHotend(temph, endnum);
   #if TEMP_SENSOR_BED != 0
     setTargetBed(tempb);
   #else
     UNUSED(tempb);
   #endif
   fanSpeed = fan;
   lcd_return_to_status();
 }

  #if TEMP_SENSOR_0 != 0
    void lcd_preheat_pla0() { _lcd_preheat(0, plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed); }
    void lcd_preheat_abs0() { _lcd_preheat(0, absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed); }
    void lcd_preheat_gum0() { _lcd_preheat(0, gumPreheatHotendTemp, gumPreheatHPBTemp, gumPreheatFanSpeed); }
  #endif

  #if HOTENDS > 1
    void lcd_preheat_pla1() { _lcd_preheat(1, plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed); }
    void lcd_preheat_abs1() { _lcd_preheat(1, absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed); }
    void lcd_preheat_gum1() { _lcd_preheat(1, gumPreheatHotendTemp, gumPreheatHPBTemp, gumPreheatFanSpeed); }
    #if HOTENDS > 2
      void lcd_preheat_pla2() { _lcd_preheat(2, plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed); }
      void lcd_preheat_abs2() { _lcd_preheat(2, absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed); }
      void lcd_preheat_gum2() { _lcd_preheat(2, gumPreheatHotendTemp, gumPreheatHPBTemp, gumPreheatFanSpeed); }
      #if HOTENDS > 3
        void lcd_preheat_pla3() { _lcd_preheat(3, plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed); }
        void lcd_preheat_abs3() { _lcd_preheat(3, absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed); }
        void lcd_preheat_gum3() { _lcd_preheat(3, gumPreheatHotendTemp, gumPreheatHPBTemp, gumPreheatFanSpeed); }
      #endif
    #endif

    void lcd_preheat_pla0123() {
      #if HOTENDS > 1
        setTargetHotend(plaPreheatHotendTemp, 1);
        #if HOTENDS > 2
          setTargetHotend(plaPreheatHotendTemp, 2);
          #if HOTENDS > 3
            setTargetHotend(plaPreheatHotendTemp, 3);
          #endif
        #endif
      #endif
      lcd_preheat_pla0();
    }
    void lcd_preheat_abs0123() {
      #if HOTENDS > 1
        setTargetHotend(absPreheatHotendTemp, 1);
        #if HOTENDS > 2
          setTargetHotend(absPreheatHotendTemp, 2);
          #if HOTENDS > 3
            setTargetHotend(absPreheatHotendTemp, 3);
          #endif
        #endif
      #endif
      lcd_preheat_abs0();
    }
    void lcd_preheat_gum0123() {
      #if HOTENDS > 1
        setTargetHotend(gumPreheatHotendTemp, 1);
        #if HOTENDS > 2
          setTargetHotend(gumPreheatHotendTemp, 2);
          #if HOTENDS > 3
            setTargetHotend(gumPreheatHotendTemp, 3);
          #endif
        #endif
      #endif
      lcd_preheat_gum0();
    }

  #endif // HOTENDS > 1

  #if TEMP_SENSOR_BED != 0
    void lcd_preheat_pla_bedonly() { _lcd_preheat(0, 0, plaPreheatHPBTemp, plaPreheatFanSpeed); }
    void lcd_preheat_abs_bedonly() { _lcd_preheat(0, 0, absPreheatHPBTemp, absPreheatFanSpeed); }
    void lcd_preheat_gum_bedonly() { _lcd_preheat(0, 0, gumPreheatHPBTemp, gumPreheatFanSpeed); }
  #endif

  #if TEMP_SENSOR_0 != 0 && (TEMP_SENSOR_1 != 0 || TEMP_SENSOR_2 != 0 || TEMP_SENSOR_3 != 0 || TEMP_SENSOR_BED != 0)

  #endif // TEMP_SENSOR_0 && (TEMP_SENSOR_1 || TEMP_SENSOR_2 || TEMP_SENSOR_3 || TEMP_SENSOR_BED)
  void volver_info() {
    lcd_return_to_status();
  }
  void pla_k() {
    setTargetHotend(190, 1);
    setTargetBed(60);
    lcd_return_to_status();
  }
  void abs_k() {
    setTargetHotend(230, 1);
    setTargetBed(80);
    lcd_return_to_status();
  }
  void flex_k() {
    setTargetHotend(210, 1);
    setTargetBed(70);
    lcd_return_to_status();
  }
  void nylon_k() {
    setTargetHotend(240, 1);
    setTargetBed(80);
    lcd_return_to_status();
  }
  void pla_k2() {
    enqueue_and_echo_commands_P(PSTR("M104 T0 S190"));
    posicion_anterior = 31;
    lcd_goto_screen(auxiliar_bot_chico, true, 0);
  }
  void abs_k2() {
    enqueue_and_echo_commands_P(PSTR("M104 T0 S230"));
    posicion_anterior = 31;
    lcd_goto_screen(auxiliar_bot_chico, true, 0);
  }
  void flex_k2() {
    enqueue_and_echo_commands_P(PSTR("M104 T0 S210"));
    posicion_anterior = 31;
    lcd_goto_screen(auxiliar_bot_chico, true, 0);
  }
  void nylon_k2() {
    enqueue_and_echo_commands_P(PSTR("M104 T0 S240"));
    posicion_anterior = 31;
    lcd_goto_screen(auxiliar_bot_chico, true, 0);
  }
  void lcd_cooldown() {
    disable_all_heaters();
    disable_all_coolers();
    fanSpeed = 0;
    lcd_return_to_status();
  }

  #if ENABLED(SDSUPPORT) && ENABLED(MENU_ADDAUTOSTART)
    static void lcd_autostart_sd() {
      card.checkautostart(true);
    }
  #endif


  #if(CON_SENSOR_INDUCTIVO)
  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
    /**
     *
     * "Prepare" > "Bed Leveling" handlers
     *
     */

    //static uint8_t _lcd_level_bed_position;
    static uint8_t aux_de_tiempo2;

    // Utility to go to the next mesh point
    // A raise is added between points if MIN_Z_HEIGHT_FOR_HOMING is in use
    // Note: During Manual Bed Leveling the homed Z position is MESH_HOME_SEARCH_Z
    // Z position will be restored with the final action, a G28
    /*
    inline void _mbl_goto_xy(float x, float y) {
      current_position[Z_AXIS] = MESH_HOME_SEARCH_Z + MIN_Z_HEIGHT_FOR_HOMING;
      line_to_current(Z_AXIS);
      current_position[X_AXIS] = x + home_offset[X_AXIS];
      current_position[Y_AXIS] = y + home_offset[Y_AXIS];
      line_to_current(manual_feedrate[X_AXIS] <= manual_feedrate[Y_AXIS] ? X_AXIS : Y_AXIS);
      #if MIN_Z_HEIGHT_FOR_HOMING > 0
        current_position[Z_AXIS] = MESH_HOME_SEARCH_Z;
        line_to_current(Z_AXIS);
      #endif
      stepper.synchronize();
      //enqueue_and_echo_commands_P(PSTR("G1 Z0"));
    }

    static void _lcd_level_goto_next_point();
    //static void _mash_auto_04();

    static void _lcd_level_bed_done() {
        enqueue_and_echo_commands_P(PSTR("M500"));
      if (lcdDrawUpdate) lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_DONE));
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_CALL_NO_REDRAW
        #endif
      ;
    }
    */
    //--------------------------------------------------------------------------
      //static void _mash_auto_04();
      /**
       * Step 7: Get the Z coordinate, then goto next point or exit
       */
       /*
      static void _mash_auto_06() {
        ENCODER_DIRECTION_NORMAL();
        // Encoder wheel adjusts the Z position
          //lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
          refresh_cmd_timeout();
          //NOLESS(current_position[Z_AXIS], -(LCD_PROBE_Z_RANGE) * 0.5);
          //NOMORE(current_position[Z_AXIS],  (LCD_PROBE_Z_RANGE) * 0.5);
          lcdDrawUpdate =
            #if ENABLED(DOGLCD)
              LCDVIEW_CALL_REDRAW_NEXT
            #else
              LCDVIEW_REDRAW_NOW
            #endif
          ;

        if(aux_de_tiempo2 == _lcd_level_bed_position){
          enqueue_and_echo_commands_P(PSTR("G30"));
          aux_de_tiempo2++;
        }
        static bool debounce_click = false;
        if (aux_de_tiempo == _lcd_level_bed_position + 1) {
          if (true) {
            SERIAL_MV("Valor:", offset_mesh);
            SERIAL_EM("\n");
            SERIAL_MV("  hola: \n", _lcd_level_bed_position);
            current_position[Z_AXIS] = offset_mesh;
            debounce_click = true; // ignore multiple "clicks" in a row
            mbl.set_zigzag_z(_lcd_level_bed_position++, offset_mesh + (offset_mesh_valor));
            if (_lcd_level_bed_position == (MESH_NUM_X_POINTS) * (MESH_NUM_Y_POINTS)) {
              lcd_goto_screen(_lcd_level_bed_done, true);

              current_position[Z_AXIS] = MESH_HOME_SEARCH_Z + MIN_Z_HEIGHT_FOR_HOMING;
              line_to_current(Z_AXIS);
              stepper.synchronize();

              mbl.set_has_mesh(true);
              enqueue_and_echo_commands_P(PSTR("G28"));
              lcd_return_to_status();
              //LCD_MESSAGEPGM(MSG_LEVEL_BED_DONE);
              #if HAS(BUZZER)
                buzz(200, 659);
                buzz(200, 698);
              #endif
            }
            else {
              lcd_goto_screen(_mash_auto_04, true);
            }
          }
        }
        else {
          debounce_click = false;
        }

        // Update on first display, then only on updates to Z position
        // Show message above on clicks instead
        if (lcdDrawUpdate) {
          float v = current_position[Z_AXIS] - MESH_HOME_SEARCH_Z;
          lcd_implementation_drawedit(PSTR(MSG_MOVE_Z), ftostr43sign(v + (v < 0 ? -0.0001 : 0.0001), '+'));
        }

      }
      */
      /**
       * Step 6: Display "Next point: 1 / 9" while waiting for move to finish
       */
       /*
      static void _mash_auto_05() {
        if (lcdDrawUpdate) {
          char msg[10];
          sprintf_P(msg, PSTR("%i / %u"), (int)(_lcd_level_bed_position + 1), (MESH_NUM_X_POINTS) * (MESH_NUM_Y_POINTS));
          lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_NEXT_POINT), msg);
        }

        lcdDrawUpdate =
          #if ENABLED(DOGLCD)
            LCDVIEW_CALL_REDRAW_NEXT
          #else
            LCDVIEW_CALL_NO_REDRAW
          #endif
        ;
      }
      */
      /**
       * Step 5: Initiate a move to the next point
       */
       /*
      static void _mash_auto_04() {
        // Set the menu to display ahead of blocking call
        lcd_goto_screen(_mash_auto_05);

        // _mbl_goto_xy runs the menu loop until the move is done
        int8_t px, py;
        mbl.zigzag(_lcd_level_bed_position, px, py);
        _mbl_goto_xy(mbl.get_probe_x(px), mbl.get_probe_y(py));

        // After the blocking function returns, change menus
        lcd_goto_screen(_mash_auto_06);
      }
      */
      /**
       * Step 4: Display "Click to Begin", wait for click
       *         Move to the first probe position
       */
       /*
      static void _mash_auto_03() {
        if (lcdDrawUpdate) lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_WAITING));
        if (axis_homed[X_AXIS] && axis_homed[Y_AXIS] && axis_homed[Z_AXIS]) {
          _lcd_level_bed_position = 0;
          current_position[Z_AXIS] = MESH_HOME_SEARCH_Z;
            #if Z_HOME_DIR > 0
              + Z_MAX_POS
            #endif
          ;
          planner.set_position_mm(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
          lcd_goto_screen(_mash_auto_04, true);
        }
      }
      */
      /**
       * Step 3: Display "Homing XYZ" - Wait for homing to finish
       */
       /*
      static void _mash_auto_02() {
        aux_de_tiempo2 = 0;
        aux_de_tiempo = 0;
        if (lcdDrawUpdate) lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_HOMING), NULL);
        lcdDrawUpdate =
          #if ENABLED(DOGLCD)
            LCDVIEW_CALL_REDRAW_NEXT
          #else
            LCDVIEW_CALL_NO_REDRAW
          #endif
        ;
        if (axis_homed[X_AXIS] && axis_homed[Y_AXIS] && axis_homed[Z_AXIS])
          lcd_goto_screen(_mash_auto_03);
      }
      */
      /**
       * Step 2: Continue Bed Leveling...
       */
       /*
      static void _mash_auto_01() {
        defer_return_to_status = true;
        axis_homed[X_AXIS] = axis_homed[Y_AXIS] = axis_homed[Z_AXIS] = false;
        mbl.reset();
        enqueue_and_echo_commands_P(PSTR("G28"));
        lcd_goto_screen(_mash_auto_02);
      }
      */
    //--------------------------------------------------------------------------
  /**
   *
   * "Prepare" > "Bed Leveling" handlers
   *
   */

  static uint8_t _lcd_level_bed_position;

  // Utility to go to the next mesh point
  // A raise is added between points if MIN_Z_HEIGHT_FOR_HOMING is in use
  // Note: During Manual Bed Leveling the homed Z position is MESH_HOME_SEARCH_Z
  // Z position will be restored with the final action, a G28
  inline void _mbl_goto_xy(float x, float y) {
    current_position[Z_AXIS] = MESH_HOME_SEARCH_Z + MIN_Z_HEIGHT_FOR_HOMING;
    //line_to_current(Z_AXIS);
    current_position[X_AXIS] = x + home_offset[X_AXIS];
    current_position[Y_AXIS] = y + home_offset[Y_AXIS];
    line_to_current(manual_feedrate[X_AXIS] <= manual_feedrate[Y_AXIS] ? X_AXIS : Y_AXIS);
    #if MIN_Z_HEIGHT_FOR_HOMING > 0
      current_position[Z_AXIS] = MESH_HOME_SEARCH_Z;
      //line_to_current(Z_AXIS);
    #endif
    stepper.synchronize();
  }

  static void _lcd_level_goto_next_point();

  static void _lcd_level_bed_done() {
    enqueue_and_echo_commands_P(PSTR("M500"));
    lcd_implementation_drawedit(PSTR(MSG_MENUS_FIN));
  }

  /**
   * Step 7: Get the Z coordinate, then goto next point or exit
   */
  static void _lcd_level_bed_get_z() {
    ENCODER_DIRECTION_NORMAL();
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);

    // Encoder wheel adjusts the Z position
    if (encoderPosition) {
      refresh_cmd_timeout();
      current_position[Z_AXIS] += float((int32_t)encoderPosition) * (MBL_Z_STEP);
      //NOLESS(current_position[Z_AXIS], -(LCD_PROBE_Z_RANGE) * 0.5);
      //NOMORE(current_position[Z_AXIS],  (LCD_PROBE_Z_RANGE) * 0.5);
      NOLESS(current_position[Z_AXIS], - 5);
      NOMORE(current_position[Z_AXIS], MESH_HOME_SEARCH_Z * 2);
      line_to_current(Z_AXIS);
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_REDRAW_NOW
        #endif
      ;
      encoderPosition = 0;
    }

    static bool debounce_click = false;
    if (LCD_CLICKED) {
      if (!debounce_click) {
        debounce_click = true; // ignore multiple "clicks" in a row
        mbl.set_zigzag_z(_lcd_level_bed_position++, current_position[Z_AXIS] - MESH_HOME_SEARCH_Z + 5);
        if (_lcd_level_bed_position == (MESH_NUM_X_POINTS) * (MESH_NUM_Y_POINTS)) {
          lcd_goto_screen(_lcd_level_bed_done, true);

          current_position[Z_AXIS] = MESH_HOME_SEARCH_Z + MIN_Z_HEIGHT_FOR_HOMING;
          line_to_current(Z_AXIS);
          stepper.synchronize();

          mbl.set_has_mesh(true);
          enqueue_and_echo_commands_P(PSTR("G28"));

          se_permiten_carteles = true;
          lcd_return_to_status();
          //LCD_MESSAGEPGM(MSG_LEVEL_BED_DONE);
          #if HAS(BUZZER)
            buzz(200, 659);
            buzz(200, 698);
          #endif
        }
        else {
          lcd_goto_screen(_lcd_level_goto_next_point, true);
        }
      }
    }
    else {
      debounce_click = false;
    }

    // Update on first display, then only on updates to Z position
    // Show message above on clicks instead
    if (lcdDrawUpdate) {
      float v = current_position[Z_AXIS] - MESH_HOME_SEARCH_Z + 5;
      lcd_implementation_drawedit(PSTR(MSG_MOVE_Z), ftostr43sign(v + (v < 0 ? -0.0001 : 0.0001), '+'));
    }

  }

  /**
   * Step 6: Display "Next point: 1 / 9" while waiting for move to finish
   */
  static void _lcd_level_bed_moving() {
    if (lcdDrawUpdate) {
      u8g.drawBox(0, 55, 128, 10);
      u8g.drawBox(0, 0, 128, 10);
      char msg[10];
      sprintf_P(msg, PSTR("%i / %u"), (int)(_lcd_level_bed_position + 1), (MESH_NUM_X_POINTS) * (MESH_NUM_Y_POINTS));
      lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_NEXT_POINT), msg);

    }

    lcdDrawUpdate =
      #if ENABLED(DOGLCD)
        LCDVIEW_CALL_REDRAW_NEXT
      #else
        LCDVIEW_CALL_NO_REDRAW
      #endif
    ;
  }

  /**
   * Step 5: Initiate a move to the next point
   */
  static void _lcd_level_goto_next_point() {
    // Set the menu to display ahead of blocking call
    lcd_goto_screen(_lcd_level_bed_moving);

    // _mbl_goto_xy runs the menu loop until the move is done
    int8_t px, py;
    mbl.zigzag(_lcd_level_bed_position, px, py);
    SERIAL_MV("px: ",px);
    SERIAL_MV("py: ",py);
    _mbl_goto_xy((mbl.get_probe_x(px)), mbl.get_probe_y(py));

    enqueue_and_echo_commands_P(PSTR("G1 Z0"));
    // After the blocking function returns, change menus
    lcd_goto_screen(_lcd_level_bed_get_z);
  }

  /**
   * Step 4: Display "Click to Begin", wait for click
   *         Move to the first probe position
   */
  static void _lcd_level_bed_homing_done() {
    if (lcdDrawUpdate){
      u8g.drawBox(0, 25, 128, 18);
      u8g.setPrintPos(10, 33);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_MENUS_AUTOLEVEL_07));
      u8g.setPrintPos(37, 42);
      lcd_printPGM(PSTR(MSG_CONTINUAR));
      //u8g.setPrintPos(16, 44);
      u8g.setColorIndex(1);
      //lcd_printPGM(PSTR(MSG_ESPERA));
    } //lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_WAITING));
    if (LCD_CLICKED) {
      _lcd_level_bed_position = 0;
      current_position[Z_AXIS] = MESH_HOME_SEARCH_Z;
        #if Z_HOME_DIR > 0
          + Z_MAX_POS
        #endif
      ;
      planner.set_position_mm(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
      lcd_goto_screen(_lcd_level_goto_next_point, true);
    }
  }

  /**
   * Step 3: Display "Homing XYZ" - Wait for homing to finish
   */
  static void _lcd_level_bed_homing() {
    if (lcdDrawUpdate){
      u8g.drawBox(0, 0, 128, 18);
      u8g.setPrintPos(10, 12);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALIBRAR_HOME));
      //u8g.setPrintPos(28, 17);
      //lcd_printPGM(PSTR(MSG_MENSAJE_04));
      u8g.setPrintPos(16, 44);
      u8g.setColorIndex(1);
      lcd_printPGM(PSTR(MSG_ESPERA));
    } //lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_HOMING), NULL);
    //orlando
    lcdDrawUpdate =
      #if ENABLED(DOGLCD)
        LCDVIEW_CALL_REDRAW_NEXT
      #else
        LCDVIEW_CALL_NO_REDRAW
      #endif
    ;
    if (axis_homed[X_AXIS] && axis_homed[Y_AXIS] && axis_homed[Z_AXIS])
      lcd_goto_screen(_lcd_level_bed_homing_done);
  }

  /**
   * Step 2: Continue Bed Leveling...
   */
  static void _lcd_level_bed_continue() {
    defer_return_to_status = true;
    axis_homed[X_AXIS] = axis_homed[Y_AXIS] = axis_homed[Z_AXIS] = false;
    mbl.reset();
    enqueue_and_echo_commands_P(PSTR("G28"));
    lcd_goto_screen(_lcd_level_bed_homing);
  }

  /**
   * Step 1: MBL entry-point: "Cancel" or "Level Bed"
   */
  static void lcd_level_bed() {
    START_MENU();
    MENU_ITEM(back, MSG_LEVEL_BED_CANCEL);
    MENU_ITEM(submenu, MSG_LEVEL_BED, _lcd_level_bed_continue);
    END_MENU();
  }


    //--------------------------------------------------------------------------
    //AUTO Calibrasion por orlando de kuttercraft
    //--------------------------------------------------------------------------

    static void _lcd_inicio_autolevel_03();

    //--------------------------------------------------------------------------
    //permite cambiar el valor del offset este se guarda al terminar
    //--------------------------------------------------------------------------
    float posicion_z;
    void menu_mesh();
    void ajustar_offset_manual_g77(/* arguments */) {
      //float aux_offset = g77_offset;

      ENCODER_DIRECTION_NORMAL();

      // Encoder wheel adjusts the Z position
      if (encoderPosition) {
        refresh_cmd_timeout();

        posicion_z += float((int32_t)encoderPosition) * (MBL_Z_STEP);
        NOLESS(posicion_z, - 5);
        NOMORE(posicion_z, MESH_HOME_SEARCH_Z * 2);
        //line_to_current(Z_AXIS);
        lcdDrawUpdate =
          #if ENABLED(DOGLCD)
            LCDVIEW_CALL_REDRAW_NEXT
          #else
            LCDVIEW_REDRAW_NOW
          #endif
        ;
        encoderPosition = 0;
      }
      if(LCD_CLICKED){
        g77_offset = posicion_z;
        //aux_offset = g77_offset - aux_offset;
        //continuar orlando=

        defer_return_to_status = false;

        mbl.set_zigzag_z(1 - 1,(mbl.z_values[1-1][1-1] + (g77_offset)));
        mbl.set_zigzag_z(2 - 1,(mbl.z_values[1-1][2-1] + (g77_offset)));
        mbl.set_zigzag_z(3 - 1,(mbl.z_values[1-1][3-1] + (g77_offset)));
        mbl.set_zigzag_z(4 - 1,(mbl.z_values[2-1][3-1] + (g77_offset)));
        mbl.set_zigzag_z(5 - 1,(mbl.z_values[2-1][2-1] + (g77_offset)));
        mbl.set_zigzag_z(6 - 1,(mbl.z_values[2-1][1-1] + (g77_offset)));
        mbl.set_zigzag_z(7 - 1,(mbl.z_values[3-1][1-1] + (g77_offset)));
        mbl.set_zigzag_z(8 - 1,(mbl.z_values[3-1][2-1] + (g77_offset)));
        mbl.set_zigzag_z(9 - 1,(mbl.z_values[3-1][3-1] + (g77_offset)));

        enqueue_and_echo_commands_P(PSTR("M500"));
        //lcd_return_to_status();
        lcd_goto_screen(menu_mesh, true, 2);
      }

      if (lcdDrawUpdate) {
        float v = posicion_z - MESH_HOME_SEARCH_Z + 5;
        lcd_implementation_drawedit(PSTR(MSG_MOVE_Z_OFFSET), ftostr43sign(v + (v < 0 ? -0.0001 : 0.0001), '+'));
      }
    }

    //--------------------------------------------------------------------------
    //mueve el estrusor a la posicion de calibrado
    //y la artura cambia con el movimieno
    //--------------------------------------------------------------------------
    static void _lcd_inicio_autolevel_00();
    void salida_calibracion_offset(){
      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;

      if((commands_in_queue == 0)){
        //enqueue_and_echo_commands_P(PSTR("G1 Z0"));
        lcd_goto_screen(_lcd_inicio_autolevel_00);
      }
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_CALL_NO_REDRAW
        #endif
      ;
    }

    void ajustar_offset_g77(/* arguments */) {
      u8g.drawBox(0, 55, 128, 10);
      u8g.drawBox(0, 0, 128, 10);

      g77_offset = 0;
      float aux_offset = g77_offset;

      ENCODER_DIRECTION_NORMAL();

      // Encoder wheel adjusts the Z position
      if (encoderPosition) {
        refresh_cmd_timeout();
        current_position[Z_AXIS] += float((int32_t)encoderPosition) * (MBL_Z_STEP);
        NOLESS(current_position[Z_AXIS], - 5);
        NOMORE(current_position[Z_AXIS], MESH_HOME_SEARCH_Z * 2);
        line_to_current(Z_AXIS);
        lcdDrawUpdate =
          #if ENABLED(DOGLCD)
            LCDVIEW_CALL_REDRAW_NEXT
          #else
            LCDVIEW_REDRAW_NOW
          #endif
        ;
        encoderPosition = 0;
      }
      if(LCD_CLICKED){
        g77_offset = current_position[Z_AXIS];
        aux_offset = g77_offset - aux_offset;
        //continuar orlando=

        defer_return_to_status = false;

        mbl.set_zigzag_z(1 - 1,(mbl.z_values[1-1][1-1] + (aux_offset)));
        mbl.set_zigzag_z(2 - 1,(mbl.z_values[1-1][2-1] + (aux_offset)));
        mbl.set_zigzag_z(3 - 1,(mbl.z_values[1-1][3-1] + (aux_offset)));
        mbl.set_zigzag_z(4 - 1,(mbl.z_values[2-1][3-1] + (aux_offset)));
        mbl.set_zigzag_z(5 - 1,(mbl.z_values[2-1][2-1] + (aux_offset)));
        mbl.set_zigzag_z(6 - 1,(mbl.z_values[2-1][1-1] + (aux_offset)));
        mbl.set_zigzag_z(7 - 1,(mbl.z_values[3-1][1-1] + (aux_offset)));
        mbl.set_zigzag_z(8 - 1,(mbl.z_values[3-1][2-1] + (aux_offset)));
        mbl.set_zigzag_z(9 - 1,(mbl.z_values[3-1][3-1] + (aux_offset)));

        enqueue_and_echo_commands_P(PSTR("M420 S1\nM500"));
        //ir al automatico
        //lcd_return_to_status();
        lcd_goto_screen(salida_calibracion_offset);
      }

      if (lcdDrawUpdate) {
        float v = current_position[Z_AXIS] - MESH_HOME_SEARCH_Z + 5;
        lcd_implementation_drawedit(PSTR(MSG_MOVE_Z), ftostr43sign(v + (v < 0 ? -0.0001 : 0.0001), '+'));
      }
    }
    void acomodar_para_calibrar_offset_1(/* arguments */) {

      //if (lcdDrawUpdate) lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_HOMING), NULL);
      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;

      //SERIAL_EM("Espera 02");
      if((commands_in_queue == 0)){
        //SERIAL_EM("Fin");
        enqueue_and_echo_commands_P(PSTR("G1 Z0"));
        lcd_goto_screen(ajustar_offset_g77);
      }
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_CALL_NO_REDRAW
        #endif
      ;

    }


    void acomodar_para_calibrar_offset(/* arguments */) {

      //if (lcdDrawUpdate) lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_HOMING), NULL);
      u8g.drawBox(0, 0, 128, 18);
      u8g.setPrintPos(10, 12);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALIBRAR_HOME));
      //u8g.setPrintPos(28, 17);
      //lcd_printPGM(PSTR(MSG_MENSAJE_04));
      u8g.setPrintPos(16, 44);
      u8g.setColorIndex(1);
      lcd_printPGM(PSTR(MSG_ESPERA));

      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;

      //SERIAL_EM("Espera 01");
      if((commands_in_queue == 0)){
        //SERIAL_EM("Fin");
        enqueue_and_echo_commands_P(PSTR("G91\nG1 X44 Y-41 F3000\nG90"));
        lcd_goto_screen(acomodar_para_calibrar_offset_1);
      }
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_CALL_NO_REDRAW
        #endif
      ;

    }
    void preparar_offset_manual(/* arguments */) {
      /* code */
      mbl.set_zigzag_z(1 - 1,(mbl.z_values[1-1][1-1] - (g77_offset)));
      mbl.set_zigzag_z(2 - 1,(mbl.z_values[1-1][2-1] - (g77_offset)));
      mbl.set_zigzag_z(3 - 1,(mbl.z_values[1-1][3-1] - (g77_offset)));
      mbl.set_zigzag_z(4 - 1,(mbl.z_values[2-1][3-1] - (g77_offset)));
      mbl.set_zigzag_z(5 - 1,(mbl.z_values[2-1][2-1] - (g77_offset)));
      mbl.set_zigzag_z(6 - 1,(mbl.z_values[2-1][1-1] - (g77_offset)));
      mbl.set_zigzag_z(7 - 1,(mbl.z_values[3-1][1-1] - (g77_offset)));
      mbl.set_zigzag_z(8 - 1,(mbl.z_values[3-1][2-1] - (g77_offset)));
      mbl.set_zigzag_z(9 - 1,(mbl.z_values[3-1][3-1] - (g77_offset)));

      posicion_z = g77_offset;

      lcd_goto_screen(ajustar_offset_manual_g77);
    }
    void preparar_offset(/* arguments */) {
      //no se conose la posicion de los ejes
      axis_homed[X_AXIS] = axis_homed[Y_AXIS] = axis_homed[Z_AXIS] = false;
      //borra el mapa
      mbl.reset();
      //cambia el offset a 0
      g77_offset = 0;
      //no hay salida por tiempo
      defer_return_to_status = true;

      //realizar home
      enqueue_and_echo_commands_P(PSTR("G28"));
      //enqueue_and_echo_commands_P(PSTR(""));
      lcd_goto_screen(acomodar_para_calibrar_offset);
    }
    //
    static float redondeo_cama(float valor_neto){
      //.025
      //.050
      //.075
      //.100 mas alto
      int valor_entero = (int) (valor_neto * 10.0);
      float valor_decimal = (valor_neto * 10.0) - valor_entero;

      float valor_redondeo = 0.0;

      // se decide que numero es el mas cercano
      if(valor_decimal <= -0.75){
        valor_redondeo = -0.75;
      }else if(valor_decimal <= -0.50){
        valor_redondeo = -0.50;
      }else if(valor_decimal <= -0.25){
        valor_redondeo = -0.25;
      }else if(valor_decimal <= 0.00){
        valor_redondeo = 0.00;
      }else if(valor_decimal <= 0.25){
        valor_redondeo = 0.25;
      }else if(valor_decimal <= 0.50){
        valor_redondeo = 0.50;
      }else if(valor_decimal <= 0.75){
        valor_redondeo = 0.75;
      }else if(valor_decimal == 1.0){
        valor_redondeo = 0.0;
      }else{
        valor_redondeo = 1.0;
      }
      //se vuelve a reconstruir el numero
      return valor_decimal = (valor_entero / 10.0) + (valor_redondeo / 10.0);

    }

    static void _lcd_inicio_autolevel_04() {//_lcd_level_bed_get_z
      u8g.drawBox(0, 25, 128, 18);
      u8g.setColorIndex(0);
      u8g.setPrintPos(25, 37);
      lcd_printPGM(PSTR(MSG_MIDIENDO));
      u8g.setColorIndex(1);

      SERIAL_EM("Tarea 04");
        lcdDrawUpdate =
          #if ENABLED(DOGLCD)
            LCDVIEW_CALL_REDRAW_NEXT
          #else
            LCDVIEW_REDRAW_NOW
          #endif
        ;

      if (g77_finalizo) {


          mbl.set_zigzag_z(_lcd_level_bed_position++, redondeo_cama(g77_valor_del_z) + g77_offset);
          SERIAL_MV("Valor del z:", redondeo_cama(g77_valor_del_z));
          SERIAL_EM("");

          //salida
          if (_lcd_level_bed_position == (MESH_NUM_X_POINTS) * (MESH_NUM_Y_POINTS)) {
            lcd_goto_screen(_lcd_level_bed_done, true);

            current_position[Z_AXIS] = MESH_HOME_SEARCH_Z + MIN_Z_HEIGHT_FOR_HOMING;
            line_to_current(Z_AXIS);
            stepper.synchronize();

            //SERIAL_EM("Tarea 04 final");

            mbl.set_has_mesh(true);
            enqueue_and_echo_commands_P(PSTR("G28"));
            se_permiten_carteles = true;
            //SERIAL_EM("se_permiten_carteles = false;");
            lcd_return_to_status();

            #if HAS(BUZZER)
              buzz(200, 659);
              buzz(200, 698);
            #endif
          }else{
            //SERIAL_EM("Tarea 04 Rutina");
            lcd_goto_screen(_lcd_inicio_autolevel_03, true);
          }
      }

    }

    static void _lcd_inicio_autolevel_03() {
      g77_finalizo = false;

      SERIAL_EM("Tarea 03 moving");
      lcd_goto_screen(_lcd_level_bed_moving);


      int8_t px, py;
      mbl.zigzag(_lcd_level_bed_position, px, py);
      SERIAL_MV("px: ",px);
      SERIAL_MV("py", py);
      SERIAL_EM("--------------------------------");
      _mbl_goto_xy(mbl.get_probe_x(px), mbl.get_probe_y(py));


      SERIAL_EM("Tarea 03 valor de z");
      enqueue_and_echo_commands_P(PSTR("G77"));
      lcd_goto_screen(_lcd_inicio_autolevel_04);
    }


    static void _lcd_inicio_autolevel_02() {
      /*
      if (lcdDrawUpdate){
        u8g.drawBox(0, 25, 128, 18);
        u8g.setPrintPos(10, 33);
        u8g.setColorIndex(0);
        lcd_printPGM(PSTR(MSG_MENUS_AUTOLEVEL_07));
        u8g.setPrintPos(37, 42);
        lcd_printPGM(PSTR(MSG_CONTINUAR));
        //u8g.setPrintPos(16, 44);
        u8g.setColorIndex(1);
        //lcd_printPGM(PSTR(MSG_ESPERA));
      }
      */
      if (commands_in_queue == 0) {
        _lcd_level_bed_position = 0;
        current_position[Z_AXIS] = MESH_HOME_SEARCH_Z;
          #if Z_HOME_DIR > 0
            + Z_MAX_POS
          #endif
        ;
        planner.set_position_mm(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
        SERIAL_EM("Tarea 02");
        lcd_goto_screen(_lcd_inicio_autolevel_03, true);
      }
    }

    static void _lcd_inicio_autolevel_01() { //_lcd_level_bed_homing
      //se realiza actualizaciones de pantalla
      if (lcdDrawUpdate){
        u8g.drawBox(0, 0, 128, 18);
        u8g.setPrintPos(10, 12);
        u8g.setColorIndex(0);
        lcd_printPGM(PSTR(MSG_CALIBRAR_HOME));
        //u8g.setPrintPos(28, 17);
        //lcd_printPGM(PSTR(MSG_MENSAJE_04));
        u8g.setPrintPos(16, 44);
        u8g.setColorIndex(1);
        lcd_printPGM(PSTR(MSG_ESPERA));
      }

      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;

      if((commands_in_queue == 0)){

        lcd_goto_screen(_lcd_inicio_autolevel_02);
      }

      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_CALL_NO_REDRAW
        #endif
      ;

    }

    static void _lcd_inicio_autolevel_00() {//_lcd_level_bed_continue
      //no hay salida por tiempo
      defer_return_to_status = true;
      //no se conose la posicion de los ejes
      axis_homed[X_AXIS] = axis_homed[Y_AXIS] = axis_homed[Z_AXIS] = false;
      //se borra la matris
      mbl.reset();
      //se realiza un home
      enqueue_and_echo_commands_P(PSTR("G28"));
      SERIAL_EM("Tarea 00");
      //salida a la rutina 01
      lcd_goto_screen(_lcd_inicio_autolevel_01);
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //Ajusta el Offset mientras se imprime
    //inicio
    //--------------------------------------------------------------------------
    void menu_ajustar_offset_print();
    void ajustar_offset_print_01(/* arguments */) {
      float aux_offset = g77_offset;

      ENCODER_DIRECTION_NORMAL();

      // Encoder wheel adjusts the Z position
      if (encoderPosition) {
        refresh_cmd_timeout();

        posicion_z += float((int32_t)encoderPosition) * (MBL_Z_STEP);
        NOLESS(posicion_z, - 5);
        NOMORE(posicion_z, MESH_HOME_SEARCH_Z * 2);
        //line_to_current(Z_AXIS);
        lcdDrawUpdate =
          #if ENABLED(DOGLCD)
            LCDVIEW_CALL_REDRAW_NEXT
          #else
            LCDVIEW_REDRAW_NOW
          #endif
        ;
        encoderPosition = 0;
      }
      if(LCD_CLICKED){
        g77_offset = posicion_z;
        //aux_offset = g77_offset - aux_offset;
        //continuar orlando=

        defer_return_to_status = false;

        mbl.set_zigzag_z(1 - 1,(mbl.z_values[1-1][1-1] - (aux_offset)));
        mbl.set_zigzag_z(2 - 1,(mbl.z_values[1-1][2-1] - (aux_offset)));
        mbl.set_zigzag_z(3 - 1,(mbl.z_values[1-1][3-1] - (aux_offset)));
        mbl.set_zigzag_z(4 - 1,(mbl.z_values[2-1][3-1] - (aux_offset)));
        mbl.set_zigzag_z(5 - 1,(mbl.z_values[2-1][2-1] - (aux_offset)));
        mbl.set_zigzag_z(6 - 1,(mbl.z_values[2-1][1-1] - (aux_offset)));
        mbl.set_zigzag_z(7 - 1,(mbl.z_values[3-1][1-1] - (aux_offset)));
        mbl.set_zigzag_z(8 - 1,(mbl.z_values[3-1][2-1] - (aux_offset)));
        mbl.set_zigzag_z(9 - 1,(mbl.z_values[3-1][3-1] - (aux_offset)));

        mbl.set_zigzag_z(1 - 1,(mbl.z_values[1-1][1-1] + (g77_offset)));
        mbl.set_zigzag_z(2 - 1,(mbl.z_values[1-1][2-1] + (g77_offset)));
        mbl.set_zigzag_z(3 - 1,(mbl.z_values[1-1][3-1] + (g77_offset)));
        mbl.set_zigzag_z(4 - 1,(mbl.z_values[2-1][3-1] + (g77_offset)));
        mbl.set_zigzag_z(5 - 1,(mbl.z_values[2-1][2-1] + (g77_offset)));
        mbl.set_zigzag_z(6 - 1,(mbl.z_values[2-1][1-1] + (g77_offset)));
        mbl.set_zigzag_z(7 - 1,(mbl.z_values[3-1][1-1] + (g77_offset)));
        mbl.set_zigzag_z(8 - 1,(mbl.z_values[3-1][2-1] + (g77_offset)));
        mbl.set_zigzag_z(9 - 1,(mbl.z_values[3-1][3-1] + (g77_offset)));

        enqueue_and_echo_commands_P(PSTR("M500"));
        //lcd_return_to_status();
        lcd_goto_screen(menu_ajustar_offset_print, true, 2);
      }

      if (lcdDrawUpdate) {
        float v = posicion_z - MESH_HOME_SEARCH_Z + 5;
        lcd_implementation_drawedit(PSTR(MSG_MOVE_Z), ftostr43sign(v + (v < 0 ? -0.0001 : 0.0001), '+'));
      }
    }
    void ajustar_offset_print(/* arguments */) {
      posicion_z = g77_offset;
      lcd_goto_screen(ajustar_offset_print_01);
    }
    //boton para modificar el offset
    /*
    void boton_ajustar_offset_print(int valor) {
      if (valor == 2){
        u8g.setPrintPos(25, 63);
        u8g.setColorIndex(0);
        lcd_printPGM(PSTR(MSG_CALIBRAR_BASE));// calibrar cama
        u8g.setColorIndex(1);
        u8g.drawBitmapP(89,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, mesh_cont_b);
        if (LCD_CLICKED) {
          lcd_goto_screen(ajustar_offset_print, true);
        }
      }else{
        u8g.drawBitmapP(89,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, mesh_cont_a);
      }
    }
    */

    //nuevo menu de la 3.7 Alinear eje z
    void menu_calibracion_mecanica();
    void tarea_alinear_eje_z_04(){
      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
      defer_return_to_status = true;

      if((commands_in_queue == 0)){
        soft_endstop_max[Z_AXIS] = Z_MAX_POS;
        defer_return_to_status = false;
        enqueue_and_echo_commands_P(PSTR("G91\nG1 Z-50 F2000\nG28\nM117 Eje Z Alineado"));
        if(autolevel_on_off){
          lcd_goto_screen(menu_mesh, true, 6);
        }else{
          lcd_goto_screen(menu_calibracion_mecanica, true, 2);
        }
      }

      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_CALL_NO_REDRAW
        #endif
      ;
    }

    void tarea_alinear_eje_z_03(){

      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
      defer_return_to_status = true;
      //visual
      u8g.setColorIndex(1);
      u8g.drawBox(0, 55, 128, 10);
      u8g.setPrintPos(16, 31);
      lcd_printPGM(PSTR(MSG_ESPERA));
      u8g.setPrintPos(25, 40);
      lcd_printPGM(PSTR(MSG_MIDIENDO));
      u8g.drawBox(0, 0, 128, 10);

      // destination[Z_AXIS] += 5;
      // planner.buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], 1, 0, 0);
      // stepper.synchronize();

      enqueue_and_echo_commands_P(PSTR("G91\nG1 Z1 F2000\nG90"));

      if(current_position[Z_AXIS] == (Z_MAX_POS + 80)){
        lcd_goto_screen(tarea_alinear_eje_z_04, true);
      }
      //permite lcd Draw Update
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_CALL_NO_REDRAW
        #endif
      ;
    }

    void tarea_alinear_eje_z_02(/* arguments */) {
      //permite lcd Draw Update
      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
      defer_return_to_status = true;
      //visual
      u8g.setColorIndex(1);
      u8g.drawBox(0, 55, 128, 10);
      u8g.setPrintPos(16, 31);
      lcd_printPGM(PSTR(MSG_ESPERA));
      u8g.setPrintPos(25, 40);
      lcd_printPGM(PSTR(MSG_MIDIENDO));
      u8g.drawBox(0, 0, 128, 10);
      //funcion
      if(current_position[Z_AXIS] == 40 || current_position[Z_AXIS] == 45){
        soft_endstop_max[Z_AXIS] = (Z_MAX_POS + 80);
        current_position[Z_AXIS] = 0;
        lcd_goto_screen(tarea_alinear_eje_z_03, true);
      }

      //permite lcd Draw Update
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_CALL_NO_REDRAW
        #endif
      ;
    }
    void tarea_alinear_eje_z_01() {
      //inicio de rutina
      enqueue_and_echo_commands_P(PSTR("G28\nG91\nG1 Z40 X40 F4000\nG90"));
      lcd_goto_screen(tarea_alinear_eje_z_02, true, 6);
    }

    void alinear_eje_z(/* arguments */) {
      #if(PK3_EXTEN || PK3_PLUS_PLUS || SI_ES_UNA_PK3)
        u8g.setColorIndex(1);
        u8g.drawBox(0, 55, 128, 10);
        u8g.setPrintPos(3, 23);
        lcd_printPGM(PSTR(MSG_ALINIACION_Z_ADV_01));
        u8g.setPrintPos(3, 31);
        lcd_printPGM(PSTR(MSG_ALINIACION_Z_ADV_02));
        u8g.setPrintPos(13, 40);
        lcd_printPGM(PSTR(MSG_ALINIACION_Z_ADV_03));
        u8g.setPrintPos(3, 49);
        lcd_printPGM(PSTR(MSG_ALINIACION_Z_ADV_04));
        u8g.drawBox(0, 0, 128, 10);
        if (LCD_CLICKED) {
           lcd_goto_screen(tarea_alinear_eje_z_01, true);
        }
      #elif(PK2_PLUS_PLUS || PK2 || PK1 || ESPESIAL)
        u8g.setColorIndex(1);
        u8g.drawBox(0, 55, 128, 10);
        u8g.setPrintPos(7, 31);
        lcd_printPGM(PSTR(MSG_ALINIACION_Z_ERROR_01));
        u8g.setPrintPos(3, 40);
        lcd_printPGM(PSTR(MSG_ALINIACION_Z_ERROR_02));
        u8g.drawBox(0, 0, 128, 10);
        if (LCD_CLICKED) {
           lcd_goto_screen(menu_mesh, true, 6);
        }
      #endif
    }

    ////////////////////////////////////////////////////////////////////////////
    void cartel_error_1(){
      //error de cama maximo o minimo
      defer_return_to_status = true;

      u8g.setColorIndex(1);
      u8g.drawBox(0, 55, 128, 10);

      u8g.setPrintPos(7, 25);
      lcd_printPGM(PSTR(MSG_ADV_T1_CAMA));//"Advertencia T0 CAMA"

      u8g.setPrintPos(10, 34);
      lcd_printPGM(PSTR(MSG_TEM_ERROR_02));//"Proteccion Termica"

      u8g.setPrintPos(15, 43);
      lcd_printPGM(PSTR(MSG_TEM_ERROR_09));//"Maximo o Minimo"

      u8g.drawBox(0, 0, 128, 10);
      u8g.setColorIndex(0);
      u8g.setPrintPos(3, 63);
      lcd_printPGM(PSTR(MSG_WEB"/blog"));

      if (LCD_CLICKED) {
        defer_return_to_status = false;
        lcd_goto_screen(Kuttercraft_menu_start, false);
      }

    }

    void cartel_error_2(){
      //error de hotend maximo o minimo
      defer_return_to_status = true;

      u8g.setColorIndex(1);
      u8g.drawBox(0, 55, 128, 10);

      u8g.setPrintPos(1, 25);
      lcd_printPGM(PSTR(MSG_ADV_T0_HOTEND));//"Advertencia T0 Hotend"

      u8g.setPrintPos(10, 34);
      lcd_printPGM(PSTR(MSG_TEM_ERROR_02));//"Proteccion Termica"

      u8g.setPrintPos(19, 43);
      lcd_printPGM(PSTR(MSG_TEM_ERROR_09));//"Maximo o Minimo"

      u8g.drawBox(0, 0, 128, 10);
      u8g.setColorIndex(0);
      u8g.setPrintPos(3, 63);
      lcd_printPGM(PSTR(MSG_WEB"/blog"));

      if (LCD_CLICKED) {
        defer_return_to_status = false;
        lcd_goto_screen(Kuttercraft_menu_start, false);
      }

    }

    void cartel_error_3(){
      //error de hotend variacion muy alta
      defer_return_to_status = true;

      u8g.setColorIndex(1);
      u8g.drawBox(0, 55, 128, 10);

      u8g.setPrintPos(1, 25);
      lcd_printPGM(PSTR(MSG_ADV_T0_HOTEND));//"Advertencia T0 Hotend"

      u8g.setPrintPos(10, 34);
      lcd_printPGM(PSTR(MSG_TEM_ERROR_02));//"Proteccion Termica"

      u8g.setPrintPos(7, 43);
      lcd_printPGM(PSTR(MSG_TEM_ERROR_07));//Variacion detectada

      u8g.drawBox(0, 0, 128, 10);
      u8g.setColorIndex(0);
      u8g.setPrintPos(3, 63);
      lcd_printPGM(PSTR(MSG_WEB"/blog"));

      if (LCD_CLICKED) {
         defer_return_to_status = false;
        lcd_goto_screen(Kuttercraft_menu_start, false);
      }

    }
    void cartel_error_4(){
      //error de hotend caida o no subida

      defer_return_to_status = true;

      u8g.setColorIndex(1);
      u8g.drawBox(0, 55, 128, 10);

      u8g.setPrintPos(1, 25);
      lcd_printPGM(PSTR(MSG_ADV_T0_HOTEND));//"Advertencia T0 Hotend"

      u8g.setPrintPos(10, 34);
      lcd_printPGM(PSTR(MSG_TEM_ERROR_02));//"Proteccion Termica"

      u8g.setPrintPos(3, 43);
      lcd_printPGM(PSTR(MSG_TEM_ERROR_08));//no hay calentamiento

      u8g.drawBox(0, 0, 128, 10);
      u8g.setColorIndex(0);
      u8g.setPrintPos(3, 63);
      lcd_printPGM(PSTR(MSG_WEB"/blog"));

      if (LCD_CLICKED) {
         defer_return_to_status = false;
        lcd_goto_screen(Kuttercraft_menu_start, false);
      }

    }

    void cartel_error_mgs(int numero_error) {
      switch (numero_error) {
        case 0:
          break;

        case 1:
          if(print_job_counter.imprimiendo_estado){
            gcode_M25();
          }

          disable_all_heaters(); // switch off all heaters.
          disable_all_coolers(); // switch off all coolers.
          lcd_setstatus(MSG_TEM_ERROR_09);

          if(se_permiten_carteles){
            lcd_goto_screen(cartel_error_1); //error de cama maximo o minimo
          }

          break;
        case 2:
          if(print_job_counter.imprimiendo_estado){
            gcode_M25();
          }

          disable_all_heaters(); // switch off all heaters.
          disable_all_coolers(); // switch off all coolers.
          lcd_setstatus(MSG_TEM_ERROR_09);

          if(se_permiten_carteles){
            lcd_goto_screen(cartel_error_2); //error de hotend maximo o minimo
          }

          break;
        case 3:
          if(print_job_counter.imprimiendo_estado){
            gcode_M25();
          }
          lcd_setstatus(MSG_TEM_ERROR_07);
          disable_all_heaters(); // switch off all heaters.
          disable_all_coolers(); // switch off all coolers.

          if(se_permiten_carteles){
            lcd_goto_screen(cartel_error_3); //error de hotend variacion muy alta
          }

          break;
        case 4:
          if(print_job_counter.imprimiendo_estado){
            gcode_M25();
          }
          lcd_setstatus(MSG_TEM_ERROR_08);
          disable_all_heaters(); // switch off all heaters.
          disable_all_coolers(); // switch off all coolers.

          if(se_permiten_carteles){
            lcd_goto_screen(cartel_error_4); //error de hotend caida o no subida
          }

          break;
      }
    }


    ////////////////////////////////////////////////////////////////////////////

    //static void boton_mesh_ajus(int valor);
    //nuevo menu de la 3.7 ajuste de offset
    void boton_ajustar_offset_print(int valor) {
      if (valor == 2){
        u8g.setPrintPos(16, 63);
        u8g.setColorIndex(0);

        lcd_printPGM(PSTR(MSG_MENUS_AUTOLEVEL_02));

        u8g.setColorIndex(1);

        u8g.drawBitmapP(80,20,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, calibrar_offset_b);
        if (LCD_CLICKED) {
          lcd_goto_screen(ajustar_offset_print, true);
        }
      }else{
        u8g.drawBitmapP(80,20,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, calibrar_offset_a);
      }
    }
    void matris_level();
    void boton_ajustar_mapa_cama_print(int valor) {
      if (valor == 1){
        u8g.setPrintPos(28, 63);
        u8g.setColorIndex(0);

        lcd_printPGM(PSTR(MSG_CALIBRAR_MAPA_));

        u8g.setColorIndex(1);

        u8g.drawBitmapP(53,20,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, mapa_de_cama_b);
        if (LCD_CLICKED) {
          //lcd_goto_screen(ajustar_offset_print, true);
          lcd_goto_screen(matris_level, true);
        }
      }else{
        u8g.drawBitmapP(53,20,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, mapa_de_cama_a);
      }
    }
    void Kuttercraft_menu_print();
    void boton_ajustar_volver_print(int valor) {
      if (valor == 0){
        u8g.setPrintPos(46, 63);
        u8g.setColorIndex(0);

        lcd_printPGM(PSTR(MSG_VOLVER));

        u8g.setColorIndex(1);

        u8g.drawBitmapP(28,20,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, boton_volver_b);
        if (LCD_CLICKED) {
          //lcd_goto_screen(ajustar_offset_print, true);
            lcd_goto_screen(Kuttercraft_menu_print, true, 2);
        }
      }else{
        u8g.drawBitmapP(26,20,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, boton_volver_a);
      }
    }

    void menu_ajustar_offset_print(){
      u8g.drawBox(0, 55, 128, 10);
      u8g.drawBox(0, 0, 128, 10);

      u8g.setPrintPos(19, 8);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_MENUS_AUTOLEVEL_00));//15
      u8g.setColorIndex(1);

      if (encoderPosition > 1 && encoderPosition < 50){
        encoderPosition = 2;
      }
      if (encoderPosition > 50){
        encoderPosition = 0;
      }
      posicion_anterior = 46;
      boton_ajustar_volver_print(encoderPosition);
      boton_ajustar_mapa_cama_print(encoderPosition);
      boton_ajustar_offset_print(encoderPosition);
    }
    /*
    void menu_ajustar_offset_print() {
      u8g.drawBox(0, 55, 128, 10);
      u8g.drawBox(0, 0, 128, 10);

      u8g.setPrintPos(19, 8);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_MENUS_AUTOLEVEL_00));//15
      u8g.setColorIndex(1);

      if (encoderPosition > 1 && encoderPosition < 50){
        encoderPosition = 2;
      }
      if (encoderPosition > 50){
        encoderPosition = 0;
      }
      posicion_anterior = 46;
      volver_boton_grande(encoderPosition, 9, auxiliar_bot_grande);
      //boton_mesh_ajus(encoderPosition);
      boton_ajustar_offset_print(encoderPosition);
    }
    */
    //--------------------------------------------------------------------------
    //Ajusta el Offset mientras se imprime
    //Fin
    //--------------------------------------------------------------------------



  #endif  // MANUAL_BED_LEVELING

  //--
  //
  //---------------------
  //botones de mover eje
  void boton_chico(int x, int y, int numero, int valor, int x_text, const char* name, const char* desactivo, const char* activoA, screenFunc_t ir_a) {
    if (valor == numero){
      u8g.setPrintPos(x_text, 63);
      u8g.setColorIndex(0);
      lcd_print(name);
      u8g.setColorIndex(1);
      u8g.drawBitmapP(x,y,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, activoA);
      if (LCD_CLICKED) {
        //lcd_save_previous_menu();
        lcd_goto_screen(ir_a, true);
      }
    }else{
      u8g.drawBitmapP(x,y,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, desactivo);
    }
  }
  //crea botones grandes
  void boton_grande(int x, int y, int numero, int valor, int x_text, const char* name, const char* desactivo, const char* activoA, screenFunc_t ir_a) {
    if (valor == numero){
      u8g.setPrintPos(x_text, 63);
      u8g.setColorIndex(0);
      //lcd_printPGM(name);
      lcd_print(name);
      //lcd_printPGM(PSTR(name));
      u8g.setColorIndex(1);
      u8g.drawBitmapP(x,y,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, activoA);
      if (LCD_CLICKED) {
        //lcd_save_previous_menu();
        lcd_goto_screen(ir_a, true);
      }
    }else{
      u8g.drawBitmapP(x,y,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, desactivo);
    }
  }
  void eje_volver_boton(int valor,int x,screenFunc_t ir_a) {
    if (valor == 0){
      u8g.setPrintPos(45, 63);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_VOLVER));
      u8g.setColorIndex(1);
      u8g.drawBitmapP(x,23,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_volver_b);
      if (LCD_CLICKED) {
        lcd_goto_screen(ir_a, true);
      }
    }else{
      u8g.drawBitmapP(x,23,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_volver_a);
    }
  }
  void volver_boton_grande(int valor,int x,screenFunc_t ir_a) {
    if (valor == 0){
      u8g.setPrintPos(45, 63);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_VOLVER));
      u8g.setColorIndex(1);
      u8g.drawBitmapP(x,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT,volver_boton_b);
      if (LCD_CLICKED) {
        lcd_goto_screen(ir_a, true);
      }
    }else{
      u8g.drawBitmapP(x,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT,volver_boton_a);
    }
  }

  //----------------------------------------------------------------------------
  void menu_pre_filamento() {
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(22, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_PRE));
    u8g.setColorIndex(1);
    if (encoderPosition > 3 && encoderPosition < 50){
      encoderPosition = 4;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //boton_chico(int x, int y, int numero, int valor, const char* name, const char* desactivo, const char* activoA, screenFunc_t ir_a)
    posicion_anterior = 8;
    eje_volver_boton(encoderPosition, 2, auxiliar_bot_chico);
    boton_chico(27, 23, 1, encoderPosition, 12, MSG_PREHEAT_PLA, boton_pla_a, boton_pla_b, pla_k);
    boton_chico(52, 23, 2, encoderPosition, 12, MSG_PREHEAT_ABS, boton_abs_a, boton_abs_b, abs_k);
    boton_chico(77, 23, 3, encoderPosition, 9, MSG_PREHEAT_FLEX, boton_flex_a, boton_flex_b, flex_k);
    boton_chico(102, 23, 4, encoderPosition, 6, MSG_PREHEAT_NYLON, boton_nylon_a, boton_nylon_b, nylon_k);
  }
  //accion menu enfiar o apagar Motores
  void enfriar_manual(){
    fanSpeed = 0;
    setTargetHotend(0, 1);
    setTargetBed(0);
    enqueue_and_echo_commands_P(PSTR("M117 Enfriando"));
    lcd_goto_screen(volver_info, true);
  }
  void apagar_motor_manual(){
    enqueue_and_echo_commands_P(PSTR("M84\n M117 Motor Apagado"));
    lcd_goto_screen(volver_info, true);
  }
  //menu seleccionas boquilla, cama caliente o fan de capa
  //
  void velocidad_manual(){
    u8g.drawBox(0, 0, 128, 18);
    u8g.setPrintPos(33, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_MENSAJE_01));
    u8g.setPrintPos(28, 17);
    lcd_printPGM(PSTR(MSG_MENSAJE_04));
    u8g.setPrintPos(24, 44);
    u8g.setColorIndex(1);
    lcd_printPGM(PSTR(MSG_VELOCIDAD));

    encoderRateMultiplierEnabled = true;

    if(encoderPosition >= 0 && encoderPosition <= 300) {
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else if(encoderPosition >= 300 && encoderPosition <= 500){
      encoderPosition = 300;
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else{
      encoderPosition = 0;
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }
    if(LCD_CLICKED) {
      feedrate_percentage = encoderPosition;
      lcd_goto_screen(volver_info, true);
    }
  }
  void flow_manual(){
    u8g.drawBox(0, 0, 128, 18);
    u8g.setPrintPos(33, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_MENSAJE_01));
    u8g.setPrintPos(49, 17);
    lcd_printPGM(PSTR(MSG_MENSAJE_FLOW_01));
    u8g.setPrintPos(49, 44);
    u8g.setColorIndex(1);
    lcd_printPGM(PSTR(MSG_MENSAJE_FLOW_01));

    encoderRateMultiplierEnabled = true;

    if(encoderPosition >= 0 && encoderPosition <= 500) {
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else if(encoderPosition >= 500 && encoderPosition <= 1000){
      encoderPosition = 500;
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else{
      encoderPosition = 0;
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }
    if(LCD_CLICKED) {
      flow_percentage[0] = encoderPosition;
      lcd_goto_screen(Kuttercraft_menu_print, true, 6);
    }
  }
  //modificar velocidad
  void ir_velocidad_manual() {
    lcd_goto_screen(velocidad_manual, true, feedrate_percentage);
  }
  //modificar flow
  void ir_cambiar_flow() {
    lcd_goto_screen(flow_manual, true, flow_percentage[0]);
  }

  bool ajuste_pid = false;
  float set_temp_pid;
  static void fan_manual();

  void boquilla_manual(){
    u8g.drawBox(0, 0, 128, 18);
    u8g.setPrintPos(33, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_MENSAJE_01));
    u8g.setPrintPos(21, 17);
    lcd_printPGM(PSTR(MSG_MENSAJE_02));
    u8g.setPrintPos(24, 44);
    u8g.setColorIndex(1);
    lcd_printPGM(PSTR(MSG_NOZZLE));

    encoderRateMultiplierEnabled = true;

    if(encoderPosition >= 0 && encoderPosition <= 280) {
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else if(encoderPosition >= 280 && encoderPosition <= 500){
      encoderPosition = 280;
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else{
      encoderPosition = 0;
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }
    if(LCD_CLICKED) {
      if(!ajuste_pid){
        setTargetHotend(encoderPosition, 1);
        lcd_goto_screen(volver_info, true);
      }else{
        set_temp_pid = encoderPosition;
        SERIAL_MV("valor inicial", set_temp_pid);
        lcd_goto_screen(fan_manual, true);
        //fan_manual()
      }
    }
  }
  void cama_caliente_manual(){
    u8g.drawBox(0, 0, 128, 18);
    u8g.setPrintPos(33, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_MENSAJE_01));
    u8g.setPrintPos(21, 17);
    lcd_printPGM(PSTR(MSG_MENSAJE_02));
    u8g.setPrintPos(10, 44);
    u8g.setColorIndex(1);
    lcd_printPGM(PSTR(MSG_BED));

    encoderRateMultiplierEnabled = true;

    if(encoderPosition >= 0 && encoderPosition <= 110) {
      u8g.setPrintPos(92, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else if(encoderPosition >= 110 && encoderPosition <= 500){
      encoderPosition = 110;
      u8g.setPrintPos(95, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else{
      encoderPosition = 0;
      u8g.setPrintPos(95, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }
    if(LCD_CLICKED) {
      setTargetBed(encoderPosition);
      lcd_goto_screen(volver_info, true);
    }
  }

  static void espera_pid();

  void fan_manual(){
    //texto de el fan de capa
    u8g.drawBox(0, 0, 128, 18);
    u8g.setPrintPos(33, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_MENSAJE_01));
    u8g.setPrintPos(28, 17);
    lcd_printPGM(PSTR(MSG_MENSAJE_04));
    u8g.setPrintPos(19, 44);
    u8g.setColorIndex(1);
    lcd_printPGM(PSTR(MSG_FAN_SPEED));
    //se permite los saltos de encoder
    encoderRateMultiplierEnabled = true;

    //limites del encoder entre 0 a 100
    u8g.setPrintPos(91, 44);
    if(encoderPosition >= 0 && encoderPosition <= 100) {
      lcd_print(itostr3(int(encoderPosition)));
    }else if(encoderPosition >= 100 && encoderPosition <= 500){
      encoderPosition = 100;
      lcd_print(itostr3(int(encoderPosition)));
    }else{
      encoderPosition = 0;
      lcd_print(itostr3(int(encoderPosition)));
    }

    //accion al hacer click
    if(LCD_CLICKED) {
      if(!ajuste_pid){
        fanSpeed = (encoderPosition*255)/100;
        lcd_goto_screen(volver_info, true);
      }else{
        fanSpeed = (encoderPosition*255)/100;
        ajuste_pid = false;
        enqueue_and_echo_commands_P(PSTR("M117"));

        enqueue_and_echo_commands_P(PSTR("M313"));
        lcd_goto_screen(espera_pid, true);

      }
    }
  }
  void set_boquilla(){
    lcd_goto_screen(boquilla_manual, true, int(degTargetHotend(0)));
  }
  void set_cama(){
    lcd_goto_screen(cama_caliente_manual, true, int(degTargetBed()));
  }
  void set_fan(){
    lcd_goto_screen(fan_manual, true, int((fanSpeed * 100) / 255));
  }

  void menu_manual_temp() {
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(31, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_TEMPERATURE));
    u8g.setColorIndex(1);
    if (encoderPosition > 3 && encoderPosition < 50){
      encoderPosition = 4;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //boton_chico(int x, int y, int numero, int valor, const char* name, const char* desactivo, const char* activoA, screenFunc_t ir_a)
    posicion_anterior = 7;
    eje_volver_boton(encoderPosition, 2, auxiliar_bot_chico);
    boton_chico(27, 23, 1, encoderPosition, 40, MSG_NOZZLE, boton_cali_a, boton_cali_b, set_boquilla);
    boton_chico(52, 23, 2, encoderPosition, 27, MSG_BED, boton_cama_a, boton_cama_b, set_cama);
    boton_chico(77, 23, 3, encoderPosition, 32, MSG_FAN_SPEED, boton_fan_a, boton_fan_b, set_fan);
    boton_chico(102, 23, 4, encoderPosition, 43, MSG_COOLDOWN, boton_enfriar_a, boton_enfriar_b, enfriar_manual);
  }
  //----------------------------------------------------------------------------
  //Filamento y menus
  // dibuja la barra de carga :)
  // se puede usar para animaciones
  FORCE_INLINE void _dibuja_maestro(bool blink) {
    if (blink)
      u8g.drawBitmapP(13,30,STATUS_CARGA_BY_TE_WIDTH,STATUS_CARGA_HEIGHT,barra_carga_01);
    else {
      //if (!blink)
        u8g.drawBitmapP(13,30,STATUS_CARGA_BY_TE_WIDTH,STATUS_CARGA_HEIGHT,barra_carga_04);
    }
  }
  //
  void continuar_boton(screenFunc_t con, int valor, int x){
    if (valor == 1){
      u8g.setPrintPos(37, 63);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CONTINUAR));
      u8g.setColorIndex(1);
      u8g.drawBitmapP(x,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT,seguir_boton_b);//volver_boton_a
      if(LCD_CLICKED) {lcd_goto_screen(con, true);}
    }else{
      u8g.drawBitmapP(x,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT,seguir_boton_a);//9
    }
  }
  //----------------------------------------------------------------------------//
  //menu cargar o descarga (ajustes de filamento)
  //barra_de_espera(tiempo, pos texto, mensaje, salida);
  //generador del menu espera de barra de espera
  static void barra_de_espera(int tiempo_a_esperar, int x,const char* mensaje_espera, screenFunc_t ir_a) {
    //encargados de actualizar
    bool blink = lcd_blink();
    lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
    defer_return_to_status = true;
    if (lcdDrawUpdate);
    //mensages que se muestran en el menu
    u8g.drawBox(0, 0, 128, 10);
    u8g.drawBox(0, 55, 128, 10);
    u8g.setColorIndex(0);
    u8g.setPrintPos(15, 8);
    lcd_printPGM(PSTR(MSG_ESPERA));
    u8g.setPrintPos(x, 63);
    lcd_print(mensaje_espera);
    u8g.setColorIndex(1);
    _dibuja_maestro(blink);

    //1000 == un_contador_de_tiempo es igual a 50 segundos
    un_contador_de_tiempo++;

    //pregunta si se ejecuto el suficiente tiempo como para salir
    if(un_contador_de_tiempo > tiempo_a_esperar){
      //rest valores
      un_contador_de_tiempo = 0;
      defer_return_to_status = false;
      //salida
      lcd_goto_screen(ir_a, true);
    }
    //encargado de seguir actualizando
    lcdDrawUpdate =
      #if ENABLED(DOGLCD)
        LCDVIEW_CALL_REDRAW_NEXT
      #else
        LCDVIEW_CALL_NO_REDRAW
      #endif
    ;
  }


  //encargado del retirar filameno
  void retirar_filamento() {
    if(un_contador_de_tiempo == 0){
      enqueue_and_echo_commands_P(PSTR(MSG_RETIRAR_FILAMENTO));
    }
    //codigo para volver a posicion inicial
    posicion_anterior = 31;
    //lleva al menu de barra de espera
    //barra_de_espera(tiempo, pos texto, mensaje, salida);
    barra_de_espera(520, 7, MSG_FILAMENT_CHANGE_LOAD_2, auxiliar_bot_grande);
  }
  //encargado del cargar filameno
  void cargar_filamento() {
    //1000 == un_contador_de_tiempo es igual a 50 segundos
    if(un_contador_de_tiempo == 0){
      enqueue_and_echo_commands_P(PSTR(MSG_CARGAR_FILAMENTO));
    }
    //codigo para volver a posicion inicial
    posicion_anterior = 30;
    //barra_de_espera(tiempo, pos texto, mensaje, salida);
    barra_de_espera(810, 10, MSG_FILAMENT_CHANGE_EXTRUDE_2, auxiliar_bot_grande);
  }
  // el menu de carga y descarda, mientras no esta imprimiendo
  void cargar_retirar() {
    defer_return_to_status = true;
    //texto
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(7, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_FILAMENTO));
    u8g.setColorIndex(1);
    //comprueba la pocisione del eje si no se conose o es muy baja sube el eje z sube
    if(int(current_position[Z_AXIS]) == 0){
      enqueue_and_echo_commands_P(PSTR("G91\nG1 Z10 F3500\nG90"));
    }
    //LIMITES del encoder
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }

    posicion_anterior = 0;

    if(LCD_CLICKED){defer_return_to_status = false;}

    //botones
    volver_boton_grande(encoderPosition, 1, auxiliar_bot_grande);
    boton_grande(49, 18, 1, encoderPosition, 16, MSG_CARGAR,  cargar_boton_a, cargar_boton_b,   cargar_filamento);
    boton_grande(89, 18, 2, encoderPosition, 15, MSG_RETIRAR, retirar_boton_a, retirar_boton_b,  retirar_filamento);
  }
//se encarga de esperar a que la temperatura actual sea igual ala objetivo
  void espera_pid(/* arguments */) {
    //animaciones
    bool blink = lcd_blink();
    lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
    defer_return_to_status = true;
    if (lcdDrawUpdate);

    //texto
    u8g.drawBox(0, 0, 128, 10);
    u8g.drawBox(0, 55, 128, 10);
    u8g.setColorIndex(0);
    u8g.setPrintPos(25, 8);
    lcd_printPGM(PSTR(MSG_PID01));

    u8g.setPrintPos(1, 63);
    u8g.setFont(u8g_font_5x8);
    lcd_printPGM(PSTR(MSG_PID03));
    u8g.setColorIndex(1);
    //animaciones
    _dibuja_maestro(blink);
    //texto
    u8g.setPrintPos(13, 28);
    u8g.setFont(u8g_font_6x10);
    lcd_printPGM(PSTR(MSG_PID02));
    u8g.setPrintPos(95, 28);
    lcd_print(itostr3(int(degHotend(0)) + 0.5 ));

    //Actualizacion
    lcdDrawUpdate =
      #if ENABLED(DOGLCD)
        LCDVIEW_CALL_REDRAW_NEXT
      #else
        LCDVIEW_CALL_NO_REDRAW
      #endif
    ;
  }

  static void menu_de_carga() {
    bool blink = lcd_blink();
    lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
    defer_return_to_status = true;
    if (lcdDrawUpdate);
    u8g.drawBox(0, 0, 128, 10);
    u8g.drawBox(0, 55, 128, 10);
    u8g.setColorIndex(0);
    u8g.setPrintPos(15, 8);
    lcd_printPGM(PSTR(MSG_ESPERA));
    u8g.setPrintPos(24, 63);
    lcd_printPGM(PSTR(MSG_CALENTADO));
    u8g.setColorIndex(1);
    _dibuja_maestro(blink);
    u8g.setPrintPos(70, 28);
    lcd_print(itostr3(int(degHotend(0)) + 0.5 ));
    lcd_print('/');
    lcd_print(itostr3(int(degTargetHotend(0)) + 0.5 ));
    if(degHotend(0) >= degTargetHotend(0) - 5){
      defer_return_to_status = false;
      lcd_goto_screen(cargar_retirar, true);
    }
    //Actualizacion
    lcdDrawUpdate =
      #if ENABLED(DOGLCD)
        LCDVIEW_CALL_REDRAW_NEXT
      #else
        LCDVIEW_CALL_NO_REDRAW
      #endif
    ;
  }

  //menu de carga calibrar 01
  static void menu_de_carga_calibar_01(bool unaVerdad, screenFunc_t aDondeIr, const char* texto) {
    bool blink = lcd_blink();
    lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
    defer_return_to_status = true;
    if (lcdDrawUpdate);
    u8g.drawBox(0, 0, 128, 10);
    u8g.drawBox(0, 55, 128, 10);
    u8g.setColorIndex(0);
    u8g.setPrintPos(15, 8);
    lcd_printPGM(PSTR(MSG_ESPERA));
    u8g.setPrintPos(10, 63);
    lcd_print(texto);
    u8g.setColorIndex(1);
    _dibuja_maestro(blink);
    //condicion
    if(unaVerdad){
      defer_return_to_status = false;
      lcd_goto_screen(aDondeIr, true);}
    //Actualizacion
    lcdDrawUpdate =
      #if ENABLED(DOGLCD)
        LCDVIEW_CALL_REDRAW_NEXT
      #else
        LCDVIEW_CALL_NO_REDRAW
      #endif
    ;
  }
  void aux_preseteo(/* arguments */) {
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(22, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_PRE));
    u8g.setColorIndex(1);
    if (encoderPosition > 3 && encoderPosition < 50){
      encoderPosition = 4;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //boton_chico(int x, int y, int numero, int valor, const char* name, const char* desactivo, const char* activoA, screenFunc_t ir_a)
    posicion_anterior = 32;
    eje_volver_boton(encoderPosition, 2, auxiliar_bot_chico);
    boton_chico(27, 23, 1, encoderPosition, 12, MSG_PREHEAT_PLA, boton_pla_a, boton_pla_b, pla_k2);
    boton_chico(52, 23, 2, encoderPosition, 12, MSG_PREHEAT_ABS, boton_abs_a, boton_abs_b, abs_k2);
    boton_chico(77, 23, 3, encoderPosition, 9, MSG_PREHEAT_FLEX, boton_flex_a, boton_flex_b, flex_k2);
    boton_chico(102, 23, 4, encoderPosition, 6, MSG_PREHEAT_NYLON, boton_nylon_a, boton_nylon_b, nylon_k2);
  }
  //El menu que permite elejir la temperatura de el extrusor en "ajustes de filamento"
  static void sele_temperatura() {
    u8g.drawBox(0, 0, 128, 18);
    u8g.setPrintPos(33, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_MENSAJE_01));
    u8g.setPrintPos(21, 17);
    lcd_printPGM(PSTR(MSG_MENSAJE_02));
    u8g.setPrintPos(24, 44);
    u8g.setColorIndex(1);
    lcd_printPGM(PSTR(MSG_MENSAJE_03));

    encoderRateMultiplierEnabled = true;

    if(encoderPosition >= 0 && encoderPosition <= 280) {
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else if(encoderPosition >= 280 && encoderPosition <= 500){
      encoderPosition = 280;
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }else{
      encoderPosition = 0;
      u8g.setPrintPos(84, 44);
      lcd_print(itostr3(int(encoderPosition)));
    }
    //aumentar velocidad
    if(LCD_CLICKED) {
      if(encoderPosition <= 100){
        lcd_goto_screen(aux_preseteo, true);
        }else{
        setTargetHotend(encoderPosition, 1);
        lcd_goto_screen(menu_de_carga);
      }
    }

  }
  //confirmar se se quiere seguir.
  //solo cundo se entra a "ajustes de filameno" == MSG_FILAMENTO
  void confirmar(screenFunc_t con, int x, int anterior, const char* texto) {
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(x, 8);
    u8g.setColorIndex(0);
    lcd_print(texto);
    u8g.setColorIndex(1);

    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 1;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    posicion_anterior = anterior;
    volver_boton_grande(encoderPosition, 23, auxiliar_bot_grande);
    continuar_boton(con, encoderPosition, 74);
  }

  //----------------------------------------------------------------------------//
  //inici menu de calibracion
  void home_z_calibrar(){
    #if(PK2_PLUS_PLUS)
    enqueue_and_echo_commands_P(PSTR("G1 Z15 F1000\nG28 Z\nG1 Z0"));
    #elif(PK3_PLUS_PLUS)
      enqueue_and_echo_commands_P(PSTR("G1 Z15 F1000\nG28 Z\nG1 Z0"));
    #else
      enqueue_and_echo_commands_P(PSTR("G1 Z15 F400\nG28 Z\nG1 Z0"));
    #endif
      lcd_goto_screen(auxiliar_bot_grande, true);
  }
  void salida_calibracion_06(/* arguments */) {
    defer_return_to_status = false;
    lcd_goto_screen(volver_info, true);
  }
  void salida_calibracion_05(/* arguments */) {
    enqueue_and_echo_commands_P(PSTR("G1 Z10\nG28\nM84"));
    lcd_goto_screen(salida_calibracion_06, true);
  }
  void cuarto_punto() {
    defer_return_to_status = true;
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(19, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_CALI_02 MSG_CALI_06));
    u8g.setColorIndex(1);
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    posicion_anterior = 23;

    boton_grande(9, 18, 0, encoderPosition, 46, MSG_VOLVER, volver_boton_a,  volver_boton_b,  auxiliar_bot_chico);
    boton_grande(49, 18, 1, encoderPosition, 10, MSG_Z_HOME, home_boton_a,  home_boton_b,  home_z_calibrar);
    boton_grande(89, 18, 2, encoderPosition, 37, MSG_CONTINUAR, seguir_boton_a,  seguir_boton_b,  salida_calibracion_05);
  }
  void espera_cuarto_punto(/* arguments */) {
    barra_de_espera(140, 37, MSG_CALI_01, cuarto_punto);
  }
  void salida_calibracion_04(/* arguments */) {
    enqueue_and_echo_commands_P(PSTR(MSG_PRIMER_CUARTO));
    lcd_goto_screen(espera_cuarto_punto, true);
  }
  void tercero_punto() {
    defer_return_to_status = true;
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(19, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_CALI_02 MSG_CALI_05));
    u8g.setColorIndex(1);
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    posicion_anterior = 22;

    boton_grande(9, 18, 0, encoderPosition, 46, MSG_VOLVER, volver_boton_a,  volver_boton_b,  auxiliar_bot_chico);
    boton_grande(49, 18, 1, encoderPosition, 10, MSG_Z_HOME, home_boton_a,  home_boton_b,  home_z_calibrar);
    boton_grande(89, 18, 2, encoderPosition, 37, MSG_CONTINUAR, seguir_boton_a,  seguir_boton_b,  salida_calibracion_04);
  }
  void espera_tercero_punto(/* arguments */) {
    barra_de_espera(75, 31, MSG_CALI_01, tercero_punto);
  }
  void salida_calibracion_03(/* arguments */) {
    enqueue_and_echo_commands_P(PSTR(MSG_PRIMER_TERCERO));
    lcd_goto_screen(espera_tercero_punto, true);
  }
  void segundo_punto() {
    defer_return_to_status = true;
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(19, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_CALI_02 MSG_CALI_04));
    u8g.setColorIndex(1);
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    posicion_anterior = 21;

    boton_grande(9, 18, 0, encoderPosition, 46, MSG_VOLVER, volver_boton_a,  volver_boton_b,  auxiliar_bot_chico);
    boton_grande(49, 18, 1, encoderPosition, 10, MSG_Z_HOME, home_boton_a,  home_boton_b,  home_z_calibrar);
    boton_grande(89, 18, 2, encoderPosition, 37, MSG_CONTINUAR, seguir_boton_a,  seguir_boton_b,  salida_calibracion_03);
  }
  void espera_segundo_punto(/* arguments */) {
    barra_de_espera(155, 31, MSG_CALI_01, segundo_punto);
  }
  void salida_calibracion_02(/* arguments */) {
    enqueue_and_echo_commands_P(PSTR(MSG_PRIMER_SEGUNDO));
    lcd_goto_screen(espera_segundo_punto, true);
  }
  void primer_punto() {
    defer_return_to_status = true;
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(19, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_CALI_02 MSG_CALI_03));
    u8g.setColorIndex(1);
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 1;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    posicion_anterior = 20;
    boton_grande(23, 18, 0, encoderPosition, 10, MSG_Z_HOME, home_boton_a,  home_boton_b,  home_z_calibrar);
    boton_grande(74, 18, 1, encoderPosition, 37, MSG_CONTINUAR, seguir_boton_a,  seguir_boton_b,  salida_calibracion_02);
  }
  void espera_primer_punto(/* arguments */) {
    barra_de_espera(75, 31, MSG_CALI_01, primer_punto);
  }
  void salida_calibracion_01(/* arguments */) {
    enqueue_and_echo_commands_P(PSTR(MSG_PRIMER_PUNTO));
    lcd_goto_screen(espera_primer_punto, true);
  }
  void salida_calibracion_00(/* arguments */) {
    menu_de_carga_calibar_01(axis_homed[X_AXIS] && axis_homed[Y_AXIS] && axis_homed[Z_AXIS], salida_calibracion_01, MSG_CALIBRAR_HOME);
  }
  void comenzar_calibracion() {
    enqueue_and_echo_commands_P(PSTR("G28"));
    lcd_goto_screen(salida_calibracion_00, true);
  }
  //Fin de calibracion
  //----------------------------------------------------------------------------//
  void confirmar_filamento()  { confirmar(sele_temperatura, 7,  0, MSG_FILAMENTO); }
  void confirmar_calibrar()   { confirmar(comenzar_calibracion, 35, 2, MSG_CALIBRAR); }
  //menu de temperatura (manual o Predeterminado)

  void menu_calibracion_mecanica(/* arguments */) {
    //texto
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(31, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_CALIBRAR));
    u8g.setColorIndex(1);
    //barreras
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //botones
    posicion_anterior = 2;

    volver_boton_grande(encoderPosition, 9, auxiliar_bot_grande);
    boton_grande(49, 18, 1, encoderPosition, 25, MSG_CALIBRAR_BASE, calibracion_mecanica_blanco,     calibracion_mecanica_negro,     comenzar_calibracion);
    boton_grande(89, 18, 2, encoderPosition, 22, MGS_ALINEAR_EJE_Z, boton_aliniar_eje_grande_blanco,  boton_aliniar_eje_grande_negro,  alinear_eje_z);
  }

  //----------------------------------------------------------------------------//
  //menu temperatura (predeterminada o manual=
  void temp_manu_pre(/* arguments */) {
    //texto
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(31, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_TEMPERATURE));
    u8g.setColorIndex(1);
    //barreras
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //botones
    posicion_anterior = 3;
    volver_boton_grande(encoderPosition, 9, auxiliar_bot_grande);
    boton_grande(49, 18, 1, encoderPosition, 21, MSG_PRE, temp_pre_boton_a,     temp_pre_boton_b,     menu_pre_filamento);
    boton_grande(89, 18, 2, encoderPosition, 46, MSG_MANUAL, temp_manual_boton_a,  temp_manual_boton_b,  menu_manual_temp);
  }
  //----------------------------------------------------------------------------//
  //menu mover eje (home o mover eje)

  void mover_eje() {
    if(tipo_eje == 0) {
      _lcd_move_xyz(PSTR(MSG_MOVE_X), X_AXIS);
    }else if(tipo_eje == 1){
      _lcd_move_xyz(PSTR(MSG_MOVE_Y), Y_AXIS);
    }else if(tipo_eje == 2){
      _lcd_move_xyz(PSTR(MSG_MOVE_Z), Z_AXIS);
    }
  }
  void mover_eje_01() {
    move_menu_scale = 0.1;
    mover_eje();
  }
  void mover_eje_1() {
    move_menu_scale = 1.0;
    mover_eje();
  }
  void mover_eje_5() {
    move_menu_scale = 5.0;
    mover_eje();
  }
  void seleccionar_mm_mover() {
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(34, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_PREPARE));
    u8g.setColorIndex(1);
    if (encoderPosition > 2 && encoderPosition < 50){
      encoderPosition = 3;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //boton_chico(int x, int y, int numero, int valor, const char* name, const char* desactivo, const char* activoA, screenFunc_t ir_a)
    lcd_save_previous_menu();
    eje_volver_boton(encoderPosition, 15, auxiliar_bot_chico);
    boton_chico(40, 23, 1, encoderPosition, 31, MSG_01MM,  boton_01_a,  boton_01_b, mover_eje_01);
    boton_chico(65, 23, 2, encoderPosition, 37, MSG_1MM,  boton_1_a,   boton_1_b, mover_eje_1);
    boton_chico(90, 23, 3, encoderPosition, 37, MSG_5MM,  boton_5_a,   boton_5_b, mover_eje_5);
  }
  void mover_eje_volver_01(){
    lcd_goto_screen(seleccionar_mm_mover, true, 1);
  }
  void mover_eje_volver_1(){
    lcd_goto_screen(seleccionar_mm_mover, true, 2);
  }
  void mover_eje_volver_5(){
    lcd_goto_screen(seleccionar_mm_mover, true, 3);
  }
  void seleccionar_eje_x() {
    posicion_anterior = 4;
    tipo_eje = 0;
    seleccionar_mm_mover();
  }
  void seleccionar_eje_y() {
    posicion_anterior = 5;
    tipo_eje = 1;
    seleccionar_mm_mover();
  }
  void seleccionar_eje_z() {
    posicion_anterior = 6;
    tipo_eje = 2;
    seleccionar_mm_mover();
  }
  void seleccionar_eje_mover() {
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(34, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_PREPARE));
    u8g.setColorIndex(1);
    if (encoderPosition > 2 && encoderPosition < 50){
      encoderPosition = 3;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //boton_chico(int x, int y, int numero, int valor, const char* name, const char* desactivo, const char* activoA, screenFunc_t ir_a)
    posicion_anterior = 3;
    eje_volver_boton(encoderPosition, 15, auxiliar_bot_chico);
    boton_chico(40, 23, 1, encoderPosition, 49, MSG_X_EJE, boton_x_a, boton_x_b, seleccionar_eje_x);
    boton_chico(65, 23, 2, encoderPosition, 49, MSG_Y_EJE, boton_y_a, boton_y_b, seleccionar_eje_y);
    boton_chico(90, 23, 3, encoderPosition, 49, MSG_Z_EJE, boton_z_a, boton_z_b, seleccionar_eje_z);
  }
  //menu Home
  void home_manual(){
    posicion_anterior = 11;
    enqueue_and_echo_commands_P(PSTR("G28"));
    lcd_goto_screen(auxiliar_bot_chico, true);
  }
  void home_x_manual(){
    posicion_anterior = 12;
    enqueue_and_echo_commands_P(PSTR("G28 X"));
    lcd_goto_screen(auxiliar_bot_chico, true);
  }
  void home_y_manual(){
    posicion_anterior = 13;
    enqueue_and_echo_commands_P(PSTR("G28 Y"));
    lcd_goto_screen(auxiliar_bot_chico, true);
  }
  void seleccionar_eje_home();

  void primero_necesita_home_x_y(){
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    //texto
    u8g.setPrintPos(31, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_ADV)); //Advertencia
    u8g.setColorIndex(1);

    u8g.setPrintPos(16, 30);
    lcd_printPGM(PSTR("Primero Realizar"));
    u8g.setPrintPos(4, 40);
    lcd_printPGM(PSTR("Home (Eje X) (Eje Y)"));
    if (LCD_CLICKED) {
      lcd_goto_screen(seleccionar_eje_home,true,4);
    }
  }

  void primero_sonda_z_fuera(){
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    //texto
    u8g.setPrintPos(31, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_ADV)); //Advertencia
    u8g.setColorIndex(1);

    u8g.setPrintPos(25, 30);
    lcd_printPGM(PSTR("Sonda Z Fuera"));
    u8g.setPrintPos(7, 40);
    lcd_printPGM(PSTR("De Zona De Medicion"));

    if (LCD_CLICKED) {
      lcd_goto_screen(seleccionar_eje_home,true,4);
    }
  }

  void alerta_sonda_z(){
    lcd_goto_screen(primero_sonda_z_fuera,true);
  }

  void home_z_manual(){
    posicion_anterior = 14;
    if(axis_homed[X_AXIS] && axis_homed[Y_AXIS]){
      enqueue_and_echo_commands_P(PSTR("G28 Z"));
      lcd_goto_screen(auxiliar_bot_chico, true);
    }else{
      lcd_goto_screen(primero_necesita_home_x_y,true);
    }
  }

  void seleccionar_eje_home() {
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(1, 8);
    u8g.setColorIndex(0);
    //lcd_printPGM(PSTR(MSG_FILAMENTO));
    lcd_printPGM(PSTR("Llevar Al Origen Eje:"));
    u8g.setColorIndex(1);
    if (encoderPosition > 3 && encoderPosition < 50){
      encoderPosition = 4;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //boton_chico(int x, int y, int numero, int valor, const char* name, const char* desactivo, const char* activoA, screenFunc_t ir_a)
    posicion_anterior = 2;
    eje_volver_boton(encoderPosition, 2, auxiliar_bot_chico);
    boton_chico(27,   23, 1, encoderPosition, 16, MSG_HOME_MENU, boton_inicio_a,  boton_inicio_b, home_manual);
    boton_chico(52,   23, 2, encoderPosition, 10, MSG_X_HOME, boton_x_a,       boton_x_b, home_x_manual);
    boton_chico(77,   23, 3, encoderPosition, 10, MSG_Y_HOME, boton_y_a,       boton_y_b, home_y_manual);
    boton_chico(102,  23, 4, encoderPosition, 10, MSG_Z_HOME, boton_z_a,       boton_z_b, home_z_manual);

  }
  void menu_mover_eje() {
    //texto
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(34, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_PREPARE));
    u8g.setColorIndex(1);
    //barreras
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }

    posicion_anterior = 4;
    volver_boton_grande(encoderPosition, 9, auxiliar_bot_grande);
    boton_grande(49, 18, 1, encoderPosition, 34, MSG_PREPARE, mover_boton_a,  mover_boton_b,  seleccionar_eje_mover);
    boton_grande(89, 18, 2, encoderPosition, 16, MSG_HOME_MENU, home_boton_a,   home_boton_b,   seleccionar_eje_home);

  }
  static void auxiliar_bot_chico() {
    if(posicion_anterior == 3){
      //se entro a mover eje boton 1
      lcd_goto_screen(menu_mover_eje, true, 1);
    }else if(posicion_anterior == 4){
      //se entro a eje x boton 1
      lcd_goto_screen(seleccionar_eje_mover, true, 1);
    }else if(posicion_anterior == 5){
      //se entro a eje y boton 2
      lcd_goto_screen(seleccionar_eje_mover, true, 2);
    }else if(posicion_anterior == 6){
      //se entro a eje z boton 3
      lcd_goto_screen(seleccionar_eje_mover, true, 3);
    }else if(posicion_anterior == 2){
      //se entro a home boton 2
      lcd_goto_screen(menu_mover_eje, true, 2);
    }else if(posicion_anterior == 7){
      //se entro a home boton 2
      lcd_goto_screen(temp_manu_pre, true, 2);
    }else if(posicion_anterior == 8){
      lcd_goto_screen(temp_manu_pre, true, 1);
    }else if(posicion_anterior == 11){
      lcd_goto_screen(seleccionar_eje_home, true, 1);
    }else if(posicion_anterior == 12){
      lcd_goto_screen(seleccionar_eje_home, true, 2);
    }else if(posicion_anterior == 13){
      lcd_goto_screen(seleccionar_eje_home, true, 3);
    }else if(posicion_anterior == 14){
      lcd_goto_screen(seleccionar_eje_home, true, 4);
    }else if(posicion_anterior == 21){
      lcd_goto_screen(salida_calibracion_01, true, 1);
    }else if(posicion_anterior == 22){
      lcd_goto_screen(salida_calibracion_02, true, 1);
    }else if(posicion_anterior == 23){
      lcd_goto_screen(salida_calibracion_03, true, 1);
    }else if(posicion_anterior == 32){
      lcd_goto_screen(Kuttercraft_menu, true, 2);
    }else if(posicion_anterior == 31){
      lcd_goto_screen(menu_de_carga, true, 0);
    }
  }
  // botones menu print
  void cartel_de_pausar_impresion(){
    //error de cama maximo o minimo
    defer_return_to_status = true;
    //bool blink = lcd_blink();
    lcdDrawUpdate = LCDVIEW_REDRAW_NOW;

    u8g.setColorIndex(1);
    u8g.drawBox(0, 55, 128, 10);

    u8g.setPrintPos(10, 25);
    lcd_printPGM(PSTR(MSG_PAUSANDO_IMPRESION));

    u8g.setPrintPos(13, 34);
    lcd_printPGM(PSTR(MSG_PUEDE_TARDAR_UNOS));

    u8g.setPrintPos(28, 43);
    lcd_printPGM(PSTR(MSG_SEGUNDOS));//"Maximo o Minimo"

    //encargado de seguir actualizando
    lcdDrawUpdate =
      #if ENABLED(DOGLCD)
        LCDVIEW_CALL_REDRAW_NEXT
      #else
        LCDVIEW_CALL_NO_REDRAW
      #endif
    ;

    u8g.drawBox(0, 0, 128, 10);

    un_contador_de_tiempo++;

    if (un_contador_de_tiempo == 200) {
      un_contador_de_tiempo = 0;
      defer_return_to_status = false;
      lcd_return_to_status();
    }

  }

  void pausa_print() {
    //para evitar errores durante la pausa
    un_contador_de_tiempo = 0;

    if(save_on_off){
      se_pauso_el_autoguardado = true;
      save_on_off = false;
    }
    enqueue_and_echo_commands_P(PSTR("M25"));
    lcd_goto_screen(cartel_de_pausar_impresion, true);
  }

  void cartel_de_canselar_impresion(){
    //error de cama maximo o minimo
    defer_return_to_status = true;
    //bool blink = lcd_blink();
    lcdDrawUpdate = LCDVIEW_REDRAW_NOW;

    u8g.setColorIndex(1);
    u8g.drawBox(0, 55, 128, 10);

    u8g.setPrintPos(4, 25);
    lcd_printPGM(PSTR(MSG_CANCELANDO_IMPRESION));

    u8g.setPrintPos(13, 34);
    lcd_printPGM(PSTR(MSG_PUEDE_TARDAR_UNOS));

    u8g.setPrintPos(28, 43);
    lcd_printPGM(PSTR(MSG_SEGUNDOS));//"Maximo o Minimo"

    //encargado de seguir actualizando
    lcdDrawUpdate =
      #if ENABLED(DOGLCD)
        LCDVIEW_CALL_REDRAW_NEXT
      #else
        LCDVIEW_CALL_NO_REDRAW
      #endif
    ;

    u8g.drawBox(0, 0, 128, 10);

    card.stopPrint();
    un_contador_de_tiempo++;

    if (un_contador_de_tiempo == 100) {
      un_contador_de_tiempo = 0;
      defer_return_to_status = false;
      lcd_return_to_status();
    }

  }

  void cancelar_print() {
    un_contador_de_tiempo = 0;
    salida_de_emg_temp_bed = false;
    salida_de_emg_temp_hotend = false;
    se_estaba_imprimiendo = false;
    print_job_counter.imprimiendo_estado = false;

    clear_command_queue();

    card.stopPrint();
    enqueue_and_echo_commands_P(PSTR("M117\nM84\nM117 Impresion Cancelada"));

    lcd_goto_screen(cartel_de_canselar_impresion, true);

  }

  void reiniciar_print(/* arguments */) {
    asm volatile ("jmp 0");
    lcd_goto_screen(volver_info, true);
    /* code */
  }

  void cambiar_filamento() {
    //gcode_M600();
    enqueue_and_echo_commands_P(PSTR("M600"));
    lcd_goto_screen(volver_info, true);
  }
  void ajustes();
  void salir_ajustes(/* arguments */) {
    lcd_goto_screen(ajustes, true, 3);
  }
  void salir_del_pid(){
    enqueue_and_echo_commands_P(PSTR("M500"));
    lcd_goto_screen(volver_info, true);
    enqueue_and_echo_commands_P(PSTR("M117 PID Actualizado"));
    apagar_error_temp = false;
  }
  void removeSpacesInPlace(char* str) {
     size_t str_len = strlen(str); // para tener strlen se debe incluir <string.h>
     char result [str_len];
     size_t p = 0; size_t i = 0;
     for (i = 0; i < str_len; ++i)
      {
        if (str[i] != ' ')
        { // result necesita su propio posicionador o iremos dejando agujeros
          result[p] = str[i];
          // Sólo avanzamos la posición p si se realiza la inserción
          p++;
        }
      }
      // funciones como printf buscan el caracter 0
      // aunque dejamos str con el mismo tamaño en memoria, ponemos la
      // marca de fin de la cadena en donde corresponde
      if (p < str_len) str[p] = '\0';
      // Reescribimos str con el contenido de result
      for (i = 0; i < p; ++i)
      {
         str[i] = result[i];
      }
  }
  void version_k() {
    u8g.drawBitmapP(23, 0, 10, 12,logo_kuttercraft);
    u8g.drawBox(0 , 55, 128, 9);
    u8g.drawBox(0 , 35, 128, 9);

    u8g.setPrintPos(12, 22);
    lcd_printPGM(PSTR(MSG_VER_VERSION));

    u8g.setPrintPos(12, 32);
    lcd_printPGM(PSTR(MSG_VER_MODELO));

    u8g.setPrintPos(66, 22);
    lcd_printPGM(PSTR(SHORT_BUILD_VERSION));

    u8g.setPrintPos(66, 32);
    lcd_printPGM(PSTR(MSG_TIPO));

    u8g.setPrintPos(46, 43);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_VER_SERIE));
    u8g.setColorIndex(1);
    u8g.setPrintPos(28, 53);
    //PK31234-1234

    lcd_print(numero_de_serie);

    // u8g.setFont(u8g_font_5x8);
    // u8g.setPrintPos(16, 35);
    // lcd_printPGM(PSTR(MSG_WWW MSG_WEB));
    // u8g.setFont(u8g_font_6x10);
    u8g.setColorIndex(0);
    u8g.setPrintPos(8, 63);
    lcd_printPGM(PSTR(MSG_VER_IND_ARG));
    u8g.setColorIndex(1);

    if(LCD_CLICKED) {
      //posicion_anterior = 40;
      //lcd_goto_screen(auxiliar_bot_grande, true);
      lcd_goto_screen(ajustes, true, 1);
      if(nuevo_serial){
        nuevo_serial = false;
        Config_StoreSettings();
      }
    }
  }
  #if(CON_SENSOR_INDUCTIVO)
  //los puntos del Mesh
  uint8_t mash_x = 1;
  uint8_t mash_y = 1;
  uint8_t punto_mesh = 1;

  void cambiarPuntoMesh(){
    ENCODER_DIRECTION_NORMAL();
    encoderRateMultiplierEnabled = true;
    if (encoderPosition) {
      refresh_cmd_timeout();
      posicion_z += float((int32_t)encoderPosition) *  (0.025);
      NOLESS(posicion_z, -(LCD_PROBE_Z_RANGE) * 0.5);
      NOMORE(posicion_z,  (LCD_PROBE_Z_RANGE) * 0.5);
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_REDRAW_NOW
        #endif
      ;
      encoderPosition = 0;
    }

    static bool debounce_click = false;
    if (LCD_CLICKED) {
        debounce_click = true;
        mbl.set_zigzag_z(punto_mesh - 1, posicion_z + (posicion_z < 0 ? -0.0001 : 0.0001));
          mbl.set_has_mesh(true);
          #if HAS(BUZZER)
            buzz(200, 659);
            buzz(200, 698);
          #endif
          posicion_z = 0;
          //mbl.reset();
          Config_StoreSettings();
          salirDeMeshManual(punto_mesh);
          //lcd_goto_screen(_lcd_level_bed_done, true);
    }
    if (lcdDrawUpdate) {
      float v = posicion_z;
      lcd_implementation_drawedit(PSTR(MSG_MOVE_Z), ftostr43sign(v + (v < 0 ? -0.0001 : 0.0001), '+'));
    }
  }
  void irAcambiarPuntoMesh() {
    posicion_z = mbl.z_values[mash_y-1][mash_x-1];
    lcd_goto_screen(cambiarPuntoMesh, true);
  }
  void botonUnoMesh(int valor) {
    u8g.setPrintPos(35, 26);
    if(valor == 1){
      u8g.setPrintPos(31, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALI_02"01"));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(41,47,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_A);
      //u8g.setPrintPos(43, 59);
      //lcd_printPGM(PSTR("01"));
      u8g.setPrintPos(43, 58);
      lcd_printPGM(PSTR("01"));

      if(LCD_CLICKED) {
        mash_x = 1;
        mash_y = 1;
        punto_mesh = 1;
        posicion_z = mbl.z_values[mash_y-1][mash_x-1];
        lcd_goto_screen(irAcambiarPuntoMesh, true);
      }
    }else{
      u8g.drawBitmapP(41,47,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_B);
      u8g.setColorIndex(0);
      u8g.setPrintPos(43, 58);
      lcd_printPGM(PSTR("01"));
      u8g.setColorIndex(1);
    }
  }
  void botonDosMesh(int valor) {
    u8g.setPrintPos(35, 42);
    if(valor == 2){
      u8g.setPrintPos(31, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALI_02"02"));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(58,47,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_A);
      u8g.setPrintPos(60, 58);
      lcd_printPGM(PSTR("02"));
      u8g.setColorIndex(1);
      if(LCD_CLICKED) {
        mash_x = 2;
        mash_y = 1;
        punto_mesh = 2;
        lcd_goto_screen(irAcambiarPuntoMesh, true);
      }
    }else{
      u8g.drawBitmapP(58,47,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_B);
      u8g.setColorIndex(0);
      u8g.setPrintPos(60, 58);
      lcd_printPGM(PSTR("02"));
      u8g.setColorIndex(1);
    }
  }
  void botonTresMesh(int valor) {
    u8g.setPrintPos(35, 58);
    if(valor == 3){
      u8g.setPrintPos(31, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALI_02"03"));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(75,47,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_A);
      u8g.setPrintPos(77, 58);
      lcd_printPGM(PSTR("03"));
      u8g.setColorIndex(1);
      if(LCD_CLICKED){
        mash_x = 3;
        mash_y = 1;
        punto_mesh = 3;
        lcd_goto_screen(irAcambiarPuntoMesh, true);
      }
    }else{
      u8g.drawBitmapP(75,47,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_B);
      u8g.setColorIndex(0);
      u8g.setPrintPos(77, 58);
      lcd_printPGM(PSTR("03"));
      u8g.setColorIndex(1);
    }
  }
  void botonCuatroMesh(int valor) {
    u8g.setPrintPos(58, 58);
    if(valor == 4){
      u8g.setPrintPos(31, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALI_02"04"));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(75,30,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_A);
      u8g.setPrintPos(77, 41);
      lcd_printPGM(PSTR("04"));

      if(LCD_CLICKED) {
        mash_x = 3;
        mash_y = 2;
        punto_mesh = 4;
        lcd_goto_screen(irAcambiarPuntoMesh, true);
      }
    }else{
      u8g.drawBitmapP(75,30,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_B);
      u8g.setColorIndex(0);
      u8g.setPrintPos(77, 41);
      lcd_printPGM(PSTR("04"));
      u8g.setColorIndex(1);
    }
  }
  void botonCincoMesh(int valor) {
    u8g.setPrintPos(58, 42);
    if(valor == 5){
      u8g.setPrintPos(31, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALI_02"05"));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(58,30,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_A);
      u8g.setPrintPos(60, 41);
      lcd_printPGM(PSTR("05"));
      u8g.setColorIndex(1);
      if(LCD_CLICKED) {
        mash_x = 2;
        mash_y = 2;
        punto_mesh = 5;
        lcd_goto_screen(irAcambiarPuntoMesh, true);
      }
    }else{
      u8g.drawBitmapP(58,30,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_B);
      u8g.setColorIndex(0);
      u8g.setPrintPos(60, 41);
      lcd_printPGM(PSTR("05"));
      u8g.setColorIndex(1);
    }
  }
  void botonSeisMesh(int valor) {
    u8g.setPrintPos(58, 26);
    if(valor == 6){
      u8g.setPrintPos(31, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALI_02"06"));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(41,30,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_A);
      u8g.setPrintPos(43, 41);
      lcd_printPGM(PSTR("06"));
      u8g.setColorIndex(1);
      if(LCD_CLICKED){
        mash_x = 1;
        mash_y = 2;
        punto_mesh = 6;
        lcd_goto_screen(irAcambiarPuntoMesh, true);
      }
    }else{
      u8g.drawBitmapP(41,30,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_B);
      u8g.setColorIndex(0);
      u8g.setPrintPos(43, 41);
      lcd_printPGM(PSTR("06"));
      u8g.setColorIndex(1);
    }
  }
  void botonSieteMesh(int valor) {
    u8g.setPrintPos(81, 26);
    if(valor == 7){
      u8g.setPrintPos(31, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALI_02"07"));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(41,13,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_A);
      u8g.setPrintPos(43, 24);
      lcd_printPGM(PSTR("07"));
      u8g.setColorIndex(1);
      if(LCD_CLICKED){
        mash_x = 1;
        mash_y = 3;
        punto_mesh = 7;
        lcd_goto_screen(irAcambiarPuntoMesh, true);
      }
    }else{
      u8g.drawBitmapP(41,13,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_B);
      u8g.setColorIndex(0);
      u8g.setPrintPos(43, 24);
      lcd_printPGM(PSTR("07"));
      u8g.setColorIndex(1);
    }
  }
  void botonOchoMesh(int valor) {
    u8g.setPrintPos(81, 42);
    if(valor == 8){
      u8g.setPrintPos(31, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALI_02"08"));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(58,13,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_A);
      u8g.setPrintPos(60, 24);
      lcd_printPGM(PSTR("08"));
      if(LCD_CLICKED){
        mash_x = 2;
        mash_y = 3;
        punto_mesh = 8;
        lcd_goto_screen(irAcambiarPuntoMesh, true);
      }
    }else{
      u8g.drawBitmapP(58,13,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_B);
      u8g.setColorIndex(0);
      u8g.setPrintPos(60, 24);
      lcd_printPGM(PSTR("08"));
      u8g.setColorIndex(1);
    }
  }
  void botonNueveMesh(int valor) {
    u8g.setPrintPos(81, 58);
    if(valor == 9){
      u8g.setPrintPos(31, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CALI_02"09"));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(75,13,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_A);
      u8g.setPrintPos(77, 24);
      lcd_printPGM(PSTR("09"));

      if(LCD_CLICKED){
        mash_x = 3;
        mash_y = 3;
        punto_mesh = 9;
        lcd_goto_screen(irAcambiarPuntoMesh, true);
      }
    }else{
      u8g.drawBitmapP(75,13,MESH_BOTON_W_BY_TEWIDTH,MESH_BOTON_W_HEIGHT,punto_Uno_B);
      u8g.setColorIndex(0);
      u8g.setPrintPos(77, 24);
      lcd_printPGM(PSTR("09"));
      u8g.setColorIndex(1);
    }
  }

  void menu_mesh();
  void vovel_mesh(int valor) {
    if (valor == 0){
      u8g.setPrintPos(46, 9);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_VOLVER));
      u8g.setColorIndex(1);
      u8g.drawBitmapP(3,27,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_volver_a);
      if (LCD_CLICKED) {
        if(print_job_counter.imprimiendo_estado){
          lcd_goto_screen(menu_ajustar_offset_print, true, 1);
        }else{
          lcd_goto_screen(menu_mesh, true, 5);
        }
      }
    }else{
      u8g.drawBitmapP(3,27,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_volver_b);
    }
  }

  void matris_level(/* arguments */) {
    u8g.drawBox(0, 0, 128, 10);
    if (encoderPosition > 8  && encoderPosition < 50){
      encoderPosition = 9;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    lcd_save_previous_menu();
    vovel_mesh(encoderPosition);
    botonUnoMesh(encoderPosition);
    botonDosMesh(encoderPosition);
    botonTresMesh(encoderPosition);
    botonCuatroMesh(encoderPosition);
    botonCincoMesh(encoderPosition);
    botonSeisMesh(encoderPosition);
    botonSieteMesh(encoderPosition);
    botonOchoMesh(encoderPosition);
    botonNueveMesh(encoderPosition);
    //botonDiezMesh(encoderPosition);
    //botonOnceMesh(encoderPosition);
    //botonDoceMesh(encoderPosition);
    //botonTreseMesh(encoderPosition);
    //botonCatorseMesh(encoderPosition);
    //botonQuinceMesh(encoderPosition);
    //botonDieciseisMesh(encoderPosition);
  }
  static void salirDeMeshManual(uint8_t valor){
    lcd_goto_screen(matris_level, true, valor);
  }
  static void ajustes();
  void ajustar_offset_mesh_04() {
    ENCODER_DIRECTION_NORMAL();
    if (encoderPosition) {
      refresh_cmd_timeout();
      offset_mesh_valor += float((int32_t)encoderPosition) * (MBL_Z_STEP);
      //NOLESS(offset_mesh_valor, 0);
      //NOMORE(offset_mesh_valor, MESH_HOME_SEARCH_Z * 2);
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_REDRAW_NOW
        #endif
      ;
      encoderPosition = 0;
    }
    static bool debounce_click = false;
    if (LCD_CLICKED) {
      if (!debounce_click) {
        debounce_click = true;
        enqueue_and_echo_commands_P(PSTR("M500"));
        #if HAS(BUZZER)
          buzz(200, 659);
          buzz(200, 698);
        #endif
        defer_return_to_status = false;
        lcd_goto_screen(ajustes, true, 3);
      }
    }
    else {
      debounce_click = false;
    }

    if (lcdDrawUpdate) {
      float v = offset_mesh_valor;
      //SERIAL_MV("\np: ", v);
      lcd_implementation_drawedit(PSTR(MSG_MOVE_Z), ftostr43sign(v + (v < 0 ? -0.0001 : 0.0001), '+'));
    }
    //-------------------------------------------------------------------------
  }
  void ajustar_offset_mesh_03() {
    //-------------------------------------------------------------------------
    encoderRateMultiplierEnabled = true;
    //-------------------------------------------------------------------------
    ENCODER_DIRECTION_NORMAL();
    if (encoderPosition) {
      refresh_cmd_timeout();
      current_position[Z_AXIS] += float((int32_t)encoderPosition) * (MBL_Z_STEP);
      NOLESS(current_position[Z_AXIS], 0);
      NOMORE(current_position[Z_AXIS], MESH_HOME_SEARCH_Z * 2);
      line_to_current(Z_AXIS);
      lcdDrawUpdate =
        #if ENABLED(DOGLCD)
          LCDVIEW_CALL_REDRAW_NEXT
        #else
          LCDVIEW_REDRAW_NOW
        #endif
      ;
      encoderPosition = 0;
    }

    static bool debounce_click = false;
    if (LCD_CLICKED) {
      if (!debounce_click) {
        debounce_click = true;
        offset_mesh_valor = current_position[Z_AXIS] - 5;
        enqueue_and_echo_commands_P(PSTR("M500\nG1 Z10\nG28"));
        #if HAS(BUZZER)
          buzz(200, 659);
          buzz(200, 698);
        #endif
        defer_return_to_status = false;
        lcd_goto_screen(ajustes, true, 3);
      }
    }
    else {
      debounce_click = false;
    }

    if (lcdDrawUpdate) {
      float v = current_position[Z_AXIS] - 5;
      //SERIAL_MV("\np: ", v);
      lcd_implementation_drawedit(PSTR(MSG_MOVE_Z), ftostr43sign(v + (v < 0 ? -0.0001 : 0.0001), '+'));
    }
    //-------------------------------------------------------------------------
  }
  void ajustar_offset_mesh_02() {
      enqueue_and_echo_commands_P(PSTR("G92 Z5"));
      lcd_goto_screen(ajustar_offset_mesh_03);
  }
  void ajustar_offset_mesh_01() {
    if (lcdDrawUpdate) lcd_implementation_drawedit(PSTR(MSG_LEVEL_BED_HOMING), NULL);
    lcdDrawUpdate =
      #if ENABLED(DOGLCD)
        LCDVIEW_CALL_REDRAW_NEXT
      #else
        LCDVIEW_CALL_NO_REDRAW
      #endif
    ;
    if (axis_homed[X_AXIS] && axis_homed[Y_AXIS] && axis_homed[Z_AXIS])
      lcd_goto_screen(ajustar_offset_mesh_02);
  }
  void ajustar_offset_mesh() {
    defer_return_to_status = true;
    axis_homed[X_AXIS] = axis_homed[Y_AXIS] = axis_homed[Z_AXIS] = false;
    enqueue_and_echo_commands_P(PSTR("G28\nG91\nG1 X44 Y-41 F1500\nG90"));
    lcd_goto_screen(ajustar_offset_mesh_01);
  }
  #endif
  void numero_random() {
    //PK312MM-1234
    if(strlen(numero_de_serie) != 12){
      //GENERA UN NUMERO RANDOM
      nuevo_serial = true;
      char random_n[8]; //aux de char
      //si el numero de serie no tiene la cantidad de diguitos correctos crea uno random
      //if(strlen(numero_de_serie) != 12){
        //vacia las variables
        numero_de_serie[0] = '\0';
        contenedor_str[0] = '\0';


        strcpy(random_n, ltostr7(millis())); //guarda el tiempo como numero random
        removeSpacesInPlace(random_n);//elimina los espacios

        strncpy(contenedor_str, random_n, 4);//solo se guarda los ultimos 4 valores

        //borra los espacios en blando
        removeSpacesInPlace(contenedor_str);

        strcat(numero_de_serie, "PK3");
        strcat(numero_de_serie, "13");
        strcat(numero_de_serie, "NN-");

        //agrega los 0 que falten en el numero
        for (int i = 0; i < 4 - strlen(contenedor_str); i++) {
          strcat(numero_de_serie, PSTR("0"));
        }

        //añade la ultima parte del codigo
        strcat(numero_de_serie, contenedor_str);
        //removeSpacesInPlace(numero_de_serie);
        //Config_StoreSettings();//guarda el numero
        SERIAL_MV("Serial:", numero_de_serie);
        SERIAL_E;
    }
  }


  void ir_version_k(/* arguments */) {
    numero_random();
    lcd_goto_screen(version_k, true);
  }

  void coor_estado(/* arguments */) {
    if(ver_cordenadas){
      ver_cordenadas = false;
    }else{
      ver_cordenadas = true;
    }
    enqueue_and_echo_commands_P(PSTR("M500"));
  }

  //boton
  void boton_calibracion_automatica(int valor) {
    if (valor == 4){
      u8g.setPrintPos(34, 63);
      u8g.setColorIndex(0);

      lcd_print(MSG_AUTO);

      u8g.setColorIndex(1);

      u8g.drawBitmapP(41,29,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, calibrar_automatico_b);
      if (LCD_CLICKED) {
        se_permiten_carteles = false;
        lcd_goto_screen(_lcd_inicio_autolevel_00, true);
      }
    }else{
      u8g.drawBitmapP(41,29,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, calibrar_automatico_a);
    }
  }

  void boton_modificar_offset(int valor) {
    if (valor == 2){
      u8g.setPrintPos(16, 63);
      u8g.setColorIndex(0);

      lcd_print(MSG_MENUS_AUTOLEVEL_02);

      u8g.setColorIndex(1);

      u8g.drawBitmapP(78,4,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, modificar_offset_b);
      if (LCD_CLICKED) {
        lcd_goto_screen(preparar_offset_manual, true);

      }
    }else{
      u8g.drawBitmapP(78,4,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, modificar_offset_a);
    }
  }

  void cartel_alerte_autolevel() {
    u8g.drawBox(0, 54, 128, 10);
    u8g.drawBox(0, 0, 128, 10);

    u8g.setPrintPos(31, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_ADV)); //Advertencia
    u8g.setColorIndex(1);

    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 1;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    if(encoderPosition == 0){
      u8g.drawBitmapP(39, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_detener_b);
      u8g.drawBitmapP(66, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_reanudar_a);
      u8g.setColorIndex(0);
      u8g.setPrintPos(34, 63);
      //
      lcd_printPGM(PSTR(MSG_ZPROBE_CANCELAR)); //Advertencia
      if (LCD_CLICKED) {
        lcd_goto_screen(ajustes, true, 5);
      }
    }else{
      u8g.drawBitmapP(39, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_detener_a);
      u8g.drawBitmapP(66, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_reanudar_b);
      u8g.setColorIndex(0);
      u8g.setPrintPos(43, 63);
      lcd_printPGM(PSTR(MSG_ACEPTAR)); //Advertencia
      if (LCD_CLICKED) {
        if(autolevel_on_off){
          autolevel_on_off = false;
          enqueue_and_echo_commands_P(PSTR("M420 S0\nM500"));
        }else{
          autolevel_on_off = true;
          enqueue_and_echo_commands_P(PSTR("M420 S1\nM500"));
        }
        lcd_goto_screen(ajustes, true, 5);
      }
    }
    u8g.setColorIndex(1);

    if(autolevel_on_off){
      u8g.setPrintPos(10, 19);
      lcd_printPGM(PSTR("[OFF]Use Endstop Z"));
      u8g.setPrintPos(25, 28);
      lcd_printPGM(PSTR("Para calibrar"));
    }else{
      u8g.setPrintPos(1, 19);
      lcd_printPGM(PSTR("[ON]Cuidado Endstop Z"));
      u8g.setPrintPos(13, 28);
      lcd_printPGM(PSTR("no debe activarse"));
    }
  }
  ///
  void cartel_alerte_reinicio_de_memoria() {
    u8g.drawBox(0, 54, 128, 10);
    u8g.drawBox(0, 0, 128, 10);

    u8g.setPrintPos(31, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_ADV)); //Advertencia
    u8g.setColorIndex(1);

    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 1;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    if(encoderPosition == 0){
      u8g.drawBitmapP(39, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_detener_b);
      u8g.drawBitmapP(66, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_reanudar_a);
      u8g.setColorIndex(0);
      u8g.setPrintPos(34, 63);
      lcd_printPGM(PSTR(MSG_ZPROBE_CANCELAR));
      if(LCD_CLICKED){
        lcd_goto_screen(ajustes, true, 13);
      }

    }else{
      u8g.drawBitmapP(39, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_detener_a);
      u8g.drawBitmapP(66, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_reanudar_b);
      u8g.setColorIndex(0);
      u8g.setPrintPos(43, 63);
      lcd_printPGM(PSTR(MSG_ACEPTAR));
      if(LCD_CLICKED){
        lcd_return_to_status();
        enqueue_and_echo_commands_P(PSTR("M502\nM500"));
      }
    }
    //texto
    u8g.setColorIndex(1);
    u8g.setPrintPos(1, 19);
    lcd_printPGM(PSTR("Reiniciar Seteos fabr"));
    u8g.setPrintPos(19, 28);
    lcd_printPGM(PSTR("Sera Permanente"));
  }

  void cartel_alerte_sensor_de_filamento() {
    u8g.drawBox(0, 54, 128, 10);
    u8g.drawBox(0, 0, 128, 10);

    u8g.setPrintPos(31, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_ADV)); //Advertencia
    u8g.setColorIndex(1);

    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 1;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    if(encoderPosition == 0){
      u8g.drawBitmapP(39, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_detener_b);
      u8g.drawBitmapP(66, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_reanudar_a);
      u8g.setColorIndex(0);
      u8g.setPrintPos(34, 63);
      lcd_printPGM(PSTR(MSG_ZPROBE_CANCELAR));
      if(LCD_CLICKED){
        lcd_goto_screen(ajustes, true, 8);
      }

    }else{
      u8g.drawBitmapP(39, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_detener_a);
      u8g.drawBitmapP(66, 30,STATUS_MENU_A_BY_TEWIDTH, STATUS_MENU_A_HEIGHT, boton_reanudar_b);
      u8g.setColorIndex(0);
      u8g.setPrintPos(43, 63);
      lcd_printPGM(PSTR(MSG_ACEPTAR));
      if(LCD_CLICKED){
        if(on_off_sensor_de_filamento){
          on_off_sensor_de_filamento = false;
        }else{
          on_off_sensor_de_filamento = true;
        }
        lcd_goto_screen(ajustes, true, 8);
        enqueue_and_echo_commands_P(PSTR("M500"));
      }

    }
    if(on_off_sensor_de_filamento){
      u8g.setColorIndex(1);
      u8g.setPrintPos(13, 19);
      lcd_printPGM(PSTR("Desactivar Sensor"));
      u8g.setPrintPos(28, 28);
      lcd_printPGM(PSTR("De filamento"));
    }else{
      u8g.setColorIndex(1);
      u8g.setPrintPos(22, 19);
      lcd_printPGM(PSTR("Activar Sensor"));
      u8g.setPrintPos(10, 28);
      lcd_printPGM(PSTR("Pausara la maquina"));
    }
  }
  void no_hay_sensor(){
    u8g.drawBitmapP(58, 22, 2, 15, no_logros_ico);

    u8g.drawBox(0, 54, 128, 10);
    u8g.drawBox(0, 0, 128, 10);

    u8g.setColorIndex(0);
    u8g.setPrintPos(4, 8);
    lcd_printPGM(PSTR("Opcion No Disponible"));
    u8g.setPrintPos(13, 63);
    lcd_printPGM(PSTR("Lo Tiene Activado"));
    u8g.setColorIndex(1);

    u8g.setPrintPos(22, 51);
    lcd_printPGM(PSTR("El Firmware No"));

    if(LCD_CLICKED){
      mbl.reset();
      autolevel_on_off = false;
      lcd_goto_screen(ajustes, true,5);
      enqueue_and_echo_commands_P(PSTR("M420 S0\nM500"));
    }
  }
  ///
  #if(SENSOR_INDUCTIVO_NUEVO)
  void autolevel_estado() {
    lcd_goto_screen(cartel_alerte_autolevel, true);
  }
  #else
  void autolevel_estado() {
    lcd_goto_screen(no_hay_sensor, true);
  }
  #endif

  void guardado_estado(/* arguments */) {
    if(save_on_off){
      save_on_off = false;
    }else{
      save_on_off = true;
    }
    enqueue_and_echo_commands_P(PSTR("M500"));
  }

  void sensor_de_filamento_estado(/* arguments */) {
    lcd_goto_screen(cartel_alerte_sensor_de_filamento, true);
  }

  void encoder_estado(/* arguments */) {
    if(dir_encoder == 1){
      dir_encoder = -1;
    }else{
      dir_encoder = 1;
    }
    enqueue_and_echo_commands_P(PSTR("M500"));
  }

  void elegir_temp(/* arguments */) {
    enqueue_and_echo_commands_P(PSTR("G28"));
    ajuste_pid = true;
    apagar_error_temp = true;
    lcd_goto_screen(boquilla_manual, true);
  }
  //------------------------------------------------------
  //inicio continuar impresion
  #if(AVISO_DE_CORTE_DE_LUZ)
  int solo_una_vez = 0;
  void restart_home_boton(int valor) {
    //boton de canselar reanudar impresion
    if (valor == 0){
      u8g.setPrintPos(13, 63);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_LUZ_06));
      u8g.setColorIndex(1);
      u8g.drawBitmapP(37,25,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_inicio_b);

      if (LCD_CLICKED) {
        //evita que se ejecute varias veces
        if(solo_una_vez == 0){
          solo_una_vez++;
          //
          card.getnrfilenames();
          card.getWorkDirName();
          //permite acomodar la imprecion al nivel de la base
          imprimir_desde_base = true;
          //ejecuta restart
          restart_gcode("restart.gcode");
        }

      }
    }else{
      u8g.drawBitmapP(37,25,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_inicio_a);
    }
  }

  void restart_posicion_boton(int valor) {
    //continuar con la renudacion de la impresion
    if (valor == 1){
      u8g.setPrintPos(4, 63);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_LUZ_07));
      u8g.setColorIndex(1);
      u8g.drawBitmapP(65,25,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_reanudar_b);

      if (LCD_CLICKED) {
        //evita que se ejecute varias veces
        if(solo_una_vez == 0){
          solo_una_vez++;
          card.getnrfilenames();
          card.getWorkDirName();
          //ejecuta restart
          restart_gcode("restart.gcode");
        }

      }
    }else{
      u8g.drawBitmapP(65,25,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_reanudar_a);
    }
  }

  //se puede seleccionar si seguir imprimiendo desde el ultimo punto guardado
  //o imprimir lo que falta como un archivo nuevo
  void menu_posicion_inicio() {
    //la ultima posicion
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 19);
    u8g.setPrintPos(10, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_LUZ_04));
    u8g.setPrintPos(13, 17);
    lcd_printPGM(PSTR(MSG_LUZ_05));
    u8g.setColorIndex(1);
    //barreras
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 1;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }

    posicion_anterior = 4;

    restart_home_boton(encoderPosition);
    restart_posicion_boton(encoderPosition);
  }

  void restart_not_boton(int valor) {
    //boton de canselar reanudar impresion
    if (valor == 0){
      u8g.setPrintPos(40, 63);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR("Cancelar"));
      u8g.setColorIndex(1);
      u8g.drawBitmapP(37,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_detener_b);
      if (LCD_CLICKED) {
        //save_on_off = false;
        se_estaba_imprimiendo = false;
        enqueue_and_echo_commands_P(PSTR("M500"));
        lcd_return_to_status();
      }
    }else{
      u8g.drawBitmapP(37,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_detener_a);
    }
  }

  //dibuja el boton "ok" del menu corte de luz
  void restart_ok_boton(int valor) {
    if (valor == 1){
      u8g.setPrintPos(37, 63);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_CONTINUAR));
      u8g.setColorIndex(1);

      u8g.drawBitmapP(65,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_reanudar_b);

      if(LCD_CLICKED) lcd_goto_screen(menu_posicion_inicio, true, 1);

    }else u8g.drawBitmapP(65,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_reanudar_a);
  }
  void menu_restart_yes_not() {
    //menu canselar restar o usar restar
    //texto
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 28);
    u8g.setPrintPos(28, 8);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR(MSG_LUZ_01));
    u8g.setPrintPos(19, 17);
    lcd_printPGM(PSTR(MSG_LUZ_02));
    u8g.setPrintPos(16, 26);
    lcd_printPGM(PSTR(MSG_LUZ_03));
    u8g.setColorIndex(1);
    //barreras encoder
    if (encoderPosition > 1 && encoderPosition < 50){
      encoderPosition = 1;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }

    posicion_anterior = 4;

    restart_ok_boton(encoderPosition);
    restart_not_boton(encoderPosition);
  }
  #endif

  void menu_qr(){
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 10);
    u8g.setPrintPos(31, 8);
    u8g.drawBitmapP(49, 17, 4, 29, codigo_qr);

    u8g.setColorIndex(0);
    u8g.setPrintPos(1, 63);
    lcd_printPGM(PSTR(MSG_WEB MSG_STORE));

    u8g.setPrintPos(13, 8);
    lcd_printPGM(PSTR(MSG_STORE_M));

    u8g.setColorIndex(1);

    if(LCD_CLICKED) {
      posicion_anterior = 40;
      lcd_goto_screen(Kuttercraft_menu, true, 8);

    }
  }
  void borrar_estadistica() {
    print_job_counter.data.numberPrints=0;
    print_job_counter.data.completePrints=0;
    print_job_counter.data.printTime=0;
    print_job_counter.data.printer_usage_seconds=0;
    print_job_counter.data.filamentUsed=0;
    numero_de_logro = 0;
    numero_de_logro_= 0;
    ultimo_numeros_de_kilometros = 0;
    numero_ventana_mantenimiento = 0;
    enqueue_and_echo_commands_P(PSTR("M500"));
    lcd_setstatus("Estadistica Borrada");
    lcd_goto_screen(volver_info, true);
  }

  void ir_cambiar_version();
  void salida_de_sistema() {
    lcd_goto_screen(ajustes, true, 15);
  }
  void ir_menu_sistema(){
    if (encoderPosition > 2 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    START_MENU();
    MENU_ITEM(function, MSG_MAIN "              " LCD_STR_UPLEVEL, salida_de_sistema);
    MENU_ITEM(function, "Cambiar Version", ir_cambiar_version);
    MENU_ITEM(function, "Borrar Estadistica",  borrar_estadistica);
    END_MENU();
  }



  //MODELO //MES //AÑO //SERIE
  void cambiar_serie();

  void ir_cambiar_serie() {
    lcd_goto_screen(cambiar_serie,true);
  }

  void cambiar_serie(){
    u8g.drawBox(0, 51, 128, 14);
    u8g.drawBox(0, 0, 128, 10);

    //limites
    if (encoderPosition > 9 && encoderPosition < 80){
      encoderPosition = 9;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }

    //muestra el numero seleccionado
    u8g.setPrintPos(66, 40);
    lcd_print(contenedor_str);

    //textos
    u8g.setPrintPos(22, 40);
    lcd_printPGM(PSTR("Codigo:"));

    u8g.setColorIndex(0);
    u8g.setPrintPos(7, 8);
    lcd_printPGM(PSTR("Ingrese Generacion:"));

    u8g.setPrintPos(54, 61);
    lcd_print(itostr3(int(encoderPosition)));
    u8g.setColorIndex(1);

    //accion
    if(LCD_CLICKED){
      if(strlen(contenedor_str) <= 3){
        //guardado del nuevo numero
        strcat(contenedor_str, itostr3(int(encoderPosition)));//guarda la info en contenedor_str y lo comvierte en char
        removeSpacesInPlace(contenedor_str);//elimina los espacios en blanco

        lcd_goto_screen(ir_cambiar_serie, true, 0);//recarga este menu para ingresar el siguiente numero
      }else{
        //Agrega una separacion "-"
        strcat(numero_de_serie, PSTR("-"));

        //agrega 0 dependiendo de la catidad de digitos
        for (int i = 0; i < 4 - strlen(contenedor_str); i++) {
          strcat(numero_de_serie, PSTR("0"));
        }
        strcat(numero_de_serie, contenedor_str);
        removeSpacesInPlace(numero_de_serie);

        //Salida
        lcd_setstatus(numero_de_serie);
        lcd_goto_screen(volver_info, true);
        //limpiamos
        contenedor_str[0] = '\0';
        //guarda el valor
        enqueue_and_echo_commands_P(PSTR("M500"));
      }
    }
  }

  void cambiar_version_generacion(){
    u8g.drawBox(0, 51, 128, 14);
    u8g.drawBox(0, 0, 128, 10);

    //limites
    if (encoderPosition > 30 && encoderPosition < 80){
      encoderPosition = 30;
    }
    if (encoderPosition > 80){
      encoderPosition = 15;
    }

    u8g.setColorIndex(0);
    u8g.setPrintPos(7, 8);
    lcd_printPGM(PSTR("Ingrese Generacion:"));

    u8g.setPrintPos(54, 61);
    lcd_print(itostr3(int(encoderPosition)));
    u8g.setColorIndex(1);

    //accion
    if(LCD_CLICKED){
      strcpy(contenedor_str, itostr3(int(encoderPosition)));//guarda el numero
      removeSpacesInPlace(contenedor_str);//elimina espacios en blanco

      strcat(numero_de_serie, contenedor_str);
      contenedor_str[0] = '\0';

      lcd_goto_screen(cambiar_serie,true);
    }
  }

  void cambiar_version_mes(){
    u8g.drawBox(0, 51, 128, 14);
    u8g.drawBox(0, 0, 128, 10);

    //limites
    if (encoderPosition > 12 && encoderPosition < 50){
      encoderPosition = 12;
    }
    if (encoderPosition > 50){
      encoderPosition = 1;
    }
    if (encoderPosition == 0){
      encoderPosition = 1;
    }

    u8g.setColorIndex(0);
    u8g.setPrintPos(28, 8);
    lcd_printPGM(PSTR("Ingrese Mes:"));

    u8g.setPrintPos(54, 61);
    lcd_print(itostr3(int(encoderPosition)));
    u8g.setColorIndex(1);

    //accion
    if(LCD_CLICKED){
      strcpy(contenedor_str, itostr3(int(encoderPosition)));
      removeSpacesInPlace(contenedor_str);
      //MSG_TIPO
      strcat(numero_de_serie, PSTR("PK3"));

      if(strlen(contenedor_str) == 1){
        strcat(numero_de_serie, PSTR("0"));
        strcat(numero_de_serie, contenedor_str);
      }else{
        strcat(numero_de_serie, contenedor_str);
      }

      contenedor_str[0] = '\0';
      lcd_goto_screen(cambiar_version_generacion,true,20);
    }
  }

  void ir_cambiar_version(){
    //limpia estos dos contenedores
    numero_de_serie[0] = '\0';
    contenedor_str[0] = '\0';
    lcd_goto_screen(cambiar_version_mes,true,1);
  }
  ////////////////////////////////////////
  void entrar_a_sistema();
  void entrar_a_sistema_1() {
    lcd_goto_screen(entrar_a_sistema, true, 0);
  }

  //void removeSpacesInPlace();
  void entrar_a_sistema() {
    u8g.drawBox(0, 51, 128, 14);
    u8g.drawBox(0, 0, 128, 10);

    //limites
    if (encoderPosition > 11 && encoderPosition < 50){
      encoderPosition = 11;
    }
    if (encoderPosition > 50){
      encoderPosition = 1;
    }
    //letras

    u8g.setPrintPos(66, 40);
    lcd_print(contenedor_str);//el numero completo
    //textos
    u8g.setPrintPos(22, 40);
    lcd_printPGM(PSTR("Codigo:"));//visual
    u8g.setColorIndex(0);
    u8g.setPrintPos(22, 8);
    lcd_printPGM(PSTR("Ingrese Codigo"));//titulo
    u8g.setPrintPos(54, 61);

    if(encoderPosition == 10){
      u8g.setPrintPos(46, 61);
      lcd_printPGM(PSTR("Borrar"));//titulo
    }else if(encoderPosition == 11){
      u8g.setPrintPos(49, 61);
      lcd_printPGM(PSTR("Salir"));//titulo
    }else{
      lcd_print(itostr3(int(encoderPosition)));//numero a elegir
    }

    u8g.setColorIndex(1);

    //accion
    if(LCD_CLICKED){
      if(encoderPosition == 10){
        //pregunta si es menor a 1
        if(strlen(contenedor_str) <= 1){
          contenedor_str[0] = '\0';
          lcd_goto_screen(entrar_a_sistema_1, true, 0);
        }else{
          contenedor_str[strlen(contenedor_str) - 1] = '\0';
          lcd_goto_screen(entrar_a_sistema_1, true, 0);
        }
      }else if(encoderPosition == 11){
        lcd_goto_screen(ajustes, true, 15);
      }else{
        if(strlen(contenedor_str) <= 5){
          //guardado del nuevo numero
          strcat(contenedor_str, itostr3(int(encoderPosition)));//guarda la info en contenedor_str y lo comvierte en char
          removeSpacesInPlace(contenedor_str);//elimina los espacios en blanco
          //recurcion
          lcd_goto_screen(entrar_a_sistema_1, true, 0);
        }else{
          //pregunta si esta bien la contraseña
          if(strcmp(contenedor_str, "011990") == 0){
            //resetea y sigue
            contenedor_str[0] = '\0';
            lcd_goto_screen(ir_menu_sistema,true);
          }else{
            lcd_setstatus("Codigo Incorrecto");
            lcd_goto_screen(volver_info, true);
          }
        }
      }

    }
  }

  void ir_entrar_a_sistema(){
    contenedor_str[0] = '\0';
    lcd_goto_screen(entrar_a_sistema, true);
  }
  uint16_t day, hours, minutes;
  millis_t t;

  void estadisticas_filamento() {
    u8g.drawBox(0, 0, 128, 10);
    u8g.drawBox(0, 54, 128, 10);
    //Titulo
    u8g.setColorIndex(0);
    u8g.setPrintPos(19, 8);
    lcd_printPGM(PSTR("Filamento Usado"));
    u8g.setColorIndex(1);
    //Informacion
    u8g.setPrintPos(10, 22);
    lcd_printPGM(PSTR("Kilometros:"));
    u8g.setPrintPos(34, 35);
    lcd_printPGM(PSTR("Metros:"));
    u8g.setPrintPos(4, 48);
    lcd_printPGM(PSTR("Centimetros:"));
    //se obtiene los valores a imprimir

    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7((long)print_job_counter.data.filamentUsed / 1000 / 1000));//guarda la info en contenedor_str y lo comvierte en char
    removeSpacesInPlace(contenedor_str);//elimina los espacios en blanco

    u8g.setPrintPos(76, 22);
    lcd_print(contenedor_str);//impreme la variable
    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(((long)print_job_counter.data.filamentUsed / 1000) % 1000));
    removeSpacesInPlace(contenedor_str);

    u8g.setPrintPos(76, 35);
    lcd_print(contenedor_str);
    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(((long)print_job_counter.data.filamentUsed / 10) % 100));
    removeSpacesInPlace(contenedor_str);

    u8g.setPrintPos(76, 48);
    lcd_print(contenedor_str);
    /////////////////////////////////////////////////////
    if(LCD_CLICKED){
      lcd_goto_screen(ajustes, true, 2);
    }

  }

  void estadisticas_tiempo_prendida() {
    u8g.drawBox(0, 0, 128, 10);
    u8g.drawBox(0, 54, 128, 10);
    //Titulo
    u8g.setColorIndex(0);
    u8g.setPrintPos(19, 8);
    lcd_printPGM(PSTR("Tiempo Encedida"));
    u8g.setColorIndex(1);
    //Informacion
    u8g.setPrintPos(46, 22);
    lcd_printPGM(PSTR("Dias:"));
    u8g.setPrintPos(40, 35);
    lcd_printPGM(PSTR("Horas:"));
    u8g.setPrintPos(28, 48);
    lcd_printPGM(PSTR("Minutos:"));
    //se obtiene los valores a imprimir
    t       = print_job_counter.data.printer_usage_seconds / 60;
    day     = t / 60 / 24;
    hours   = (t / 60) % 24;
    minutes = t % 60;

    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(day));//guarda la info en contenedor_str y lo comvierte en char
    removeSpacesInPlace(contenedor_str);//elimina los espacios en blanco

    u8g.setPrintPos(76, 22);
    lcd_print(contenedor_str);//impreme la variable
    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(hours));
    removeSpacesInPlace(contenedor_str);

    u8g.setPrintPos(76, 35);
    lcd_print(contenedor_str);
    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(minutes));
    removeSpacesInPlace(contenedor_str);

    u8g.setPrintPos(76, 48);
    lcd_print(contenedor_str);
    /////////////////////////////////////////////////////
    if(LCD_CLICKED){
      lcd_goto_screen(estadisticas_filamento, true);
    }

  }

  void estadisticas_tiempo_imprimiendo() {
    u8g.drawBox(0, 0, 128, 10);
    u8g.drawBox(0, 54, 128, 10);
    //Titulo
    u8g.setColorIndex(0);
    u8g.setPrintPos(7, 8);
    lcd_printPGM(PSTR("Tiempo de impresion"));
    u8g.setColorIndex(1);
    //Informacion
    u8g.setPrintPos(46, 22);
    lcd_printPGM(PSTR("Dias:"));
    u8g.setPrintPos(40, 35);
    lcd_printPGM(PSTR("Horas:"));
    u8g.setPrintPos(28, 48);
    lcd_printPGM(PSTR("Minutos:"));
    //se obtiene los valores a imprimir
    t       = print_job_counter.data.printTime / 60;
    day     = t / 60 / 24;
    hours   = (t / 60) % 24;
    minutes = t % 60;

    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(day));//guarda la info en contenedor_str y lo comvierte en char
    removeSpacesInPlace(contenedor_str);//elimina los espacios en blanco

    u8g.setPrintPos(76, 22);
    lcd_print(contenedor_str);//impreme la variable
    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(hours));
    removeSpacesInPlace(contenedor_str);

    u8g.setPrintPos(76, 35);
    lcd_print(contenedor_str);
    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(minutes));
    removeSpacesInPlace(contenedor_str);

    u8g.setPrintPos(76, 48);
    lcd_print(contenedor_str);
    /////////////////////////////////////////////////////
    if(LCD_CLICKED){
      lcd_goto_screen(estadisticas_tiempo_prendida, true);
    }
  }

  void estadisticas_impreciones() {
    u8g.drawBox(0, 0, 128, 10);
    u8g.drawBox(0, 54, 128, 10);
    //Titulo
    u8g.setColorIndex(0);
    u8g.setPrintPos(31, 8);
    lcd_printPGM(PSTR(MSG_EST_IMPRECION));
    u8g.setColorIndex(1);
    //Informacion
    u8g.setPrintPos(34, 22);
    lcd_printPGM(PSTR(" "MSG_EST_TOTAL));
    u8g.setPrintPos(10, 35);
    lcd_printPGM(PSTR(MSG_EST_TERMINADAS));
    u8g.setPrintPos(4, 48);
    lcd_printPGM(PSTR(MSG_EST_INCOMPLETAS));

    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(print_job_counter.data.numberPrints));//guarda la info en contenedor_str y lo comvierte en char
    removeSpacesInPlace(contenedor_str);//elimina los espacios en blanco

    u8g.setPrintPos(77, 22);
    lcd_print(contenedor_str);//impreme la variable
    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(print_job_counter.data.completePrints));
    removeSpacesInPlace(contenedor_str);

    u8g.setPrintPos(77, 35);
    lcd_print(contenedor_str);
    /////////////////////////////////////////////////////
    strcpy(contenedor_str, ltostr7(print_job_counter.data.numberPrints - print_job_counter.data.completePrints));
    removeSpacesInPlace(contenedor_str);

    u8g.setPrintPos(77, 48);
    lcd_print(contenedor_str);
    /////////////////////////////////////////////////////
    if(LCD_CLICKED){
      lcd_goto_screen(estadisticas_tiempo_imprimiendo, true);
    }

  }

  void enviar_gcode_todo();
  void recurcion_enviar_gcode_todo() {
    lcd_goto_screen(enviar_gcode_todo, true,0);
  }
  void enviar_gcode_m_g();
  char auxiliar_str[3];
  void enviar_gcode_todo() {
    bool salida_esperada = false;
    u8g.drawBox(0, 51, 128, 14);
    u8g.drawBox(0, 0, 128, 10);

    //limites
    if (encoderPosition > 22 && encoderPosition < 50){
      encoderPosition = 22;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //letras

    u8g.setPrintPos(1, 39);
    lcd_print(contenedor_str);//el numero completo
    //textos
    u8g.setPrintPos(43, 26);
    lcd_printPGM(PSTR("Comando:"));//visual
    u8g.setColorIndex(0);
    u8g.setPrintPos(22, 8);
    lcd_printPGM(PSTR("Ingrese Codigo"));//titulo
    u8g.setPrintPos(61, 61);

    //muestra el caracter selecionado M,G y salir
    switch (encoderPosition) {
      case 10:
        lcd_printPGM(PSTR("."));
        break;
      case 11:
        lcd_printPGM(PSTR("X"));
        break;
      case 12:
        lcd_printPGM(PSTR("Y"));
        break;
      case 13:
        lcd_printPGM(PSTR("Z"));
        break;
      case 14:
        lcd_printPGM(PSTR("E"));
        break;
      case 15:
        lcd_printPGM(PSTR("F"));
        break;
      case 16:
        lcd_printPGM(PSTR("S"));
        break;
      case 17:
        lcd_printPGM(PSTR("P"));
        break;
      case 18:
        lcd_printPGM(PSTR("C"));
        break;
      case 19:
        u8g.setPrintPos(43, 61);
        lcd_printPGM(PSTR("Espacio"));
        break;
      case 20:
        u8g.setPrintPos(46, 61);
        lcd_printPGM(PSTR("Enviar"));
        break;
      case 21:
        u8g.setPrintPos(46, 61);
        lcd_printPGM(PSTR("Borrar"));
        break;
      case 22:
        u8g.setPrintPos(49, 61);
        lcd_printPGM(PSTR("Salir"));
        break;
      default:
        //numeros el 0 al 9
        u8g.setPrintPos(49, 61);
        lcd_print(itostr3(int(encoderPosition)));
        break;
    }
    if(LCD_CLICKED){
      switch (encoderPosition) {
        case 10:
          salida_esperada = true;
          strcat(contenedor_str,PSTR("."));
          break;
        case 11:
          salida_esperada = true;
          strcat(contenedor_str,PSTR("X"));
          break;
        case 12:
          salida_esperada = true;
          strcat(contenedor_str,PSTR("Y"));
          break;
        case 13:
          salida_esperada = true;
          strcat(contenedor_str,PSTR("Z"));
          break;
        case 14:
          salida_esperada = true;
          strcat(contenedor_str,PSTR("E"));
          break;
        case 15:
          salida_esperada = true;
          strcat(contenedor_str,PSTR("F"));
          break;
        case 16:
          salida_esperada = true;
          strcat(contenedor_str,PSTR("S"));
          break;
        case 17:
          salida_esperada = true;
          strcat(contenedor_str,PSTR("P"));
          break;
        case 18:
          salida_esperada = true;
          strcat(contenedor_str,PSTR("C"));
          break;
        case 19:
          salida_esperada = true;
          strcat(contenedor_str,PSTR(" "));
          break;
        case 20:
          enqueue_and_echo_command(contenedor_str);
          lcd_goto_screen(ajustes, true, 14);
          break;
        case 21:
          //BORRAR
          if(strlen(contenedor_str) <= 1){
            contenedor_str[0] = '\0';
            lcd_goto_screen(enviar_gcode_m_g, true, 0);
          }else{
            contenedor_str[strlen(contenedor_str) - 1] = '\0';
            salida_esperada = true;
          }
          break;
        case 22:
          lcd_goto_screen(ajustes, true, 14);
          break;
        default:
          auxiliar_str[0] = '\0';
          salida_esperada = true;
          strcat(auxiliar_str, itostr3(int(encoderPosition)));
          removeSpacesInPlace(auxiliar_str);
          strcat(contenedor_str, auxiliar_str);
          break;
      }
      if(salida_esperada){
        lcd_goto_screen(recurcion_enviar_gcode_todo, true);
      }
    }

    u8g.setColorIndex(1);

  }
  //obtenido logro
  void enviar_gcode_m_g(){
    u8g.drawBox(0, 51, 128, 14);
    u8g.drawBox(0, 0, 128, 10);

    //limites
    if (encoderPosition > 2 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //letras

    u8g.setPrintPos(1, 39);
    lcd_print(contenedor_str);//el numero completo
    //textos
    u8g.setPrintPos(43, 26);
    lcd_printPGM(PSTR("Comando:"));//visual
    u8g.setColorIndex(0);
    u8g.setPrintPos(22, 8);
    lcd_printPGM(PSTR("Ingrese Codigo"));//titulo
    u8g.setPrintPos(61, 61);

    //muestra el caracter selecionado M,G y salir
    switch (encoderPosition) {
      case 0:
        lcd_printPGM(PSTR("G"));
        break;
      case 1:
        lcd_printPGM(PSTR("M"));
        break;
      case 2:
        u8g.setPrintPos(49, 61);
        lcd_printPGM(PSTR("Salir"));
        break;
    }
    u8g.setColorIndex(1);
    if(LCD_CLICKED){
      switch (encoderPosition) {
        case 0:
          strcat(contenedor_str,PSTR("G"));
          lcd_goto_screen(enviar_gcode_todo, true);
          break;
        case 1:
          strcat(contenedor_str,PSTR("M"));
          lcd_goto_screen(enviar_gcode_todo, true);
          break;
        case 2:
          lcd_goto_screen(ajustes, true, 14);
          break;
      }
    }
  }
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////
  void mantenimiento() {
    defer_return_to_status = true;
    u8g.drawBox(0, 54, 128, 11);
    u8g.drawBox(0, 0, 128, 10);
    //Aviso mantenimiento
    //recuerde revisar
    //mantenimiento_ico
    u8g.drawBitmapP(53, 17, 3, 22,mantenimiento_ico);

    u8g.setPrintPos(16, 51);
    lcd_printPGM(PSTR("Recuerde Revisar"));

    u8g.setColorIndex(0);
    u8g.setPrintPos(7, 8);
    lcd_printPGM(PSTR("Aviso Mantenimiento"));

    switch (numero_de_matenimiento) {
      case 0:
        u8g.setPrintPos(13, 62);
        lcd_printPGM(PSTR("Boquilla y Barrel"));//50km
        break;
      case 1:
        u8g.setPrintPos(31, 62);
        lcd_printPGM(PSTR("Lubricacion"));//101km
        break;
      case 2:
        u8g.setPrintPos(31, 62);
        lcd_printPGM(PSTR("Correas GT2"));//200km
        break;
      case 3:
        u8g.setPrintPos(4, 62);
        lcd_printPGM(PSTR("Estado de Los Cables"));//299km
        break;
    }
    u8g.setColorIndex(1);
    if (LCD_CLICKED) {
      defer_return_to_status = false;
      lcd_return_to_status();
    }
  }

  void logros();
  void logros_piezas();

  void ver_ventana_logros_cantidad(){
    bool bool_logros = false;

    if(print_job_counter.data.completePrints >= 2015){
      if(numero_de_logro_ == 11){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 1984){
      if(numero_de_logro_ == 10){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 1492){
      if(numero_de_logro_ == 9){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 1024){
      if(numero_de_logro_ == 8){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 512){
      if(numero_de_logro_ == 7){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 314){
      if(numero_de_logro_ == 6){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 138){
      if(numero_de_logro_ == 5){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 101){
      if(numero_de_logro_ == 4){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 64){
      if(numero_de_logro_ == 3){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 42){
      if(numero_de_logro_ == 2){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 27){
      if(numero_de_logro_ == 1){
        bool_logros = true;
      }
    }else if(print_job_counter.data.completePrints >= 10){
      if(numero_de_logro_ == 0){
        bool_logros = true;
      }
    }

    if(bool_logros){
      bool_logros = false;
      se_permite_el_movimineto = false;
      numero_de_logro_ += 1;
      sonido_final();
      lcd_goto_screen(logros_piezas, true);
      Config_StoreSettings();
    }
  }


  void ver_ventana_logros(){
    bool bool_mantenimiento = false;
    bool bool_logro = false;
    bool bool_de_error = false;
    long numeros_de_kilometros = (long)print_job_counter.data.filamentUsed / 1000 / 1000;

    if(numeros_de_kilometros >= 1){
      bool_de_error =true;
    }

    if(numeros_de_kilometros == 0 || numero_de_logro == 0){
      //pasa el numero a metros
      numeros_de_kilometros = ((long)print_job_counter.data.filamentUsed / 1000) % 1000;
      //preguta si es mayor a 100m
      if(numeros_de_kilometros >= 100 || bool_de_error){
        //y si es el primer logro
        if(numero_de_logro == 0){
          bool_logro = true;
        }
      }
    //Es mayor a metros
    }else{
      //se encarga de pregutar si se tiene que ejecutar el cartel de mantenimiento
      //motores
      if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 320){
        if(numero_ventana_mantenimiento == 9){
          numero_de_matenimiento = 3;
          lcd_goto_screen(mantenimiento, true);
          ultimo_numeros_de_kilometros = numeros_de_kilometros;
          numero_ventana_mantenimiento = 0;
          sonido_final();
          Config_StoreSettings();
        }
      //boquilla
      }else if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 300){
        if(numero_ventana_mantenimiento == 8){
          bool_mantenimiento = true;
          numero_de_matenimiento = 0;
          lcd_goto_screen(mantenimiento, true);
        }
      //lubricacion
      }else if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 270){
          if(numero_ventana_mantenimiento == 7){
            bool_mantenimiento = true;
            numero_de_matenimiento = 1;
            lcd_goto_screen(mantenimiento, true);
          }
    //boquilla
      }else if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 240){
          if(numero_ventana_mantenimiento == 6){
            bool_mantenimiento = true;
            numero_de_matenimiento = 0;
            lcd_goto_screen(mantenimiento, true);
          }
      //correa
      }else if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 210){
          if(numero_ventana_mantenimiento == 5){
            bool_mantenimiento = true;
            numero_de_matenimiento = 2;
            lcd_goto_screen(mantenimiento, true);
          }
      //boquilla
      }else if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 180){
          if(numero_ventana_mantenimiento == 4){
            bool_mantenimiento = true;
            numero_de_matenimiento = 0;
            lcd_goto_screen(mantenimiento, true, 0);
          }
      //lubricacion
      }else if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 150){
          if(numero_ventana_mantenimiento == 3){
            bool_mantenimiento = true;
            numero_de_matenimiento = 1;
            lcd_goto_screen(mantenimiento, true);
          }
      //boquilla
      }else if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 120){
        if(numero_ventana_mantenimiento == 2){
          bool_mantenimiento = true;
          numero_de_matenimiento = 0;
          lcd_goto_screen(mantenimiento, true);
        }
      //lubricacion
      }else if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 90){
        if(numero_ventana_mantenimiento == 1){
          bool_mantenimiento = true;
          numero_de_matenimiento = 1;
          lcd_goto_screen(mantenimiento, true);
        }
      //boquilla
      }else if(numeros_de_kilometros - ultimo_numeros_de_kilometros >= 60){
        if(numero_ventana_mantenimiento == 0){
          bool_mantenimiento = true;
          numero_de_matenimiento = 0;
          lcd_goto_screen(mantenimiento, true);
        }
      }

      //pregunta se se llego algun logro
      if(numeros_de_kilometros >= 12756){
        if(numero_de_logro == 22){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 11812){
        if(numero_de_logro == 21){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 5038){
        if(numero_de_logro == 20){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 3700){
        if(numero_de_logro == 19){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 2472){
        if(numero_de_logro == 18){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 1605){
        if(numero_de_logro == 17){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 1531){
        if(numero_de_logro == 16){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 1427){
        if(numero_de_logro == 15){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 1105){
        if(numero_de_logro == 14){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 1058){
        if(numero_de_logro == 13){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 914){
        if(numero_de_logro == 12){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 745){
        if(numero_de_logro == 11){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 621){
        if(numero_de_logro == 10){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 505){
        if(numero_de_logro == 9){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 474){
        if(numero_de_logro == 8){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 362){
        if(numero_de_logro == 7){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 311){
        if(numero_de_logro == 6){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 148){
        if(numero_de_logro == 5){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 107){
        if(numero_de_logro == 4){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 53){
        if(numero_de_logro == 3){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 12){
        if(numero_de_logro == 2){
          bool_logro = true;
        }
      }else if(numeros_de_kilometros >= 1){
        if(numero_de_logro == 1){
          bool_logro = true;
        }
      }
    }
    if(bool_mantenimiento){
      sonido_final();
      numero_ventana_mantenimiento++;
      Config_StoreSettings();
    }
    //abre la ventana de logros
    if(bool_logro){
      bool_logro = false;
      se_permite_el_movimineto = false;
      numero_de_logro += 1;
      sonido_final();
      lcd_goto_screen(logros, true);
      Config_StoreSettings();
    }
  }
  void menu_logros();

  void ticket() {
    u8g.drawBox(0, 56, 128, 8);
    u8g.drawBox(36, 1, 57, 1);
    u8g.drawBox(36, 4, 57, 1);
    u8g.drawBox(36, 23, 57, 1);
    u8g.drawBox(36, 26, 57, 1);
    //x, y, columnas, filas, mapa
    u8g.drawBitmapP(27, 1, 2, 26,ticket_izq);
    u8g.drawBitmapP(93, 1, 2, 26,ticket_der);

    u8g.setPrintPos(40, 13);
    lcd_printPGM(PSTR("Cupon De"));

    u8g.setPrintPos(38, 22);
    lcd_printPGM(PSTR("Descuento"));
    u8g.setFont(u8g_font_5x8);

    //pk31245-1245
    u8g.setPrintPos(34, 34);
    lcd_print(numero_de_serie);



    u8g.setColorIndex(0);
    u8g.setPrintPos(11, 63);
    //kuttercraft.com/legal
    lcd_printPGM(PSTR(MSG_WEB "/promo"));//
    u8g.setColorIndex(1);


    u8g.setPrintPos(16, 55);
    lcd_printPGM(PSTR("Ver Condiciones En:"));

    //u8g.setPrintPos(21, 48);
    u8g.setPrintPos(29, 41);
    lcd_printPGM(PSTR("Descuento: "));


    switch (numero_de_ticket) {
      case 1:
        lcd_printPGM(PSTR("TEST"));
        u8g.setPrintPos(24, 48);
        lcd_printPGM(PSTR("10% "MSG_LOGROS_FILAMENTO));//16
        break;

      case 2:
        lcd_printPGM(PSTR("AWSD"));
        u8g.setPrintPos(21, 48);
        lcd_printPGM(PSTR("15% "MSG_LOGROS_RESPUESTOS));//17
        break;

      case 3:
        lcd_printPGM(PSTR("EDBA"));
        u8g.setPrintPos(24, 48);
        lcd_printPGM(PSTR("10% "MSG_LOGROS_FILAMENTO));
        break;

      case 4:
        lcd_printPGM(PSTR("BSAS"));
        u8g.setPrintPos(19, 48);
        lcd_printPGM(PSTR("Un Envio Gratis!!!"));//18
        break;

      case 5:
        lcd_printPGM(PSTR("APEX"));
        u8g.setPrintPos(26, 48);
        lcd_printPGM(PSTR(" 5% "MSG_LOGROS_MAQUINAS));//15
        break;

      case 6:
        lcd_printPGM(PSTR("BOOM"));
        u8g.setPrintPos(29, 48);
        lcd_printPGM(PSTR("25% En GriloN3"));
        break;

      case 7:
        lcd_printPGM(PSTR("BOLO"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("10% "MSG_LOGROS_TODO));
        break;

      case 8:
        lcd_printPGM(PSTR("COSA"));
        u8g.setPrintPos(29, 48);
        lcd_printPGM(PSTR("15% "MSG_LOGROS_INSUMOS));
        break;

      case 9:
        lcd_printPGM(PSTR("DIAL"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("10% "MSG_LOGROS_TODO));
        break;

      case 10:
        lcd_printPGM(PSTR("DATA"));
        u8g.setPrintPos(14, 48);
        lcd_printPGM(PSTR("50% "MSG_LOGROS_SERVI));
        break;

      case 11:
        lcd_printPGM(PSTR("DATO"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("15% "MSG_LOGROS_TODO));
        break;

      case 12:
        lcd_printPGM(PSTR("DEAL"));
        u8g.setPrintPos(26, 48);
        lcd_printPGM(PSTR("15% "MSG_LOGROS_MAQUINAS));
        break;

      case 13:
        lcd_printPGM(PSTR("FARO"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("15% "MSG_LOGROS_TODO));
        break;

      case 14:
        lcd_printPGM(PSTR("FORO"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("20% "MSG_LOGROS_TODO));
        break;

      case 15:
        lcd_printPGM(PSTR("COPO"));
        u8g.setPrintPos(26, 48);
        lcd_printPGM(PSTR("15% "MSG_LOGROS_MAQUINAS));
        break;

      case 16:
        lcd_printPGM(PSTR("CAPO"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("20% "MSG_LOGROS_TODO));
        break;

      case 17:
        lcd_printPGM(PSTR("AAHH"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("20% "MSG_LOGROS_TODO));
        break;

      case 18:
        lcd_printPGM(PSTR("AULA"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("20% "MSG_LOGROS_TODO));
        break;

      case 19:
        lcd_printPGM(PSTR("AUWW"));
        u8g.setPrintPos(26, 48);
        lcd_printPGM(PSTR("20% "MSG_LOGROS_MAQUINAS));
        break;

      case 20:
        lcd_printPGM(PSTR("XDXD"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("25% "MSG_LOGROS_TODO));
        break;

      case 21:
        lcd_printPGM(PSTR("XPXS"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("25% "MSG_LOGROS_TODO));
        break;

      case 22:
        lcd_printPGM(PSTR("IOSX"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("25% "MSG_LOGROS_TODO));
        break;

      case 23:
        lcd_printPGM(PSTR("DIOS"));
        u8g.setPrintPos(11, 48);
        lcd_printPGM(PSTR("30% "MSG_LOGROS_TODO));
        break;
      //////////////////////////////////////////////////////////////
      //10 PIEZAS
      case 101:
        lcd_printPGM(PSTR("AVRG"));
        //10 DESFUENTO EN FILA
        u8g.setPrintPos(24, 48);
        lcd_printPGM(PSTR("10% "MSG_LOGROS_FILAMENTO));
        break;
      //27 PIEZAS
      case 102:
        lcd_printPGM(PSTR("RFFF"));
        //10 DESFUENTO EN REPURTOS
        u8g.setPrintPos(21, 48);
        lcd_printPGM(PSTR("10% "MSG_LOGROS_RESPUESTOS));
        break;
      //42 PIEZAS
      case 103:
        lcd_printPGM(PSTR("TXTS"));
        //12 DESFUENTO EN FILA
        u8g.setPrintPos(24, 48);
        lcd_printPGM(PSTR("12% "MSG_LOGROS_FILAMENTO));
        break;
      //64 PIEZAS
      case 104:
        lcd_printPGM(PSTR("CADT"));
        //12 DESFUENTO EN REPURTOS
        u8g.setPrintPos(21, 48);
        lcd_printPGM(PSTR("12% "MSG_LOGROS_RESPUESTOS));
        break;
      //101 PIEZAS
      case 105:
        lcd_printPGM(PSTR("BUUH"));
        //15 DESFUENTO EN FILA
        u8g.setPrintPos(24, 48);
        lcd_printPGM(PSTR("15% "MSG_LOGROS_FILAMENTO));
        break;
      //138 PIEZAS
      case 106:
        lcd_printPGM(PSTR("HOHO"));
        //15 REPURTOS
        u8g.setPrintPos(21, 48);
        lcd_printPGM(PSTR("15% "MSG_LOGROS_RESPUESTOS));
        break;
      //314 PIEZAS
      case 107:
        lcd_printPGM(PSTR("LOGO"));
        // 31.4 BOQULLAS
        u8g.setPrintPos(19, 48);
        lcd_printPGM(PSTR("31.4% En Boquillas"));
        break;
      //512 PIEZAS
      case 108:
        lcd_printPGM(PSTR("MIRA"));
        //20 DESFUENTO EN REPURTOS
        u8g.setPrintPos(21, 48);
        lcd_printPGM(PSTR("20% "MSG_LOGROS_RESPUESTOS));
        break;
      //1024 PIEZAS
      case 109:
        lcd_printPGM(PSTR("OUEL"));
        //20 DESFUENTO EN REPURTOS
        u8g.setPrintPos(21, 48);
        lcd_printPGM(PSTR("20% "MSG_LOGROS_RESPUESTOS));
        break;
      //1492 PIEZAS
      case 110:
        lcd_printPGM(PSTR("HNWP"));
        //10 MAQUINA
        u8g.setPrintPos(26, 48);
        lcd_printPGM(PSTR("10% "MSG_LOGROS_MAQUINAS));
        break;
      //1984 PIEZAS
      case 111:
        lcd_printPGM(PSTR("FURT"));
        //20 DESFUENTO EN REPURTOS
        u8g.setPrintPos(21, 48);
        lcd_printPGM(PSTR("20% "MSG_LOGROS_RESPUESTOS));
        break;
      //2015 PIEZAS
      case 112:
        lcd_printPGM(PSTR("INIT"));
        //15 EN MAQUINAS 2015
        u8g.setPrintPos(26, 48);
        lcd_printPGM(PSTR("15% "MSG_LOGROS_MAQUINAS));
        break;
    }

    if (LCD_CLICKED) {
      defer_return_to_status = false;
      if(tipo_de_logro){
        lcd_goto_screen(menu_logros, true,2);
      }else{
        lcd_goto_screen(menu_logros, true,1);
      }
    }
  }

  void logros_piezas(/* arguments */) {
    //variable auxiliar para logros
    int aux_numero_de_logro = numero_de_logro_;

    u8g.drawBox(0, 55, 128, 9);
    u8g.drawBox(26, 17, 76, 1);
    u8g.drawBox(26, 27, 76, 1);

    //x, y, columnas, filas, mapa
    u8g.drawBitmapP(12, 17, 2, 15,bandera_izq);
    u8g.drawBitmapP(102, 17, 2, 15,bandera_der);
    //estrella
    u8g.drawBitmapP(54, 0, 3, 18, estrella);
    u8g.drawBitmapP(35, 5, 3, 18, estrella);
    u8g.drawBitmapP(73, 5, 3, 18, estrella);

    u8g.setColorIndex(0);
    u8g.drawBox(20, 18, 88, 9);
    u8g.setColorIndex(1);

    u8g.setPrintPos(22, 26);
    lcd_printPGM(PSTR("Logro Obtenido"));

    u8g.setPrintPos(19, 41);
    lcd_printPGM(PSTR("Haz Impreso Tus"));

    //limites
    if (encoderPosition > numero_de_logro_ && encoderPosition < 50){
      encoderPosition = numero_de_logro_;
    }
    if (encoderPosition > 50){
      encoderPosition = 1;
    }
    if (encoderPosition == 0){
      encoderPosition = 1;
    }

    if(se_permite_el_movimineto){
      aux_numero_de_logro = encoderPosition;
    }else{
      defer_return_to_status = true;
    }

    numero_de_ticket = aux_numero_de_logro + 100;

    switch (aux_numero_de_logro) {
      case 1:
        u8g.setPrintPos(7, 51);
        lcd_printPGM(PSTR("Primeras 10 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(13, 63);
        lcd_printPGM(PSTR("Felicitaciones!!!"));

        break;

      case 2:
        u8g.setPrintPos(7, 51);
        lcd_printPGM(PSTR("Primeras 27 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(10, 63);
        lcd_printPGM(PSTR("Impresor Constante"));
        break;

      case 3:
        u8g.setPrintPos(7, 51);
        lcd_printPGM(PSTR("Primeras 42 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(7, 63);
        lcd_printPGM(PSTR("Impresor de la vida"));
        break;

      case 4:
        u8g.setPrintPos(7, 51);
        lcd_printPGM(PSTR("Primeras 64 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(1, 63);
        lcd_printPGM(PSTR("Impresor de poligonos"));
        break;

      case 5:
        u8g.setPrintPos(4, 51);
        lcd_printPGM(PSTR("Primeras 101 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(7, 63);
        lcd_printPGM(PSTR("Impresor de manchas"));
        break;

      //138
      case 6:
        u8g.setPrintPos(4, 51);
        lcd_printPGM(PSTR("Primeras 138 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(25, 63);
        lcd_printPGM(PSTR("Espectacular!"));
        break;

      case 7:
        u8g.setPrintPos(4, 51);
        lcd_printPGM(PSTR("Primeras 314 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(10, 63);
        lcd_printPGM(PSTR("Impresor de Tartas"));
        break;

      case 8:
        u8g.setPrintPos(4, 51);
        lcd_printPGM(PSTR("Primeras 512 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(28, 63);
        lcd_printPGM(PSTR("Mini Fabrica"));
        break;

      case 9:
        u8g.setPrintPos(1, 51);
        lcd_printPGM(PSTR("Primeras 1024 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(1, 63);
        lcd_printPGM(PSTR("Un megabytes Impresos"));
        break;

      case 10:
        u8g.setPrintPos(1, 51);
        lcd_printPGM(PSTR("Primeras 1492 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(16, 63);
        lcd_printPGM(PSTR("Impresor Moderno"));
        break;

      case 11:
        u8g.setPrintPos(1, 51);
        lcd_printPGM(PSTR("Primeras 1984 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(7, 63);
        lcd_printPGM(PSTR("Impresor del futuro"));
        break;

      case 12:
        u8g.setPrintPos(1, 51);
        lcd_printPGM(PSTR("Primeras 2015 Piezas!"));

        u8g.setColorIndex(0);
        u8g.setPrintPos(1, 63);
        lcd_printPGM(PSTR("Se Funda Kuttercraft!"));
        break;
    }
    if (LCD_CLICKED) {
      lcd_goto_screen(ticket, true);
    }
  }

  void logros() {
    //variable auxiliar para logros
    int aux_numero_de_logro = numero_de_logro;

    u8g.drawBox(0, 55, 128, 9);
    u8g.drawBox(26, 17, 76, 1);
    u8g.drawBox(26, 27, 76, 1);

    //x, y, columnas, filas, mapa
    u8g.drawBitmapP(12, 17, 2, 15,bandera_izq);
    u8g.drawBitmapP(102, 17, 2, 15,bandera_der);
    //estrella
    u8g.drawBitmapP(54, 0, 3, 18, estrella);
    u8g.drawBitmapP(35, 5, 3, 18, estrella);
    u8g.drawBitmapP(73, 5, 3, 18, estrella);

    u8g.setColorIndex(0);
    u8g.drawBox(20, 18, 88, 9);
    u8g.setPrintPos(13, 63);
    lcd_printPGM(PSTR("Felicitaciones!!!"));
    u8g.setColorIndex(1);

    u8g.setPrintPos(22, 26);
    lcd_printPGM(PSTR("Logro Obtenido"));

    //limites
    if (encoderPosition > numero_de_logro && encoderPosition < 50){
      encoderPosition = numero_de_logro;
    }
    if (encoderPosition > 50){
      encoderPosition = 1;
    }
    if (encoderPosition == 0){
      encoderPosition = 1;
    }

    if(se_permite_el_movimineto){
      aux_numero_de_logro = encoderPosition;
    }else{
      defer_return_to_status = true;
    }

    numero_de_ticket = aux_numero_de_logro;

    switch (aux_numero_de_logro) {

      case 1:
        u8g.setPrintPos(22, 41);
        lcd_printPGM(PSTR("Primeros Pasos"));

        u8g.setPrintPos(16, 51);
        lcd_printPGM(PSTR("Haz Impreso 100m"));
        break;

      case 2:
        u8g.setPrintPos(10, 41);
        lcd_printPGM(PSTR("Calentando Motores"));

        u8g.setPrintPos(19, 51);
        lcd_printPGM(PSTR("Haz Impreso 1km"));
        break;

      case 3:
        u8g.setPrintPos(1, 41);
        lcd_printPGM(PSTR("De Lanus al Obelisco!"));

        u8g.setPrintPos(16, 51);
        lcd_printPGM(PSTR("Haz Impreso 12Km"));
        break;

      case 4:
        u8g.setPrintPos(7, 41);
        lcd_printPGM(PSTR("De CABA a La Plata!"));

        u8g.setPrintPos(16, 51);
        lcd_printPGM(PSTR("Haz Impreso 53km"));
        break;

      case 5:
        u8g.setPrintPos(1, 41);
        lcd_printPGM(PSTR("Camino De Los 7 Lagos"));

        u8g.setPrintPos(13, 51);
        lcd_printPGM(PSTR("Haz Impreso 107km"));
        break;

      case 6:
        u8g.setPrintPos(1, 41);
        lcd_printPGM(PSTR("De Lanus a Chivilcoy!"));

        u8g.setPrintPos(13, 51);
        lcd_printPGM(PSTR("Haz Impreso 148km"));
        break;

      case 7:
        u8g.setPrintPos(4, 41);
        lcd_printPGM(PSTR("De Colonia a Rosario"));

        u8g.setPrintPos(13, 51);
        lcd_printPGM(PSTR("Haz Impreso 311km"));
        break;

      case 8:
        u8g.setPrintPos(4, 41);
        lcd_printPGM(PSTR("De La Habana a Miami"));

        u8g.setPrintPos(13, 51);
        lcd_printPGM(PSTR("Haz Impreso 362km"));
        break;

      case 9:
        u8g.setPrintPos(16, 41);
        lcd_printPGM(PSTR("De Iguazu a Ctes"));

        u8g.setPrintPos(13, 51);
        lcd_printPGM(PSTR("Haz Impreso 474km"));
        break;

      case 10:
        u8g.setPrintPos(7, 41);
        lcd_printPGM(PSTR("Madrid a Barcelona!"));

        u8g.setPrintPos(13, 51);
        lcd_printPGM(PSTR("Haz Impreso 505km"));
        break;

      case 11:
        u8g.setPrintPos(1, 41);
        lcd_printPGM(PSTR("De Fortaleza a Recife"));

        u8g.setPrintPos(13, 51);
        lcd_printPGM(PSTR("Haz Impreso 621km"));
        break;

      case 12:
        u8g.setPrintPos(10, 41);
        lcd_printPGM(PSTR("De Cordoba a Salta"));

        u8g.setPrintPos(13, 51);
        lcd_printPGM(PSTR("Haz Impreso 745km"));
        break;

      case 13:
        u8g.setPrintPos(7, 41);
        lcd_printPGM(PSTR("De Londres a Munich"));

        u8g.setPrintPos(13, 51);
        lcd_printPGM(PSTR("Haz Impreso 914km"));
        break;

      case 14:
        u8g.setPrintPos(1, 41);
        lcd_printPGM(PSTR("Olavarria a Bariloche"));

        u8g.setPrintPos(10, 51);
        lcd_printPGM(PSTR("Haz Impreso 1058km"));
        break;

      case 15:

        u8g.setPrintPos(19, 41);
        lcd_printPGM(PSTR("De Roma a Paris"));

        u8g.setPrintPos(10, 51);
        lcd_printPGM(PSTR("Haz Impreso 1105km"));
        break;

      case 16:
        u8g.setPrintPos(4, 41);
        lcd_printPGM(PSTR("De Canarias a Malaga"));

        u8g.setPrintPos(10, 51);
        lcd_printPGM(PSTR("Haz Impreso 1427km"));
        break;

      case 17:
        u8g.setPrintPos(13, 41);
        lcd_printPGM(PSTR("De Atenas A Praga"));

        u8g.setPrintPos(10, 51);
        lcd_printPGM(PSTR("Haz Impreso 1531km"));
        break;

      case 18:
        u8g.setPrintPos(13, 41);
        lcd_printPGM(PSTR("De Berlin a Moscu"));

        u8g.setPrintPos(10, 51);
        lcd_printPGM(PSTR("Haz Impreso 1605km"));
        break;

      case 19:
        u8g.setPrintPos(4, 41);
        lcd_printPGM(PSTR("De Kamchatka a Tokio"));

        u8g.setPrintPos(10, 51);
        lcd_printPGM(PSTR("Haz Impreso 2472km"));
        break;

      case 20:
        u8g.setPrintPos(4, 41);
        lcd_printPGM(PSTR("Ushuaia a La Quiaca!"));

        u8g.setPrintPos(10, 51);
        lcd_printPGM(PSTR("Haz Impreso 3700km"));
        break;

      case 21:
        u8g.setPrintPos(19, 41);
        lcd_printPGM(PSTR("De MDP a Bogota"));

        u8g.setPrintPos(10, 51);
        lcd_printPGM(PSTR("Haz Impreso 5038km"));
        break;


      case 22:
        u8g.setPrintPos(19, 41);
        lcd_printPGM(PSTR("De CABA a Praga"));

        u8g.setPrintPos(7, 51);
        lcd_printPGM(PSTR("Haz Impreso 11812km"));
        break;

      case 23:
        u8g.setPrintPos(1, 41);
        lcd_printPGM(PSTR("La Vuelta al Mundo!!!"));

        u8g.setPrintPos(7, 51);
        lcd_printPGM(PSTR("Haz Impreso 12756km"));
        break;
    }
    u8g.setColorIndex(1);
    if (LCD_CLICKED) {
      lcd_goto_screen(ticket, true);
    }
  }

  void ir_enviar_gcode() {
    contenedor_str[0] = '\0';
    lcd_goto_screen(enviar_gcode_m_g, true);
  }

  void ir_a_estadisticas() {
    lcd_goto_screen(estadisticas_impreciones, true);
  }
  void salir_ajustes_2(){
    lcd_goto_screen(Kuttercraft_menu, true, 9);

  }
  void reiniciar_memori_() {
    lcd_goto_screen(cartel_alerte_reinicio_de_memoria, true);
  }
  void no_hay_logros(){
    //no hay logros
    //no_logros_ico
    u8g.drawBitmapP(58, 17, 2, 15, no_logros_ico);
    u8g.setPrintPos(24, 46);
    lcd_printPGM(PSTR("No Hay Logros"));
    //volver
    if (LCD_CLICKED) {
      if(tipo_de_logro){
        lcd_goto_screen(menu_logros, true, 2);
      }else{
        lcd_goto_screen(menu_logros, true,1);
      }
    }
  }
  void ir_logros_km() {
    tipo_de_logro = false;
    if(numero_de_logro == 0){
      lcd_goto_screen(no_hay_logros, true);
    }else{
      se_permite_el_movimineto = true;
      lcd_goto_screen(logros, true);
    }
  }

  void ir_logros_pz() {
    tipo_de_logro = true;
    if(numero_de_logro_ == 0){
      lcd_goto_screen(no_hay_logros, true);
    }else{
      se_permite_el_movimineto = true;
      lcd_goto_screen(logros_piezas, true);
    }
  }
  void salir_de_logros() {
    lcd_goto_screen(ajustes, true, 3);
  }

  void menu_logros(){
    if (encoderPosition > 2 && encoderPosition < 50){
      encoderPosition = 2;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }

    START_MENU();
    MENU_ITEM(function, MSG_MAIN "              " LCD_STR_UPLEVEL, salir_de_logros);
    MENU_ITEM(function, "Kilometros", ir_logros_km);
    MENU_ITEM(function, "Piezas",     ir_logros_pz);
    END_MENU();
  }

  void menu_logros_(){
    lcd_goto_screen(menu_logros, true);
  }

  // void numero_random(){
  //   char ms_[8];
  //   contenedor_str[0] = '\0';
  //   strcpy(ms_, ltostr7(millis()));
  //   removeSpacesInPlace(ms_);
  //
  //   strncpy(contenedor_str,ms_,4);
  //
  // }

  //encargado de borrar_estadistica cuando se detecta el no uso de esta parte de memoria
  void esta_memoria_corrupta() {
    if(strcmp(confi_actualizacion, "4.0.0") != 0){
      strcpy(confi_actualizacion, "4.0.0");
      borrar_estadistica();
    }
  }


  void prender_apagara_sonido() {
    if(on_off_sonido_final){
      on_off_sonido_final = false;
    }else{
      on_off_sonido_final = true;
    }
    Config_StoreSettings();
  }
  void prender_apagara_estatus_guardado() {
    if(estatus_guardado){
      estatus_guardado = false;
    }else{
      estatus_guardado = true;
    }
    Config_StoreSettings();
  }

  void ajustes() {

    START_MENU();
    MENU_ITEM(function, MSG_MAIN "              " LCD_STR_UPLEVEL, salir_ajustes_2);
    MENU_ITEM(function, "Version",                ir_version_k);
    MENU_ITEM(function, "Estadistica",            ir_a_estadisticas);
    MENU_ITEM(function, "Logros",                 menu_logros_);

    //no funcional
    MENU_ITEM(function, "Idioma           [ES]",  salir_ajustes);

    //----------------------------mesh level----------------------------//
    #if(CON_SENSOR_INDUCTIVO)
      if(autolevel_on_off){
        MENU_ITEM(function, "Autolevel        [On]",  autolevel_estado);
      }else{
        MENU_ITEM(function, "Autolevel       [Off]",  autolevel_estado);
      }
    #endif
    //--------------------------visualización--------------------------//
    //                  "Idioma           [ES]"
    if(ver_cordenadas){
      MENU_ITEM(function, "Informacion   [Coord]",  coor_estado);
    }else{
      MENU_ITEM(function, "Informacion   [Capas]",  coor_estado);
    }

    if(save_on_off){
      MENU_ITEM(function, "Guardado         [On]",  guardado_estado);
    }else{
      MENU_ITEM(function, "Guardado        [Off]",  guardado_estado);
    }

    if(on_off_sensor_de_filamento){
      MENU_ITEM(function, "Sensor filamento [On]",  sensor_de_filamento_estado);
    }else{
      MENU_ITEM(function, "Sensor filamento[Off]",  sensor_de_filamento_estado);
    }

    if(dir_encoder == 1){
      MENU_ITEM(function, "Encoder         [DER]",  encoder_estado);
    }else{
      MENU_ITEM(function, "Encoder         [IZQ]",  encoder_estado);
    }

    if(on_off_sonido_final){
      MENU_ITEM(function, "KutterMelody     [On]",  prender_apagara_sonido);
    }else{
      MENU_ITEM(function, "KutterMelody    [Off]",  prender_apagara_sonido);
    }
    if(estatus_guardado){
      MENU_ITEM(function, "Info de M117     [On]",  prender_apagara_estatus_guardado);
    }else{
      MENU_ITEM(function, "Info de M117    [Off]",  prender_apagara_estatus_guardado);
    }

    #if(AUTO_TEMP_K)
      MENU_ITEM(function, "Ajustar PID", elegir_temp);
    #endif

    MENU_ITEM(function, "Config. Fabrica", reiniciar_memori_);

    MENU_ITEM(function, "Enviar Gcode", ir_enviar_gcode);

    MENU_ITEM(function, "Sistema", ir_entrar_a_sistema);
    END_MENU();
  }
  //----------------------------------------------------------------------------//
  //menunus mesh
  //----------------------------------------------------------------------------//
  #if(CON_SENSOR_INDUCTIVO)
  void menu_mesh();

  void boton_calibrar_offset_n(int valor) {
    if (valor == 1){
      u8g.setPrintPos(19, 63);
      u8g.setColorIndex(0);

      lcd_print(MSG_MENUS_AUTOLEVEL_04);

      u8g.setColorIndex(1);

      u8g.drawBitmapP(53,4,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, calibrar_offset_b);
      if (LCD_CLICKED) {
        se_permiten_carteles = false;
        lcd_goto_screen(preparar_offset, true);
      }
    }else{
      u8g.drawBitmapP(53,4,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, calibrar_offset_a);
    }
  }

  void boton_calibracion_manual_automatica(int valor) {
    if (valor == 3){
      u8g.setPrintPos(13, 63);
      u8g.setColorIndex(0);

      lcd_print(MSG_MANUAL_AUTO);

      u8g.setColorIndex(1);

      u8g.drawBitmapP(16,29,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, calibracion_manual_automatico_b);
      if (LCD_CLICKED) {
        se_permiten_carteles = false;
        lcd_goto_screen(_lcd_level_bed_continue, true);
      }
    }else{
      u8g.drawBitmapP(16,29,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, calibracion_manual_automatico_a);
    }
  }

  //boton
  void boton_mapa_de_calibracion(int valor) {
    if (valor == 5){
      u8g.setPrintPos(28, 63);
      u8g.setColorIndex(0);

      lcd_print(MSG_CALIBRAR_MAPA_);

      u8g.setColorIndex(1);

      u8g.drawBitmapP(66,29,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, mapa_de_cama_b);
      if (LCD_CLICKED) {
        lcd_goto_screen(matris_level, true);
      }
    }else{
      u8g.drawBitmapP(66,29,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, mapa_de_cama_a);
    }
  }
  //boton
  void boton_aliniar_eje_z(int valor) {
    if (valor == 6){
      u8g.setPrintPos(25, 63);
      u8g.setColorIndex(0);

      lcd_print(MGS_ALINEAR_EJE_Z);

      u8g.setColorIndex(1);

      u8g.drawBitmapP(91,29,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, aliniar_eje_z_b);
      if (LCD_CLICKED) {
        SERIAL_EM("A liniar eje z");
        lcd_goto_screen(alinear_eje_z, true);
      }
    }else{
      u8g.drawBitmapP(91,29,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT, aliniar_eje_z_a);
    }
  }
  //boton volver
  void boton_volver_calibrasion(int valor) {
    if (valor == 0){
      u8g.setPrintPos(45, 63);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_VOLVER));
      u8g.setColorIndex(1);
      u8g.drawBitmapP(28,4,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_volver_b);
      if (LCD_CLICKED) {
        lcd_goto_screen(Kuttercraft_menu, true, 4);
      }
    }else{
      u8g.drawBitmapP(28,4,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_volver_a);
    }
  }

    //---------------------------------
    //nuevo menu de auto calibracion 3.7
    //final
    //---------------------------------
    void menu_mesh(/* arguments */) {
      u8g.drawBox(0, 55, 128, 10);


      if (encoderPosition > 5 && encoderPosition < 50){
        encoderPosition = 6;
      }

      if (encoderPosition > 50){
        encoderPosition = 0;
      }
      posicion_anterior = 2;
      //volver aqui orlando
      boton_volver_calibrasion(encoderPosition);
      boton_calibrar_offset_n(encoderPosition);             //se_permiten_carteles = false;
      boton_modificar_offset(encoderPosition);
      boton_calibracion_manual_automatica(encoderPosition); //se_permiten_carteles = false;
      boton_calibracion_automatica(encoderPosition);        //se_permiten_carteles = false;
      boton_mapa_de_calibracion(encoderPosition);
      boton_aliniar_eje_z(encoderPosition);

      //boton_mesh_ajus(encoderPosition); --------------
      //boton_mesh_cont(encoderPosition);
    }
  #endif
  //---------------------------------Fin  menu mesh-----------------------------//
  //----------------------------------------------------------------------------//

//-----------------------------Inicio cambio de filamento print-----------------------//
//static void Kuttercraft_menu_print();

void seleccionar_capa(/* arguments */) {
  u8g.drawBox(0, 0, 128, 18);
  u8g.setPrintPos(31, 8);
  u8g.setColorIndex(0);
  lcd_printPGM(PSTR("Seleccionar"));//11
  u8g.setPrintPos(22, 16);
  lcd_printPGM(PSTR("capa de cambio"));//14
  u8g.setPrintPos(34, 44);
  u8g.setColorIndex(1);
  lcd_printPGM(PSTR("Capa:"));

  encoderRateMultiplierEnabled = true;

  if(encoderPosition >= actual_capas + 1 && encoderPosition <= total_capas) {

  }else if(encoderPosition >= total_capas && encoderPosition <= total_capas + 1000){
    encoderPosition = total_capas;
  }else{
    encoderPosition = actual_capas + 1;
  }

  lcd_print(ftostr5rj(encoderPosition));

  if(LCD_CLICKED) {
    capas_de_cambio = encoderPosition;
    cambiar_fila_on_off = true;
    lcd_goto_screen(volver_info, true);
  }
}
void cambiar_filamento_ahora(int valor){
  if (valor == 1){
    u8g.setPrintPos(49, 63);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR("Ahora"));//5
    u8g.setColorIndex(1);
    u8g.drawBitmapP(49,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, cargar_boton_b);
    if (LCD_CLICKED) {
      lcd_goto_screen(cambiar_filamento, true);
    }
  }else{
    u8g.drawBitmapP(49,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, cargar_boton_a);
  }
}
void mesage_error_fila(/* arguments */) {
  u8g.drawBox(0, 22, 128, 18);
  u8g.setPrintPos(28, 30);
  u8g.setColorIndex(0);
  lcd_printPGM(PSTR("Error: capas"));//12
  u8g.setPrintPos(19, 39);
  lcd_printPGM(PSTR("no establecidas"));//15
  if (LCD_CLICKED) {
    lcd_goto_screen(volver_info, true);
  }
}
void cancelar_cambio_bot(int valor) {
  if (valor == 1){
    u8g.setPrintPos(19, 63);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR("Cancelar cambio"));//15
    u8g.setColorIndex(1);
    u8g.drawBitmapP(74,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT,seguir_boton_b);//volver_boton_a

    if(LCD_CLICKED) {
      capas_de_cambio = 0;
      cambiar_fila_on_off = false;
      lcd_goto_screen(volver_info, true);
    }
  }else{
    u8g.drawBitmapP(74,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT,seguir_boton_a);//9
  }
}
void cancelar_cambio(/* arguments */) {
  u8g.drawBox(0, 55, 128, 10);
  u8g.drawBox(0, 0, 128, 10);

  u8g.setPrintPos(6, 8);
  u8g.setColorIndex(0);
  lcd_printPGM(PSTR("Cambio capa: "));//17
  //u8g.setPrintPos(35, 8);
  lcd_print(ftostr5rj(capas_de_cambio));
  u8g.setColorIndex(1);

  if (encoderPosition > 0 && encoderPosition < 50){
    encoderPosition = 1;
  }
  if (encoderPosition > 50){
    encoderPosition = 0;
  }
  posicion_anterior = 2;
  volver_boton_grande(encoderPosition, 23, volver_info);
  cancelar_cambio_bot(encoderPosition);
}
void cambiar_filamento_en_capa(int valor) {
  if (valor == 2){
    u8g.setPrintPos(40, 63);
    u8g.setColorIndex(0);
    lcd_printPGM(PSTR("En Capa:"));//8
    u8g.setColorIndex(1);
    u8g.drawBitmapP(89,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, retirar_boton_b);
    if (LCD_CLICKED) {
      if(cambiar_fila_on_off){
        lcd_goto_screen(cancelar_cambio, true);
      }else if(total_capas == 0 || total_capas <= actual_capas){
        lcd_goto_screen(mesage_error_fila, true);
      }else{
        lcd_goto_screen(seleccionar_capa, true);
      }
    }
  }else{
    u8g.drawBitmapP(89,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, retirar_boton_a);
  }
}
void menu_cambio_filamento_print() {
  u8g.drawBox(0, 55, 128, 10);
  u8g.drawBox(0, 0, 128, 10);

  u8g.setPrintPos(13, 8);
  u8g.setColorIndex(0);
  lcd_printPGM(PSTR("Cambiar filamento"));//17
  u8g.setColorIndex(1);

  if (encoderPosition > 1 && encoderPosition < 50){
    encoderPosition = 2;
  }
  if (encoderPosition > 50){
    encoderPosition = 0;
  }
  posicion_anterior = 5;
  volver_boton_grande(encoderPosition, 9, auxiliar_bot_grande);
  cambiar_filamento_ahora(encoderPosition);
  cambiar_filamento_en_capa(encoderPosition);
}



//------------------------------Fin  cambio de filamento print------------------------//
//------------------------------Inicio  guardado------------------------//
#if(GUARDAR)
void save_save(){
  SERIAL_EM("ok");
  SERIAL_EM("\n");
  card.stopPrint(true);
}
void save_save1(){
  lcd_goto_screen(save_save, true);
  lcd_goto_screen(Kuttercraft_menu, true);
}
void cambiar_filamento_boton(int valor) {
  if(valor == 7){
    u8g.drawBitmapP(102,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_cali_b);
    u8g.setPrintPos(41, 63);
    u8g.setColorIndex(0);           //Cambia el color a azul
    lcd_printPGM(PSTR(MSG_CALIBRAR));
    u8g.setColorIndex(1);           //Cambia el color a blanco
    //Accion al pulsar
    if(LCD_CLICKED) {
      //lcd_goto_screen(sdasdasd, false);
      card.stopPrint(true);
    }
  }else{
    u8g.drawBitmapP(102,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_cali_a);
  }
}
#endif
//------------------------------Fin  guardado------------------------//
  //----------------------------------------------------------------------------//
  //--Botones Accion Inicio--//
  //Botones del menu Kuttercraft son 10
  //Inicio -> vuelve al inicio
  void botonUno(int valor) {
    if(valor == 0){
      u8g.drawBitmapP(2,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_inicio_b);
      //se escribe la descripción
      u8g.setPrintPos(44, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_INICIO));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      //Accion al pulsar
      if(LCD_CLICKED) {lcd_goto_screen(volver_info, true);}
    }else{
      u8g.drawBitmapP(2,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_inicio_a);
    }
  }
  //Trajeta -> menu sd
  void botonDos(int valor) {
    if(valor == 1){
      if (card.cardOK) {
        u8g.drawBitmapP(27,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_sd_b);
        u8g.setPrintPos(33, 63);
        u8g.setColorIndex(0);           //Cambia el color a azul
        lcd_printPGM(PSTR(MSG_TARJETA));
        u8g.setColorIndex(1);           //Cambia el color a blanco
        //Accion al pulsar
        if(LCD_CLICKED) {lcd_goto_screen(lcd_sdcard_menu, true);/*lcd_goto_screen(lcd_prepare_menu);*/}
      }else{
        u8g.drawBitmapP(27,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_recargar_b);
        u8g.setPrintPos(8, 63);
        u8g.setColorIndex(0);           //Cambia el color a azul
        lcd_printPGM(PSTR(MSG_TARJETA_NO));
        u8g.setColorIndex(1);           //Cambia el color a blanco
        //Accion al pulsar
        if(LCD_CLICKED) {enqueue_and_echo_commands_P(PSTR("M21"));}
      }
    }else{
      if (card.cardOK) {
        u8g.drawBitmapP(27,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_sd_a);
      }else{
        u8g.drawBitmapP(27,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_recargar_a);
      }
    }
  }
  //Filamento -> menu de carga y desgarga
  void botonTres(int valor) {
    if(valor == 2){
      u8g.drawBitmapP(52,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_filamento_b);
      u8g.setPrintPos(7, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_FILAMENTO));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      //Accion al pulsar
      if(LCD_CLICKED) {
        lcd_goto_screen(sele_temperatura, true);
      }
    }else{
      u8g.drawBitmapP(52,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_filamento_a);
    }

  }
  //temperatura -> Opcciones de temperatura
  void botonCuatro(int valor) {
    if(valor == 3){
      u8g.drawBitmapP(77,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_Temp_b);
      u8g.setPrintPos(32, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_TEMPERATURE));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      //Accion al pulsar
      if(LCD_CLICKED) {lcd_goto_screen(temp_manu_pre, true);}
    }else{
      u8g.drawBitmapP(77,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_Temp_a);
    }
  }
  //calibrar -> calibracion asistida
  void botonCinco(int valor) {
    if(valor == 4){
      u8g.drawBitmapP(102,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_cali_b);
      u8g.setPrintPos(41, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_CALIBRAR));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      //Accion al pulsar
      if(LCD_CLICKED) {
        #if(CON_SENSOR_INDUCTIVO)
          if(mbl.has_mesh()){

            lcd_goto_screen(menu_mesh, true);
          }else{
            lcd_goto_screen(menu_calibracion_mecanica, true);
          }
        #else
          lcd_goto_screen(menu_calibracion_mecanica, true);
        #endif
      }
    }else{
      u8g.drawBitmapP(102,3,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_cali_a);
    }
  }
  //---------------------------------
  void botonSeis_Prender(int valor) {
    if(valor == 5){
      u8g.setPrintPos(34, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_LED_OFF));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      u8g.drawBitmapP(2,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_led_prendido_b);
      if(LCD_CLICKED) {
        lcd_goto_screen(led_prende_apaga);
      }
    }else{
      u8g.drawBitmapP(2,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_led_prendido_a);
    }
  }
  void botonSeis_Apagar(int valor) {
    if(valor == 5){
      u8g.setPrintPos(28, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_LED_ON));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      u8g.drawBitmapP(2,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_led_b);
      if(LCD_CLICKED) {
        lcd_goto_screen(led_prende_apaga);
      }
    }else{
      u8g.drawBitmapP(2,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_led_a);
    }
  }
  //---------------------------------
  //elije el tipo de boton led (encendido o apagado)
  void botonSeis(int valor) {
    if(ledKuttercraft){
      botonSeis_Prender(valor);
    }else{
      botonSeis_Apagar(valor);
    }
  }
  //lleva a el menu mover eje
  void botonSiete(int valor) {
    if(valor == 6){
      u8g.drawBitmapP(27,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_mover_b);
      u8g.setPrintPos(34, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_PREPARE));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      //Accion al pulsar
      if(LCD_CLICKED) {lcd_goto_screen(menu_mover_eje, true);}
    }else{
      u8g.drawBitmapP(27,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_mover_a);
    }
  }

  void botonOcho(int valor) {
    if(valor == 7){
      u8g.drawBitmapP(52,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_apagar_b);
      u8g.setPrintPos(22, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_OFF_MOTOR));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      //Accion al pulsar
      if(LCD_CLICKED) {lcd_goto_screen(apagar_motor_manual, true);}
    }else{
      u8g.drawBitmapP(52,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_apagar_a);
    }
  }

  void botonNueve(int valor) {
    if(valor == 8){
      u8g.drawBitmapP(77,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,store_boton_b);
      u8g.setPrintPos(13, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_STORE_M));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      //Accion al pulsar
      if(LCD_CLICKED) {lcd_goto_screen(menu_qr, true);}
    }else{
      u8g.drawBitmapP(77,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,store_boton_a);
    }
  }

  void botonDiez(int valor) {
    if(valor == 9){
      u8g.drawBitmapP(102,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_ajustes_b);
      u8g.setPrintPos(44, 63);
      u8g.setColorIndex(0);           //Cambia el color a azul
      lcd_printPGM(PSTR(MSG_CONTROL));
      u8g.setColorIndex(1);           //Cambia el color a blanco
      //Accion al pulsar
      if(LCD_CLICKED) {lcd_goto_screen(ajustes, true);}
    }else{
      u8g.drawBitmapP(102,30,STATUS_MENU_A_BY_TEWIDTH,STATUS_MENU_A_HEIGHT,boton_ajustes_a);
    }
  }
  //--Botones Accion Fin--//

  //--Print Menu Kuttercraft Inicio--//
  static void Kuttercraft_menu() {
      u8g.drawBox(0, 55, 128, 10);
      if (encoderPosition > 9 && encoderPosition < 200){
        encoderPosition = 0;
      }
      if (encoderPosition > 200){
        encoderPosition = 9;
      }
      lcd_save_previous_menu();
      botonUno(encoderPosition);
      //boton_chico(2 , 3, 0, encoderPosition, 46, MSG_INICIO, boton_inicio_a, boton_inicio_b, volver_info);
      botonDos(encoderPosition);
      //boton_chico(52 , 3, 2, encoderPosition, 7, MSG_FILAMENTO, boton_filamento_a, boton_filamento_b, confirmar_filamento);
      botonTres(encoderPosition);
      botonCuatro(encoderPosition);
      botonCinco(encoderPosition);
      botonSeis(encoderPosition);
      botonSiete(encoderPosition);
      botonOcho(encoderPosition);
      botonNueve(encoderPosition);
      botonDiez(encoderPosition);

  }
  void no_hay_sensor_print(){
    u8g.drawBitmapP(58, 22, 2, 15, no_logros_ico);

    u8g.drawBox(0, 54, 128, 10);
    u8g.drawBox(0, 0, 128, 10);

    u8g.setColorIndex(0);
    u8g.setPrintPos(4, 8);
    lcd_printPGM(PSTR("Opcion No Disponible"));
    u8g.setPrintPos(13, 63);
    lcd_printPGM(PSTR("Lo Tiene Activado"));
    u8g.setColorIndex(1);

    u8g.setPrintPos(22, 51);
    lcd_printPGM(PSTR("El Firmware No"));

    if(LCD_CLICKED){
      //mbl.reset();
      autolevel_on_off = false;
      lcd_goto_screen(Kuttercraft_menu_print, true,2);
    }
  }
  void Kuttercraft_menu_print(/* arguments */) {
    u8g.drawBox(0, 55, 128, 10);
    u8g.drawBox(0, 0, 128, 2);

    if (encoderPosition > 8 && encoderPosition < 50){
      encoderPosition = 9;
    }
    if (encoderPosition > 50){
      encoderPosition = 0;
    }
    //primera fila

    boton_chico(2, 4, 0, encoderPosition, 46, MSG_INICIO, boton_inicio_a, boton_inicio_b, volver_info);

    boton_chico(27, 4, 1, encoderPosition, 31, MSG_TEMPERATURE, boton_Temp_a, boton_Temp_b, temp_manu_pre);

    //offset
    #if(SENSOR_INDUCTIVO_NUEVO)
      boton_chico(52, 4, 2, encoderPosition, 25, MSG_CALIBRAR_BASE, boton_cali_a, boton_cali_b, menu_ajustar_offset_print);
    #else
      boton_chico(52, 4, 2, encoderPosition, 25, MSG_CALIBRAR_BASE, boton_cali_a, boton_cali_b, no_hay_sensor_print);
    #endif

    boton_chico(77, 4, 3, encoderPosition, 13, MSG_FILAMENT_CHANGE, boton_filamento_a, boton_filamento_b, menu_cambio_filamento_print);

    if(ledKuttercraft){
      boton_chico(102, 4, 4, encoderPosition, 34, MSG_LED_OFF, boton_led_prendido_a, boton_led_prendido_b, led_prende_apaga2);
    }else{
      boton_chico(102, 4, 4, encoderPosition, 28, MSG_LED_ON, boton_led_a, boton_led_b, led_prende_apaga2);
    }

    //segunda fila

    if (!en_pausa){
      boton_chico(2, 30, 5, encoderPosition, 16, MSG_PAUSE_PRINT, boton_pausa_a, boton_pausa_b, pausa_print);
    }else{
      boton_chico(2, 30, 5, encoderPosition, 10, MSG_RESUME_PRINT, boton_reanudar_a, boton_reanudar_b, pausa_print);
    }

    boton_chico(27, 30, 6, encoderPosition, 19, MSG_AJUSTAR_FLOW, boton_ajustes_a, boton_ajustes_b, ir_cambiar_flow);

    boton_chico(52, 30, 7, encoderPosition, 37, MSG_VELOCIDAD, boton_velocidad_a, boton_velocidad_b, ir_velocidad_manual);

    boton_chico(77, 30, 8, encoderPosition, 10, MSG_STOP_PRINT_01, boton_detener_a, boton_detener_b, cancelar_print);

    boton_chico(102, 30, 9, encoderPosition, 7, MSG_STOP_PRINT, reiniciar_boton_a, reiniciar_boton_b, reiniciar_print);
    //
    #if(GUARDAR)
      //cambiar_filamento_boton(encoderPosition);
    #endif
  }
  static void Kuttercraft_menu_start() {
    //Kuttercraft_menu()
    //pregunta si hay una sd
    if (card.cardOK) {

      //si se esta leyendo un achivo
      if (print_job_counter.imprimiendo_estado){
        lcd_goto_screen(Kuttercraft_menu_print, true, 0);
      }else{
        //si save_on_off esta prendido
        if(save_on_off && se_estaba_imprimiendo){
          lcd_goto_screen(menu_restart_yes_not, true, 0); //va al menu de corte de luz
        }else{
          lcd_goto_screen(Kuttercraft_menu, true, 0);
        }
      }

    //solo hay una posibilidad mostrar menu pricital
    }else{lcd_goto_screen(Kuttercraft_menu, true, 0);}
  }
  //listado de pocisiones del encoder anterior
  static void auxiliar_bot_grande() {
    if(posicion_anterior == 0){
      //se entro a mover eje boton 1
      lcd_goto_screen(Kuttercraft_menu, true, 2);
    }else if(posicion_anterior == 1){
      //se entro a eje x boton 1
      lcd_goto_screen(Kuttercraft_menu, true, 1);
    }else if(posicion_anterior == 2){
      //se entro a eje x boton 1
      lcd_goto_screen(Kuttercraft_menu, true, 4);
    }else if(posicion_anterior == 3){
      //se entro a eje x boton 1
      if (card.cardOK) {
        if (card.isFileOpen()) {
          lcd_goto_screen(Kuttercraft_menu_print, true, 1);
        }else{
          lcd_goto_screen(Kuttercraft_menu, true, 3);
        }
      }else{
        lcd_goto_screen(Kuttercraft_menu, true, 3);
      }
    }else if(posicion_anterior == 4){
      //se entro a eje x boton 1
      lcd_goto_screen(Kuttercraft_menu, true, 6);

    }else if(posicion_anterior == 5){
      lcd_goto_screen(Kuttercraft_menu_print, true, 3);

    //----------------------------------
    //calibracion
    }else if(posicion_anterior == 20){
      lcd_goto_screen(primer_punto, true, 0);
    }else if(posicion_anterior == 21){
      lcd_goto_screen(segundo_punto, true, 1);
    }else if(posicion_anterior == 22){
      lcd_goto_screen(tercero_punto, true, 1);
    }else if(posicion_anterior == 23){
      lcd_goto_screen(cuarto_punto, true, 1);
    }else if(posicion_anterior == 30){
    //----------------------------------
    //cargar filameno
      lcd_goto_screen(cargar_retirar, true, 1);
    }else if(posicion_anterior == 31){
      lcd_goto_screen(cargar_retirar, true, 2);
    }else if(posicion_anterior == 40){
      lcd_goto_screen(Kuttercraft_menu, true, 9);

    }else if(posicion_anterior == 46){
      //----------------------------------
      //Led en menu Kuttercraft_menu_print
      lcd_goto_screen(Kuttercraft_menu_print, true, 2);
      //----------------------------------
      //Led en menu autolevel y offset
    }
    #if HAS(BED_PROBE) && NOMECH(DELTA)
      else if(posicion_anterior == 50){
        lcd_goto_screen(menu_mesh, true, 2);
      }
    #endif

  }
  static void lcd_prepare_menu() {

    //
    // ^ Main
    //
  }

  #if MECH(DELTA)

    static void _goto_tower_pos(const float &a) {
      do_blocking_move_to(
        a < 0 ? X_HOME_POS : sin(a) * -(DELTA_PRINTABLE_RADIUS),
        a < 0 ? Y_HOME_POS : cos(a) *  (DELTA_PRINTABLE_RADIUS),
        4
      );
    }

    static void _goto_tower_x() { _goto_tower_pos(RADIANS(120)); }
    static void _goto_tower_y() { _goto_tower_pos(RADIANS(240)); }
    static void _goto_tower_z() { _goto_tower_pos(0); }
    static void _goto_center()  { _goto_tower_pos(-1); }

  #endif // DELTA


  /**
   * If the most recent manual move hasn't been fed to the planner yet,
   * and the planner can accept one, send immediately
   */
  inline void manage_manual_move() {
    if (manual_move_axis != (int8_t)NO_AXIS && ELAPSED(millis(), manual_move_start_time) && !planner.is_full()) {
      #if MECH(DELTA)
        inverse_kinematics(current_position);
        planner.buffer_line(delta[TOWER_1], delta[TOWER_2], delta[TOWER_3], current_position[E_AXIS], manual_feedrate[manual_move_axis]/60, manual_move_e_index, active_driver);
      #else
        planner.buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[manual_move_axis]/60, manual_move_e_index, active_driver);
      #endif
      manual_move_axis = (int8_t)NO_AXIS;
    }
  }

  /**
   * Set a flag that lcd_update() should start a move
   * to "current_position" after a short delay.
   */
  inline void manual_move_to_current(AxisEnum axis
    #if EXTRUDERS > 1
      , int8_t eindex = -1
    #endif
  ) {
    #if EXTRUDERS > 1
      if (axis == E_AXIS) manual_move_e_index = eindex >= 0 ? eindex : active_extruder;
    #endif
    manual_move_start_time = millis() + (move_menu_scale < 0.99 ? 0UL : 250UL); // delay for bigger moves
    manual_move_axis = (int8_t)axis;
  }

  /**
   *
   * "Prepare" > "Move Axis" submenu
   *
   */

  static void _lcd_move_xyz(const char* name, AxisEnum axis) {
    if (LCD_CLICKED) {
      //lcd_goto_screen(seleccionar_mm_mover, true, valor);
      if(move_menu_scale == 0.1){
        lcd_goto_screen(mover_eje_volver_01, true);
      }else if(move_menu_scale == 1.0){
        lcd_goto_screen(mover_eje_volver_1, true);
      }else if(move_menu_scale == 5.0){
        lcd_goto_screen(mover_eje_volver_5, true);
      }
    }
    ENCODER_DIRECTION_NORMAL();
    if (encoderPosition) {
      refresh_cmd_timeout();

      // Limit to software endstops, if enabled
      float min = (soft_endstops_enabled && SOFTWARE_MIN_ENDSTOPS) ? soft_endstop_min[axis] : current_position[axis] - 1000,
            max = (soft_endstops_enabled && SOFTWARE_MAX_ENDSTOPS) ? soft_endstop_max[axis] : current_position[axis] + 1000;

      // Get the new position
      current_position[axis] += float((int32_t)encoderPosition) * move_menu_scale;

      // Limit only when trying to move towards the limit
      if ((int32_t)encoderPosition < 0) NOLESS(current_position[axis], min);
      if ((int32_t)encoderPosition > 0) NOMORE(current_position[axis], max);

      manual_move_to_current(axis);

      encoderPosition = 0;
      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
    }
    if (lcdDrawUpdate) lcd_implementation_drawedit(name, ftostr41sign(current_position[axis]));
  }

  #if ENABLED(AUTO_BED_LEVELING_FEATURE)
    static void _lcd_move_z_offset(const char* name, AxisEnum axis) {
    //if (LCD_CLICKED) { lcd_goto_previous_menu(true); return; }
    ENCODER_DIRECTION_NORMAL();
    if (encoderPosition) {
      refresh_cmd_timeout();

      // Limit to software endstops, if enabled
      float min = (soft_endstops_enabled && SOFTWARE_MIN_ENDSTOPS) ? soft_endstop_min[axis] : current_position[axis] - 1000,
            max = (soft_endstops_enabled && SOFTWARE_MAX_ENDSTOPS) ? soft_endstop_max[axis] : current_position[axis] + 1000;

      // Get the new position
      current_position[axis] += float((int32_t)encoderPosition) * move_menu_scale;

      // Delta limits XY based on the current offset from center
      // This assumes the center is 0,0
      #if MECH(DELTA)
        if (axis != Z_AXIS) {
          max = sqrt(sq(DELTA_PRINTABLE_RADIUS) - sq(current_position[Y_AXIS - axis]));
          min = -max;
        }
      #endif

      // Limit only when trying to move towards the limit
      if ((int32_t)encoderPosition < 0) NOLESS(current_position[axis], min);
      if ((int32_t)encoderPosition > 0) NOMORE(current_position[axis], max);

      manual_move_to_current(axis);

      encoderPosition = 0;
      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
    }
    float offset_val = current_position[axis] - 10;
    if (LCD_CLICKED) {
      zprobe_zoffset =  offset_val;
      enqueue_and_echo_commands_P(PSTR("M500"));
      //lcd_goto_screen(offset_Guardar);
      lcd_goto_previous_menu(true);
      return;
      //offset_Guardar();
    }
    if (lcdDrawUpdate) lcd_implementation_drawedit(name, ftostr41sign(offset_val));
  }
  #endif
  static void lcd_move_x() { /*_lcd_move_xyz(PSTR(MSG_MOVE_X), X_AXIS);*/ }
  static void lcd_move_y() { /*_lcd_move_xyz(PSTR(MSG_MOVE_Y), Y_AXIS); */}
  static void lcd_move_z() { /*_lcd_move_xyz(PSTR(MSG_MOVE_Z), Z_AXIS);*/ }
  #if ENABLED(AUTO_BED_LEVELING_FEATURE)
    static void lcd_move_z_offset() { _lcd_move_z_offset(PSTR(MSG_MOVE_Z_OFFSET), Z_AXIS); }
  #endif
  static void _lcd_move_e(
    #if EXTRUDERS > 1
      int8_t eindex = -1
    #endif
  ) {
    if (LCD_CLICKED) { lcd_goto_previous_menu(true); return; }
    ENCODER_DIRECTION_NORMAL();
    if (encoderPosition) {
      current_position[E_AXIS] += float((int32_t)encoderPosition) * move_menu_scale;
      encoderPosition = 0;
      manual_move_to_current(E_AXIS
        #if EXTRUDERS > 1
          , eindex
        #endif
      );
      lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
    }
    if (lcdDrawUpdate) {
      PGM_P pos_label;
      #if EXTRUDERS == 1
        pos_label = PSTR(MSG_MOVE_E);
      #else
        switch (eindex) {
          case 0: pos_label = PSTR(MSG_MOVE_E "0"); break;
          case 1: pos_label = PSTR(MSG_MOVE_E "1"); break;
          #if EXTRUDERS > 2
            case 2: pos_label = PSTR(MSG_MOVE_E "2"); break;
            #if EXTRUDERS > 3
              case 3: pos_label = PSTR(MSG_MOVE_E "3"); break;
            #endif // EXTRUDERS > 3
          #endif // EXTRUDERS > 2
        }
      #endif // EXTRUDERS > 1
      lcd_implementation_drawedit(pos_label, ftostr41sign(current_position[E_AXIS]));
    }
  }

  static void lcd_move_e() { _lcd_move_e(); }
  #if EXTRUDERS > 1
    static void lcd_move_e0() { _lcd_move_e(0); }
    static void lcd_move_e1() { _lcd_move_e(1); }
    #if EXTRUDERS > 2
      static void lcd_move_e2() { _lcd_move_e(2); }
      #if EXTRUDERS > 3
        static void lcd_move_e3() { _lcd_move_e(3); }
      #endif
    #endif
  #endif // EXTRUDERS > 1

  /**
   *
   * "Prepare" > "Move Xmm" > "Move XYZ" submenu
   *
   */

  #if MECH(DELTA) || MECH(SCARA)
    #define _MOVE_XYZ_ALLOWED (axis_homed[X_AXIS] && axis_homed[Y_AXIS] && axis_homed[Z_AXIS])
  #else
    #define _MOVE_XYZ_ALLOWED true
  #endif
  /*
  static void _lcd_move_menu_axis() {
    START_MENU();
    MENU_ITEM(back, MSG_MOVE_AXIS);

    if (_MOVE_XYZ_ALLOWED) {
      MENU_ITEM(submenu, MSG_MOVE_X, lcd_move_x);
      MENU_ITEM(submenu, MSG_MOVE_Y, lcd_move_y);
      MENU_ITEM(submenu, MSG_MOVE_Z, lcd_move_z);
    }
    if (move_menu_scale < 10.0) {

      #if ENABLED(DONDOLO_SINGLE_MOTOR) || ENABLED(DONDOLO_DUAL_MOTOR)
        if (active_extruder)
          MENU_ITEM(gcode, MSG_SELECT " E0", PSTR("T0"));
        else
          MENU_ITEM(gcode, MSG_SELECT " E1", PSTR("T1"));
      #endif

      MENU_ITEM(submenu, MSG_MOVE_E, lcd_move_e);
      #if EXTRUDERS > 1
        MENU_ITEM(submenu, MSG_MOVE_E "0", lcd_move_e0);
        MENU_ITEM(submenu, MSG_MOVE_E "1", lcd_move_e1);
        #if EXTRUDERS > 2
          MENU_ITEM(submenu, MSG_MOVE_E "2", lcd_move_e2);
          #if EXTRUDERS > 3
            MENU_ITEM(submenu, MSG_MOVE_E "3", lcd_move_e3);
          #endif
        #endif
      #endif // EXTRUDERS > 1
    }
    END_MENU();
  }
  */
  //Mover Eje de Kuttercraft

  static void _lcd_move_menu_axis_k() {
    /*
    if (_MOVE_XYZ_ALLOWED) {
      MENU_ITEM(submenu, MSG_MOVE_X, lcd_move_x);
      MENU_ITEM(submenu, MSG_MOVE_Y, lcd_move_y);
      MENU_ITEM(submenu, MSG_MOVE_Z, lcd_move_z);
    }
    */
    if(tipo_eje==0){
      lcd_move_x();
    }
    if(tipo_eje==1){
      lcd_move_y();
    }
    if(tipo_eje==2){
      lcd_move_z();
    }
    if(tipo_eje==3){
      lcd_move_e();
    }
  }
  /*
  static void lcd_move_menu_10mm() {
    move_menu_scale = 10.0;
    _lcd_move_menu_axis();
  }
  static void lcd_move_menu_1mm() {
    move_menu_scale = 1.0;
    _lcd_move_menu_axis();
  }
  static void lcd_move_menu_01mm() {
    move_menu_scale = 0.1;
    _lcd_move_menu_axis();
  }
  */
  static void lcd_move_menu_10mm_k() {
    move_menu_scale = 10.0;
    _lcd_move_menu_axis_k();
  }
  static void lcd_move_menu_1mm_k() {
    move_menu_scale = 1.0;
    _lcd_move_menu_axis_k();
  }
  static void lcd_move_menu_01mm_k() {
    move_menu_scale = 0.1;
    _lcd_move_menu_axis_k();
  }


  //Kuttercraft
  static void lcd_move_menu_kuttercraft() {
    START_MENU();
    MENU_ITEM(back, MSG_PREPARE);

    if (_MOVE_XYZ_ALLOWED)
      MENU_ITEM(submenu, MSG_MOVE_X, eje_x);
      MENU_ITEM(submenu, MSG_MOVE_Y, eje_y);
      MENU_ITEM(submenu, MSG_MOVE_Z, eje_z);
      MENU_ITEM(submenu, MSG_MOVE_E, eje_e);
    // TODO:X,Y,Z,E
    END_MENU();
  }

  static void eje_x() {
    tipo_eje = 0;
    lcd_move_menu_kuttercraft_a();
  }
  static void eje_y() {
    tipo_eje = 1;
    lcd_move_menu_kuttercraft_a();
  }
  static void eje_z() {
    tipo_eje = 2;
    lcd_move_menu_kuttercraft_a();
  }
  static void eje_e() {
    tipo_eje = 3;
    lcd_move_menu_kuttercraft_a();
  }
  static void lcd_move_menu_kuttercraft_a() {
    START_MENU();
    MENU_ITEM(back, MSG_PREPARE);

    if (_MOVE_XYZ_ALLOWED)
    if(tipo_eje==3){
      //MENU_ITEM(submenu, MSG_MOVE_10MM, lcd_move_menu_10mm_k);
      MENU_ITEM(submenu, MSG_MOVE_1MM, lcd_move_menu_1mm_k);
      MENU_ITEM(submenu, MSG_MOVE_01MM, lcd_move_menu_01mm_k);
    }else{
      MENU_ITEM(submenu, MSG_MOVE_10MM, lcd_move_menu_10mm_k);
      MENU_ITEM(submenu, MSG_MOVE_1MM, lcd_move_menu_1mm_k);
      MENU_ITEM(submenu, MSG_MOVE_01MM, lcd_move_menu_01mm_k);
    }
    // TODO:X,Y,Z,E
    END_MENU();
  }
  /**
   *
   * "Control" submenu
   *
   */
   #if ENABLED(AUTO_BED_LEVELING_FEATURE)

   static void calibrar_Offset_Dos() {
     if (lcdDrawUpdate);
     //enqueue_and_echo_commands_P(PSTR("G28\nG28 Z"));
     //enqueue_and_echo_commands_P(PSTR("G1 Z10"));
     lcd_move_z_offset();
   }
   static void calibrar_Offset_Uno() {
     if (lcdDrawUpdate);
     enqueue_and_echo_commands_P(PSTR("G1 Z10"));
     move_menu_scale = 0.1;
     u8g.setFont(u8g_font_6x10);
     u8g.drawStr(10,30,"Llevando al origen");
     u8g.setFont(u8g_font_5x8);
     u8g.drawStr(15,62,"Espere...");
       lcdDrawUpdate =
         #if ENABLED(DOGLCD)
           LCDVIEW_CALL_REDRAW_NEXT
         #else
           LCDVIEW_CALL_NO_REDRAW
         #endif
       ;
       if (axis_homed[X_AXIS] && axis_homed[Y_AXIS] && axis_homed[Z_AXIS])
         //enqueue_and_echo_commands_P(PSTR("G1 Z0\nG1 X150 Y150\nM666 P-10"));
         lcd_goto_screen(calibrar_Offset_Dos);
   }
   static void calibrar_Offset() {
     defer_return_to_status = true;
     //BORRADO POR LA TITAN
    //enqueue_and_echo_commands_P(PSTR("G28\nM666 P-10\nG28 Z"));
       lcd_goto_screen(calibrar_Offset_Uno);
   }
   #endif

  /**
   *
   * "Temperature" submenu
   *
   */

  #if ENABLED(PID_AUTOTUNE_MENU)

    #if ENABLED(PIDTEMP)
      int autotune_temp[HOTENDS] = ARRAY_BY_HOTENDS(150);
      const int heater_maxtemp[HOTENDS] = ARRAY_BY_HOTENDS_N(HEATER_0_MAXTEMP, HEATER_1_MAXTEMP, HEATER_2_MAXTEMP, HEATER_3_MAXTEMP);
    #endif

    #if ENABLED(PIDTEMPBED)
      int autotune_temp_bed = 70;
    #endif

    static void _lcd_autotune(int h) {
      char cmd[30];
      sprintf_P(cmd, PSTR("M303 U1 H%i S%i"), h,
        #if HAS_PID_FOR_BOTH
          h < 0 ? autotune_temp_bed : autotune_temp[h]
        #elif ENABLED(PIDTEMPBED)
          autotune_temp_bed
        #else
          autotune_temp[h]
        #endif
      );
      enqueue_and_echo_command(cmd);
    }

  #endif //PID_AUTOTUNE_MENU

  #if ENABLED(PIDTEMP)

    // Helpers for editing PID Ki & Kd values
    // grab the PID value out of the temp variable; scale it; then update the PID driver
    void copy_and_scalePID_i(int h) {
      PID_PARAM(Ki, h) = scalePID_i(raw_Ki);
      updatePID();
    }
    void copy_and_scalePID_d(int h) {
      PID_PARAM(Kd, h) = scalePID_d(raw_Kd);
      updatePID();
    }
    #define _PIDTEMP_BASE_FUNCTIONS(hindex) \
      void copy_and_scalePID_i_H ## hindex() { copy_and_scalePID_i(hindex); } \
      void copy_and_scalePID_d_H ## hindex() { copy_and_scalePID_d(hindex); }

    #if ENABLED(PID_AUTOTUNE_MENU)
      #define _PIDTEMP_FUNCTIONS(hindex) \
        _PIDTEMP_BASE_FUNCTIONS(hindex); \
        void lcd_autotune_callback_H ## hindex() { _lcd_autotune(hindex); }
    #else
      #define _PIDTEMP_FUNCTIONS(hindex) _PIDTEMP_BASE_FUNCTIONS(hindex)
    #endif

    _PIDTEMP_FUNCTIONS(0);
    #if HOTENDS > 1
      _PIDTEMP_FUNCTIONS(1);
      #if HOTENDS > 2
        _PIDTEMP_FUNCTIONS(2);
        #if HOTENDS > 3
          _PIDTEMP_FUNCTIONS(3);
        #endif // HOTENDS > 3
      #endif // HOTENDS > 2
    #endif // HOTENDS > 1

  #endif // PIDTEMP

  #if DISABLED(LASERBEAM)

    /**
     *
     * "Control" > "Temperature" submenu
     *
     */
    static void lcd_control_temperature_menu() {
      START_MENU();

      //
      // ^ Control
      //
      MENU_ITEM(back, MSG_CONTROL);

      #if ENABLED(PIDTEMP)
        #define _PID_BASE_MENU_ITEMS(HLABEL, hindex) \
          raw_Ki = unscalePID_i(PID_PARAM(Ki, hindex)); \
          raw_Kd = unscalePID_d(PID_PARAM(Kd, hindex)); \
          MENU_ITEM_EDIT(float52, MSG_PID_P HLABEL, &PID_PARAM(Kp, hindex), 1, 9990); \
          MENU_ITEM_EDIT_CALLBACK(float52, MSG_PID_I HLABEL, &raw_Ki, 0.01, 9990, copy_and_scalePID_i_H ## hindex); \
          MENU_ITEM_EDIT_CALLBACK(float52, MSG_PID_D HLABEL, &raw_Kd, 1, 9990, copy_and_scalePID_d_H ## hindex)

        #if ENABLED(PID_ADD_EXTRUSION_RATE)
          #define _PID_MENU_ITEMS(HLABEL, hindex) \
            _PID_BASE_MENU_ITEMS(HLABEL, hindex); \
            MENU_ITEM_EDIT(float3, MSG_PID_C HLABEL, &PID_PARAM(Kc, hindex), 1, 9990)
        #else
          #define _PID_MENU_ITEMS(HLABEL, hindex) _PID_BASE_MENU_ITEMS(HLABEL, hindex)
        #endif

        #if ENABLED(PID_AUTOTUNE_MENU)
          #define PID_MENU_ITEMS(HLABEL, hindex) \
            _PID_MENU_ITEMS(HLABEL, hindex); \
            MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(int3, MSG_PID_AUTOTUNE HLABEL, &autotune_temp[hindex], 150, heater_maxtemp[hindex] - 15, lcd_autotune_callback_H ## hindex)
        #else
          #define PID_MENU_ITEMS(HLABEL, hindex) _PID_MENU_ITEMS(HLABEL, hindex)
        #endif

        PID_MENU_ITEMS("", 0);
        #if HOTENDS > 1
          PID_MENU_ITEMS(MSG_H1, 1);
          #if HOTENDS > 2
            PID_MENU_ITEMS(MSG_H2, 2);
            #if HOTENDS > 3
              PID_MENU_ITEMS(MSG_H3, 3);
            #endif // HOTENDS > 3
          #endif // HOTENDS > 2
        #endif // HOTENDS > 1
      #endif // PIDTEMP

      //
      // Idle oozing
      //
      #if ENABLED(IDLE_OOZING_PREVENT)
        MENU_ITEM_EDIT(bool, MSG_IDLEOOZING, &IDLE_OOZING_enabled);
      #endif

      END_MENU();
    }

  #endif // !LASERBEAM

  static void _reset_acceleration_rates() { planner.reset_acceleration_rates(); }
  static void _planner_refresh_positioning() { planner.refresh_positioning(); }

  /**
   *
   * "Control" > "Motion" submenu
   *
   */
  static void lcd_control_motion_menu() {
    START_MENU();
    MENU_ITEM(back, MSG_CONTROL);
    #if HAS(BED_PROBE)
      MENU_ITEM_EDIT(float32, MSG_ZPROBE_ZOFFSET, &zprobe_zoffset, Z_PROBE_OFFSET_RANGE_MIN, Z_PROBE_OFFSET_RANGE_MAX);
    #endif
    // Manual bed leveling, Bed Z:
    #if(CON_SENSOR_INDUCTIVO)
      MENU_ITEM_EDIT(float43, MSG_BED_Z, &mbl.z_offset, -1, 1);
    #endif
    MENU_ITEM_EDIT(float5, MSG_ACC, &planner.acceleration, 10, 99000);
    MENU_ITEM_EDIT(float3, MSG_VXY_JERK, &planner.max_xy_jerk, 1, 990);
    #if MECH(DELTA)
      MENU_ITEM_EDIT(float3, MSG_VZ_JERK, &planner.max_z_jerk, 1, 990);
    #else
      MENU_ITEM_EDIT(float52, MSG_VZ_JERK, &planner.max_z_jerk, 0.1, 990);
    #endif
    MENU_ITEM_EDIT(float3, MSG_VMAX MSG_X, &planner.max_feedrate_mm_s[X_AXIS], 1, 999);
    MENU_ITEM_EDIT(float3, MSG_VMAX MSG_Y, &planner.max_feedrate_mm_s[Y_AXIS], 1, 999);
    MENU_ITEM_EDIT(float3, MSG_VMAX MSG_Z, &planner.max_feedrate_mm_s[Z_AXIS], 1, 999);
    MENU_ITEM_EDIT(float3, MSG_VMIN, &planner.min_feedrate_mm_s, 0, 999);
    MENU_ITEM_EDIT(float3, MSG_VTRAV_MIN, &planner.min_travel_feedrate_mm_s, 0, 999);
    MENU_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_X, &planner.max_acceleration_mm_per_s2[X_AXIS], 100, 99000, _reset_acceleration_rates);
    MENU_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_Y, &planner.max_acceleration_mm_per_s2[Y_AXIS], 100, 99000, _reset_acceleration_rates);
    MENU_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_Z, &planner.max_acceleration_mm_per_s2[Z_AXIS], 10, 99000, _reset_acceleration_rates);
    MENU_ITEM_EDIT(float5, MSG_A_TRAVEL, &planner.travel_acceleration, 100, 99000);
    MENU_ITEM_EDIT_CALLBACK(float52, MSG_XSTEPS, &planner.axis_steps_per_mm[X_AXIS], 5, 9999, _planner_refresh_positioning);
    MENU_ITEM_EDIT_CALLBACK(float52, MSG_YSTEPS, &planner.axis_steps_per_mm[Y_AXIS], 5, 9999, _planner_refresh_positioning);
    #if MECH(DELTA)
      MENU_ITEM_EDIT_CALLBACK(float52, MSG_ZSTEPS, &planner.axis_steps_per_mm[Z_AXIS], 5, 9999, _planner_refresh_positioning);
    #else
      MENU_ITEM_EDIT_CALLBACK(float51, MSG_ZSTEPS, &planner.axis_steps_per_mm[Z_AXIS], 5, 9999, _planner_refresh_positioning);
    #endif
    #if EXTRUDERS > 0
      MENU_ITEM_EDIT(float3, MSG_VE_JERK MSG_E "0", &planner.max_e_jerk[0], 1, 990);
      MENU_ITEM_EDIT(float3, MSG_VMAX MSG_E "0", &planner.max_feedrate_mm_s[E_AXIS], 1, 999);
      MENU_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_E "0", &planner.max_acceleration_mm_per_s2[E_AXIS], 100, 99000, _reset_acceleration_rates);
      MENU_ITEM_EDIT(float5, MSG_A_RETRACT MSG_E "0", &planner.retract_acceleration[0], 100, 99000);
      MENU_ITEM_EDIT_CALLBACK(float51, MSG_E0STEPS, &planner.axis_steps_per_mm[E_AXIS], 5, 9999, _planner_refresh_positioning);
      #if EXTRUDERS > 1
        MENU_ITEM_EDIT(float3, MSG_VE_JERK MSG_E "1", &planner.max_e_jerk[1], 1, 990);
        MENU_ITEM_EDIT(float3, MSG_VMAX MSG_E "1", &planner.max_feedrate_mm_s[E_AXIS + 1], 1, 999);
        MENU_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_E "1", &planner.max_acceleration_mm_per_s2[E_AXIS + 1], 100, 99000, _reset_acceleration_rates);
        MENU_ITEM_EDIT(float5, MSG_A_RETRACT MSG_E "1", &planner.retract_acceleration[1], 100, 99000);
        MENU_ITEM_EDIT_CALLBACK(float51, MSG_E1STEPS, &planner.axis_steps_per_mm[E_AXIS + 1], 5, 9999, _planner_refresh_positioning);
        #if EXTRUDERS > 2
          MENU_ITEM_EDIT(float3, MSG_VE_JERK MSG_E "2", &planner.max_e_jerk[2], 1, 990);
          MENU_ITEM_EDIT(float3, MSG_VMAX MSG_E "2", &planner.max_feedrate_mm_s[E_AXIS + 2], 1, 999);
          MENU_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_E "2", &planner.max_acceleration_mm_per_s2[E_AXIS + 2], 100, 99000, _reset_acceleration_rates);
          MENU_ITEM_EDIT(float5, MSG_A_RETRACT MSG_E "2", &planner.retract_acceleration[2], 100, 99000);
          MENU_ITEM_EDIT_CALLBACK(float51, MSG_E2STEPS, &planner.axis_steps_per_mm[E_AXIS + 2], 5, 9999, _planner_refresh_positioning);
          #if EXTRUDERS > 3
            MENU_ITEM_EDIT(float3, MSG_VE_JERK MSG_E  "3", &planner.max_e_jerk[3], 1, 990);
            MENU_ITEM_EDIT(float3, MSG_VMAX MSG_E "3", &planner.max_feedrate_mm_s[E_AXIS + 3], 1, 999);
            MENU_ITEM_EDIT_CALLBACK(long5, MSG_AMAX MSG_E "3", &planner.max_acceleration_mm_per_s2[E_AXIS + 3], 100, 99000, _reset_acceleration_rates);
            MENU_ITEM_EDIT(float5, MSG_A_RETRACT MSG_E "3", &planner.retract_acceleration[3], 100, 99000);
            MENU_ITEM_EDIT_CALLBACK(float51, MSG_E3STEPS, &planner.axis_steps_per_mm[E_AXIS + 3], 5, 9999, _planner_refresh_positioning);
          #endif // EXTRUDERS > 3
        #endif // EXTRUDERS > 2
      #endif // EXTRUDERS > 1
    #endif // EXTRUDERS > 0

    #if ENABLED(ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED)
      MENU_ITEM_EDIT(bool, MSG_ENDSTOP_ABORT, &abort_on_endstop_hit);
    #endif
    #if MECH(SCARA)
      MENU_ITEM_EDIT(float74, MSG_XSCALE, &axis_scaling[X_AXIS], 0.5, 2);
      MENU_ITEM_EDIT(float74, MSG_YSCALE, &axis_scaling[Y_AXIS], 0.5, 2);
    #endif
    END_MENU();
  }

  /**
   *
   * "Control" > "Filament" submenu
   *
   */
  #if DISABLED(LASERBEAM)
    static void lcd_control_volumetric_menu() {
      START_MENU();
      MENU_ITEM(back, MSG_CONTROL);

      MENU_ITEM_EDIT_CALLBACK(bool, MSG_VOLUMETRIC_ENABLED, &volumetric_enabled, calculate_volumetric_multipliers);

      if (volumetric_enabled) {
        #if EXTRUDERS == 1
          MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, MSG_FILAMENT_SIZE_EXTRUDER, &filament_size[0], DEFAULT_NOMINAL_FILAMENT_DIA - .5, DEFAULT_NOMINAL_FILAMENT_DIA + .5, calculate_volumetric_multipliers);
        #else // EXTRUDERS > 1
          MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, MSG_FILAMENT_SIZE_EXTRUDER " 0", &filament_size[0], DEFAULT_NOMINAL_FILAMENT_DIA - .5, DEFAULT_NOMINAL_FILAMENT_DIA + .5, calculate_volumetric_multipliers);
          MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, MSG_FILAMENT_SIZE_EXTRUDER " 1", &filament_size[1], DEFAULT_NOMINAL_FILAMENT_DIA - .5, DEFAULT_NOMINAL_FILAMENT_DIA + .5, calculate_volumetric_multipliers);
          #if EXTRUDERS > 2
            MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, MSG_FILAMENT_SIZE_EXTRUDER " 2", &filament_size[2], DEFAULT_NOMINAL_FILAMENT_DIA - .5, DEFAULT_NOMINAL_FILAMENT_DIA + .5, calculate_volumetric_multipliers);
            #if EXTRUDERS > 3
              MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, MSG_FILAMENT_SIZE_EXTRUDER " 3", &filament_size[3], DEFAULT_NOMINAL_FILAMENT_DIA - .5, DEFAULT_NOMINAL_FILAMENT_DIA + .5, calculate_volumetric_multipliers);
            #endif // EXTRUDERS > 3
          #endif // EXTRUDERS > 2
        #endif // EXTRUDERS > 1
      }

      END_MENU();
    }
  #endif // !LASERBEAM

  /**
   *
   * "Control" > "Contrast" submenu
   *
   */
  #if HAS(LCD_CONTRAST)
    static void lcd_set_contrast() {
      if (LCD_CLICKED) { lcd_goto_previous_menu(true); return; }
      ENCODER_DIRECTION_NORMAL();
      if (encoderPosition) {
        set_lcd_contrast(lcd_contrast + encoderPosition);
        encoderPosition = 0;
        lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
      }
      if (lcdDrawUpdate) {
        lcd_implementation_drawedit(PSTR(MSG_CONTRAST),
          #if LCD_CONTRAST_MAX >= 100
            itostr3(lcd_contrast)
          #else
            itostr2(lcd_contrast)
          #endif
        );
      }
    }
  #endif // HAS(LCD_CONTRAST)

  /**
   *
   * "Control" > "Retract" submenu
   *
   */
  #if ENABLED(FWRETRACT)
    static void lcd_control_retract_menu() {
      START_MENU();
      MENU_ITEM(back, MSG_CONTROL);
      MENU_ITEM_EDIT(bool, MSG_AUTORETRACT, &autoretract_enabled);
      MENU_ITEM_EDIT(float52, MSG_CONTROL_RETRACT, &retract_length, 0, 100);
      #if EXTRUDERS > 1
        MENU_ITEM_EDIT(float52, MSG_CONTROL_RETRACT_SWAP, &retract_length_swap, 0, 100);
      #endif
      MENU_ITEM_EDIT(float3, MSG_CONTROL_RETRACTF, &retract_feedrate, 1, 999);
      MENU_ITEM_EDIT(float52, MSG_CONTROL_RETRACT_ZLIFT, &retract_zlift, 0, 999);
      MENU_ITEM_EDIT(float52, MSG_CONTROL_RETRACT_RECOVER, &retract_recover_length, 0, 100);
      #if EXTRUDERS > 1
        MENU_ITEM_EDIT(float52, MSG_CONTROL_RETRACT_RECOVER_SWAP, &retract_recover_length_swap, 0, 100);
      #endif
      MENU_ITEM_EDIT(float3, MSG_CONTROL_RETRACT_RECOVERF, &retract_recover_feedrate, 1, 999);
      END_MENU();
    }
  #endif // FWRETRACT

  #if ENABLED(LASERBEAM)

    static void lcd_laser_menu() {
      START_MENU();
      MENU_ITEM(back, MSG_MAIN);
      MENU_ITEM(submenu, "Set Focus", lcd_laser_focus_menu);
      MENU_ITEM(submenu, "Test Fire", lcd_laser_test_fire_menu);
      #if ENABLED(LASER_PERIPHERALS)
        if (laser_peripherals_ok()) {
          MENU_ITEM(function, "Turn On Pumps/Fans", action_laser_acc_on);
        }
        else if (!(planner.movesplanned() || IS_SD_PRINTING)) {
          MENU_ITEM(function, "Turn Off Pumps/Fans", action_laser_acc_off);
        }
      #endif // LASER_PERIPHERALS
      END_MENU();
    }

    static void lcd_laser_test_fire_menu() {
      START_MENU();
       MENU_ITEM(back, "Laser Functions");
       MENU_ITEM(function, " 20%  50ms", action_laser_test_20_50ms);
       MENU_ITEM(function, " 20% 100ms", action_laser_test_20_100ms);
       MENU_ITEM(function, "100%  50ms", action_laser_test_100_50ms);
       MENU_ITEM(function, "100% 100ms", action_laser_test_100_100ms);
       MENU_ITEM(function, "Warm-up Laser 2sec", action_laser_test_warm);
       END_MENU();
    }

    static void action_laser_acc_on() { enqueue_and_echo_commands_P(PSTR("M80")); }
    static void action_laser_acc_off() { enqueue_and_echo_commands_P(PSTR("M81")); }
    static void action_laser_test_20_50ms() { laser_test_fire(20, 50); }
    static void action_laser_test_20_100ms() { laser_test_fire(20, 100); }
    static void action_laser_test_100_50ms() { laser_test_fire(100, 50); }
    static void action_laser_test_100_100ms() { laser_test_fire(100, 100); }
    static void action_laser_test_warm() { laser_test_fire(15, 2000); }

    static void laser_test_fire(uint8_t power, int dwell) {
      enqueue_and_echo_commands_P(PSTR("M80"));  // Enable laser accessories since we don't know if its been done (and there's no penalty for doing it again).
      laser_fire(power);
      delay(dwell);
      laser_extinguish();
    }

    float focalLength = 0;
    static void lcd_laser_focus_menu() {
      START_MENU();
      MENU_ITEM(back, "Laser Functions");
      MENU_ITEM(function, "1mm", action_laser_focus_1mm);
      MENU_ITEM(function, "2mm", action_laser_focus_2mm);
      MENU_ITEM(function, "3mm - 1/8in", action_laser_focus_3mm);
      MENU_ITEM(function, "4mm", action_laser_focus_4mm);
      MENU_ITEM(function, "5mm", action_laser_focus_5mm);
      MENU_ITEM(function, "6mm - 1/4in", action_laser_focus_6mm);
      MENU_ITEM(function, "7mm", action_laser_focus_7mm);
      MENU_ITEM_EDIT_CALLBACK(float32, "Custom", &focalLength, 0, LASER_FOCAL_HEIGHT, action_laser_focus_custom);
      END_MENU();
    }

    static void action_laser_focus_custom() { laser_set_focus(focalLength); }
    static void action_laser_focus_1mm() { laser_set_focus(1); }
    static void action_laser_focus_2mm() { laser_set_focus(2); }
    static void action_laser_focus_3mm() { laser_set_focus(3); }
    static void action_laser_focus_4mm() { laser_set_focus(4); }
    static void action_laser_focus_5mm() { laser_set_focus(5); }
    static void action_laser_focus_6mm() { laser_set_focus(6); }
    static void action_laser_focus_7mm() { laser_set_focus(7); }

    static void laser_set_focus(float f_length) {
      if (!axis_homed[Z_AXIS]) {
        enqueue_and_echo_commands_P(PSTR("G28 Z F150"));
      }
      focalLength = f_length;
      float focus = LASER_FOCAL_HEIGHT - f_length;
      char cmd[20];

      sprintf_P(cmd, PSTR("G0 Z%s F150"), ftostr52sign(focus));
      enqueue_and_echo_commands_P(cmd);
    }

  #endif // LASERBEAM

  #if ENABLED(SDSUPPORT)

    #if !PIN_EXISTS(SD_DETECT)
      static void lcd_goto_screen(menu_restart_yes_not, true, 0);() {
        card.mount();
        encoderTopLine = 0;
      }
    #endif

    static void lcd_sd_updir() {
      card.updir();
      encoderTopLine = 0;
    }

    /**
     *
     * "Print from SD" submenu
     *
     */
     void salir_sd(/* arguments */) {
       lcd_goto_screen(Kuttercraft_menu, true, 1);
     }
    void lcd_sdcard_menu() {
      //establese el movimiento de encoder
      ENCODER_DIRECTION_MENUS();

      if (lcdDrawUpdate == 0 && LCD_CLICKED == 0) return; // nothing to do (so don't thrash the SD card)

      //genera la variable para los nombres de la sd
      uint16_t fileCnt = card.getnrfilenames();
      START_MENU();
      //lo que permite salir de la sd
      MENU_ITEM(function, MSG_MAIN "              " LCD_STR_UPLEVEL, salir_sd);

      //obtiene para los nombres de la sd
      card.getWorkDirName();
      //pregunta si se esta adentro de una carpeta
      //de ser cierto genera una salida
      if (!(fullName[0] == '/')) {
        MENU_ITEM(function, LCD_STR_FOLDER MSG_VOL_CARP, lcd_sd_updir);
      }

      //GENERA UNA LISTA DE LOS ARCHIVOS DE LA SD
      for (uint16_t i = 0; i < fileCnt; i++) {
        if (_menuLineNr == _thisItemNr) {
          card.getfilename(
            #if ENABLED(SDCARD_RATHERRECENTFIRST)
              fileCnt-1 -
            #endif
            i
          );
          //es una carpeta o un archivo
          if (card.filenameIsDir)
            MENU_ITEM(sddirectory, MSG_CARD_MENU, fullName);
          else
            //es un archivo
            MENU_ITEM(sdfile, MSG_CARD_MENU, fullName);
        }
        else {
          MENU_ITEM_DUMMY();
        }
      }
      //FINAL DEL MENU
      END_MENU();
    }

  #endif // SDSUPPORT

  #if ENABLED(FILAMENT_CHANGE_FEATURE)
    static void menus_de_cambio_de_filamento();
    static void lcd_filament_change_resume_print() {
      filament_change_menu_response = 4;
      lcdDrawUpdate = 2;
      lcd_goto_screen(lcd_status_screen, true);
    }
    static void salida_de_un_circulo_de_carga_de_filamento_2() {
      filament_change_menu_response = 9;
      lcd_goto_screen(menus_de_cambio_de_filamento, true , 0);
    }

    static void salida_de_un_circulo_de_carga_de_filamento_3() {
      filament_change_menu_response = 9;
      lcd_goto_screen(menus_de_cambio_de_filamento, true , 1);
    }

    static void lcd_filament_change_extrude_more() {
      filament_change_menu_response = 2;
      barra_de_espera(250, 16, MSG_CARGAR, salida_de_un_circulo_de_carga_de_filamento_2);
    }
    static void lcd_filament_change_extrude_less() {
      filament_change_menu_response = 3;
      barra_de_espera(450, 13, MSG_RETIRAR, salida_de_un_circulo_de_carga_de_filamento_3);
    }
    //botones del menu de cambio de filamento
    void boton_reanudar_retirar_f(int valor) {
      if (valor == 1){
        u8g.setPrintPos(15, 63);
        u8g.setColorIndex(0);

        lcd_printPGM(PSTR(MSG_RETIRAR));

        u8g.setColorIndex(1);

        u8g.drawBitmapP(49,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, retirar_boton_b);
        if (LCD_CLICKED) {
          filament_change_menu_response = 3;
          lcd_goto_screen(lcd_filament_change_extrude_less, false);
        }
      }else{
        u8g.drawBitmapP(49,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, retirar_boton_a);
      }
    }
    //
    void boton_reanudar_cargar_f(int valor) {
      if (valor == 0){
        u8g.setPrintPos(16, 63);
        u8g.setColorIndex(0);

        lcd_printPGM(PSTR(MSG_CARGAR));

        u8g.setColorIndex(1);

        u8g.drawBitmapP(9,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, cargar_boton_b);
        if (LCD_CLICKED) {
          lcd_goto_screen(lcd_filament_change_extrude_more, false);
        }
      }else{
        u8g.drawBitmapP(9,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, cargar_boton_a);
      }
    }
    //
    void boton_reanudar_impresion(int valor) {
      if (valor == 2){
        u8g.setPrintPos(10, 63);
        u8g.setColorIndex(0);

        lcd_printPGM(PSTR(MSG_FILAMENT_CHANGE_OPTION_RESUME));

        u8g.setColorIndex(1);

        u8g.drawBitmapP(89,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, seguir_boton_b);
        if (LCD_CLICKED) {
          lcd_goto_screen(lcd_filament_change_resume_print, true);
        }
      }else{
        u8g.drawBitmapP(89,18,STATUS_EXP_BY_TE_WIDTH,STATUS_EXP_HEIGHT, seguir_boton_a);
      }
    }
    //menu de cambio de filamento
    void menus_de_cambio_de_filamento() {
      defer_return_to_status = true;
      u8g.drawBox(0, 55, 128, 10);
      u8g.drawBox(0, 0, 128, 10);
      u8g.setPrintPos(7, 8);
      u8g.setColorIndex(0);
      lcd_printPGM(PSTR(MSG_FILAMENT_CHANGE_HEADER));
      u8g.setColorIndex(1);
      if (encoderPosition > 1 && encoderPosition < 50){
        encoderPosition = 2;
      }
      if (encoderPosition > 50){
        encoderPosition = 0;
      }
      posicion_anterior = 0;
      if(LCD_CLICKED){defer_return_to_status = false;}


      boton_reanudar_cargar_f(encoderPosition);
      boton_reanudar_retirar_f(encoderPosition);
      boton_reanudar_impresion(encoderPosition);
  }

    static void lcd_filament_change_option_menu() {
      START_MENU();
      #if LCD_HEIGHT > 2
        STATIC_ITEM(MSG_FILAMENT_CHANGE_OPTION_HEADER, true, false);
      #endif
      MENU_ITEM(function, MSG_FILAMENT_CHANGE_OPTION_RESUME, lcd_filament_change_resume_print);
      MENU_ITEM(function, MSG_FILAMENT_CHANGE_OPTION_EXTRUDE, lcd_filament_change_extrude_more);
      END_MENU();
    }

    static void lcd_filament_change_init_message() {
      START_SCREEN();
      STATIC_ITEM(MSG_FILAMENT_CHANGE_HEADER, true, true);
      STATIC_ITEM(MSG_FILAMENT_CHANGE_INIT_1);
      #ifdef MSG_FILAMENT_CHANGE_INIT_2
        STATIC_ITEM(MSG_FILAMENT_CHANGE_INIT_2);
      #endif
      #ifdef MSG_FILAMENT_CHANGE_INIT_3
        STATIC_ITEM(MSG_FILAMENT_CHANGE_INIT_3);
      #endif
      END_SCREEN();
    }

    static void lcd_filament_change_unload_message() {
      START_SCREEN();
      STATIC_ITEM(MSG_FILAMENT_CHANGE_HEADER, true, true);
      STATIC_ITEM(MSG_FILAMENT_CHANGE_UNLOAD_1);
      #ifdef MSG_FILAMENT_CHANGE_UNLOAD_2
        STATIC_ITEM(MSG_FILAMENT_CHANGE_UNLOAD_2);
      #endif
      END_SCREEN();
    }

    static void lcd_filament_change_insert_message() {
      START_SCREEN();
      STATIC_ITEM(MSG_FILAMENT_CHANGE_HEADER, true, true);
      STATIC_ITEM(MSG_FILAMENT_CHANGE_INSERT_1);
      #ifdef MSG_FILAMENT_CHANGE_INSERT_2
        STATIC_ITEM(MSG_FILAMENT_CHANGE_INSERT_2);
      #endif
      END_SCREEN();
    }

    static void lcd_filament_change_load_message() {
      START_SCREEN();
      STATIC_ITEM(MSG_FILAMENT_CHANGE_HEADER, true, true);
      STATIC_ITEM(MSG_FILAMENT_CHANGE_LOAD_1);
      #ifdef MSG_FILAMENT_CHANGE_LOAD_2
        STATIC_ITEM(MSG_FILAMENT_CHANGE_LOAD_2);
      #endif
      END_SCREEN();
    }

    static void lcd_filament_change_extrude_message() {
      START_SCREEN();
      STATIC_ITEM(MSG_FILAMENT_CHANGE_HEADER, true, true);
      STATIC_ITEM(MSG_FILAMENT_CHANGE_EXTRUDE_1);
      #ifdef MSG_FILAMENT_CHANGE_EXTRUDE_2
        STATIC_ITEM(MSG_FILAMENT_CHANGE_EXTRUDE_2);
      #endif
      END_SCREEN();
    }

    static void lcd_filament_change_resume_message() {
      START_SCREEN();
      STATIC_ITEM(MSG_FILAMENT_CHANGE_HEADER, true, true);
      STATIC_ITEM(MSG_FILAMENT_CHANGE_RESUME_1);
      #ifdef MSG_FILAMENT_CHANGE_RESUME_2
        STATIC_ITEM(MSG_FILAMENT_CHANGE_RESUME_2);
      #endif
      END_SCREEN();
    }

    void lcd_filament_change_show_message(FilamentChangeMessage message) {
      switch (message) {
        case FILAMENT_CHANGE_MESSAGE_INIT:
          defer_return_to_status = true;
          lcd_goto_screen(lcd_filament_change_init_message);
          break;
        case FILAMENT_CHANGE_MESSAGE_UNLOAD:
          lcd_goto_screen(lcd_filament_change_unload_message);
          break;
        case FILAMENT_CHANGE_MESSAGE_INSERT:
          lcd_goto_screen(lcd_filament_change_insert_message);
          break;
        case FILAMENT_CHANGE_MESSAGE_LOAD:
          lcd_goto_screen(lcd_filament_change_load_message);
          break;
        case FILAMENT_CHANGE_MESSAGE_EXTRUDE:
          lcd_goto_screen(lcd_filament_change_extrude_message);
          break;
        case FILAMENT_CHANGE_MESSAGE_OPTION:
          filament_change_menu_response = FILAMENT_CHANGE_RESPONSE_WAIT_FOR;
          lcd_goto_screen(menus_de_cambio_de_filamento, true , 1);
          break;
        case FILAMENT_CHANGE_MESSAGE_RESUME:
          lcd_goto_screen(lcd_filament_change_resume_message);
          break;
        case FILAMENT_CHANGE_MESSAGE_STATUS:
          lcd_return_to_status();
          break;
      }
    }

  #endif // FILAMENT_CHANGE_FEATURE

  /**
   *
   * Functions for editing single values
   *
   * The "menu_edit_type" macro generates the functions needed to edit a numerical value.
   *
   * For example, menu_edit_type(int, int3, itostr3, 1) expands into these functions:
   *
   *   bool _menu_edit_int3();
   *   void menu_edit_int3(); // edit int (interactively)
   *   void menu_edit_callback_int3(); // edit int (interactively) with callback on completion
   *   static void _menu_action_setting_edit_int3(const char* pstr, int* ptr, int minValue, int maxValue);
   *   static void menu_action_setting_edit_int3(const char* pstr, int* ptr, int minValue, int maxValue);
   *   static void menu_action_setting_edit_callback_int3(const char* pstr, int* ptr, int minValue, int maxValue, screenFunc_t callback); // edit int with callback
   *
   * You can then use one of the menu macros to present the edit interface:
   *   MENU_ITEM_EDIT(int3, MSG_SPEED, &feedrate_percentage, 10, 999)
   *
   * This expands into a more primitive menu item:
   *   MENU_ITEM(setting_edit_int3, MSG_SPEED, PSTR(MSG_SPEED), &feedrate_percentage, 10, 999)
   *
   *
   * Also: MENU_MULTIPLIER_ITEM_EDIT, MENU_ITEM_EDIT_CALLBACK, and MENU_MULTIPLIER_ITEM_EDIT_CALLBACK
   *
   *       menu_action_setting_edit_int3(PSTR(MSG_SPEED), &feedrate_percentage, 10, 999)
   */
  #define menu_edit_type(_type, _name, _strFunc, scale) \
    bool _menu_edit_ ## _name () { \
      ENCODER_DIRECTION_NORMAL(); \
      bool isClicked = LCD_CLICKED; \
      if ((int32_t)encoderPosition < 0) encoderPosition = 0; \
      if ((int32_t)encoderPosition > maxEditValue) encoderPosition = maxEditValue; \
      if (lcdDrawUpdate) \
        lcd_implementation_drawedit(editLabel, _strFunc(((_type)((int32_t)encoderPosition + minEditValue)) / scale)); \
      if (isClicked) { \
        *((_type*)editValue) = ((_type)((int32_t)encoderPosition + minEditValue)) / scale; \
        lcd_goto_previous_menu(true); \
      } \
      return isClicked; \
    } \
    void menu_edit_ ## _name () { _menu_edit_ ## _name(); } \
    void menu_edit_callback_ ## _name () { if (_menu_edit_ ## _name ()) (*callbackFunc)(); } \
    static void _menu_action_setting_edit_ ## _name (const char* pstr, _type* ptr, _type minValue, _type maxValue) { \
      lcd_save_previous_menu(); \
      \
      lcdDrawUpdate = LCDVIEW_CLEAR_CALL_REDRAW; \
      \
      editLabel = pstr; \
      editValue = ptr; \
      minEditValue = minValue * scale; \
      maxEditValue = maxValue * scale - minEditValue; \
      encoderPosition = (*ptr) * scale - minEditValue; \
    } \
    static void menu_action_setting_edit_ ## _name (const char* pstr, _type* ptr, _type minValue, _type maxValue) { \
      _menu_action_setting_edit_ ## _name(pstr, ptr, minValue, maxValue); \
      currentScreen = menu_edit_ ## _name; \
    }\
    static void menu_action_setting_edit_callback_ ## _name (const char* pstr, _type* ptr, _type minValue, _type maxValue, screenFunc_t callback) { \
      _menu_action_setting_edit_ ## _name(pstr, ptr, minValue, maxValue); \
      currentScreen = menu_edit_callback_ ## _name; \
      callbackFunc = callback; \
    }
  menu_edit_type(int, int3, itostr3, 1);
  menu_edit_type(float, float3, ftostr3, 1);
  menu_edit_type(float, float32, ftostr32, 100);
  menu_edit_type(float, float43, ftostr43sign, 1000);
  menu_edit_type(float, float5, ftostr5rj, 0.01);
  menu_edit_type(float, float51, ftostr51sign, 10);
  menu_edit_type(float, float52, ftostr52sign, 100);
  menu_edit_type(unsigned long, long5, ftostr5rj, 0.01);

  /**
   *
   * Handlers for RepRap World Keypad input
   *
   */
  #if ENABLED(REPRAPWORLD_KEYPAD)
    static void reprapworld_keypad_move_z_up() {
      encoderPosition = 1;
      move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
      lcd_move_z();
    }
    static void reprapworld_keypad_move_z_down() {
      encoderPosition = -1;
      move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
      lcd_move_z();
    }
    static void reprapworld_keypad_move_x_left() {
      encoderPosition = -1;
      move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
      lcd_move_x();
    }
    static void reprapworld_keypad_move_x_right() {
      encoderPosition = 1;
      move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
      lcd_move_x();
    }
    static void reprapworld_keypad_move_y_down() {
      encoderPosition = 1;
      move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
      lcd_move_y();
    }
    static void reprapworld_keypad_move_y_up() {
      encoderPosition = -1;
      move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
      lcd_move_y();
    }
    static void reprapworld_keypad_move_home() {
      enqueue_and_echo_commands_P(PSTR("G28")); // move all axis home
    }
  #endif // REPRAPWORLD_KEYPAD


  /**
   *
   * Audio feedback for controller clicks
   *
   */

  #if ENABLED(LCD_USE_I2C_BUZZER)
    void lcd_buzz(long duration, uint16_t freq) { // called from buzz() in Marlin_main.cpp where lcd is unknown
      lcd.buzz(duration, freq);
    }
  #endif

  void lcd_quick_feedback() {
    lcdDrawUpdate = LCDVIEW_CLEAR_CALL_REDRAW;
    next_button_update_ms = millis() + 500;

    #if ENABLED(LCD_USE_I2C_BUZZER)
      #if DISABLED(LCD_FEEDBACK_FREQUENCY_HZ)
        #define LCD_FEEDBACK_FREQUENCY_HZ 100
      #endif
      #if DISABLED(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
        #define LCD_FEEDBACK_FREQUENCY_DURATION_MS (1000/6)
      #endif
      lcd.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
    #elif HAS(BUZZER)
      #if DISABLED(LCD_FEEDBACK_FREQUENCY_HZ)
        #define LCD_FEEDBACK_FREQUENCY_HZ 5000
      #endif
      #if DISABLED(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
        #define LCD_FEEDBACK_FREQUENCY_DURATION_MS 2
      #endif
      buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
    #else
      #if DISABLED(LCD_FEEDBACK_FREQUENCY_DURATION_MS)
        #define LCD_FEEDBACK_FREQUENCY_DURATION_MS 2
      #endif
      HAL::delayMilliseconds(LCD_FEEDBACK_FREQUENCY_DURATION_MS);
    #endif
  }

  /**
   *
   * Menu actions
   *
   */
  static void menu_action_back() { lcd_goto_previous_menu(); }
  static void menu_action_submenu(screenFunc_t func) { lcd_save_previous_menu(); lcd_goto_screen(func); }
  static void menu_action_gcode(const char* pgcode) { enqueue_and_echo_commands_P(pgcode); }
  static void menu_action_function(screenFunc_t func) { (*func)(); }

  #if ENABLED(SDSUPPORT)
    //la accion al tocar un archivo en la sd
    static void menu_action_sdfile(const char* longFilename) {
      contador_comandos_save = 0;
      solo_una_vez = 0;

      char cmd[30];
      char* c;

      sprintf_P(cmd, PSTR("M23 %s"), longFilename);
      for (c = &cmd[4]; *c; c++) *c = tolower(*c);

      if((strcmp(cmd, "M23 restart.gcode")) == 0){
        lcd_goto_screen(menu_restart_yes_not, true);

      }else{
        //indica que no hay que ignorar el calentamiento
        salida_de_emg_temp_hotend = true;
        salida_de_emg_temp_bed = true;
        //se guanda que se esta imprimiendo un archivo
        se_estaba_imprimiendo = true;
        //segurada el estado de impresion
        print_job_counter.imprimiendo_estado = true;
        //Accion al seleccionar una archivo a imprimir
        sprintf_P(cmd, PSTR("M23 %s"), longFilename);
        for (c = &cmd[4]; *c; c++) *c = tolower(*c);

        enqueue_and_echo_commands_P(PSTR("M500"));
        enqueue_and_echo_command(cmd);
        enqueue_and_echo_commands_P(PSTR("M24"));
        lcd_return_to_status();
      }

    }

    static void restart_gcode(const char* longFilename) {
      contador_comandos_save = 0;
      char cmd[30];
      char* c;
      //indica que no hay que ignorar el calentamiento
      salida_de_emg_temp_hotend = true;
      salida_de_emg_temp_bed = true;
      //se guanda que se esta imprimiendo un archivo
      se_estaba_imprimiendo = true;
      //segurada el estado de impresion
      print_job_counter.imprimiendo_estado = true;
      //para recordar que se viene de un corte de luz y ignorar el final del restart
      se_viene_de_un_corte_de_luz = true;

      //Accion al seleccionar una archivo a imprimir
      sprintf_P(cmd, PSTR("M23 %s"), longFilename);
      for (c = &cmd[4]; *c; c++) *c = tolower(*c);

      enqueue_and_echo_command(cmd);
      if(imprimir_desde_base){
        enqueue_and_echo_commands_P(PSTR("G28\nM118\nM24"));
      }else{
        enqueue_and_echo_commands_P(PSTR("M24"));
      }

      lcd_goto_screen(volver_info, true);
    }
    //se usa para crear las subcarpetas
    static void menu_action_sddirectory(const char* longFilename) {
      char nameFile[15];

      char cmd[50];
      char* c;

      strcpy(nameFile, "restart.gcode");

      sprintf_P(cmd, PSTR("M38 %s"), longFilename);
      for (c = &cmd[4]; *c; c++) *c = tolower(*c);


      strcat(cmd, "\nM23 restart.gcode\nM24");

      card.startWrite(nameFile, false);
      card.write_command(cmd);
      card.finishWrite();

      card.chdir(longFilename);
      encoderPosition = 0;
    }

  #endif // SDSUPPORT

  static void menu_action_setting_edit_bool(const char* pstr, bool* ptr) {UNUSED(pstr); *ptr = !(*ptr); }
  static void menu_action_setting_edit_callback_bool(const char* pstr, bool* ptr, screenFunc_t callback) {
    menu_action_setting_edit_bool(pstr, ptr);
    (*callback)();
  }

#endif // ULTIPANEL

/** LCD API **/
void lcd_init() {

  lcd_implementation_init();


  #if defined (LUZ_LED_KUTTERCARFT)
    pinMode(LUZ_KUTTERCRAFT, OUTPUT);
    analogWrite(LUZ_KUTTERCRAFT, 0);
  #endif
  //#if (KUTTERCRAFT_MULTIFILAMENT)
    //pinMode(11, OUTPUT);
  //  analogWrite(11, 0);
//  #endif

  #if ENABLED(NEWPANEL)
    #if BUTTON_EXISTS(EN1)
      SET_INPUT(BTN_EN1);
      PULLUP(BTN_EN1, HIGH);
    #endif

    #if BUTTON_EXISTS(EN2)
      SET_INPUT(BTN_EN2);
      PULLUP(BTN_EN2, HIGH);
    #endif

    #if BUTTON_EXISTS(ENC)
      SET_INPUT(BTN_ENC);
      PULLUP(BTN_ENC, HIGH);
    #endif

    #if ENABLED(REPRAPWORLD_KEYPAD)
      pinMode(SHIFT_CLK, OUTPUT);
      pinMode(SHIFT_LD, OUTPUT);
      pinMode(SHIFT_OUT, INPUT);
      PULLUP(SHIFT_OUT, HIGH);
      WRITE(SHIFT_LD, HIGH);
    #endif

    #if BUTTON_EXISTS(UP)
      SET_INPUT(BTN_UP);
    #endif
    #if BUTTON_EXISTS(DWN)
      SET_INPUT(BTN_DWN);
    #endif
    #if BUTTON_EXISTS(LFT)
      SET_INPUT(BTN_LFT);
    #endif
    #if BUTTON_EXISTS(RT)
      SET_INPUT(BTN_RT);
    #endif

  #else  // Not NEWPANEL

    #if ENABLED(SR_LCD_2W_NL) // Non latching 2 wire shift register
      pinMode(SR_DATA_PIN, OUTPUT);
      pinMode(SR_CLK_PIN, OUTPUT);
    #elif ENABLED(SHIFT_CLK)
      pinMode(SHIFT_CLK, OUTPUT);
      pinMode(SHIFT_LD, OUTPUT);
      pinMode(SHIFT_EN, OUTPUT);
      pinMode(SHIFT_OUT, INPUT);
      PULLUP(SHIFT_OUT, HIGH);
      WRITE(SHIFT_LD, HIGH);
      WRITE(SHIFT_EN, LOW);
    #endif // SR_LCD_2W_NL

  #endif // !NEWPANEL

  #if ENABLED(SDSUPPORT) && PIN_EXISTS(SD_DETECT)
    SET_INPUT(SD_DETECT_PIN);
    PULLUP(SD_DETECT_PIN, HIGH);
    lcd_sd_status = 2; // UNKNOWN
  #endif

  #if ENABLED(LCD_HAS_SLOW_BUTTONS)
    slow_buttons = 0;
  #endif

  lcd_buttons_update();

  #if ENABLED(ULTIPANEL)
    encoderDiff = 0;
  #endif
  esta_memoria_corrupta();
}

int lcd_strlen(const char* s) {
  int i = 0, j = 0;
  while (s[i]) {
    if ((s[i] & 0xc0) != 0x80) j++;
    i++;
  }
  return j;
}

int lcd_strlen_P(const char* s) {
  int j = 0;
  while (pgm_read_byte(s)) {
    if ((pgm_read_byte(s) & 0xc0) != 0x80) j++;
    s++;
  }
  return j;
}

bool lcd_blink() {
  static uint8_t blink = 0;
  static millis_t next_blink_ms = 0;
  millis_t ms = millis();
  if (ELAPSED(ms, next_blink_ms)) {
    blink ^= 0xFF;
    next_blink_ms = ms + 1000 - LCD_UPDATE_INTERVAL / 2;
  }
  return blink != 0;
}

/**
 * Update the LCD, read encoder buttons, etc.
 *   - Read button states
 *   - Check the SD Card slot state
 *   - Act on RepRap World keypad input
 *   - Update the encoder position
 *   - Apply acceleration to the encoder position
 *   - Set lcdDrawUpdate = LCDVIEW_CALL_REDRAW_NOW on controller events
 *   - Reset the Info Screen timeout if there's any input
 *   - Update status indicators, if any
 *
 *   Run the current LCD menu handler callback function:
 *   - Call the handler only if lcdDrawUpdate != LCDVIEW_NONE
 *   - Before calling the handler, LCDVIEW_CALL_NO_REDRAW => LCDVIEW_NONE
 *   - Call the menu handler. Menu handlers should do the following:
 *     - If a value changes, set lcdDrawUpdate to LCDVIEW_REDRAW_NOW and draw the value
 *       (Encoder events automatically set lcdDrawUpdate for you.)
 *     - if (lcdDrawUpdate) { redraw }
 *     - Before exiting the handler set lcdDrawUpdate to:
 *       - LCDVIEW_CLEAR_CALL_REDRAW to clear screen and set LCDVIEW_CALL_REDRAW_NEXT.
 *       - LCDVIEW_REDRAW_NOW or LCDVIEW_NONE to keep drawingm but only in this loop.
 *       - LCDVIEW_REDRAW_NEXT to keep drawing and draw on the next loop also.
 *       - LCDVIEW_CALL_NO_REDRAW to keep drawing (or start drawing) with no redraw on the next loop.
 *     - NOTE: For graphical displays menu handlers may be called 2 or more times per loop,
 *             so don't change lcdDrawUpdate without considering this.
 *
 *   After the menu handler callback runs (or not):
 *   - Clear the LCD if lcdDrawUpdate == LCDVIEW_CLEAR_CALL_REDRAW
 *   - Update lcdDrawUpdate for the next loop (i.e., move one state down, usually)
 *
 * No worries. This function is only called from the main thread.
 */
void lcd_update() {

  #if ENABLED(ULTIPANEL)
    static millis_t return_to_status_ms = 0;
    manage_manual_move();
  #endif
  lcd_buttons_update();

  #if ENABLED(SDSUPPORT) && PIN_EXISTS(SD_DETECT)

    bool sd_status = IS_SD_INSERTED;
    if (sd_status != lcd_sd_status && lcd_detected()) {
      lcdDrawUpdate = LCDVIEW_CLEAR_CALL_REDRAW;
      lcd_implementation_init( // to maybe revive the LCD if static electricity killed it.
        #if ENABLED(LCD_PROGRESS_BAR) && ENABLED(ULTIPANEL)
          currentScreen == lcd_status_screen
        #endif
      );

      if (sd_status) {
        card.mount();
        if (lcd_sd_status != 2) LCD_MESSAGEPGM(MSG_SD_INSERTED);
      }
      else {
        card.unmount();
        if (lcd_sd_status != 2) LCD_MESSAGEPGM(MSG_SD_REMOVED);
      }

      lcd_sd_status = sd_status;

    }
  #endif // SDSUPPORT && SD_DETECT_PI
  millis_t ms = millis();
  if (ELAPSED(ms, next_lcd_update_ms)) {

    next_lcd_update_ms = ms + LCD_UPDATE_INTERVAL;

    #if ENABLED(LCD_HAS_STATUS_INDICATORS)
      lcd_implementation_update_indicators();
    #endif

    #if ENABLED(LCD_HAS_SLOW_BUTTONS)
      slow_buttons = lcd_implementation_read_slow_buttons(); // buttons which take too long to read in interrupt context
    #endif

    #if ENABLED(ULTIPANEL)

      #if ENABLED(REPRAPWORLD_KEYPAD)

        #if MECH(DELTA) || MECH(SCARA)
          #define _KEYPAD_MOVE_ALLOWED (axis_homed[X_AXIS] && axis_homed[Y_AXIS] && axis_homed[Z_AXIS])
        #else
          #define _KEYPAD_MOVE_ALLOWED true
        #endif

        if (REPRAPWORLD_KEYPAD_MOVE_HOME)       reprapworld_keypad_move_home();
        if (_KEYPAD_MOVE_ALLOWED) {
          if (REPRAPWORLD_KEYPAD_MOVE_Z_UP)     reprapworld_keypad_move_z_up();
          if (REPRAPWORLD_KEYPAD_MOVE_Z_DOWN)   reprapworld_keypad_move_z_down();
          if (REPRAPWORLD_KEYPAD_MOVE_X_LEFT)   reprapworld_keypad_move_x_left();
          if (REPRAPWORLD_KEYPAD_MOVE_X_RIGHT)  reprapworld_keypad_move_x_right();
          if (REPRAPWORLD_KEYPAD_MOVE_Y_DOWN)   reprapworld_keypad_move_y_down();
          if (REPRAPWORLD_KEYPAD_MOVE_Y_UP)     reprapworld_keypad_move_y_up();
        }
      #endif

      bool encoderPastThreshold = (abs(encoderDiff) >= ENCODER_PULSES_PER_STEP);
      if (encoderPastThreshold || LCD_CLICKED) {
        if (encoderPastThreshold) {
          int32_t encoderMultiplier = 1;

          #if ENABLED(ENCODER_RATE_MULTIPLIER)

            if (encoderRateMultiplierEnabled) {
              int32_t encoderMovementSteps = abs(encoderDiff) / ENCODER_PULSES_PER_STEP;

              if (lastEncoderMovementMillis != 0) {
                // Note that the rate is always calculated between to passes through the
                // loop and that the abs of the encoderDiff value is tracked.
                float encoderStepRate = (float)(encoderMovementSteps) / ((float)(ms - lastEncoderMovementMillis)) * 1000.0;

                if (encoderStepRate >= ENCODER_100X_STEPS_PER_SEC)     encoderMultiplier = 10;
                else if (encoderStepRate >= ENCODER_10X_STEPS_PER_SEC) encoderMultiplier = 5;

                #if ENABLED(ENCODER_RATE_MULTIPLIER_DEBUG)
                  SERIAL_SMV(DEB, "Enc Step Rate: ", encoderStepRate);
                  SERIAL_MV("  Multiplier: ", encoderMultiplier);
                  SERIAL_MV("  ENCODER_10X_STEPS_PER_SEC: ", ENCODER_10X_STEPS_PER_SEC);
                  SERIAL_EMV("  ENCODER_100X_STEPS_PER_SEC: ", ENCODER_100X_STEPS_PER_SEC);
                #endif
              }

              lastEncoderMovementMillis = ms;
            } // encoderRateMultiplierEnabled
          #endif // ENCODER_RATE_MULTIPLIER

          encoderPosition += (encoderDiff * encoderMultiplier) / ENCODER_PULSES_PER_STEP;
          encoderDiff = 0;
        }
        return_to_status_ms = ms + LCD_TIMEOUT_TO_STATUS;
        lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
      }
    #endif // ULTIPANEL

    // We arrive here every ~100ms when idling often enough.
    // Instead of tracking the changes simply redraw the Info Screen ~1 time a second.
    static int8_t lcd_status_update_delay = 1; // first update one loop delayed

    if (!lcd_status_update_delay--){
      //pregunta si la temperatura es normal
      //ver_error_de_tempe();

      //pregunta por los logros
      if(!print_job_counter.imprimiendo_estado){
        ver_ventana_logros();
        ver_ventana_logros_cantidad();

        //mantenimiento
        //se encarga de guardar el status cada 3 minutes
        // if(esperar_tres_minutos >= 360){
        //   esperar_tres_minutos = 0;
        //   Config_StoreSettings();
        // }else{
        //   esperar_tres_minutos++;
        // }

      }

      lcd_status_update_delay = 9;
      if(currentScreen == lcd_status_screen) {
        lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
      }
    }

    if (lcdDrawUpdate) {
      switch (lcdDrawUpdate) {
        case LCDVIEW_CALL_NO_REDRAW:
          lcdDrawUpdate = LCDVIEW_NONE;
          break;
        case LCDVIEW_CLEAR_CALL_REDRAW: // set by handlers, then altered after (rarely occurs here)
        case LCDVIEW_CALL_REDRAW_NEXT:  // set by handlers, then altered after (never occurs here?)
          lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
        case LCDVIEW_REDRAW_NOW:        // set above, or by a handler through LCDVIEW_CALL_REDRAW_NEXT
        case LCDVIEW_NONE:
          break;
      }

      #if ENABLED(ULTIPANEL)
        #define CURRENTSCREEN() (*currentScreen)()
      #else
        #define CURRENTSCREEN() lcd_status_screen()
      #endif

      #if ENABLED(DOGLCD)  // Changes due to different driver architecture of the DOGM display
        static int8_t dot_color = 0;
        dot_color = 1 - dot_color;
        u8g.firstPage();
        do {
          lcd_setFont(FONT_MENU);
          u8g.setPrintPos(125, 0);
          u8g.setColorIndex(dot_color); // Set color for the alive dot
          u8g.drawPixel(127, 63); // draw alive dot
          u8g.setColorIndex(1); // black on white
          CURRENTSCREEN();
        } while (u8g.nextPage());
      #else
        CURRENTSCREEN();
      #endif
    }

    #if ENABLED(ULTIPANEL)
      // Return to Status Screen after a timeout
      if (currentScreen == lcd_status_screen || defer_return_to_status)
        return_to_status_ms = ms + LCD_TIMEOUT_TO_STATUS;
      else if (ELAPSED(ms, return_to_status_ms))
        lcd_return_to_status();

    #endif // ULTIPANEL

    switch (lcdDrawUpdate) {
      case LCDVIEW_CLEAR_CALL_REDRAW:
        lcd_implementation_clear();
      case LCDVIEW_CALL_REDRAW_NEXT:
        lcdDrawUpdate = LCDVIEW_REDRAW_NOW;
        break;
      case LCDVIEW_REDRAW_NOW:
        lcdDrawUpdate = LCDVIEW_NONE;
        break;
      case LCDVIEW_NONE:
        break;
    }
  }
}

void set_utf_strlen(char* s, uint8_t n) {
  uint8_t i = 0, j = 0;
  while (s[i] && (j < n)) {
    #if ENABLED(MAPPER_NON)
      j++;
    #else
      if ((s[i] & 0xC0u) != 0x80u) j++;
    #endif
    i++;
  }
  while (j++ < n) s[i++] = ' ';
  s[i] = '\0';
}

void lcd_finishstatus(bool persist = false) {
  set_utf_strlen(lcd_status_message, LCD_WIDTH);
  #if !(ENABLED(LCD_PROGRESS_BAR) && (PROGRESS_MSG_EXPIRE > 0))
    UNUSED(persist);
  #endif

  #if ENABLED(LCD_PROGRESS_BAR)
    progress_bar_ms = millis();
    #if PROGRESS_MSG_EXPIRE > 0
      expire_status_ms = persist ? 0 : progress_bar_ms + PROGRESS_MSG_EXPIRE;
    #endif
  #endif
  lcdDrawUpdate = LCDVIEW_CLEAR_CALL_REDRAW;

  #if HAS(LCD_FILAMENT_SENSOR) || HAS(LCD_POWER_SENSOR)
    previous_lcd_status_ms = millis();  //get status message to show up for a while
  #endif
}

#if ENABLED(LCD_PROGRESS_BAR) && PROGRESS_MSG_EXPIRE > 0
  void dontExpireStatus() { expire_status_ms = 0; }
#endif

bool lcd_hasstatus() { return (lcd_status_message[0] != '\0'); }

void lcd_setstatus(const char* message, bool persist) {
  if (lcd_status_message_level > 0) return;
  strncpy(lcd_status_message, message, 3 * (LCD_WIDTH));
  set_utf_strlen(lcd_status_message, LCD_WIDTH);
  lcd_finishstatus(persist);
}

void lcd_setstatuspgm(const char* message, uint8_t level) {
  if (level >= lcd_status_message_level) {
    strncpy_P(lcd_status_message, message, 3 * (LCD_WIDTH));
    set_utf_strlen(lcd_status_message, LCD_WIDTH);
    lcd_status_message_level = level;
    lcd_finishstatus(level > 0);
  }
}

void lcd_setalertstatuspgm(const char* message) {
  lcd_setstatuspgm(message, 1);
  #if ENABLED(ULTIPANEL)
    lcd_return_to_status();
  #endif
}

void lcd_reset_alert_level() { lcd_status_message_level = 0; }

#if HAS(LCD_CONTRAST)
  void set_lcd_contrast(int value) {
    lcd_contrast = constrain(value, LCD_CONTRAST_MIN, LCD_CONTRAST_MAX);
    u8g.setContrast(lcd_contrast);
  }
#endif

#if ENABLED(ULTIPANEL)

  /**
   * Setup Rotary Encoder Bit Values (for two pin encoders to indicate movement)
   * These values are independent of which pins are used for EN_A and EN_B indications
   * The rotary encoder part is also independent to the chipset used for the LCD
   */
  #if defined(EN_A) && defined(EN_B)
    #define encrot0 0
    #define encrot1 2
    #define encrot2 3
    #define encrot3 1
  #endif

  #define GET_BUTTON_STATES(DST) \
    uint8_t new_##DST = 0; \
    WRITE(SHIFT_LD, LOW); \
    WRITE(SHIFT_LD, HIGH); \
    for (int8_t i = 0; i < 8; i++) { \
      new_##DST >>= 1; \
      if (READ(SHIFT_OUT)) SBI(new_##DST, 7); \
      WRITE(SHIFT_CLK, HIGH); \
      WRITE(SHIFT_CLK, LOW); \
    } \
    DST = ~new_##DST; //invert it, because a pressed switch produces a logical 0


  /**
   * Read encoder buttons from the hardware registers
   * Warning: This function is called from interrupt context!
   */
  void lcd_buttons_update() {
    #if ENABLED(NEWPANEL)
      uint8_t newbutton = 0;
      #if BUTTON_EXISTS(EN1)
        if (BUTTON_PRESSED(EN1)) newbutton |= EN_A;
      #endif
      #if BUTTON_EXISTS(EN2)
        if (BUTTON_PRESSED(EN2)) newbutton |= EN_B;
      #endif
      #if LCD_HAS_DIRECTIONAL_BUTTONS || BUTTON_EXISTS(ENC)
        millis_t now = millis();
      #endif

      #if LCD_HAS_DIRECTIONAL_BUTTONS
        if (ELAPSED(now, next_button_update_ms)) {
          if (false) {
            // for the else-ifs below
          }
          #if BUTTON_EXISTS(UP)
            else if (BUTTON_PRESSED(UP)) {
              encoderDiff = -(ENCODER_STEPS_PER_MENU_ITEM);
              next_button_update_ms = now + 300;
            }
          #endif
          #if BUTTON_EXISTS(DWN)
            else if (BUTTON_PRESSED(DWN)) {
              encoderDiff = ENCODER_STEPS_PER_MENU_ITEM;
              next_button_update_ms = now + 300;
            }
          #endif
          #if BUTTON_EXISTS(LFT)
            else if (BUTTON_PRESSED(LFT)) {
              encoderDiff = -(ENCODER_PULSES_PER_STEP);
              next_button_update_ms = now + 300;
            }
          #endif
          #if BUTTON_EXISTS(RT)
            else if (BUTTON_PRESSED(RT)) {
              encoderDiff = ENCODER_PULSES_PER_STEP;
              next_button_update_ms = now + 300;
            }
          #endif
        }
      #endif

      #if BUTTON_EXISTS(ENC)
        if (ELAPSED(now, next_button_update_ms) && BUTTON_PRESSED(ENC)) newbutton |= EN_C;
      #endif

      buttons = newbutton;
      #if ENABLED(LCD_HAS_SLOW_BUTTONS)
        buttons |= slow_buttons;
      #endif
      #if ENABLED(REPRAPWORLD_KEYPAD)
        GET_BUTTON_STATES(buttons_reprapworld_keypad);
      #endif
    #else
      GET_BUTTON_STATES(buttons);
    #endif // !NEWPANEL

    // Manage encoder rotation
    #if ENABLED(INVERT_ROTARY_SWITCH)
      #define ENCODER_DIFF_CW  (encoderDiff += encoderDirection)
      #define ENCODER_DIFF_CCW (encoderDiff -= encoderDirection)
    #else
    //if(dir_encoder){
      #define ENCODER_DIFF_CW  (encoderDiff += dir_encoder)
      #define ENCODER_DIFF_CCW (encoderDiff -= dir_encoder)
    //}else{
      //#define ENCODER_DIFF_CW  (encoderDiff++)
      //#define ENCODER_DIFF_CCW (encoderDiff--)
    //}
    #endif
    #define ENCODER_SPIN(_E1, _E2) switch (lastEncoderBits) { case _E1: ENCODER_DIFF_CW; break; case _E2: ENCODER_DIFF_CCW; }

    uint8_t enc = 0;
    if (buttons & EN_A) enc |= B01;
    if (buttons & EN_B) enc |= B10;
    if (enc != lastEncoderBits) {
      switch (enc) {
        case encrot0: ENCODER_SPIN(encrot3, encrot1); break;
        case encrot1: ENCODER_SPIN(encrot0, encrot2); break;
        case encrot2: ENCODER_SPIN(encrot1, encrot3); break;
        case encrot3: ENCODER_SPIN(encrot2, encrot0); break;
      }
    }
    lastEncoderBits = enc;
  }

  bool lcd_detected(void) {
    #if (ENABLED(LCD_I2C_TYPE_MCP23017) || ENABLED(LCD_I2C_TYPE_MCP23008)) && ENABLED(DETECT_DEVICE)
      return lcd.LcdDetected() == 1;
    #else
      return true;
    #endif
  }

  bool lcd_clicked() { return LCD_CLICKED; }

#endif // ULTIPANEL

/*********************************/
/** Number to string conversion **/
/*********************************/

#define DIGIT(n) ('0' + (n))
#define DIGIMOD(n) DIGIT((n) % 10)

char conv[8];

// Convert float to rj string with 123 or -12 format
char *ftostr3(const float& x) { return itostr3((int)x); }

// Convert float to rj string with _123, -123, _-12, or __-1 format
char *ftostr4sign(const float& x) { return itostr4sign((int)x); }

// Convert unsigned int to string with 12 format
char* itostr2(const uint8_t& x) {
  //sprintf(conv,"%5.1f",x);
  int xx = x;
  conv[0] = DIGIMOD(xx / 10);
  conv[1] = DIGIMOD(xx);
  conv[2] = '\0';
  return conv;
}

// Convert float to string with +123.4 / -123.4 format
char* ftostr41sign(const float& x) {
  int xx = int(abs(x * 10)) % 10000;
  conv[0] = x >= 0 ? '+' : '-';
  conv[1] = DIGIMOD(xx / 1000);
  conv[2] = DIGIMOD(xx / 100);
  conv[3] = DIGIMOD(xx / 10);
  conv[4] = '.';
  conv[5] = DIGIMOD(xx);
  conv[6] = '\0';
  return conv;
}

// Convert signed float to string with 023.45 / -23.45 format
char *ftostr32(const float& x) {
  long xx = abs(x * 100);
  conv[0] = x >= 0 ? DIGIMOD(xx / 10000) : '-';
  conv[1] = DIGIMOD(xx / 1000);
  conv[2] = DIGIMOD(xx / 100);
  conv[3] = '.';
  conv[4] = DIGIMOD(xx / 10);
  conv[5] = DIGIMOD(xx);
  conv[6] = '\0';
  return conv;
}

// Convert signed float to string (6 digit) with -1.234 / _0.000 / +1.234 format
char* ftostr43sign(const float& x, char plus/*=' '*/) {
  long xx = x * 1000;
  if (xx == 0)
    conv[0] = ' ';
  else if (xx > 0)
    conv[0] = plus;
  else {
    xx = -xx;
    conv[0] = '-';
  }
  conv[1] = DIGIMOD(xx / 1000);
  conv[2] = '.';
  conv[3] = DIGIMOD(xx / 100);
  conv[4] = DIGIMOD(xx / 10);
  conv[5] = DIGIMOD(xx);
  conv[6] = '\0';
  return conv;
}

// Convert unsigned float to string with 1.23 format
char* ftostr12ns(const float& x) {
  long xx = x * 100;
  xx = abs(xx);
  conv[0] = DIGIMOD(xx / 100);
  conv[1] = '.';
  conv[2] = DIGIMOD(xx / 10);
  conv[3] = DIGIMOD(xx);
  conv[4] = '\0';
  return conv;
}

// Convert signed int to lj string with +012 / -012 format
char* itostr3sign(const int& x) {
  int xx;
  if (x >= 0) {
    conv[0] = '+';
    xx = x;
  }
  else {
    conv[0] = '-';
    xx = -x;
  }
  conv[1] = DIGIMOD(xx / 100);
  conv[2] = DIGIMOD(xx / 10);
  conv[3] = DIGIMOD(xx);
  conv[4] = '.';
  conv[5] = '0';
  conv[6] = '\0';
  return conv;
}

// Convert signed int to rj string with 123 or -12 format
char* itostr3(const int& x) {
  int xx = x;
  if (xx < 0) {
    conv[0] = '-';
    xx = -xx;
  }
  else
    conv[0] = xx >= 100 ? DIGIMOD(xx / 100) : ' ';

  conv[1] = xx >= 10 ? DIGIMOD(xx / 10) : ' ';
  conv[2] = DIGIMOD(xx);
  conv[3] = '\0';
  return conv;
}

// Convert unsigned int to lj string with 123 format
char* itostr3left(const int& xx) {
  if (xx >= 100) {
    conv[0] = DIGIMOD(xx / 100);
    conv[1] = DIGIMOD(xx / 10);
    conv[2] = DIGIMOD(xx);
    conv[3] = '\0';
  }
  else if (xx >= 10) {
    conv[0] = DIGIMOD(xx / 10);
    conv[1] = DIGIMOD(xx);
    conv[2] = '\0';
  }
  else {
    conv[0] = DIGIMOD(xx);
    conv[1] = '\0';
  }
  return conv;
}

// Convert signed int to rj string with _123, -123, _-12, or __-1 format
char *itostr4sign(const int& x) {
  int xx = abs(x);
  int sign = 0;
  if (xx >= 100) {
    conv[1] = DIGIMOD(xx / 100);
    conv[2] = DIGIMOD(xx / 10);
  }
  else if (xx >= 10) {
    conv[0] = ' ';
    sign = 1;
    conv[2] = DIGIMOD(xx / 10);
  }
  else {
    conv[0] = ' ';
    conv[1] = ' ';
    sign = 2;
  }
  conv[sign] = x < 0 ? '-' : ' ';
  conv[3] = DIGIMOD(xx);
  conv[4] = '\0';
  return conv;
}

// Convert unsigned float to rj string with 12345 format
char* ftostr5rj(const float& x) {
  long xx = abs(x);
  conv[0] = xx >= 10000 ? DIGIMOD(xx / 10000) : ' ';
  conv[1] = xx >= 1000 ? DIGIMOD(xx / 1000) : ' ';
  conv[2] = xx >= 100 ? DIGIMOD(xx / 100) : ' ';
  conv[3] = xx >= 10 ? DIGIMOD(xx / 10) : ' ';
  conv[4] = DIGIMOD(xx);
  conv[5] = '\0';
  return conv;
}

// Convert signed float to string with +1234.5 format
char* ftostr51sign(const float& x) {
  long xx = abs(x * 10);
  conv[0] = (x >= 0) ? '+' : '-';
  conv[1] = DIGIMOD(xx / 10000);
  conv[2] = DIGIMOD(xx / 1000);
  conv[3] = DIGIMOD(xx / 100);
  conv[4] = DIGIMOD(xx / 10);
  conv[5] = '.';
  conv[6] = DIGIMOD(xx);
  conv[7] = '\0';
  return conv;
}

// Convert signed float to string with +123.45 format
char* ftostr52sign(const float& x) {
  long xx = abs(x * 100);
  conv[0] = (x >= 0) ? '+' : '-';
  conv[1] = DIGIMOD(xx / 10000);
  conv[2] = DIGIMOD(xx / 1000);
  conv[3] = DIGIMOD(xx / 100);
  conv[4] = '.';
  conv[5] = DIGIMOD(xx / 10);
  conv[6] = DIGIMOD(xx);
  conv[7] = '\0';
  return conv;
}

// Convert signed float to space-padded string with -_23.4_ format
char* ftostr52sp(const float& x) {
  long xx = x * 100;
  uint8_t dig;
  if (xx < 0) { // negative val = -_0
    xx = -xx;
    conv[0] = '-';
    dig = (xx / 1000) % 10;
    conv[1] = dig ? DIGIT(dig) : ' ';
  }
  else { // positive val = __0
    dig = (xx / 10000) % 10;
    if (dig) {
      conv[0] = DIGIT(dig);
      conv[1] = DIGIMOD(xx / 1000);
    }
    else {
      conv[0] = ' ';
      dig = (xx / 1000) % 10;
      conv[1] = dig ? DIGIT(dig) : ' ';
    }
  }

  conv[2] = DIGIMOD(xx / 100); // lsd always

  dig = xx % 10;
  if (dig) { // 2 decimal places
    conv[5] = DIGIT(dig);
    conv[4] = DIGIMOD(xx / 10);
    conv[3] = '.';
  }
  else { // 1 or 0 decimal place
    dig = (xx / 10) % 10;
    if (dig) {
      conv[4] = DIGIT(dig);
      conv[3] = '.';
    }
    else {
      conv[3] = conv[4] = ' ';
    }
    conv[5] = ' ';
  }
  conv[6] = '\0';
  return conv;
}

char* ltostr7(const long& x) {
  if (x >= 1000000)
    conv[0]=(x/1000000)%10+'0';
  else
    conv[0]=' ';
  if (x >= 100000)
    conv[1]=(x/100000)%10+'0';
  else
    conv[1]=' ';
  if (x >= 10000)
    conv[2]=(x/10000)%10+'0';
  else
    conv[2]=' ';
  if (x >= 1000)
    conv[3]=(x/1000)%10+'0';
  else
    conv[3]=' ';
  if (x >= 100)
    conv[4]=(x/100)%10+'0';
  else
    conv[4]=' ';
  if (x >= 10)
    conv[5]=(x/10)%10+'0';
  else
    conv[5]=' ';
  conv[6]=(x)%10+'0';
  conv[7]=0;
  return conv;
}

#endif // ULTRA_LCD

#if ENABLED(SDSUPPORT) && ENABLED(SD_SETTINGS)
  void set_sd_dot() {
    #if ENABLED(DOGLCD)
      u8g.firstPage();
      do {
        u8g.setColorIndex(1);
        u8g.drawPixel(0, 0); // draw sd dot
        u8g.setColorIndex(1); // black on white
        (*currentScreen)();
      } while( u8g.nextPage() );
    #endif
  }
  void unset_sd_dot() {
    #if ENABLED(DOGLCD)
      u8g.firstPage();
      do {
        u8g.setColorIndex(0);
        u8g.drawPixel(0, 0); // draw sd dot
        u8g.setColorIndex(1); // black on white
        (*currentScreen)();
      } while( u8g.nextPage() );
    #endif
  }
#endif
