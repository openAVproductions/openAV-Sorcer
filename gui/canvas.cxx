
#ifndef CANVAS
#define CANVAS

#include "canvas.hxx"

#include <gtkmm/drawingarea.h>

#include <vector>
#include <sstream>
#include <iostream>

#include <gdkmm.h>

using namespace std;

float clip(float v)
{
  float tmp = v;
  if ( v > 1.0f )
    tmp = 1.0f;
  if ( v < 0.0f )
    tmp = 0.0f;
  
  return tmp;
}

bool Canvas::on_motion_notify_event(GdkEventMotion* event)
{
  float zero = 0.f;
  
  float tmp = 0.f;
  
  if ( mouseDown )
  {
    //cout << "pointer @ " << event->x << " " << event->y << "  deltaX: " << event->x - clickX << "  deltaY: " << clickY - event->y << endl;
    float deltaXValue = float(event->x - clickX) / movementWeight;
    float deltaYValue = float(clickY - event->y) / movementWeight;
    
    switch ( clickedWidget )
    {
      case OSC1_GRAPH:
          values[WAVETABLE1_POS] = clip(clickXvalue + deltaXValue);
          values[OSC1_VOL] = clip(clickYvalue + deltaYValue);
          write_function( controller, WAVETABLE1_POS, sizeof(float), 0, (const void*)&values[WAVETABLE1_POS] );
          if ( oscOn[0] )
            write_function( controller, OSC1_VOL      , sizeof(float), 0, (const void*)&values[OSC1_VOL      ] );
          else
            write_function( controller, OSC1_VOL      , sizeof(float), 0, (const void*)&zero );
          break;
      case OSC2_GRAPH:
          values[WAVETABLE2_POS] = clip(clickXvalue + deltaXValue);
          values[OSC2_VOL] = clip(clickYvalue + deltaYValue);
          write_function( controller, WAVETABLE2_POS, sizeof(float), 0, (const void*)&values[WAVETABLE2_POS] );
          if ( oscOn[1] )
            write_function( controller, OSC2_VOL      , sizeof(float), 0, (const void*)&values[OSC2_VOL      ] );
          else
            write_function( controller, OSC2_VOL      , sizeof(float), 0, (const void*)&zero );
          break;
      case OSC3_GRAPH:
          values[OSC3_VOL] = clip(clickYvalue + deltaYValue);
          if ( oscOn[2] )
            write_function( controller, OSC3_VOL      , sizeof(float), 0, (const void*)&values[OSC3_VOL      ] );
          else
            write_function( controller, OSC3_VOL      , sizeof(float), 0, (const void*)&zero );
          break;
      case LFO_TO_WAVE1:
          values[LFO1_WAVETABLE1_POS] = clip(clickYvalue + deltaYValue);
          write_function( controller, LFO1_WAVETABLE1_POS, sizeof(float), 0, (const void*)&values[LFO1_WAVETABLE1_POS] );
          break; 
      case LFO_TO_WAVE2:
          values[LFO1_WAVETABLE2_POS] = clip(clickYvalue + deltaYValue);
          write_function( controller, LFO1_WAVETABLE2_POS, sizeof(float), 0, (const void*)&values[LFO1_WAVETABLE2_POS] );
          break; 
      case LFO_RATE:
          values[LFO1_FREQ] = clip(clickYvalue + deltaYValue);
          tmp = values[LFO1_FREQ];
          //cout << "writing LFO rate " << tmp << endl;
          write_function( controller, LFO1_FREQ, sizeof(float), 0, (const void*)&tmp );
          break; 
      case LFO_AMP:
          values[LFO1_AMP] = clip(clickYvalue + deltaYValue);
          write_function( controller, LFO1_AMP, sizeof(float), 0, (const void*)&values[LFO1_AMP] );
          break;
      case FILTER_CUTOFF:
          values[FILTER1_CUTOFF] = clip(clickYvalue + deltaYValue);
          tmp = values[FILTER1_CUTOFF];
          //cout << "filter cutoff " << tmp << endl;
          write_function( controller, FILTER1_CUTOFF, sizeof(float), 0, (const void*)&tmp );
          break; 
      case FILTER_LFO_RANGE:
          values[FILTER1_LFO_RANGE] = clip(clickYvalue + deltaYValue);
          tmp = values[FILTER1_LFO_RANGE];
          //cout << "port id, filter LFO range " << FILTER1_LFO_RANGE << " , " << tmp << endl;
          write_function( controller, FILTER1_LFO_RANGE, sizeof(float), 0, (const void*)&tmp );
          break;
      case ADSR_A:
          values[PORT_ADSR_ATTACK] =  clip(clickYvalue + 0.02 + deltaYValue);
          write_function( controller, PORT_ADSR_ATTACK, sizeof(float), 0, (const void*)&values[PORT_ADSR_ATTACK] );
          break;
      case ADSR_D:
          values[PORT_ADSR_DECAY] = clip(clickYvalue + deltaYValue);
          write_function( controller, PORT_ADSR_DECAY, sizeof(float), 0, (const void*)&values[PORT_ADSR_DECAY] );
          break;
      case ADSR_S:
          values[PORT_ADSR_SUSTAIN] = clip(clickYvalue + deltaYValue);
          write_function( controller, PORT_ADSR_SUSTAIN, sizeof(float), 0, (const void*)&values[PORT_ADSR_SUSTAIN] );
          break;
      case ADSR_R:
          values[PORT_ADSR_RELEASE] = clip(clickYvalue + deltaYValue);
          write_function( controller, PORT_ADSR_RELEASE, sizeof(float), 0, (const void*)&values[PORT_ADSR_RELEASE] );
          break;
      case MASTER:
          values[MASTER_VOL] = clip(clickYvalue + deltaYValue);
          write_function( controller, MASTER_VOL, sizeof(float), 0, (const void*)&values[MASTER_VOL] );
          break;
      default: 
          // return, so the widget doesn't redraw itself un-necessarily
          return true;
    }
    
    redraw();
  }
  
  return true;
}

