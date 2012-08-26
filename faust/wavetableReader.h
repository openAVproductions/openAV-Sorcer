#include <sndfile.h>

#include <stdio.h>
#include <stdlib.h>

bool wave1loaded = false;
bool wave2loaded = false;
bool wave3loaded = false;
bool wave4loaded = false;

float wave1[752];
float wave2[752];
float wave3[752];
float wave4[752];

float wavetableRead(const char* fileName, float* buffer)
{
  for (int i = 0; i < 752; i++)
    buffer[i] = 0.f;
  
  SNDFILE *infile = 0;
  SF_INFO sfinfo;
  
  infile = sf_open (fileName, SFM_READ, &sfinfo);
  
  if ( infile == 0 )
  {
    printf ("Not able to open input file\n");
    return 1;
  }
  
  sf_readf_float (infile, buffer, 752);
  
  sf_close (infile);
  
  printf ("Successfully loaded %s\n", fileName);
  
}

float wavetable1(int index)
{
  if ( wave1loaded == false )
  {
     wavetableRead("/home/openav/sorcer/wavetables/shout_0.wav", &wave1[0] );
     wave1loaded = true;
  }
  
  float output = wave1[index];
  
  return output;
}

float wavetable2(int index)
{
  if ( wave2loaded == false )
  {
     wavetableRead("/home/openav/sorcer/wavetables/shout_100.wav", &wave2[0] );
     wave2loaded = true;
  }
  
  float output = wave2[index];
  
  return output;
}

float wavetable3(int index)
{
  if ( wave3loaded == false )
  {
     wavetableRead("/home/openav/sorcer/wavetables/sqwak_0.wav", &wave3[0] );
     wave3loaded = true;
  }
  
  float output = wave3[index];
  
  return output;
}

float wavetable4(int index)
{
  if ( wave4loaded == false )
  {
     wavetableRead("/home/openav/sorcer/wavetables/sqwak_100.wav", &wave4[0] );
     wave4loaded = true;
  }
  
  float output = wave4[index];
  
  return output;
}
