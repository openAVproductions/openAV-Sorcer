
#ifndef CANVAS
#define CANVAS


#include <gtkmm/drawingarea.h>

#include <vector>
#include <sstream>
#include <iostream>

#include <gdkmm.h>

#include "refractor.h"

using namespace std;

class Canvas : public Gtk::DrawingArea
{
  public:
    Canvas(GuiState* gui)
    {
      guiState = gui;
      sampleSet = false;
      width = 956;
      height = 546;
      set_size_request( width, height );
      
      target = 0; // for mousemove
      
      xos = 38;
      yos = 83;
      
      sampleX = 882;
      sampleY = 198;
      
      guiState->pitch = 0.5;
      guiState->masterVol = 0.7;
      
      loadHeaderImage();
      
      // connect GTK signals
      add_events( Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK| Gdk::POINTER_MOTION_MASK );
      
      signal_motion_notify_event() .connect( sigc::mem_fun(*this, &Canvas::on_mouse_move ) );
      signal_button_press_event()  .connect( sigc::mem_fun(*this, &Canvas::on_button_press_event) );
      signal_button_release_event().connect( sigc::mem_fun(*this, &Canvas::on_button_release_event) );
    }
    
    void setSample(long frames, float* data)
    {
      cout << "Canvas::setSample()" << endl;
      
      if ( frames && data )
      {
        sampleFrames = frames;
        sampleData   = data;
        sampleSet = true;
        cout << "Canvas got sample of " << frames << " frames." << endl;
      }
    }
        
