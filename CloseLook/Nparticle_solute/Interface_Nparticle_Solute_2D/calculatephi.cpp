
double* calculatephi(double* eta, double* phi, int mboxsize,int nboxsize, int p)
{
   int i, j, jn, pnn, pind;
   int size2=mboxsize*nboxsize;
      // making the phi array
        for (j=0;j<nboxsize;j++)
        {
          jn=j*mboxsize;
          for (i=0;i<mboxsize;i++)
          {
            phi[i+jn]=0;
            for (pind=0;pind<p;pind++)
            {
              pnn=pind*size2;
              phi[i+jn]=phi[i+jn]+eta[i+jn+pnn]*eta[i+jn+pnn];
            }
          }
        }

return phi;
}
