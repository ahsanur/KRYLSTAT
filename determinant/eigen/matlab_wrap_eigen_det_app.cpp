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

#include <iostream>
#include "eigen_det_app.h"
#include <math.h>
#include "mex.h"
#include "matrix.h"
#include <stdlib.h>

using namespace std;

typedef MappedSparseMatrix<double,RowMajor,size_t> MappedSparseMatrixType;


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs != 6)
    {
        mexErrMsgTxt("Need 6 input arguments: matrix and colouringtype");
    }
    if (nlhs>2)
    {
        mexErrMsgTxt("Maximum of 2 output arguments");
    }
    
    
    
    plhs[1]=mxCreateDoubleMatrix(1,1,mxREAL);
    
    int i=0;
    MappedSparseMatrixType A(mxGetM(prhs[i]), mxGetM(prhs[i]), mxGetNzmax(prhs[i]), mxGetJc(prhs[i]), mxGetIr(prhs[i]), mxGetPr(prhs[i]));

    double ldet;
    
    
    VectorXd margVar(1);
    double neigh=mxGetScalar(prhs[3]);
    size_t dist_col(static_cast<size_t>(neigh));
    double cgTol=mxGetScalar(prhs[1]);
    size_t cgMaxIter(static_cast<int>(mxGetScalar(prhs[2])));
    // mexPrintf("YEP %i \n",dist_col);
    
    logDetMargVar(A,cgTol,cgMaxIter,dist_col,ldet,margVar,3);
    
    plhs[0]=mxCreateDoubleScalar(ldet);
    
    
    
    /*plhs[0]=mxCreateDoubleMatrix(1,1,mxREAL);
    delete[] irsInt;
    delete[] jcsInt;*/
    
}