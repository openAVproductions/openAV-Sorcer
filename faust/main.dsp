declare name "Sorcer";

import("math.lib");
import("music.lib");
import("filter.lib");
import("oscillator.lib");


vol = hslider("vol", 0.3, 0, 10, 0.01); // %
attack = hslider("attack", 0.08, 0, 1, 0.001); // sec
decay = hslider("decay", 0.3, 0, 1, 0.001); // sec
sustain = hslider("sustain", 0.8, 0, 1, 0.01); // %
release = hslider("release", 0.2, 0, 1, 0.001); // sec
freq = nentry("freq", 80, 20, 20000, 1) / 2.0; // Hz
gain = nentry("gain", 0.3, 0, 10, 0.01); // %
gate = button("gate"); // 0/1

wavetable = hslider("wavetable", 0.0, 0, 1, 0.01);

// Custom wavetable read functions:
    osc1readWave1 = ffunction(float wavetable1(int), "wavetableReader.h","-lsndfile");
    osc1readWave2 = ffunction(float wavetable2(int), "wavetableReader.h","-lsndfile");
    

// Custom clipping functions
    lowClip  = ffunction( float lowClip (float, float), "helpers.h","");
    highClip = ffunction( float highClip(float, float), "helpers.h","");


// LFO 1
    lfo1freq = hslider("lfo1freq", 3.0, 0, 10, 0.01);
    lfo1amp  = hslider("lfo1amp" , 0.1, 0,  1, 0.001);
    lfo1output = osc( lfo1freq ) * lfo1amp;

// OSC 1
    osc1w1 = osc1readWave1( ( (osc(freq)+ 1) / 2.0) * 751 );
    osc1w2 = osc1readWave2( ( (osc(freq)+ 1) / 2.0) * 751 );
    osc1output = osc1w1 * wavetable + (1-wavetable) * osc1w2;

// OSC 3
    osc3vol    = hslider("osc3vol", 0.3, 0, 1, 0.001);
    osc3octave = hslider("osc3octave", 0, -4, 0, 1);
    osc3output = osc(freq) * osc3vol; // +freq*(1/osc3octave)


// Oscillator Accumulator
    oscOutputsignal = osc1output + osc3output;

// Filter1
    filter1lfo1range = hslider("filter1lfo1range", 300, 0, 6000, 1);
    filter1freqSmooth =  lowClip( 0, lfo1output *filter1lfo1range ) + hslider("filter1cutoff", 2000, 80, 20000, 0.1)  : smooth(tau2pole( 0.05 ));
    filterOutputSignal = oscOutputsignal : lowpass( 4 , filter1freqSmooth );

//y = signal * gate : vgroup("1-adsr", adsr(attack, decay, sustain, release) );

finalSignal = filterOutputSignal;

process = finalSignal
  * (gate : vgroup("1-adsr", adsr(attack, decay, sustain, release)))
  * gain : vgroup("2-master", *(vol));
