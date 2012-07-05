declare name "Source";

// Compile with :
// faust -a dsp.cpp -o source_engine.cpp main.dsp

import("math.lib");
import("music.lib");
import("filter.lib");
import("oscillator.lib");


vol = hslider("vol", 0.3, 0, 10, 0.01); // %
attack = hslider("attack", 0.01, 0, 1, 0.001); // sec
decay = hslider("decay", 0.3, 0, 1, 0.001); // sec
sustain = hslider("sustain", 0.5, 0, 1, 0.01); // %
release = hslider("release", 0.2, 0, 1, 0.001); // sec
freq = nentry("freq", 440, 20, 20000, 1); // Hz
gain = nentry("gain", 0.3, 0, 10, 0.01); // %
gate = button("gate"); // 0/1

wavetable = hslider("wavetable", 0.0, 0, 1, 0.01);

// Custom wavetable read functions:
    readWave1 = ffunction(float wavetable1(int), "wavetableReader.h","-lsndfile");
    readWave2 = ffunction(float wavetable2(int), "wavetableReader.h","-lsndfile");



w1 = readWave1( ( (osc(freq)+ 1) / 2.0) * 751 );
w2 = readWave2( ( (osc(freq)+ 1) / 2.0) * 751 );

signal = w1 * wavetable + (1-wavetable) * w2;

y = signal * gate : vgroup("1-adsr", adsr(attack, decay, sustain, release) );

process = y,y;
