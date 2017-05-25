#include <iostream>
#include <vector>
#include "Relation.h"
#include "join.hpp"
#include "Atom.hpp"

using namespace std;
int main() {
    string source = "facebook.txt";
    string test_source1 = "test1.txt";
    string test_source2 = "test2.txt";
    string test_source3 = "test3.txt";
    string dest = "facebook_out.txt";
    
    Relation r,r1,r2,r3;
    vector<string> var1{"x", "y", "z"};
    vector<string> var2{"y", "z", "t"};
    vector<string> var3{"z", "t", "f"};
    
    r1.read(test_source1);
    r2.read(test_source2);
    r3.read(test_source3);
    Atom atom1(r1, var1);
    Atom atom2(r2, var2);
    Atom atom3(r3, var3);
    
//    atom1.rel.print();
//    cout << endl;
//    atom2.rel.print();
    
    Atom temp = join(atom1,atom2);
    Atom a = join(temp, atom3);
    cout << endl;
    a.print();
//    Relation r;
//    r.read(source);
//    cout << "Arity " << r.arity << " Size "<< r.size() <<endl;
//    vector<int> v = {2,1};
//    r.sort(v);
//    r.print();
    return 0;
}