    bool redraw()
    {
      Glib::RefPtr<Gdk::Window> win = get_window();
      if (win)
      {
          Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                  get_allocation().get_height());
          win->invalidate_rect(r, false);
      }
      return true;
    }
    
    // will redraw a portion of the screen
    bool redraw(int x, int y, int sx, int sy)
    {
      Glib::RefPtr<Gdk::Window> win = get_window();
      if (win)
      {
          Gdk::Rectangle r(x,y,sx,sy);
          win->invalidate_rect(r, false);
      }
      return true;
    }
    
  protected:
    // GUI state struct
    GuiState* guiState;
    
    // Image header
    bool headerLoaded;
    Glib::RefPtr< Gdk::Pixbuf > imagePointer;
    Cairo::RefPtr< Cairo::ImageSurface > imageSurfacePointer;
    
    // Sample variables
    bool sampleSet;
    long sampleFrames;
    float* sampleData;
    
    // interface
    static const char*  headerNames[];
    bool mouseDown;
    float clickValue;
    float clickValueX;
    
    float* target;
    
    float mouseDeltaX;
    float mouseDeltaY;
    
    int width, height;
    
    // sample draw area
    int xos;
    int yos;
    int sampleX;
    int sampleY;
    
    // ### Functions below here
    
    enum Colour {
      COLOUR_ORANGE_1 = 0,
      COLOUR_ORANGE_2,
      COLOUR_ORANGE_3,
      COLOUR_GREEN_1,
      COLOUR_GREEN_2,
      COLOUR_GREEN_3,
      COLOUR_BLUE_1,
      COLOUR_BLUE_2,
      COLOUR_BLUE_3,
      COLOUR_PURPLE_1,
      COLOUR_PURPLE_2,
      COLOUR_PURPLE_3,
      COLOUR_GREY_1,
      COLOUR_GREY_2,
      COLOUR_GREY_3,
      COLOUR_GREY_4,
      // specials
      COLOUR_BACKGROUND,
      COLOUR_RECORD_RED,
      COLOUR_TRANSPARENT,
    };
    
    static const int retriggerColour[];
    
    // Enum values are defined by which port index they have on the DSP
    // part. They are defined in refractor.h
    enum ClickedWidget {
      CLICKED_WIDGET_NONE           = 0,
      CLICKED_WIDGET_RETRIGGER      = REFRACTOR_CONTROL_RETRIGGER,
      CLICKED_WIDGET_BITCRUSHER     = REFRACTOR_CONTROL_BITCRUSH,
      CLICKED_WIDGET_DISTORTION     = REFRACTOR_CONTROL_DISTORTION,
      CLICKED_WIDGET_HIGHPASS       = REFRACTOR_CONTROL_HIGHPASS,
      CLICKED_WIDGET_LOWPASS        = REFRACTOR_CONTROL_LOWPASS,
      CLICKED_WIDGET_MASTER_VOLUME  = REFRACTOR_MASTER_VOLUME,
      CLICKED_WIDGET_PITCH          = REFRACTOR_CONTROL_PITCHSPEED,
    };
    
    ClickedWidget clickedWidget;
    
    void loadHeaderImage()
    {
      // Load pixbuf
      try {
        imagePointer = Gdk::Pixbuf::create_from_file ("/usr/lib/lv2/refractor.lv2/header.png");
        headerLoaded = true;
      }
      catch(Glib::FileError& e)
      {
        headerLoaded = false;
      }
      
      if ( !headerLoaded ) // if not in /usr/lib/lv2, try local
      {
        try {
          imagePointer = Gdk::Pixbuf::create_from_file ("/usr/local/lib/lv2/refractor.lv2/header.png");
          headerLoaded = true;
        }
        catch(Glib::FileError& e)
        {
          cout << "Refractor: Header image could not be loaded! Continuing..." << e.what() << endl;
          headerLoaded = false;
          return;
        }
      }
      
      // Detect transparent colors for loaded image
      Cairo::Format format = Cairo::FORMAT_RGB24;
      if (imagePointer->get_has_alpha())
      {
          format = Cairo::FORMAT_ARGB32;
      }
      
      // Create a new ImageSurface
      imageSurfacePointer = Cairo::ImageSurface::create  (format, imagePointer->get_width(), imagePointer->get_height());
      
      // Create the new Context for the ImageSurface
      Cairo::RefPtr< Cairo::Context > imageContextPointer = Cairo::Context::create (imageSurfacePointer);
      
      // Draw the image on the new Context
      Gdk::Cairo::set_source_pixbuf (imageContextPointer, imagePointer, 0.0, 0.0);
      imageContextPointer->paint();
      
      headerLoaded = true;
    }
    
    bool on_expose_event			(GdkEventExpose* event)
    {
      Glib::RefPtr<Gdk::Window> window = get_window();
      
      if(window)
      {
        Gtk::Allocation allocation = get_allocation();
        width = allocation.get_width();
        height = allocation.get_height();
        
        // clip reigon
        Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
        cr->rectangle(event->area.x, event->area.y,
                event->area.width, event->area.height);
        cr->clip();
        
        cr->rectangle(event->area.x, event->area.y,
            event->area.width, event->area.height);
        cr->set_source_rgb(0.0,0.0,0.0);
        cr->fill();
        
        if ( headerLoaded )
        {
          cr->save();
          
          // draw header
          cr->set_source (imageSurfacePointer, 0.0, 0.0);
          cr->rectangle (0.0, 0.0, imagePointer->get_width(), imagePointer->get_height());
          cr->clip();
          cr->paint();
          
          cr->restore();
        }
        
        // 5px border around audio
        Background(cr, xos - 5, yos - 5, sampleX + 10, sampleY + 10, -1);
        
        cr->move_to( xos , yos + 0.5 * sampleY );
        
        
        if ( sampleSet ) // instance access feature nessisary
        {
          // loop for drawing each sample
          float* current = sampleData;
          for (long i = 0; i < sampleFrames; i += 20 )
          {
            cr->line_to( xos + sampleX * ( float(i) / sampleFrames), yos + sampleY * 0.5 + (*current) * sampleY * 0.5 );
            current += 20;
          }
          cr->set_line_width(0.8);
          setColour( cr, COLOUR_GREY_4 );
          cr->stroke();
          
          // draw slice markers every 16th section
          for(int i = 0; i < 16; i++)
          {
            Marker(cr, i / 16.f, "");
          }
        }
        else
        {
          // display "Your host doesn't support instance access"
          std::string text = "Your host doesn't support Instance Access";
          cr->move_to( xos + sampleX / 5, yos + sampleY / 2 + 12.5 );
          setColour( cr, COLOUR_GREY_4 );
          cr->set_font_size( 25 );
          cr->show_text( text );
        }
        
        /*
        // draw playing segments by using "voices" from plugin
        for(int i = 0; i < 8; i++ )
        {
          if ( sampleVoice[i].playing )
          {
            cout << "SampleVoice " << i << " playing, drawing now!" << endl;
            float start = sampleVoice[i].startFrame / sampleFrames;
            
            cr->rectangle( xos + start * sampleX, yos, sampleX * (1 / 16.f) , sampleY );
            setColour( cr, COLOUR_BLUE_1, 0.3 );
            cr->fill();
          }
        }
        */
        
        // loop draw the bottom 5 backgrounds
        int drawX = 33;
        for ( int i = 0; i < 5; i++ )
        {
          Background(cr, drawX, 310, 159, 215, i);
          drawX += 159 + 24;
        }
        
        // draw source widget
        drawSource(cr);
        
        // draw retrigger widget
        drawRetrigger(cr);
        
        // draw destroy widget
        drawDestroy(cr);
        
        // draw remove widget
        drawRemove(cr);
        
        // draw master widget
        drawMaster(cr);
      }
      return true;
    }
    
    void drawDestroy(Cairo::RefPtr<Cairo::Context> cr)
    {
      int border = 10;
      int x = 400 + border;
      int y = 330 + border;
      
      int xSize = 158 - 2 * border;
      int ySize = 195 - 2 * border;
      
      // bitcrush, distortion graph backgrounds
      cr->rectangle( x, y, xSize, (ySize/2.f) - 5 );
      cr->rectangle( x, y + ySize/2.f + 5, xSize, (ySize/2.f) - 5 );
      setColour( cr, COLOUR_GREY_4 );
      cr->fill();
      
      // BitCrush
      {
        int x = 410;
        int y = 340;
        
        int xSize = 138;
        int ySize = 175 / 2 - 5;
        
        bool active = true;
        float bitcrush = 1 - (guiState->bitcrush*0.75);
        float sampRate = 1; //guiState->masterVol;
        
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
        
        // move to bottom left
        cr->move_to( x , y + ySize );
        
        // draw steps for bitcrush
        int prevY = y + ySize;
        for ( int i = 0; i < 16; i++)
        {
          int currY = y + ySize - (ySize * i/(bitcrush*16.f));
          int currX = x + xSize * i/(bitcrush*16.f);
          if ( currY > y + ySize || currX > x + xSize )
            break;
          
          cr->line_to ( currX, prevY );
          cr->line_to ( currX, currY );
          prevY = currY;
        }
        
        cr->line_to ( x + xSize, prevY );
        cr->line_to ( x + xSize, y + ySize );
        cr->close_path();
        
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
      
      // Distortion
      {
        int x = 410;
        int y = 340 + 175 / 2 + 5;
        
        int xSize = 138;
        int ySize = 175 / 2 - 5;
        
        bool active = true;
        float distort = guiState->distortion;
        
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
        
        // move to bottom left
        cr->move_to( x , y + ySize );
        
        cr->curve_to( x + xSize * distort          , y+ySize,   // control point 1
                      x + xSize - (xSize * distort), y      ,   // control point 2
                      x + xSize                           , y      );  // end of curve 1, start curve 2
        
        cr->line_to ( x + xSize, y + ySize );
        cr->close_path();
        
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
      
      
      // bitcrush, distortion outline
      cr->rectangle( x, y, xSize, (ySize/2.f) - 5 );
      cr->rectangle( x, y + ySize/2.f + 5, xSize, (ySize/2.f) - 5 );
      setColour( cr, COLOUR_GREY_1 );
      cr->stroke();
    }
    void drawRemove(Cairo::RefPtr<Cairo::Context> cr)
    {
      int border = 10;
      int x = 583 + border;
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
          int x = 593;
          int y = 340;
          
          int xSize = 138;
          int ySize = 175 / 2 - 5;
          
          bool active = true;
          float cutoff = 0.2 + ((1-guiState->highpass)*0.7f);
          
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
          int x = 593;
          int y = 340 + 175 / 2 + 5;
          
          int xSize = 138;
          int ySize = 175 / 2 - 5;
          
          bool active = true;
          float cutoff = 0.2 + ((guiState->lowpass)*0.7f);
          
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
    
    
    void drawMaster(Cairo::RefPtr<Cairo::Context> cr)
    {
      int border = 10;
      int x = 766 + border;
      int y = 330 + border;
      
      int xSize = 158 - 2 * border;
      int ySize = 195 - 2 * border;
      
      cr->set_line_width(1.1);
      
      // LIMITER ZONE
        float limiter = guiState->limiterThreshold;
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
        cr->move_to( 795, 438 );
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
        cr->move_to( 795, 488 );
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
        cr->rectangle( 878, y + ySize*0.87*(1-guiState->masterVol), 16, 24);
        setColour( cr, COLOUR_GREY_4 );
        cr->fill_preserve();
        setColour( cr, COLOUR_BLUE_1);
        cr->set_line_width(1.5);
        cr->stroke();
        cr->set_line_width(1.1);
    }
    
    
    void drawRetrigger(Cairo::RefPtr<Cairo::Context> cr)
    {
      int border = 10;
      int x = 216 + border;
      int y = 330 + border;
      
      int xSize = 158 - 2 * border;
      int ySize = 195 - 2 * border;
      
      // background
      cr->rectangle( x, y, xSize, ySize );
      setColour( cr, COLOUR_GREY_4 );
      cr->fill();
      
      // draw highlighted squares as appropriate
      float retrig = 0; // 0 - 4;
      
      for ( int i = 0; i < 16; i++ )
      {
        // draw the rectangle position based on the current retrigger amount
        cr->rectangle(x + (xSize/5.f ) * guiState->retriggerArray[i],
                      y + (ySize/16.f) * i,
                      xSize * 0.2f,
                      ySize * 1/16.f );
        // set the colour based on retrigger amount
        setColour( cr, static_cast<Colour>(retriggerColour[ guiState->retriggerArray[i] ]), 0.6 );
        cr->fill();
      }
      
      // vertical lines
      for ( int i = 0; i < 5; i ++)
      {
        cr->move_to( x + xSize * 0.2 * i, y );
        cr->line_to( x + xSize * 0.2 * i, y + ySize );
      }
      
      // horizontal lines
      for ( int i = 0; i < 16; i ++)
      {
        cr->move_to( x        , y + ySize * 1/16.f * i );
        cr->line_to( x + xSize, y + ySize * 1/16.f * i );
      }
      
      // outline rectangle
      cr->rectangle( x, y, xSize, ySize );
      cr->set_line_width(1.1);
      setColour( cr, COLOUR_GREY_1 );
      cr->stroke();
    }
    
    void drawSource(Cairo::RefPtr<Cairo::Context> cr)
    {
      int border = 10;
      int x = 34 + border;
      int y = 330 + border;
      
      int xSize = 158 - 2 * border;
      int ySize = 195 - 2 * border;
      
      
      // draw red | green
        // background
        cr->rectangle( 90, y, 26, ySize );
        setColour( cr, COLOUR_GREY_4 );
        cr->fill();
        
        // draw each reverse / forward block
        for ( int i = 0; i < 16; i++ )
        {
          bool playReverse = guiState->reverseArray[i];
          
          // uncomment to draw array of "reverse" slices
          //cr->rectangle( 90, y, 26, ySize );
          
          // set the colour based on reverse
          if ( playReverse )
            setColour( cr, COLOUR_RECORD_RED, 0.3 );
          else
            setColour( cr, COLOUR_GREEN_1   , 0.2 );
          cr->fill();
        }
      
      // draw 16 keys
        // key background
        cr->rectangle( x, y, xSize/4.f, ySize );
        cr->set_source_rgb( 0.9, 0.9, 0.9);
        cr->fill();
        
        // 16 keys lines
        for ( int i = 0; i < 16; i ++)
        {
          cr->move_to( x               , y + ySize * 1/16.f * i );
          cr->line_to( x + xSize, y + ySize * 1/16.f * i );
        }
        
        // end of keys line
        cr->move_to( x + xSize/4.f, y        );
        cr->line_to( x + xSize/4.f, y + ySize);
      
        // black keys
        for ( int i = 0; i < 16; i ++)
        {
          if ( i == 1 || i == 3 ||i == 6 ||i == 8 ||i == 10 ||i == 13 || i == 15 )
          {
            cr->rectangle( x, y + ySize * 1/16.f * i, xSize/4.f, ySize * 1/16.f);
            setColour( cr, COLOUR_GREY_4 );
            cr->fill();
          }
        }
      
      // FADER 
        //background
        cr->rectangle( x + xSize * 3 / 5.f, y, xSize * 2 / 5.f, ySize);
        setColour( cr, COLOUR_GREY_4 );
        cr->fill();
        
        
        // top red
        cr->move_to( x + xSize * 3 / 5.f, y + 2);
        cr->line_to( x + xSize          , y + 2);
        setColour( cr, COLOUR_RECORD_RED );
        cr->stroke();
        
        // top orange
        cr->move_to( x + xSize * 3 / 5.f, y + ySize / 4 );
        cr->line_to( x + xSize          , y + ySize / 4  );
        setColour( cr, COLOUR_ORANGE_1 );
        cr->stroke();
        
        // middle green
        cr->move_to( x + xSize * 3 / 5.f, y + 2*ySize / 4 );
        cr->line_to( x + xSize          , y + 2*ySize / 4 );
        setColour( cr, COLOUR_GREEN_1 );
        cr->stroke();
        
        // low orange
        cr->move_to( x + xSize * 3 / 5.f, y + 3*ySize / 4 );
        cr->line_to( x + xSize          , y + 3*ySize / 4 );
        setColour( cr, COLOUR_ORANGE_1 );
        cr->stroke();
        
        // low red
        cr->move_to( x + xSize * 3 / 5.f, y + ySize );
        cr->line_to( x + xSize          , y + ySize );
        setColour( cr, COLOUR_RECORD_RED );
        cr->stroke();
        
        
        // master volume outline & center separator
        cr->rectangle( x + xSize * 3 / 5.f, y, xSize * 2 / 5.f, ySize + 1);
        cr->move_to( x + xSize * 4 / 5.f, y );
        cr->line_to( x + xSize * 4 / 5.f, y + ySize + 1 );
        setColour( cr, COLOUR_GREY_1 );
        cr->set_line_width(1.1);
        cr->stroke();
        
        // Fader on the top
        int yPx = y + ySize*0.87*(1-guiState->pitch);
        //cout << "drawing source fader @ " << guiState->pitch << "  px value " << yPx << endl;
        cr->rectangle( 146, yPx , 16, 24);
        setColour( cr, COLOUR_GREY_4 );
        cr->fill_preserve();
        cr->set_line_width(1.5);
        setColour( cr, COLOUR_BLUE_1);
        cr->stroke();
      
      
      // outline rectangle
      cr->rectangle( 90, y, 26, ySize );
      cr->set_line_width(1.1);
      setColour( cr, COLOUR_GREY_1 );
      cr->stroke();
      
      // outline rectangle
      cr->rectangle( x, y, xSize / 4.f, ySize );
      cr->set_line_width(1.1);
      setColour( cr, COLOUR_GREY_1 );
      cr->stroke();
    }
    
    void Marker(Cairo::RefPtr<Cairo::Context> cr, float position, string name)
    {
      setColour( cr, COLOUR_GREY_1 );
      cr->set_line_width(1.2);
      cr->move_to( xos + sampleX * position , yos );
      cr->line_to( xos + sampleX * position , yos + sampleY );
      cr->stroke();
    }
    
    void Background(Cairo::RefPtr<Cairo::Context> cr, float x, float y, float sizeX, float sizeY, int num)
    {
      // fill background
      cr->rectangle( x, y, sizeX, sizeY);
      setColour( cr, COLOUR_GREY_3 );
      cr->fill();
      
      // set up dashed lines, 1 px off, 1 px on
      std::valarray< double > dashes(2);
      dashes[0] = 2.0;
      dashes[1] = 2.0;
      
      cr->set_dash (dashes, 0.0);
      cr->set_line_width(1.0);
      
      // loop over each 2nd line, drawing dots
      for ( int i = x; i < x + sizeX; i += 4 )
      {
        cr->move_to( i, y );
        cr->line_to( i, y + sizeY );
      }
      
      setColour( cr, COLOUR_GREY_4, 0.5 );
      cr->stroke();
      cr->unset_dash();
      
      // draw header
      if ( 0 <= num && num < 5 )
      {
        // backing
        cr->rectangle( x, y, sizeX, 20);
        setColour( cr, COLOUR_GREY_4 );
        cr->fill();
        
        // text
        std::string text = headerNames[num];
        cr->move_to( x + 10, y + 14 );
        setColour( cr, COLOUR_BLUE_1 );
        cr->set_font_size( 10 );
        cr->show_text( text );
        
        // lower stripe
        cr->move_to( x        , y + 20 );
        cr->line_to( x + sizeX, y + 20 );
        setColour( cr, COLOUR_BLUE_1 );
        cr->stroke();
      }
      
      // stroke rim
      cr->rectangle( x, y, sizeX, sizeY);
      setColour( cr, COLOUR_BLUE_1 );
      cr->stroke();
    }
    
    void SimpleDial( Cairo::RefPtr<Cairo::Context> cr, bool active, float x, float y, float value)
    {
      int xc = x + 16;
      int yc = y + 22;
      
      float radius = 14;
      
      cr->set_line_cap( Cairo::LINE_CAP_ROUND );
      cr->set_line_join( Cairo::LINE_JOIN_ROUND);
      cr->set_line_width(2.8);
      
      // Arc Angle Value
      cr->set_line_width(2.4);
      cr->move_to(xc,yc);
      cr->set_source_rgba( 0,0,0,0 );
      cr->stroke();
      
      // main arc
      if ( active )
        setColour(cr, COLOUR_GREY_4 );
      else
        setColour(cr, COLOUR_GREY_3 );
      cr->arc(xc,yc, radius, 2.46, 0.75 );
      cr->move_to(xc,yc);
      cr->stroke();
      
      cr->set_line_width(2.8);
      float angle;
      
      if ( value < 0 )
        value = 0.f;
      
      angle = 2.46 + (4.54 * value);
      
      if ( active )
        setColour(cr, COLOUR_GREY_1 );
      else
        setColour(cr, COLOUR_GREY_2 );
      
      cr->set_line_width(1.7);
      cr->arc(xc,yc, 13, 2.46, angle );
      cr->line_to(xc,yc);
      cr->stroke();
      cr->arc(xc,yc, 17, 2.46, angle );
      cr->line_to(xc,yc);
      cr->stroke();
    }
    
    void setColour( Cairo::RefPtr<Cairo::Context> cr, Colour c, float alpha)
    {
      switch( c )
      {
        case COLOUR_ORANGE_1:
          cr->set_source_rgba( 255 / 255.f, 104 / 255.f ,   0 / 255.f , alpha ); break;
        case COLOUR_ORANGE_2:
          cr->set_source_rgba( 178 / 255.f,  71 / 255.f ,   0 / 255.f , alpha ); break;
        case COLOUR_ORANGE_3:
          cr->set_source_rgba(  89 / 255.f,  35 / 255.f ,   0 / 255.f , alpha ); break;
        case COLOUR_GREEN_1:
          cr->set_source_rgba(  25 / 255.f, 255 / 255.f ,   0 / 255.f , alpha ); break;
        case COLOUR_GREEN_2:
          cr->set_source_rgba(  17 / 255.f, 179 / 255.f ,   0 / 255.f , alpha ); break;
        case COLOUR_GREEN_3:
          cr->set_source_rgba(   8 / 255.f,  89 / 255.f ,   0 / 255.f , alpha ); break;
        case COLOUR_BLUE_1:
          cr->set_source_rgba(   0 / 255.f, 153 / 255.f , 255 / 255.f , alpha ); break;
        case COLOUR_BLUE_2:
          cr->set_source_rgba(  20 / 255.f,  73 / 255.f , 109 / 255.f , alpha ); break;
        case COLOUR_BLUE_3:
          cr->set_source_rgba(   0 / 255.f,  53 / 255.f ,  89 / 255.f , alpha ); break;
        case COLOUR_PURPLE_1:
          cr->set_source_rgba( 230 / 255.f,   0 / 255.f , 255 / 255.f , alpha ); break;
        case COLOUR_PURPLE_2:
          cr->set_source_rgba( 161 / 255.f,   0 / 255.f , 179 / 255.f , alpha ); break;
        case COLOUR_PURPLE_3:
          cr->set_source_rgba(  80 / 255.f,   0 / 255.f ,  89 / 255.f , alpha ); break;
        case COLOUR_GREY_1:
          cr->set_source_rgba( 130 / 255.f, 130 / 255.f , 130 / 255.f , alpha ); break;
        case COLOUR_GREY_2:
          cr->set_source_rgba(  98 / 255.f,  98 / 255.f ,  98 / 255.f , alpha ); break;
        case COLOUR_GREY_3:
          cr->set_source_rgba(  66 / 255.f,  66 / 255.f ,  66 / 255.f , alpha ); break;
        case COLOUR_GREY_4:
          cr->set_source_rgba(  28 / 255.f,  28 / 255.f ,  28 / 255.f , alpha ); break;
        case COLOUR_RECORD_RED:
          cr->set_source_rgba(  226 / 255.f, 0/255.f , 0/255.f, alpha ); break;
        case COLOUR_TRANSPARENT:
          cr->set_source_rgba(  0, 0, 0, 0.f ); break;
        case COLOUR_BACKGROUND: default:
          cr->set_source_rgba(  40 / 255.f,  40 / 255.f ,  40 / 255.f , alpha ); break;
      }
    }
    
    void setColour( Cairo::RefPtr<Cairo::Context> cr, Colour c)
    {
      setColour(cr, c, 1.f);
    }
    
    bool on_button_press_event(GdkEventButton* event)
    {
      // set mousedown to false first, if a hotspot is clicked, it will
      // be set to true, same with clickedWidget
      mouseDown = false;
      clickedWidget = CLICKED_WIDGET_NONE;
      
      // for calculation of mouseMove
      clickValue = 1 - ( event->y / height);
      clickValueX = 1- ( event->x / height);
      
      cout << "Refractor: Press event \tX " << event->x << "\tY " << event->y << endl;
      if ( event->button == 1 || event->button == 3 )
      {
        int x = event->x;
        int y = event->y;
        
        
        if ( x > 623 && y > 26 && y < 70 ) // OpenAV
        {
          // If GVFS isn't installed on the local machine, GTK won't know
          // how to handle 
          GError* e = NULL;
          gtk_show_uri(NULL, "http://www.openavproductions.com", GDK_CURRENT_TIME, &e);
          if ( e )
          {
            cout << "Error opening \"www.openavproductions.com\"... " << e->message << endl;
          }
          return true;
        }
        
        if ( x > 411 && y > 341 && x < 549 && y < 424 )
        {
          //cout << "Bitcrush click!" << endl;
          mouseDown = true;
          target = &guiState->bitcrush;
          clickedWidget = CLICKED_WIDGET_BITCRUSHER;
        }
        
        if ( x > 411 && y > 434 && x < 549 && y < 516 )
        {
          //cout << "Distort click!" << endl;
          mouseDown = true;
          target = &guiState->distortion;
          clickedWidget = CLICKED_WIDGET_DISTORTION;
        }
        
        if ( x > 594 && y > 341 && x < 732 && y < 424 )
        {
          //cout << "Highpass click!" << endl;
          mouseDown = true;
          target = &guiState->highpass;
          clickedWidget = CLICKED_WIDGET_HIGHPASS;
        }
        
        if ( x > 594 && y > 434 && x < 732 && y < 516 )
        {
          //cout << "Lowpass click!" << endl;
          mouseDown = true;
          target = &guiState->lowpass;
          clickedWidget = CLICKED_WIDGET_LOWPASS;
        }
        
        if ( x > 236 && y > 340 && x < 236 + 138 && y < 340 + 175 ) // RETRIGGER ZONE
        {
          // location 399, 330
          // size = 158 x 195
          
          if ( event->button == 3 )
          {
            float writeVal;
            for (int i = 0; i < 16; i++)
            {
              
              // can't use a control port like this, only the "latest" value
              // is accepted, so the others aren't affected. Need to look at using
              // a custom "reset" Atom event. 
              /*
              guiState->retriggerArray[i] = 0;
              redraw(220, 340, 200, 500 );
              writeVal = int(i);
              guiState->write_function( guiState->controller, REFRACTOR_CONTROL_RETRIGGER, sizeof(float), 0, (const void*)&writeVal );
              */
            }
            return true;
          }
          
          int border = 10;
          int x = 216 + border;
          int y = 330 + border;
          
          int xSize = 158 - 2 * border;
          int ySize = 195 - 2 * border;
          
          int slice = (event->y - y) / (ySize / 16.f);
          int retrg = (event->x - x) / (xSize /  5.f);
          
          // write value into gui state
          guiState->retriggerArray[slice] = retrg;
          redraw(x,y,xSize,ySize);
          
          // multiplex slice & retrigger into the one float variable, to send to DSP
          float value = slice + (retrg / 10.f);
          //cout << "Retrigger slice " << slice << ", retrig " << retrg << "  value " << value << endl;
          
          guiState->write_function( guiState->controller, REFRACTOR_CONTROL_RETRIGGER, sizeof(float), 0, (const void*)&value);
        }
        
        if ( x > 859 && y > 341 && x < 917 && y < 516 )
        {
          if ( event->button == 3 )
          {
            guiState->masterVol = 0.7;
            redraw(859, 341, 200, 500 );
            guiState->write_function( guiState->controller, CLICKED_WIDGET_MASTER_VOLUME, sizeof(float), 0, (const void*)&guiState->masterVol);
            return true;
          }
          clickedWidget = CLICKED_WIDGET_MASTER_VOLUME;
          mouseDown = true;
          target = &guiState->masterVol;
        }
        
        if ( x > 127 && y > 341 && x < 185 && y < 516 )
        {
          if ( event->button == 3 )
          {
            guiState->pitch = 0.5;
            redraw(127, 185, 200, 500 );
            guiState->write_function( guiState->controller, CLICKED_WIDGET_PITCH, sizeof(float), 0, (const void*)&guiState->pitch);
            return true;
          }
          
          clickedWidget = CLICKED_WIDGET_PITCH;
          mouseDown = true;
          target = &guiState->pitch;
        }
        
      }
    }
    
    bool on_button_release_event(GdkEventButton* event)
    {
      mouseDown = false;
      target = 0;
    }
    
    bool on_mouse_move(GdkEventMotion* event)
    {
      // if mouse moves over "openavproductions.com", show "link" cursor
      if ( event->x > 623 && event->y > 20 && event->y < 70 )
      {
        Glib::RefPtr <Gdk::Window> ref_window;
        ref_window = get_window();
        Gdk::Cursor m_Cursor(Gdk::HAND1);
        ref_window->set_cursor(m_Cursor);
      }
      else
      {
        Glib::RefPtr <Gdk::Window> ref_window;
        ref_window = get_window();
        ref_window->set_cursor();
      }
      
      // when a click occurs, float* target is set to that location,
      // here we just work with target
      if ( mouseDown && target && clickedWidget != CLICKED_WIDGET_NONE )
      {
        float click  = 1 - (event->y / height);
        float clickX = 1 - (event->x / height);
        
        float delta = clickValue - click;
        
        bool invert = false;
        
        if ( clickedWidget == CLICKED_WIDGET_HIGHPASS )
        {
          // use X delta, not Y
          delta = clickValueX - clickX;
          invert = true;
          //cout << "Highpass delta = " << delta << endl;
        }
        else if ( clickedWidget == CLICKED_WIDGET_LOWPASS  )
        {
          // use X delta, not Y
          delta = clickValueX - clickX;
          invert = true;
          //cout << "Lowpass delta = " << delta << endl;
        }
        
        clickValue  = 1 - ( event->y / height); // reset the starting point, no acceleration 
        clickValueX = 1 - ( event->x / height); // reset the starting point, no acceleration 
        
        //cout << "Delta = " << click << endl;
        
        // scale delta to make it have more effect
        float value = (*target) - delta * 3;
        
        if ( invert )
        {
          value = (*target) + delta * 3;
        }
        
        if ( value > 1.f) value = 1.f;
        if ( value < 0.f) value = 0.f;
        
        if ( clickedWidget == REFRACTOR_CONTROL_RETRIGGER )
        {
          guiState->retrigger = value;
          value = int(value * 16.f);
          redraw(227,447, 40, 40);
        }
        else if ( clickedWidget == REFRACTOR_MASTER_VOLUME )
        {
          guiState->masterVol = value;
          redraw( 859, 340, 60, 190);
        }
        else if ( clickedWidget == REFRACTOR_CONTROL_BITCRUSH )
        {
          guiState->bitcrush = value;
          redraw( 406,330, 150, 100);
        }
        else if ( clickedWidget == REFRACTOR_CONTROL_DISTORTION )
        {
          guiState->distortion = value;
          redraw( 406,430, 150, 100);
        }
        else if ( clickedWidget == CLICKED_WIDGET_HIGHPASS )
        {
          guiState->highpass = value;
          redraw( 594,330, 150, 100);
        }
        else if ( clickedWidget == CLICKED_WIDGET_LOWPASS )
        {
          guiState->lowpass = value;
          redraw( 594,430, 150, 100);
        }
        else if ( clickedWidget == CLICKED_WIDGET_PITCH )
        {
          //cout << "Setting & drawing pitch / source" << endl;
          guiState->pitch = value;
          redraw( 127, 340, 70, 200);
        }
        //cout << "Writing value " << value << " scaled to " << *target << " to port index" << clickedWidget << endl;
        guiState->write_function( guiState->controller, clickedWidget, sizeof(float), 0, (const void*)&value);
      }
    }
    
};

const char* Canvas::headerNames[] = {"Source",
                                     "Retrigger",
                                     "Destroy",
                                     "Remove",
                                     "Master" };

const int Canvas::retriggerColour[]= {COLOUR_PURPLE_3,
                                      COLOUR_BLUE_1,
                                      COLOUR_GREEN_1,
                                      COLOUR_ORANGE_1,
                                      COLOUR_RECORD_RED };

#endif
