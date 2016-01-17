/*---------------------------------------------------------------------------*\

  FILE........: fsk.c
  AUTHOR......: Brady O'Brien
  DATE CREATED: 7 January 2016

  C Implementation of 2FSK modulator/demodulator, based on octave/fsk_horus.m

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2016 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2.1, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

/*---------------------------------------------------------------------------*\

                               DEFINES

\*---------------------------------------------------------------------------*/

/* P oversampling rate constant -- should probably be init-time configurable */
#define ct_P 8

/*---------------------------------------------------------------------------*\

                               INCLUDES

\*---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "fsk.h"
#include "comp_prim.h"
#include "kiss_fftr.h"

/*---------------------------------------------------------------------------*\

                               FUNCTIONS

\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\

  FUNCTION....: fsk_create
  AUTHOR......: Brady O'Brien
  DATE CREATED: 7 January 2016
  
  Create and initialize an instance of the FSK modem. Returns a pointer
  to the modem state/config struct. One modem config struct may be used
  for both mod and demod. returns NULL on failure.

\*---------------------------------------------------------------------------*/

struct FSK * fsk_create(int Fs, int Rs, int tx_f1,int tx_f2)
{
    struct FSK *fsk;
    int i;
    int Ndft = 0;
    int memold;
    
    /* Check configuration validity */
    assert(Fs > 0 );
    assert(Rs > 0 );
    assert(tx_f1 > 0);
    assert(tx_f2 > 0);
    assert(ct_P > 0);
    /* Ts (Fs/Rs) must be an integer */
    assert( (Fs%Rs) == 0 );
    /* Ts/P (Fs/Rs/P) must be an integer */
    assert( ((Fs/Rs)%ct_P) == 0 );
    
    fsk = (struct FSK*) malloc(sizeof(struct FSK));
    if(fsk == NULL) return NULL;
    
    /* Find smallest 2^N value that fits Fs for efficient FFT */
    /* It would probably be better to use KISS-FFt's routine here */
    for(i=1; i; i<<=1)
        if(Fs&i)
            Ndft = i<<1;
    
    /* Set constant config parameters */
    fsk->Fs = Fs;
    fsk->Rs = Rs;
    fsk->Ts = Fs/Rs;
    fsk->N = Fs;
    fsk->P = ct_P;
    fsk->Nsym = fsk->N/fsk->Ts;
    fsk->Ndft = Ndft;
    fsk->Nmem = fsk->N+(2*fsk->Ts);
    fsk->f1_tx = tx_f1;
    fsk->f2_tx = tx_f2;
    fsk->nin = fsk->N;
    
    /* Set up rx state */
    fsk->phi1_d = 0;
    fsk->phi2_d = 0;
    fsk->phi1_c.real = 1;
    fsk->phi1_c.imag = 0;
    fsk->phi2_c.real = 1;
    fsk->phi2_c.imag = 0;
    
    memold = (4*fsk->Ts);
    
    fsk->nstash = memold; 
    fsk->samp_old = (float*) malloc(sizeof(float)*memold);
    if(fsk->samp_old == NULL){
        free(fsk);
        return NULL;
    }
    
    for(int i=0;i<memold;i++)fsk->samp_old[i]=0;
    
    fsk->fft_cfg = kiss_fftr_alloc(Ndft,0,NULL,NULL);
    if(fsk->fft_cfg == NULL){
        free(fsk->samp_old);
        free(fsk);
        return NULL;
    }
    
    fsk->norm_rx_timing = 0;
    
    /* Set up tx state */
    fsk->tx_phase = 0;
    fsk->tx_phase_c.imag = 0;
    fsk->tx_phase_c.real = 1;
    
    /* Set up demod stats */
    fsk->EbNodB = 0;
    fsk->f1_est = 0;
    fsk->f2_est = 0;
    
    return fsk;
}

uint32_t fsk_nin(struct FSK *fsk){
    return (uint32_t)fsk->nin;
}

void fsk_destroy(struct FSK *fsk){
    free(fsk->fft_cfg);
    free(fsk->samp_old);
    free(fsk);
}

