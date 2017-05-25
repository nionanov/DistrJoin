//
// Created by Nikolay Ionanov on 12/05/2017.
//

#include "Relation.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <algorithm>

using namespace std;
void Relation::read(string filepath) {
    ifstream file(filepath);
    string line;
    this->clear();
    int i;
    if(file.is_open()){
        while(getline(file,line)) {
            vector<int> vec;
            istringstream iss(line);
            while (iss >> i) {
                vec.push_back(i);
            }
            push_back(vec);
        }
        if(this->size() > 0)
            this->arity = (int)(*this)[0].size();
    } else {
        cout << "Unable to open file " <<filepath << endl;
    }
}


//int Relation::size(){
//    return (int)vector< vector<int> >::size();
//}

Relation::Relation(){
    this->arity = 0;
}

void Relation::print() {
    for (auto it = this->begin(); it < this->end(); it++) {
        for(auto it2 = it->begin(); it2 < it->end(); it2++)
            cout << *it2 << " ";
        cout << endl;
    }
}

class cmp{
public:
    vector<int> permutation;
    cmp(vector<int> permutation){
        this->permutation = permutation;
    }
    
    bool operator ()(vector<int> v1, vector<int> v2) const{
        
        for (auto it = permutation.begin(); it < permutation.end(); it ++) {
            if(v1[(*it)-1] > v2[(*it)-1]){
                return false;
            }
            else if(v1[(*it)-1] < v2[(*it)-1]){
                return true;
            }
        }
        return false;
    }
};


void Relation::sort(vector<int> permutation){
    assert(permutation.size() <= this->arity);
    cmp comp(permutation);
    std::sort(this->begin(), this->end(),comp);
}


void Relation::write(string filename) {
    ofstream file(filename);
    if(file.is_open()){
        for (int i = 0; i < this->size(); i++){
            for (int j = 0; j < this->arity; j++) {
                file << (*this)[i][j] << " ";
            }
            file << endl;
        }
    } else {
        cout << "File doesn't exist" << endl;
    }
}
