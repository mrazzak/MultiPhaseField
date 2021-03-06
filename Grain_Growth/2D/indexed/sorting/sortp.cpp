 
// find 6 largest eta from the claculation list and assign them back to eta and inds matrix
int sortp(double* maxetas,int* maxinds,double* eta2, int* inds2,int size2, int p,int i, int jn)
{
  int ni,ii,maxi;
  double maxeta;
  
  for (ni=0;ni<p;ni++)
  {
    maxeta=maxetas[0];
    maxi=0;
    for (ii=0;ii<10;ii++)
    {
      if (maxetas[ii]>maxeta)
      {
        maxeta=maxetas[ii];
        maxi=ii;
      }
    }
    //so we set vaule of maximum eta in the maxetas to -1 which is smaller than all of the rest so next time second maximum wil be selected
    maxetas[maxi]=-1; 
    eta2[i+jn+ni*size2]=maxeta; //output
    inds2[i+jn+ni*size2]=maxinds[maxi];
  }
  for (ii=0;ii<10;ii++)
    {
      maxetas[ii]=0;
    }
  return 0;
}
