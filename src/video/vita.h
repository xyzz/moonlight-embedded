/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2016 Ilya Zhuravlev
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
#include <math.h>

// Vita's sceVideodecInitLibrary only accept resolution that is multiple of 16 on either dimension,
// and the smallest resolution is 64
// Full supported resolution list can be found at:
// https://github.com/MakiseKurisu/vita-sceVideodecInitLibrary-test/
#define ROUND_NEAREST_16(x)                     (round(((double) (x)) / 16) * 16)
#define VITA_DECODER_RESOLUTION_LOWER_BOUND(x)  ((x) < 64 ? 64 : (x))
#define VITA_DECODER_RESOLUTION(x)              (VITA_DECODER_RESOLUTION_LOWER_BOUND(ROUND_NEAREST_16(x)))

void vitavideo_start();
void vitavideo_stop();
void vitavideo_show_poor_net_indicator();
void vitavideo_hide_poor_net_indicator();
int vitavideo_initialized();
