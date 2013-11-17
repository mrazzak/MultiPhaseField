 
 // <source lang=cpp>
 
 // 3D simulation with a optimized calculations
 #include <fstream>
 #include <stdio.h>
 #include <stdlib.h>
 #include <iostream>
 #include <cstdlib>
 #include <math.h>
 #include <time.h>
 
 
 #include "peribc.cpp"
 
 using namespace std;
 
 int main(int a, char** charinput)
 {
   char* dirstr;
   dirstr=charinput[1];
   cout <<"Data will be saved in this Directory:---->" << dirstr <<endl;
   int R;
   R=atoi(charinput[2]);
   cout << "The radius of big sphere is = " << R <<"  ----" <<endl;
   
   // geometry settings
   int mboxsize=50; // x axis in pixels
   int nboxsize=50; // y axis
   int kboxsize=50;
   double delx=1;      // length unit per pixel
   int size3=mboxsize*nboxsize*kboxsize;
   int size2=mboxsize*nboxsize;
   int i,j,k,jn,kn,nn,ii,jj,kk;
   double irand,jrand,prand, krand;
   
   int *inds;
   inds= new int[mboxsize*nboxsize*kboxsize];
   for (i=0; i<mboxsize*nboxsize*kboxsize;i++){
     inds[i]=0;
   }
   // number of nucleas at the beginning of simulation
   int nuclein;
   nuclein=int(mboxsize*nboxsize*kboxsize/1000); // ~0.1 percent of grid points are nuclei
   cout << "step 1:before nucleation" <<endl;
   
   for (nn=2;nn<nuclein+1;nn++){
     irand=rand();
     jrand=rand();
     krand=rand();
     ii=int((nboxsize*irand)/RAND_MAX);
     jj=int((mboxsize*jrand)/RAND_MAX);
     kk=int((kboxsize*krand)/RAND_MAX);
     inds[ii+jj*mboxsize+kk*size2]=nn;
   }
   
   // putting the large grain in the center
   for (k=0;k<kboxsize;k++)
   {
     kn=k*size2;
     for (j=0;j<nboxsize;j++)
     {
       jn=j*mboxsize;
       for (i=0;i<mboxsize;i++)
       {
         if ((i-mboxsize/2)*(i-mboxsize/2)+(j-nboxsize/2)*(j-nboxsize/2)+(k-kboxsize/2)*(k-kboxsize/2)<R*R)
         {
           inds[i+jn+kn]=1;
         }
       }
     }
   }
   int tn=1;
   // writing indexed matrix
   char filename3[200];
   ofstream myfile3;
   // make a string like "result_5.txt"
   sprintf (filename3, "%sInds_%d.txt",dirstr, tn);
   myfile3.open (filename3);
   for (k=0;k<kboxsize;k++)
   {
     kn=k*size2;
     for (j=0;j<nboxsize;j++)
     {
       jn=j*mboxsize;
       for (i=0;i<mboxsize;i++)
       {
         myfile3 << inds[i+jn+kn] << "   ";
         
       }
       myfile3 << endl;
     }
   }
   myfile3.close();
   
   cout << "Inital Structure has been saved." <<endl;
   return 0;
 }