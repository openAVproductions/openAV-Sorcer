
#ifndef SORCER_PORTS_H
#define SORCER_PORTS_H

enum SORCER_PORTS
{
  ADSR_ATTACK = 0,
  ADSR_DECAY,
  ADSR_RELEASE,
  ADSR_SUSTAIN,
  
  VOLUME,
  
  FILTER_CUTOFF,
  
  LFO_TO_FILTER_RANGE,
  
  LFO_TO_WAVETABLE_1,
  LFO_TO_WAVETABLE_2,
  
  LFO_AMPLITUDE,
  LFO_FREQUENCY,
  
  OSC_1_VOLUME,
  OSC_2_VOLUME,
  OSC_3_VOLUME,
  
  OUTPUT_DB,
  
  WAVETABLE_1_POS,
  WAVETABLE_2_POS,
  
  AUDIO_OUT,
  
  // Ignored by UI
  EVENT_IN,
  POLYPHONY,
};

#define SORCER_URI "http://www.openavproductions.com/sorcer"
#define SORCER_UI_URI "http://www.openavproductions.com/sorcer/gui"

#endif