/*
 * Internal function to estimate the frequencies of the two tones within a block of samples.
 * This is split off because it is fairly complicated, needs a bunch of memory, and probably
 * takes more cycles than the rest of the demod.
 * Parameters:
 * fsk - FSK struct from demod containing FSK config
 * fsk_in - block of samples in this demod cycles, must be nin long
 * f1_est - pointer to f1 estimate variable in demod
 * f2_est - pointer to f2 estimate variable in demod
 * twist - pointer to twist estimate in demod - Note: this isn't correct right now
 */
void fsk_demod_freq_est(struct FSK *fsk, float fsk_in[],float *f1_est,float *f2_est,float *twist){
    int Ndft = fsk->Ndft;
    int Fs = fsk->Fs;
    int nin = fsk->nin;
    int i,j;
    int fft_samps;
    float hann;
    float max;
    int m1,m2;
    kiss_fftr_cfg fft_cfg = fsk->fft_cfg;
    
    /* Array to do complex FFT from using kiss_fft */
    /* It'd probably make more sense here to use kiss_fftr */
    //kiss_fft_scalar fftin[Ndft];
    kiss_fft_scalar *fftin = (kiss_fft_scalar*)malloc(sizeof(kiss_fft_scalar)*Ndft);
    //kiss_fft_cpx fftout[(Ndft/2)+1];
    kiss_fft_cpx *fftout = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx)*(Ndft/2)+1);
    fft_samps = nin<Ndft?nin:Ndft;
    
    /* Copy FSK buffer into reals of FFT buffer and apply a hann window */
    for(i=0; i<fft_samps ; i++){
        /* Note : This is a sort of bug copied from fsk_horus */
        /* if N+Ts/2 > Ndft, the end of the hann window may be cut off */
        /* resulting in a dirty FFT */
        /* An easy fix would be to ensure that Ndft is always greater than N+Ts/2 
         * instead of N */
        /* This bug isn't a big deal and can't show up in the balloon config */
        /* as 8192 > 8040 */
        hann = sinf((M_PI*(float)i)/((float)nin-1));
        
        fftin[i] = (kiss_fft_scalar)hann*hann*fsk_in[i];
    }
    /* Zero out the remaining slots */
    for(; i<Ndft;i++){
        fftin[i] = 0;
    }
    
    /* Do the FFT */
    kiss_fftr(fft_cfg,fftin,fftout);
    
    /* Find the magnitude^2 of each freq slot and stash away in the real
     * value, so this only has to be done once. Since we're only comparing
     * these values and only need the mag of 2 points, we don't need to do
     * a sqrt to each value */
    for(i=0; i<Ndft/2; i++){
        fftout[i].r = (fftout[i].r*fftout[i].r) + (fftout[i].i*fftout[i].i) ;
    }
    
    /* Find the maximum */
    max = 0;
    m1 = 0;
    for(i=0; i<Ndft/2; i++){
        if(fftout[i].r > max){
            max = fftout[i].r;
            m1 = i;
        }
    }
    
    /* Zero out 100Hz around the maximum */
    i = m1 - 100*Ndft/Fs;
    i = i<0 ? 0 : i;
    j = m1 + 100*Ndft/Fs;
    j = j>Ndft/2 ? Ndft/2 : j;
    
    for(;i<j;i++)
        fftout[i].r = 0;
    
    /* Find the other maximum */
    max = 0;
    m2 = 0;
    for(i=0; i<Ndft/2; i++){
        if(fftout[i].r > max){
            max = fftout[i].r;
            m2 = i;
        }
    }
    
    /* f1 is always the lower tone */
    if(m1>m2){
        j=m1;
        m1=m2;
        m2=j;
    }
    
    *f1_est = (float)m1*(float)Fs/(float)Ndft;
    *f2_est = (float)m2*(float)Fs/(float)Ndft;
    *twist = 20*log10f((float)(m1)/(float)(m2));
    //printf("ESTF - f1 = %f, f2 = %f, twist = %f \n",*f1_est,*f2_est,*twist);
    
    free(fftin);
    free(fftout);
}

/*
 * Euler's formula in a new convenient function
 */
static inline COMP comp_exp_j(float phi){
    COMP res;
    res.real = cosf(phi);
    res.imag = sinf(phi);
    return res;
}

/*
 * Quick and easy complex 0
 */
static inline COMP comp0(){
    COMP res;
    res.real = 0;
    res.imag = 0;
    return res;
}

