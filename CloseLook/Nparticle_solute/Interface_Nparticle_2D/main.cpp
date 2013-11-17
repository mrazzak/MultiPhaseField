// 3D simulation with a optimized calculations
// for an interface moving in array of particles
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <time.h>
#include <sstream>
#include <string>

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
// geometry settings

// functions
int peribc();
int symbc();
double* shpere();
double* ParticleDistro();
double* calculatephi();
double etavolume();
int WriteResults();

int main()
{
  
  ifstream infile;
  infile.open ("input.txt");
  if (infile.is_open())
  {
    cout << "Successfully opened the file";
  }
  else
  {
    cout << "Please write file address to the input file. Directory name should be with no space, e.g. /mysimulation/input.txt  :  " << endl;
    char dirinput[300];
    cin >> dirinput;
    infile.open (dirinput);
    if (infile.is_open()==0) {
      cout << "I could not open the file specified. The program can not continue.";
      exit(1);
    }
  }
  string aline;
  getline( infile, aline);   getline( infile, aline);//header line
  cout <<  "Reading input.txt file:";
  getline( infile, aline);
  cout << aline ; //Reading and saving directory:
  getline( infile, aline);
  stringstream value(aline);
  char dirstr[200];
  value >>dirstr;
  cout <<"Data will be saved in this Directory:---->" << dirstr <<endl;
  // geometry settings
  int scale=1;
  int mboxsize=100*scale; // x axis in pixels
  int nboxsize=100*scale; // y axis
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 7
  stringstream(aline)>> mboxsize;
  cout <<mboxsize <<endl;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 10
  stringstream(aline)>> nboxsize;
  cout <<nboxsize <<endl;
  double delx;      // length unit per pixel
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 13
  stringstream(aline)>> delx;
  delx=delx/scale;
  cout <<delx <<endl;
  int p=2; // depth of indexed matrix.
  double thresh; //threshold value for choosing active nodes
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 16
  stringstream(aline)>> thresh;
  cout <<thresh <<endl;
  
  // model parameters
  double delt;
  getline( infile, aline); getline( infile, aline);
  cout << aline;
  getline( infile, aline); // 19
  stringstream(aline)>> delt;
  cout <<delt <<endl;
  
  // number of time steps for first calculation without boolean matrix.
  int timesteps1=100;
  
  int timesteps;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 22
  stringstream(aline)>> timesteps;
  cout <<timesteps <<endl;
  
  int writingtimesteps;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 25
  stringstream(aline)>> writingtimesteps;
  cout <<writingtimesteps <<endl;
  
  double L;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 28
  stringstream(aline)>> L;
  cout <<L <<endl;
  double m;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 31
  stringstream(aline)>> m;
  cout <<m <<endl;
  
  double gamma=2*1.5*m;
  double kappa;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 34
  stringstream(aline)>> kappa;
  cout <<kappa <<endl;
  
  // number of nucleas at the beginning of simulation
  double particles_fraction;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 37
  stringstream(aline)>>particles_fraction ;
  cout << particles_fraction <<endl;
  
  double G[2];
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 40
  stringstream(aline)>> G[1];
  cout << G[1] <<endl;
  G[2]=-G[1];
  
  double gii;
  int Pr;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 43
  stringstream(aline)>> Pr;
  cout << Pr <<endl;
  
  double asol;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 40
  stringstream(aline)>> asol;
  cout << asol <<endl;
  
  double bsol;
  getline( infile, aline); getline( infile, aline);
  cout << aline ;
  getline( infile, aline); // 40
  stringstream(aline)>> bsol;
  cout << bsol <<endl;
  
  // model parameters
  double alpha=m ;
  double beta=m;
  double epsilon=2*5.0;
  
  int i,j,tn;
  double *eta;
  eta= new double[mboxsize*nboxsize*p];
  double *eta2;
  eta2= new double[mboxsize*nboxsize*p];
  bool *mbool;
  mbool= new bool[mboxsize*nboxsize*p];
  double* phi;
  phi= new double[mboxsize*nboxsize];
  
  double* Mvel;
  Mvel=new double[mboxsize*nboxsize*p];
  
  double vol;
  double sumterm,sumtermp;
  double detadtM;
  double detadt;
  int pn,psn,pind;
  double delx2=2.0/3.0/(delx*delx);
  double delxgrad=1/(2*delx);
  double veloc;
  double gradx,grady,grad;
  double error, detadtpast;
  int itr, averageitr=0;
  
  int size2=mboxsize*nboxsize;
  int jn, pnn;
  int inplus1, inminus1, jnplus1,jnminus1;
  double del2;
  //initialize everything to zero
  for (j=0;j<nboxsize;j++)
  {
    jn=j*mboxsize;
    for (i=0;i<mboxsize;i++)
    {
      for (pnn=0;pnn<p;pnn++)
      {
        eta[i+j*mboxsize+pnn*size2]=0;
        eta2[i+j*mboxsize+pnn*size2]=0;
      }
    }
  }
  //setting initial condition  (one interface on top of the domain)
  int initialpos=int(0.10*mboxsize);
  for (j=0;j<nboxsize;j++)
  {
    for (i=0;i<mboxsize;i++)
    {
      if (i<initialpos)
      {
        eta[i+j*mboxsize]=1;
        eta[i+j*mboxsize+1*mboxsize*nboxsize]=0;
      }
      else
      {
        eta[i+j*mboxsize]=0;
        eta[i+j*mboxsize+1*mboxsize*nboxsize]=1;
      }
    }
  }
  
  // particles distribution specification
  double* ppf;
  ppf=ParticleDistro(mboxsize,nboxsize, particles_fraction, Pr);
  double Pf=0; //actual particles vlocume fraction
  
  for (j=0;j<nboxsize;j++)
  {
    for (i=0;i<mboxsize;i++)
    {
      if (ppf[i+j*mboxsize]==1)
      {Pf=Pf+1;}
    }
  }
  cout << "Actual particles volume fraction = " << Pf/size2 << endl;
  //dynamics
  //calculating processing time
  clock_t time1;
  time1=clock();
  cout << "Initialization ended." <<endl;
  //first loop over all the nodes to creates the obtimized matrix mbool
  for (tn=0;tn<timesteps1;tn++)
  {
    for (j=0;j<nboxsize;j++)
    {
      jn=j*mboxsize;
      jnplus1=symbc(j+1,nboxsize)*mboxsize;
      jnminus1=symbc(j-1,nboxsize)*mboxsize;
      for (i=0;i<mboxsize;i++)
      {
        inplus1=symbc(i+1,mboxsize);
        inminus1=symbc(i-1,mboxsize);
        // here is the sum of all order parameters^2 for the point i and j
        sumterm=0;
        for (psn=0;psn<p;psn++)
        {
          sumterm=sumterm+eta[i+jn+psn*size2]*eta[i+jn+psn*size2];
        }
        // calculation of nabla square eta
        for (pn=0;pn<p;pn++)
        {
          pnn=pn*size2;
          del2=delx2*((eta[inplus1+jn+pnn]+eta[inminus1+jn+pnn]+eta[i+jnplus1+pnn]+eta[i+jnminus1+pnn])-4*eta[i+jn+pnn]);
          sumtermp=eta[i+jn+pnn]*sumterm-eta[i+jn+pnn]*eta[i+jn+pnn]*eta[i+jn+pnn];
          detadtM=-alpha*eta[i+jn+pnn]+beta*eta[i+jn+pnn]*eta[i+jn+pnn]*eta[i+jn+pnn]-kappa*del2;
          detadt=-L*(detadtM+2*gamma*sumtermp+epsilon*eta[i+jn+pnn]*ppf[i+jn]);
          eta2[i+jn+pnn]=eta[i+jn+pnn]+delt*detadt;
          if (eta2[i+jn+pnn]>thresh && eta2[i+jn+pnn]<(1-thresh)) // optimization function
            {
              mbool[i+jn+pnn]=true;
              mbool[inplus1+jn+pnn]=true;
              mbool[inminus1+jn+pnn]=true;
              mbool[i+jnplus1+pnn]=true;
              mbool[i+jnminus1+pnn]=true;
            }
            else
            {
              mbool[i+jn+pnn]=false; 
            }
            // to make sure eta is not outside the equilibrium values. This increases stability of calculation by controlling bounds of the eta whithin equilibrium values
            if (eta2[i+jn+pnn]>1) eta2[i+jn+pnn]=1;
            if (eta2[i+jn+pnn]<0) eta2[i+jn+pnn]=0;
        }
      }
    }
    
    //setting eta equal to the new eta2 for the next time step
    for (pind=0;pind<p;pind++)
    {
      pnn=pind*size2;
      for (j=0;j<nboxsize;j++)
      {
        jn=j*mboxsize;
        for (i=0;i<mboxsize;i++)
        {
          eta[i+jn+pnn]=eta2[i+jn+pnn];
        }
      }
      
    }
    cout << tn << "\n";
  }
  cout << "time required for 10 time steps:" << double((clock()-time1))/double(CLOCKS_PER_SEC) << "seconds. \n";
  //optimized loop -----------------------------------------------------------------------------------------------------
  tn=0;
  vol=etavolume(eta,mboxsize, nboxsize);
  cout << "Initial volume is:" << vol*delx*delx <<endl;
  ofstream volfile; //file containing volume data logs
  int nvol;
  char volfilename[200];
  nvol=sprintf(volfilename,"%sVollog.log",dirstr);
  volfile.open (volfilename);
  
  while (vol<(0.95*size2-Pf))
  {
    tn=tn+1;
    time1=clock();
    for (j=0;j<nboxsize;j++)
    {
      jn=j*mboxsize;
      jnplus1=symbc(j+1,nboxsize)*mboxsize;
      jnminus1=symbc(j-1,nboxsize)*mboxsize;
      for (i=0;i<mboxsize;i++)
      {
        for (pn=0;pn<p;pn++)
        {
          pnn=pn*size2;
          if (mbool[i+jn+pnn]==true)
          {
            inplus1=symbc(i+1,mboxsize);
            inminus1=symbc(i-1,mboxsize);
            // here is the sum of all order parameters^2 for the point i and j
            sumterm=0;
            for (psn=0;psn<p;psn++)
            {
              sumterm=sumterm+eta[i+jn+psn*size2]*eta[i+jn+psn*size2];
            }
            del2=delx2*((eta[inplus1+jn+pnn]+eta[inminus1+jn+pnn]+eta[i+jnplus1+pnn]+eta[i+jnminus1+pnn])
            +0.25*(eta[inplus1+jnplus1+pnn]+eta[inminus1+jnminus1+pnn]+eta[inminus1+jnplus1+pnn]+eta[inplus1+jnminus1+pnn])
            -5*eta[i+jn+pnn]);
            sumtermp=eta[i+jn+pnn]*sumterm-eta[i+jn+pnn]*eta[i+jn+pnn]*eta[i+jn+pnn];
            detadtM=-alpha*eta[i+jn+pnn]+beta*eta[i+jn+pnn]*eta[i+jn+pnn]*eta[i+jn+pnn]-kappa*del2;
            grad=delxgrad*sqrt((eta[inplus1+jn+pnn]-eta[inminus1+jn+pnn])*(eta[inplus1+jn+pnn]-eta[inminus1+jn+pnn])
                 +(eta[i+jnplus1+pnn]-eta[i+jnminus1+pnn])*(eta[i+jnplus1+pnn]-eta[i+jnminus1+pnn]));
            Mvel[i+jn+pnn]=0;
            error=1234567890;
            itr=0;
            while (error>0.00001 and itr<50){
              detadtpast=Mvel[i+jn+pnn];
              veloc=Mvel[i+jn+pnn]/grad;
              if (fabs(grad)<0.00005){
                veloc=0;
              }
              Pf=asol*veloc/(1+bsol*veloc*veloc);
              detadt=-L*(detadtM+2*gamma*sumtermp+epsilon*eta[i+jn+pnn]*ppf[i+jn]+3*(eta[i+jn+pnn]-eta[i+jn+pnn]*eta[i+jn+pnn])*G[pn]
                      +3*eta[i+jn+pnn]*(1-eta[i+jn+pnn])*(Pf));
              Mvel[i+jn+pnn]=detadt; // detadt for next time step
              error=fabs(detadtpast-Mvel[i+jn+pnn]);
              itr++;
              averageitr++;
       
            }

            eta2[i+jn+pnn]=eta[i+jn+pnn]+delt*detadt;
            if (eta2[i+jn+pnn]>thresh && eta2[i+jn+pnn]<(1-thresh)) // optimization function
            {
              mbool[i+jn+pnn]=true;
              mbool[inplus1+jn+pnn]=true;
              mbool[inminus1+jn+pnn]=true;
              mbool[i+jnplus1+pnn]=true;
              mbool[i+jnminus1+pnn]=true;
            }
            else
            {
              mbool[i+jn+pnn]=false; 
            }
            
            // to make sure eta is not outside the equilibrium values. This increases stability of calculation by controlling bounds of the eta whithin equilibrium values
            if (eta2[i+jn+pnn]>1) eta2[i+jn+pnn]=1;
            if (eta2[i+jn+pnn]<0) eta2[i+jn+pnn]=0;
          }
        }
      }
    }
    //setting eta equal to the new eta2 for the next time step
    for (pind=0;pind<p;pind++)
    {
      pnn=pind*size2;
      for (j=0;j<nboxsize;j++)
      {
        jn=j*mboxsize;
        for (i=0;i<mboxsize;i++)
        {
          eta[i+jn+pnn]=eta2[i+jn+pnn];
        }
      }
    }
    vol=etavolume(eta,mboxsize, nboxsize);
    volfile <<tn*delt <<"    " <<vol*delx*delx <<endl;
    // write array into a file each 100 time steps
    if  (tn % 500 ==0)
    {
      cout << "Time required for time step number [" << tn << " ] is :" << double(clock()-time1)/double(CLOCKS_PER_SEC) << " seconds. \n";
      cout << "The volume is:" << vol*delx*delx <<endl;
      phi=calculatephi(eta,phi, mboxsize,nboxsize,p);
      char filename[200];
      // writing
      ofstream myfile2;
      // make a string like "result_5.txt"
      sprintf (filename, "%sFullres_%d.txt", dirstr,tn);
      myfile2.open (filename);
      
      for (j=0;j<nboxsize;j++)
      {
        jn=j*mboxsize;
        for (i=0;i<mboxsize;i++)
        {
          myfile2 << phi[i+jn] << " "; 
        }
        myfile2 << endl;
      }
      
      myfile2.close();      
    }
  }
  vol=etavolume(eta,mboxsize, nboxsize); //final volume
  cout << "Final volume is:" << vol*delx*delx << endl;
  return 0;
}
