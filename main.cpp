#include <iostream>
#include "FTthreadManager.h"
#include <unistd.h>
#include <time.h>
#include <random>
#include <vector>
#include "cheader.h"

const int n_tasks = 10;
const int n_threads = 5;

const int M = 1024*128;
const int N = 1024;

FTthreadManager tm;

int foo( int id )
{
    std::random_device rd;
    std::mt19937 gen( rd() );;
    std::uniform_real_distribution<double> dis( 0, 1 );
    std::vector < std::vector <double> > mat( M, std::vector<double>( N ) );
    std::vector < double > vec_A(N);
    std::vector < double > vec_B(M);
    
    for(int m=0; m<M; ++m)
        for(int n=0; n<N; ++n)
            mat[m][n] = dis( gen );
    
    for(int n=0; n<N; ++n)
        vec_A[n] = dis( gen );

    std::cout << "task '" << id << "' initialized data." << std::endl;
    
    for(int m=0; m<M; ++m) {
        vec_B[m] = 0;
        for(int n=0; n<N; n++)
            vec_B[m] += mat[m][n]*vec_A[n];
    }
    
    std::cout << "task '" << id << "' finished computation!" << std::endl;

    return 0;
}

int main() 
{
    FTthreadManager tm(n_threads);
    tm.init();
    for(int i=0; i<n_tasks; ++i) {
        tm.submit( foo, i );
    }
    tm.synchronize();
    tm.fini();
}
