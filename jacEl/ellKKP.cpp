// This file is part of the KRYLSTAT function library
//
// Copyright (C) 2011 Erlend Aune <erlenda@math.ntnu.no>
//
// The KRYLSTAT library is free software; 
// you can redistribute it and/or modify it under the terms 
// of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the 
// License, or (at your option) any later version.
//
// Alternatively, you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// The KRYLSTAT library is distributed in the 
// hope that it will be useful, but WITHOUT ANY WARRANTY; without
// even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU Lesser General Public License
// or the GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License and a copy of the GNU General Public License along with
// the KRYLSTAT library. If not, see 
// <http://www.gnu.org/licenses/>.

#include "ellKKP.h"

void ellKKP(base L,base *K,base *Kp)
{
  const base pi(static_cast<long double>(3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679L));
  const base eps(static_cast<long double>(1.0e-18L));
  const base two(2.L);
  const base one(1.L);
  const base zero(0.L);
  
  base m=std::exp(-two*pi*(L));
  
  if(L>base(10.L)){*K=pi/two;*Kp=pi*(L)+std::log(base(4.L));return;}
  
  base a0(one);
  base b0=std::sqrt(one-m);
  base s0=m;
  base i1(zero);
  base mm(one);
  
  base a1(zero);
  base b1(zero);
  base c1(zero);
  base w1(zero);
  

  
  while(std::abs(mm)>std::abs(eps))
  {
    a1=(a0+b0)/two;
    b1=std::sqrt(a0*b0);
    c1=(a0-b0)/two;
    i1=i1+one;
    w1=std::pow(two,i1)*std::pow(c1,two);
    mm=w1;
    s0=s0+w1;
    a0=a1;
    b0=b1;
  }
  *K=pi/(two*a1);
  
  if (  std::abs( one-m  )<std::abs(eps) )
  {
    *K=(pi/(two*a1))*std::numeric_limits<long double>::infinity( );
    // K->imag()=zero.real();
  }
  
  a0=one;
  b0=sqrt(m);
  s0=one-m;
  i1=base(zero);
  mm=one;
  
  while(std::abs(mm)>std::abs(eps))
  {
    a1=(a0+b0)/two;
    b1=std::sqrt(a0*b0);
    c1=(a0-b0)/two;
    i1=i1+one;
    w1=std::pow(two,i1)*std::pow(c1,two);
    mm=w1;
    s0=s0+w1;
    a0=a1;
    b0=b1;
  }
  *Kp=pi/(two*a1);
  
  if (  std::abs(m)<std::abs(eps)  )
  {
    *Kp=(pi/(two*a1))*std::numeric_limits<long double>::infinity( );
    //K->imag()=zero.real();
  }
 
}

void ellKKP(double L,double &K,double &Kp)
{
  base tempL=static_cast<base>(L);
  base tempK=static_cast<base>(K);
  base tempKp=static_cast<base>(Kp);
  base *Kpt=&tempK;
  base *Kppt=&tempKp;
  ellKKP(tempL,Kpt,Kppt);
  K=*Kpt;
  Kp=*Kppt;
}

