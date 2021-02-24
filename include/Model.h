#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include "bdd.h"

#include "Net.h"
#include"../MDGraph.h"
#include"../Modular_Obs_Graph.h"
#include "../Modular_Class_of_state.h"
using namespace std;

class Model
{
    public:
        bdd M0;
        bdd SecretBdd;
        bdd cross;

        //methods
        Model();
        virtual ~Model();

        virtual bool Set_Div(bdd &S) const=0;
        virtual bool Set_Bloc(bdd &S) const=0;
        virtual bdd CanonizeR(bdd s, unsigned int i) =0;

        //EnableObs
        virtual Set firable_obs(bdd State)=0;

        //saturation
        virtual bdd Accessible_epsilon (bdd From)=0;

        //from bdd to bdd
        virtual bdd StepForward(bdd From)=0;
        virtual bdd StepBackward(bdd From)=0;

        //from bdd and transition to bdd
        virtual bdd get_successor(bdd From,int t)=0;
        virtual bdd GetPrerel(bdd s , int t)=0;

    protected:

    private:
};

#endif // MODEL_H