/*
 * Compare the magnitude of a and b. if |a|>|b|, return true, otw false.
 * This needs no square roots
 */
static inline int comp_mag_gt(COMP a,COMP b){
    return ((a.real*a.real)+(a.imag*a.imag)) > ((b.real*b.real)+(b.imag*b.imag));
}

static inline COMP comp_normalize(COMP a){
	float av = sqrtf((a.real*a.real)+(a.imag*a.imag));
	COMP b;
	b.real = a.real/av;
	b.imag = a.imag/av;
	return b;
}


void fsk_demod(struct FSK *fsk, uint8_t rx_bits[], float fsk_in[]){
    int N = fsk->N;
    int Ts = fsk->Ts;
    int Rs = fsk->Rs;
    int Fs = fsk->Fs;
    int nsym = fsk->Nsym;
    int nin = fsk->nin;
    int P = fsk->P;
    int Nmem = fsk->Nmem;
    int i,j,dc_i,cbuf_i;
    float ft1,ft2;
    float twist; /* NOTE: This is not correct ATM */
    int nstash = fsk->nstash;
    COMP *f1_int, *f2_int;
    COMP t1,t2;
    float phi1_d = fsk->phi1_d;
    float phi2_d = fsk->phi2_d;
    float phi_ft = 0; /* Phase of fine timing estimator */
    int nold = Nmem-nin;
    float dphi1,dphi2,dphift;
    float f1,f2;
    float rx_timing,norm_rx_timing;//,old_norm_rx_timing;//,d_norm_rx_timing;
    int using_old_samps;
    float *sample_src;
    COMP *f1_intbuf,*f2_intbuf;
    
    /* Estimate tone frequencies */
    fsk_demod_freq_est(fsk,fsk_in,&f1,&f2,&twist);
    
    
    /* allocate memory for the integrated samples */
    /* Note: This must be kept after fsk_demod_freq_est for memory usage reasons */
    //f1_int = (COMP*) alloca(sizeof(COMP)*(nsym+1)*P);
    //f2_int = (COMP*) alloca(sizeof(COMP)*(nsym+1)*P);
    
    /* Allocate circular buffers for integration */
    //f1_intbuf = (COMP*) alloca(sizeof(COMP)*Ts);
    //f2_intbuf = (COMP*) alloca(sizeof(COMP)*Ts);
        /* Note: This must be kept after fsk_demod_freq_est for memory usage reasons */
    f1_int = (COMP*) malloc(sizeof(COMP)*(nsym+1)*P);
    f2_int = (COMP*) malloc(sizeof(COMP)*(nsym+1)*P);
    
    /* Allocate circular buffers for integration */
    f1_intbuf = (COMP*) malloc(sizeof(COMP)*Ts);
    f2_intbuf = (COMP*) malloc(sizeof(COMP)*Ts);
    
    /* Note: This would all be quite a bit faster with complex oscillators, like
     * TX. */
    /* TODO: change these to complex oscillators */
    /* Figure out how much to nudge each sample downmixer for every sample */
    dphi1 = 2*M_PI*((float)(f1)/(float)(Fs));
    dphi2 = 2*M_PI*((float)(f2)/(float)(Fs));


    dc_i = 0;
    cbuf_i = 0;
    sample_src = &(fsk->samp_old[nstash-nold]);
    using_old_samps = 1;
    
    /* Pre-fill integration buffer */
    for(dc_i=0; dc_i<Ts-(Ts/P); dc_i++){
        /* Switch sample source to new samples when we run out of old ones */
        if(dc_i>=nold && using_old_samps){
            sample_src = &fsk_in[0];
            dc_i = 0;
            using_old_samps = 0;
        }
        /* Downconvert and place into integration buffer */
        f1_intbuf[dc_i]=fcmult(sample_src[dc_i],comp_exp_j(phi1_d));;
        f2_intbuf[dc_i]=fcmult(sample_src[dc_i],comp_exp_j(phi2_d));;

        /* Spin downconversion phases */
        phi1_d -= dphi1;
        phi2_d -= dphi2;
        if(phi1_d<0) phi1_d+=2*M_PI;
        if(phi2_d<0) phi2_d+=2*M_PI;
    }
    cbuf_i = dc_i;
    
    /* Integrate over Ts at offsets of Ts/P */
    for(i=0; i<(nsym+1)*P; i++){
        /* Downconvert and Place Ts/P samples in the integration buffers */
        for(j=0; j<(Ts/P); j++,dc_i++){
            /* Switch sample source to new samples when we run out of old ones */
            if(dc_i>=nold && using_old_samps){
                sample_src = &fsk_in[0];
                dc_i = 0;
                using_old_samps = 0;
            }
            /* Downconvert and place into integration buffer */
            f1_intbuf[cbuf_i+j]=fcmult(sample_src[dc_i],comp_exp_j(phi1_d));;
            f2_intbuf[cbuf_i+j]=fcmult(sample_src[dc_i],comp_exp_j(phi2_d));;

            /* Spin downconversion phases */
            phi1_d -= dphi1;
            phi2_d -= dphi2;
            if(phi1_d<0) phi1_d+=2*M_PI;
            if(phi2_d<0) phi2_d+=2*M_PI;
        }
        cbuf_i += Ts/P;
        if(cbuf_i>=Ts) cbuf_i = 0;
        
        /* Integrate over the integration buffers, save samples */
        t1 = t2 = comp0();
        for(j=0; j<Ts; j++){
            t1 = cadd(t1,f1_intbuf[j]);
            t2 = cadd(t2,f2_intbuf[j]);
        }
        f1_int[i] = t1;
        f2_int[i] = t2;
        
    }

    fsk->phi1_d = phi1_d;
    fsk->phi2_d = phi2_d;

    /* Stash samples away in the old sample buffer for the next round of bit getting */
    memcpy((void*)&(fsk->samp_old[0]),(void*)&(fsk_in[nin-nstash]),sizeof(float)*nstash);
    
    /* Fine Timing Estimation */
    /* Apply magic nonlinearity to f1_int and f2_int, shift down to 0, 
     * exract angle */
     
    /* Figure out how much to spin the oscillator to extract magic spectral line */
    dphift = 2*M_PI*((float)(Rs)/(float)(P*Rs));
    t1=comp0();
    for(i=0; i<(nsym+1)*P; i++){
        /* Get abs of f1_int[i] and f2_int[i] */
        ft1 = sqrtf( (f1_int[i].real*f1_int[i].real) + (f1_int[i].imag*f1_int[i].imag) );
        ft2 = sqrtf( (f2_int[i].real*f2_int[i].real) + (f2_int[i].imag*f2_int[i].imag) );
        /* Add and square 'em */
        ft1 = ft1-ft2;
        ft1 = ft1*ft1;
        /* Spin the oscillator for the magic line shift */
        /* Down shift and accumulate magic line */
        t1 = cadd(t1,fcmult(ft1,comp_exp_j(phi_ft)));
        phi_ft -= dphift;
        if(phi_ft<0) phi_ft+=2*M_PI;
    }
    /* Get the magic angle */
    norm_rx_timing =  -atan2f(t1.imag,t1.real)/(2*M_PI);
    rx_timing = norm_rx_timing*(float)P;
    
    //old_norm_rx_timing = fsk->norm_rx_timing;
    fsk->norm_rx_timing = norm_rx_timing;
    
    /* Estimate sample clock offset */
    //d_norm_rx_timing = norm_rx_timing - old_norm_rx_timing;
    
    /* Filter out big jumps in due to nin change */
    //if(fabsf(d_norm_rx_timing) < .2){
    
    /* Figure out how many samples are needed the next modem cycle */
    if(norm_rx_timing > 0.25)
        fsk->nin = N+Ts/2;
    else if(norm_rx_timing < -0.25)
        fsk->nin = N-Ts/2;
    else
        fsk->nin = N;
    
    /* Re-sample the integrators with linear interpolation magic */
    int low_sample = (int)floorf(rx_timing);
    float fract = rx_timing - (float)low_sample;
    int high_sample = (int)ceilf(rx_timing);
  
    /* FINALLY, THE BITS */
    /* also, resample f1_int,f2_int */
    for(i=0; i<nsym; i++){
        int st = (i+1)*P;
        t1 =         fcmult(1-fract,f1_int[st+ low_sample]);
        t1 = cadd(t1,fcmult(  fract,f1_int[st+high_sample]));
        t2 =         fcmult(1-fract,f2_int[st+ low_sample]);
        t2 = cadd(t2,fcmult(  fract,f2_int[st+high_sample]));
        
        /* THE BIT! */
        rx_bits[i] = comp_mag_gt(t2,t1)?1:0;
        /* Soft output goes here */
    }
    
    free(f1_int);
    free(f2_int);
    free(f1_intbuf);
    free(f2_intbuf);
    
    printf("rx_timing: %3.2f low_sample: %d high_sample: %d fract: %3.3f nin_next: %d\n", rx_timing, low_sample, high_sample, fract, fsk->nin);
}