bool Canvas::on_button_release_event(GdkEventButton* event)
{
  mouseDown = false;
  clickedWidget = CLICKED_NONE;
  return true;
}

bool Canvas::on_button_press_event(GdkEventButton* event)
{
  float zero = 0.f;
  //cout << "Click @ " << event->x << " " << event->y << endl;
  
  int x = event->x;
  int y = event->y;
  
  mouseDown = true;
  
  clickX = x;
  clickY = y;
  
  clickXvalue = float(event->x - clickX) / movementWeight;
  clickYvalue = float(clickY - event->y) / movementWeight;
  
  if ( x > 492 && y > 340 && x < 549 && y < 517 ) // Master vol
  {
    clickedWidget = MASTER;
    clickYvalue += values[MASTER_VOL];
  }
  
  
  if ( x > 37 && y > 73 && x < 83 && y < 93 ) // Osc1 header
  {
    /*
    cout << "OSC 1 header, toggle on off" << endl;
    oscOn[0] = !oscOn[0];
    if ( oscOn[0] )
      write_function( controller, OSC1_VOL      , sizeof(float), 0, (const void*)&values[OSC1_VOL      ] );
    else
      write_function( controller, OSC1_VOL      , sizeof(float), 0, (const void*)&zero );
    redraw();
    */
  }
  else if ( x > 83 && y > 73 && x < 192 && y < 93 ) // Osc1 waveform select
  {
    float waveform = 5-(5 * ((192-83) - (x-83)) / 108);
    //cout << "OSC 1 waveform : " << waveform << endl;
  }
  else if ( x > 44 && y > 101 && x < 180 && y < 183 ) // Osc1 waveform graph
  {
    clickedWidget = OSC1_GRAPH;
    clickXvalue += values[WAVETABLE1_POS];
    clickYvalue += values[OSC1_VOL];
    //cout << "OSC 1 waveform : " << waveform << endl;
  }
  
  if ( x > 35 && y > 235 && x < 86 && y < 254 ) // Osc2 header
  {
    /*
    cout << "OSC 2 header, toggle on off" << endl;
    oscOn[1] = !oscOn[1];
    if ( oscOn[1] )
      write_function( controller, OSC1_VOL      , sizeof(float), 0, (const void*)&values[OSC2_VOL      ] );
    else
      write_function( controller, OSC1_VOL      , sizeof(float), 0, (const void*)&zero );
    redraw();
    */
  }
  else if ( x > 44 && y > 261 && x < 180 && y < 343 ) // Osc2 waveform graph
  {
    clickedWidget = OSC2_GRAPH;
    clickXvalue += values[WAVETABLE2_POS];
    clickYvalue += values[OSC2_VOL];
  }
  
  if ( x > 35 && y > 393 && x < 86 && y < 413 ) // Osc3 header
  {
    /*
    cout << "OSC 3 header, toggle on off" << endl;
    oscOn[2] = !oscOn[2];
    if ( oscOn[2] )
      write_function( controller, OSC3_VOL      , sizeof(float), 0, (const void*)&values[OSC3_VOL      ] );
    else
      write_function( controller, OSC3_VOL      , sizeof(float), 0, (const void*)&zero );
    redraw();
    */
  }
  else if ( x > 44 && y > 421 && x < 180 && y < 502 ) // Osc3 waveform graph
  {
    //cout << "OSC 3 graph" << endl;
    clickedWidget = OSC3_GRAPH;
    clickYvalue += values[OSC3_VOL];
  }
  
  
  
  //     LFO, left
  if ( x > 240 && y > 197 && x < 286 && y < 234 ) // LFO to wavetable 1
  {
    clickedWidget = LFO_TO_WAVE1;
    clickYvalue += values[LFO1_WAVETABLE1_POS];
  }
  else if ( x > 243 && y > 242 && x < 285 && y < 279 ) // LFO to wavetable 2
  {
    clickedWidget = LFO_TO_WAVE2;
    clickYvalue += values[LFO1_WAVETABLE2_POS];
  }
  
  //     LFO, right
  if ( x > 310 && y > 200 && x < 353 && y < 232 ) // LFO to wavetable 1
  {
    clickedWidget = LFO_RATE;
    clickYvalue += values[LFO1_FREQ];
  }
  else if ( x > 310 && y > 246 && x < 353 && y < 280 ) // LFO to wavetable 2
  {
    clickedWidget = LFO_AMP;
    clickYvalue += values[LFO1_AMP];
  }
  
  
  //     REMOVE, left
  if ( x > 245 && y > 454 && x < 287 && y < 494 ) // LFO to wavetable 1
  {
    clickedWidget = FILTER_CUTOFF;
    clickYvalue += values[FILTER1_CUTOFF];
  }
  else if ( x > 310 && y > 454 && x < 353 && y < 494 ) // LFO to wavetable 2
  {
    clickedWidget = FILTER_LFO_RANGE;
    clickYvalue += values[FILTER1_LFO_RANGE];
  }
  
  
  //     ADSR, left
  if ( x > 424 && y > 197 && x < 470 && y < 234 ) // A
  {
    clickedWidget = ADSR_A;
    clickYvalue += values[PORT_ADSR_ATTACK];
  }
  else if ( x > 424 && y > 242 && x < 470 && y < 279 ) // D
  {
    clickedWidget = ADSR_D;
    clickYvalue += values[PORT_ADSR_DECAY];
  }
  
  //     ADSR, right
  if ( x > 494 && y > 200 && x < 537 && y < 232 ) // S
  {
    clickedWidget = ADSR_S;
    clickYvalue += values[PORT_ADSR_SUSTAIN];
  }
  else if ( x > 494 && y > 246 && x < 537 && y < 280 ) // R
  {
    clickedWidget = ADSR_R;
    clickYvalue += values[PORT_ADSR_RELEASE];
  }
  
}


