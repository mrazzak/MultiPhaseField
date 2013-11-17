// distributes particles in randomly
//#include "ParticleDistro.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

//#include "sphere.h"
#include "sphere.cpp"
// #include "peribc.cpp"

int NucleiDistro(int* inds, double* eta, int mboxsize, int nboxsize, int kboxsize, int N, int Pr)
{ 
  int particlesn=N; // number of particles
  // cout <<"Number of particles are" << particlesn << endl;
  
  int nn,ii,jj,kk; //random indexes
  int i,j,k; // loops
  int iind,jind,kind; //periodic indexes
  double irand,jrand,prand, krand;
  double *sphi;
  sphi=sphere(Pr);
  
  for (k=0;k<kboxsize;k++)
  {
    for (j=0;j<nboxsize;j++)
    {
      for (i=0;i<mboxsize;i++)
      {
	eta[i+j*mboxsize+k*mboxsize*nboxsize]=0;
      }
    }
  }
  
  srand ( time(NULL) );
  for (nn=2;nn<particlesn+2;nn++){
    irand=rand();
    jrand=rand();
    krand=rand();
    prand=rand();
    // these are random index of the top corner of a box contaning the particle.
    // The box is generated by sphere.cpp 
    ii=int((mboxsize*irand)/RAND_MAX);
    jj=int((nboxsize*jrand)/RAND_MAX);
    kk=int((kboxsize*krand)/RAND_MAX);
    for (i=-Pr;i<Pr+1;i++)
    {
      iind=peribc(ii+i,mboxsize);
      for (j=-Pr;j<Pr+1;j++)
      {
	jind=peribc(jj+j,mboxsize);
	for (k=-Pr;k<Pr+1;k++)
	{
	  kind=peribc(kk+k,mboxsize);
	  eta[(iind)+(jind)*mboxsize+(kind)*mboxsize*nboxsize]=sphi[(i+Pr)+(j+Pr)*2*Pr+(k+Pr)*2*Pr*2*Pr]+eta[(iind)+(jind)*mboxsize+(kind)*mboxsize*nboxsize];
	  if (eta[(iind)+(jind)*mboxsize+(kind)*mboxsize*nboxsize]>1) { eta[(iind)+(jind)*mboxsize+(kind)*mboxsize*nboxsize]=1.00;}
	  if (eta[(iind)+(jind)*mboxsize+(kind)*mboxsize*nboxsize]>0.99) {inds[(iind)+(jind)*mboxsize+(kind)*mboxsize*nboxsize]=nn;} // if two nuclie overlap then one that is the last overwrites the previous one
	}
      }
    }
  }
  
  return 0;
}
