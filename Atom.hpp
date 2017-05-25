//
//  Atom.hpp
//  DistrJoin
//
//  Created by Nikolay Ionanov on 23/05/2017.
//  Copyright © 2017 Nikolay Ionanov. All rights reserved.
//

#ifndef Atom_hpp
#define Atom_hpp

#include "Relation.h"
#include "map"

class Atom {
public:
    Relation rel; // underlying relation
    vector<string> var; // variables
    map<string, int> varId; // indices of variables
    void print();
    Atom(Relation rel, vector<string> var);
    Atom(vector<string> var);
    Atom();
};

#endif /* Atom_hpp */
