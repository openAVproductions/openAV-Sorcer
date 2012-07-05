#include <sndfile.h>

#include <stdio.h>
#include <stdlib.h>

bool wave1loaded = false;
bool wave2loaded = false;

float wave1[752];
float wave2[752];

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
  
}

float wavetable1(int index)
{
  if ( wave1loaded == false )
  {
     wavetableRead("/root/shout_0.wav", &wave1[0] );
     wave1loaded = true;
  }
  
  float output = wave1[index];
  
  return output;
}

float wavetable2(int index)
{
  if ( wave2loaded == false )
  {
     wavetableRead("/root/shout_100.wav", &wave2[0] );
     wave2loaded = true;
  }
  
  float output = wave2[index];
  
  return output;
}
