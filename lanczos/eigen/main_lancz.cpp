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

#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET

#include <omp.h>
#include <Eigen/Eigenvalues>
#include <Eigen/Sparse>
#include <iostream>
#include "eigen_lanczos.h"
#include "../../ratApps/sncndn.h"
#include "../../timer.h"

// Include Alglib for its tridiagonal eigen solver
#include "linalg.h"

//#include <fftw3.h>
// #include <time.h>




void checkAlglib(const double &,const double &);
void checkEigenTrid(const double &, const double &);
void some_stlVectorTest();






using namespace Eigen;

int main()
{
  const double diagEl=2.0001;
  const double offDiagEl=-1.;
  
  int_type sizeAmat=100000;
  int_type nshifts=4;
  DynamicSparseMatrix<double> AB(sizeAmat,sizeAmat);
  AB.reserve(3*sizeAmat);
  for (int iii=0;iii<sizeAmat;++iii){AB.insert(iii,iii)=diagEl;}
  for (int iii=0;iii<sizeAmat-1;++iii){AB.insert(iii+1,iii)=offDiagEl;}
  for (int iii=0;iii<sizeAmat-1;++iii){AB.insert(iii,iii+1)=offDiagEl;}
  AB.finalize();
  SparseMatrix<double> A(AB);
  
//   SparseMatrix<bool> BSpar(sizeAmat,sizeAmat);
//   BSpar.reserve(3*sizeAmat);
//   for (int iii=0;iii<sizeAmat;++iii){BSpar.insert(iii,iii)=true;}
//   for (int iii=0;iii<sizeAmat-1;++iii){BSpar.insert(iii+1,iii)=true;}
//   for (int iii=0;iii<sizeAmat-1;++iii){BSpar.insert(iii,iii+1)=true;}
//   bool *vBoolPoint=BSpar._valuePtr();
//   for (int iii=0;iii<10;++iii){std::cout << vBoolPoint[iii] << ",";}
  
  VectorXd b=VectorXd::Random(sizeAmat);
  VectorXd x=VectorXd::Zero(sizeAmat);
  double minEig=0;
  double maxEig=0;
  default_monitor<double> mon(b,2000,1e-2);
  
  
  eigen_lanc_extremal_eig_mult(A, b,x, minEig, maxEig, mon);
  

  
  
  

  checkAlglib(diagEl,offDiagEl);
  
  checkEigenTrid(diagEl,offDiagEl);
  
  some_stlVectorTest();
  
  
  



  
  return 1;
}






void checkAlglib(const double &diagEl,const double &offDiagEl)
{
  // Checking alglib interface
  alglib::ae_int_t sizeT=5000;
  alglib::ae_int_t type=0;
  double *aaa=new double[sizeT];
  double *bbb=new double[sizeT-1];
  for(int iii=0;iii<sizeT;++iii){aaa[iii]=diagEl;}
  for(int iii=0;iii<sizeT-1;++iii){bbb[iii]=offDiagEl;}  
  
  alglib::real_1d_array r1;
  alglib::real_1d_array r2;
  alglib::real_2d_array z;
  z.setlength(sizeT,sizeT);
  
  bool success=0;
  
  r1.setcontent(sizeT,aaa);
  r2.setcontent(sizeT-1,bbb);
  
  delete[] aaa; aaa=0;
  delete[] bbb; bbb=0;
  
  //for(int iii=0;iii<sizeT;++iii){std::cout << r1[iii] << ", ";}
  std::cout << "\n\n";
  
  success=alglib::smatrixtdevd(r1, r2, sizeT,type, z);
  
  std::cout << "Success: " << success << "\n";
  
  //for (int iii=0;iii<sizeT;++iii){std::cout << r1(iii) << ", ";}
  std::cout << "\n\n";
  // End checking alglib interface
  
  std::cout << "minEig=" << r1(0) << ", maxEig=" << r1(sizeT-1) << "\n";
  
}






void checkEigenTrid(const double &diagEl,const double &offDiagEl)
{
  int sizeT=100;
  int start=0;
  int end=sizeT-1;
  double *aaa=new double[sizeT];
  double *bbb=new double[sizeT-1];
  bool *matrixQ=0;
  for(int iii=0;iii<sizeT;++iii){aaa[iii]=diagEl;}
  for(int iii=0;iii<sizeT-1;++iii){bbb[iii]=offDiagEl;} 
  
  int iter=0;
  int m_maxIterations=100;
  while (end>0)
  {
    for (int i = start; i<end; ++i)
      if (internal::isMuchSmallerThan(internal::abs(bbb[i]),(internal::abs(aaa[i])+internal::abs(aaa[i+1]))))
        bbb[i] = 0;

    // find the largest unreduced block
    while (end>0 && bbb[end-1]==0)
    {
      iter = 0;
      end--;
    }
    if (end<=0)
      break;

    // if we spent too many iterations on the current element, we give up
    iter++;
    if(iter > m_maxIterations) break;

    start = end - 1;
    while (start>0 && bbb[start-1]!=0)
      start--;

    internal::tridiagonal_qr_step<RowMajor>(aaa, bbb, start, end, matrixQ, sizeT);
  }
  // internal::tridiagonal_qr_step<RowMajor>(aaa, bbb, start, end, matrixQ, sizeT);
  
  Map<VectorXd> avec(aaa,sizeT);
  Map<VectorXd> bvec(bbb,sizeT-1);
  //VectorXd dd(avec);
  
  std::cout << avec.maxCoeff() << ", " << avec.minCoeff() << "\n";
  std::cout << bvec.maxCoeff() << ", " << bvec.minCoeff() << "\n";
  
  delete[] aaa; aaa=0;
  delete[] bbb; bbb=0;
  
  
}





void some_stlVectorTest()
{
  std::vector<double> b;
  b.push_back(30);
  b.push_back(20);
  double *gets=&(*b.begin());
  
  for (int iii=0;iii<2;++iii)
  {
    std::cout << gets[iii] << "\n";
  }
}