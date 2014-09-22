/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <iostream>

#include <FL/x.H>

// include the URI and global data of this plugin
#include "ports.h"

// this is our custom widget include
#include "sorcer_widget.h"

// core spec include
#include "lv2/lv2plug.in/ns/lv2core/lv2.h"

// GUI
#include "lv2/lv2plug.in/ns/extensions/ui/ui.h"

using namespace std;

typedef struct {
  SorcerUI* widget;
  
  float sidechainAmp;
  
  LV2UI_Write_Function write_function;
  LV2UI_Controller controller;
} SorcerGUI;

static LV2UI_Handle instantiate(const struct _LV2UI_Descriptor * descriptor,
                const char * plugin_uri,
                const char * bundle_path,
                LV2UI_Write_Function write_function,
                LV2UI_Controller controller,
                LV2UI_Widget * widget,
                const LV2_Feature * const * features)
{
  if (strcmp(plugin_uri, SORCER_URI) != 0) {
      fprintf(stderr, "SORCER_URI error: this GUI does not support plugin with URI %s\n", plugin_uri);
      return NULL;
  }
  
  SorcerGUI* self = (SorcerGUI*)malloc(sizeof(SorcerGUI));
  if (self == NULL) return NULL;
  
  self->controller     = controller;
  self->write_function = write_function;
  
  void* parentXwindow = 0;
  LV2UI_Resize* resize = NULL;
  
  for (int i = 0; features[i]; ++i) {
    if (!strcmp(features[i]->URI, LV2_UI__parent)) {
      parentXwindow = features[i]->data;
    } else if (!strcmp(features[i]->URI, LV2_UI__resize)) {
      resize = (LV2UI_Resize*)features[i]->data;
    }
  }
  
  // in case FLTK hasn't opened it yet
  fl_open_display();
  
  self->widget = new SorcerUI();
  
  self->widget->window->border(0);
  self->widget->subbass->X(0.5);
  
  // write functions into the widget
  self->widget->controller = controller;
  self->widget->write_function = write_function;
  
  // set host to change size of the window
  if (resize)
  {
    resize->ui_resize(resize->handle, self->widget->getWidth(), self->widget->getHeight());
  }
  else
  {
    cout << "SorcerUI: Warning, host doesn't support resize extension.\n\
    Please ask the developers of the host to support this extension. "<< endl;
  }
  
  fl_embed( self->widget->window, (Window)parentXwindow );
  
  
  return (LV2UI_Handle)self;
}



static void cleanup(LV2UI_Handle ui) {
  SorcerGUI *pluginGui = (SorcerGUI *) ui;
  delete pluginGui->widget;
  free( pluginGui);
}

static void port_event(LV2UI_Handle ui,
               uint32_t port_index,
               uint32_t buffer_size,
               uint32_t format,
               const void * buffer)
{
  SorcerGUI *self = (SorcerGUI *) ui;
  
  if ( format == 0 )
  {
    float value =  *(float *)buffer;
    switch ( port_index )
    {
      
      case ADSR_ATTACK: {
            self->widget->adsr->attack( value );
            self->widget->attack->value( value );
            } break;
      case ADSR_DECAY: {
            self->widget->adsr->decay( value );
            self->widget->decay->value( value );
            } break;
      case ADSR_SUSTAIN: {
            self->widget->adsr->sustain( value );
            self->widget->sustain->value( value );
            } break;
      case ADSR_RELEASE: {
            self->widget->adsr->release( value );
            self->widget->release->value( value );
            } break;
      
      case VOLUME: {
            self->widget->volume->value( value );
            } break;
      case OUTPUT_DB: {
            float tmp = (1-(value / -96.f));
            self->widget->volume->amplitude( tmp, tmp );
            } break;
      
      case COMPRESSOR_ATTACK: {
            self->widget->compAttack->value( value );
            } break;
      case COMPRESSOR_RELEASE: {
            self->widget->compRelease->value( value );
            } break;
      case COMPRESSOR_MAKEUP: {
            self->widget->compMakeup->value( value );
            } break;
      case COMPRESSOR_THRESHOLD: {
            self->widget->compThres->value( value );
            } break;
      case COMPRESSOR_ENABLE: {
            
            self->widget->comp->set_active( value > 0.5 ? true : false );
            } break;
      
      case FILTER_CUTOFF: {
            self->widget->filter->value( value );
            self->widget->filterCutoff->value( value );
            } break;
      case LFO_TO_FILTER_RANGE: {
            self->widget->filter->modulation( value );
            self->widget->filterLfoMod->value( value );
            } break;
            
      case LFO_TO_WAVETABLE_1: {
            self->widget->wave1->modulation( value );
            self->widget->lfoToW1->value( value );
            } break;
      case LFO_TO_WAVETABLE_2: {
            self->widget->wave2->modulation( value );
            self->widget->lfoToW2->value( value );
            } break;
      
      case LFO_AMPLITUDE: {
            self->widget->lfo->value( value );
            self->widget->lfoAmp->value( value );
            } break;
      case LFO_FREQUENCY: {
            self->widget->lfo->modulation( value );
            self->widget->lfoSpeed->value( value );
            } break;
      
      case OSC_1_VOLUME: {
            self->widget->wave1->Y( value );
            } break;
      case OSC_2_VOLUME: {
            self->widget->wave2->Y( value );
            } break;
      case OSC_3_VOLUME: {
            self->widget->subbass->Y( value );
            } break;
      
      
      case WAVETABLE_1_POS: {
            self->widget->wave1->X( value );
            } break;
      case WAVETABLE_2_POS: {
            self->widget->wave2->X( value );
            } break;
    }
  }
  
  return;
}


static int ui_show(LV2UI_Handle handle)
{
  SorcerGUI *self = (SorcerGUI *) handle;
  self->widget->window->show();
  
  return 0;
}

static int ui_hide(LV2UI_Handle handle)
{
  SorcerGUI *self = (SorcerGUI *) handle;
  self->widget->window->hide();
  
  return 0;
}

static int
idle(LV2UI_Handle handle)
{
  SorcerGUI* self = (SorcerGUI*)handle;
  
  self->widget->idle();
  
  return 0;
}

static const void*
extension_data(const char* uri)
{
  static const LV2UI_Show_Interface show = { ui_show, ui_hide };
  static const LV2UI_Idle_Interface idle_iface = { idle };
  
  if (!strcmp(uri, LV2_UI__showInterface))
  {
    return (void*)&show;
  }
  else if (!strcmp(uri, LV2_UI__idleInterface))
  {
    return (void*)&idle_iface;
  }
  return NULL;
}

static LV2UI_Descriptor descriptors[] = {
    {SORCER_UI_URI, instantiate, cleanup, port_event, extension_data}
};

const LV2UI_Descriptor * lv2ui_descriptor(uint32_t index)
{
  if (index >= sizeof(descriptors) / sizeof(descriptors[0]))
  {
      return NULL;
  }
  return descriptors + index;
}
