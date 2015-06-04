
#include <stdio.h>
#include <cmath>

int prevPrint;

float lowClip(float lim, float sig)
{
  if ( sig < lim )
  {
    return lim;
  }
  
  return sig;
}


float highClip(float lim, float sig)
{
  if ( sig > lim )
  {
    return lim;
  }
  
  return sig;
}

float clip( float low, float high, float sig )
{
  if ( sig > high )
    sig = high;
  
  if ( sig < low )
    sig = low;
  
  return sig;
}

// this function will replace a "signal" when it is less than the "lowReplaceLimit"
// use case is an LFO when the speed is less than lowReplaceLimit, useValue instead
float replace( float lowReplaceLimit, float useValue, float sig )
{
  if ( sig < lowReplaceLimit )
  {
    return useValue;
  }
  
  return sig;
}

float print ( float x )
{
  if ( x != prevPrint)
  {
    printf( "%f\n" , x );
    prevPrint = x;
  }
  
  return x;
}

float mtof( float m )
{
  float tmp = pow( 2.0, ( float(m) - 69.0) / 12.0 );
  return tmp;
}
