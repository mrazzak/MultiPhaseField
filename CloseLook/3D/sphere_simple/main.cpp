// 3D simulation with a optimized calculations
// for an interface moving in array of particles
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <time.h>

// #include <pngwriter.h>
//#include "peribc.h"
//#include "sphere.h"
//#include "ParticleDistro.h"
//#include "WriteResults.h"

#include "peribc.cpp"
#include "symbc.cpp"
#include "ParticleDistro.cpp"
#include "etavolume.cpp"
#include "calculatephi.cpp"

using namespace std;
// global variables

// functions
int peribc();
int symbc();
double* shpere();
double* ParticleDistro();
double* calculatephi();
double etavolume();
int WriteResults();

int main(int a, char** charinput)
{
  char* dirstr;
  dirstr=charinput[1];
  cout <<"Data will be saved in this Directory:---->" << dirstr <<endl;
  
  // geometry settings
  int p=2; // phase field numbers
  int scale=2;
  int mboxsize=150*scale; // x axis in pixels
  int nboxsize=150*scale; // y axis
  int kboxsize=150*scale;
  int r=60*scale;
  double delx=2/scale;      // length unit per pixel
  int Pr=5;               // particle distribution properties
  double particles_fraction=0.0;
  
  // model parameters
  double delt=0.1;
  int timesteps1=10;
  int timesteps=10001;
  int writingtimestep=100;
  double L=1;
  double alpha=1.0 ;
  double beta=1.0 ;
  double gamma=1.5;
  double kappa=2.0;
  double epsilon=2*0.0;
  double G[2];
  G[0]=0.0;
  G[1]=0.0;

  int i,j,k,tn;
  double thresh=0.00001; //threshold value for choosing active nodes
  double *eta;
  eta= new double[mboxsize*nboxsize*kboxsize*p];
  double *eta2;
  eta2= new double[mboxsize*nboxsize*kboxsize*p];
  bool *mbool;
  mbool= new bool[mboxsize*nboxsize*kboxsize*p];
  double* phi;
  phi= new double[mboxsize*nboxsize*kboxsize];
  double vol;
  double sumterm,sumtermp;
  double detadtM;
  double detadt;
  int pn,psn,pind;
  double delx2=1/(delx*delx);
  int size3=mboxsize*nboxsize*kboxsize;
  int size2=mboxsize*nboxsize;
  int jn, kn, pnn;
  int inplus1, inminus1, jnplus1,jnminus1, knplus1, knminus1;
  double del2;
  //setting initial condition  (one interface on top of the domain)
  int initialpos=int(0.10*mboxsize);
  
  for (k=0;k<kboxsize;k++)
  {
    for (j=0;j<nboxsize;j++)
    {
      for (i=0;i<mboxsize;i++)
      {
        
//                 //initial condition for octahedron
//         if (i<(mboxsize/2+1) && j<(nboxsize/2+1) && k<(nboxsize/2+1)) 
//           eta[i+j*mboxsize+k*size2+size3*1]=0.99;
//         if (i<(mboxsize/2+1) && j>(nboxsize/2-1) && k<(nboxsize/2+1))
//           eta[i+j*mboxsize+k*size2+size3*2]=0.99;
//         if (i>(mboxsize/2-1) && j>(nboxsize/2-1) && k<(nboxsize/2+1))
//           eta[i+j*mboxsize+k*size2+size3*3]=0.99;
//         if (i>(mboxsize/2-1) && j<(nboxsize/2+1) && k<(nboxsize/2+1))
//           eta[i+j*mboxsize+k*size2+size3*4]=0.99;
//         if (i<(mboxsize/2+1) && j<(nboxsize/2+1) && k>(nboxsize/2-1))
//           eta[i+j*mboxsize+k*size2+size3*5]=0.99;
//         if (i<(mboxsize/2+1) && j>(nboxsize/2-1) && k>(nboxsize/2-1))
//           eta[i+j*mboxsize+k*size2+size3*6]=0.99;
//         if (i>(mboxsize/2-1) && j>(nboxsize/2-1) && k>(nboxsize/2-1))
//           eta[i+j*mboxsize+k*size2+size3*7]=0.99;
//         if (i>(mboxsize/2-1) && j<(nboxsize/2+1) && k>(nboxsize/2-1))
//           eta[i+j*mboxsize+k*size2+size3*8]=0.99;
//         if (((i-mboxsize/2)*(i-mboxsize/2)+(j-nboxsize/2)*(j-mboxsize/2)+(k-kboxsize/2)*(k-kboxsize/2))<r*r)
//         {
//           eta[i+j*mboxsize+k*size2]=1;
//           eta[i+j*mboxsize+k*size2+size3*1]=0;
//           eta[i+j*mboxsize+k*size2+size3*2]=0;
//           eta[i+j*mboxsize+k*size2+size3*3]=0;
//           eta[i+j*mboxsize+k*size2+size3*4]=0;
//           eta[i+j*mboxsize+k*size2+size3*5]=0;
//           eta[i+j*mboxsize+k*size2+size3*6]=0;
//           eta[i+j*mboxsize+k*size2+size3*7]=0;
//           eta[i+j*mboxsize+k*size2+size3*8]=0;
//         }
//         
//         
        
        
        if (((i-mboxsize/2)*(i-mboxsize/2)+(j-nboxsize/2)*(j-mboxsize/2)+(k-kboxsize/2)*(k-kboxsize/2))<r*r)
        {
          eta[i+j*mboxsize+k*mboxsize*nboxsize]=1;
          eta[i+j*mboxsize+k*mboxsize*nboxsize+1*mboxsize*nboxsize*kboxsize]=0;
        }
        else
        {
          eta[i+j*mboxsize+k*mboxsize*nboxsize]=0;
          eta[i+j*mboxsize+k*mboxsize*nboxsize+1*mboxsize*nboxsize*kboxsize]=1;
        }
        
        
      }
    }
  }
  // particles distribution specification
  double* ppf;
  //ppf=ParticleDistro(int particlesn, int Pr, int mboxsize, int nboxsize, int kboxsize);
  ppf=ParticleDistro(mboxsize,nboxsize, kboxsize, particles_fraction, Pr);
  double Pf=0; //actual particles vlocume fraction
  for (k=0;k<kboxsize;k++)
  {
    for (j=0;j<nboxsize;j++)
    {
      for (i=0;i<mboxsize;i++)
      {
        if (ppf[i+j*mboxsize+k*mboxsize*nboxsize]==1)
        {Pf=Pf+1;}
      }
    }
  }
  cout << "Actual particles volume fraction = " << Pf/size3 << endl;
  //dynamics
  //calculating processing time
  clock_t time1;
  time1=clock();
  cout << "Initialization ended." <<endl;
  //first loop over all the nodes to creates the obtimized matrix mbool
  for (tn=0;tn<timesteps1;tn++)
  {
    for (k=0;k<kboxsize;k++)
    {
      kn=k*size2;
      knplus1=peribc(k+1,kboxsize)*size2;
      knminus1=peribc(k-1,kboxsize)*size2;
      for (j=0;j<nboxsize;j++)
      {
        jn=j*mboxsize;
        jnplus1=peribc(j+1,mboxsize)*mboxsize;
        jnminus1=peribc(j-1,mboxsize)*mboxsize;
        for (i=0;i<mboxsize;i++)
        {
          inplus1=peribc(i+1,mboxsize);
          inminus1=peribc(i-1,mboxsize);
          // here is the sum of all order parameters^2 for the point i and j
          sumterm=0;
          for (psn=0;psn<p;psn++)
          {
            sumterm=sumterm+eta[i+jn+kn+psn*size3]*eta[i+jn+kn+psn*size3];
          }
          // calculation of nabla square eta
          for (pn=0;pn<p;pn++)
          {
            pnn=pn*size3;
            del2=delx2*((eta[inplus1+jn+kn+pnn]+eta[inminus1+jn+kn+pnn]+eta[i+jnplus1+kn+pnn]+eta[i+jnminus1+kn+pnn]+eta[i+jn+knplus1+pnn]+eta[i+jn+knminus1+pnn])-6*eta[i+jn+kn+pnn]);
            sumtermp=eta[i+jn+kn+pnn]*sumterm-pow(eta[i+jn+kn+pnn],3);
            detadtM=-alpha*eta[i+jn+kn+pnn]+beta*pow(eta[i+jn+kn+pnn],3)-kappa*del2;
            detadt=-L*(detadtM+2*gamma*sumtermp);
            //cout << ppf[i+jn+kn] << endl;
            if (fabs(detadt)>thresh) // optimization function
            {
              mbool[i+jn+kn+pnn]=true;
              mbool[inplus1+jn+kn+pnn]=true;
              mbool[inminus1+jn+kn+pnn]=true;
              mbool[i+jnplus1+kn+pnn]=true;
              mbool[i+jnminus1+kn+pnn]=true;
              mbool[i+jn+knplus1+pnn]=true;
              mbool[i+jn+knminus1+pnn]=true;
            }
            else
            {
              mbool[i+jn+kn+pnn]=false; 
            }
            eta2[i+jn+kn+pnn]=eta[i+jn+kn+pnn]+delt*detadt;
            // to make sure eta is not outside the equilibrium values. This increases stability of calculation by controlling bounds of the eta whithin equilibrium values
            if (eta2[i+jn+kn+pnn]>1) eta2[i+jn+kn+pnn]=1;
            if (eta2[i+jn+kn+pnn]<0) eta2[i+jn+kn+pnn]=0;
          }
        }
      }
    }
    //setting eta equal to the new eta2 for the next time step
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
    cout << tn << "\n";
  }
  cout << "time required for 10 time steps:" << double((clock()-time1))/double(CLOCKS_PER_SEC) << "seconds. \n";
  //optimized loop -----------------------------------------------------------------------------------------------------
  tn=0;
  vol=etavolume(eta,mboxsize, nboxsize, kboxsize);
  cout << "Initial volume is:" << vol*delx*delx*delx <<endl;
  ofstream volfile; //file containing volume data logs
  int nvol;
  char volfilename[200];
  sprintf (volfilename, "%sGrainVol.txt",dirstr);
  volfile.open (volfilename);
  
  while (vol>50)
  {
    tn=tn+1;
    time1=clock();
    for (k=0;k<kboxsize;k++)
    {
      kn=k*size2;
      knplus1=peribc(k+1,kboxsize)*size2;
      knminus1=peribc(k-1,kboxsize)*size2;
      for (j=0;j<nboxsize;j++)
      {
        jn=j*mboxsize;
        jnplus1=peribc(j+1,mboxsize)*mboxsize;
        jnminus1=peribc(j-1,mboxsize)*mboxsize;
        for (i=0;i<mboxsize;i++)
        {
          for (pn=0;pn<p;pn++)
          {
            pnn=pn*size3;
            if (2<4) //mbool[i+jn+kn+pnn]==true
            {
              inplus1=peribc(i+1,mboxsize);
              inminus1=peribc(i-1,mboxsize);
              // here is the sum of all order parameters^2 for the point i and j
              sumterm=0;
              for (psn=0;psn<p;psn++)
              {
                sumterm=sumterm+eta[i+jn+kn+psn*size3]*eta[i+jn+kn+psn*size3];
              }
              // calculation of nabla square eta
              del2=delx2*((eta[inplus1+jn+kn+pnn]+eta[inminus1+jn+kn+pnn]+eta[i+jnplus1+kn+pnn]+eta[i+jnminus1+kn+pnn]+eta[i+jn+knplus1+pnn]+eta[i+jn+knminus1+pnn])
              -6*eta[i+jn+kn+pnn]);
              sumtermp=eta[i+jn+kn+pnn]*sumterm-pow(eta[i+jn+kn+pnn],3);
              detadtM=-alpha*eta[i+jn+kn+pnn]+beta*pow(eta[i+jn+kn+pnn],3)-kappa*del2;
              detadt=-L*(detadtM+2*gamma*sumtermp);
              if (fabs(detadt)>thresh) // optimization function
              {
                mbool[i+jn+kn+pnn]=true;
                mbool[inplus1+jn+kn+pnn]=true;
                mbool[inminus1+jn+kn+pnn]=true;
                mbool[i+jnplus1+kn+pnn]=true;
                mbool[i+jnminus1+kn+pnn]=true;
                mbool[i+jn+knplus1+pnn]=true;
                mbool[i+jn+knminus1+pnn]=true;
              }
              else
              {
                mbool[i+jn+kn+pnn]=false; 
              }
              eta2[i+jn+kn+pnn]=eta[i+jn+kn+pnn]+delt*detadt;
              // to make sure eta is not outside the equilibrium values. This increases stability of calculation by controlling bounds of the eta whithin equilibrium values
              if (eta2[i+jn+kn+pnn]>1) eta2[i+jn+kn+pnn]=1;
              if (eta2[i+jn+kn+pnn]<0) eta2[i+jn+kn+pnn]=0;
            }
          }
        }
      }
    }
    //setting eta equal to the new eta2 for the next time step
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
   vol=etavolume(eta,mboxsize, nboxsize, kboxsize);
    cout << "The volume at " << tn << " is: " << vol*delx*delx*delx <<endl;
    volfile <<tn*delt <<"    " <<vol*delx*delx*delx << "        " << 1 <<endl;
    // write array into a file each 100 time steps
    if  (tn % writingtimestep ==0)
    {
      phi=calculatephi(eta,phi, mboxsize,nboxsize,kboxsize,p);
      int n;
      char filename[200];
      // writing
      ofstream myfile2;
      // make a string like "result_5.txt"
      sprintf (filename, "%sFullres_%d.txt",dirstr, tn);
      myfile2.open (filename);
      for (k=0;k<kboxsize;k++)
      {
        kn=k*size2;
        for (j=0;j<nboxsize;j++)
        {
          jn=j*mboxsize;
          for (i=0;i<mboxsize;i++)
          {
            myfile2 << phi[i+jn+kn] << "        "; 
          }
          myfile2 << endl;
        }
      }
      myfile2.close();      
    }
  }
  vol=etavolume(eta,mboxsize, nboxsize, kboxsize); //final volume
  cout << "Final volume is:" << vol*delx*delx*delx << endl;
  return 0;
}