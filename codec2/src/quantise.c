/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: quantise.c
  AUTHOR......: David Rowe                                                          
  DATE CREATED: 31/5/92                                                       
                                                                             
  Quantisation functions for the sinusoidal coder.  
                                                                             
\*---------------------------------------------------------------------------*/

/*
  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 2, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <assert.h>
#include <ctype.h>
#include "sine.h"
#include "quantise.h"
#include "lpc.h"
#include "dump.h"
#include <lsp.h>
#include <speex_bits.h>
#include <quant_lsp.h>

#define MAX_ORDER    20
#define LSP_DELTA1 0.05         /* grid spacing for LSP root searches */
#define MAX_CB       10         /* max number of codebooks */

/* describes each codebook  */

typedef struct {
    int   k;        /* dimension of vector                   */
    int   m;        /* elements in codebook                  */
    char *fn;       /* file name of text file storing the VQ */
} LSP_CB;

/* lsp_q describes entire quantiser made up of several codebooks */

//#define FIRST_ATTEMPT
#ifdef FIRST_ATTEMPT

LSP_CB lsp_q[] = {
    {2, 1024, "../unittest/lsp12_01.txt" },   /* LSP 1,2    */
    {2,  512, "../unittest/lsp34_01.txt" },   /* LSP 3,4    */
    {3,  512, "../unittest/lsp57_01.txt" },   /* LSP 5,6,7  */
    {3,  512, "../unittest/lsp810_01.txt"},   /* LSP 8,9,10 */
    {0,    0, ""}
};
#else

/* 10+10+8 = 28 bit LSP quantiser */

LSP_CB lsp_q[] = {
    {3, 1024, "../unittest/lsp123.txt" },   /* LSP 1,2,3     */
    {4, 1024, "../unittest/lsp4567.txt" },  /* LSP 4,5,6,7   */
    {3,  256, "../unittest/lsp8910.txt" },  /* LSP 8,9,10    */
    {0,    0, ""}
};
#endif

/* ptr to each codebook */

static float *plsp_cb[MAX_CB];

/*---------------------------------------------------------------------------*\
									      
  quantise_uniform

  Simulates uniform quantising of a float.

\*---------------------------------------------------------------------------*/

void quantise_uniform(float *val, float min, float max, int bits)
{
    int   levels = 1 << (bits-1);
    float norm;
    int   index;

    /* hard limit to quantiser range */

    printf("min: %f  max: %f  val: %f  ", min, max, val[0]);
    if (val[0] < min) val[0] = min;
    if (val[0] > max) val[0] = max;

    norm = (*val - min)/(max-min);
    printf("%f  norm: %f  ", val[0], norm);
    index = fabs(levels*norm + 0.5);

    *val = min + index*(max-min)/levels;

    printf("index %d  val_: %f\n", index, val[0]);
}

/*---------------------------------------------------------------------------*\
									      
  lspd_quantise

  Simulates differential lsp quantiser

\*---------------------------------------------------------------------------*/

void lsp_quantise(
  float lsp[], 
  float lsp_[],
  int   order
) 
{
    int   i;
    float dlsp[MAX_ORDER];
    float dlsp_[MAX_ORDER];

    dlsp[0] = lsp[0];
    for(i=1; i<order; i++)
	dlsp[i] = lsp[i] - lsp[i-1];

    for(i=0; i<order; i++)
	dlsp_[i] = dlsp[i];

    quantise_uniform(&dlsp_[0], 0.1, 0.5, 5);

    lsp_[0] = dlsp_[0];
    for(i=1; i<order; i++)
	lsp_[i] = lsp_[i-1] + dlsp_[i];
}

/*---------------------------------------------------------------------------*\

  scan_line()

  This function reads a vector of floats from a line in a text file.

\*---------------------------------------------------------------------------*/

