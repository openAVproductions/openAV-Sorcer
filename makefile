
CC=g++
CFLAGS=-g -Wall

INCLUDES=$(shell pkg-config --cflags sndfile cairomm-1.0 ntk ntk_images)
LDFLAGS=$(shell pkg-config --libs sndfile cairomm-1.0 ntk ntk_images) -fPIC -shared -Wl,-z,nodelete

UISOURCES=gui/sorcer_widget.cxx gui/sorcer_ui.cxx
UIOBJECTS=$(UISOURCES:.cpp=.o)
UI=sorcer.lv2/sorcer_ui.so



all: folder ui dsp

# *NOT* needed for building in general: the generated main.cpp file is included
# in the repository for convinience and to remove the dependency on FAUST.
faustGenerateCpp:
	cd faust/ && faust -cn sorcer -a lv2synth.cpp main.dsp -o main.cpp

folder:
	mkdir -p sorcer.lv2/

install:
	#cp -r sorcer.lv2 ~/.lv2/
	install -d $(HOME)/.lv2/sorcer.lv2
	install -t $(HOME)/.lv2/sorcer.lv2 ./sorcer.lv2/*


ui: $(UISOURCES) $(UI)

# use manual command to build
dsp:
	cd faust/ && g++ -fPIC -shared -ffast-math -O3 -DPLUGIN_URI='"http://www.openavproductions.com/sorcer"'   main.cpp -o sorcer.so
	cd faust/ && g++ -DPLUGIN_URI='"http://www.openavproductions.com/sorcer"' main.cpp
	cd faust/ && ./a.out > sorcer.ttl
	cd faust/ && patch -u sorcer.ttl addUiRdf.patch
	cp faust/sorcer.so 	sorcer.lv2/
	cp faust/sorcer.ttl sorcer.lv2/

$(UI): $(UIOBJECTS)
	$(CC) $(UIOBJECTS)  $(INCLUDES) $(CFLAGS) $(LDFLAGS) -o $@


.cpp.o:
	$(CC) $< -g $(CFLAGS) -c -o $@


clean:
#	rm -f uiTest
	rm -f sorcer.lv2/sorcer.so
	rm -f sorcer.lv2/sorcer_ui.so