void ellPJC(base u,base m,base &sn,base &cn,base &dn,base &am)
{
  // returns x = { sn, cn, dn, ph }
  const base pi(static_cast<base>(3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679L));
  //const base eps(static_cast<base>(1.0e-18L));
  const base zero(0.0L);
  const base one(1.0L);
  const base two(2.0L);
  const base four(4.0L);
  const base half(0.5L);
  const base quart(0.25L);
  

  base ai, b, phi, t, twon;
  base a[9];
  base c[9];
  int i;

  // Check for special cases

  if ( m < zero || m > one )
  {
    // domain error
    // error( "Elliptic.Jacobi()", ERR_DOMAIN );
    printf( "Elliptic.Jacobi(). Domain error. m<0 or m>1");
    sn = 0.0;
    cn = 0.0;
    am = 0.0;
    dn = 0.0;
    return;
  }
  if( m < base(1.0e-9L) )
  {
    // m > 0 and near 0
    t = std::sin(u);
    b = std::cos(u);
    ai = quart * m * (u - t*b);
    sn = t - ai*b;
    cn = b + ai*t;
    am = u - ai;
    dn = one - half*m*t*t;
    return;
  }

  if( m >= base(1.0L - 1.0e-10L) )
  {
    // m < 1 and near 1
    ai = quart * (one-m);
    b = std::cosh(u);
    t = std::tanh(u);
    phi = one/b;
    twon = b * std::sinh(u);
    sn = t + ai * (twon - u)/(b*b);
    am = two*std::atan(std::exp(u)) - (half*pi) + ai*(twon - u)/b;
    ai *= t * phi;
    cn = phi - ai * (twon - u);
    dn = phi + ai * (twon + u);
    return;
  }

  // AGM scale
  a[0] = one;
  b = std::sqrt(one - m);
  c[0] = std::sqrt(m);
  twon = one;
  i = 0;

  while ( std::abs(c[i]/a[i]) > std::numeric_limits<base>::epsilon() )
  {
    if( i > 7 )
    {
      //error( "Elliptic.Jacobi()", ERR_OVERFLOW );
      printf("Elliptic.Jacobi(), overflow error");

      
      // backward recurrence
      phi = twon * a[i] * u;
      do
      {
        t = c[i] * std::sin(phi) / a[i];
        b = phi;
        phi = half*(std::asin(t) + phi);
      }
      while ( --i != 0 );

      sn = std::sin(phi);
      t = std::cos(phi);
      cn = t;
      dn = t/std::cos(phi-b);
      am = phi;
      return;
    }
    ai = a[i];
    ++i;
    c[i] = half*( ai - b );
    t = std::sqrt( ai * b );
    a[i] = half*( ai + b );
    b = t;
    twon *= two;
  }

  // backward recurrence
  phi = twon * a[i] * u;
  do
  {
    t = c[i] * std::sin(phi) / a[i];
    b = phi;
    phi = half*(std::asin(t) + phi);
  }
  while ( --i != 0 );

  sn = std::sin(phi);
  t = std::cos(phi);
  cn = t;
  dn = t / std::cos(phi-b);
  am = phi;
  return;
}


void ellPJC(double u, double m,double &sn,double &cn, double &dn,double &am)
{
  base tempU(static_cast<base>(u));
  base tempM(static_cast<base>(m));
  base tempSN,tempCN,tempDN,tempAM;
  
  ellPJC(tempU,tempM,tempSN,tempCN,tempDN,tempAM);
  sn=static_cast<double>(tempSN);
  cn=static_cast<double>(tempCN);
  dn=static_cast<double>(tempDN);
  am=static_cast<double>(tempAM);
}

void ellPJC(cpxBase const &u, base m,cpxBase &sn, cpxBase &cn,cpxBase &dn)
{
  base x = u.real();
  base y = u.imag();
  base snx, cnx, dnx, amx;
  base sn_, cn_, dn_, am_;
  base one(1.0L);

  // call real version
  ellPJC(x,m, snx, cnx, dnx, amx);
  ellPJC(y,one-m, sn_, cn_, dn_, am_);

  base sny_i = sn_ / cn_;  // sny/i
  base cny = one / cn_;
  base dny = dn_ / cn_;

  cpxBase d = one + m * std::sqrt( snx * sny_i );
  sn = cpxBase( snx * cny * dny, sny_i * cnx * dnx ) / d;
  cn = cpxBase( cnx * cny, -snx * sny_i * dnx * dny ) / d;
  dn = cpxBase( dnx * dny, -m * snx * sny_i * cnx * cny ) / d;
}

void ellPJC(cpxDbl const &u, double m, cpxDbl &sn,cpxDbl &cn, cpxDbl &dn)
{
  cpxBase tempU=cpxBase(static_cast<cpxBase>(u));
  base tempM(static_cast<base>(m));
  cpxBase tempSN=cpxBase(static_cast<cpxBase>(sn));
  cpxBase tempCN=cpxBase(static_cast<cpxBase>(cn));
  cpxBase tempDN=cpxBase(static_cast<cpxBase>(dn));
  
  ellPJC(tempU,tempM,tempSN,tempCN,tempDN);
  
  sn=static_cast<cpxDbl>(tempSN);
  cn=static_cast<cpxDbl>(tempCN);
  dn=static_cast<cpxDbl>(tempDN);
}

