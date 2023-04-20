#include "AtlasStyle.C"
#include "AtlasLabels.C"
#include "util/TtTRFUtil.C"
#include "GetEffexpost.C"

struct jetprob {
  double x0,x1,x2;
} ;


 jetprob guessxif01(double f60, double f61, double k0, double k1, double k2, bool is4b=false){
     jetprob res;
    
     double mk3 = (-1 + k0 + k1 + k2);

    if (is4b){
      // assume f60 -> g61, f61 -> g62
      double g61=f60, g62=f61; // to remember what we are assuming
      res.x0=(g61*k1*(k1 + k2))/\
        (g61*(k1 - k2)*mk3 + \
          k1*(k1 + g62*mk3));
      res.x1=((k1 + k2)*(g62*k1 - g61*k2))/\
        (g61*(k1 - k2)*mk3 + \
          k1*(k1 + g62*mk3));
    }
    else {
      res.x0=(f60*k0*(k0 + k1 + k2))/  \
       (f60*(k0 - k1)*mk3 + \
         k0*(k0 + f61*mk3));
      res.x1=((f61*k0 - f60*k1)*(k0 + k1 + k2))/   \
       (f60*(k0 - k1)*mk3 +  \
         k0*(k0 + f61*mk3));
    }



     res.x2=1.0-res.x1 -res.x0;  // no check is done about x2!!
     return res;
 }

 jetprob guessxif12(double f61, double f62, double k0, double k1, double k2, bool is4b=false){
     jetprob res;

     double mk3 = (-1 + k0 + k1 + k2);

     if (is4b){
       // assume f61 -> g62, f62 -> g63
       double g62=f61, g63=f62;
       res.x0=(k1*((-1 + g63)*k1 + g63*k2) - \
           g62*(pow(k2,2) + k1*(-1 + k0 + k2)))/\
          (k1 - k1*(k0 + 2*k1) - pow(k2,2) + \
            g63*(k1 - k2)*mk3 - \
            g62*mk3*(-1 + k0 + k1 + 2*k2));
        res.x1=(k2*((-1 + g63)*k1 + g63*k2) + \
            g62*((-1 + k0)*k2 + pow(k1 + k2,2)))/\
          (-k1 + k1*(k0 + 2*k1) + pow(k2,2) - \
            g63*(k1 - k2)*mk3 + \
            g62*mk3*(-1 + k0 + k1 + 2*k2));
     }
     else {
     res.x0=(k0*((-1 + f62)*k0 + f62*(k1 + k2)) -  \
        f61*(k0*(-1 + k1) + k1*(k1 + k2)))/ \
      (-pow(k0,2) - k1*(f61*(-1 + k1) + k1) - f61*k2 + \
         f61*pow(k2,2) + f62*(k0 - k1)*mk3 + \
        k0*(k1 - f61*k1 + k2 + f61*k2));
     res.x1=(-(k1*((-1 + f62)*k0 + f62*(k1 + k2))) + \
        f61*(k0*(-1 + k2) + k2*(k1 + k2)))/ \
      (-pow(k0,2) - k1*(f61*(-1 + k1) + k1) - f61*k2 + \
        f61*pow(k2,2) + f62*(k0 - k1)*mk3 + \
        k0*(k1 - f61*k1 + k2 + f61*k2));
     }

     res.x2=1.0-res.x1 -res.x0;  // no check is done about x2!!

     return res;
 }

// double nonnormfracfrom5j (double *xx, double *p){
// }

double nonnormfrac (double *xx, double *p){

  double res=0;
  int nbtilde=(int) xx[0], njtilde=(int) xx[1], njtildefromwhere=((int) p[5])-2;
  double k[4]; 
  double x0=1-p[3]-p[4],x1=p[3],x2=p[4]; 

  k[0]=p[0]; k[1]=p[1]; k[2]=p[2]; k[3] = 1 - k[1] - k[2] - k[0]; 
  

  if ( nbtilde > njtilde || nbtilde < 0 || njtilde < njtildefromwhere) return 0;
  if ( k[3] < -0.00001  || k[3] >  1.00001 ) std::cout << "k3 unphysical: "<< k[3] <<  "." << std::endl;
  if ( x0 >  1.00001  || x0 < -0.00001 ) std::cout << "x0 unphysical." << std::endl;

  double xx0[3][2];
  if (njtilde==njtildefromwhere ){
    if (nbtilde < 4) res = k[nbtilde];
    else { 
      res = 0;
      std::cout << "WARNING in study_extrap: efficiency extraction region probably not yet implemented ..." << std::endl;
    }
  }
  else {
       for (int ib=0; ib<3; ib++) {
             xx0[ib][0]=nbtilde - ib; 
             xx0[ib][1]=njtilde - 1; 
       }
       res = x0 * nonnormfrac(xx0[0], p) 
           + x1 * nonnormfrac(xx0[1], p) 
           + x2 * nonnormfrac(xx0[2], p);
  }

  return res;
}

double nonnormfracge4b (double *xx, double *p){

  double res=0;
  int nbtilde=(int) xx[0], njtilde=(int) xx[1], njtildefromwhere=((int) p[5])-2;;
  double k[4]; 
  double x0=1-p[3]-p[4],x1=p[3],x2=p[4]; 

  k[0]=p[0]; k[1]=p[1]; k[2]=p[2]; k[3] = 1 - k[1] - k[2] - k[0]; 


  if ( nbtilde > njtilde || nbtilde < 1 || njtilde < njtildefromwhere) return 0;
  if ( k[3] < -0.00001   || k[3] > 1.00001 ) std::cout << "k3 unphysical: "<< k[3] <<  "." << std::endl;
  if ( x0 > 1.00001 || x0 < -0.00001   ) std::cout << "x0 unphysical." << std::endl;

  double xx0[3][2];
  if (njtilde==njtildefromwhere ){
    if (nbtilde < 4) res = k[nbtilde];
    else { 
      res = 0;
      std::cout << "WARNING in study_extrap: efficiency extraction region probably not yet implemented ..." << std::endl;
    }
    res/=1.0-k[0]; // this to retrieve -ge4b- fractions
  }
  else {
       for (int ib=0; ib<3; ib++) {
             xx0[ib][0]=nbtilde - ib; 
             xx0[ib][1]=njtilde - 1; 
       }
       res = x0 * nonnormfracge4b(xx0[0], p) \
           + x1 * nonnormfracge4b(xx0[1], p) \
           + x2 * nonnormfracge4b(xx0[2], p);
  }

  return res;
}

