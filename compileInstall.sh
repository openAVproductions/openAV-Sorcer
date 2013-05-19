#! /bin/bash

echo -e "\e[34m:: \e[97mBuilding GUI"

cd gui

./waf configure > /dev/null 2>&1
./waf > /dev/null 2>&1
echo -e "\e[1A\e[73C \e[34m[\e[97mDONE\e[34m]"



echo -e "\e[34m:: \e[97mCopying GUI to sorcer.lv2..."
cp build/libsorcer_gui.so ../sorcer.lv2/sorcer_gui.so
cp ../planning/header.png ../sorcer.lv2/header.png
echo -e "\e[1A\e[73C \e[34m[\e[97mDONE\e[34m]"


cd ../faust
# Uncomment to build FAUST->CPP: not needed as repo contains release CPP file
#echo -e "\e[34m:: \e[97mBuilding FAUST dsp..."
#faust -cn sorcer -a lv2synth.cpp main.dsp -o main.cpp
#echo -e "\e[1A\e[73C \e[34m[\e[97mDONE\e[34m]"



echo -e "\e[34m:: \e[97mBuilding plugin..."
g++ -fPIC -shared -ffast-math -O3 -DPLUGIN_URI='"http://www.openavproductions.com/sorcer"'   main.cpp -o sorcer.so
echo -e "\e[1A\e[73C \e[34m[\e[97mDONE\e[34m]"


echo -e "\e[34m:: \e[97mRetrieving plugin metadata..."
g++ -DPLUGIN_URI='"http://www.openavproductions.com/sorcer"' main.cpp
./a.out > sorcer.ttl
echo -e "\e[1A\e[73C \e[34m[\e[97mDONE\e[34m]"


echo -e "\e[34m:: \e[97mCopying .ttl and .so..."
cp ./sorcer.so ../sorcer.lv2/
cp ./sorcer.ttl ../sorcer.lv2/
echo -e "\e[1A\e[73C \e[34m[\e[97mDONE\e[34m]"


echo -e "\e[34m:: \e[97mPatching plugin UI RDF..."
cd ../sorcer.lv2
patch -u sorcer.ttl addUiRdf.patch > /dev/null 2>&1
cd ..
echo -e "\e[1A\e[73C \e[34m[\e[97mDONE\e[34m]"


if [ $(whoami) = root ]
then
  echo -e "\e[34m:: \e[97mCopying bundle to /usr/lib/lv2/sorcer.lv2"
  install -d /usr/lib/lv2/sorcer.lv2
  install -t /usr/lib/lv2/sorcer.lv2 ./sorcer.lv2/*
else
  echo -e "\e[34m:: \e[97mCopying bundle to $HOME/.lv2/sorcer.lv2"
  install -d $HOME/.lv2/sorcer.lv2
  install -t $HOME/.lv2/sorcer.lv2 ./sorcer.lv2/*
fi
echo -e "\e[1A\e[73C \e[34m[\e[97mDONE\e[34m] \e[97m"
