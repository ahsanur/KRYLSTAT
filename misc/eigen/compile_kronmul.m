mex CXX="g++-4.4" CC="gcc-4.4" ...
    CXXFLAGS="\$CXXFLAGS -I. -I/home/anne/erlenda/libs/eigen" -v -largeArrayDims CXXOPTIMFLAGS="-O2 -DNDEBUG" ... 
    LDOPTIMFLAGS="-O2 -fopenmp"  kronmult_eigen.cpp