double frac (double *xx, double *p){
   // return all (nb,nj) fractions either in efficiency extraction or in source regions 
   double sum = 0, frac=0;
   int nbtilde=(int) xx[0], njtilde=(int) xx[1], njtildefromwhere=((int) p[5])-2;;
   if ( nbtilde > njtilde || nbtilde < 0 || njtilde < njtildefromwhere) return 0;

   double **xx0 = new double* [njtilde+1];  
   for (int ib=0; ib<=njtilde; ib++) {
     xx0[ib]=new double [2];
     xx0[ib][0]=ib;
     xx0[ib][1]=njtilde;
     sum+=nonnormfrac(xx0[ib], p);
   }

   if (sum>-0.000001 && sum < 1.000001) frac = nonnormfrac(xx0[nbtilde], p) / sum;
   else cout << "Non physical sum "<< sum << ", returning unnormalised. "<< endl;

   return frac;      
}

double fracge4b (double *xx, double *p){
   // return all (nb,nj) fractions either in efficiency extraction or in source regions 
   double sum = 0, frac=0;
   int nbtilde=(int) xx[0], njtilde=(int) xx[1], njtildefromwhere=((int) p[5])-2;;

   if ( nbtilde > njtilde || nbtilde < 1 || njtilde < njtildefromwhere) return 0;

   double **xx0 = new double* [njtilde+1];  
   for (int ib=1; ib<=njtilde; ib++) {
     xx0[ib]=new double [2];
     xx0[ib][0]=ib;
     xx0[ib][1]=njtilde;
     sum+=nonnormfracge4b(xx0[ib], p);
   }

   if (sum>-0.000001 && sum < 1.000001) frac = nonnormfracge4b(xx0[nbtilde], p) / sum;
   else cout << "Non physical sum "<< sum << ", returning unnormalised. "<< endl;
	
   return frac;      
}

/*
double efffrom5jge4b_correct_but_old(double *xx, double *p){
  double res=0,x=xx[0];
  double k0=p[0], k1=p[1], k2=p[2], k3 = 1.0 -k0-k1-k2,x0=1-p[3]-p[4],x1=p[3],x2=p[4]; // p[5];

  if ( k3 < -0.00001   || k3 > 1.00001 ) std::cout << "k3 unphysical: "<< k3 <<  "." << std::endl;
  if ( x0 > 1.00001 || x0 < -0.00001   ) std::cout << "x0 unphysical." << std::endl;

  double k0min1 = -1 + k0, k0k1min1=-1 + k0 + k1;
  if (x>=5.0 && x<6)       res = (2*k0k1min1 + k2)/(2.*k0min1);
  else if (x>=6.0 && x<7)  res = ((2*k0k1min1 + k2)*x0)/(3.*k0min1) + 
    ((-3 + 3*k0 + 2*k1 + k2)*x1)/(3.*k0min1) + 
    ((2*k1 + 3*k2)*x2)/(3 - 3*k0);
  else if (x>=7.0 && x<8)  res = ((2*k0k1min1 + k2)*pow(x0,2))/(4.*k0min1) + 
    ((-4 + 4*k0 + 2*k1 + k2)*pow(x1,2))/(4.*k0min1) + 
    ((3*k1 + 4*k2)*x1*x2)/(2 - 2*k0) + (k1*pow(x2,2))/(1 - k0) + 
    x0*(((-3 + 3*k0 + 2*k1 + k2)*x1)/(2.*k0min1) + 
       ((-4 + 4*k0 + 2*k1 + k2)*x2)/(2.*k0min1));
  else if (x>=8.0 && x<9)  res = ((2*k0k1min1 + k2)*pow(x0,3))/(5.*k0min1) + 
    ((-5 + 5*k0 + 2*k1 + k2)*pow(x1,3))/(5.*k0min1) + 
    ((12*k1 + 15*k2)*pow(x1,2)*x2)/(5 - 5*k0) - 
    (3*k1*x1*pow(x2,2))/k0min1 + 
    pow(x0,2)*((3*(-3 + 3*k0 + 2*k1 + k2)*x1)/(5.*k0min1) + 
       (3*(-4 + 4*k0 + 2*k1 + k2)*x2)/(5.*k0min1)) + 
    x0*((3*(-4 + 4*k0 + 2*k1 + k2)*pow(x1,2))/(5.*k0min1) + 
       (6*(-5 + 5*k0 + 2*k1 + k2)*x1*x2)/(5.*k0min1) + 
       ((12*k1 + 15*k2)*pow(x2,2))/(5 - 5*k0));
  else if (x>=9.0 && x<10)  res = ((2*k0k1min1 + k2)*pow(x0,4))/(6.*k0min1) + 
    ((-6 + 6*k0 + 2*k1 + k2)*pow(x1,4))/(6.*k0min1) + 
    ((10*k1 + 12*k2)*pow(x1,3)*x2)/(3 - 3*k0) - 
    (6*k1*pow(x1,2)*pow(x2,2))/k0min1 + 
    pow(x0,3)*((2*(-3 + 3*k0 + 2*k1 + k2)*x1)/(3.*k0min1) + 
       (2*(-4 + 4*k0 + 2*k1 + k2)*x2)/(3.*k0min1)) + 
    pow(x0,2)*(((-4 + 4*k0 + 2*k1 + k2)*pow(x1,2))/k0min1 + 
       (2*(-5 + 5*k0 + 2*k1 + k2)*x1*x2)/k0min1 + 
       ((-6 + 6*k0 + 2*k1 + k2)*pow(x2,2))/k0min1) + 
    x0*((2*(-5 + 5*k0 + 2*k1 + k2)*pow(x1,3))/(3.*k0min1) + 
       (2*(-6 + 6*k0 + 2*k1 + k2)*pow(x1,2)*x2)/k0min1 - 
       (2*(5*k1 + 6*k2)*x1*pow(x2,2))/k0min1 - 
       (4*k1*pow(x2,3))/k0min1);
  else if (x>=10.0 && x<11)  res = ((2*k0k1min1 + k2)*pow(x0,5))/(7.*k0min1) + 
    ((-7 + 7*k0 + 2*k1 + k2)*pow(x1,5))/(7.*k0min1) + 
    ((30*k1 + 35*k2)*pow(x1,4)*x2)/(7 - 7*k0) - 
    (10*k1*pow(x1,3)*pow(x2,2))/k0min1 + 
    pow(x0,4)*((5*(-3 + 3*k0 + 2*k1 + k2)*x1)/(7.*k0min1) + 
       (5*(-4 + 4*k0 + 2*k1 + k2)*x2)/(7.*k0min1)) + 
    pow(x0,3)*((10*(-4 + 4*k0 + 2*k1 + k2)*pow(x1,2))/
        (7.*k0min1) + (20*(-5 + 5*k0 + 2*k1 + k2)*x1*x2)/
        (7.*k0min1) + (10*(-6 + 6*k0 + 2*k1 + k2)*pow(x2,2))/
        (7.*k0min1)) + pow(x0,2)*
     ((10*(-5 + 5*k0 + 2*k1 + k2)*pow(x1,3))/(7.*k0min1) + 
       (30*(-6 + 6*k0 + 2*k1 + k2)*pow(x1,2)*x2)/(7.*k0min1) + 
       (30*(-7 + 7*k0 + 2*k1 + k2)*x1*pow(x2,2))/(7.*k0min1) + 
       ((60*k1 + 70*k2)*pow(x2,3))/(7 - 7*k0)) + 
    x0*((5*(-6 + 6*k0 + 2*k1 + k2)*pow(x1,4))/(7.*k0min1) + 
       (20*(-7 + 7*k0 + 2*k1 + k2)*pow(x1,3)*x2)/(7.*k0min1) - 
       (30*(6*k1 + 7*k2)*pow(x1,2)*pow(x2,2))/(7.*k0min1) - 
       (20*k1*x1*pow(x2,3))/k0min1);
    else res =0;

    return res;
}

*/

