#! /bin/sh

cd faust

faust -cn source -a jack-gtk.cpp main.dsp -o main.cpp

g++ '-I/usr/lib/faust/' -DPLUGIN_URI='"http://www.openavproductions.com/sorcer"'   main.cpp `pkg-config --cflags --libs jack gtk+-2.0` -lsndfile -o sorcer

./sorcer
