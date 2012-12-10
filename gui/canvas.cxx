
#ifndef CANVAS
#define CANVAS

#include "canvas.hxx"

#include <gtkmm/drawingarea.h>

#include <vector>
#include <sstream>
#include <iostream>

#include <gdkmm.h>

using namespace std;

bool Canvas::on_motion_notify_event(GdkEventMotion* event)
{
  if ( mouseDown )
  {
    cout << "pointer @ " << event->x << " " << event->y << "  deltaX: " << event->x - clickX << "  deltaY: " << clickY - event->y << endl;
    float deltaXValue = float(event->x - clickX) / movementWeight;
    cout << deltaXValue << endl;
    switch ( clickedWidget )
    {
      case OSC1_GRAPH:
          values[WAVETABLE1_POS] = clickXvalue + deltaXValue;
          cout << "wavtable pos "<< values[WAVETABLE1_POS] << endl;
          write_function( controller, WAVETABLE1_POS, sizeof(float), 0, (const void*)&values[WAVETABLE1_POS] );
          break;
    }
  }
  
  redraw();
  
  return true;
}

bool Canvas::on_button_release_event(GdkEventButton* event)
{
  mouseDown = false;
  return true;
}

bool Canvas::on_button_press_event(GdkEventButton* event)
{
  
  cout << "Click @ " << event->x << " " << event->y << endl;
  
  int x = event->x;
  int y = event->y;
  
  mouseDown = true;
  
  clickX = x;
  clickY = y;
  
  clickXvalue = float(event->x - clickX) / movementWeight;
  
  if ( x > 37 && y > 73 && x < 83 && y < 93 ) // Osc1 header
  {
    cout << "OSC 1 header, toggle on off" << endl;
    float writeVal;
    if ( oscVol[0] )
      writeVal = 0.f;
    else
      writeVal = 1.f;
    
    oscVol[0] = !oscVol[0];
    write_function( controller, OSC1_VOL, sizeof(float), 0, (const void*)&writeVal );
    redraw();
  }
  else if ( x > 83 && y > 73 && x < 192 && y < 93 ) // Osc1 waveform select
  {
    float waveform = 5-(5 * ((192-83) - (x-83)) / 108);
    cout << "OSC 1 waveform : " << waveform << endl;
  }
  else if ( x > 44 && y > 101 && x < 180 && y < 183 ) // Osc1 waveform graph
  {
    clickedWidget = OSC1_GRAPH;
    clickXvalue += values[WAVETABLE1_POS];
    //cout << "OSC 1 waveform : " << waveform << endl;
  }
  
  if ( x > 35 && y > 235 && x < 86 && y < 254 ) // Osc2 header
  {
    cout << "OSC 2 header, toggle on off" << endl;
    float writeVal;
    if ( oscVol[1] )
      writeVal = 0.f;
    else
      writeVal = 1.f;
    
    oscVol[1] = !oscVol[1];
    
    write_function( controller, OSC2_VOL, sizeof(float), 0, (const void*)&writeVal );
    redraw();
  }
  
  if ( x > 35 && y > 393 && x < 86 && y < 413 ) // Osc2 header
  {
    cout << "OSC 2 header, toggle on off" << endl;
    float writeVal;
    if ( oscVol[2] )
      writeVal = 0.f;
    else
      writeVal = 1.f;
    
    oscVol[2] = !oscVol[2];
    
    write_function( controller, OSC3_VOL, sizeof(float), 0, (const void*)&writeVal );
    redraw();
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
    
    float wavetableMod = 0.7;
    cr->rectangle(drawX, drawY, 138 * wavetableMod, 2); 
    setColour( cr, COLOUR_GREEN_1, 0.7 );
    cr->stroke();
  }
  // Waveform data: Volume
  {
    int drawX = X+135;
    int drawY = Y;
    
    float volume = 0.7;
    cr->rectangle(drawX, drawY+ 82*(1-volume), 2,  (82*volume) ); 
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
        int m1y = (y1 + yS / 2)   -   53 * lfoAmp;
        
        int m2x = x1 + xS / 3;
        int m2y = m1y;
        
        int endX = x1 + xS / 2;
        int endY = y1 + yS / 2;
        cr->curve_to(m1x, m1y, m2x, m2y, endX, endY);
        
        int m3x = x1 + 2 * xS / 3;
        int m3y = y1 + yS + 11;
        
        int m4x = x1 + 5 * xS / 6;
        int m4y = m3y;
        
        int end2X = x1 + xS;
        int end2Y = y1 + yS / 2;
        cr->curve_to(m3x, m3y, m4x, m4y, end2X, end2Y);
        
        //cr->line_to( x1 + Xs, y1 + 82 ); // br
        //cr->line_to( x1     , y1 + 82 ); // bl
        cr->close_path();
        setColour( cr, COLOUR_GREY_4, 0.5 );
        cr->fill_preserve();
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
  SimpleDial( cr, true, 246, 215, 0.5);
  SimpleDial( cr, true, 315, 215, 0.7);
  
  
  // Graph outline
  {
    cr->rectangle( X, Y, Xs, Ys );
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
  int Xs= 181-43;
  int Ys= 183-101;
  
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
    
    float wavetableMod = 0.7;
    cr->rectangle(drawX, drawY, 138 * wavetableMod, 2); 
    setColour( cr, COLOUR_GREEN_1, 0.7 );
    cr->stroke();
  }
  // Waveform data: Volume
  {
    int drawX = X+135;
    int drawY = Y;
    
    float volume = 0.7;
    cr->rectangle(drawX, drawY+ 82*(1-volume), 2,  (82*volume) ); 
    setColour( cr, COLOUR_ORANGE_1, 1.0 );
    cr->stroke();
  }
  
  
  
  // Graph outline
  {
    cr->rectangle( X, Y, Xs, Ys );
    setColour( cr, COLOUR_GREY_1 );
    cr->set_line_width(1.1);
    cr->stroke();
  }
  
}

void Canvas::drawOSC(Cairo::RefPtr<Cairo::Context> cr, int num)
{
  // wafeform co-ords
  int X =  43;
  int Y = 101;
  int Xs= 181-43;
  int Ys= 183-101;
    
  if ( num == 1 )
  {
    Y += 159;
  }
  if ( num == 2 )
  {
    Y += 318;
  }
  
  if ( num == 1 ) // draw status of all switches etc
  {
    // Oscillators on off
      int drawY = 74;
      for(int i = 0; i < 3; i++)
      {
        cr->rectangle( 33, drawY, 53, 20 );
        if ( oscVol[i] )
          setColour( cr, COLOUR_GREEN_1 , 0.3);
        else
          setColour( cr, COLOUR_GREY_1 , 0.1);
        cr->fill();
        drawY += 135 + 24;
      }
  }
  
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
    
    float wavetableMod = 0.7;
    cr->rectangle(drawX, drawY, 138 * values[WAVETABLE1_POS], 2);
    setColour( cr, COLOUR_GREEN_1, 0.7 );
    cr->stroke();
  }
  // Waveform data: Volume
  {
    int drawX = X+135;
    int drawY = Y;
    
    float volume = 0.7;
    cr->rectangle(drawX, drawY+ 82*(1-volume), 2,  (82*volume) ); 
    setColour( cr, COLOUR_ORANGE_1, 1.0 );
    cr->stroke();
  }
  
  // Waveform select boxes
  {
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
    
    drawX += 162 / 4.f;
    for(int i = 0; i < 3; i++)
    {
      cr->move_to(drawX, drawY+1);
      cr->line_to(drawX, drawY+17-1);
      drawX += 162 / 4.f;
    }
    setColour( cr, COLOUR_BLUE_1 );
    cr->stroke();
  }
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
    float limiter = 0.5;
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
    setColour( cr, COLOUR_BLUE_1, 0.2 );
    cr->fill_preserve();
    setColour( cr, COLOUR_BLUE_1 );
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
    cr->rectangle( x + 102, y + ySize*0.87*(1-masterVol), 16, 24);
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
  
  // HIGHPASS
  {
      int x = 376 - 158 + border;
      int y = 340;
      
      int xSize = 138;
      int ySize = 175 / 2 - 5;
      
      bool active = true;
      float highpass = 0.0;
      float cutoff = 0.2 + ((1-highpass)*0.7f);
      
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
      
      // move to bottom right, draw line to middle right
      cr->move_to( x + xSize, y + ySize );
      cr->line_to( x + xSize, y + (ySize*0.47));
      
      // Curve
      cr->curve_to( x + xSize - (xSize*cutoff)    , y+(ySize*0.5)    ,   // control point 1
                    x + xSize - (xSize*cutoff)    , y+(ySize * 0.0)     ,   // control point 2
                    x + xSize - (xSize*cutoff) -10, y+    ySize     );  // end of curve 1
      
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
  
  // LOWPASS
  {
      int x = 376 - 158 + border;
      int y = 340 + 175 / 2 + 5;
      
      int xSize = 138;
      int ySize = 175 / 2 - 5;
      
      bool active = true;
      float lowpass = 1.0;
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
  
  // highpass, lowpass outline
  cr->rectangle( x, y, xSize, (ySize/2.f) - 5 );
  cr->rectangle( x, y + ySize/2.f + 5, xSize, (ySize/2.f) - 5 );
  setColour( cr, COLOUR_GREY_1 );
  cr->set_line_width(1.1);
  cr->stroke();
}




#endif
