#ifndef OS_OPACITY MANAGER INTERFACE_H
#define OS_OPACITY MANAGER INTERFACE_H

#include <iostream>
#include <vector>
#include <stack>
#include "../RdPBDD.h"
#include"../MDGraph.h"
#include "Net.h"
#include "../Class_of_state.h"
#include "OS_Supervision_Manager.h"
using namespace std;
/**** an interface that manages the OpacityManager ****/
class OS_Opacity_Manager_Interface
{
    public:
        /***attributes***/

        /***operations***/
        /*constructor destructor*/
        OS_Opacity_Manager_Interface();//Can't be instantiated but can have a constructor which will be called by derived classes
        //[A class that contains at least one pure virtual function is considered an abstract class]
        virtual ~OS_Opacity_Manager_Interface();

        /*other methods*/
        virtual bool OpacifyByControl(stack<StackElt> st, vector<Class_Of_State*> src, vector<Class_Of_State*> dest, Transition t, MDGraph SOG, bdd Secret )=0;
        // not sure if StackElt is the correct type to use! //same as Transition
        virtual void CutOldBranches(Class_Of_State*, MDGraph&)=0;
        virtual void SlimOldBranches(Class_Of_State*, MDGraph&)=0;
        virtual void computeAndOpacify_SOG(MDGraph& g)=0;
    protected:

    private:
};

#endif // OS_OPACITY MANAGER INTERFACE_H
