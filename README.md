Sorcer - OpenAV Productions
===========================

Official page: http://openavproductions.com/sorcer

This is the repository of a wavetable synthesizer called Sorcer.

![screenshot](https://raw.github.com/harryhaaren/openAV-Sorcer/master/gui/sorcer.png "Sorcer Screenshot")

Install
-------
Use make to compile and install Sorcer:
```
make
make install
```

Running
-------
After the INSTALL step Ardour3, QTractor, and any other LV2 host should
automatically find Sorcer, and add it to the list of loadable plugins.

If you have the JALV LV2 host installed, the "run.sh" script can be used to
launch Sorcer as a standalone JACK client.

$  ./run.sh


Presets
-------
Presets are included in this repo, and copied to the 


Bug Reports
-----------
Please use the issue tracker on github to report bugs:
https://github.com/harryhaaren/openAV-Sorcer/issues/new


Details
-------
I've removed the dependency on FAUST to install: the generated CPP file is
included in the repo now. To tweak the FAUST code, just edit faust/main.dsp.
Running "make faustGenerateCpp" to re-generate the CPP code from faust.
The LV2-FAUST libraries are necessary, ensure they're installed.


Contact
-------
If you have a particular question, email me!
```
harryhaaren@gmail.com
```

Cheers, -Harry
