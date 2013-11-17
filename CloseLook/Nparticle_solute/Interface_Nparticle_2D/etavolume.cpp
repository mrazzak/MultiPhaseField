//calculates volume of the first order parameter in the eta matrix
double etavolume(double* eta,int mboxsize, int nboxsize)
{
  int i, j, jn;
  double vol=0.00;
  

    for (j=0;j<nboxsize;j++)
    {
      jn=j*mboxsize;
      for (i=0;i<mboxsize;i++)
      {
        vol=vol+eta[i+jn];
      }
    }
  return vol;
}