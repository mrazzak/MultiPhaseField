// A circular shape grain moving with curvature and pinning force also acting on it.
//parameters :  a.out [dome radius] [friction force]
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
#include "createcircle.cpp"

using namespace std;
int symbc();
int WriteResults();
double* createcircle();
double etavolume();

double sign(double x){
  if (x>0){return 1;}
  if (x<0){return -1;}
  if (x=0){return 0;}
  return 0;
}

// ------- main routin --------
int main(int a, char** charinput)
{
  ofstream myfile ;
  // model parameters
  double delt=0.05;
  int timesteps1=301;
  
  double L=1;
  double alpha=2 ;
  double beta=2 ;
  double gamma=2*1.5;
  double kappa=4;
  
  double Lf;
  double Pz;
  Pz=double(atoi(charinput[2]))/1000;
  cout << "The friction force is = " << Pz <<"  ----" <<endl;
  
  int i,j,tn;
  // geometry settings
  // geometry settings
  int scale=2;
  int r; //dome radius
  r= atoi(charinput[1]);
  cout << "The dome radius is = " << r <<"  ----" <<endl;
  int p=2; // phase field numbers
  int mboxsize=10*scale+2*r; // x axis in pixels
  int nboxsize=10*scale+2*r; // y axis
  double delx=2/scale;      // length unit per pixel
  
  double thresh=0.00000000001; //threshold value for choosing active nodes
  double* eta;
  eta= new double[mboxsize*nboxsize*p];
  double *eta2;
  eta2= new double[mboxsize*nboxsize*p];
  int nn,ii,jj;
  int inplus1, inminus1, jnplus1,jnminus1, knplus1, knminus1;
  double irand,jrand,prand;
  
  eta=createcircle(eta, mboxsize, nboxsize,r);
  // particles distribution specification
  
  //dynamics
  double sumterm,sumtermp;
  double detadtM;
  double detadt;
  int pn,psn,pind;
  double delx2=2.0/3.0/(delx*delx);
  int size=mboxsize*nboxsize;
  int jn, pnn;
  double del2, pzi;
  
  //calculating processing time
  clock_t time1;
  time1=clock();
  
  for (tn=1;tn<timesteps1;tn++)
  {
    time1=clock();
    #pragma omp parallel for
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
          // simple del^2:: del2=delx2*((eta[inplus1+jn+pnn]+eta[inminus1+jn+pnn]+eta[i+jnplus1+pnn]+eta[i+jnminus1+pnn])-4*eta[i+jn+pnn]);
          del2=delx2*((eta[inplus1+jn+pnn]+eta[inminus1+jn+pnn]+eta[i+jnplus1+pnn]+eta[i+jnminus1+pnn])
          +0.25*(eta[inplus1+jnplus1+pnn]+eta[inminus1+jnminus1+pnn]+eta[inminus1+jnplus1+pnn]+eta[inplus1+jnminus1+pnn])
          -5*eta[i+jn+pnn]);
          sumtermp=eta[i+jn+pnn]*sumterm-pow(eta[i+jn+pnn],3);
          detadtM=-alpha*eta[i+jn+pnn]+beta*pow(eta[i+jn+pnn],3)-kappa*del2;
          detadt=-L*(detadtM+2*gamma*sumtermp);
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
  //cout << "time required for 300 time steps:" << double((clock()-time1))/double(CLOCKS_PER_SEC) << "seconds. \n";
  //optimized loop -----------------------------------------------------------------------------------------------------
  ofstream volfile;
  // make a string like "result_5.txt"
  int n;
  char filename[200];
  n=sprintf (filename,"/media/disk/sim_res/Circ_Friction_cpp/onset/vollog_%d.log", int(r*delx));
  volfile.open (filename);
  double vol=mboxsize*nboxsize, initvol;
  double pastvol=vol;
  bool ismoving=false;
  initvol=etavolume(eta,mboxsize, nboxsize)*delx*delx;
  tn=0;
  char stn[50];
  while (vol>100)
  {
    tn=tn+1;
    vol=etavolume(eta,mboxsize, nboxsize)*delx*delx;
      n=sprintf(stn,"%.2f %.6f %.5f",tn*delt,vol,Pz);
      volfile << stn << endl;
    //#pragma omp parallel for
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
          sumterm=0;// here is the sum of all order parameters^2 for the point i and j
          for (psn=0;psn<p;psn++)
          {
            sumterm=sumterm+eta[i+jn+psn*size]*eta[i+jn+psn*size];
          }
          //del2=delx2*((eta[inplus1+jn+pnn]+eta[inminus1+jn+pnn]+eta[i+jnplus1+pnn]+eta[i+jnminus1+pnn])-4*eta[i+jn+pnn]);
          del2=delx2*((eta[inplus1+jn+pnn]+eta[inminus1+jn+pnn]+eta[i+jnplus1+pnn]+eta[i+jnminus1+pnn])
          +0.25*(eta[inplus1+jnplus1+pnn]+eta[inminus1+jnminus1+pnn]+eta[inminus1+jnplus1+pnn]+eta[inplus1+jnminus1+pnn])
          -5*eta[i+jn+pnn]);
          sumtermp=eta[i+jn+pnn]*sumterm-(eta[i+jn+pnn]*eta[i+jn+pnn]*eta[i+jn+pnn]);
          detadtM=-alpha*eta[i+jn+pnn]+beta*(eta[i+jn+pnn]*eta[i+jn+pnn]*eta[i+jn+pnn])-kappa*del2+2*gamma*sumtermp;
          pzi=3*eta[i+jn+pnn]*(1-eta[i+jn+pnn])*sign(detadtM)*Pz;
          if (fabs(detadtM)<fabs(pzi)){
            Lf=0;
          }
          else{
            Lf=L;
          }
          detadt=-Lf*(detadtM-pzi);
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
    // write array into a file each 100 time steps
    if  (tn % 100000 ==0)
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
      n=sprintf (filename, "/media/disk/sim_res/Circ_Friction_cpp/onset/Pz%d/%d/result_%d.txt",60, r, tn);
      cout << filename << "\n";
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
    }
    
    if  (tn % 50 ==0)
    {
      if ((pastvol-vol)<1){ //so it is not moving then decrease Pz so it will start to move next time.
        //cout << "Interface is not moving!! for Pz=" <<Pz << " and r =" << sqrt(vol/3.1415) << endl;
        Pz=Pz-.00005*Pz;
      }
      else{ismoving=true;}
      pastvol=vol;
    }
  }
  volfile.close();
  cout << "Calculation for r=" << r << " is finished." << endl;
  cout << "Final Pz was" << Pz << endl; 
  return 0;
}

