
/********              Graph.cpp     *******/
#include"Modular_Obs_Graph.h"
#include <utility>
#include <iostream>
using namespace std;

bdd * Temp;//tableau interm�diaire pour calculer la taille (nb bdd) du graphe
/*********                  setInitialState    *****/

void Modular_Obs_Graph::setInitialState(Modular_Class_Of_State *c)
{
	//cout << "initialisaiton state"<<endl ;
	currentstate=initialstate=c;

}
/*----------------------find()----------------*/
Modular_Class_Of_State * Modular_Obs_Graph::find(Modular_Class_Of_State* c)
{
	//cout<< "find function"<<endl ;
	bool arret;
	for(Modular_Obs_Graph_Nodes::const_iterator i=GONodes.begin();!(i==GONodes.end());i++)
	{
		//if((c->blocage!=(*i)->blocage)||(c->boucle!=(*i)->boucle))
		//{
			arret=false;
			for(unsigned int k=0;((k<(c->State).size())&&(!arret));k++)
				if(!(c->State[k]==(*i)->State[k]))
					arret=true;
			if(!arret)
				return *i;
		//}
	}
		return NULL;
}
/*----------------------find2()----------------*/
//version modulaire on v�rifie la projection
Modular_Class_Of_State * Modular_Obs_Graph::find2(Modular_Class_Of_State* c, Set SRConcernes)
{
	bool arret;
	for(Modular_Obs_Graph_Nodes::const_iterator i=GONodes.begin();!(i==GONodes.end());i++)
	{
		//if((c->blocage!=(*i)->blocage)||(c->boucle!=(*i)->boucle))
		//{
			arret=false;
			for(unsigned int k=0;((k<(c->State).size())&&(!arret));k++)
				if(!(SRConcernes.find(k)==SRConcernes.end()))
					if(!(c->State[k]==(*i)->State[k]))
						arret=true;
			if(!arret&&(c->blocage==(*i)->blocage)&&(c->boucle==(*i)->boucle))
				return *i;
		//}
	}
		return NULL;
}
/*-----------------------AddARc2----------------*/
void Modular_Obs_Graph::addArc(Modular_Class_Of_State* Pred,Modular_Class_Of_State* Suc,const char* t)
{

	cout<<"ici addArc entre "<<*Pred<<" et "<<*Suc<<endl;
	Modular_Edge arc=Modular_Edge(Suc,t);
	Modular_Edge cra=Modular_Edge(Pred,t);
	if(Pred->Successors.find(arc)==Pred->Successors.end())
	{
		cout<<"OK \n";
		Pred->Successors.insert(Pred->Successors.begin(),arc);
		Suc->Predecessors.insert(Suc->Predecessors.begin(),cra);
		nbArcs++;
	}
//	else
//	/cout<<"KO \n";
//	int toto;cin>>toto;
}
/*----------------------insert() ------------*/
void Modular_Obs_Graph::insert(Modular_Class_Of_State *c)
{
	//cout<< "insert function"<<endl ;
	c->Visited=false;
	this->GONodes.push_back(c);
	nbStates++;
}