double nextfracfromprev (double *xx, double *p){
  double res=0,xb=xx[0];

 double xx1[2];
 if (xb<p[5]){ //eff extraction: return nj=6, nb = nb
   xx1[0]=xb;
   xx1[1]=p[5]-1;
 }
 else { 
    return 0;
 }
 return frac(xx1,p);
}

double nextfracfromprevge4b (double *xx, double *p){
  double res=0,xb=xx[0];

  double xx1[2];
  if (xb<p[5]){ //eff extraction: return nj=6, nb = nb
    xx1[0]=xb;
    xx1[1]=p[5]-1;
  }
  else { //convention of returing source regions: return nj=7,8,9,... but nbtilde=0 (nb=2)
      // nothing here!
      return 0;
  }
  return fracge4b(xx1,p);
}

double eff_master(double *xx, double *p, int  isge4b=0){
   // return ge4b effective efficiencies in nj=xx[0]]
   // for optimisation purposes, use nonrenormalised fractions and then normalise just once for all
   //  NB: normalisation is a II order effect
   double sum = 0, eff=0;

   int njtilde=((int)( xx[0] ))-2, njtildefromwhere=((int) p[5])-2;;

   if (njtilde<njtildefromwhere) {
      cout << "WARNING in study_extrap: trying to predict efficiencies in a non-validated region! "<< endl;
      return 0;
   }

   double **xx0 = new double* [njtilde+1];
   double ffi=0;
   for (int ib=isge4b; ib<=njtilde; ib++) {
     xx0[ib]=new double [2];
     xx0[ib][0]=ib;
     xx0[ib][1]=njtilde;
     ffi= (isge4b==0 ?  nonnormfrac(xx0[ib], p) : nonnormfracge4b(xx0[ib], p) );
     sum+=ffi;
     eff+=(ib-isge4b)*ffi;
   }

   if (sum>-0.00001 && sum < 1.00001) eff /= (sum*(njtilde-isge4b));

  return eff;
}


double effge4b(double *xx, double *p){
  return eff_master(xx, p,1);
}

double eff(double *xx, double *p){
  return eff_master(xx, p);
}