void Canvas::drawLFO(Cairo::RefPtr<Cairo::Context> cr)
{
  
  // LFO box co-ords
  int X =  227;
  int Y = 101;
  int Xs= 181-43;
  int Ys= 183-101;
  
  // WAVEFORM graph
    cr->rectangle( X, Y, Xs, Ys );
    setColour( cr, COLOUR_GREY_4 );
    cr->fill();
    
      
  // background box for dials
    cr->rectangle( X, Y + Ys + 10, Xs, Ys + 5 );
    setColour( cr, COLOUR_GREY_4 );
    cr->fill();
    
    // draw guides
    std::valarray< double > dashes(2);
    dashes[0] = 2.0;
    dashes[1] = 2.0;
    cr->set_dash (dashes, 0.0);
    cr->set_line_width(1.0);
    cr->set_source_rgb (0.4,0.4,0.4);
    for ( int i = 0; i < 4; i++ )
    {
      cr->move_to( X + ((Xs / 4.f)*i), Y );
      cr->line_to( X + ((Xs / 4.f)*i), Y + Ys );
    }
    for ( int i = 0; i < 4; i++ )
    {
      cr->move_to( X     , Y + (( Ys / 4.f)*i) );
      cr->line_to( X + Xs, Y + (( Ys / 4.f)*i) );
    }
    cr->stroke();
    cr->unset_dash();

  
  // Waveform data: WavetableMod
  {
    int drawX = X;
    int drawY = Y + 79;
    
    float wavetableMod = values[LFO1_FREQ];
    cr->rectangle(drawX, drawY, 138 * wavetableMod, 2); 
    setColour( cr, COLOUR_GREEN_1, 0.7 );
    cr->stroke();
  }
  // Waveform data: Volume
  {
    int drawX = X+135;
    int drawY = Y;
    
    float volume = 0.7;
    cr->rectangle(drawX, drawY+ 82*(1-values[LFO1_AMP]), 2,  (82*values[LFO1_AMP]) ); 
    setColour( cr, COLOUR_ORANGE_1, 1.0 );
    cr->stroke();
  }
  
  
  // sinewave (in graph 1)
        int x1 = 228;
        int xS = Xs; // already defined
        int y1 = 103;
        int yS = 83;
        cr->move_to( x1, y1 + yS / 2 );
        
        int m1x = x1 + xS / 6;
        int m1y = (y1 + yS / 2)   -   53 * values[LFO1_AMP];
        
        int m2x = x1 + xS / 3;
        int m2y = m1y;
        
        int endX = x1 + xS / 2;
        int endY = y1 + yS / 2;
        cr->curve_to(m1x, m1y, m2x, m2y, endX, endY);
        
        int m3x = x1 + 2 * xS / 3;
        int m3y = y1 + yS / 2.f + ((yS + 7) * 0.5 * values[LFO1_AMP]);
        
        int m4x = x1 + 5 * xS / 6;
        int m4y = m3y;
        
        int end2X = x1 + xS;
        int end2Y = y1 + yS / 2;
        cr->curve_to(m3x, m3y, m4x, m4y, end2X, end2Y);
        
        //cr->line_to( x1 + Xs, y1 + 82 ); // br
        //cr->line_to( x1     , y1 + 82 ); // bl
        //cr->close_path();
        //setColour( cr, COLOUR_GREY_4, 0.5 );
        //cr->fill_preserve();
        setColour( cr, COLOUR_GREEN_1 );
        cr->stroke();
        
        /*
        // sinewave =- WITH AMP -=
        cr->move_to( 146, 64 + 82 / 2 );
        
        m1x = 146 + 172 / 6;
        m1y = (64 + 82 / 2)   -   53 * 1; // data->amp;
        
        m2x = 146 + 172 / 3;
        m2y = m1y;
        
        endX = 146 + 172 / 2;
        endY = 64 + 82 / 2;
        cr->curve_to(m1x, m1y, m2x, m2y, endX, endY);
        
        m3x = 146 + 2 * 172 / 3;
        m3y = (64 + 82 / 2)   +   53 * 1; // data->amp;
        
        m4x = 146 + 5 * 172 / 6;
        m4y = m3y;
        
        end2X = 146 + 172;
        end2Y = 64 + 82 / 2;
        cr->curve_to(m3x, m3y, m4x, m4y, end2X, end2Y);
        
        setColour( cr, COLOUR_GREY_1, 2 * 0.3 ); // rate
        cr->fill_preserve();
        setColour( cr, COLOUR_GREEN_1 );
        cr->stroke();
        */
  
  // LFO dials
  SimpleDial( cr, true, 246, 195, values[LFO1_WAVETABLE1_POS]);
  SimpleDial( cr, true, 315, 195, values[LFO1_FREQ] );
  SimpleDial( cr, true, 246, 240, values[LFO1_WAVETABLE2_POS]);
  SimpleDial( cr, true, 315, 240, values[LFO1_AMP] );
  
  
  // Graph outline
  {
    cr->rectangle( X, Y, Xs, Ys ); // higher
    cr->rectangle( X, Y + Ys + 10, Xs, Ys + 5 ); // lower
    setColour( cr, COLOUR_GREY_1 );
    cr->set_line_width(1.1);
    cr->stroke();
    
  }
  
}

