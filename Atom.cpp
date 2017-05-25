//
//  Atom.cpp
//  DistrJoin
//
//  Created by Nikolay Ionanov on 23/05/2017.
//  Copyright © 2017 Nikolay Ionanov. All rights reserved.
//

#include "Atom.hpp"
#include <iostream>
using namespace std;
Atom::Atom(Relation rel, vector<string> var){
    this->rel = rel;
    this->var = var;
    
    for(int i = 0; i < var.size(); i++)
        varId[var[i]] = i+1;
}

Atom::Atom(vector<string> var){
    this->var = var;
    this->rel.arity = (int)var.size();
    for(int i = 0; i < var.size(); i++)
        varId[var[i]] = i+1;
}

Atom::Atom(){
    
}

void Atom::print(){
    for (auto it = this->var.begin(); it < this->var.end(); it++)
        cout << *it << " ";
    cout << endl;
    this->rel.print();
}
