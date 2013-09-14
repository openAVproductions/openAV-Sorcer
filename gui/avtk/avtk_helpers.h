/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */


#ifndef AVTK_HELPERS_H
#define AVTK_HELPERS_H

#include <iostream>

using namespace std;

inline void avtk_stroke_line(cairo_t* cr, bool active)
{
  cairo_save(cr);
  // stroke cutoff lines, or border backgrounds
  cairo_set_line_width(cr, 1.5);
  cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 0.8 );
  cairo_restore(cr);
}

#endif // AVTK_HELPERS_H