void Canvas::drawADSR(Cairo::RefPtr<Cairo::Context> cr)
{
  
  // ADSR box co-ords
  int X =  227+184;
  int Y = 101;
  int Xs= 126;
  int Ys= 183-101;
  
  // WAVEFORM graph
    cr->rectangle( X, Y, Xs, Ys );
    setColour( cr, COLOUR_GREY_4 );
    cr->fill();
  
    // background box for dials
    cr->rectangle( X, Y + Ys + 10, Xs, Ys + 5 );
    setColour( cr, COLOUR_GREY_4 );
    cr->fill();
    
    // draw guides
    std::valarray< double > dashes(2);
    dashes[0] = 2.0;
    dashes[1] = 2.0;
    cr->set_dash (dashes, 0.0);
    cr->set_line_width(1.0);
    cr->set_source_rgb (0.4,0.4,0.4);
    for ( int i = 0; i < 4; i++ )
    {
      cr->move_to( X + ((Xs / 4.f)*i), Y );
      cr->line_to( X + ((Xs / 4.f)*i), Y + Ys );
    }
    for ( int i = 0; i < 4; i++ )
    {
      cr->move_to( X     , Y + (( Ys / 4.f)*i) );
      cr->line_to( X + Xs, Y + (( Ys / 4.f)*i) );
    }
    cr->stroke();
    cr->unset_dash();
  
  // ADSR graph plotting
    float a = values[PORT_ADSR_ATTACK];
    float d = values[PORT_ADSR_DECAY];
    float s = 1 - values[PORT_ADSR_SUSTAIN];
    float r = values[PORT_ADSR_RELEASE];
    
    cr->move_to( X + 2, Y + Ys - 2 );
    
    cr->line_to( X + 5 + (Xs * (a / 5.f)), Y + Ys * 0.1   ); // attack
    
    cr->rel_line_to( Xs * (d / 5.2f),   (Ys*0.9) * s   ); // decay, and sustain height
    
    cr->rel_line_to( Xs * 0.4, 0  ); // sustain horizontal line
    
    cr->rel_line_to( 0.85 * Xs * ( (r) / 5.f), Ys - (Ys*0.9) * s - Ys * 0.1  ); // remaining Y down
    
    setColour( cr, COLOUR_GREY_4, 0.5 );
    cr->fill_preserve();
    setColour( cr, COLOUR_BLUE_1 );
    cr->set_line_width(2);
    cr->set_line_join(Cairo::LINE_JOIN_ROUND);
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);
    cr->stroke();
  
  
  // ADSR dials
  SimpleDial( cr, true, 246+184, 195, values[PORT_ADSR_ATTACK]);
  SimpleDial( cr, true, 315+184, 195, values[PORT_ADSR_SUSTAIN] );
  SimpleDial( cr, true, 246+184, 240, values[PORT_ADSR_DECAY]);
  SimpleDial( cr, true, 315+184, 240, values[PORT_ADSR_RELEASE] );
  
  // Graph outline
  {
    cr->rectangle( X, Y, Xs, Ys ); // high
    cr->rectangle( X, Y + Ys + 10, Xs, Ys + 5 ); //lower
    setColour( cr, COLOUR_GREY_1 );
    cr->set_line_width(1.1);
    cr->stroke();
  }
  
}

