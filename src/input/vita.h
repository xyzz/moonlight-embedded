/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2016 Ilya Zhuravlev, Sunguk Lee, Vasyl Horbachenko
 *
 * Moonlight is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Moonlight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Moonlight; if not, see <http://www.gnu.org/licenses/>.
 */

enum {
  NO_TOUCH_ACTION = 0,
  ON_SCREEN_TOUCH,
  SCREEN_TAP,
  SWIPE_START,
  ON_SCREEN_SWIPE
} TouchScreenState;

enum {
  TOUCHSEC_NORTHWEST    = 0x0001,
  TOUCHSEC_NORTHEAST    = 0x0002,
  TOUCHSEC_SOUTHWEST    = 0x0004,
  TOUCHSEC_SOUTHEAST    = 0x0008,
  TOUCHSEC_SPECIAL_NW   = 0x0010,
  TOUCHSEC_SPECIAL_NE   = 0x0020,
  TOUCHSEC_SPECIAL_SW   = 0x0040,
  TOUCHSEC_SPECIAL_SE   = 0x0080,

  ANALOG_LEFTX          = 0x0100,
  ANALOG_LEFTY          = 0x0200,
  ANALOG_RIGHTX         = 0x0400,
  ANALOG_RIGHTY         = 0x0800,
  ANALOG_LEFT_TRIGGER   = 0x1000,
  ANALOG_RIGHT_TRIGGER  = 0x2000,
} InputSectionValue;

#define INPUT_TYPE_MASK         0xfff00000
#define INPUT_VALUE_MASK        0x000fffff

#define INPUT_TYPE_KEYBOARD     0x00000000
#define INPUT_TYPE_SPECIAL      0x00100000
#define INPUT_TYPE_MOUSE        0x00200000
#define INPUT_TYPE_GAMEPAD      0x00400000
#define INPUT_TYPE_ANALOG       0x00800000
#define INPUT_TYPE_TOUCHSCREEN  0x01000000

enum {
  INPUT_SPECIAL_KEY_PAUSE
};

bool vitainput_init();
void vitainput_config(CONFIGURATION config);

void vitainput_start(void);
void vitainput_stop(void);