void scan_line(FILE *fp, float f[], int n)
/*  FILE   *fp;		file ptr to text file 		*/
/*  float  f[]; 	array of floats to return 	*/
/*  int    n;		number of floats in line 	*/
{
    char   s[MAX_STR];
    char   *ps,*pe;
    int	   i;

    fgets(s,MAX_STR,fp);
    ps = pe = s;
    for(i=0; i<n; i++) {
	while( isspace(*pe)) pe++;
	while( !isspace(*pe)) pe++;
	sscanf(ps,"%f",&f[i]);
	ps = pe;
    }
}

/*---------------------------------------------------------------------------*\

  load_cb

  Loads a single codebook (LSP vector quantiser) into memory.

\*---------------------------------------------------------------------------*/

void load_cb(char *filename, float *cb, int k, int m)
{
    FILE *ftext;
    int   lines;
    int   i;

    ftext = fopen(filename,"rt");
    if (ftext == NULL) {
	printf("Error opening text file: %s\n",filename);
	exit(1);
    }

    lines = 0;
    for(i=0; i<m; i++) {
	scan_line(ftext, &cb[k*lines++], k);
    }

    fclose(ftext);
}

/*---------------------------------------------------------------------------*\

  quantise_init

  Loads the entire LSP quantiser comprised of several vector quantisers
  (codebooks).

\*---------------------------------------------------------------------------*/

void quantise_init()
{
    int i,k,m;

    i = 0;
    while(lsp_q[i].k) {
	k = lsp_q[i].k;
	m = lsp_q[i].m;
	plsp_cb[i] = (float*)malloc(sizeof(float)*k*m);
	assert(plsp_cb[i] != NULL);
	load_cb(lsp_q[i].fn, plsp_cb[i], k, m);
	i++;
	assert(i < MAX_CB);
    }
}

/*---------------------------------------------------------------------------*\

  quantise

  Quantises vec by choosing the nearest vector in codebook cb, and
  returns the vector index.  The squared error of the quantised vector
  is added to se.

\*---------------------------------------------------------------------------*/

long quantise(float cb[], float vec[], int k, int m, float *se)
/* float   cb[][K];	current VQ codebook		*/
/* float   vec[];	vector to quantise		*/
/* int	   k;		dimension of vectors		*/
/* int     m;		size of codebook		*/
/* float   *se;		accumulated squared error 	*/
{
   float   e;		/* current error		*/
   long	   besti;	/* best index so far		*/
   float   beste;	/* best error so far		*/
   long	   j;
   int     i;

   besti = 0;
   beste = 1E32;
   for(j=0; j<m; j++) {
	e = 0.0;
	for(i=0; i<k; i++)
	    e += pow(cb[j*k+i]-vec[i],2.0);
	if (e < beste) {
	    beste = e;
	    besti = j;
	}
   }

   *se += beste;

   return(besti);
}

static float gmin=PI;

float get_gmin(void) { return gmin; }

void min_lsp_dist(float lsp[], int order)
{
    int   i;

    for(i=1; i<order; i++)
	if ((lsp[i]-lsp[i-1]) < gmin)
	    gmin = lsp[i]-lsp[i-1];
}

/*---------------------------------------------------------------------------*\
									      
  lpc_model_amplitudes

  Derive a LPC model for amplitude samples then estimate amplitude samples
  from this model with optional LSP quantisation.

  Returns the spectral distortion for this frame.

\*---------------------------------------------------------------------------*/