void Canvas::drawOSC(Cairo::RefPtr<Cairo::Context> cr)
{
  int drawY = 74;
  int Y = 101;
  
  for ( int num = 0; num < 3; num++ )
  {
    // wafeform co-ords
    int X =  43;
    int Xs= 181-43;
    int Ys= 183-101;
    
    // Oscillators on off
    /*
    cr->rectangle( 33, drawY, 53, 20 );
    if ( oscOn[num] )
      setColour( cr, COLOUR_GREEN_1 , 0.3);
    else
      setColour( cr, COLOUR_GREY_1 , 0.1);
    cr->fill();
    drawY += 135 + 24;
    */
    
    // WAVEFORM graph
      cr->rectangle( X, Y, Xs, Ys );
      setColour( cr, COLOUR_GREY_4 );
      cr->fill();
      
      // draw guides
      std::valarray< double > dashes(2);
      dashes[0] = 2.0;
      dashes[1] = 2.0;
      cr->set_dash (dashes, 0.0);
      cr->set_line_width(1.0);
      cr->set_source_rgb (0.4,0.4,0.4);
      for ( int i = 0; i < 4; i++ )
      {
        cr->move_to( X + ((Xs / 4.f)*i), Y );
        cr->line_to( X + ((Xs / 4.f)*i), Y + Ys );
      }
      for ( int i = 0; i < 4; i++ )
      {
        cr->move_to( X     , Y + (( Ys / 4.f)*i) );
        cr->line_to( X + Xs, Y + (( Ys / 4.f)*i) );
      }
      cr->stroke();
      cr->unset_dash();
    
    // Waveform data: WavetableMod
    {
      int drawX = X;
      int drawY = Y + 79;
      
      cr->rectangle(drawX, drawY, 138 * values[WAVETABLE1_POS+num], 2);
      setColour( cr, COLOUR_GREEN_1, 0.9 );
      cr->stroke();
    }
    
    // Waveform data: Volume
    {
      int drawX = X+135;
      int drawY = Y;
      
      float volume = 0.7;
      cr->rectangle(drawX, drawY+ 82*(1-values[OSC1_VOL+num]), 2,  (82*values[OSC1_VOL+num]) ); 
      setColour( cr, COLOUR_PURPLE_1, 1.0 );
      cr->stroke();
    }
    // graph center circle:
    {
      cr->save();
      cr->arc(X + Xs/4.f + (Xs/2.f) * values[WAVETABLE1_POS+num],
              Y + Ys/4.f + (Ys/2.f) * (1-values[OSC1_VOL+num]),
              7, 0, 6.28 );
      cr->set_line_width( 2.0 );
      setColour( cr, COLOUR_ORANGE_1, 1.0 );
      cr->stroke();
      cr->restore();
    }
    
    // Waveform select boxes
    {
      /*
      int drawX = X + 43;
      int drawY = Y - 27;
      int boxXs= 105;
      int boxYs=  20;
      
      for(int i = 0; i < 5; i++)
      {
        cr->move_to(drawX, drawY+1);
        cr->line_to(drawX, drawY+boxYs-1);
        drawX += 21;
      }
      setColour( cr, COLOUR_BLUE_1 );
      cr->stroke();
      */
    }
    
    // Graph outline
    {
      cr->rectangle( X, Y, Xs, Ys );
      setColour( cr, COLOUR_GREY_1 );
      cr->set_line_width(1.1);
      cr->stroke();
    }
    
    // Lower select boxes
    {
      int drawX = X - 11;
      int drawY = Y + 91;
      
      // bg
      cr->rectangle(drawX+1, drawY, 159, 17);
      setColour( cr, COLOUR_GREY_4 );
      cr->fill_preserve();
      
      setColour( cr, COLOUR_BLUE_1 );
      cr->set_line_width(0.9);
      cr->stroke();
      
      /*
      drawX += 162 / 4.f;
      for(int i = 0; i < 3; i++)
      {
        cr->move_to(drawX, drawY+1);
        cr->line_to(drawX, drawY+17-1);
        drawX += 162 / 4.f;
      }
      setColour( cr, COLOUR_BLUE_1 );
      cr->stroke();
      */
    }
    
    // move down to next OSC
    Y += 159;
  }
  
  // Waveform data: Wavetable LFO mod
    cr->rectangle( 43, 199, 138 * values[LFO1_WAVETABLE1_POS], 3);
    cr->rectangle( 43, 358, 138 * values[LFO1_WAVETABLE2_POS], 3);
    setColour( cr, COLOUR_GREEN_1, 0.9 );
    cr->stroke();
  
}