/*----------------------InitVisit()------------------------*/
void Modular_Obs_Graph::InitVisit(Modular_Class_Of_State * S,size_t nb)
{
	cout<<"____________ nb = __________________ "<<nb<<endl;
	cout<<"____________ nbStates = __________________ "<<nbStates<<endl;
	if(nb<=nbStates)
	{
		cout<<"nb < = nbStates\n";
		S->Visited=false;
		for(Modular_Edges::const_iterator i =S->Successors.begin();!(i==S->Successors.end());i++)
		{

				if((*i).first->Visited==true)
				{
					nb++;
					InitVisit((*i).first,nb);
				}
		}

	}
}
/*----------------------NbBddNod()------------------------*/
void Modular_Obs_Graph::TAB_BDDNODES(Modular_Class_Of_State * S, size_t &nb)
{
	if(S->Visited==false)
	{
		//cout<<"ETAT NON VISITE : ";
		//cout<<*S<<endl;
		//int tt;cin>>tt;
		for(unsigned int k=0;k<S->State.size();k++,nb++)
			Temp[nb-1]=S->State[k];
		nb--;
		S->Visited=true;
		//int bddnode=0;
		//for(k=0;k<S->State.size();k++)
		//	bddnode+=bdd_nodecount(S->State[k]);
		//int size_succ=0;
		for(Modular_Edges::const_iterator i=S->Successors.begin();!(i==S->Successors.end());i++)
		{
			if((*i).first->Visited==false)
			{
				nb++;
				TAB_BDDNODES((*i).first,nb);
			}
		}
	}
}
/*------------------------------------------Affichage du graphe -------------*/
void Modular_Obs_Graph::printCompleteInformation(int nbsubnets)
{


	cout << "\n\nGRAPH SIZE : \n";
	//cout<< "\n\tNB MARKING : "<< nbMarking;
	cout<< "\n\tNB NODES : "<< nbStates;
	cout<<"\n\tNB ARCS : " <<nbArcs<<endl;
	cout<<" \n\nCOMPLETE INFORMATION ?(y/n)\n";
	char c;
	cin>>c;
	//InitVisit(initialstate,n);
	Temp=new bdd[nbStates*nbsubnets];
	size_t n=1;
	//cout<<"NB BDD NODE : "<<NbBddNode(initialstate,n)<<endl;
	TAB_BDDNODES(initialstate,n);
	cout<<"NB BDD NODE : "<<bdd_anodecount(Temp,nbStates*nbsubnets)<<endl;
	//cout<<"Shared Nodes : "<<bdd_anodecount(Tab,nbStates)<<endl;
	InitVisit(initialstate,1);
	if(c=='y'||c=='Y')
	{
		size_t n=1;
		 printGraph(initialstate,n);
	}

}
/*********                  printGraph    *****/

void Modular_Obs_Graph::printGraph(Modular_Class_Of_State *s,size_t &nb)
{
	cout<<"Print Graph \n";
	//int toto;cin>>toto;
	if(nb<=nbStates)
	{
		cout<<"\nSTATE NUMBER "<<nb<<" : \n";
		s->Visited=true;
		cout << "on va calculer les  successors"<< endl;
		printsuccessors(s);
		getchar();
		cout << "on va calculer les  predecessors"<< endl;
		printpredecessors(s);
		getchar();
		Modular_Edges::const_iterator i;
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
/*-------------------------Reset()----------------------------*/
void Modular_Obs_Graph::Reset()
{
	currentstate=NULL;
	nbArcs=nbMarking=nbStates=0;
}
/*---------void print_successors_class(Class_Of_State *)------------*/
void Modular_Obs_Graph::printsuccessors(Modular_Class_Of_State *s)
{
	//cout << "fonction de calcule de successor "<<endl ;
	Modular_Edges::const_iterator i;
	cout<<*s<<endl;
	if(s->boucle)
		cout<<"\n\tON BOUCLE DESSUS AVEC EPSILON\n";
	if(s->blocage)
		cout<<"\n\tEXISTENCE D'UN ETAT BLOCANT\n";
	cout<<"\n\tSES SUCCESSEURS SONT  ( "<<s->Successors.size()<<" ) :\n\n";
	getchar();
	for(i =s->Successors.begin();!(i==s->Successors.end());i++)
	{
		cout<<" \t---"<<(*i).second<<" -->";
		cout<<*((*i).first)<<endl;
		getchar();
	}
}
/*---------void printpredescessors(Class_Of_State *)------------*/
void Modular_Obs_Graph::printpredecessors(Modular_Class_Of_State *s)
{ 	//cout << "fonction de calcule de predecessor "<<endl ;
	Modular_Edges::const_iterator i;
	cout<<"\n\tSES PREDESCESSEURS SONT  ( "<<s->Predecessors.size()<<" ) :\n\n";
	getchar();
	for(i =s->Predecessors.begin();!(i==s->Predecessors.end());i++)
	{
		cout<<" \t---"<<(*i).second<<" -->";
		cout<<*(*i).first<<endl;
		getchar();
	}
}

