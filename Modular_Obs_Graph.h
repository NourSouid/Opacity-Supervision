

/******************    Graph.hpp  **************************/

#ifndef _Modular_ObsGraph_

#define _Modular_ObsGraph_
#include<iostream>
#include<time.h>
#include<vector>
#include"Modular_Class_of_state.h"
#include <utility>
using namespace std;

typedef set<int> Set;
typedef std::vector<Modular_Class_Of_State*> Modular_Obs_Graph_Nodes;
class Modular_Obs_Graph
{
	private:
		void printGraph(Modular_Class_Of_State *, size_t &);
		Modular_Obs_Graph_Nodes GONodes;
	public:
		void Reset();
		Modular_Class_Of_State *initialstate;
		Modular_Class_Of_State *currentstate;
		size_t nbStates;
		size_t nbMarking;
		size_t nbArcs;
		void Liberer(Modular_Class_Of_State *S);
		Modular_Class_Of_State* find(Modular_Class_Of_State*);
		Modular_Class_Of_State* find2(Modular_Class_Of_State*,Set);
		Modular_Edges& get_successor(Modular_Class_Of_State*);
		void printsuccessors(Modular_Class_Of_State *);
		void InitVisit(Modular_Class_Of_State * S,size_t nb);
		void TAB_BDDNODES(Modular_Class_Of_State *,size_t&);
		void printpredecessors(Modular_Class_Of_State *);
		void addArc(){nbArcs++;}
		void addArc(Modular_Class_Of_State*,Modular_Class_Of_State*,const char*);
		void insert(Modular_Class_Of_State*);
		Modular_Obs_Graph(){nbStates=nbArcs=nbMarking=0;}
		void setInitialState(Modular_Class_Of_State*);  //Define the initial state of this graph
		void printCompleteInformation(int nbsubnets);
};
#endif