void Canvas::drawMaster(Cairo::RefPtr<Cairo::Context> cr)
{
  int border = 10;
  int x = 559 - 158 + border;
  int y = 330 + border;
  
  int xSize = 158 - 2 * border;
  int ySize = 195 - 2 * border;
  
  cr->set_line_width(1.1);
  
  // LIMITER ZONE
    float limiter = 0.0;
    //graph background
    cr->rectangle( x, y, 76, 76);
    setColour( cr, COLOUR_GREY_4 );
    cr->fill();
    
    {
      int xSize = 76;
      int ySize = 76;
      // draw "frequency guides"
      std::valarray< double > dashes(2);
      dashes[0] = 2.0;
      dashes[1] = 2.0;
      cr->set_dash (dashes, 0.0);
      cr->set_line_width(1.0);
      cr->set_source_rgb (0.4,0.4,0.4);
      for ( int i = 0; i < 4; i++ )
      {
        cr->move_to( x + ((xSize / 4.f)*i), y );
        cr->line_to( x + ((xSize / 4.f)*i), y + ySize );
      }
      for ( int i = 0; i < 4; i++ )
      {
        cr->move_to( x       , y + ((ySize / 4.f)*i) );
        cr->line_to( x +xSize, y + ((ySize / 4.f)*i) );
      }
      cr->stroke();
      cr->unset_dash();
    }
    
    // "normal" line
    cr->move_to( x     , y + 76 );
    cr->line_to( x + 76, y      );
    setColour( cr, COLOUR_GREY_1, 0.5 );
    cr->stroke();
    
    // "active" line
    cr->move_to( x , y + 76 - 76 * limiter );
    cr->line_to(x + 76 - 76 * limiter, y  );
    cr->line_to(x + 76, y       );
    cr->line_to(x + 76, y + 76  );
    cr->line_to(x     , y + 76  );
    cr->close_path();
    //setColour( cr, COLOUR_BLUE_1, 0.2 );
    cr->fill_preserve();
    //setColour( cr, COLOUR_BLUE_1 );
    cr->set_line_width(1.0);
    cr->stroke();
    
    //cr->line_to(x + xSize, y + ySize );
    //cr->line_to(x , y + ySize );
    cr->close_path();
    
    // master limiter graph outline
    cr->rectangle( x, y, 76, 76);
    setColour( cr, COLOUR_GREY_1 );
    cr->stroke();
  
  // SAMPLE Zone
    // background
    cr->rectangle( x, y + 84, 76, 42);
    setColour( cr, COLOUR_GREY_4 );
    cr->fill();
    
    // text
    cr->move_to( x + 19, 438 );
    setColour( cr, COLOUR_GREY_1 );
    cr->show_text( "Sample" );
    
    // load button
    cr->rectangle( x + 4, y + 84 + 24, 32, 14);
    setColour( cr, COLOUR_GREY_1, 0.5 );
    cr->fill_preserve();
    setColour( cr, COLOUR_GREY_1 );
    cr->stroke();
    
    // load text
    cr->move_to( x + 7.5, y + 84 + 35 );
    setColour( cr, COLOUR_GREY_4 );
    cr->show_text( "Load" );
    
    // save button
    cr->rectangle( x + 40, y + 84 + 24, 32, 14);
    setColour( cr, COLOUR_GREY_1, 0.5 );
    cr->fill_preserve();
    setColour( cr, COLOUR_GREY_1 );
    cr->stroke();
    
    // save text
    cr->move_to( x + 43, y + 84 + 35 );
    setColour( cr, COLOUR_GREY_4 );
    cr->show_text( "Save" );
    
    // SAMPLE load save zone outline
    cr->rectangle( x, y + 84, 76, 42);
    setColour( cr, COLOUR_GREY_1 );
    cr->stroke();
  
  
  // SESSION ZONE
    //load save zone background
    cr->rectangle( x, y + 134, 76, 42);
    setColour( cr, COLOUR_GREY_4 );
    cr->fill();
    
    // text
    cr->move_to( x + 19, 488 );
    setColour( cr, COLOUR_GREY_1 );
    cr->show_text( "Session" );
    
    // load button
    cr->rectangle( x + 4, y + 134 + 24, 32, 14);
    setColour( cr, COLOUR_GREY_1, 0.5 );
    cr->fill_preserve();
    setColour( cr, COLOUR_GREY_1 );
    cr->stroke();
    
    // load text
    cr->move_to( x + 7.5, y + 134 + 35 );
    setColour( cr, COLOUR_GREY_4 );
    cr->show_text( "Load" );
    
    // save button
    cr->rectangle( x + 40, y + 134 + 24, 32, 14);
    setColour( cr, COLOUR_GREY_1, 0.5 );
    cr->fill_preserve();
    setColour( cr, COLOUR_GREY_1 );
    cr->stroke();
    
    // save text
    cr->move_to( x + 43, y + 134 + 35 );
    setColour( cr, COLOUR_GREY_4 );
    cr->show_text( "Save" );
    
    // session load save zone outline
    cr->rectangle( x, y + 134, 76, 42);
    setColour( cr, COLOUR_GREY_1 );
    cr->stroke();
  
  
  // FADER 
    //background
    cr->rectangle( x + xSize * 3 / 5.f, y, xSize * 2 / 5.f, ySize);
    setColour( cr, COLOUR_GREY_4 );
    cr->fill();
    
    // 0dB line
    cr->move_to( x + xSize * 3 / 5.f, y + 2 );
    cr->line_to( x + xSize          , y + 2 );
    setColour( cr, COLOUR_RECORD_RED );
    cr->stroke();
    
    // -10dB line
    cr->move_to( x + xSize * 3 / 5.f, y + 40 );
    cr->line_to( x + xSize          , y + 40 );
    setColour( cr, COLOUR_ORANGE_1 );
    cr->stroke();
    
    // -20dB line
    cr->move_to( x + xSize * 3 / 5.f, y + 76 );
    cr->line_to( x + xSize          , y + 76 );
    setColour( cr, COLOUR_GREEN_1 );
    cr->stroke();
    
    // master volume outline & center separator
    cr->rectangle( x + xSize * 3 / 5.f, y, xSize * 2 / 5.f, ySize + 1);
    cr->move_to( x + xSize * 4 / 5.f, y );
    cr->line_to( x + xSize * 4 / 5.f, y + ySize + 1 );
    setColour( cr, COLOUR_GREY_1 );
    cr->stroke();
    
    // Fader on the top
    float masterVol = 0.7;
    cr->rectangle( x + 102, y + ySize*0.87*(1-values[MASTER_VOL]), 16, 24);
    setColour( cr, COLOUR_GREY_4 );
    cr->fill_preserve();
    setColour( cr, COLOUR_BLUE_1);
    cr->set_line_width(1.5);
    cr->stroke();
    cr->set_line_width(1.1);
}