double efffrom5j_correct_but_old (double *xx, double *p){
  double res=0,x=xx[0];
  double k0=p[0], k1=p[1], k2=p[2], k3=1.0-k1-k0-k2,x0=1-p[3]-p[4],x1=p[3],x2=p[4]; // p[5];

  if ( k3 < -0.00001   || k3 > 1.00001 ) std::cout << "k3 unphysical: "<< k3 <<  "." << std::endl;
  if ( x0 > 1.00001 || x0 < -0.00001   ) std::cout << "x0 unphysical." << std::endl;

  if (x>=5.0 && x<6) res = 3 - 3*k0 - 2*k1 - k2;
  else if (x>=6.0 && x<7.0) res = (3 - 3*k0 - 2*k1 - k2)*x0 + (4 - 3*k0 - 2*k1 - k2)*x1 + (2*k0 + 3*k1 + 4*k2)*x2;
  else if (x>=7.0 && x<8.0) res = (3 - 3*k0 - 2*k1 - k2)*pow(x0,2) + (5 - 3*k0 - 2*k1 - k2)*pow(x1,2) + (6*k0 + 8*k1 + 10*k2)*x1*x2 + 
    (4*k0 + 5*k1)*pow(x2,2) + x0*(-2*(-4 + 3*k0 + 2*k1 + k2)*x1 - 2*(-5 + 3*k0 + 2*k1 + k2)*x2);
  else if (x>=8.0 && x<9.0) res = (3 - 3*k0 - 2*k1 - k2)*pow(x0,3) + (6 - 3*k0 - 2*k1 - k2)*pow(x1,3) + 3*(4*k0 + 5*k1 + 6*k2)*pow(x1,2)*x2 + 
    3*(5*k0 + 6*k1)*x1*pow(x2,2) + 6*k0*pow(x2,3) + 
    pow(x0,2)*(-3*(-4 + 3*k0 + 2*k1 + k2)*x1 - 3*(-5 + 3*k0 + 2*k1 + k2)*x2) + 
    x0*(-3*(-5 + 3*k0 + 2*k1 + k2)*pow(x1,2) - 6*(-6 + 3*k0 + 2*k1 + k2)*x1*x2 + 3*(4*k0 + 5*k1 + 6*k2)*pow(x2,2));
  else if (x>=9.0 && x<10.0) res = (3 - 3*k0 - 2*k1 - k2)*pow(x0,4) + (7 - 3*k0 - 2*k1 - k2)*pow(x1,4) + 4*(5*k0 + 6*k1 + 7*k2)*pow(x1,3)*x2 + 
    6*(6*k0 + 7*k1)*pow(x1,2)*pow(x2,2) + 28*k0*x1*pow(x2,3) + 
    pow(x0,3)*(-4*(-4 + 3*k0 + 2*k1 + k2)*x1 - 4*(-5 + 3*k0 + 2*k1 + k2)*x2) + 
    pow(x0,2)*(-6*(-5 + 3*k0 + 2*k1 + k2)*pow(x1,2) - 12*(-6 + 3*k0 + 2*k1 + k2)*x1*x2 - 
       6*(-7 + 3*k0 + 2*k1 + k2)*pow(x2,2)) + x0*
     (-4*(-6 + 3*k0 + 2*k1 + k2)*pow(x1,3) - 12*(-7 + 3*k0 + 2*k1 + k2)*pow(x1,2)*x2 + 
       12*(5*k0 + 6*k1 + 7*k2)*x1*pow(x2,2) + 4*(6*k0 + 7*k1)*pow(x2,3));
  else if (x>=10.0 ) res= (3 - 3*k0 - 2*k1 - k2)*pow(x0,5) + (8 - 3*k0 - 2*k1 - k2)*pow(x1,5) + 5*(6*k0 + 7*k1 + 8*k2)*pow(x1,4)*x2 + 
    (70*k0 + 80*k1)*pow(x1,3)*pow(x2,2) + 80*k0*pow(x1,2)*pow(x2,3) + 
    pow(x0,4)*(-5*(-4 + 3*k0 + 2*k1 + k2)*x1 - 5*(-5 + 3*k0 + 2*k1 + k2)*x2) + 
    pow(x0,3)*(-10*(-5 + 3*k0 + 2*k1 + k2)*pow(x1,2) - 20*(-6 + 3*k0 + 2*k1 + k2)*x1*x2 - 
       10*(-7 + 3*k0 + 2*k1 + k2)*pow(x2,2)) + pow(x0,2)*
     (-10*(-6 + 3*k0 + 2*k1 + k2)*pow(x1,3) - 30*(-7 + 3*k0 + 2*k1 + k2)*pow(x1,2)*x2 - 
       30*(-8 + 3*k0 + 2*k1 + k2)*x1*pow(x2,2) + 10*(6*k0 + 7*k1 + 8*k2)*pow(x2,3)) + 
    x0*(-5*(-7 + 3*k0 + 2*k1 + k2)*pow(x1,4) - 20*(-8 + 3*k0 + 2*k1 + k2)*pow(x1,3)*x2 + 
       30*(6*k0 + 7*k1 + 8*k2)*pow(x1,2)*pow(x2,2) + 20*(7*k0 + 8*k1)*x1*pow(x2,3) + 40*k0*pow(x2,4));
else res = 0;
if (res>0) res/=(((int)x)-2.0);
return res;
}

bool extractstrings(string filename, vector<string> * pieces){
     size_t last=-1,next=0;
     bool result=true;
     for (int itimes=0; itimes<3; itimes++){
           next = filename.find("_",last+1);
           result = result && (next != string::npos);
           if (!result) return result;
           pieces->push_back(filename.substr(last+1, next - last-1));
           last = next;
     }    
     last = filename.find("__");
     next = filename.find(".root");
     result = result && (next != string::npos) && (last != string::npos) ;
     if (!result) return result;
     pieces->push_back(filename.substr(last+2, next -  last -2));
     return result;
}

