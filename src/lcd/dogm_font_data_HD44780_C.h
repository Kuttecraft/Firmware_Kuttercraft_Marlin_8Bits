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
  Fontname: HD44780_C v1.2
  Copyright: A. Hardtung, public domain
  Capital A Height: 7, '1' Height: 7
  Calculated Max Values w= 5 h= 8 x= 2 y= 7 dx= 6 dy= 0 ascent= 8 len= 8
  Font Bounding box     w= 6 h= 9 x= 0 y=-2
  Calculated Min Values           x= 0 y=-1 dx= 0 dy= 0
  Pure Font   ascent = 7 descent=-1
  X Font      ascent = 7 descent=-1
  Max Font    ascent = 8 descent=-1
*/
#include <U8glib.h>
const u8g_fntpgm_uint8_t HD44780_C_5x7[2522] U8G_SECTION(".progmem.HD44780_C_5x7") = {
  0, 6, 9, 0, 254, 7, 1, 145, 3, 34, 32, 255, 255, 8, 255, 7,
  255, 0, 0, 0, 6, 0, 0, 1, 7, 7, 6, 2, 0, 128, 128, 128,
  128, 128, 0, 128, 3, 2, 2, 6, 1, 5, 160, 160, 5, 7, 7, 6,
  0, 0, 80, 80, 248, 80, 248, 80, 80, 5, 7, 7, 6, 0, 0, 32,
  120, 160, 112, 40, 240, 32, 5, 7, 7, 6, 0, 0, 192, 200, 16, 32,
  64, 152, 24, 5, 7, 7, 6, 0, 0, 96, 144, 160, 64, 168, 144, 104,
  2, 3, 3, 6, 1, 4, 192, 64, 128, 3, 7, 7, 6, 1, 0, 32,
  64, 128, 128, 128, 64, 32, 3, 7, 7, 6, 1, 0, 128, 64, 32, 32,
  32, 64, 128, 5, 5, 5, 6, 0, 1, 32, 168, 112, 168, 32, 5, 5,
  5, 6, 0, 1, 32, 32, 248, 32, 32, 2, 3, 3, 6, 2, 255, 192,
  64, 128, 5, 1, 1, 6, 0, 3, 248, 2, 2, 2, 6, 2, 0, 192,
  192, 5, 5, 5, 6, 0, 1, 8, 16, 32, 64, 128, 5, 7, 7, 6,
  0, 0, 112, 136, 152, 168, 200, 136, 112, 3, 7, 7, 6, 1, 0, 64,
  192, 64, 64, 64, 64, 224, 5, 7, 7, 6, 0, 0, 112, 136, 8, 112,
  128, 128, 248, 5, 7, 7, 6, 0, 0, 248, 16, 32, 16, 8, 8, 240,
  5, 7, 7, 6, 0, 0, 16, 48, 80, 144, 248, 16, 16, 5, 7, 7,
  6, 0, 0, 248, 128, 240, 8, 8, 136, 112, 5, 7, 7, 6, 0, 0,
  48, 64, 128, 240, 136, 136, 112, 5, 7, 7, 6, 0, 0, 248, 8, 16,
  32, 32, 32, 32, 5, 7, 7, 6, 0, 0, 112, 136, 136, 112, 136, 136,
  112, 5, 7, 7, 6, 0, 0, 112, 136, 136, 120, 8, 16, 96, 2, 5,
  5, 6, 2, 0, 192, 192, 0, 192, 192, 2, 6, 6, 6, 2, 255, 192,
  192, 0, 192, 64, 128, 4, 7, 7, 6, 0, 0, 16, 32, 64, 128, 64,
  32, 16, 5, 3, 3, 6, 0, 2, 248, 0, 248, 4, 7, 7, 6, 1,
  0, 128, 64, 32, 16, 32, 64, 128, 5, 7, 7, 6, 0, 0, 112, 136,
  8, 16, 32, 0, 32, 5, 6, 6, 6, 0, 0, 112, 136, 8, 104, 168,
  112, 5, 7, 7, 6, 0, 0, 112, 136, 136, 248, 136, 136, 136, 5, 7,
  7, 6, 0, 0, 240, 136, 136, 240, 136, 136, 240, 5, 7, 7, 6, 0,
  0, 112, 136, 128, 128, 128, 136, 112, 5, 7, 7, 6, 0, 0, 224, 144,
  136, 136, 136, 144, 224, 5, 7, 7, 6, 0, 0, 248, 128, 128, 240, 128,
  128, 248, 5, 7, 7, 6, 0, 0, 248, 128, 128, 240, 128, 128, 128, 5,
  7, 7, 6, 0, 0, 112, 136, 128, 184, 136, 136, 112, 5, 7, 7, 6,
  0, 0, 136, 136, 136, 248, 136, 136, 136, 1, 7, 7, 6, 2, 0, 128,
  128, 128, 128, 128, 128, 128, 5, 7, 7, 6, 0, 0, 56, 16, 16, 16,
  16, 144, 96, 5, 7, 7, 6, 0, 0, 136, 144, 160, 192, 160, 144, 136,
  5, 7, 7, 6, 0, 0, 128, 128, 128, 128, 128, 128, 248, 5, 7, 7,
  6, 0, 0, 136, 216, 168, 136, 136, 136, 136, 5, 7, 7, 6, 0, 0,
  136, 136, 200, 168, 152, 136, 136, 5, 7, 7, 6, 0, 0, 112, 136, 136,
  136, 136, 136, 112, 5, 7, 7, 6, 0, 0, 240, 136, 136, 240, 128, 128,
  128, 5, 7, 7, 6, 0, 0, 112, 136, 136, 136, 168, 144, 104, 5, 7,
  7, 6, 0, 0, 240, 136, 136, 240, 160, 144, 136, 5, 7, 7, 6, 0,
  0, 120, 128, 128, 112, 8, 8, 240, 5, 7, 7, 6, 0, 0, 248, 32,
  32, 32, 32, 32, 32, 5, 7, 7, 6, 0, 0, 136, 136, 136, 136, 136,
  136, 112, 5, 7, 7, 6, 0, 0, 136, 136, 136, 136, 136, 80, 32, 5,
  7, 7, 6, 0, 0, 136, 136, 136, 136, 136, 168, 80, 5, 7, 7, 6,
  0, 0, 136, 136, 80, 32, 80, 136, 136, 5, 7, 7, 6, 0, 0, 136,
  136, 136, 80, 32, 32, 32, 5, 7, 7, 6, 0, 0, 248, 8, 16, 32,
  64, 128, 248, 3, 7, 7, 6, 1, 0, 224, 128, 128, 128, 128, 128, 224,
  5, 7, 7, 6, 0, 0, 32, 112, 160, 160, 168, 112, 32, 3, 7, 7,
  6, 1, 0, 224, 32, 32, 32, 32, 32, 224, 5, 3, 3, 6, 0, 4,
  32, 80, 136, 5, 1, 1, 6, 0, 0, 248, 2, 2, 2, 6, 2, 5,
  128, 64, 5, 5, 5, 6, 0, 0, 112, 8, 120, 136, 120, 5, 7, 7,
  6, 0, 0, 128, 128, 176, 200, 136, 136, 240, 5, 5, 5, 6, 0, 0,
  112, 128, 128, 136, 112, 5, 7, 7, 6, 0, 0, 8, 8, 104, 152, 136,
  136, 120, 5, 5, 5, 6, 0, 0, 112, 136, 248, 128, 112, 5, 7, 7,
  6, 0, 0, 48, 72, 224, 64, 64, 64, 64, 5, 6, 6, 6, 0, 255,
  112, 136, 136, 120, 8, 112, 5, 7, 7, 6, 0, 0, 128, 128, 176, 200,
  136, 136, 136, 1, 7, 7, 6, 2, 0, 128, 0, 128, 128, 128, 128, 128,
  3, 8, 8, 6, 1, 255, 32, 0, 32, 32, 32, 32, 160, 64, 4, 7,
  7, 6, 0, 0, 128, 128, 144, 160, 192, 160, 144, 3, 7, 7, 6, 1,
  0, 192, 64, 64, 64, 64, 64, 224, 5, 5, 5, 6, 0, 0, 208, 168,
  168, 168, 168, 5, 5, 5, 6, 0, 0, 176, 200, 136, 136, 136, 5, 5,
  5, 6, 0, 0, 112, 136, 136, 136, 112, 5, 6, 6, 6, 0, 255, 240,
  136, 136, 240, 128, 128, 5, 6, 6, 6, 0, 255, 120, 136, 136, 120, 8,
  8, 5, 5, 5, 6, 0, 0, 176, 200, 128, 128, 128, 5, 5, 5, 6,
  0, 0, 112, 128, 112, 8, 240, 5, 7, 7, 6, 0, 0, 64, 64, 224,
  64, 64, 72, 48, 5, 5, 5, 6, 0, 0, 136, 136, 136, 152, 104, 5,
  5, 5, 6, 0, 0, 136, 136, 136, 80, 32, 5, 5, 5, 6, 0, 0,
  136, 136, 168, 168, 80, 5, 5, 5, 6, 0, 0, 136, 80, 32, 80, 136,
  5, 6, 6, 6, 0, 255, 136, 136, 136, 120, 8, 112, 5, 5, 5, 6,
  0, 0, 248, 16, 32, 64, 248, 5, 5, 5, 6, 0, 2, 184, 168, 168,
  168, 184, 5, 5, 5, 6, 0, 2, 184, 136, 184, 160, 184, 5, 5, 5,
  6, 0, 2, 184, 160, 184, 136, 184, 5, 6, 6, 6, 0, 1, 8, 40,
  72, 248, 64, 32, 5, 5, 5, 6, 0, 0, 56, 112, 224, 136, 240, 0,
  0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0,
  6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0,
  0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0,
  6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0,
  0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0,
  6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0,
  0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0,
  0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0,
  6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 6, 0, 0, 5,
  7, 7, 6, 0, 0, 248, 136, 128, 240, 136, 136, 240, 5, 7, 7, 6,
  0, 0, 248, 136, 128, 128, 128, 128, 128, 5, 7, 7, 6, 0, 0, 80,
  0, 248, 128, 240, 128, 248, 5, 7, 7, 6, 0, 0, 168, 168, 168, 112,
  168, 168, 168, 5, 7, 7, 6, 0, 0, 240, 8, 8, 112, 8, 8, 240,
  5, 7, 7, 6, 0, 0, 136, 136, 152, 168, 200, 136, 136, 5, 8, 8,
  6, 0, 0, 80, 32, 136, 152, 168, 168, 200, 136, 5, 7, 7, 6, 0,
  0, 120, 40, 40, 40, 40, 168, 72, 5, 7, 7, 6, 0, 0, 248, 136,
  136, 136, 136, 136, 136, 5, 7, 7, 6, 0, 0, 136, 136, 136, 80, 32,
  64, 128, 5, 7, 7, 6, 0, 0, 32, 112, 168, 168, 168, 112, 32, 5,
  7, 7, 6, 0, 0, 136, 136, 136, 120, 8, 8, 8, 5, 7, 7, 6,
  0, 0, 168, 168, 168, 168, 168, 168, 248, 5, 7, 7, 6, 0, 0, 192,
  64, 64, 112, 72, 72, 112, 5, 7, 7, 6, 0, 0, 136, 136, 136, 200,
  168, 168, 200, 5, 7, 7, 6, 0, 0, 112, 136, 8, 56, 8, 136, 112,
  5, 7, 7, 6, 0, 0, 144, 168, 168, 232, 168, 168, 144, 5, 7, 7,
  6, 0, 0, 120, 136, 136, 120, 40, 72, 136, 5, 7, 7, 6, 0, 0,
  24, 96, 128, 240, 136, 136, 112, 4, 5, 5, 6, 0, 0, 224, 144, 224,
  144, 224, 5, 5, 5, 6, 0, 0, 248, 136, 128, 128, 128, 5, 7, 7,
  6, 0, 0, 80, 0, 112, 136, 248, 128, 112, 5, 5, 5, 6, 0, 0,
  168, 168, 112, 168, 168, 5, 5, 5, 6, 0, 0, 240, 8, 48, 8, 240,
  5, 5, 5, 6, 0, 0, 136, 152, 168, 200, 136, 5, 7, 7, 6, 0,
  0, 80, 32, 136, 152, 168, 200, 136, 4, 5, 5, 6, 0, 0, 144, 160,
  192, 160, 144, 5, 5, 5, 6, 0, 0, 248, 40, 40, 168, 72, 5, 5,
  5, 6, 0, 0, 136, 216, 168, 136, 136, 5, 5, 5, 6, 0, 0, 136,
  136, 248, 136, 136, 5, 5, 5, 6, 0, 0, 248, 136, 136, 136, 136, 5,
  5, 5, 6, 0, 0, 248, 32, 32, 32, 32, 5, 5, 5, 6, 0, 0,
  136, 136, 120, 8, 8, 5, 5, 5, 6, 0, 0, 168, 168, 168, 168, 248,
  5, 5, 5, 6, 0, 0, 192, 64, 112, 72, 112, 5, 5, 5, 6, 0,
  0, 136, 136, 200, 168, 200, 4, 5, 5, 6, 0, 0, 128, 128, 224, 144,
  224, 5, 5, 5, 6, 0, 0, 112, 136, 56, 136, 112, 5, 5, 5, 6,
  0, 0, 144, 168, 232, 168, 144, 5, 5, 5, 6, 0, 0, 120, 136, 120,
  40, 72, 5, 5, 5, 6, 0, 1, 32, 72, 144, 72, 32, 5, 5, 5,
  6, 0, 1, 32, 144, 72, 144, 32, 5, 3, 3, 6, 0, 0, 72, 144,
  216, 5, 3, 3, 6, 0, 4, 216, 72, 144, 5, 7, 7, 6, 0, 0,
  144, 208, 176, 144, 56, 40, 56, 5, 7, 7, 6, 0, 0, 32, 0, 32,
  64, 128, 136, 112, 5, 7, 7, 6, 0, 0, 24, 32, 32, 112, 32, 32,
  192, 5, 7, 7, 6, 0, 0, 32, 80, 64, 240, 64, 64, 120, 1, 2,
  2, 6, 2, 0, 128, 128, 1, 4, 4, 6, 2, 0, 128, 128, 128, 128,
  3, 5, 5, 6, 1, 0, 160, 160, 160, 0, 224, 3, 5, 5, 6, 1,
  0, 160, 160, 160, 0, 160, 5, 7, 7, 6, 0, 0, 160, 0, 232, 16,
  32, 64, 128, 5, 5, 5, 6, 0, 1, 216, 112, 32, 112, 216, 5, 7,
  7, 6, 0, 0, 160, 64, 168, 16, 32, 64, 128, 3, 6, 6, 6, 1,
  1, 224, 64, 64, 64, 64, 224, 5, 6, 6, 6, 0, 1, 248, 80, 80,
  80, 80, 248, 5, 7, 7, 6, 0, 0, 32, 112, 168, 32, 32, 32, 32,
  5, 7, 7, 6, 0, 0, 32, 32, 32, 32, 168, 112, 32, 5, 7, 7,
  6, 0, 0, 128, 144, 176, 248, 176, 144, 128, 5, 7, 7, 6, 0, 0,
  8, 72, 104, 248, 104, 72, 8, 5, 7, 7, 6, 0, 0, 128, 136, 168,
  248, 168, 136, 128, 5, 7, 7, 6, 0, 0, 128, 224, 136, 16, 32, 64,
  128, 2, 2, 2, 6, 2, 2, 192, 192, 5, 8, 8, 6, 0, 255, 120,
  40, 40, 40, 72, 136, 248, 136, 5, 8, 8, 6, 0, 255, 136, 136, 136,
  136, 136, 136, 248, 8, 5, 8, 8, 6, 0, 255, 168, 168, 168, 168, 168,
  168, 248, 8, 5, 6, 6, 6, 0, 255, 120, 40, 72, 136, 248, 136, 5,
  7, 7, 6, 0, 255, 32, 32, 112, 168, 168, 112, 32, 5, 6, 6, 6,
  0, 255, 136, 136, 136, 136, 248, 8, 5, 6, 6, 6, 0, 255, 168, 168,
  168, 168, 248, 8, 2, 2, 2, 6, 2, 6, 64, 128, 3, 1, 1, 6,
  1, 7, 160, 5, 2, 2, 6, 0, 6, 72, 176, 5, 8, 8, 6, 0,
  0, 16, 32, 0, 112, 136, 248, 128, 112, 5, 6, 6, 6, 0, 255, 112,
  128, 136, 112, 32, 96, 3, 7, 7, 6, 1, 0, 160, 0, 160, 160, 160,
  32, 192, 5, 6, 6, 6, 0, 1, 32, 112, 112, 112, 248, 32, 5, 5,
  5, 6, 0, 1, 80, 0, 136, 0, 80, 5, 5, 5, 6, 0, 1, 112,
  136, 136, 136, 112, 5, 7, 7, 6, 0, 0, 136, 144, 168, 88, 184, 8,
  8, 5, 7, 7, 6, 0, 0, 136, 144, 184, 72, 184, 8, 56, 5, 7,
  7, 6, 0, 0, 136, 144, 184, 72, 152, 32, 56, 5, 8, 8, 6, 0,
  0, 192, 64, 192, 72, 216, 56, 8, 8, 5, 7, 7, 6, 0, 0, 136,
  248, 136, 248, 136, 248, 136, 4, 5, 5, 6, 0, 2, 192, 0, 48, 0,
  96, 5, 8, 8, 6, 0, 0, 64, 160, 224, 168, 8, 40, 120, 32, 5,
  8, 8, 6, 0, 0, 64, 112, 64, 120, 64, 112, 64, 224, 5, 8, 8,
  6, 0, 0, 32, 112, 32, 248, 32, 112, 32, 112, 5, 7, 7, 6, 0,
  0, 104, 0, 232, 0, 104, 16, 56, 5, 8, 8, 6, 0, 0, 16, 112,
  16, 240, 16, 112, 16, 56, 5, 7, 7, 6, 0, 1, 32, 112, 32, 248,
  32, 112, 32, 5, 8, 8, 6, 0, 0, 16, 144, 80, 48, 80, 144, 16,
  56, 5, 8, 8, 6, 0, 0, 48, 72, 32, 80, 80, 32, 144, 96, 5,
  7, 7, 6, 0, 0, 120, 168, 168, 120, 40, 40, 40, 5, 8, 8, 6,
  0, 0, 248, 248, 248, 248, 248, 248, 248, 248
};