void Canvas::drawRemove(Cairo::RefPtr<Cairo::Context> cr)
{
  int border = 10;
  int x = 376 - 158 + border;
  int y = 330 + border;
  
  int xSize = 158 - 2 * border;
  int ySize = 195 - 2 * border;
  
  // highpass, lowpass graph backgrounds
  cr->rectangle( x, y, xSize, (ySize/2.f) - 5 );
  cr->rectangle( x, y + ySize/2.f + 5, xSize, (ySize/2.f) - 5 );
  setColour( cr, COLOUR_GREY_4 );
  cr->fill();
  
  // LOWPASS
  {
      int x = 376 - 158 + border;
      int y = 340;
      
      int xSize = 138;
      int ySize = 175 / 2 - 5;
      
      bool active = true;
      float lowpass = values[FILTER1_CUTOFF];
      float cutoff = 0.2 + (lowpass*0.7f);
      
      // draw "frequency guides"
      std::valarray< double > dashes(2);
      dashes[0] = 2.0;
      dashes[1] = 2.0;
      cr->set_dash (dashes, 0.0);
      cr->set_line_width(1.0);
      cr->set_source_rgb (0.4,0.4,0.4);
      for ( int i = 0; i < 4; i++ )
      {
        cr->move_to( x + ((xSize / 4.f)*i), y );
        cr->line_to( x + ((xSize / 4.f)*i), y + ySize );
      }
      for ( int i = 0; i < 4; i++ )
      {
        cr->move_to( x       , y + ((ySize / 4.f)*i) );
        cr->line_to( x +xSize, y + ((ySize / 4.f)*i) );
      }
      cr->stroke();
      cr->unset_dash();
      
      // move to bottom left, draw line to middle left
      cr->move_to( x , y + ySize );
      cr->line_to( x , y + (ySize*0.47));
      
      // Curve
      cr->curve_to( x + xSize * cutoff    , y+(ySize*0.5)  ,   // control point 1
                    x + xSize * cutoff    , y+(ySize * 0.0),   // control point 2
                    x + xSize * cutoff +10, y+ ySize       );  // end of curve 1
      
      if ( active )
        setColour(cr, COLOUR_BLUE_1, 0.2 );
      else
        setColour(cr, COLOUR_GREY_1, 0.2 );
      cr->close_path();
      cr->fill_preserve();
      
      // stroke cutoff line
      cr->set_line_width(1.5);
      if ( active )
        setColour(cr, COLOUR_BLUE_1 );
      else
        setColour(cr, COLOUR_GREY_1 );
      cr->stroke();
  }
  
  //SimpleDial( cr, true, x + border + xSize / 4 , y + ySize * 3/2, values[LFO1_WAVETABLE1_POS]);
  SimpleDial( cr, true, x + xSize/4.f-15  , y + ySize/2.f + 25, values[FILTER1_CUTOFF]);
  SimpleDial( cr, true, x + xSize*3/4.f-15, y + ySize/2.f + 25, values[FILTER1_LFO_RANGE]);
  
  // Filter modulation amount
  {
    // co-ords of the box
    int x = 376 - 158 + border;
    int y = 340;
    int xSize = 138;
    int ySize = 175 / 2 - 5;
    
    cr->rectangle( x+(xSize/2) -  120/2.f * values[FILTER1_LFO_RANGE], y + 10, 240/2.f * values[FILTER1_LFO_RANGE], 3);
    setColour( cr, COLOUR_GREEN_1, 0.9 );
    cr->stroke();
  }
  
  // highpass, lowpass outline
  cr->rectangle( x, y, xSize, (ySize/2.f) - 5 );
  cr->rectangle( x, y + ySize/2.f + 5, xSize, (ySize/2.f) - 5 );
  setColour( cr, COLOUR_GREY_1 );
  cr->set_line_width(1.1);
  cr->stroke();
}




#endif