void study_extrap(string filename ="ttTRFmaps2018_mc_mc16a_incl__ttPy8.root", string parsyst="" ){
  void Perc(TH1D*hratio_0,TH1D*href);
  void SetGraphLimits(TH1D*hratio);
  void SetGraphLimits(TH1D*hratio, int ratio);

  vector<string>*pieces=new vector<string>();
  vector<string> titlestrings={"Check: ", "Sample: ", "Campaign: ", "Systematics: "};
  if (!extractstrings(filename,pieces)) {
     cout<< "ERROR in study extrap: input map not recognised. Exiting ..."<< endl;
     return;
  }
  else {
     int counter=0;
     for (auto & its : * pieces) {
        cout << titlestrings[counter] << its << endl;
        counter++;
     }
  }

  std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
  std::cout.precision(4);
  SetAtlasStyle();
  TCanvas *c1=new TCanvas("c1","c1",800,900/6*5);
  c1->Draw();
  c1->cd();
  TPad * cup =new TPad("up","up",0,0.50*0.5,1,1);
  TPad * cdown =new TPad("down","down",0,0,1,0.50*0.5);
  cup->SetBottomMargin(cup->GetBottomMargin()*0.09);
  cup->Draw();
  cup->cd();

  bool isge4bmap = filename.find("4b")!=std::string::npos, isdilep = filename.find("dilep")!=std::string::npos;

  double xmin=5,xmax=11,xfitmax;
  if (isdilep) {xmin=4; xmax=9;}
  if (filename.find("data")!=string::npos) xfitmax=xmin+2;
  else xfitmax=xmax;

  // this has to stay in memory
  TF1*fconst = new TF1("fconst","[0]",xmin,xmax);
  fconst->SetLineColor(2);
  fconst->SetLineStyle(10);
  fconst->SetLineWidth(3);


  std::shared_ptr<TFile> _file0 = std::make_shared<TFile>(("data/"+filename).c_str(),"UPDATE");
  _file0->SetName("fincl");

  // VERSION: check to increase the subsubsubversion number!!
  TNamed *vername=(TNamed*) _file0->Get("version");
  vername->SetTitle("2.0.5");
  vername->Write("",TObject::kOverwrite);


  TH1D*h_eff_Nj=(TH1D*)_file0->Get("h_eff_Nj");
  SetGraphLimits(h_eff_Nj,0+10*(isdilep));
  h_eff_Nj->Draw();
  h_eff_Nj->GetYaxis()->SetTitleOffset(2.3);
  h_eff_Nj->GetXaxis()->SetTitleOffset(4.3);

  c1->cd();
  cup->cd();
  if (filename.find("data")==string::npos) ATLASLabelNew(0.4,0.89,"Internal Simulation",1,35);
  else ATLASLabelNew(0.55,0.89,"Internal",1,35);

  TF1*fC; 
  if (!isge4bmap) fC = new TF1("fC",eff,xmin,xmax,6);
  else fC = new TF1("fC",effge4b,xmin,xmax,6);

  // double a=0.8807, b=0.11389, c=0.00521241;
  TH2D* h_frac_nBTags = (TH2D*)_file0->Get("h_frac_nBTags");
  double a=h_frac_nBTags->GetBinContent(h_frac_nBTags->FindBin(xmin-4,0)), b=h_frac_nBTags->GetBinContent(h_frac_nBTags->FindBin(xmin-4,1)),
         c=h_frac_nBTags->GetBinContent(h_frac_nBTags->FindBin(xmin-4,2));

  cout << "Setting lower eff. extr. region b-tag fractions to: "<<"(a, b, c) =" << a << ","<< b<<"," <<c << endl;
  // hfractemp is going to fit f6/f5 or g6/g5 in ljets/dilepOS 
  TH1D*hfractemp=new TH1D("hfractemp","hfractemp",(int)(xmin),0,xmin);

  // hfractemp->GetXaxis()->SetTitle("n_{b}");
  //hfractemp->GetYaxis()->SetTitle(Form("f_{%d}^{b}",(int)xmin+1));
  //hfractemp->GetYaxis()->SetTitleSize(35);
  //hfractemp->GetYaxis()->SetTitleOffset(2.5);

  if (!isge4bmap)  hfractemp->GetYaxis()->SetRangeUser((1.0+9.0*isdilep)*2e-6,2.5);
  else hfractemp->GetYaxis()->SetRangeUser((1.0+9.0*isdilep)*2e-5,2.5);
  hfractemp->GetXaxis()->SetLabelSize(20);
  hfractemp->GetXaxis()->SetTitleOffset(0.8);
  const char * xlabels[5] = {"0#tilde{b}","1#tilde{b}","2#tilde{b}","3#tilde{b}","4#tilde{b}"};
  cout << endl <<"Setting higher eff. extr. region b-tag fractions to: "<< endl;
  std::cout.precision(5);
  for (int ibin=1; ibin<=xmin; ibin++){
      hfractemp->GetXaxis()->SetBinLabel(ibin,xlabels[ibin-1]);
      hfractemp->SetBinContent(ibin,h_frac_nBTags->GetBinContent(h_frac_nBTags->FindBin(xmin-3,ibin-0.5)));
      hfractemp->SetBinError(ibin,h_frac_nBTags->GetBinError(h_frac_nBTags->FindBin(xmin-3,ibin-0.5)));
      if (hfractemp->GetBinError(ibin) < 1e-9 && ibin>1) hfractemp->SetBinError(ibin,hfractemp->GetBinError(ibin-1)*0.1);
      cout << "          " << ibin-1 << "b~  with content  "<< hfractemp->GetBinContent(ibin) << "+-" <<hfractemp->GetBinError(ibin) << endl;
  }
  std::cout.precision(4);
  //TCanvas *c2=new TCanvas("c2","c2");
  cup->cd();

  // Show where we are
  TLatex l; 
  l.SetNDC();
  l.SetTextFont(73);
  l.SetTextSize(32);
  string bmul="3b",year="2015-16";
  if (isge4bmap) bmul="#geq 4b";
  if (filename.find("mc16d")!=string::npos) year="2017";
  l.DrawLatex(0.66,0.75,("#epsilon_{"+bmul+"}, "+ year).c_str());

  // Draw inner pad
  TPad * cinside =new TPad("cin","cin",0.22,0.63,0.33 + (0.33-0.22)*2.0  ,0.8 + (0.8-0.6)/3.0);
  cinside->SetFillStyle(4000);
  cinside->SetLeftMargin(cinside->GetLeftMargin()*1.7);
  //cinside->SetBottomMargin(cinside->GetBottomMargin()*1.7);
  cinside->Draw();
  cinside->cd();
  int lastnonzero;
  for (lastnonzero=hfractemp->GetNbinsX(); hfractemp->GetBinContent(lastnonzero)<=0; lastnonzero--) ;
  //hfractemp->SetMinimum(hfractemp->GetBinContent(lastnonzero)*0.1);
  GetMaximumManually(hfractemp,-1,0.0000000000001);
  cinside->SetLogy();


  TF1 * ffrac; 
  if (!isge4bmap)  ffrac = new TF1("ffrac",nextfracfromprev,0,xmin,6);
  else ffrac = new TF1("ffrac",nextfracfromprevge4b,0,xmin,6);
  ffrac->FixParameter(0,a);
  ffrac->FixParameter(1,b);
  ffrac->FixParameter(2,c);
  ffrac->FixParameter(5,(int) xmin);
  ffrac->SetParLimits(3,0.0,0.1);
  ffrac->SetParLimits(4,0.0,0.1);

  cout << endl << endl<< "               +++++++++++++++++++++++    Fitting x1,x2   ++++++++++++++++++++" << endl;
  

  double f60=hfractemp->GetBinContent(1), f61=hfractemp->GetBinContent(2), f62=hfractemp->GetBinContent(3); 
  double g61=f61/(1-f60),g62=f62/(1-f60),g63=hfractemp->GetBinContent(4)/(1-f60);

  jetprob  firstguess  = guessxif01(f60,f61,a,b,c); 
  jetprob  secondguess = guessxif12(f61,f62,a,b,c); 

  if ( isge4bmap ){
         firstguess = guessxif01(g61,g62,a,b,c,true);
         secondguess = guessxif12(g62,g63,a,b,c,true);
  }
  cout << "From simple formulae would expect x0="<< firstguess.x0  << ", x1 = "<< firstguess.x1<<  ", x2 = "<< firstguess.x2  << endl;
  cout << "                               or x0="<< secondguess.x0 << ", x1 = "<< secondguess.x1<< ", x2 = "<< secondguess.x2 << ", while we get"<< endl;
  ffrac->SetParameter(3,firstguess.x1);
  ffrac->SetParameter(4,firstguess.x2);
  ffrac->FixParameter(5,(int) xmin);
  ffrac->SetParName(3,"x1");
  ffrac->SetParName(4,"x2");
  ffrac->SetLineColor(419);
  ffrac->SetLineStyle(2);
  hfractemp->Fit(ffrac,"L+","same");
  jetprob fromfrac;
  fromfrac.x1=ffrac->GetParameter(3); 
  fromfrac.x2=ffrac->GetParameter(4);
  //ffrac->Draw("same");
  hfractemp->SetDirectory(gROOT);
  cout << "Prob: " << ffrac->GetProb() << endl;
  cout << endl << "                       ===========    End  fitting  x1, x2    ==========" << endl << endl;
  
  // show label
  int excl=1;
  TLatex lfr; 
  lfr.SetNDC();
  lfr.SetTextFont(73);
  lfr.SetTextSize(35);
  if (!isge4bmap)  {
     lfr.DrawLatex(0.76,0.72,Form("f_{%d}^{b}",(int)xmin+1));
     excl=0;
  }
  else lfr.DrawLatex(0.76,0.72,Form("g_{%d}^{b}",(int)xmin+1));

  cup->cd();
  fC->SetParLimits(3,0,0.1);
  fC->SetParLimits(3,0,0.05);
  fC->SetParameters(a,b,c,1-0.333*(3*a+2*b+1*c),0.01);
  fC->FixParameter(0,a);
  fC->FixParameter(1,b);
  fC->FixParameter(2,c);
  fC->FixParameter(5,(int) xmin);
  fC->SetLineColor(3);
  h_eff_Nj->Fit(fC,"LVN","goff",xmin+1,xfitmax);
  fC->DrawCopy("same");
  cout << "Prob: " << fC->GetProb() << endl;
  cout << "(x1_est-x1)/x1 (%) : " << 100.0*(firstguess.x1/fC->GetParameter(3)-1)  << endl << "(x2_est-x2)/x2 (%) : "  << 100.0*(firstguess.x2/fC->GetParameter(4)-1) << endl;
  cout << "(x1_est-x1)/x1 (%) : " << 100.0*(secondguess.x1/fC->GetParameter(3)-1) << endl << "(x2_est-x2)/x2 (%) : " << 100.0*(secondguess.x2/fC->GetParameter(4)-1) << endl;
  TF1*fCest = (TF1*) fC->DrawCopy("same");
  fCest->SetParameter(3,fromfrac.x1);
  fCest->SetParameter(4,fromfrac.x2);
  fCest->SetLineColor(ffrac->GetLineColor());
  fCest->SetLineStyle(ffrac->GetLineStyle());
  fCest->Draw("same");

  // Draw constant extrapolation in xmin, xmin+1 (a la '15-16)
  h_eff_Nj->Fit(fconst,"N","goff",xmin,xmin+2);
  fconst->Draw("same");
  c1->cd();
  cdown->SetBottomMargin(cdown->GetBottomMargin()*1.52);
  // cdown->SetGrid();
  cdown->Draw();
  cdown->cd();
  TH1D* h_eff_Nj_extrap=(TH1D*) h_eff_Nj->Clone("h_eff_Nj_extrap_0"), * h_eff_Nj_extrap_old=(TH1D*) h_eff_Nj->Clone("h_eff_Nj_extrap_old");
  for (int ibin=1; ibin<= h_eff_Nj_extrap->GetNbinsX(); ibin++) { 
    h_eff_Nj_extrap->SetBinContent(ibin,fCest->Eval(4+ibin-1));
    // cout << "Setting ibin " << ibin << " with content  "<< h_eff_Nj_extrap->GetBinContent(ibin) << endl;
    h_eff_Nj_extrap_old->SetBinContent(ibin,fconst->Eval(4+ibin-1));
  }
  h_eff_Nj_extrap->SetLineColor(fCest->GetLineColor());
  h_eff_Nj_extrap->SetLineStyle(fCest->GetLineStyle());
  TH1D*hratio_0=(TH1D*) h_eff_Nj_extrap->Clone("hratio_0");
  Perc(hratio_0,h_eff_Nj);
  int ratiotrue=1+10*isdilep;
  if (!isge4bmap) ratiotrue=2+10*isdilep;
  SetGraphLimits(hratio_0,ratiotrue);
  h_eff_Nj_extrap_old->SetLineColor(fconst->GetLineColor());
  h_eff_Nj_extrap_old->SetLineStyle(fconst->GetLineStyle());
  h_eff_Nj_extrap_old->SetLineWidth(fconst->GetLineWidth());
  TH1D*hratio_old=(TH1D*) h_eff_Nj_extrap_old->Clone("hratio_old");
  Perc(hratio_old,h_eff_Nj);
  SetGraphLimits(hratio_old,ratiotrue);
  // Save extrapolation histogram extracted from only first two bins!
  h_eff_Nj_extrap->SetTitle("Global efficiencies vs Nj as predicted from a fit on b-tag fractions in efficiency extraction regions.");
  h_eff_Nj_extrap->Write("h_eff_Nj_extrap",TObject::kOverwrite);

  // Now get the best ideal extrapolation, not possible in data obviously
  cup->cd();
  h_eff_Nj_extrap=(TH1D*) h_eff_Nj->Clone("h_eff_Nj_extrap");
  for (int ibin=1; ibin<= h_eff_Nj_extrap->GetNbinsX(); ibin++) h_eff_Nj_extrap->SetBinContent(ibin,fC->Eval(4+ibin-1));
  // h_eff_Nj_extrap->SetFillColor(3);
  h_eff_Nj_extrap->SetLineColor(3);
  h_eff_Nj_extrap->SetFillStyle(3015);  
  h_eff_Nj_extrap->Draw("hist same");
  h_eff_Nj->SetDirectory(gROOT);
  h_eff_Nj_extrap->SetDirectory(gROOT);

  _file0->cd();
  TH1D*h_eff_Nj_dist=(TH1D*) h_eff_Nj->Clone(("h_eff_Nj"+parsyst+"_dist").c_str());
  if (parsyst!="") h_eff_Nj_dist->SetTitle(Form("Global efficiencies vs Nj as computed ex-post from ttTRF ntuples, using %s parametrisation.", parsyst.c_str()));
  else h_eff_Nj_dist->SetTitle("Global efficiencies vs Nj as computed ex-post from ttTRF ntuples, using default (ptdRj) parametrisation.");
  for (int ibin=0; ibin<=h_eff_Nj_dist->GetNbinsX()+1; ibin++) h_eff_Nj_dist->SetBinContent(ibin, 0.0);
  string sample=pieces->at(1), camp=pieces->at(2),syst=pieces->at(3);

  //////
  string selincl="";
  if (isdilep) selincl="dilep";
  for (int ibin = xmin-3; ibin<=xmax-5; ibin++) {
        cout << endl << "Sample: "<< sample << ", jmul: "<< ibin+3<< ", syst: "<< syst << endl; 
        h_eff_Nj_dist->SetBinContent(ibin,GetEffexpost(sample,ibin+3,camp,syst,excl,selincl,0,1,parsyst));
  }
  cout << endl<< "Sample: "<< sample << ", jmul: "<< xmax-1<< ", syst: "<< syst << endl; 
  h_eff_Nj_dist->SetBinContent(xmax-4,GetEffexpost(sample,xmax-1,camp,syst, excl,selincl+"_ge",0,1,parsyst));
  //////

  _file0->cd();
  h_eff_Nj_dist->SetLineColor(613);
  h_eff_Nj_dist->SetLineStyle(1);
  h_eff_Nj_dist->SetFillStyle(0);

  h_eff_Nj_dist->Draw("hist same");
  // Save "distorsion" histogram (extracted purely from bare ttTRF, so safe for data in any nj)!
  // h_eff_Nj_dist->SetDirectory(_file0);
  h_eff_Nj_dist->Write(("h_eff_Nj"+parsyst+"_dist").c_str(),TObject::kOverwrite);
  h_eff_Nj_dist->SetDirectory(gROOT);
  cdown->cd();
  //draw stat error
  TH1D* hratioone=(TH1D*) h_eff_Nj->Clone("hratioone");
  Perc(hratioone,h_eff_Nj);
  SetGraphLimits(hratioone,ratiotrue);
  hratioone->SetFillColor(5);
  hratioone->Draw("E2");
  hratioone->DrawCopy("hist same");
  hratioone->SetDirectory(gROOT);
  hratioone->GetYaxis()->SetTitle("#Delta (%) ");
  
  TH1D* hratio=(TH1D*) h_eff_Nj_extrap->Clone("hratio");
  Perc(hratio,h_eff_Nj);
  //hratio->GetXaxis()->SetTitleOffset(hratio->GetXaxis()->GetTitleOffset()*1.5);
  hratio->Draw("hist same");
  hratio_0->Draw("same hist");
  hratio_0->SetDirectory(gROOT);
  hratio_old->Draw("same hist");
  hratio_old->SetDirectory(gROOT);
  hratio->SetDirectory(gROOT);
  TH1D* hratio_dist=(TH1D*) h_eff_Nj_dist->Clone("hratio_dist");
  Perc(hratio_dist,h_eff_Nj);
  hratio_dist->Draw("same hist");
  hratio_dist->SetDirectory(gROOT);

  cup->cd();
  h_eff_Nj->GetXaxis()->SetLabelOffset(4.3);
  // delete _file0;
  if (parsyst!="") c1->SaveAs((filename.substr(0,filename.find(".root"))+"_"+parsyst+".pdf").c_str());
  else             c1->SaveAs((filename.substr(0,filename.find(".root"))+".pdf").c_str());
}

