#! /bin/sh

cd faust

faust -cn sorcer -a lv2synth.cpp main.dsp -o main.cpp

g++ -shared -DPLUGIN_URI='"http://www.openavproductions.com/sorcer"'   main.cpp -lsndfile -o sorcer.so

g++ -DPLUGIN_URI='"http://www.openavproductions.com/sorcer"' -lsndfile main.cpp

./a.out > sorcer.ttl

cp ./sorcer.so ../sorcer.lv2/
cp ./sorcer.ttl ../sorcer.lv2/

cd ../

cp -r ./sorcer.lv2/ /usr/lib/lv2/
