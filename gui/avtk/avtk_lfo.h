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


#ifndef AVTK_LFO_H
#define AVTK_LFO_H

#include <FL/Fl_Slider.H>

namespace Avtk {

class LFO : public Fl_Slider
{
  public:
    LFO(int _x, int _y, int _w, int _h, const char *_label =0):
        Fl_Slider(_x, _y, _w, _h, _label)
    {
      x = _x;
      y = _y;
      w = _w;
      h = _h;
      
      active = true;
      
      label = _label;
      
      highlight = false;
      mouseOver = false;
    }
    
    bool active;
    bool mouseOver;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    void modulation(float m)
    {
      mod = m;
      redraw();
    }
    
    float mod;
    
    void draw()
    {
      if (damage() & FL_DAMAGE_ALL)
      {
        cairo_t *cr = Fl::cairo_cc();
        
        cairo_save( cr );
        

        // WAVEFORM graph
        cairo_rectangle( cr, x, y, w, h );
        cairo_set_source_rgb( cr,28 / 255.f,  28 / 255.f ,  28 / 255.f  );
        cairo_fill(cr);
        
        
        // set up dashed lines, 1 px off, 1 px on
        double dashes[1];
        dashes[0] = 2.0;
        
        cairo_set_dash ( cr, dashes, 1, 0.0);
        cairo_set_line_width( cr, 1.0);
        
        // loop over each 2nd line, drawing dots
        cairo_set_line_width(cr, 1.0);
        cairo_set_source_rgb(cr, 0.4,0.4,0.4);
        for ( int i = 0; i < 4; i++ )
        {
          cairo_move_to( cr, x + ((w / 4.f)*i), y );
          cairo_line_to( cr, x + ((w / 4.f)*i), y + h );
        }
        for ( int i = 0; i < 4; i++ )
        {
          cairo_move_to( cr, x    , y + ((h / 4.f)*i) );
          cairo_line_to( cr, x + w, y + ((h / 4.f)*i) );
        }
        
        cairo_set_source_rgba( cr,  66 / 255.f,  66 / 255.f ,  66 / 255.f , 0.5 );
        cairo_stroke(cr);
        cairo_set_dash ( cr, dashes, 0, 0.0);
        
        
      
      // LFO graph plotting
        float wavetableMod = mod;
        float lfoAmp = value();
        float volume = value();
        
        
        // sinewave (in graph 1)
        int x1 = x;
        int xS = w;
        int y1 = y;
        int yS = h;
        cairo_move_to( cr, x1, y1 + yS / 2 );
        
        int m1x = x1 + xS / 6;
        int m1y = (y1 + yS / 2) - 48 * lfoAmp;
        
        int m2x = x1 + xS / 3;
        int m2y = m1y;
        
        int endX = x1 + xS / 2;
        int endY = y1 + yS / 2;
        cairo_curve_to( cr, m1x, m1y, m2x, m2y, endX, endY);
        
        int m3x = x1 + 2 * xS / 3;
        int m3y = y1 + yS / 2.f + ((yS + 7) * 0.5 * lfoAmp);
        
        int m4x = x1 + 5 * xS / 6;
        int m4y = m3y;
        
        int end2X = x1 + xS;
        int end2Y = y1 + yS / 2;
        cairo_curve_to( cr, m3x, m3y, m4x, m4y, end2X, end2Y);
        cairo_close_path(cr);
        
        if ( true )
          cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 0.2 );
        else
          cairo_set_source_rgba( cr,  66 / 255.f,  66 / 255.f ,  66 / 255.f , 0.5 );
        cairo_fill(cr);
        
        cairo_move_to(cr, x  , y+(h/2));
        cairo_line_to(cr, x+w, y+(h/2));
        cairo_set_source_rgba( cr,  66 / 255.f,  66 / 255.f ,  66 / 255.f , 0.7 );
        cairo_set_line_width(cr, 1.5);
        cairo_stroke(cr);
        
        // redraw curve in blue
        cairo_move_to( cr, x1, y1 + yS / 2 );
        cairo_curve_to( cr, m1x, m1y, m2x, m2y, endX, endY);
        cairo_curve_to( cr, m3x, m3y, m4x, m4y, end2X, end2Y);
        cairo_set_source_rgba( cr, 0 / 255.f, 153 / 255.f , 255 / 255.f , 1 );
        cairo_stroke( cr );
        
        // Waveform data: WavetableMod
        {
          int drawX = x;
          int drawY = y + h - 2;
          
          cairo_set_line_width(cr, 3.3);
          cairo_rectangle( cr, x, y + h - 2, w * wavetableMod, 1);
          cairo_set_source_rgba( cr, 25 / 255.f, 255 / 255.f ,   0 / 255.f , 0.7 );
          cairo_stroke( cr );
        }
        // Waveform data: Volume
        {
          int drawX = x+w-3;
          int drawY = y;
          
          float volume = 0.7;
          cairo_set_line_width(cr, 2.4);
          cairo_rectangle( cr, drawX, drawY+ h*(1-lfoAmp), 1,  (h*lfoAmp) ); 
          cairo_set_source_rgba( cr, 255 / 255.f, 104 / 255.f ,   0 / 255.f , 1 );
          cairo_stroke( cr );
        }
        
        // stroke outline
        cairo_rectangle(cr, x, y, w, h);
        cairo_set_source_rgba( cr,  126 / 255.f,  126 / 255.f ,  126 / 255.f , 0.8 );
        cairo_set_line_width(cr, 1.9);
        cairo_stroke( cr );
        
        
        if ( !active )
        {
          // big grey X
          cairo_set_line_width(cr, 20.0);
          cairo_set_source_rgba(cr, 0.4,0.4,0.4, 0.7);
          
          cairo_move_to( cr, x + (3 * w / 4.f), y + ( h / 4.f ) );
          cairo_line_to( cr, x + (w / 4.f), y + ( 3 *h / 4.f ) );
          
          cairo_move_to( cr, x + (w / 4.f), y + ( h / 4.f ) );
          cairo_line_to( cr, x + (3 * w / 4.f), y + ( 3 *h / 4.f ) );
          cairo_set_line_cap ( cr, CAIRO_LINE_CAP_BUTT);
          cairo_stroke( cr );
        }
        
        cairo_restore( cr );
        
        draw_label();
      }
    }
    
    void resize(int X, int Y, int W, int H)
    {
      Fl_Widget::resize(X,Y,W,H);
      x = X;
      y = Y;
      w = W;
      h = H;
      redraw();
    }
    
    int handle(int event)
    {
      switch(event) {
        case FL_PUSH:
          highlight = 1;
          if ( Fl::event_button() == FL_RIGHT_MOUSE )
          {
            active = !active;
            redraw();
            do_callback();
          }
          return 1;
        case FL_DRAG: {
            int t = Fl::event_inside(this);
            if (t != highlight) {
              highlight = t;
              redraw();
            }
          }
          return 1;
        case FL_ENTER:
          mouseOver = true;
          redraw();
          return 1;
        case FL_LEAVE:
          mouseOver = false;
          redraw();
          return 1;
        case FL_RELEASE:
          if (highlight) {
            highlight = 0;
            redraw();
            do_callback();
          }
          return 1;
        case FL_SHORTCUT:
          if ( test_shortcut() )
          {
            do_callback();
            return 1;
          }
          return 0;
        default:
          return Fl_Widget::handle(event);
      }
    }
};

} // Avtk

#endif // AVTK_LFO_H