void Perc(TH1D*hratio_0,TH1D*href){
  TH1D*href_str = (TH1D*) href->Clone("href_str");
  for (int ibin=1; ibin<= href->GetNbinsX(); ibin++) href_str->SetBinError(ibin,0);
  hratio_0->Add(href_str,-1);
  hratio_0->Divide(href_str);
  hratio_0->Scale(100.0);
  delete href_str;
}

void SetGraphLimits(TH1D*hratio){
  // found manually minimum and maximum ...
  double gmin=100, gmax=-100;
  for (int ibin=2; ibin<= hratio->GetNbinsX(); ibin++) {
        double thisx = hratio->GetBinContent(ibin),delx= hratio->GetBinError(ibin);
        if (thisx!=0 && thisx+delx> gmax) gmax=thisx+delx;
        if (thisx!=0 && thisx-delx< gmin) gmin=thisx-delx;
  }
  hratio->SetMaximum(gmax*(1+6*(gmax-gmin)/(gmax+gmin) ));
  hratio->SetMinimum(gmin*(1-6*(gmax-gmin)/(gmax+gmin) ));
}

void SetGraphLimits(TH1D*hratio, int ratio){
  // we know which histogram we are restyling, so use usual values:
  if (!hratio) cout << "Error in SetGraphLimits: no hist passed ! Par = "<< ratio << endl;
  else cout << "INFO in SetGraphLimits: hist correctly passed ." << endl;
  double gmax = 0.07, gmin =0.01;
  if (ratio==10) {// dilep, upper plot
     gmax=0.09; gmin=0.02;
     hratio->GetXaxis()->SetRangeUser(4,9);     
     hratio->GetXaxis()->SetNdivisions(105);     
  }
  int ndiv=500 + (gmax-gmin)*100;
  if ((ratio%10)==1) {gmax=50; gmin=-50; ndiv=504;}
  else if ((ratio%10)==2) {gmax=15; gmin=-15; ndiv=506;}
  if ((ratio/10)>0 && (ratio%10) > 0) {gmax*=2; gmin*=2;}
  hratio->SetMaximum(gmax);
  hratio->SetMinimum(gmin);
  hratio->GetYaxis()->SetNdivisions(ndiv,kFALSE);
}

