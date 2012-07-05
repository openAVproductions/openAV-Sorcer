#! /bin/sh

cd faust

faust -cn source -a lv2synth.cpp main.dsp -o main.cpp

g++ -shared -DPLUGIN_URI='"http://www.openavproductions.com/source"'   main.cpp -lsndfile -o source.so

g++ -DPLUGIN_URI='"http://www.openavproductions.com/source"' -lsndfile main.cpp

./a.out > source.ttl

cp ./source.so ../source.lv2/
cp ./source.ttl ../source.lv2/

cd ../

cp -r ./source.lv2/ /usr/lib/lv2/