float lpc_model_amplitudes(
  float  Sn[],			/* Input frame of speech samples */
  MODEL *model,			/* sinusoidal model parameters */
  int    order,                 /* LPC model order */
  int    lsp_quant,             /* optional LSP quantisation if non-zero */
  float  ak[]                   /* output aks */
)
{
  float Wn[M];
  float R[MAX_ORDER+1];
  float E;
  int   i,j;
  float snr;	
  float lsp[MAX_ORDER];
  int   roots;            /* number of LSP roots found */
  int   index;
  float se;
  int   l,k,m;
  float *cb;
  
  for(i=0; i<M; i++)
    Wn[i] = Sn[i]*w[i];
  autocorrelate(Wn,R,M,order);
  levinson_durbin(R,ak,order);
  
  E = 0.0;
  for(i=0; i<=order; i++)
      E += ak[i]*R[i];
 
  if (lsp_quant) {
    roots = lpc_to_lsp(&ak[1], order, lsp, 5, LSP_DELTA1, NULL);
    if (roots != order)
	printf("LSP roots not found\n");

    i = 0; /* i-th codebook            */
    l = 0; /* which starts at l-th lsp */
    while(lsp_q[i].k) {
	k = lsp_q[i].k;
	m = lsp_q[i].m;
	cb = plsp_cb[i];
        index = quantise(cb, &lsp[l], k, m, &se);
	for(j=0; j<k; j++)
	    lsp[l+j] = cb[index*k+j];
	l += k;
	assert(l <= order);
	i++;
	assert(i < MAX_CB);
    }
    
    lsp_to_lpc(lsp, &ak[1], order, NULL);
    dump_lsp(lsp);
  }

  aks_to_M2(ak,order,model,E,&snr);   /* {ak} -> {Am} LPC decode */

  return snr;
}

/*---------------------------------------------------------------------------*\
                                                                         
   aks_to_M2()                                                             
                                                                         
   Transforms the linear prediction coefficients to spectral amplitude    
   samples.  This function determines A(m) from the average energy per    
   band using an FFT.                                                     
                                                                        
\*---------------------------------------------------------------------------*/

void aks_to_M2(
  float ak[],	/* LPC's */
  int   order,
  MODEL *model,	/* sinusoidal model parameters for this frame */
  float E,	/* energy term */
  float *snr	/* signal to noise ratio for this frame in dB */
)
{
  COMP Pw[FFT_DEC];	/* power spectrum */
  int i,m;		/* loop variables */
  int am,bm;		/* limits of current band */
  float r;		/* no. rads/bin */
  float Em;		/* energy in band */
  float Am;		/* spectral amplitude sample */
  float signal, noise;
  float E1,Am1;

  Am1 = model->A[1];

  r = TWO_PI/(FFT_DEC);

  /* Determine DFT of A(exp(jw)) --------------------------------------------*/

  for(i=0; i<FFT_DEC; i++) {
    Pw[i].real = 0.0;
    Pw[i].imag = 0.0;
  }

  for(i=0; i<=order; i++)
    Pw[i].real = ak[i];
  four1(&Pw[-1].imag,FFT_DEC,1);

  /* Determine power spectrum P(w) = E/(A(exp(jw))^2 ------------------------*/

  for(i=0; i<FFT_DEC/2; i++)
    Pw[i].real = E/(Pw[i].real*Pw[i].real + Pw[i].imag*Pw[i].imag);
  dump_Pw(Pw);

  /* Determine magnitudes by linear interpolation of P(w) -------------------*/

  signal = noise = 0.0;
  for(m=1; m<=model->L; m++) {
    am = floor((m - 0.5)*model->Wo/r + 0.5);
    bm = floor((m + 0.5)*model->Wo/r + 0.5);
    Em = 0.0;

    for(i=am; i<bm; i++)
      Em += Pw[i].real;
    Am = sqrt(Em);

    signal += pow(model->A[m],2.0);
    noise  += pow(model->A[m] - Am,2.0);
    model->A[m] = Am;
  }
  *snr = 10.0*log10(signal/noise);

  /* 
     Attenuate fundamental by 30dB if F0 < 150 Hz and LPC modelling
     error for A[1] is larger than 6dB.

     LPC modelling often makes big errors on 1st harmonic, for example
     when the fundamental has been removed by analog high pass
     filtering before sampling.  However on unfiltered speech from
     high quality sources we would like to keep the fundamental to
     maintain the speech quality.  So we check the error in A[1] and
     attenuate it if the error is large to avoid annoying low
     frequency energy after LPC modelling.

     This will require a single bit to quantise, on top of the other
     spectral magnitude bits (i.e. LSP bits + 1 total).
   */

  E1 = fabs(20.0*log10(Am1) - 20.0*log10(model->A[1]));
  if (E1 > 6.0)
      if (model->Wo < (PI*150.0/4000)) {
	  model->A[1] *= 0.032;
      }

}
