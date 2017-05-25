//
// Created by Nikolay Ionanov on 12/05/2017.
//

#ifndef DISTRJOIN_RELATION_H
#define DISTRJOIN_RELATION_H
#include <vector>
#include <string>

using namespace std;
class Relation: public vector< vector<int> > {

public:
    int arity;

    Relation();
    void read(string filename); // reads a relation from the file
    void write(string filename); //writes a relation from a file
    void sort(vector<int> permutation);
//    int size();
    void print();
};


#endif //DISTRJOIN_RELATION_H


/*
 * vector
 *
 * Relation : privaate vector
 *
 * Relation2 : public Relation
 *
 */