void fsk_mod_realphase(struct FSK *fsk,float fsk_out[],uint8_t tx_bits[]){
    float tx_phase = fsk->tx_phase; /* current TX phase */
    int f1_tx = fsk->f1_tx;         /* '0' frequency */
    int f2_tx = fsk->f2_tx;         /* '1' frequency */
    int Ts = fsk->Ts;               /* samples-per-symbol */
    int Fs = fsk->Fs;               /* sample freq */
    int i,j;
    uint8_t bit;
    
    /* delta-phase per cycle for both symbol freqs */
    float dph_f1 = 2*M_PI*((float)(f1_tx)/(float)(Fs));
    float dph_f2 = 2*M_PI*((float)(f2_tx)/(float)(Fs));

    /* Note: Right now, this mirrors fm.c and fsk_horus.m, but it
     * ought to be possible to make it more efficent (one complex mul per
     * cycle) by using a complex number for the phase. */
     
    /* Outer loop through bits */
    for(i=0; i<fsk->Nsym; i++){
        bit = tx_bits[i];
        for(j=0; j<Ts; j++){
            /* Nudge phase forward a bit */
            tx_phase += (bit==0)?dph_f1:dph_f2;
            /* Make sure phase stays on [0,2pi] */
            if(tx_phase>2*M_PI)
                tx_phase -= 2*M_PI;
            fsk_out[i*Ts+j] = 2*cosf(tx_phase);
        }
    }
    
    /* save TX phase */
    fsk->tx_phase = tx_phase;
    
}

