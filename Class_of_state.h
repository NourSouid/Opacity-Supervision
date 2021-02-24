#ifndef CLASS_OF_STATE
#define CLASS_OF_STATE
#include "bdd.h"
#include <map>
#include <set>
#include <vector>
#include <utility>
#include <ext/hash_map>
#include <iostream>
using namespace std;
typedef set<int> Set;
//no class_of_state.cpp
//the definition of functions are all included here
class Class_Of_State
{
	public:
	    /****Attributes****/
	    bdd class_state;
	    Set firable;
		//Set Marked; // un set pour mettre les marked places
		void * Class_Appartenance;

		bool boucle;
		bool blocage;
		bool Visited;//browse the graph recursively
		//if visited no need to retreat

		vector<std::pair<Class_Of_State*,int> > Predecessors, Successors;
		std::pair<Class_Of_State*,int>  LastEdge;

		/****methods****/
	    //default constructor
		Class_Of_State(){
		    boucle=blocage=Visited=0;}

		//parametric constructor
		Class_Of_State(bdd class_state, Set firable, void * Class_Appartenance, vector<std::pair<Class_Of_State*,int> > Predecessors, vector<std::pair<Class_Of_State*,int> > Successors, std::pair<Class_Of_State*,int> LastEdge, bool boucle=0, bool blocage=0, bool Visited=0){
		    this->class_state=class_state;
		    this->boucle=boucle;
            this->blocage=blocage;
            this->Visited=Visited;

		    this->firable=firable;
		    this->Class_Appartenance=Class_Appartenance;
            this->Predecessors=Predecessors;
            this->Successors=Successors;
            this->LastEdge=LastEdge;
		    }

		//copy constructor
		Class_Of_State(const Class_Of_State& c){
            this->boucle=c.boucle;
            this->blocage=c.blocage;
            this->Visited=c.Visited;
		    this->class_state=c.class_state;
		    this->firable=c.firable;
		    this->Class_Appartenance=c.Class_Appartenance;
            this->Predecessors=c.Predecessors;
            this->Successors=c.Successors;
            this->LastEdge=c.LastEdge;//????
		}

		/**overload of useful operators**/
		//assignment operator
		//member function
		Class_Of_State& operator= (Class_Of_State &c){
		    if(&c!=this){
            this->boucle=boucle;
            this->blocage=blocage;
            this->Visited=Visited;
            this->class_state=c.class_state;
		    this->firable=c.firable;
		    this->Class_Appartenance=c.Class_Appartenance;
            this->Predecessors=c.Predecessors;
            this->Successors=c.Successors;
            this->LastEdge=c.LastEdge;
            }
            return (*this);}

        //comparison
        bool operator==(Class_Of_State const& a);
        bool operator!=(Class_Of_State const& a);

        //minus operator
        //independent function friend of the class
        //implementation in "Class_of_state.cpp
        friend Class_Of_State operator-(Class_Of_State& a, Class_Of_State& b);
        friend Class_Of_State operator+(Class_Of_State& a, Class_Of_State& b);
};
typedef pair<Class_Of_State*, int> Edge;
typedef vector<Edge> Edges;

#endif
