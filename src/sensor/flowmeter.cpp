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
 * flowmeter.h - Flowmeter control library for Arduino - Version 1
 * Copyright (c) 2016 Franco (nextime) Lanza.  All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "../../base.h"
#include <Arduino.h>

#if ENABLED(FLOWMETER_SENSOR)

  volatile int flowrate_pulsecount;
  float flowrate;
  static millis_t flowmeter_timer = 0;
  static millis_t lastflow = 0;
  void flowrate_pulsecounter();

  void flow_init() {
    flowrate = 0;
    flowrate_pulsecount = 0;
    pinMode(FLOWMETER_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(FLOWMETER_PIN), flowrate_pulsecounter, FALLING);
  }

  void flowrate_manage() {
    millis_t  now;
    now = millis();
    if(ELAPSED(now, flowmeter_timer)) {
      detachInterrupt(digitalPinToInterrupt(FLOWMETER_PIN));
      flowrate  = (float)(((1000.0 / (float)((float)now - (float)lastflow)) * (float)flowrate_pulsecount) / (float)FLOWMETER_CALIBRATION);
      #if ENABLED(FLOWMETER_DEBUG)
        SERIAL_SM(DEB, "FLOWMETER DEBUG ");
        SERIAL_MV(" flowrate:", flowrate);
        SERIAL_MV(" flowrate_pulsecount:", flowrate_pulsecount);
        SERIAL_EMV(" CALIBRATION:", FLOWMETER_CALIBRATION);
      #endif
      flowmeter_timer = now + 1000UL;
      lastflow = now;
      flowrate_pulsecount = 0;
      attachInterrupt(digitalPinToInterrupt(FLOWMETER_PIN), flowrate_pulsecounter, FALLING);
    }
  }

  float get_flowrate() {
    return flowrate;
  }

  void flowrate_pulsecounter() {
    // Increment the pulse counter
    flowrate_pulsecount++;
  }

#endif // FLOWMETER_SENSOR
