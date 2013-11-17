// sparce structure with boolian variable.
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <time.h>


// #include <pngwriter.h>

#include "symbc.cpp"
//#include "WriteResults.h"
#include "etavolume.cpp"
#include "createdome.cpp"

using namespace std;
int symbc();
int WriteResults();
double* createdome();
double etavolume();
// ------- main routin --------
int main(int a, char** charinput)
{
  ofstream myfile ;
  // model parameters
  double delt=0.05;
  int timesteps1=299;
  
  double L=1;
  double alpha=1 ;
  double beta=1 ;
  double gamma=1;
  double kappa=2;
  double epsilon=5;
  int i,j,tn;
  // geometry settings
  int r; //dome radius
  r= atoi(charinput[1]);

  int p=2; // phase field numbers
  int scale=2;
  int mboxsize=100*scale; // x axis in pixels
  int nboxsize=100*scale; // y axis
  double delx=2/scale;      // length unit per pixel

  
  double thresh=0.001; //threshold value for choosing active nodes
  double* eta;
  eta= new double[mboxsize*nboxsize*p];
  double *eta2;
  eta2= new double[mboxsize*nboxsize*p];
  bool *mbool;
  mbool= new bool[mboxsize*nboxsize*p];
  int nn,ii,jj;
  int inplus1, inminus1, jnplus1,jnminus1, knplus1, knminus1;
  double irand,jrand,prand;
  
  eta=createdome(eta, mboxsize, nboxsize,r);
  // particles distribution specification
  
  //dynamics
  double sumterm,sumtermp;
  double detadtM;
  double detadt;
  int pn,psn,pind;
  double delx2=2.0/3.0/(delx*delx);
  int size=mboxsize*nboxsize;
  int jn, pnn;
  double del2;
  
  //calculating processing time
  clock_t time1;
  time1=clock();
  
  for (tn=1;tn<timesteps1;tn++)
  {
    time1=clock();
    for (j=0;j<nboxsize;j++)
    {
      jn=j*mboxsize;
      jnplus1=symbc(j+1,mboxsize)*mboxsize;
      jnminus1=symbc(j-1,mboxsize)*mboxsize;
      for (i=0;i<mboxsize;i++)
      {
        inplus1=symbc(i+1,nboxsize);
        inminus1=symbc(i-1,nboxsize);
        // here is the sum of all order parameters^2 for the point i and j
        sumterm=0;
        for (psn=0;psn<p;psn++)
        {
          sumterm=sumterm+eta[i+jn+psn*size]*eta[i+jn+psn*size];
        }
        // calculation of nabla square eta
        for (pn=0;pn<p;pn++)
        {
          pnn=pn*size;
          del2=delx2*((eta[inplus1+jn+pnn]+eta[inminus1+jn+pnn]+eta[i+jnplus1+pnn]+eta[i+jnminus1+pnn])-4*eta[i+jn+pnn]);
          //     del2=delx2*((eta[inplus1+jn+pnn]+eta[inminus1+jn+pnn]+eta[i+jnplus1+pnn]+eta[i+jnminus1+pnn])
          //     +0.25*(eta[inplus1+jnplus1+pnn]+eta[inplus1+jnminus1+pnn]+eta[inminus1+jnplus1+pnn]+eta[inminus1+jnminus1+pnn])
          //     -5*eta[i+jn+pnn]);
          sumtermp=eta[i+jn+pnn]*sumterm-pow(eta[i+jn+pnn],3);
          detadtM=-alpha*eta[i+jn+pnn]+beta*pow(eta[i+jn+pnn],3)-kappa*del2;
          detadt=-L*(detadtM+2*gamma*sumtermp);
          if (fabs(detadt)>thresh) // optimization function
          {
            mbool[i+jn+pnn]=true;
            mbool[symbc(i+1,nboxsize)+jn+pnn]=true;
            mbool[symbc(i-1,nboxsize)+jn+pnn]=true;
            mbool[i+symbc(j+1,mboxsize)*mboxsize+pnn]=true;
            mbool[i+symbc(j-1,mboxsize)*mboxsize+pnn]=true;
          }
          else
          {
            mbool[i+jn+pnn]=false; 
          }
          
          eta2[i+jn+pnn]=eta[i+jn+pnn]+delt*detadt;
          // to make sure eta is not outside the equilibrium values. This increases stability of calculation by controlling bounds of the eta whithin equilibrium values
          if (eta2[i+jn+pnn]>1) eta2[i+jn+pnn]=1;
          if (eta2[i+jn+pnn]<0) eta2[i+jn+pnn]=0;
        }
      }
    }
    //setting eta equal to the new eta2 for the next time step
    for (i=0;i<mboxsize;i++)
    {
      for (j=0;j<nboxsize;j++)
      {
        jn=j*mboxsize;
        for (pind=0;pind<p;pind++)
        {
          pnn=pind*size;
          eta[i+jn+pnn]=eta2[i+jn+pnn];
        }
      }
    }
    //cout << tn << "\n";
  }
  cout << "time required for 300 time steps:" << double((clock()-time1))/double(CLOCKS_PER_SEC) << "seconds. \n";
  //optimized loop -----------------------------------------------------------------------------------------------------
  ofstream volfile;
  // make a string like "result_5.txt"
  int n;
  char filename[200];
  n=sprintf (filename,"/media/disk/sim_res/Dome_Friction_cpp/vollog_%d.log",r);
  volfile.open (filename);
  double vol=mboxsize*nboxsize, initvol;
  initvol=etavolume(eta,mboxsize, nboxsize)*delx*delx;
  tn=0;
  while (initvol*0.5<vol)
  {
    tn=tn+1;
    time1=clock();
    vol=etavolume(eta,mboxsize, nboxsize)*delx*delx;
    volfile << tn*delt << " " << vol << endl;
    for (j=0;j<mboxsize;j++)
    {
      jn=j*mboxsize;
      jnplus1=symbc(j+1,mboxsize)*mboxsize;
      jnminus1=symbc(j-1,mboxsize)*mboxsize;
      for (i=0;i<nboxsize;i++)
      {
        inplus1=symbc(i+1,nboxsize);
        inminus1=symbc(i-1,nboxsize);
        // calculation of nabla square eta
        for (pn=0;pn<p;pn++)
        {
          pnn=pn*size;
          if (mbool[i+jn+pnn]==true)
          {
            sumterm=0;// here is the sum of all order parameters^2 for the point i and j
            for (psn=0;psn<p;psn++)
            {
              sumterm=sumterm+eta[i+jn+psn*size]*eta[i+jn+psn*size];
            }
            /* del2[pn]=delx2*(0.5*(eta[symbc(i+1,nboxsize)+jn+pnn]-2*eta[i+jn+pnn]+eta[symbc(i-1,nboxsize)+jn+pnn])
            +0.25*(eta[symbc(i+2,nboxsize)+jn+pnn]-2*eta[i+jn+pnn]+eta[symbc(i-2,nboxsize)+jn+pnn]))
            +delx2*(0.5*(eta[i+symbc(j+1,mboxsize)*mboxsize+pnn]-2*eta[i+jn+pnn]+eta[i+symbc(j-1,mboxsize)*mboxsize+pnn])
            +0.25*(eta[i+symbc(j+2,mboxsize)*mboxsize+pnn]-2*eta[i+jn+pnn]+eta[i+symbc(j-2, mboxsize)*mboxsize+pnn]));
            */
            del2=delx2*((eta[inplus1+jn+pnn]+eta[inminus1+jn+pnn]+eta[i+jnplus1+pnn]+eta[i+jnminus1+pnn])-4*eta[i+jn+pnn]);
            /*          del2=delx2*((eta[symbc(i+1,nboxsize)+jn+pnn]+eta[symbc(i-1,nboxsize)+jn+pnn]+eta[i+symbc(j+1,mboxsize)*mboxsize+pnn]+eta[i+symbc(j-1,mboxsize)*mboxsize+pnn])
            +0.25*(eta[symbc(i+1,nboxsize)+symbc(j+1,mboxsize)*mboxsize+pnn]+eta[symbc(i+1,nboxsize)+symbc(j-1,mboxsize)*mboxsize+pnn]+eta[symbc(i-1,nboxsize)+symbc(j+1,mboxsize)*mboxsize+pnn]+eta[symbc(i-1,nboxsize)+symbc(j-1,mboxsize)*mboxsize+pnn])
            -5*eta[i+jn+pnn]);
            */
            sumtermp=eta[i+jn+pnn]*sumterm-pow(eta[i+jn+pnn],3);
            detadtM=-alpha*eta[i+jn+pnn]+beta*pow(eta[i+jn+pnn],3)-kappa*del2;
            detadt=-L*(detadtM+2*gamma*sumtermp);
            if (fabs(detadt)>thresh) // optimization function
            {
              mbool[i+jn+pnn]=true;
              mbool[symbc(i+1,nboxsize)+jn+pnn]=true;
              mbool[symbc(i-1,nboxsize)+jn+pnn]=true;
              mbool[i+symbc(j+1,mboxsize)*mboxsize+pnn]=true;
              mbool[i+symbc(j-1,mboxsize)*mboxsize+pnn]=true;
            }
            else
            {
              mbool[i+jn+pnn]=false; 
            }
            eta2[i+jn+pnn]=eta[i+jn+pnn]+delt*detadt;
            // to make sure eta is not outside the equilibrium values. This increases stability of calculation by controlling bounds of the eta whithin equilibrium values
            if (eta2[i+jn+pnn]>1) eta2[i+jn+pnn]=1;
            if (eta2[i+jn+pnn]<0) eta2[i+jn+pnn]=0;
          }
        }
      }
    }
    //setting eta equal to the new eta2 for the next time step
    for (i=0;i<mboxsize;i++)
    {
      for (j=0;j<nboxsize;j++)
      {
        jn=j*mboxsize;
        for (pind=0;pind<p;pind++)
        {
          pnn=pind*size;
          eta[i+jn+pnn]=eta2[i+jn+pnn];
        }
      }
    }
    // write array into a file each 100 time steps
    if  (tn % 5000 ==0)
    {
      double *phi;
      phi= new double[mboxsize*nboxsize];
      // making the phi array
      for (i=0;i<mboxsize;i++)
      {
        for (j=0;j<nboxsize;j++)
        {
          jn=j*mboxsize;
          phi[i+jn]=0;
          for (pind=0;pind<p;pind++)
          {
            pnn=pind*size;
            phi[i+jn]=phi[i+jn]+eta[i+jn+pnn]*eta[i+jn+pnn];
          }
        }
      }
      // writing
      ofstream myfile;
      int n;
      char filename[200];
      n=sprintf (filename, "/media/disk/sim_res/Dome_Friction_cpp/%d/result_%d.txt", r, tn);
      myfile.open (filename);
      for (i=0;i<mboxsize;i++)
      {
        for (j=0;j<nboxsize;j++)
        {
          myfile << phi[i+j*mboxsize] << "      "; 
        }
        myfile << "\n";
      }
      myfile.close();
    cout << "Time required for time step number [ " << tn << " ] is :" << double(clock()-time1)/double(CLOCKS_PER_SEC) << " seconds. \n";      
    }
  }
  volfile.close();
  cout << "Calculation for r=" << r << " is finished." << endl;
  if (tn>2000000){return 0}
  return 0;
}
