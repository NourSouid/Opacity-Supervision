
/********              Graph.cpp     *******/
#include"MDGraph.h"
#include"Net.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::find
#include <vector>
/*#include <conio>*/

bdd * Tab;

/********* setInitialState    *****/


void MDGraph::setInitialState(Class_Of_State *c)
{
	//cout << " initialisation  avec classe"<<  c->class_state << endl;

	currentstate=initialstate=c;
}
/*----------------------find()----------------*/
//find gnode with id equals c
//remember that c is initial state
Class_Of_State * MDGraph::find(Class_Of_State* c) //c is a pointer?
{
//MetaGrapheNodes = vector of Class_Of_State?
	for(MetaGrapheNodes::const_iterator i=GONodes.begin();!(i==GONodes.end());i++)
//from i=first value of GONodes (MetaGrapheNodes) till last, i++
    if(c->class_state.id()==(*i)->class_state.id())
    	//if c.class_state.id= *i.class_state.id
			return *i;

	//else	statement
	return NULL;
}

/*----------------------insert() ------------*/
void MDGraph::insert(Class_Of_State *c)
{
//cout<<"insert aggregate"<<endl;
	c->Visited=false;
//	c=c & fdd_ithset(2);
	//cout << "c visited "<<endl ;

	this->GONodes.push_back(c);
	//cout<<"GONOdes"<<endl;
	nbAggregates++;
	//cout<<"nb agreg : "<<nbAggregates;
}
void MDGraph::deleteAggregate(Class_Of_State* c)
{
    //cout<<"delete agregate "<< c->class_state<<" from sog "<<endl;

	this->GONodes.erase(std::remove(this->GONodes.begin(), this->GONodes.end(), c), this->GONodes.end());
//	cout<<"GONOdes remove"<<endl;
	nbAggregates--;
//	cout<<"nb agreg : "<<nbAggregates;
	delete(c);
}
/*----------------------NbBddNod()------------------------*/
int MDGraph::NbBddNode(Class_Of_State * S, size_t& nb)
{
	//cout << "appele de la methode NbBddNOde "<< endl;

	if(S->Visited==false)
	{
		//cout<<"insertion du meta etat numero :"<<nb<<"son id est :"<<S->class_state.id()<<endl;
		//cout<<"sa taille est :"<<bdd_nodecount(S->class_state)<<" noeuds \n";
		Tab[nb-1]=S->class_state;
		S->Visited=true;
		int bddnode=bdd_nodecount(S->class_state);
		//cout << " le bdd node count is "<< bddnode ;
		int size_succ=0;
		for(Edges::const_iterator i=S->Successors.begin();!(i==S->Successors.end());i++)
		{
			if((*i).first->Visited==false)
			{
				nb++;
				size_succ+=NbBddNode((*i).first,nb);
			}
		}
		return size_succ+bddnode;

	}
	else
		return 0;
}

/*----------------------Visualisation du graphe------------------------*/
void MDGraph::printCompleteInformation()
{

	cout<<" \n";
	cout << "print complete information: "<<endl;
	cout << "\n\nGRAPH SIZE : \n";
	//cout<< "\n\tNB MARKING : "<< nbMarking;
	cout<< "\n\tNB NODES : "<< nbAggregates;
	cout<<"\n\tNB ARCS : " <<nbArcs<<endl;
	cout<<" \n\nCOMPLETE INFORMATION Only when your system is opaque ?(y/n)\n";
	char c;
    cin>>c;
	//InitVisit(initialstate,n);
	Tab=new bdd[(int)nbAggregates];
	size_t n=1;
	cout<<"NB BDD NODE : "<<NbBddNode(initialstate,n)<<endl;
	NbBddNode(initialstate,n);
	//cout<<"NB BDD NODE : "<<bdd_anodecount(Tab,(int)nbStates)<<endl;
	//cout<<"Shared Nodes : "<<bdd_anodecount(Tab,nbStates)<<endl;
	InitVisit(initialstate,1);
	//int toto;cin>>toto;
	//bdd Union=UnionMetaState(initialstate,1);
	//cout<<"a titre indicatif taille de l'union : "<<bdd_nodecount(Union)<<endl;
	if(c=='y'||c=='Y')
	{
		size_t n=1;
		 printGraph(initialstate,n);
	}


}
/*----------------------InitVisit()------------------------*/
void MDGraph::InitVisit(Class_Of_State * S,size_t nb)
{
	//cout << "InitVisit function now called "<<endl;
	if(nb<=nbAggregates)
	{
		S->Visited=false;
		for(Edges::const_iterator i=S->Successors.begin();!(i==S->Successors.end());i++)
		{

				if((*i).first->Visited==true)
				{
					nb++;
					InitVisit((*i).first,nb);
				}
		}

	}
}
/*********                  printGraph    *****/

