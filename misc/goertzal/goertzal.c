/*
  goertzal.c
  David Rowe & Matthew Cook
  21 June 2014

  Step by Step port of Goertzal algorithm to fixed point, and tone
  decoder automated testing.

  ref:
    http://en.wikipedia.org/wiki/Goertzel_algorithm

  usage:
    $ gcc goertzal.c -o goertzal -Wall -lm
    $ ./goertzal
*/

#include <assert.h>
#include <stdio.h>
#include <math.h>

#define FS   8000
#define F    1000
#define N     768
#define AMP   512

/* Vanilla float version.  Note optional printfs to dump state 
   variables for plotting */

float goertzal(short x[], int nmax, float coeff) {
    float s, power;
    float sprev, sprev2;
    int   n;

    sprev = 0;
    sprev2 = 0;
    for(n=0; n<nmax; n++) {
        s = x[n] + coeff * sprev - sprev2;
        sprev2 = sprev;
        sprev = s;
        //printf("%f\n", s);
    }

    power = sprev2*sprev2 + sprev*sprev - coeff*sprev*sprev2;

    return power;
}

/* Just coeff in fixed point, lets us test if this has the right scaling. */

float goertzal1(short x[], int nmax, float coeff) {
    float s, power;
    short coeff_fix;
    float sprev, sprev2;
    int   n;

    coeff_fix = (1<<14)*coeff;

    sprev = 0;
    sprev2 = 0;
    for(n=0; n<N; n++) {
        s = x[n] + (coeff_fix * sprev)/(1<<14) - sprev2;
        sprev2 = sprev;
        sprev = s;       
    }

    power = sprev2*sprev2 + sprev*sprev - coeff*sprev*sprev2;

    return power;
}

/* main loop in fixed point */

float goertzal2(short x[], int nmax, float coeff) {
    float power, sprev, sprev2;
    short coeff_q14;
    short z, zprev, zprev2;
    int   mult;
    int   n;

    coeff_q14 = (1<<14)*coeff; /* this could be a pre-computed constant */

    /*
         s = x[n] + (coeff * sprev) - sprev2
      s(n) = x[n] + (coeff * s[n-1]) - s[n-2]
      
      s has a maximum value of about 2^21, so allowing for the sign
      bit a suitable format for 32 bit numbers would be Q22.10, that
      gives us 21 mag bits to left of the decimal and 10 to the right.
      
      So lets multiply both sides of by 2^10:

      2^10(s[n]) = 2^10(x[n] + (coeff * s[n-1]) - s[n-2])         
      2^10(s[n]) = 2^10(x[n]) + 2^10((coeff * s[n-1]) - 2^10(s[n-2])

      However this means we need 32 bit storage: s, sprev, and sprev2
      would all be 32 bits which also means the (coeff * sprev) multiply
      would be 16 by 32 bits.  Yuck.  

      So lets convert from 32 bits to 16 bits. This is like stripping
      off the least sig 16 bits, or dividing by 2^16, which is converting
      from Q22.10 to Q6.10.  We move to C bit shift notation below as it's
      clearer in plain text.

              2^10(s[n]) = 2^10(x[n] + (coeff * s[n-1]) - s[n-2])         
                s[n]<<10 =  (x[n] + (coeff * s[n-1]) - s[n-2])<<10         
             (s<<10)>>16 = ((x[n] + (coeff * s[n-1]) - s[n-2])<<10)>>16
                 s[n]>>6 =  (x[n] + (coeff * s[n-1]) - s[n-2])>>6
                 s[n]>>6 =   x[n]>>6 + (coeff * (s[n-1]>>6)) - s[n-2]>>6

      Note s>>6, s[n-1]>>6, s[n-2]>>6 must all be in the same format,
      as they are just time delayed versions of each other.  In
      particular the middle term is expressed as: 

      (coeff * s[n-1])>>6 = (coeff * (s>[n-1]>.6)) to keep s[n-1]>>6 in
      the same format as s[n] and s[n-2].  This is subtle, but important.

      A very neat trick is to use a change of variables:

      z[n] = s[n]>>6
      
      Now we have:
      
      z[n] = x[n]>>6 + (coeff * z[n-1]) - z[n-2]

      Now coeff is stored in Q2.14, so:

      z[n] = x[n]>>6 + ((coeff<<14) * z[n-1])>>14 - z[n-2]
      z[n] = x[n]>>6 +  (coeff_q14 * z[n-1]) >>14 - z[n-2]
     
      Note we need the >>14 to balance out the scaling of coeff.  We
      multiply two 16 bit numbers to get a 32 bit result.  This can be
      scaled back down to 16 bits before being added to the
      other terms.  Numbers being added must all have the same format
      (same scaling).

      Finally, we can recover s[n]:

      sn = z[n]<<6
    */

    zprev = 0;
    zprev2 = 0;
    for(n=0; n<nmax; n++) {
        mult = (int)coeff_q14 * (int)zprev;
        z = (x[n]>>6) + (int)(mult>>14) - zprev2;
        zprev2 = zprev;
        zprev = z;       
    }

    sprev   = (float)zprev*pow(2,6);
    sprev2  = (float)zprev2*pow(2,6);
    
    power = sprev2*sprev2 + sprev*sprev - coeff*sprev*sprev2;

    return power;
}

/* lastly, final power calculation in fixed point */

