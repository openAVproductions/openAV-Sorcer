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

declare name "Sorcer";
declare author "OpenAV Productions";
declare description "Wavetable Synth";

import("math.lib");
import("music.lib");
import("filter.lib");
import("oscillator.lib");


vol = hslider("vol", 0.3, 0, 1, 0.01); // %
attack = 0.01 + hslider("attack", 0.01, 0.01, 1, 0.001); // sec
decay = hslider("decay", 0.3, 0, 1, 0.001) * 0.8 + 0.2; // sec
sustain = hslider("sustain", 1.0, 0, 1, 0.01) + 0.1; // %
release = hslider("release", 0.2, 0, 1, 0.001) + 0.1; // sec

freq = nentry("freq", 20, 20, 20000, 1) / 2.0; // Hz
gain = nentry("gain", 0.3, 0, 1, 0.01); // %
gate = button("gate"); // 0/1

wavetable1pos = hslider("wavetable1pos", 0.0, 0, 1, 0.01);
wavetable2pos = hslider("wavetable2pos", 0.0, 0, 1, 0.01);

// Custom wavetable read functions:
    osc1readWave1 = ffunction(float wavetable1(int), "wavetableReader.h","");
    osc1readWave2 = ffunction(float wavetable2(int), "wavetableReader.h","");
    osc2readWave1 = ffunction(float wavetable3(int), "wavetableReader.h","");
    osc2readWave2 = ffunction(float wavetable4(int), "wavetableReader.h","");


// Custom clipping functions
    clip     = ffunction( float clip (float , float , float ), "helpers.h","");
    lowClip  = ffunction( float lowClip (float , float  ), "helpers.h","");
    highClip = ffunction( float highClip(float , float  ), "helpers.h","");


// Routing variables
    lfo1_wavetable1pos = hslider("lfo1_wavetable1pos", 0.0, 0, 1, 0.01);
    lfo1_wavetable2pos = hslider("lfo1_wavetable2pos", 0.0, 0, 1, 0.01);

// LFO 1
    lfo1freqZeroOne = hslider("lfo1freq", 0.3, 0.0, 1.0, 0.001);
    
    lfo1freq = lfo1freqZeroOne * 10;
    
    // concider using a different value when speed < 0.1 or so, as otherwise
    // the output value depends on the current phase of the LFO
    
    lfo1amp  = hslider("lfo1amp" , 0.1, 0,  1, 0.001);
    lfo1output = osc( lfo1freq ) * (lfo1amp-0.01);

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
    filter1lfo1rangeZeroOne = hslider("filter1lfo1range", 0, 0, 1, 0.0001);
    filter1lfo1rangePreClip = ( pow( (filter1lfo1rangeZeroOne*4) + 1, 4) - 1 );
    filter1lfo1range = clip( 0, 6000, filter1lfo1rangePreClip );
    
    // lin->log frequncy, pow( input, 4) 
    filter1cutoffZeroOne = hslider("filter1cutoff", 1, 0, 1, 0.001) : smooth(tau2pole( 0.06 ));
    filter1cutoff = clip ( 80, 18000, ( pow( (filter1cutoffZeroOne*0.5)+0.3 ,4) * 18000) );
    
    filter1freqSmooth =  clip( 80, 16000, (lfo1output *filter1lfo1range ) + filter1cutoff  );
    filterOutputSignal = oscOutputsignal : lowpass( 4 , filter1freqSmooth );

//y = signal * gate : vgroup("1-adsr", adsr(attack, decay, sustain, release) );

finalSignal = filterOutputSignal;

process = finalSignal
  * (gate : vgroup("1-adsr", adsr(attack, decay, sustain, release)))
  * gain : vgroup("2-master", *(vol));
