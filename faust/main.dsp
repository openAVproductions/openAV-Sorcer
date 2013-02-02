declare name "Sorcer";

import("math.lib");
import("music.lib");
import("filter.lib");
import("oscillator.lib");


vol = hslider("vol", 0.3, 0, 10, 0.01); // %
attack = hslider("attack", 0.01, 0.01, 1, 0.001); // sec
decay = hslider("decay", 0.3, 0, 1, 0.001); // sec
sustain = hslider("sustain", 1.0, 0, 1, 0.01); // %
release = hslider("release", 0.2, 0, 1, 0.001); // sec
freq = nentry("freq", 20, 20, 20000, 1) / 2.0; // Hz
gain = nentry("gain", 0.3, 0, 10, 0.01); // %
gate = button("gate"); // 0/1

wavetable1pos = hslider("wavetable1pos", 0.0, 0, 1, 0.01);
wavetable2pos = hslider("wavetable2pos", 0.0, 0, 1, 0.01);

// Custom wavetable read functions:
    osc1readWave1 = ffunction(float wavetable1(int), "wavetableReader.h","-lsndfile");
    osc1readWave2 = ffunction(float wavetable2(int), "wavetableReader.h","-lsndfile");
    osc2readWave1 = ffunction(float wavetable3(int), "wavetableReader.h","-lsndfile");
    osc2readWave2 = ffunction(float wavetable4(int), "wavetableReader.h","-lsndfile");


// Custom clipping functions
    clip     = ffunction( float clip (float , float , float ), "helpers.h","");
    lowClip  = ffunction( float lowClip (float , float  ), "helpers.h","");
    highClip = ffunction( float highClip(float , float  ), "helpers.h","");


// Routing variables
    lfo1_wavetable1pos = hslider("lfo1_wavetable1pos", 0.0, 0, 1, 0.01);
    lfo1_wavetable2pos = hslider("lfo1_wavetable2pos", 0.0, 0, 1, 0.01);

// LFO 1
    lfo1freq = hslider("lfo1freq [midi:ctrl 1]", 3, 0.0, 10.0, 0.01);
    
    // concider using a different value when speed < 0.1 or so, as otherwise
    // the output value depends on the current phase of the LFO
    
    lfo1amp  = hslider("lfo1amp" , 0.1, 0,  1, 0.001);
    lfo1output = osc( lfo1freq ) * lfo1amp;

// OSC 1
    wavetable1final = clip( 0.0, 1.0, wavetable1pos +  lfo1_wavetable1pos * lfo1output );
    
    osc1w1 = osc1readWave1( ( (osc(freq)+ 1) / 2.0) * 751 );
    osc1w2 = osc1readWave2( ( (osc(freq)+ 1) / 2.0) * 751 );
    osc1wsum = osc1w1 * (1-wavetable1final) + wavetable1final * osc1w2;
    
    osc1vol    = hslider("osc1vol", 0.3, 0, 1, 0.001);
    osc1octave = hslider("osc1octave", 0, -4, 0, 1);
    osc1output = osc1wsum * osc1vol;

// OSC 2
    wavetable2final = clip( 0.0, 1.0, wavetable2pos +  lfo1_wavetable2pos * lfo1output );
    
    osc2w1 = osc2readWave2( ( (osc(freq)+ 1) / 2.0) * 751 );
    osc2w2 = osc2readWave1( ( (osc(freq)+ 1) / 2.0) * 751 );
    osc2wsum = osc2w1 * (1-wavetable2final) + wavetable2final * osc2w2;
    
    osc2vol    = hslider("osc2vol", 0.3, 0, 1, 0.001);
    osc2octave = hslider("osc2octave", 0, -4, 0, 1);
    osc2output = osc2wsum * osc2vol;

// OSC 3
    osc3vol    = hslider("osc3vol", 0.3, 0, 1, 0.001);
    osc3octave = hslider("osc3octave", 0, -4, 0, 1);
    osc3output = osc(freq) * osc3vol; // +freq*(1/osc3octave)


// Oscillator Accumulator
    oscOutputsignal =   osc1output + osc2output + osc3output;

// Filter1
    filter1lfo1range = hslider("filter1lfo1range", 600, 0, 6000, 1);
    filter1freqSmooth =  lowClip( 10, lfo1output *filter1lfo1range ) + hslider("filter1cutoff", 18000, 80, 18000, 0.1)  : smooth(tau2pole( 0.05 ));
    filterOutputSignal = oscOutputsignal : lowpass( 4 , filter1freqSmooth );

//y = signal * gate : vgroup("1-adsr", adsr(attack, decay, sustain, release) );

finalSignal = filterOutputSignal;

process = finalSignal
  * (gate : vgroup("1-adsr", adsr(attack, decay, sustain, release)))
  * gain : vgroup("2-master", *(vol));
