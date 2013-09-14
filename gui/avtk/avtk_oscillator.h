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


#ifndef AVTK_OSCILLATOR_H
#define AVTK_OSCILLATOR_H

#include <FL/Fl_Slider.H>

namespace Avtk
{

class Oscillator : public Fl_Slider
{
  public:
    Oscillator(int _x, int _y, int _w, int _h, const char *_label =0):
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
    
    void Y (float v){ wavetableVol =  v; redraw(); }
    void X (float v){ wavetablePos =  v; redraw(); }
    
    float Y(){return wavetableVol;}
    float X(){return wavetablePos;}
    
    float wavetableVol;
    float wavetablePos;
    
    bool active;
    bool mouseOver;
    bool highlight;
    int x, y, w, h;
    const char* label;
    
    int mouseClickedX;
    int mouseClickedY;
    bool mouseClicked;
    
    void draw()
    {
      if (damage() & FL_DAMAGE_ALL)
      {
        cairo_t *cr = Fl::cairo_cc();
        
        cairo_save( cr );
        
        // WAVEFORM graph
        cairo_rectangle( cr, x, y, w, h );
        cairo_set_source_rgb( cr, 28 / 255.f,  28 / 255.f ,  28 / 255.f );
        cairo_fill( cr );
        
        // draw guides
        double dashes[2];
        dashes[0] = 2.0;
        dashes[1] = 2.0;
        cairo_set_dash ( cr, dashes, 2, 0.0);
        cairo_set_line_width( cr, 1.0);
        for ( int i = 0; i < 4; i++ )
        {
          cairo_move_to( cr, x + ((w / 4.f)*i), y );
          cairo_line_to( cr, x + ((w / 4.f)*i), y + h );
        }
        for ( int i = 0; i < 4; i++ )
        {
          cairo_move_to( cr, x    , y + (( h / 4.f)*i) );
          cairo_line_to( cr, x + w, y + (( h / 4.f)*i) );
        }
        cairo_set_source_rgba( cr,  66 / 255.f,  66 / 255.f ,  66 / 255.f , 0.5 );
        cairo_stroke( cr );
        cairo_set_dash ( cr, dashes, 0, 0.0); // disable dashes: 0 dashes
        
        
        // Waveform data: WavetableMod
        cairo_rectangle( cr, x, y + h - 4, 138 * wavetablePos, 2);
        cairo_set_source_rgb( cr, 25 / 255.f, 255 / 255.f ,   0 / 255.f  );
        cairo_stroke(cr);
        
        // Waveform data: Volume
        cairo_set_line_width(cr, 2.4);
        cairo_rectangle(cr, x+w-4, y + 82*(1-wavetableVol), 2,  (82*wavetableVol) ); 
        cairo_set_source_rgba( cr, 255 / 255.f, 104 / 255.f ,   0 / 255.f , 1 );
        cairo_stroke(cr);
        
        // graph center circle:
        cairo_arc( cr, x + w/4.f + (w/2.f) * wavetablePos,
                  y + h/4.f + (h/2.f) * (1-wavetableVol), 7, 0, 6.28 );
        cairo_set_line_width(cr, 2.0 );
        cairo_set_source_rgb( cr, 1.0, 0.48 , 0.f );
        cairo_stroke(cr);
        
        // stroke rim
        cairo_rectangle(cr, x, y, w, h);
        cairo_set_source_rgba( cr, 126 / 255.f, 126 / 255.f , 126 / 255.f , 0.8 );
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
      }// if DAMAGE
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
        case FL_DRAG:
          {
            if ( Fl::event_state(FL_BUTTON1) )
            {
              if ( mouseClicked == false ) // catch the "click" event
              {
                mouseClickedX = Fl::event_x();
                mouseClickedY = Fl::event_y();
                mouseClicked = true;
              }
              
              float deltaX = mouseClickedX - Fl::event_x();
              float deltaY = mouseClickedY - Fl::event_y();
              
              float valX = wavetablePos;
              valX -= deltaX / 100.f;
              float valY = wavetableVol;
              valY += deltaY / 100.f;
              
              if ( valX > 1.0 ) valX = 1.0;
              if ( valX < 0.0 ) valX = 0.0;
              
              if ( valY > 1.0 ) valY = 1.0;
              if ( valY < 0.0 ) valY = 0.0;
              
              //handle_drag( value + deltaY );
              X( valX );
              Y(valY);
              
              mouseClickedX = Fl::event_x();
              mouseClickedY = Fl::event_y();
              redraw();
              do_callback();
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
            mouseClicked = false;
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

#endif // AVTK_OSCILLATOR_H

