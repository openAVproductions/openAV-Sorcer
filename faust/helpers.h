
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

float print ( float x )
{
  if ( x != prevPrint)
  {
    printf( "%d\n" , x );
    prevPrint = x;
  }
  
  return x;
}

float mtof( float m )
{
  float tmp = pow( 2.0, ( float(m) - 69.0) / 12.0 );
  return tmp;
}
