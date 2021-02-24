/**
 * Implementation of an Opacity_Manager
 * A class that manages opacity of a model
 * Creates a supervisor @see OS_Supervisory_Manager
 * implements @see OS_Opacity_Manager_Interface
 * contains implementation of  OpacifyByControl(), CutOldBranches() and SlimOldBranches()
 *
 *
 */

#ifndef OS_OPACITY_MANAGER_H
#define OS_OPACITY_MANAGER_H

#include <iostream>
#include <vector>
#include <stack>
#include<set>
#include <algorithm> //std::sort
#include "../RdPBDD.h"
#include"../MDGraph.h"
#include "Net.h"
#include "../Class_of_state.h"
#include "OS_Opacity_Manager_Interface.h"
#include "Model.h"
#include "OS_Attacker.h"
using namespace std;
class OS_Opacity_Manager
{
    public:
        /***attributes***/
        string name;
        Model* model;
        vector<OS_Attacker> attackers;
        OS_Supervision_Manager supervisoryManager;
        /***operations***/
        /*constructor destructor*/
        //Default constructor
        OS_Opacity_Manager();
        //Parametric constructor
        //add a constructor with a default name
        OS_Opacity_Manager( Model* model, vector<OS_Attacker> attackers, OS_Supervision_Manager supervisoryManager, string name="undefined");
        //copy constructor
        OS_Opacity_Manager(const OS_Opacity_Manager& );
        //destructor
        virtual ~OS_Opacity_Manager();
        /*getter setter*/
        string getName();
        Model* getManagedModel()const;
        OS_Supervision_Manager getSupervisoryManager();

        void setManagedModel (Model* model);
        void setName(string name);
        void setSupervisoryManager(OS_Supervision_Manager supervisoryManager);

        /*other methods*/
        MDGraph backtracking (stack<Pair>, vector<Class_Of_State*>, vector<Class_Of_State*> , Pair);
        bool opacifyByControl(MDGraph& sog,stack<Pair> st, bdd src, bdd dest, int t, Class_Of_State* a_, bdd S, string trace );
        bool opacifyByControl_Version2(MDGraph& sog,stack<Pair> st, bdd src, bdd dest, int t, Class_Of_State* a_, bdd S  );
        void cutOldBranches(Class_Of_State*, MDGraph&);
        void slimOldBranches(Class_Of_State*, MDGraph&);
        void computeAndOpacify_SOG(MDGraph& g);

        /*Flow operators overload*/
        friend ostream& operator<<(ostream& flux, const OS_Opacity_Manager& manager);
    protected:

    private:
};

#endif // OS_OPACITY MANAGER_H