void sqrtIntPoints(int N,double minEig,double maxEig,double *intConst,double *wsq,double *dzdt)
{
  const double pi(static_cast<double>(3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679));
  double k2=minEig/maxEig;
  double L=-0.5*std::log(k2)/pi;
  double K,Kp;
  ellKKP(L,K,Kp);
  *intConst=-2*Kp*std::sqrt(minEig)/(pi*static_cast<double>(N));
  
  
  cpxDbl *t=new cpxDbl[N];
  for(int iii=0;iii<N;iii++)
  {
    t[iii]=cpxDbl(0,(0.5+static_cast<double>(iii))*Kp/static_cast<double>(N));
  }
  
  cpxDbl *sn=new cpxDbl[N];
  cpxDbl *cn=new cpxDbl[N];
  cpxDbl *dn=new cpxDbl[N];
  
  double m=std::exp(-2*pi*L);
  for(int iii=0;iii<N;iii++)
  {
    ellPJC(t[iii],m,sn[iii],cn[iii],dn[iii]);
  }
  
  cpxDbl minEigCpx(minEig,0);
  for(int iii=0;iii<N;iii++)
  {
    dzdt[iii]=(cn[iii]*dn[iii]).real();
    wsq[iii]=-(minEigCpx*std::pow(sn[iii],2)).real();
  }
  
  
  delete[] t;
  delete[] sn;
  delete[] cn;
  delete[] dn;
}


int sqrtIntPoints(double tol,double minEig,double maxEig,double *intConst,double *wsq,double *dzdt)
{
  const double pi(static_cast<double>(3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679));
  double kon=-2.*pi/(std::log(maxEig/minEig)+3.);
  int N=std::ceil(std::log(tol)/kon)+1;
  std::cout << "All before allocation. N="<<N<< "\n";
  wsq=new double[N]; 
  std::cout << "First allocation \n";
  dzdt=new double[N];
  std::cout << "Second allocation \n";
  
  
  sqrtIntPoints(N,minEig,maxEig,intConst,wsq,dzdt);
  return N;
}




void logIntPoints(int N, double minEig,double maxEig, double *intConst, cpxDbl *wsq, cpxDbl *dzdt)
{
  printf("logIntPoints() DOES NOT WORK YET \n");
  const double pi(static_cast<double>(3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679));
  double k=(std::pow(maxEig/minEig,0.25)-1.)/(std::pow(maxEig/minEig,0.25)+1);
  double L=-std::log(k)/pi;
  double K,Kp;
  ellKKP(L,K,Kp);
  
  *intConst=-8.*K*std::pow(minEig*maxEig,0.25)/(k*pi*N);
  
  cpxDbl *t=new cpxDbl[N];
  
  for (int iii=0;iii<N;iii++)
  {
    t[iii]=cpxDbl(0.,0.5*Kp)-cpxDbl(K,0.) + cpxDbl((0.5+iii)*2.*K/static_cast<double>(N),0);
  }
  
  cpxDbl *sn=new cpxDbl[N];
  cpxDbl *cn=new cpxDbl[N];
  cpxDbl *dn=new cpxDbl[N];
  
  
  double m=std::exp(-2.*pi*L);
  for (int iii=0;iii<5;iii++)
  {
    ellPJC(t[iii],m,sn[iii],cn[iii],dn[iii]);
  }
  
  cpxDbl mmf(std::pow(minEig*maxEig,0.25),0);
  cpxDbl kres=cpxDbl(1.,0.)/static_cast<cpxDbl>(k);
  for (int iii=0;iii<N;iii++)
  {
    wsq[iii]=mmf*(kres+sn[iii])/(kres-sn[iii]);
    wsq[iii]=std::pow(wsq[iii],2.);
    dzdt[iii]=cn[iii]*dn[iii]*std::pow(kres-sn[iii],2.);
    dzdt[iii]=dzdt[iii]*std::log(wsq[iii])/std::sqrt(wsq[iii]);
  }
  
  
  
  delete[] sn;
  delete[] cn;
  delete[] dn;
  delete[] t;
}
