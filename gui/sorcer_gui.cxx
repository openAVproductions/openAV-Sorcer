/*
  Sorcer : OpenAV Productions
    harryhaaren@gmail.com
  
  Sorcer by OpenAV productions is free software: you can redistribute it
  and/or modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include <iostream>

#include <gtkmm.h>

#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

// core spec include
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

// LV2 extension headers
#include "lv2/lv2plug.in/ns/ext/log/log.h"
#include "lv2/lv2plug.in/ns/ext/urid/urid.h"

#include "lv2/lv2plug.in/ns/ext/atom/atom.h"
#include "lv2/lv2plug.in/ns/ext/atom/util.h"

#define LV2_MIDI__MidiEvent "http://lv2plug.in/ns/ext/midi#MidiEvent"

// GUI
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

#include "canvas.hxx"

#define SORCER_URI "http://www.openavproductions.com/sorcer"
#define SORCER_UI_URI "http://www.openavproductions.com/sorcer/gui"

using namespace std;

typedef struct {
    Canvas* widget;
} SorcerGui;

static GtkWidget* make_gui(SorcerGui *self) {
    
    //cout << "Init GTKMM!" << endl;
    Gtk::Main* main = Gtk::Main::instance(); // _gtkmm_internals // for QT hosts
    
    main->init_gtkmm_internals();
    
    if( !Glib::thread_supported() )
    {
      //cout << "calling Glib::thread_init()" << endl;
      Glib::thread_init();
    }
    
    //cout << "create container!" << endl;
    // Return a pointer to a gtk widget containing our GUI
    GtkWidget* container = gtk_vbox_new(FALSE, 2);
    
    //cout << "new canvast!" << endl;
    self->widget = new Canvas();
    
    //cout << "adding to window!" << endl;
    // get the gobject, and add that to the container
    gtk_container_add((GtkContainer*)container, (GtkWidget*)self->widget->gobj() );
    
    return container;
}


static LV2UI_Handle instantiate(const struct _LV2UI_Descriptor * descriptor,
                const char * plugin_uri,
                const char * bundle_path,
                LV2UI_Write_Function write_function,
                LV2UI_Controller controller,
                LV2UI_Widget * widget,
                const LV2_Feature * const * features) {

    if (strcmp(plugin_uri, SORCER_URI) != 0) {
        fprintf(stderr, "SORCER_URI error: this GUI does not support plugin with URI %s\n", plugin_uri);
        return NULL;
    }
    
    SorcerGui* self = (SorcerGui*)malloc(sizeof(SorcerGui));
    
    //cout << "Allocated SourceGUI!" << endl;
    
    if (self == NULL) return NULL;
    
    for(int i = 0; features[i]; i++)
    {
      if (!strcmp(features[i]->URI, LV2_URID__map))
      {
        //cout << "Found feature URID map!" << endl;
        //self->guiState->map = (LV2_URID_Map*)features[i]->data;
      }
    }
    
    //cout << "Creating UI!" << endl;
    *widget = (LV2UI_Widget)make_gui(self);
    
    //cout << "Writing controller f(x)!" << endl;
    
    self->widget->controller = controller;
    self->widget->write_function = write_function;
    
    //cout << "returning..." << endl;
    
    return (LV2UI_Handle)self;
}

static void cleanup(LV2UI_Handle ui) {
    //printf("cleanup()\n");
    SorcerGui *pluginGui = (SorcerGui *) ui;
    free(pluginGui);
}

static void port_event(LV2UI_Handle ui,
               uint32_t port_index,
               uint32_t buffer_size,
               uint32_t format,
               const void * buffer)
{
    SorcerGui *self = (SorcerGui *) ui;
    
    //cout << "Port event on index " << port_index << "  Format is " << format << endl;
    
    if ( format == 0 )
    {
      float tmp = *(float*)buffer;
      //cout << "Port " << port_index << " gets " << tmp << endl;
      self->widget->values[port_index] = tmp;
      self->widget->redrawUpdate();
    }
    
    return;
}

static LV2UI_Descriptor descriptors[] = {
    {SORCER_UI_URI, instantiate, cleanup, port_event, NULL}
};

const LV2UI_Descriptor * lv2ui_descriptor(uint32_t index) {
    //printf("lv2ui_descriptor(%u) called\n", (unsigned int)index); 
    if (index >= sizeof(descriptors) / sizeof(descriptors[0])) {
        return NULL;
    }
    return descriptors + index;
}
