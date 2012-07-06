

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

#define SOURCE_URI "http://www.openavproductions.com/source"
#define SOURCE_UI_URI "http://www.openavproductions.com/source/gui"

using namespace std;

typedef struct {
    Canvas* widget;
} SourceGui;

static GtkWidget* make_gui(SourceGui *self) {
    
    cout << "Init GTKMM!" << endl;
    Gtk::Main::init_gtkmm_internals(); // for QT hosts
    
    
    cout << "create container!" << endl;
    // Return a pointer to a gtk widget containing our GUI
    GtkWidget* container = gtk_vbox_new(FALSE, 2);
    
    cout << "new canvast!" << endl;
    self->widget = new Canvas();
    
    cout << "adding to window!" << endl;
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

    if (strcmp(plugin_uri, SOURCE_URI) != 0) {
        fprintf(stderr, "SOURCE_URI error: this GUI does not support plugin with URI %s\n", plugin_uri);
        return NULL;
    }
    
    SourceGui* self = (SourceGui*)malloc(sizeof(SourceGui));
    
    cout << "Allocated SourceGUI!" << endl;
    
    if (self == NULL) return NULL;
    
    for(int i = 0; features[i]; i++)
    {
      if (!strcmp(features[i]->URI, LV2_URID__map))
      {
        cout << "Found feature URID map!" << endl;
        //self->guiState->map = (LV2_URID_Map*)features[i]->data;
      }
    }
    
    //self->guiState->uris.midiEvent    = self->guiState->map->map(self->guiState->map->handle, LV2_MIDI__MidiEvent);
    
    cout << "Creating UI!" << endl;
    *widget = (LV2UI_Widget)make_gui(self);
    
    cout << "Writing controller f(x)!" << endl;
    
    self->widget->controller = controller;
    self->widget->write_function = write_function;
    
    cout << "returning..." << endl;
    
    return (LV2UI_Handle)self;
}

static void cleanup(LV2UI_Handle ui) {
    printf("cleanup()\n");
    SourceGui *pluginGui = (SourceGui *) ui;
    free(pluginGui);
}

static void port_event(LV2UI_Handle ui,
               uint32_t port_index,
               uint32_t buffer_size,
               uint32_t format,
               const void * buffer)
{
    SourceGui *self = (SourceGui *) ui;
    
    cout << "Port event on index " << port_index << "  Format is " << format << endl;
    
    /*
    if ( format == 0 )
    {
      float value =  *(float *)buffer;
      switch ( port_index )
      {
        case REFRACTOR_CONTROL_RETRIGGER:
            cout << "Refractor: Retrigger control event, value = " << value << endl;
            //self->guiState->retrigger = value;
            //self->widget->redraw();
            break;
        case REFRACTOR_MASTER_VOLUME:
            cout << "Refractor: Master volume event, value = " << value << endl;
            self->guiState->masterVol = value;
            self->widget->redraw();
      }
    }
    else
    {
      LV2_ATOM_SEQUENCE_FOREACH( (LV2_Atom_Sequence*)buffer, ev)
      {
        //self->frame_offset = ev->time.frames;
        
        if (ev->body.type == self->guiState->uris.midiEvent)
        {
          cout << "Refractor GUI got MIDI event!" << endl;
          //uint8_t* const data = (uint8_t* const)(ev + 1);
        }
      }
    }
    */
    
    return;
}

static LV2UI_Descriptor descriptors[] = {
    {SOURCE_UI_URI, instantiate, cleanup, port_event, NULL}
};

const LV2UI_Descriptor * lv2ui_descriptor(uint32_t index) {
    printf("lv2ui_descriptor(%u) called\n", (unsigned int)index); 
    if (index >= sizeof(descriptors) / sizeof(descriptors[0])) {
        return NULL;
    }
    return descriptors + index;
}
