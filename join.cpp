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
#include <mpi.h>
#include <stdarg.h>

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

void printVec(vector<string> v){
    for (auto it = v.begin(); it < v.end(); it++) {
        cout << *it << " ";
    }
    cout << endl;
}

int sum(vector<int> v){
    int sum = 0;
    for(auto it = v.begin(); it < v.end(); it++)
        sum+=(*it);
    return sum;
}

//Hash function
int hash(int val, int range){
    
    return (val*2654435761 % 2^range) % range;
}

//mergest atom1 and atom2
void merge(Atom& res, Atom& atom1, Atom& atom2, vector<int>& perm1, vector<int>& perm2, vector<string> comVar,vector<string> newVar){
    auto it1 = atom1.rel.begin();
    auto it2 = atom2.rel.begin();
    while((it1 < atom1.rel.end()) && (it2 < atom2.rel.end())){
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
}


//Returns the resulting Atom after joining atom1 and atom2
Atom join( Atom& atom1, Atom& atom2) {
    
    //counter and rank for the distributed part
    int rank, nprocs;
    
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
    
    //the resulting atom
    Atom res(newVar);
    
    //sorting the relations r1 and r2 according to perm1 and perm2
    atom1.rel.sort(perm1);
    atom2.rel.sort(perm2);
    
    //Get the number of proccessors and the rank of the current one
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    vector<int> atomSizes(nprocs); //stocks the sizes of atoms in every processor
    
    if(comVar.empty()){
        //juste merge the initial arguments
        merge(res, atom1, atom2, perm1, perm2, comVar,newVar);
    }else {
    
        //creating groups according to a processor
        Atom aux1(atom1.var);
        Atom aux2(atom2.var);
        
        int varId1 = atom1.varId[comVar[0]];
        int varId2 = atom2.varId[comVar[0]];
        
        for (auto it = atom1.rel.begin(); it < atom1.rel.end(); it++)
            if((*it)[varId1-1] % nprocs == rank)
                aux1.rel.push_back(*it);
        
        for (auto it = atom2.rel.begin(); it < atom2.rel.end(); it++)
            if((*it)[varId2-1] % nprocs == rank)
                aux2.rel.push_back(*it);
        
        //merge
        merge(res, aux1, aux2, perm1, perm2, comVar,newVar);
        
        //form the vector of sizes of atoms
        int cur_size = (int)res.rel.size();
        MPI_Gather(&cur_size, 1, MPI_INT, atomSizes.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        if(rank == 0){
            cout << "vector of sizes ";
            printVec(atomSizes);
        }
        
        //assembling the result at proccessor 0
        if(rank == 0){
            int arity = res.rel.arity;
            for(int i = 1; i < nprocs; i++)
            {
                //receive the tuples from all of the processors
                vector<int> tmp(arity);
                for (int j = 0; j < atomSizes[i]; j++) {
                    MPI_Recv(tmp.data(), arity, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    res.rel.push_back(tmp);
                }
            }
            cout << "Root received " << res.rel.size() << " tuples" << endl;
        } else {
            //send res to the proccessor zero
            int arity = res.rel.arity;
            for(int i = 0; i < cur_size; i++)
                MPI_Send(res.rel[i].data(), arity, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    return res;
}

//compute a multiple join
Atom join(vector<Atom> atoms){

    if(atoms.size() == 1)
        return atoms[0];

    int rank, nprocs;
//    Atom res = atoms[0];
    //Get the number of proccessors and the rank of the current one
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //Initialize atom1
    Atom atom1 = atoms[0];
    for (int i = 1; i < atoms.size(); i++){
        
        Atom atom2 = atoms[i];
        //vector of common variables of in cur and res
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
        
        //sorting the relations r1 and r2 according to perm1 and perm2
        atom1.rel.sort(perm1);
        atom2.rel.sort(perm2);
        
        //joint variables
        vector<string> newVar(atom1.var);
        for (auto it = atom2.var.begin(); it < atom2.var.end(); it++)
            if(!contains(comVar, *it))
                newVar.push_back(*it);
        
//        cout << "Newvar "<< newVar.size() << endl;
        Atom locRes(newVar);
        
        if(!comVar.empty())
        {
            //if comVar isn't empty we distribute the data (atom1 and atom2)
            Atom aux1(atom1.var);
            Atom aux2(atom2.var);
//            cout<< "Proc " << rank << " Comvar ";
//            printVec(comVar);
            //form aux1
            /////////////
            //data-transfer
            int varId1 = atom1.varId[comVar[0]];
            int varId2 = atom2.varId[comVar[0]];
            if(i!=1){
                vector<int> to_send(nprocs);
                vector<vector<int>> to_send_id(nprocs);
                vector<int> to_recv(nprocs);
                vector<vector<int>> to_recv_id(nprocs);
            
                for (int i = 0; i < atom1.rel.size(); i++) {
                    int procNumber = atom1.rel[i][varId1-1] % nprocs;
                    to_send[procNumber]++;
                    to_send_id[procNumber].push_back(i);
                }
            
                MPI_Alltoall(to_send.data(), 1, MPI_INT, to_recv.data(),1, MPI_INT, MPI_COMM_WORLD);
                cout << "Proc " << rank << " to_send ";
                printVec(to_send);
                //send to all processors
                int arity = atom1.rel.arity;
                for(int i = 0; i < nprocs; i++)
                    for(int j = 0; j < to_send[i]; j++){
                        vector<int> tmp = atom1.rel[to_send_id[i][j]];
                    
                        MPI_Request req;
                        MPI_Isend(tmp.data(), arity, MPI_INT, i, 0,MPI_COMM_WORLD, &req);
                        MPI_Request_free(&req);
                    }
            
                //receive from all processors
                int elNum = 0; //total number of elements to receive
                for (int i = 0; i < to_recv.size(); i++)
                    elNum+=to_recv[i];
                arity = atom1.rel.arity;
                aux1.rel.resize(elNum, vector<int> (arity));
            
                int pos = 0;
                for(int i = 0; i < nprocs; i++)
                    for(int j = 0; j < to_recv[i]; j++)
                    {
                        MPI_Request req;
                        MPI_Irecv(aux1.rel[pos].data(), arity, MPI_INT, i, 0, MPI_COMM_WORLD, &req);
                        MPI_Request_free(&req);
                        pos++;
                    }
            
                //SYNCH Barrier
                MPI_Barrier(MPI_COMM_WORLD);
//                cout << "Proc " << rank << " received" << endl;
//                aux1.print();
            } else {
                for (auto it = atom1.rel.begin(); it < atom1.rel.end(); it++)
                    if((*it)[varId1-1] % nprocs == rank)
                        aux1.rel.push_back(*it);
            }
            /////////////////////////////
            //form aux2
            for (auto it = atom2.rel.begin(); it < atom2.rel.end(); it++)
                if((*it)[varId2-1] % nprocs == rank)
                    aux2.rel.push_back(*it);
            /////////////////////////////
            merge(locRes, aux1, aux2, perm1, perm2, comVar, newVar);
            atom1 = locRes;
//            cout << "Proc " << rank << " i="<<i <<  endl;
//            locRes.print();
        }
    }
    //Send all the data to the process 0
    Atom& res = atom1;
    
    vector<int> atomSizes(nprocs);//form the vector of sizes of atoms
    int cur_size = (int)res.rel.size();
    MPI_Gather(&cur_size, 1, MPI_INT, atomSizes.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    
    if(rank == 0){
        cout << "vector of sizes ";
        printVec(atomSizes);
        cout << "Total " << sum(atomSizes) << endl;
        int arity = res.rel.arity;
        for(int i = 1; i < nprocs; i++)
        {
            //receive the tuples from all of the processors
            vector<int> tmp(arity);
            for (int j = 0; j < atomSizes[i]; j++) {
                MPI_Recv(tmp.data(), arity, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                res.rel.push_back(tmp);
            }
        }
    } else {
        //send res to the proccessor zero
        int arity = res.rel.arity;
        for(int i = 0; i < cur_size; i++)
            MPI_Send(res.rel[i].data(), arity, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    
    return res;
}
