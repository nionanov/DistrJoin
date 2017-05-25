//
//  join.cpp
//  DistrJoin
//
//  Created by Nikolay Ionanov on 21/05/2017.
//  Copyright © 2017 Nikolay Ionanov. All rights reserved.
//

#include "join.hpp"
#include "Relation.h"
#include <assert.h>
#include <iostream>
#include <map>
#include "Atom.hpp"

using namespace std;
//void filter (Relation& r, vector<string>& var){
//    for(int i = 0; i < var.size(); i ++)
//        


//verifies if a vector contains the element
bool contains(vector<string> v, string element){
    if(find(v.begin(), v.end(), element) != v.end())
        return true;
    
    return false;
}

bool coincide(vector<int>& vec1, vector<int>& perm1, vector<int>& vec2, vector<int>& perm2){
    assert(perm1.size() == perm2.size());
    for (int i = 0; i < perm1.size(); i ++)
        if(vec1[perm1[i]-1] != vec2[perm2[i]-1])
            return false;
    return true;
}

bool more(vector<int>& vec1, vector<int>& perm1, vector<int>& vec2, vector<int>& perm2){
    assert(perm1.size() == perm2.size());
    
    for(int i = 0; i < perm1.size(); i ++)
        if(vec1[perm1[i]-1] < vec2[perm2[i]-1])
            return false;
    
    return true;
}

void printVec(vector<int>&v ){
    for (auto it = v.begin(); it < v.end(); it++) {
        cout << *it << " ";
    }
    cout << endl;
}

//Returns the resulting Atom after joining atom1 and atom2
Atom join( Atom& atom1, Atom& atom2) {
    
    //vector of common variables of in r1 and r2 (taken from var1 and var2)
    vector<string> comVar;
    
    //permutations serving to sort the relations
    vector<int> perm1;
    vector<int> perm2;
    
    //form the comVar
    for(auto it = atom1.var.begin(); it < atom1.var.end(); it++)
        if(contains(atom2.var, *it)){
            perm1.push_back(atom1.varId[*it]);
            perm2.push_back(atom2.varId[*it]);
            comVar.push_back(*it);
        }
    
    //joint variables
    vector<string> newVar(atom1.var);
    
    for (auto it = atom2.var.begin(); it < atom2.var.end(); it++)
        if(!contains(comVar, *it))
            newVar.push_back(*it);
    
    Atom res(newVar);
    
    //sorting the relations r1 and r2 according to perm1 and perm2
//    cout << perm1.size() << " "<< atom1.rel.arity<<endl;
//    cout << perm2.size() << " "<< atom2.rel.arity<<endl;
    atom1.rel.sort(perm1);
    atom2.rel.sort(perm2);
    
    atom1.rel.print();
    cout << endl;
    atom2.rel.print();
    
    vector<int> vec1{1,2,3};
    vector<int> vec2{2,3,4};
//    cout << "vec1 < vec2 " << more(vec1, perm1, vec2, perm2) << endl;
//    cout << "Coincide " << coincide(vec1, perm1, vec1, perm1) << endl ;
//    cout << "Coincide " << coincide(vec2, perm2, vec2, perm2) << endl ;
//    cout << "Coincide " << coincide(vec1, perm1, vec2, perm2) << endl ;
    
    //merge
    auto it1 = atom1.rel.begin();
    auto it2 = atom2.rel.begin();
    while((it1 < atom1.rel.end()) && (it2 < atom2.rel.end())){
//        cout << "lol" << endl;
//        printVec(*it1);
//        printVec(*it2);
//        cout << "Coincide " << coincide(*it1, perm1, *it2, perm2) << endl;
        if(coincide(*it1, perm1, *it2, perm2)){
            auto it11 = it1;
            auto it22 = it2;
            for (it11 = it1; (it11 < atom1.rel.end()) && coincide(*it11, perm1, *it1, perm1); it11++) {
                for (it22 = it2; (it22 < atom2.rel.end()) && coincide(*it22, perm2, *it2, perm2); it22++){
                    vector<int> tuple(newVar.size(), 0);
                    for (auto it = atom1.var.begin(); it < atom1.var.end(); it++) {
                        int oldId = atom1.varId[*it] - 1;
                        int newId = res.varId[*it] - 1;
                        tuple[newId] = (*it11)[oldId];
                    }
                    
                    for (auto it = atom2.var.begin(); it < atom2.var.end(); it++)
                        if(!contains(comVar, *it))
                        {
                            int oldId = atom2.varId[*it] - 1;
                            int newId = res.varId[*it] - 1;
                            tuple[newId] = (*it22)[oldId];
                        }
                    res.rel.push_back(tuple);
                }
            }
            it1 = it11;
            it2 = it22;
        }
        else if(more(*it1, perm1, *it2, perm2)){
            it2++;
        } else{
            it1++;
        }
    }
    
    return res;
}
