
// <source lang=cpp>

// 3D simulation with a optimized calculations
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <time.h>


// #include <pngwriter.h>

#include "symbc.cpp"
#include "GrainsStat.cpp"
//#include "resume.cpp"
//#include "WriteResults.h"

using namespace std;
int symbc();
int WriteResults();
int GrainsStat();
//int resume();

double sign(double x){
  if (x>0){return 1;}
  if (x<0){return -1;}
  if (x==0){return 0;}
  return 0;
}

// ------- main routin --------
int* MGsize;
int main(int a, char** charinput)
{
  char* dirstr;
  //sprintf(dirstr, "/media/disk/sim_res/test/");
  dirstr=charinput[1];
  cout <<"Data will be saved in this Directory:---->" << dirstr <<endl;
  
  bool fullwrite=true;
  // model parameters
  double delt=0.1;
  int timesteps1=0;
  int timesteps=12000001;
  int resumetimestep=43000;
  int writingtimesteps=100;
  double L=1;
  double m=1.0;
  double gamma=2*1.5*m;
  double kappa=2;
  double Pz=0.00;
  double Lf=L;
  int i,j,k,tn=0;
  int n;
  
  // geometry settings
  int scale=2;
  int mboxsize=50*scale; // x axis in pixels
  int nboxsize=50*scale; // y axis
  int kboxsize=50*scale;
  int r=20*scale;
  int size3=mboxsize*nboxsize*kboxsize;
  int size2=mboxsize*nboxsize;
  double delx=2/scale;      // length unit per pixel
  int p=8; // depth of indexed matrix. 6 should be more than enough for 3D system since quadrouple points have 4 order parameters in common.
  double thresh=0.0000001; //threshold value for choosing active nodes
  double *eta;
  eta= new double[mboxsize*nboxsize*kboxsize*p];
  double *eta2;
  eta2= new double[mboxsize*nboxsize*kboxsize*p];
  int *inds;
  inds= new int[mboxsize*nboxsize*kboxsize*p];
  bool* mbool;
  mbool= new bool[mboxsize*nboxsize*kboxsize*p];
  double* phi;
  if (fullwrite==true)
  {
    phi= new double[mboxsize*nboxsize*kboxsize];
  }
  for (i=0;i<mboxsize*nboxsize*kboxsize*p;i++){
   eta[i]=0;
   eta2[i]=0;
   inds[i]=0;
   mbool[i]=true;
   } 
  int* Maxindfield;
  Maxindfield= new int[mboxsize*nboxsize*kboxsize];
  // number of nucleas at the beginning of simulation
  
  int nn,ii,jj,kk,pp;
  double irand,jrand,prand, krand;
  
  // setting initial condition of a phere insize 8 grains
  /*
  for (k=0;k<kboxsize;k++)
  {
    for (j=0;j<nboxsize;j++)
    {
      for (i=0;i<mboxsize;i++)
      {
        eta[i+j*mboxsize+k*size2]=1;
      }
    }
  }
  */
  for (k=0;k<kboxsize;k++)
  {
    for (j=0;j<nboxsize;j++)
    {
      for (i=0;i<mboxsize;i++)
      {
        if (i<(mboxsize/2+1) && j<(nboxsize/2+1) && k<(nboxsize/2+1)){
          inds[i+j*mboxsize+k*size2+size3*0]=1;
          eta[i+j*mboxsize+k*size2+size3*0]=0.99;}
        if (i<(mboxsize/2+1) && j>(nboxsize/2-1) && k<(nboxsize/2+1)){
          inds[i+j*mboxsize+k*size2+size3*1]=2;
          eta[i+j*mboxsize+k*size2+size3*1]=0.99;}
        if (i>(mboxsize/2-1) && j>(nboxsize/2-1) && k<(nboxsize/2+1)){
          inds[i+j*mboxsize+k*size2+size3*2]=3;
          eta[i+j*mboxsize+k*size2+size3*2]=0.99;}
        if (i>(mboxsize/2-1) && j<(nboxsize/2+1) && k<(nboxsize/2+1)){
          inds[i+j*mboxsize+k*size2+size3*3]=4;
          eta[i+j*mboxsize+k*size2+size3*3]=0.99;}
        if (i<(mboxsize/2+1) && j<(nboxsize/2+1) && k>(nboxsize/2-1)){
          inds[i+j*mboxsize+k*size2+size3*4]=5;
          eta[i+j*mboxsize+k*size2+size3*4]=0.99;}
        if (i<(mboxsize/2+1) && j>(nboxsize/2-1) && k>(nboxsize/2-1)){
          inds[i+j*mboxsize+k*size2+size3*5]=6;
          eta[i+j*mboxsize+k*size2+size3*5]=0.99;}
        if (i>(mboxsize/2-1) && j>(nboxsize/2-1) && k>(nboxsize/2-1)){
          inds[i+j*mboxsize+k*size2+size3*0]=7;
          eta[i+j*mboxsize+k*size2+size3*0]=0.99;}
        if (i>(mboxsize/2-1) && j<(nboxsize/2+1) && k>(nboxsize/2-1)){
          inds[i+j*mboxsize+k*size2+size3*1]=8;
          eta[i+j*mboxsize+k*size2+size3*1]=0.99;}

        if(((i-mboxsize/2)*(i-mboxsize/2)+(j-nboxsize/2)*(j-mboxsize/2)+(k-kboxsize/2)*(k-kboxsize/2))<r*r){
          inds[i+j*mboxsize+k*size2+size3*0]=9;
          eta[i+j*mboxsize+k*size2+size3*0]=1;
        }
//       else{
//          inds[i+j*mboxsize+k*size2+size3*1]=8;
//          eta[i+j*mboxsize+k*size2+size3*1]=0.99;
//        }
        eta2[i+j*mboxsize+k*size2+size3]=eta[i+j*mboxsize+k*size2+size3];

      }
    }
  }

  // resuming from a file
  //if (resumetimestep>0)  resume( dirstr, inds, eta, resumetimestep, mboxsize, nboxsize, kboxsize);

  int nuclein=9;
  MGsize=new int[nuclein];
  //dynamics
  double sumterm,currenteta;
  double detadtM;
  double detadt;
  int pn,psn,pind,pos;
  double delx2=0.166666666666/(delx*delx);
  int jn, kn, pnn;
  int inplus1, inminus1, jnplus1,jnminus1, knplus1, knminus1;
  double del2,pzi;
  double sumeta, sumeta2, mineta;
  int currentind, indc, minind;
  //calculating processing time
  clock_t time1;
  time1=clock();
  cout << "Initialization ended." <<endl;

  time1=clock();
  int maxind;
  double maxeta;
  //optimized loop -----------------------------------------------------------------------------------------------------
  char filenamestat[200];
  ofstream fileGstat;
  sprintf (filenamestat, "%sGrainVol.txt",dirstr);
  fileGstat.open (filenamestat);
  int G=101;
  int oldG=1000000000;
  char stn[50];
  while (G>100)
  {
    tn=tn+1;
    for (k=0;k<kboxsize;k++)
      {
        kn=k*size2;
        for (j=0;j<nboxsize;j++)
        {
          jn=j*mboxsize;
          for (i=0;i<mboxsize;i++)
          {
            maxeta=eta[i+jn+kn];
            maxind=0;
            for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
            {
              if(maxeta<eta[i+jn+kn+indc*size3]){maxeta=eta[i+jn+kn+indc*size3]; maxind=indc;}
            }
            Maxindfield[i+jn+kn]=inds[i+jn+kn+maxind*size3];
          }
        }
      }
    G=GrainsStat(Maxindfield, MGsize, nuclein, mboxsize, nboxsize, kboxsize);
    //writing grain statistics data
    
//     if ((oldG-G)<1)
//       Pz=Pz-0.1*Pz;
//     
//     oldG=G;
   // n=printf(stn,"%.2f %.6f %.5f",tn*delt,G,Pz);
    fileGstat << tn*delt <<" " << G*delx*delx*delx <<" " << Pz<< endl;
    #pragma omp parallel for
    for (k=0;k<kboxsize;k++)
    {
      kn=k*size2;
      knplus1=symbc(k+1,kboxsize)*size2;
      knminus1=symbc(k-1,kboxsize)*size2;
      for (j=0;j<nboxsize;j++)
      {
        jn=j*mboxsize;
        jnplus1=symbc(j+1,mboxsize)*mboxsize;
        jnminus1=symbc(j-1,mboxsize)*mboxsize;
        for (i=0;i<mboxsize;i++)
        {
          for (pn=0;pn<p;pn++)
          {
            pnn=pn*size3;
            if (4>2) //mbool[i+jn+kn+pnn]==true
            {
              mbool[i+jn+kn+pnn]=false; //firts we make is false and then if detadt>thresh then it becomes true later
              inplus1=symbc(i+1,mboxsize);
              inminus1=symbc(i-1,mboxsize);
              // here is the sum of all order parameters^2 for the point i and j
              sumterm=0;
              for (psn=0;psn<p;psn++)
              {
                sumterm=sumterm+eta[i+jn+kn+psn*size3]*eta[i+jn+kn+psn*size3];
              }
              // calculation of nabla square eta
              currentind=inds[i+jn+kn+pnn];
              currenteta=eta[i+jn+kn+pnn];
              //searching for neighbors with the same index as currentind
              //  //  //  First nearest neighbors
              sumeta=0;
              for (indc=0;indc<p;indc++)
              {
                if(currentind==inds[inplus1+jn+kn+(indc)*size3]){sumeta=sumeta+eta[inplus1+jn+kn+indc*size3];}
                if(currentind==inds[inminus1+jn+kn+(indc)*size3]){sumeta=sumeta+eta[inminus1+jn+kn+indc*size3];}
                if(currentind==inds[i+jnplus1+kn+(indc)*size3]){sumeta=sumeta+eta[i+jnplus1+kn+indc*size3];}
                if(currentind==inds[i+jnminus1+kn+indc*size3]){sumeta=sumeta+eta[i+jnminus1+kn+indc*size3];}
                if(currentind==inds[i+jn+knplus1+indc*size3]){sumeta=sumeta+eta[i+jn+knplus1+indc*size3];}
                if(currentind==inds[i+jn+knminus1+indc*size3]){sumeta=sumeta+eta[i+jn+knminus1+indc*size3];}
              }
              //  //  //  Second nearest neighbors
              sumeta2=0;
              for (indc=0;indc<p;indc++)
              {
                // 0 -1 -1
                if(currentind==inds[i+jnminus1+knminus1+(indc)*size3]){sumeta2=sumeta2+eta[i+jnminus1+knminus1+indc*size3];}
                // -1 0 -1
                if(currentind==inds[inminus1+jn+knminus1+(indc)*size3]){sumeta2=sumeta2+eta[inminus1+jn+knminus1+indc*size3];}
                // 1 0 -1
                if(currentind==inds[inplus1+jn+knminus1+(indc)*size3]){sumeta2=sumeta2+eta[inplus1+jn+knminus1+indc*size3];}
                // 0 1 -1
                if(currentind==inds[i+jnplus1+knminus1+(indc)*size3]){sumeta2=sumeta2+eta[i+jnplus1+knminus1+indc*size3];}
                // -1 -1 0
                if(currentind==inds[inminus1+jnminus1+kn+(indc)*size3]){sumeta2=sumeta2+eta[inminus1+jnminus1+kn+indc*size3];}
                // 1 -1 0
                if(currentind==inds[inplus1+jnminus1+kn+(indc)*size3]){sumeta2=sumeta2+eta[inplus1+jnminus1+kn+indc*size3];}
                // -1 1 0
                if(currentind==inds[inminus1+jnplus1+kn+(indc)*size3]){sumeta2=sumeta2+eta[inminus1+jnplus1+kn+indc*size3];}
                // 1 1 0
                if(currentind==inds[inplus1+jnplus1+kn+(indc)*size3]){sumeta2=sumeta2+eta[inplus1+jnplus1+kn+indc*size3];}
                // 0 -1 1
                if(currentind==inds[i+jnminus1+knplus1+(indc)*size3]){sumeta2=sumeta2+eta[i+jnminus1+knplus1+indc*size3];}
                // -1 0 1
                if(currentind==inds[inminus1+jn+knplus1+(indc)*size3]){sumeta2=sumeta2+eta[inminus1+jn+knplus1+indc*size3];}
                // 1 0 1
                if(currentind==inds[inplus1+jn+knplus1+(indc)*size3]){sumeta2=sumeta2+eta[inplus1+jn+knplus1+indc*size3];}
                // 0 1 1
                if(currentind==inds[i+jnplus1+knplus1+(indc)*size3]){sumeta2=sumeta2+eta[i+jnplus1+knplus1+indc*size3];}
              }
              del2=delx2*(sumeta2+2*sumeta-24*currenteta);
              detadtM=m*(currenteta*currenteta*currenteta-currenteta)+gamma*(currenteta*sumterm-currenteta*currenteta*currenteta)-kappa*del2;
              pzi=3*currenteta*(1-currenteta)*sign(detadtM)*Pz;
              if (fabs(detadtM)<fabs(pzi)){
                Lf=0;
              }
              else{
                Lf=L;
              }
              detadt=-Lf*(detadtM-pzi);
              eta2[i+jn+kn+pnn]=currenteta+delt*detadt;
              if (eta2[i+jn+kn+pnn]>1) eta2[i+jn+kn+pnn]=1;
              if (eta2[i+jn+kn+pnn]<0) eta2[i+jn+kn+pnn]=0;
              // make a padding for the changing order parameter so next time step it will take that into account.
              if (eta2[i+jn+kn+pnn]>thresh) 
              {
                mbool[i+jn+kn+pnn]=true;
                if (eta2[i+jn+kn+pnn]>0.999999999) {mbool[i+jn+kn+pnn]=false;}
                pos=inplus1+jn+kn; // 1 0 0
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              pos=inminus1+jn+kn; //-1 0 0
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              pos=i+jnplus1+kn; // 0 1 0
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              pos= i+jnminus1+kn; // 0 -1 0
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              pos=i+jn+knplus1; // 0 0 1
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              pos=i+jn+knminus1; // 0 0 -1
              mineta=eta[pos];  //the first i the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              // ----------- second neighbors ------------
              // i+jnminus1+knminus1  0 -1 -1  
              pos=i+jnminus1+knminus1;
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              // inminus1+jn+knminus1 -1 0 -1 
              pos=inminus1+jn+knminus1;
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              // inplus1+jn+knminus1 1 0 -1 
              pos=inplus1+jn+knminus1;
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              // i+jnplus1+knminus1 0 1 -1 
              pos=i+jnplus1+knminus1;
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              mbool[pos+minind*size3]=true;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              // inminus1+jnminus1+kn -1 -1 0 
              pos=inminus1+jnminus1+kn;
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              // inplus1+jnminus1+kn 1 -1 0
              pos=inplus1+jnminus1+kn;
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              // inminus1+jnplus1+kn 1 3 2
              pos=inminus1+jnplus1+kn; // -1 1 0
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              // inplus1+jnplus1+kn 3 3 2
              pos=inplus1+jnplus1+kn; // 1 1 0
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              // i+jnminus1+knplus1 2 1 3
              pos=i+jnminus1+knplus1; // 0 -1 1 
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              // inminus1+jn+knplus1 1 2 3
              pos=inminus1+jn+knplus1; // -1 0 1
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              //  3 2 3
              pos=inplus1+jn+knplus1; // 1 0 1
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
              //  2 3 3
              pos=i+jnplus1+knplus1; // 0 1 1
              mineta=eta[pos];  //the first in the table is assumed as smallest 
              minind=0;
              for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
              {
                if(inds[pos+indc*size3]==currentind){minind=indc; break;}
                if(mineta>eta[pos+indc*size3]){mineta=eta[pos+indc*size3]; minind=indc;}
              }
              inds[pos+minind*size3]=currentind;
              if (eta[pos+minind*size3]<0.0001) {eta[pos+minind*size3]=0.0001;}
              mbool[pos+minind*size3]=true;
             }
            }
          }
        }
      }
    }
    //setting eta equal to the new eta2 for the next time step
    #pragma omp parallel for
    for (pind=0;pind<p;pind++)
    {
      pnn=pind*size3;
      for (k=0;k<kboxsize;k++)
      {
        kn=k*size2;
        for (j=0;j<nboxsize;j++)
        {
          jn=j*mboxsize;
          for (i=0;i<mboxsize;i++)
          {
            eta[i+jn+kn+pnn]=eta2[i+jn+kn+pnn];
          }
        }
      }
    }
    // write array into a file each 100 time steps
    if  (tn % writingtimesteps ==0)
    {
      cout << "Time required for time step number [" << tn << " ] is :" << double(clock()-time1)/double(CLOCKS_PER_SEC) << " seconds. G=" << G << "\n";
      time1=clock();
      if (fullwrite==true)
      {
        // making the phi array
        for (k=0;k<kboxsize;k++)
        {
          kn=k*size2;
          for (j=0;j<nboxsize;j++)
          {
            jn=j*mboxsize;
            for (i=0;i<mboxsize;i++)
            {
              phi[i+jn+kn]=0;
              for (pind=0;pind<p;pind++)
              {
                pnn=pind*size3;
                phi[i+jn+kn]=phi[i+jn+kn]+eta[i+jn+kn+pnn]*eta[i+jn+kn+pnn];
              }
            }
          }
        }
        char filename[200];
        ofstream myfile2;
        // make a string like "result_5.txt"
        n=sprintf (filename, "%sFullres_%d.txt",dirstr, tn);
        myfile2.open (filename);
        for (k=0;k<kboxsize;k++)
        {
          kn=k*size2;
          for (j=0;j<nboxsize;j++)
          {
            jn=j*mboxsize;
            for (i=0;i<mboxsize;i++)
            {
              myfile2 << phi[i+jn+kn] << " "; 
            }
            myfile2 << endl;
          }
        }
        myfile2.close();
      }
    }
    if  (tn % writingtimesteps ==0)
    {
      // writing indexed matrix
      char filename3[200];
      ofstream myfile3;
      // make a string like "result_5.txt"
      n=sprintf (filename3, "%sInds_%d.txt",dirstr, tn);
      myfile3.open (filename3);
      for (k=0;k<kboxsize;k++)
      {
        kn=k*size2;
        for (j=0;j<nboxsize;j++)
        {
          jn=j*mboxsize;
          for (i=0;i<mboxsize;i++)
          {
            maxeta=eta[i+jn+kn];
            maxind=0;
            for (indc=0;indc<p;indc++) //loop around the neighbor and kick the minimum out and replace it with new growing eta
            {
              if(maxeta<eta[i+jn+kn+indc*size3]){maxeta=eta[i+jn+kn+indc*size3]; maxind=indc;}
            }
            Maxindfield[i+jn+kn]=inds[i+jn+kn+maxind*size3];
            myfile3 << Maxindfield[i+jn+kn] << " ";
            
          }
          myfile3 << endl;
        }
      }
      myfile3.close();
      
    }
    cout <<tn << endl;
  }
  fileGstat.close();
  return 0;
}


// </source>
