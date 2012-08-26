
#ifndef CANVAS
#define CANVAS

#include "canvas.hxx"

#include <gtkmm/drawingarea.h>

#include <vector>
#include <sstream>
#include <iostream>

#include <gdkmm.h>

using namespace std;



bool Canvas::on_button_press_event(GdkEventButton* event)
{
  cout << "Click @ " << event->x << " " << event->y << endl;
  
  int x = event->x;
  int y = event->y;
  
  if ( x > 37 && y > 73 && x < 83 && y < 93 ) // Osc1 header
  {
    cout << "OSC 1 header, toggle on off" << endl;
  }
  else if ( x > 83 && y > 73 && x < 192 && y < 93 ) // Osc1 header
  {
    float waveform = 5-(5 * ((192-83) - (x-83)) / 108);
    cout << "OSC 1 waveform : " << waveform << endl;
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
  
  // WAVEFORM graph
    cr->rectangle( X, Y, Xs, Ys );
    setColour( cr, COLOUR_GREY_4 );
    cr->fill();
    
    // draw "frequency guides"
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
