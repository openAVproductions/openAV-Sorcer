#! /bin/sh

# this script will compile the sorcer gui, copy it to /usr/lib and test it in jalv.gtk
./waf

cp build/libsorcer_gui.so /usr/lib/lv2/sorcer.lv2/sorcer_gui.so

../run.sh