void MDGraph::printGraph(Class_Of_State *s,size_t &nb)
{
	//cout << "print graph function now is called "<<endl;
	if(nb<=nbAggregates)
	{
		cout<<"\nSTATE NUMBER "<< nb <<" : \n";
		cout<< "State root"<<bddtable<<s->class_state<<endl;
		//cout <<	s->class_state<<endl;
		s->Visited=true;
	 printsuccessors(s);
		getchar();
	printpredecessors(s);
		getchar();
		Edges::const_iterator i;
		for(i=s->Successors.begin();!(i==s->Successors.end());i++)
		{
			if((*i).first->Visited==false)
			{
				nb++;
				printGraph((*i).first, nb);
			}
		}

	}

}


/*---------void print_successors_class(Class_Of_State *)------------*/
void MDGraph::printsuccessors(Class_Of_State *s)
{
	Edges::const_iterator i;


    cout << " la classe  :  "<<s->class_state <<" \n";


	cout<< " on va affiche s-> classe of state "/*bddtable <<*/<<s->class_state<<endl;
//	if(s->boucle)
//		cout<<"\n\tON BOUCLE DESSUS AVEC EPSILON\n";
//	if(s->blocage)
//		cout<<"\n\tEXISTENCE D'UN ETAT BLOCANT\n";
	cout<<"\n\tSES SUCCESSEURS SONT  ( "<<s->Successors.size()<<" ) : \n\n"<<endl;// g changé
	//getchar();
	for(i =s->Successors.begin();!(i==s->Successors.end());i++)
	{
		cout<<" \t- t"<<(*i).second<<" ->";
		cout<<bddtable<<(*i).first->class_state<<endl;
	//
		getchar();
	}
}
/*---------void printpredescessors(Class_Of_State *)------------*/
void MDGraph::printpredecessors(Class_Of_State *s)
{ 	//cout << "MDGraph::printprecedecessors function"<<endl;
	Edges::const_iterator i;
	cout<<"\n\tSES PREDESCESSEURS SONT  ( "<<s->Predecessors.size()<<" ) :\n\n";
	//getchar();
	for(i =s->Predecessors.begin();!(i==s->Predecessors.end());i++)
	{
		cout<<" \t- t"<<(*i).second<<" ->";
		cout<<bddtable<<(*i).first->class_state<<endl;
		getchar();
	}
}

/*
*
* This method returns the transition between two aggregates
*
*
* @param  Class_Of_State* : b
*         Class_Of_State* : a
*
* @return  int: the transition between the two aggregates "a" and "b"
*/

int MDGraph::getTrans(Class_Of_State* b, Class_Of_State* a){

    for(std::vector< std::pair<Class_Of_State*,int> >::iterator it = a->Predecessors.begin(); it != a->Predecessors.end(); ++it) {
    // look in the predecessors of a
    // (*it) is a predecessor of a: it is a pair of aggregate and the transition leading to this aggregate
    // (*it).first: Class_Of_State*
    // ((*it).first->class_state) : the bdd representing the predecessor
    // (*it).second: the transition from a to b
        if( ((*it).first->class_state)== b->class_state )
        {
           // cout<<"return : "<<(*it).second<<endl;
            return (*it).second;
        }
    }//working perfectly

//   cout<<"end get Transition"<<endl;
    return -1;
}


void MDGraph::addArc(Class_Of_State* Pred, Class_Of_State* Suc, int t)
{
	cout<<"add an arc "<<Pred->class_state<<" et "<<Suc->class_state<<endl;
	std::pair< Class_Of_State*, int > arc = std::make_pair(Suc,t);
	std::pair< Class_Of_State*, int > cra = std::make_pair(Pred,t);

    vector<std::pair<Class_Of_State*,int> > ::iterator it;




    if ( std::find(Pred->Successors.begin(), Pred->Successors.end(), arc) != Pred->Successors.end() )
        {
            cout<<"OK add an arc \n";
            Pred->Successors.insert(Pred->Successors.begin(),arc);
            Suc->Predecessors.insert(Suc->Predecessors.begin(),cra);
            nbArcs++;
        }
}
