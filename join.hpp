//
//  join.hpp
//  DistrJoin
//
//  Created by Nikolay Ionanov on 21/05/2017.
//  Copyright © 2017 Nikolay Ionanov. All rights reserved.
//

#ifndef join_hpp
#define join_hpp

#include <stdio.h>
#include "Atom.hpp"

Atom join(Atom& atom1, Atom& atom2);
Atom join(vector<Atom> atoms);
Atom hyperCubeJoin(vector<Atom> atoms);
#endif /* join_hpp */