float goertzal3(short x[], int nmax, float coeff) {
    short coeff_q14;
    short z, zprev, zprev2;
    int   mult, pz;
    int   n;

    coeff_q14 = (1<<14)*coeff;

    zprev = 0;
    zprev2 = 0;
    for(n=0; n<nmax; n++) {
        mult = (int)coeff_q14 * (int)zprev;
        z = (x[n]>>6) + (mult>>14) - zprev2;
        zprev2 = zprev;
        zprev = z;       
    }

    /* 
       Lastly, lets convert the power term to fixed point:

       pz = z[n-2]*z[n-2] + z[n-1]*z[n-1] + coeff*z[n-1]*z[n-2]
       pz = z[n-2]*z[n-2] + z[n-1]*z[n-1] + (coeff<<14)*z[n-1]*z[n-2]>>14

       Given z[n] = s[n]>>6, we get:

       pz      = (s[n-2]*s[n-2] + s[n-1]*s[n-1] + coeff*s[n-1]*s[n-2]) >> 12
       pz<<12  =  s[n-2]*s[n-2] + s[n-1]*s[n-1] + coeff*s[n-1]*s[n-2]
               =  power

       i.e power = pz<<12
   */
    
    mult = (int)coeff_q14*(int)zprev;
    pz   = zprev2*zprev2 + zprev*zprev - ((short)(mult>>14))*zprev2;

    return (float)pz*pow(2.0,12);
}

void howclose(float x, float y, float delta) {
    if (fabs((x-y)/x) < delta)
        printf("  PASS\n");
    else
        printf("  FAIL\n");
}

/* automated test of for goertzal tone detector */

void test_tone(float coeff, float freq, float Fs, float gaindB, int expect_tone) {
    float amp, p, e;
    short x[N];
    int   n, tone;

    amp = AMP*pow(10.0, gaindB/20.0);
    e   = 0.0;
    for(n=0; n<N; n++) {
        x[n] = amp*cos(2.0*M_PI*(freq/Fs)*n);
        e += pow(x[n],2.0);
    }

    /* 
       Time domain energy of sinewave e = N(AMP^2)/2
       If all energy is in sine wave, goertzal output p = (AMP*N/2)^2
       So we would expect p = e*(N/2) if tone is present              

       So by comparing power in signal to power from goertzal filter 
       we can make decide if tone is presnet.  The sweep results from
       Test2/pgoertzal.m indicate the power is 10dB (linear 0.1) down
       at the edges of the detection mask.
    */

    p = goertzal3(x, N, coeff);
    if (p > e*(N/2)*0.1)
        tone = 1;
    else
        tone = 0;
    printf("  p: %e e: %e expect: %d  we got: %d ", p, e*N/2, expect_tone, tone);
    if (tone == expect_tone)
        printf(" PASS\n");
    else
        printf(" FAIL\n");
}

int main(void) {
    float  w, coeff, f, Fs, fctss; 
    float  ideal_goertzal_pwr, goertzal_float_pwr;
    float  goertzal1_pwr, goertzal2_pwr, goertzal3_pwr;
    FILE  *fpwr;
    short x[N];
    int   n;

    // Test 1: single tone

    w = 2.0* M_PI * ((float)F/FS);
    coeff = 2.0* cos(w);

    //printf("coeff = %f\n", coeff);

    for(n=0; n<N; n++) {
        x[n] = AMP*cos(2.0*M_PI*((float)F/FS)*n);
    }

    /* "ideal" DFT bin power should be (AMP*N/2)^2 */
    
    ideal_goertzal_pwr = pow((float)AMP*N/2,2.0);
    goertzal_float_pwr = goertzal(x, N, coeff);
    goertzal1_pwr      = goertzal1(x, N, coeff);
    goertzal2_pwr      = goertzal2(x, N, coeff);
    goertzal3_pwr      = goertzal3(x, N, coeff);
 
    printf("Test 1:\n");
    printf("  ideal.: %e\n", pow((float)AMP*N/2,2.0));
    printf("  float.: %e ", goertzal1_pwr);
    howclose(ideal_goertzal_pwr, goertzal_float_pwr, 0.02);
    printf("  step 1: %e ", goertzal1_pwr);
    howclose(ideal_goertzal_pwr, goertzal1_pwr, 0.02);
    printf("  step 2: %e ", goertzal2_pwr);
    howclose(ideal_goertzal_pwr, goertzal2_pwr, 0.02);
    printf("  step 3: %e ", goertzal3_pwr);
    howclose(ideal_goertzal_pwr, goertzal3_pwr, 0.02);
        
    /* Test 2: sweep and determine 3dB points for simulated CTCSS
       tone, examine with pgoertzal.m */
   
    Fs = 1000.0;

    fctss = 91.5;
    w = 2.0* M_PI * (fctss/Fs);
    coeff = 2.0* cos(w); 
    fpwr = fopen("pwr.txt", "wt");
    assert(fpwr != NULL);

    for(f=0; f<250; f+=0.1) {
        for(n=0; n<N; n++) {
            x[n] = AMP*cos(2.0*M_PI*(f/Fs)*n);
        }
        fprintf(fpwr,"%e %e %e\n", f, goertzal(x, N, coeff), goertzal3(x, N, coeff));
    }
    fclose(fpwr);

    /* Test 3: Check response of tone detector at different tone amplitudes and 
       frequencies */

    printf("\nTest 2:\n");
    test_tone(coeff, 91.5,  Fs,   0.0, 1);
    test_tone(coeff, 91.5,  Fs,  -6.0, 1);
    test_tone(coeff, 91.5,  Fs, -12.0, 1);
    test_tone(coeff, 90.59, Fs,   0.0, 1);
    test_tone(coeff, 92.42, Fs,   0.0, 1);
    test_tone(coeff, 88.0 , Fs,   0.0, 0);
    test_tone(coeff, 95.0 , Fs,   0.0, 0);
   
    return 0;
}