double FitFractions (double *xx, double *p){
   int ix = (int) xx[0];
   
   double result=0, pnb=1-p[0], pb=p[0], pnb2=1-p[1], pb2=p[1], cp=1-p[2]; //, pnj = p[2];
   switch (ix) {
        case 0:
          result=   pnb*pnb*pnb; //  + 4.0 * pnj * (pb2 + pnb2) * pnb2 * pnb2 * pnb2;
          break;          
        case 1:
          result=   3.0 * pnb*pnb*pb; //  + 12.0 * pnj * (pb2 + pnb2) * pnb2 * pnb2 * pb2;  
          break;          
        case 2:
          result=   3.0 * pnb*pb*pb * cp; //   + 12.0 * pnj * (pb2 + pnb2) * pnb2 * pb2 * pb2;
          break;          
        case 3:
          result=   pb * pb * pb * cp; //  +  4.0 * pnj * (pb2 + pnb2) * pb2 * pb2 * pb2;
          break;          
        case 4:
          result=  pnb2*pnb2*pnb2*pnb2;
          break;          
        case 5:
          result=4.0 * pnb2*pnb2*pnb2*pb2;
          break;          
        case 6:
          result=6.0 * pnb2*pnb2*pb2*pb2 * cp;
          break;          
        case 7:
          result=4.0 * pnb2*pb2*pb2*pb2 * cp;
          break;          
        case 8: 
          result=  pb2*pb2*pb2*pb2 * cp * cp;
          break;          
   }
   if (ix<4) result/= 1.0 -p[2] * (3.0 * pnb*pb*pb + pb*pb*pb); // + 4.0 * pnj * (pb2 + pnb2) * ( pnb2  + pb2  )* ( pnb2  + pb2  )* ( pnb2  + pb2  ) ;
   else result/= 1.0 - p[2]* pb2 * pb2 * ( 6.0 * pnb2*pnb2 + 4.0 * pnb2*pb2 + pb2*pb2 );
   return result;
}


