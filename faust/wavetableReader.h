#include <sndfile.h>

#include <stdio.h>
#include <stdlib.h>

// include the header arrays of the samples
#include "../wavetables/wavetable_shout_0.h"
#include "../wavetables/wavetable_shout_100.h"
#include "../wavetables/wavetable_sqwak_0.h"
#include "../wavetables/wavetable_sqwak_100.h"

float* wave1 = shout_0::wavetable;
float* wave2 = shout_100::wavetable;
float* wave3 = sqwak_0::wavetable;
float* wave4 = sqwak_100::wavetable;

float wavetable1(int index)
{
  return wave1[index];
}

float wavetable2(int index)
{
  return wave2[index];
}

float wavetable3(int index)
{  
  return wave3[index];
}

float wavetable4(int index)
{
  return wave4[index];
}
