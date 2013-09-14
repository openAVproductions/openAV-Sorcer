
CC=g++
CFLAGS=-g -Wall

INCLUDES=$(shell pkg-config --cflags sndfile cairomm-1.0 ntk ntk_images)
LDFLAGS=$(shell pkg-config --libs sndfile cairomm-1.0 ntk ntk_images) -fPIC -shared -Wl,-z,nodelete

UISOURCES=gui/sorcer_widget.cxx gui/sorcer_ui.cxx
UIOBJECTS=$(UISOURCES:.cpp=.o)
UI=sorcer.lv2/sorcer_ui.so

#SASOURCES=gui/sorcer.cxx gui/ui_helpers.cxx gui/ui_test.cxx
#SAOBJECTS=$(SASOURCES:.cpp=.o)
#SA=uiTest

#DSPSOURCES=dsp/sorcer.c
#DSPOBJECTS=$(DSPSOURCES:.cpp=.o)
#DSP=sorcer.lv2/sorcer.so

all: folder ui

#genUI:
#	cd gui; ntk-fluid -c sorcer.fl

folder:
	mkdir -p sorcer.lv2/
	

install:
	#cp dsp/sorcer.ttl sorcer.lv2/
	cp -r sorcer.lv2 ~/.lv2/
	#cp -r presets/*.lv2 ~/.lv2/

ui: $(UISOURCES) $(UI)
#dsp: $(DSPSOURCES) $(DSP)

# for testing only: builds a standalone NTK app
#sa: $(SASOURCES) $(SA)

#$(DSP): $(DSPOBJECTS)
#	$(CC) $(DSPOBJECTS) $(INCLUDES) $(CFLAGS) $(LDFLAGS) -o $@

$(UI): $(UIOBJECTS)
	$(CC) $(UIOBJECTS)  $(INCLUDES) $(CFLAGS) $(LDFLAGS) -o $@

#$(SA): $(SAOBJECTS)
#	$(CC) $(SAOBJECTS) $(INCLUDES) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $< -g $(CFLAGS) -c -o $@


clean:
#	rm -f uiTest
	rm -f sorcer.lv2/sorcer.so
	rm -f sorcer.lv2/sorcer_ui.so
