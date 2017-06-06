#include <iostream>
#include <vector>
#include "Relation.h"
#include "join.hpp"
#include "Atom.hpp"
#include <mpi.h>
using namespace std;


int hashCode(int val, int range){
    
    return (val*2654435761 % 2^range) % (range+1) - 1;
}

int main(int argc, char *argv[]) {
    string source = "facebook.txt";
    string test_source1 = "test1.txt";
    string test_source2 = "test2.txt";
    string test_source3 = "test3.txt";
    string test_source4 = "test4.txt";
    string dest = "facebook.txt";
    
    Relation r,r1,r2,r3,r4;
    vector<string> var1{"x", "y"};
    vector<string> var2{"y", "z"};
    vector<string> var3{"z", "x"};
    vector<string> var4{"t", "x"};
    
    int rank;
    
    r1.read(dest);
    r2.read(dest);
    r3.read(test_source3);
    r4.read(test_source4);
    Atom atom1(r1, var1);
    Atom atom2(r2, var2);
    Atom atom3(r3, var3);
    Atom atom4(r4, var4);
    
//    atom1.rel.print();
//    cout << endl;
//    atom2.rel.print();
    
    
    //Ex4 - 5
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//    cout << "nprocs = " << size << endl;
    
//    vector<Atom> atoms{atom1, atom2, atom3};
//    Atom a = join(atoms);
    Atom a = join(atom1, atom2);
    
    
    
    
    //end of the parallel part
//    cout << "LOL" << " processor " << rank << endl;
//    a.print();
    if(rank == 0)
        a.print();
    MPI_Finalize();
//    Relation r;
//    r.read(source);
//    cout << "Arity " << r.arity << " Size "<< r.size() <<endl;
//    vector<int> v = {2,1};
//    r.sort(v);
//    r.print();
    return 0;
}


