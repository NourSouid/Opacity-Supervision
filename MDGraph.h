

/******************    Graph.hpp  **************************/

#ifndef _MDGRAPH_ //prevent file from being included multiple times

#define _MDGRAPH_
#include <iostream>
/*#include <time>*/
#include <vector>
#include"Class_of_state.h"
#include <list>
typedef set<int> Set;
typedef vector<Class_Of_State*> MetaGrapheNodes;
class MDGraph
{
	private:
		void printGraph(Class_Of_State *, size_t &);
		MetaGrapheNodes GONodes;

	public:
		void Reset();
		Class_Of_State *initialstate;
		Class_Of_State *currentstate;
		double nbAggregates;
		double nbMarking;
		double nbArcs;
		Class_Of_State* find(Class_Of_State*);
		Edges& get_successor(Class_Of_State*);
		void printsuccessors(Class_Of_State *);
		int NbBddNode(Class_Of_State*,size_t&);
		void InitVisit(Class_Of_State * S,size_t nb);
		void printpredecessors(Class_Of_State *);
		void addArc(Class_Of_State* Pred, Class_Of_State* Suc, int t);

        void addArc(){/*cout<<"add arc //////////////////////////////"<<endl; */nbArcs++;}
		void deleteArc(){/*cout<<"delete arc \\\\\\\\\\\\\\\\\\\\\\\\"<<endl; */nbArcs--;}
		void insert(Class_Of_State*);
		void deleteAggregate(Class_Of_State*);
		MDGraph(){nbAggregates=nbArcs=nbMarking=0;}
		void setInitialState(Class_Of_State*);  //Define the initial state of this graph
		void printCompleteInformation();
		int getTrans(Class_Of_State*, Class_Of_State*);



};
#endif