void fsk_mod(struct FSK *fsk,float fsk_out[],uint8_t tx_bits[]){
    COMP tx_phase_c = fsk->tx_phase_c; /* Current complex TX phase */
    int f1_tx = fsk->f1_tx;         /* '0' frequency */
    int f2_tx = fsk->f2_tx;         /* '1' frequency */
    int Ts = fsk->Ts;               /* samples-per-symbol */
    int Fs = fsk->Fs;               /* sample freq */
    COMP dosc_f1, dosc_f2;          /* phase shift per sample */
    COMP dph;                       /* phase shift of current bit */
    int i,j;
    float tx_phase_mag;
    
    /* Figure out the amount of phase shift needed per sample */
    /*dosc_f1.real = cosf(2*M_PI*((float)(f1_tx)/(float)(Fs)));
    dosc_f1.imag = sinf(2*M_PI*((float)(f1_tx)/(float)(Fs)));
    dosc_f2.real = cosf(2*M_PI*((float)(f2_tx)/(float)(Fs)));
    dosc_f2.imag = sinf(2*M_PI*((float)(f2_tx)/(float)(Fs)));*/
    dosc_f1 = comp_exp_j(2*M_PI*((float)(f1_tx)/(float)(Fs)));
    dosc_f2 = comp_exp_j(2*M_PI*((float)(f2_tx)/(float)(Fs)));
    
    /* Outer loop through bits */
    for(i=0; i<fsk->Nsym; i++){
        /* select current bit phase shift */
        dph = tx_bits[i]==0?dosc_f1:dosc_f2;
        for(j=0; j<Ts; j++){
            tx_phase_c = cmult(tx_phase_c,dph);
            fsk_out[i*Ts+j] = 2*tx_phase_c.real;
        }
    }
    
    /* Normalize TX phase to prevent drift */
    tx_phase_mag = cabsolute(tx_phase_c);
    tx_phase_c.real = tx_phase_c.real/tx_phase_mag;
    tx_phase_c.imag = tx_phase_c.imag/tx_phase_mag;
    
    /* save TX phase */
    fsk->tx_phase_c = tx_phase_c;
    
}