void study_fractions(){
  TH1::SetDefaultSumw2();
  TCanvas *c1 = new TCanvas("c1","c1");
  c1->cd();
  TFile *_file0 = TFile::Open("/eos/infnts/atlas/gpanizzo/SM4t_L2/ljets5j/ttbar_pyt8_hdamp258.mc16d.root");
  TFile *_file1 = TFile::Open("/eos/infnts/atlas/gpanizzo/SM4t_L2/ljets6j/ttbar_pyt8_hdamp258.mc16d.root");

  TTree*nominal_Loose = (TTree*) _file0->Get("nominal_Loose");
  TTree*nominal_Loose6 = (TTree*) _file1->Get("nominal_Loose");
  TH1D* hntags = new TH1D("hntags","hntags",9,0,9.0);
  TH1D* hntags5 = new TH1D("hntags5","hntags5",9,0,9.0);
  nominal_Loose->Draw("(nBTags_MV2c10_77-1.5)>>hntags5","weight_normalise*weight_mc*weight_pileup*weight_leptonSF*weight_bTagSF_MV2c10_77*weight_jvt");
  nominal_Loose6->Draw("(nBTags_MV2c10_77-1.5+4)>>hntags","weight_normalise*weight_mc*weight_pileup*weight_leptonSF*weight_bTagSF_MV2c10_77*weight_jvt");
  // hntags->SetMinimum(0.0001);
  hntags5->Scale(1.0/hntags5->Integral(1,9));
  hntags->Scale(1.0/hntags->Integral(1,9));
  hntags5->Print("V");
  hntags->Print("V");
  hntags->Add(hntags5);
  hntags->Draw();
  c1->SetLogy();

  // Draw nb+1/nb
  for (int ibin=2; ibin<=4; ibin++) {
      hntags5->SetBinContent(ibin-1,hntags->GetBinContent(ibin)*TMath::Binomial(3,ibin-2)/(hntags->GetBinContent(ibin-1)*TMath::Binomial(3,ibin-1)));
      hntags5->SetBinError(ibin-1,hntags->GetBinError(ibin)*TMath::Binomial(3,ibin-2)/(hntags->GetBinContent(ibin-1)*TMath::Binomial(3,ibin-1)));
      cout << TMath::Binomial(3,ibin-2) << "/"<< TMath::Binomial(3,ibin-1)<< endl;
  }
  for (int ibin=6; ibin<=9; ibin++) { 
     hntags5->SetBinContent(ibin-2,hntags->GetBinContent(ibin)*TMath::Binomial(4,ibin-6)/(hntags->GetBinContent(ibin-1)*TMath::Binomial(4,ibin-5)));
     hntags5->SetBinError(ibin-2,hntags->GetBinError(ibin)*TMath::Binomial(4,ibin-6)/(hntags->GetBinContent(ibin-1)*TMath::Binomial(4,ibin-5)));
     cout << TMath::Binomial(4,ibin-6) << "/"<< TMath::Binomial(4,ibin-5)<< endl;
  }

  TCanvas*c3 = new TCanvas("c3","c3");
  c3->cd();
  hntags5->Draw();

  TCanvas*c2 = new TCanvas("c2","c2");
  c2->cd();
  TF1* fitfrac = new TF1("fitfrac",FitFractions,0,9,3);
  fitfrac->SetParameters(0.05,0.05,0.0);
  fitfrac->SetParLimits(0,0,1);
  fitfrac->SetParLimits(1,0,1);
  fitfrac->SetParLimits(2,-0.5,0.5);
  gStyle->SetOptFit(1111);
  hntags->Fit("fitfrac","VL");

  c2->SetGrid();
  TH1D*hfitted = (TH1D*) hntags->Clone("hfitted");
  for (int ibin=1; ibin<=9; ibin++) hfitted->SetBinContent(ibin,fitfrac->Eval(ibin-1+0.3));
  hfitted->Divide(hntags);
  hfitted->SetMinimum(0.75);
  hfitted->SetMaximum(1.25);
  hfitted->Draw();
}
