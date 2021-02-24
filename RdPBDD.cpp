/* -*- C++ -*- */
#include <string.h>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <stack>
#include <utility>
#include <ext/hash_map>
#include <math.h>
#include <sstream>


std::string IntToString ( int number )
{
  std::ostringstream oss;

  // Works just like cout
  oss<< number;

  // Return the underlying string
  return oss.str();
}
#include <set>
#include <fstream>
#include <algorithm>
#include "bvec.h"
#include "RdPBDD.h"

using namespace std;
int NbIt;
int itext, itint;
int MaxIntBdd;
bdd *TabMeta;
int nbmetastate;
double old_size;
//Qunatified opacity variables
vector<double> PAggs;
double P;
bool QOne=false;

const vector<class Place> *vplaces = NULL;
void my_error_handler(int errcode) {
	if (errcode == BDD_RANGE) {
		// Value out of range : increase the size of the variables...
		// but which one???
		bdd_default_errhandler(errcode);
	} else {
		bdd_default_errhandler(errcode);
	}
}

/*****************************************************************/
/*                         printhandler                          */
/*****************************************************************/
void printhandler(ostream &o, int var) {
	o << (*vplaces)[var / 2].name; //writes node name (node type is place) in file o
	if (var % 2)
		o << "_p";
}

/*****************************************************************/
/*                         class Trans                           */
/*****************************************************************/
Trans::Trans(){
//Default constructor
}

Trans::Trans(const bdd& v, bddPair* p, const bdd& r, const bdd& pr,
		const bdd& pre, const bdd& post) :
		var(v), pair(p), rel(r), prerel(pr), Precond(pre), Postcond(post) {
	//cout << "Trans" << endl;
}

Trans::Trans(const Trans& src){
    this->var= src.var;
    this->pair=src.pair;
    this->rel=src.rel;
    this->prerel=src.prerel;
	this->Precond=src.Precond;
	this->Postcond=src.Postcond;
//copy constructor
}

bdd Trans::getRel(void){
return this->rel;
}

bdd Trans::getVar(void){
return this->var;
}


void Trans::setRel(bdd a){
 this->rel=a;
}

void Trans::setVar(bdd a){
 this->var=a;
}
Trans& Trans::operator= (Trans &t){
		    if(&t!=this){
            this->pair=pair;
            this->rel=rel;
            this->prerel=prerel;
            this->Precond=t.Precond;
		    this->Postcond=t.Postcond;
            }
            return (*this);
}

//Franchissement avant
bdd Trans::operator()(const bdd& n) const {
	bdd res;
	//cout << "Franchissement avant" << endl;
	//cout << "(n, rel, var) " << n<<"___"<< rel<<"___"<< var;
	res = bdd_relprod(n, rel, var);// 1.produit relationel entre n et rel

	res = bdd_replace(res, pair); // 2.remplacer avec le resultat du produit
	return res;
}
//Franchissement arrière
bdd Trans::operator[](const bdd& n) const {
	//cout << "Franchissement arrière" << endl;
	bdd res = bdd_relprod(n, prerel, var);	// produit
	// var, prerel --> bdd
	res = bdd_replace(res, pair);	                             // remplacement
	//cout<<"res: "<<res<<endl;
	return res;
}


/*****************************************************************/
/*                         class RdPBDD                          */
/*****************************************************************/

RdPBDD::RdPBDD(const net &R, int BOUND, bool init) { //BOUND = b?? marquage maximal??
	int nbPlaces = R.places.size(), i, domain;
	vector<Place>::const_iterator p;
	set<int>::iterator myIterator;
	set<int>::iterator It;

	//cout<<"Bound is: "<<BOUND<<endl;//just checking if bound is b from main.cpp

	// bvec the data structure that contains booelan vector

	bvec *v = new bvec[nbPlaces];
	bvec *vp = new bvec[nbPlaces];
	bvec *prec = new bvec[nbPlaces];
	bvec *postc = new bvec[nbPlaces];
	int *idv = new int[nbPlaces];
	int *idvp = new int[nbPlaces];
	int *nbbit = new int[nbPlaces];
	if (!init)
		bdd_init(1000000, 10000);
	// the error handler
	bdd_error_hook((bddinthandler) my_error_handler);
	//_______________
	// each variable takes its values from the values of the net R
	transitions = R.transitions;
	Observable = R.Observable;
	NonObservable = R.NonObservable;
	Formula_Trans = R.Formula_Trans;
	transitionName = R.transitionName;
	InterfaceTrans = R.InterfaceTrans;
	placeName = R.placeName;
	OpacityChoice = R.OpacityChoce; //set manually from now one
	K_Step = 2; //R.K_step; //set manually from now on
	Nb_places = R.places.size();
	vplaces = &R.places;

	//int secret_size= R.Sys.size();
	//int secretSize = sizeof(R.Sys);
	//cout<<"size of R.Sys is = "<<secretSize<<endl;

	for (int i = 0; i < 10000; i++) {
		Secret[i] = R.Sys[i];
	}
	int iteratorSet = 0;
	SecretBdd = bdd_true();

	//part commented deleted by me

	cout << "Number of places : " << Nb_places << endl; //print number of places
	cout << "Number of transitions: " << R.transitions.size() << endl;
	//cout<<"Last place is : "<<R.places[Nb_places-1].name<<endl; //print last place
	cout << "number of observable transitions is: " << R.Observable.size()
			<< " they are: " << endl;

	for (Set::const_iterator it = R.Observable.begin();
			it != R.Observable.end(); it++) {
		cout << "t" << (*it) << " ";//<Event id="b" observable="true"/>
		//cout << "<Event id=\"" << (*it) << "\" observable=\"true\"/>"<< endl;
	}
	cout << '\n';
	cout << "number of non observable transition is: " << R.NonObservable.size()
			<< " they are: " << endl;
	for (Set::const_iterator it = R.NonObservable.begin();
			it != R.NonObservable.end(); it++) {
		cout << "t" << (*it) << " ";
		//cout << "<Event id=\"" << (*it) << "\" observable=\"false\"/>"<< endl;
	}
	cout << '\n';
	/* place domain, place bvect, place initial marking and place name */
	// domains
	//i=0;
	for (i = 0, p = R.places.begin(); p != R.places.end(); i++, p++) { //for i=0 and p=firstplace
																	   //till p=lastplace
		if (p->hasCapacity()) {
			domain = p->capacity + 1; //capcity comes from node capcity in net.h
		} else {
			domain = BOUND + 1; // the default domain
		} //this part attaches a domain (capacity+1) for each place (numbered 0 to last)

		//	cout<<"Domain"<<i<<" is: "<<domain<<endl;//just checking domain values

		// variables are created one by one (implying contigue binary variables)
		fdd_extdomain(&domain, 1);
		//cout<<"nb created var : "<<bdd_varnum()<<endl;
		fdd_extdomain(&domain, 1);
		//cout<<"nb created var : "<<bdd_varnum()<<endl;
	}
//part commented by me
/* 	ofstream fileT;
	fileT.open("transitionName.txt"); //save transition names to fileT(taransitionname)
	for (map<string, int>::iterator it = transitionName.begin();
			it != transitionName.end(); it++) {
		//string nameTransition
		fileT << it->first << "\n";
	}
	fileT.close();
	ofstream fileP;
	fileT.open("PlacesName.txt"); //save places names to fileT (placesname)
	for (map<string, int>::iterator it = placeName.begin();
			it != placeName.end(); it++) {
		//string nameTransition
		fileT << it->first << "\n";
	}
	fileT.close();*/

	// bvec
	currentvar = bdd_true(); // returns true bdd
	//cout <<"la variable courrante est " << currentvar<<endl ;

	for (i = 0; i < nbPlaces; i++) {

		nbbit[i] = fdd_varnum(2 * i);//Returns the number of BDD variables used for the finite domain
		// cout<<"nbbit["<<i<<"]= "<<nbbit[i]<<endl;
		// block
		//for each domain 2 vars? --> true and false?
		//cout<<"nb var pour "<<2*i<<" = "<<fdd_domainsize(2*i)<<endl;

		v[i] = bvec_varfdd(2 * i);		// build a boolean vector
		// cout<<"v["<<i<<"]= "<<v[i]<<endl;

		vp[i] = bvec_varfdd(2 * i + 1);
		// cout<<"vp["<<i<<"]= "<<vp[i]<<endl;
		//cout<<"nb var pour "<<2*i+1<<" = "<<fdd_domainsize(2*i+1)<<endl;
		currentvar = currentvar & fdd_ithset(2 * i); // & bitwise and.
		//	cout << "current var " << currentvar <<endl;
	}

	// initial marking
	//cout <<"initial marking" <<endl ;

	// what does this part of the code do??
	//this part we create a BDD that represents the secret

	M0 = bdd_true();

	//find value of initial state
	for (i = 0, p = R.places.begin(); p != R.places.end(); i++, p++) { //for i=0 and p=first place
																	   // as long as not p=last place
																	   // i++ and p++
		//add test if place is a secret and set marking accordingly??
		//cout<<"initial marking: "<< fdd_ithvar(2*i,p->marking)<<endl;
		M0 = M0 & fdd_ithvar(2 * i, p->marking); // MO is true, we do an and(bitwise) with the
												 //returned value of fdd_ithavar (true or false)
		//what is MO???
		//  cout<<"var= "<<2*i<<" val= "<<p->marking<<endl; //checking values for input of fdd_ithvar
	}
	//cout << "the value of the initial state M0 is: " << M0 << endl;
	//cout << "it's path count is:" << bdd_nodecount(M0) << endl;

	SecretBdd = bdd_true();

	while (!Secret[iteratorSet].empty()) //as long as set of secrets not empty
	//while(Secret[iteratorSet]!=NULL)
	{

		i = 0;
		myIterator = Secret[iteratorSet].begin();
		Secretb = bdd_true();
		while ((myIterator != Secret[iteratorSet].end()) && (i < Nb_places))
		//while not end of set of sectret and i < number of places
		{
			if ((It = Secret[iteratorSet].find(i)) != Secret[iteratorSet].end())
			// if not last value in set Secret
			//test to avoid iterator ++ while end of iterator
					{
				Secretb = Secretb & fdd_ithvar(2 * i, 1);//takes marking of the secret in consideration
				//	cout<<"after setting secret with marking = 1"<<endl;//added by me

				//cout<<"secreteb while 1: "<< Secretb<<endl;//commented
				myIterator++;
				i++;

			} else {
				Secretb = Secretb & fdd_ithvar(2 * i, 0);
				//cout<< "secreteb while 0: "<<Secretb<<endl;//commented
				i++;
			}

		}
		while (i < Nb_places) {
			Secretb = Secretb & fdd_ithvar(2 * i, 0);
			//cout <<"secreteb in extra while "<< Secretb << endl;//commented
			i++;
		}
		if (iteratorSet == 0) {
			SecretBdd = Secretb;
		}
		SecretBdd = bdd_apply(SecretBdd, Secretb, bddop_or);
		//   |            |       |______________
		//   |            |                      |
		//values    // true,  depends on the while loops, defined as 2

		//  cout << SecretBdd << endl;

		iteratorSet = iteratorSet + 1;
	}

	cout << '\n';
	//cout << "BDD representing the secret " << SecretBdd << endl;
	cout << "Number of secret states is " << bdd_pathcount(SecretBdd) << endl;
	cout << '\n';

	//cout << "la valeur de vplace est " <<  **R.places << endl;//commented
	fdd_strm_hook(printhandler);		          // printhandler

	/* Transition relation */
	/*
	cout <<" Transition relation"<<endl;
	for (vector<Transition>::const_iterator t = R.transitions.begin();
	 t != R.transitions.end(); t++){
	 cout<<"Name: "<<t->name<<endl;
	 for (vector<pair<int, int> >::const_iterator it = t->pre.begin();
	 it != t->pre.end(); it++){
	 cout<<"Pre: "<<it->first<<endl;
	 }
	 }
*/
	for (vector<Transition>::const_iterator t = R.transitions.begin();
			t != R.transitions.end(); t++) { //for each transition
	//cout<<"starting assembly of relation for each transition"<<endl;
	//cout<<"values of t:\n";
	//cout<<"name: "<<t->name<<endl;

		int np = 0;
		bdd rel = bdd_true(), var = bdd_true(), prerel = bdd_true();
		//cout<<" rel "<<rel<<endl;
		//cout <<"var "<<var<<endl;
		//cout<<"prerel "<<prerel<<endl;
		//	rel=true, var=true, prerel=true
		bdd Precond = bdd_true(), Postcond = bdd_true();
		// precond=true, postcond=true
		bddPair *p = bdd_newpair();

		for (i = 0; i < nbPlaces; i++) {
			//for each place build prec and postc
			prec[i] = bvec_con(nbbit[i], 0); // builds a boolean vector that represents the variable val(0) using nbbit[i]
			 //cout<<"prec"<<i<<" is: "<<prec[i];
				//cout <<"builds a boolean vector that represents the variable val(0) using "<<nbbit[i]<< prec[i]<<endl;
			postc[i] = bvec_con(nbbit[i], 0);// builds a boolean vector that represent the variable val(0) using nbbit[i]
			//cout <<"builds a boolean  vector that represents the variable val(0) using "<<nbbit[i]<< postc[i]<<endl;
		}

		// calculer les places adjacentes a la transition t
		// et la relation rel de la transition t
		set<int> adjacentPlace;

		// arcs pre
		//cout <<"arcs pre"<< endl;
		//int insertadjpl=0;
		for (vector<pair<int, int> >::const_iterator it = t->pre.begin();
				it != t->pre.end(); it++) {
			//	insertadjpl++;
			//cout<<"pre: "<<it->first<<" "<<it->second<<endl;
			//t is an iterator of transitions
			//cout<<"I am inside!!"<<endl;
			adjacentPlace.insert(it->first);
			//cout << it->first<< endl;
			//cout<<it->second<<endl;
			prec[it->first] = prec[it->first]
					+ bvec_con(nbbit[it->first], it->second);
			//cout<<prec[it->first]<<endl;
			// prec(firstvalueofiterator)=+bvec_con[nbbit[1],2nd value of it]
			//bvec_con --> boolean representation of integer value
			//exp 1-->F
			//	cout <<"it->second"<< it->second<< endl;
			//cout <<"prec[it->first]"<< prec[it->first]<< endl;
		}
		//cout<<"insertadjpl: "<<insertadjpl<<endl;
		// arcs post
		//cout <<"arcs post "<< endl;
		for (vector<pair<int, int> >::const_iterator it = t->post.begin();
				it != t->post.end(); it++) {
			adjacentPlace.insert(it->first);
			postc[it->first] = postc[it->first]
					+ bvec_con(nbbit[it->first], it->second);
			//cout <<"arcs post"<< postc[it->first]<<endl;
		}
		// arcs pre automodifiants
		for (vector<pair<int, int> >::const_iterator it = t->preAuto.begin();
				it != t->preAuto.end(); it++) {
			adjacentPlace.insert(it->first);
			prec[it->first] = prec[it->first] + v[it->second];
			//cout <<"arcs pre automodifiants"<< prec[it->first] << endl;
		}
		// arcs post automodifiants
		for (vector<pair<int, int> >::const_iterator it = t->postAuto.begin();
				it != t->postAuto.end(); it++) {
			adjacentPlace.insert(it->first);
			postc[it->first] = postc[it->first] + v[it->second];
			//cout <<"arcs post automodifiants"<< postc[it->first] << endl;
		}
		// arcs reset
		for (vector<int>::const_iterator it = t->reset.begin();
				it != t->reset.end(); it++) {
			adjacentPlace.insert(*it);
			prec[*it] = prec[*it] + v[*it];
		}

		np = 0;
		for (set<int>::const_iterator it = adjacentPlace.begin();
				it != adjacentPlace.end(); it++) {
			idv[np] = 2 * (*it);
			idvp[np] = 2 * (*it) + 1;
			var = var & fdd_ithset(2 * (*it));
			//Image
			// precondition
			rel = rel & (v[*it] >= prec[*it]);
			Precond = Precond & (v[*it] >= prec[*it]);
			// postcondition
			rel = rel & (vp[*it] == (v[*it] - prec[*it] + postc[*it]));
			// Pre image __________
			// precondition
			prerel = prerel & (v[*it] >= postc[*it]);
			// postcondition
			Postcond = Postcond & (v[*it] >= postc[*it]);
			prerel = prerel & (vp[*it] == (v[*it] - postc[*it] + prec[*it]));
			//___________________
			// capacité
			if (R.places[*it].hasCapacity())
				rel =rel& (vp[*it]<= bvec_con(nbbit[*it],R.places[*it].capacity));
			np++;
		}
		fdd_setpairs(p, idvp, idv, np);

		// arcs inhibitor rel

		for (vector<pair<int, int> >::const_iterator it = t->inhibitor.begin(); it != t->inhibitor.end(); it++)
			rel = rel & (v[it->first] < bvec_con(nbbit[it->first], it->second));
		Trans tt(var, p, rel, prerel, Precond, Postcond);
		//cout<<"values of tt of "<<t->name<<" are: "<<tt.var<<" "<<tt.pair->id<<" "<<tt.pair->last<<" "<<tt.rel<<" "<<tt.prerel<<" "<<tt.Precond<<" "<<tt.Postcond<<endl;

		this->relation.push_back(tt);
//		cout<<"size of vector relation "<<relation.size()<<endl;
		//cout<<"relation vector "<<relation.front().var<<" "<<relation.front().pair->id<<" "<<relation.front().pair->last<<" "<<relation.front().rel<<" "<<relation.front().prerel<<" "<<relation.front().Precond<<" "<<relation.front().Postcond<<endl;
		/*int i=0;
		 for(vector<Trans>::const_iterator itr=relation.begin(); itr!=relation.end();itr++){
		 cout<<"itr: "<<(relation)[i](M0)<<endl;
		 i++;
		 cout<<" /n";
		 }*/
		//cout<<"relation of M0 and t1"<<relation[0](M0)<<endl;
	}
	//cout <<"first relation " <<relation[1].var << endl;

	delete[] v;
	delete[] vp;
	delete[] prec;
	delete[] postc;
	delete[] idv;
	delete[] idvp;
	delete[] nbbit;

}
/*----------------------------------- Reachability space using bdd ----------*/
// part was not comented begin
bdd RdPBDD::ReachableBDD1() {
	bdd M1;
	bdd M2 = M0;
	double d, tps;
	d = (double) clock() / (double) CLOCKS_PER_SEC;
	NbIt = 0;
	MaxIntBdd = bdd_nodecount(M0);
	while (M1 != M2) {
		M1 = M2;
		for (vector<Trans>::const_iterator i = relation.begin();
				i != relation.end(); i++) {
			bdd succ = (*i)(M2);
			M2 = succ | M2;
			cout << "M2 by ReachableBDD1" << M2 << endl;
			//if(succ!=bddfalse)
		}
		NbIt++;
		int Current_size = bdd_nodecount(M2);// count the number of node used in a bdd
		if (MaxIntBdd < Current_size)
			MaxIntBdd = Current_size;
		//cout<<"Iteration numero "<<NbIt<<" nb node de reached :"<<bdd_nodecount(M2)<<endl;
//		cout << bdd_nodecount(M2) << endl;
	}
	cout << endl;
	tps = ((double) (clock()) / CLOCKS_PER_SEC) - d;
	cout << "-----------------------------------------------------\n";
	cout << "CONSTRUCTION TIME  " << tps << endl;
	cout << "Max Intermediary BDD " << MaxIntBdd << endl;
	cout << "Nombre d'iteration : " << NbIt << endl;
	//bdd Cycle=EmersonLey(M2,0);
	//cout<<Cycle.id()<<endl;
	return M2;
}
bdd RdPBDD::ReachableBDD2() {
	bdd M1;
	bdd M2 = M0;
	double d, tps;
	d = (double) clock() / (double) CLOCKS_PER_SEC;
	NbIt = 0;
	MaxIntBdd = bdd_nodecount(M0);
	while (M1 != M2) {
		M1 = M2;
		bdd Reached;
		for (vector<Trans>::const_iterator i = relation.begin();
				i != relation.end(); i++) {
			bdd succ = (*i)(M2);
			Reached = succ | Reached;
			//cout << bdd_nodecount(succ) << endl;
			//if(succ!=bddfalse)
		}
		NbIt++;
		M2 = M2 | Reached;
		cout << "M2 by ReachableBDD2" << M2 << endl;
		int Current_size = bdd_nodecount(M2);
		if (MaxIntBdd < Current_size)
			MaxIntBdd = Current_size;
		//cout<<"Iteration numero "<<NbIt<<" nb node de reached :"<<bdd_nodecount(M2)<<endl;
//		cout << bdd_nodecount(M2) << endl;
	}
	cout << endl;
	tps = ((double) (clock()) / CLOCKS_PER_SEC) - d;
	cout << "-----------------------------------------------------\n";
	cout << "CONSTRUCTION TIME  " << tps << endl;
	cout << "Max Intermediary BDD " << MaxIntBdd << endl;
	cout << "Nombre d'iteration : " << NbIt << endl;
	return M2;
}
bdd RdPBDD::ReachableBDD3() {
	double d, tps;
	d = (double) clock() / (double) CLOCKS_PER_SEC;
	bdd New, Reached, From;
	Reached = From = M0;
	cout << "reached & from avant le traitement " << "reached " << Reached
			<< "from " << From << endl;
	NbIt = 0;
	do {
		NbIt++;
		bdd succ;
		for (vector<Trans>::const_iterator i = relation.begin();
				i != relation.end(); i++)
			succ = (*i)(From) | succ;
		New = succ - Reached;
		From = New;
		cout << " new au niveau de ReachableBDD3" << New << endl;

		cout << "  From au niveau de ReachableBDD3" << From << endl;
		Reached = Reached | New;
		cout << "  Reached au niveau de ReachableBDD3" << Reached << endl;
		cout << "Iteration numero " << NbIt << " nb node de reached :"
				<< bdd_nodecount(Reached) << endl;
	} while (New != bddfalse);
	tps = (double) clock() / (double) CLOCKS_PER_SEC - d;
	cout << "TPS CONSTRUCTION : " << tps << endl;
	return Reached;
} //part was not commented end

/*----------------Fermeture transitive sur les transitions non observées ---*/
//I.E: creation of arcs between all reachable nodes from each other (if a is reachable by be
//add arc from a to b
bdd RdPBDD::Accessible_epsilon2(bdd Init) {

	bdd Reached, New, From;
	Reached = From = Init;
	do {
		bdd succ;
		for (Set::const_iterator i = NonObservable.begin();
				!(i == NonObservable.end()); i++)

			succ = relation[(*i)](From) | succ;
		New = succ - Reached;
		From = New;
		Reached = Reached | New;
	} while (New != bddfalse);
	cout << endl;
	return Reached;
}

/************************************Saturate***********************************/
/*
        Accessible_epsilon : method defined in RdPBDD; to be defined in class Model
        Accessible_epsilon : @param : bdd
        Accessible_epsilon : @return : bdd
        Accessible_epsilon : completes all the unobservable reach of bdd in parameter
        The output is a bdd that represents the aggregates of SOG
*/
bdd RdPBDD::Accessible_epsilon(bdd From) {
	//cout << "accessible epsilon0" << endl;
	bdd M1;
	bdd M2 = From;
	int it = 0;
	do {
		//cout << "accessible epsilon02" << endl;
		M1 = M2; // pour verifier si on travaille sur le meme noeud
		//cout << "accessible epsilon12" << endl;
		//cout<<"M2 = "<<M2<<endl;
		//cout << "accessible epsilon22" << endl;
		for (Set::const_iterator i = NonObservable.begin();!(i == NonObservable.end()); i++) {
			//cout << "accessible epsilon32" << endl;
			//cout << *i << endl;//correct
			//cout <<M2 << endl;//correct
			//cout<<"relation[(*i)](M2)"<<endl;
			bdd succ = relation[(*i)](M2); //find successors of M2 using nonobservable event i
			//cout << "accessible epsilon42" << endl;
			M2 = succ | M2;
			//cout << "accessible epsilon52" << endl;
		}
		//cout << "accessible epsilon2" << endl;
		TabMeta[nbmetastate] = M2;
		//cout << "accessible epsilon3" << endl;//not showed
		//cout<< "saturate (M2) = "<<M2<<endl;

		int intsize = bdd_anodecount(TabMeta, nbmetastate + 1);	//how many shared nodes are in bdd tabmeta
		//cout << "accessible epsilon4" << endl;
		if (MaxIntBdd < intsize) //maximum number of nodes??
			MaxIntBdd = intsize;
		it++;
			//cout << bdd_nodecount(M2) << endl;
	} while (M1 != M2);
	//cout << "accessible epsilon5" << endl;
	//cout << endl;
	//cout<<"M2:"<<M2<<endl;
	return M2;
}


/************************************QSaturate***********************************/
bdd RdPBDD::QSaturate(bdd From, bool init) {
	bdd M1;
	bdd M2 = From;
	std::vector<int> vtr;
	double NT =0, NST=0, aggD=0;
	int it = 0;

	//initiliaze trace count
	NT=bdd_pathcount(From);
	//cout<<"NT (initial size of input aggr)= "<<NT<<endl;

	//initialize secret traces count
	// test secret input state
	bdd test = bdd_true();
	test = M2 & SecretBdd;
	if (test == M2) NST++;
	//cout<<"NST (initial count of secret traces in input aggr)= "<<NST<<endl;


	do {

		M1 = M2;

		for (Set::const_iterator i = NonObservable.begin();
				!(i == NonObservable.end()); i++) {

			bdd succ = relation[(*i)](M2);


			if(succ != bdd_false()){
				//
				if ( !(std::find(vtr.begin(), vtr.end(), *i) != vtr.end()) )
					{
				NT++;
			//	int j= *i;
			//	cout<<"succ par t"<<j+1<<" = "<<succ<<endl;
			//	cout<<"updated NT="<<NT<<endl;
				vtr.push_back (*i);

				// test secret succ
				bdd testing;;
				testing = succ & SecretBdd;
				if (testing != bdd_false()){
					NST++;
				//	cout<<"updated NST= "<<NST<<endl;

				}

				}

			}
			//test if succ is secret
			//update NT & NST
			M2 = succ | M2;
		}

		TabMeta[nbmetastate] = M2;


		int intsize = bdd_anodecount(TabMeta, nbmetastate + 1);
		if (MaxIntBdd < intsize)
			MaxIntBdd = intsize;
		it++;

	} while (M1 != M2);

/*	if(init == true){
		NT-=1;
	}*/
	cout << '\n';
	//cout<<"Aggregate's NT= "<<NT<<endl;
	//cout<<"Aggregate's NST= "<<NST<<endl;
	if(NT!=0){
		aggD= NST/NT;
		cout<<"Current Aggregate's Opacity Degree aggD= "<<aggD<<endl;
		cout << '\n';
		if(aggD>=1) QOne=true;
		PAggs.push_back(aggD);
	}
	//insert found degrees to a vector, and cal overall degree
	return M2;
}
/*------------------------  StepForward()  --------------*/
//part was not comented begin
bdd RdPBDD::StepForward2(bdd From) {
	// cout<<"Debut Step Forward \n";
	bdd Res;
	for (Set::const_iterator i = NonObservable.begin();
			!(i == NonObservable.end()); i++) {
		bdd succ = relation[(*i)](From);
		Res = Res | succ;
	}
	//cout<<"Fin Step Forward \n";
	return Res;
}
bdd RdPBDD::StepForward(bdd From) {
	//cout<<"Debut Step Forward \n";
	bdd Res = From;
	for (Set::const_iterator i = NonObservable.begin();
			!(i == NonObservable.end()); i++) {
		bdd succ = relation[(*i)](Res);
		Res = Res | succ;
	}
	//cout<<"Fin Step Forward \n";
	return Res;
} //part was not comented end

/*--------------- verif()---------------------------------*/
//bdd RdPBDD::VerificationPost( int t)
//{
//   return relation[t].Postcond;
//   cout<< relation[t].Postcond<<endl ;
//}

/*------------------------  StepBackward()  --------------*/
bdd RdPBDD::StepBackward2(bdd From) {
	bdd Res;
	//cout<<"Ici Step Back : From.id() = "<<From.id()<<endl;
	for (vector<Trans>::const_iterator t = relation.begin();
			t != relation.end(); t++) {
		bdd succ = (*t)[From];
		Res = Res | succ;
		//  cout<<"Res.id() = "<<Res.id()<<endl;
	}
	// cout<<"Res.id() = "<<Res.id()<<endl;
	return Res;
}

bdd RdPBDD::StepBackward(bdd From) {
	bdd Res = From;
	for (vector<Trans>::const_iterator t = relation.begin();
			t != relation.end(); t++) {
		bdd succ = (*t)[Res];
		Res = Res | succ;
	}
	return Res;
}
/*---------------------------GetSuccessor()-----------*/
bdd RdPBDD::get_successor(bdd From, int t) {
	//cout << "getsuccessor "<<relation[t](From)<< endl ;
	//cout << "le preconde de ça est :  "<<relation[t].Postcond((From));

	return relation[t](From);

}

/*------------------------Firable Obs()--------------*/
/*
        firable_obs : method defined in RdPBDD; to be defined in class Model
        //firable_obs : @param : bdd
        //firable_obs : @returns : set<int> : represents transitions
        //firable_obs : all firable transitions from bdd in param
        //firable_obs : attitude depends on the type of the model used
        //firable_obs : pure virtual method in abstract class model
        //firable_obs : to be redefined in RdPBDD
*/
Set RdPBDD::firable_obs(bdd State)	//input is shared nodes
{
	//cout << "firable function"<<endl;
	Set res;
	for (Set::const_iterator i = Observable.begin(); !(i == Observable.end());i++) {
		{
		    //cout<<"i :"<<*i<<endl;
			bdd succ = relation[(*i)](State);//set of states (transitions, places) reachable by a State.
			//cout<<"successeurs : "<<succ<<endl;
			if (succ != bddfalse)
				res.insert(*i);
		}

	}

	return res;

}

/*------------------------------------Observation Graph ----------*/
void RdPBDD::compute_canonical_deterministic_graph_Opt(MDGraph& g) {
    //cout << "****** testing opacity and construction of SOG *****" << endl;
	//cout << '\n';
	// cout<<"nb bdd var : "<<bdd_varnum()<<endl;
	double d, tps;
	d = (double) clock() / (double) CLOCKS_PER_SEC;
	TabMeta = new bdd[1000];
	nbmetastate = 0;
	MaxIntBdd = 0;
	std::string tr = "";

	pile st;
	NbIt = 0;
	int nbSucceEXstate = 0;
	int CounterStates = 0;
	int nbstate = 0;
	bool Cross_Secret = false, System_tested = true;
	itext = itint = 0;
	Class_Of_State* reached_class;
	Set fire;
	// size_t max_meta_state_size;
	Class_Of_State *c = new Class_Of_State; //c=a


	bdd Complete_meta_state = Accessible_epsilon(M0); //Saturate(M0)

	// Testing simple opacity for initial aggr:
	bdd test = Complete_meta_state & SecretBdd;
	if (Complete_meta_state == test) {

		cout << '\n';
		cout<< "System is traditionally non opaque in first aggregate, no counter example to be printed"<< endl;
        cout << '\n';
        System_tested = false;
        return;
	}

//    cout<<"First aggregate is opaque"<<endl;
	fire = firable_obs(Complete_meta_state); // find all observables reached by this state
	// cout<<"Apres fireable_obs"<<endl;
	// for(Set::const_iterator itr=fire.begin(); itr!=fire.end(); itr++){
	// cout<<"fire value: "<<*itr<<endl;
	// }
	c->blocage = Set_Bloc(Complete_meta_state);      // detect if blockage
	c->boucle = Set_Div(Complete_meta_state); //detect if sequence is divergente (has an end)
	c->class_state = CanonizeR(Complete_meta_state, 0); //checks if more than 2 internal loops?
	// cout<<"Apres CanonizeR nb representant : "<<bdd_pathcount(c->class_state)<<endl;
	//c->class_state=Complete_meta_state;
	//cout<<"nbmetastate M0= "<<nbmetastate<<endl;
	TabMeta[nbmetastate] = c->class_state;
	nbmetastate++; //nombre des aggregats? //only modifier??No
	old_size = bdd_nodecount(c->class_state);//???

	st.push(Pair(couple(c, Complete_meta_state), fire));      //empiler
//	cout<<"st size : "<< st.size()<<endl;
//    cout<<"fire.size : "<<fire.size()<<endl;
	//add aggr
	g.setInitialState(c);
	g.insert(c);//V U {a}
//	cout<<"inserted the first agreg"<<endl;

	g.nbMarking += bdd_pathcount(c->class_state);
	do {  // int loopindex=0;
         // cout<<"st not empty"<<endl;
		int nbsecesor = 0;
		Pair e = st.top();   //(c,complete_eta_state),fire
							 // a and v, observable events
		st.pop();
		// cout<<"apres pop"<<endl;
		nbmetastate--; //another modifer
		while (!e.second.empty()) //there is an observable event reached by c
		{
		   // cout<<"there is an observable event reached by c"<<endl;
			//what's within e.second?
		/*	for(Set::const_iterator i = e.second.begin();
				!(i == e.second.end()); i++){
				int j= *i;
				cout<<"e.second."<<j<<endl;

			}*/

            //cout<<"another aggregate"<<endl;
			int t = *e.second.begin(); //copy observable event
			e.second.erase(t); //remove observable event from list

			double nbnode;
			reached_class = new Class_Of_State;
			{

				bdd GSuccesor = get_successor(e.first.second, t); //find succesors observable events
				// cout<<"succ e.first.second(t)"<<GSuccesor<<endl;
				bdd Complete_meta_state = Accessible_epsilon(GSuccesor); //aggregate succeossors
				// cout<< "Complete_meta_state"<<Complete_meta_state<<endl;

				reached_class->class_state = CanonizeR(Complete_meta_state, 0);
				Class_Of_State* pos = g.find(reached_class);
				//cout<<"nbmetastate Mi= "<<nbmetastate<<endl;

				if (!pos) //Localise reached state

				{
                   // cout<<"class not found before"<<endl;
					//fire = firable_obs(Complete_meta_state); //find succ with obs events
					// cout<<"reached"<<reached_class->class_state<<endl;//added by me
					/* cout << "path count from reached = "
					 << bdd_pathcount(c->class_state) << endl;	*///added by me
					bdd bc = StepBackward2(reached_class->class_state);
					// cout<< "========> bc: "<<bc << endl;
					//  bdd nawel = CanonizeR(bc,0);
					//  cout<< "---------->"<<nawel<< endl;
					// cout<<bddtable<<reached_class->class_state<<endl;

					cross = GSuccesor & SecretBdd; //test k-step opacity??
					//cout<<"value of the reached state: "<<reached_class->class_state<<endl;

					/*bdd product= bdd_true();
					 cout<<"product "<<product<<endl;*/

					T = bdd_true();
					T = Complete_meta_state & SecretBdd;// test simple opacity

					//cout<<"testing opacity, value of T "<<T<<endl;
					//cout<<"Complete_meta_state  "<<Complete_meta_state <<endl;
					// cout<< "Intersection value: "<<T<<endl;
					/*  if(reached_class->class_state == SecretBdd){
					 cout<<"equal!!"<<endl;
					 T= bdd_true();
					 }else
					 {cout<<"not equal!!"<<endl;
					 T= bdd_false();}*///added by me
					//char a = (char) (((int) '1') + t);
					string a= IntToString(t);
					tr = tr + "-->" + "t" + a;

					if (T == Complete_meta_state) //(reached_class->class_state ^ SecretBdd)== bdd_false()
                    {
						System_tested = false;
						//cout << '\n';
						//cout << "The system is not traditionally opaque after the trace  :  "<< tr << endl;
						//cout<<"Its Quantified Oapcity Value is: P=1"<<endl;
						//cout << '\n';
						return;
					}
					fire = firable_obs(Complete_meta_state); //find succ with obs events

					st.push(
							Pair(couple(reached_class, Complete_meta_state),
									fire));

					TabMeta[nbmetastate] = reached_class->class_state; //aggregate v union a
					// cout<<"reached 2: "<<reached_class->class_state<<endl; //added by me
					nbmetastate++;
					//old_size=bdd_anodecount(TabMeta,nbmetastate);
					e.first.first->Successors.insert(
							e.first.first->Successors.begin(),
							Edge(reached_class, t));
					reached_class->Predecessors.insert(
							reached_class->Predecessors.begin(),
							Edge(e.first.first, t));
					// char b = (char)(((int)'0')+t);//commented
					// tr =tr +"-->"+"t"+b;//commented
					g.addArc();
					g.insert(reached_class);
					//cout<<"inserted an aggregate"<<endl;
				}	   //end if (!pos)

				else {
                       // cout<<"class found before"<<endl;
					delete reached_class;
					e.first.first->Successors.insert(
							e.first.first->Successors.begin(), Edge(pos, t));
					pos->Predecessors.insert(pos->Predecessors.begin(),
							Edge(e.first.first, t));
					char a = (char) (((int) '0') + t);
					tr = tr + "-->" + "t" + a;
					g.addArc();
				}

			}

			/*	CounterStates = CounterStates + 1;
			 if (cross != bdd_false() && Cross_Secret == false) {
			 Cross_Secret = true;
			 cout << "The system has passed through a secret state " << endl;
			 nbstate = CounterStates;
			 cout << "nbstat" << nbstate << endl;
			 System_tested=false;
			 }
			 nbsecesor = nbsecesor + 1;*/
//	cout<< "nb de succecor "<<nbsecesor<<endl;
			//loopindex++;
			//cout<<"loopindex "<<loopindex<<endl;
		}	//end while empty enb

		/*int indice = CounterStates - nbstate;
		 if ((nbsecesor > 1) or (System_tested == true)) {
		 nbSucceEXstate = nbsecesor;
		 //cout<< "Systeme not simple opaque"<<endl;
		 //cout<<"trace: "<<tr<<endl;//added by me
		 }

		 cout<<"Cross Secret: "<<Cross_Secret<<endl;
		 cout<<"nbSucceEXstate: "<<nbSucceEXstate<<endl;

		 if (Cross_Secret == true && nbSucceEXstate == 0) {
		 int kOP = weakOpacity(reached_class->class_state, SecretBdd,
		 indice);
		 cout << "The system is " << kOP << " -weak opaque" << endl;
		 }

		 if (Cross_Secret == true && nbSucceEXstate == 0) {
		 int kOP = StrongOpacity(reached_class->class_state, SecretBdd,
		 indice);
		 if (kOP != K_Step) {
		 cout << "The system is " << kOP
		 << " -strong opaque but not " << K_Step << "strong opaque"<<endl;
		 } else {
		 cout << "The system is " << kOP << " -weak opaque" << endl;
		 }

		 }
		 // loopindex++;
		 // cout<<"loop :"<<loopindex<<endl;

		 */
		 //cout<<"non observable event reached "<<endl;
		 } while (!st.empty());
	//if (System_tested == true && QOne != true) {

	//}

	tps = (double) clock() / (double) CLOCKS_PER_SEC - d;
	//  cout<<"TIME OF CONSTRUCTIONR : "<<tps<<endl;
}



/*-----------------------WeakOpacity--------------------------*/
/* Methode  vérifie si le systéme est weak opaque ou non elle retourne la valeur K */
int RdPBDD::weakOpacity(bdd state, bdd secret, int numStatCros) {
	int i = 0;
	bdd test = bdd_true();
	bdd BW = StepBackward2(state);
	while (i <= numStatCros && test != BW) {
		BW = StepBackward2(BW);
		test = BW & secret;
		i++;
	}
	if (test == BW) {
		return i - 1;
	} else {
		return i;
	}

}
/*-------------------------------STrongOpacity------------------*/
int RdPBDD::StrongOpacity(bdd state, bdd secret, int numStatCros) {
	int i = 0;
	bdd test = bdd_true();
	bdd BW = StepBackward2(state);
	while (i <= numStatCros && test == bdd_false()) {
		BW = StepBackward2(BW);
		test = BW & secret;
		i++;
	}
	if (test != bdd_false()) {
		return i - 1;
	} else {
		return i;
	}

}
//deleted commented part 3

/*---------------------findsubvector-------------------------*/
//part commented by me
/*
bool RdPBDD::FindSbVector(vector<string> a, string c) { //pour trouver si le parent est bien lui ou nn

	int count = 0;
	std::vector<string>::const_iterator i;
//	std::vector<string>::const_iterator j ;
	for (unsigned i = 0; i < a.size(); i++)
		if (c.find(a[i]) != std::string::npos) {

			count = count + 1;
		}

	if (count == a.size())
		return true;
	else
		return false;
}*/
//delted comented part 4
/*-----------------------CanonizeR()----------------*/
bdd RdPBDD::CanonizeR(bdd s, unsigned int i) {
	bdd s1, s2;
	do {
		// cout<<"canonizeR !!"<<endl;
		//  cout<<"s: "<<s<<endl;
		itext++;
		s1 = s - bdd_nithvar(2 * i);//s- the negation of the variable 2*i(0?)
		// 	cout<<"s1: "<<s1<<endl;
		s2 = s - s1;
		// 	cout<<"s2: "<<s2<<endl;
		if ((!(s1 == bddfalse)) && (!(s2 == bddfalse))) {
			bdd front = s1;
			//cout<<"front "<<front<<endl;
			bdd reached = s1;
			//cout<<"reached: "<<reached<<endl;
			do {
				// cout<<"premiere boucle interne \n";
				itint++;
				front = StepForward(front) - reached;
				reached = reached | front;
				s2 = s2 - front;
			} while ((!(front == bddfalse)) && (!(s2 == bddfalse)));
		}
		if ((!(s1 == bddfalse)) && (!(s2 == bddfalse))) {
			bdd front = s2;
			bdd reached = s2;
			do {
				// cout<<"deuxieme boucle interne \n";
				itint++;
				front = StepForward(front) - reached;
				reached = reached | front;
				s1 = s1 - front;
			} while ((!(front == bddfalse)) && (!(s1 == bddfalse)));
		}
		s = s1 | s2;
		i++;
	} while ((i < Nb_places) && ((s1 == bddfalse) || (s2 == bddfalse)));
	if (i >= Nb_places) {
		//cout<<"____________oooooooppppppppsssssssss________\n";
		return s;
	} else {
		//  cout<<"________________p a s o o o p p p s s s ______\n";
		return (CanonizeR(s1, i) | CanonizeR(s2, i));
	}
}
/*------------------- GETpreL--------------*/
/*-----------------convert to string------------------*/
bdd RdPBDD::GetPrerel(bdd s, int t) {
	return relation[t].prerel;
}
//string RdPBDD::ConvertToString (bdd S)
//{ // on utilise cette methode afin de convertir l'output de cout
//  // d'un bdd en un string pour extraire les marked places
//
////	cout<< "Verification de l'opacité au "<<S.id()<< endl;
//	std::stringstream redirectStream;
//	    std::streambuf*   oldbuf  = std::cout.rdbuf( redirectStream.rdbuf() );
//	  cout << S <<endl ;
//	    std::string str ;
//	    std::string res;
//	    while(std::getline(redirectStream, str))
//	        {
//	          fprintf(stdout,"%s\n",str.c_str());
//	            res= res+str;
//	            // loop enter once for each line.
//	            // Note: str does not include the '\n' character.
//	        }
//
//	        // In real life use RAII to do this. Simplified here for code clarity.
//	        std::cout.rdbuf(oldbuf);
//
//	        return res;
//
//}
/*-----------tokenThestring--------------------*/
//std::vector<string> RdPBDD::Tokenizer(string entree)
//{
//	// on utilise cette methode pour maitre les places marqueés dans un vector<string> pour
//	//pouvoir les comparer avec les secrets
//
//
//	   entree.substr(0,entree.length()-1);
//	   entree.erase(std::find(entree.begin(), entree.end(), '<'));
//	   entree.erase(std::find(entree.begin(), entree.end(), '>'));
//	   string str =entree;
//	   string buf; // Have a buffer string
//	   stringstream ss(str); // Insert the string into a stream
//
//	    vector<string> tokens; // Create vector to hold our words
//
//	    while (ss >> buf)
//	        tokens.push_back(buf);
//
//	return tokens ;
//
//
//}
/*--------------------GetSetMarked(vector<string>)-------------------*/
//Set RdPBDD::GetSetMarked(std::vector<string> Vec_Ma)
//{
//	// les places marquees sont reperés avec leurs positions
//Set res ;
//  for(unsigned i=0 ; i<Vec_Ma.size() ; i++)
//  {
//	std::string S = " " ;
//	S = Vec_Ma[i];
//	 if (S.find(":1")!=std::string::npos)
//	{
//
//		res.insert(i);
//	}
//   }
//  return res ;
//}
/*---------------------------  Set_Bloc()  -------*/

bool RdPBDD::Set_Bloc(bdd &S) const {
	//cout<<"Ici detect blocage \n";
	int k = 0;
	bdd Mt = bddtrue;
	for (vector<Trans>::const_iterator i = relation.begin();
			i != relation.end(); i++, k++) {
		//cout<<"transition :"<<transitions[k].name<<endl;
		//cout<<"PRECOND :"<<bddtable<<(*i).Precond<<endl;
		//cout<<"POSTCOND :"<<bddtable<<(*i).Postcond<<endl;
		//int toto;cin>>toto;
		Mt = Mt & !((*i).Precond);
	}
	return ((S & Mt) != bddfalse);
	//BLOCAGE
}

/*-------------------------Set_Div() � revoir -----------------------------*/
bool RdPBDD::Set_Div(bdd &S) const {
	Set::const_iterator i;
	bdd To, Reached;
	//cout<<"Ici detect divergence \n";
	Reached = S;
	do {
		bdd From = Reached;
		for (i = NonObservable.begin(); !(i == NonObservable.end()); i++) {

			To = relation[*i](Reached);
			Reached = Reached | To; //Reached=To ???
			//Reached=To;
		}
		if (Reached == From)
			//cout<<"SEQUENCE DIVERGENTE \n";
			return true;
		//From=Reached;
	} while (Reached != bddfalse);
	return false;
	//cout<<"PAS DE SEQUENCE DIVERGENTE \n";
}
/*-----------FrontiereNodes() pour bdd ---------*/
bdd RdPBDD::FrontiereNodes(bdd From) const {
	bdd res = bddfalse;
	for (Set::const_iterator i = Observable.begin(); !(i == Observable.end());
			i++)
		res = res | (From & relation[*i].Precond);
	for (Set::const_iterator i = InterfaceTrans.begin();
			!(i == InterfaceTrans.end()); i++)
		res = res | (From & relation[*i].Precond);
	return res;
}
/*-------- Produit synchronis� � la vol�e de n graphes d'observation : Adaptation � la capture des s�quences bloquantes et les s�quences divergentes----------------------*/
//commented part by me
/*void RdPBDD::GeneralizedSynchProduct1(Modular_Obs_Graph& Gv, int NbSubnets,
		RdPBDD* Subnets[], int nbbddvar) {
	cout << "_____________  GeneralizedSynchProduct1  _________________________"
			<< NbSubnets << "sous-reseaux " << endl;
	//for(int k=0;k<NbSubnets;k++)
	//	cout<<*Subnets[k]<<endl;
	int pos_trans(TRANSITIONS, string);
	TabMeta = new bdd[1000000];
	nbmetastate = 0;
	MaxIntBdd = 0;
	nbmetastate = 0;
	Stack st;
	//int cpt=0;
	int k;
	bdd *Complete_meta_state = new bdd[NbSubnets];
	Set *fire = new Set[NbSubnets];
	Modular_Class_Of_State *Meta_State = new Modular_Class_Of_State;
	for (k = 0; k < NbSubnets; k++) {
		Complete_meta_state[k] = Subnets[k]->Accessible_epsilon(Subnets[k]->M0);
		fire[k] = Subnets[k]->firable_obs(Complete_meta_state[k]);
		//Meta_State->State.push_back(Subnets[k]->FrontiereNodes(Complete_meta_state[k]));
		//Meta_State->State.push_back(Subnets[k]->CanonizeR(Subnets[k]->FrontiereNodes(Complete_meta_state[k]),0));
		Meta_State->State.push_back(
				Subnets[k]->CanonizeR(Complete_meta_state[k], 0));
		-------------------- STAT ----------------------
		TabMeta[nbmetastate] = Meta_State->State[k];
		nbmetastate++;
	}
	old_size = bdd_anodecount(TabMeta, nbmetastate);
	Meta_State->blocage = true;
	for (k = 0; ((k < NbSubnets) && (Meta_State->blocage)); k++)
		Meta_State->blocage = Meta_State->blocage
				&& Subnets[k]->Set_Bloc(Complete_meta_state[k]);
	Meta_State->boucle = false;
	for (k = 0; ((k < NbSubnets) && (!Meta_State->boucle)); k++)
		Meta_State->boucle = Meta_State->boucle
				|| Subnets[k]->Set_Div(Complete_meta_state[k]);
	Gv.setInitialState(Meta_State);
	Gv.insert(Meta_State);
	nbmetastate++;
	st.push(StackElt(Couple(Meta_State, Complete_meta_state), fire));
	do {
		NbIt++;
		StackElt e = st.top();
		st.pop();
		for (k = 0; k < NbSubnets; k++) {
			while (!e.second[k].empty()) {
				int t = *e.second[k].begin();
				e.second[k].erase(t);
				bool ok = true;
				Set ConcernedSubnets;
				ConcernedSubnets.insert(k);
				string tmp = Subnets[k]->transitions[t].name;
				for (int j = 0; j < NbSubnets; j++) {
					if (j != k) {
						int num = Subnets[j]->transitionName[tmp];
						int pos = pos_trans(Subnets[j]->transitions, tmp);
						if ((pos != -1)
								&& !(Subnets[j]->Observable.find(num)
										== Subnets[j]->Observable.end())) {
							ConcernedSubnets.insert(j);
							if (e.second[j].find(num) == e.second[j].end()) {
								ok = false;
							} else
								e.second[j].erase(num);
						}
					}
				}
				if (ok) {
					Complete_meta_state = new bdd[NbSubnets];
					fire = new Set[NbSubnets];
					Meta_State = new Modular_Class_Of_State;
					for (int j = 0; j < NbSubnets; j++) {
						if (ConcernedSubnets.find(j)
								== ConcernedSubnets.end()) {
							Complete_meta_state[j] = e.first.second[j];
							Meta_State->State.push_back(
									e.first.first->State[j]);
						} else {
							Complete_meta_state[j] =
									Subnets[j]->Accessible_epsilon(
											Subnets[j]->get_successor(
													e.first.second[j],
													Subnets[j]->transitionName[tmp]));
							//Point de sortie
							//Meta_State->State.push_back(Subnets[j]->FrontiereNodes(Complete_meta_state[j]));
							//Meta_State->State.push_back(Subnets[j]->CanonizeR(Subnets[j]->FrontiereNodes(Complete_meta_state[j]),0));
							Meta_State->State.push_back(
									Subnets[j]->CanonizeR(
											Complete_meta_state[j], 0));
							-------------------- STAT ----------------------
							TabMeta[nbmetastate] = Meta_State->State[k];
							nbmetastate++;
						}
						fire[j] = Subnets[j]->firable_obs(
								Complete_meta_state[j]);
					}
					Modular_Class_Of_State * pos = Gv.find(Meta_State);
					if (!pos) {
						old_size = bdd_anodecount(TabMeta, nbmetastate);
						//Calcul de deadlock et loop attributes
						Meta_State->blocage = true;
						for (int j = 0;
								((j < NbSubnets) && (Meta_State->blocage)); j++)
							Meta_State->blocage &= Subnets[j]->Set_Bloc(
									Complete_meta_state[j]);
						Meta_State->boucle = false;
						for (int j = 0;
								((j < NbSubnets) && (!Meta_State->boucle)); j++)
							Meta_State->boucle |= Subnets[j]->Set_Div(
									Complete_meta_state[j]);
						st.push(
								StackElt(
										Couple(Meta_State, Complete_meta_state),
										fire));
						e.first.first->Successors.insert(
								e.first.first->Successors.begin(),
								Modular_Edge(Meta_State, tmp));
						Meta_State->Predecessors.insert(
								Meta_State->Predecessors.begin(),
								Modular_Edge(e.first.first, tmp));
						Gv.addArc();
						Gv.insert(Meta_State);
					} else {
						e.first.first->Successors.insert(
								e.first.first->Successors.begin(),
								Modular_Edge(pos, tmp));
						pos->Predecessors.insert(pos->Predecessors.begin(),
								Modular_Edge(e.first.first, tmp));
						Gv.addArc();
						delete Meta_State;
						//Neoud d�ja rencontr� ;
					}
				}
			}
		}
	} while (!st.empty());
	cout << " MAXIMAL INTERMEDIARY BDD SIZE \n" << MaxIntBdd << endl;
	cout << "OLD SIZE : " << bdd_anodecount(TabMeta, nbmetastate) << endl;
	cout << "NB SHARED NODES : " << bdd_anodecount(TabMeta, nbmetastate)
			<< endl;
	cout << "NB META STATE DANS CONSTRUCTION : " << nbmetastate << endl;
	cout << "NB ITERATIONS CONSTRUCTION : " << NbIt << endl;
	cout << "NB ITERATIONS EXTERNES : " << itext << endl;
	cout << "NB ITERATIONS INTERNES : " << itint << endl;
}*/
/*------------------------EmersonLey ----------------------------*/
bdd RdPBDD::EmersonLey(bdd S, bool trace) {
	cout << "ICI EMERSON LEY \n";
	double TpsInit, TpsDetect;
	double debitext, finitext;
	TpsInit = (double) (clock()) / CLOCKS_PER_SEC;
	bdd b = S;
	bdd Fair = bdd_ithvar(2 * Nb_places - 1);
	cout << "PLACE TEMOIN \n";
	//cout<<places[places.size()-1].name<<endl;
	bdd oldb;
	bdd oldd, d;
	int extit = 0;
	int init = 0;
	do {

		extit++;
		if (trace) {

			cout << "ITERATION EXTERNES NUMERO :" << extit << endl;
			debitext = (double) (clock()) / CLOCKS_PER_SEC;
			cout << "TAILLE DE B AVANT IT INTERNE : " << bdd_nodecount(b)
					<< endl;
			cout << endl << endl;
		}
		oldb = b;
		//cout<<"Fair : "<<Fair.id()<<endl;
		d = b & Fair;
		//cout<<"d : "<<d.id()<<endl;
		//init=0;
		do {
			init++;
			if (trace) {

				cout << "ITERATION INTERNES NUMERO :" << init << endl;
				cout << "HEURE : " << (double) (clock()) / CLOCKS_PER_SEC
						<< endl;
				cout << "TAILLE DE D : " << bdd_nodecount(d) << endl;
			}
			oldd = d;
			bdd inter = b & StepForward2(d);
			//cout<<"Tille de inter :"<<bdd_nodecount(inter)<<endl;
			d = d | inter;
		} while (!(oldd == d));
		if (trace)
			cout << "\nTAILLE DE D APRES ITs INTERNES : " << bdd_nodecount(d)
					<< endl;
		b = b & StepBackward2(d);
		init++;
		if (trace) {
			cout << "\n\nTAILLE DE B APRES ELEMINER LES PRED DE D : "
					<< bdd_nodecount(b) << endl;
			finitext = (double) (clock()) / CLOCKS_PER_SEC;
			cout << "DUREE DE L'ITERATION EXTERNE NUMERO " << extit << "  :  "
					<< finitext - debitext << endl;
			cout << endl
					<< "_________________________________________________\n\n";
		}
	} while (!(b == oldb));
	cout << "NOMBRE D'ITERATIONS EXTERNES -----:" << extit << endl;
	cout << "NOMBRE D'ITERATIONS INTERNES -----:" << init << endl;
	TpsDetect = ((double) (clock()) / CLOCKS_PER_SEC) - TpsInit;
	cout << "DETECTION DE CYCLE TIME  " << TpsDetect << endl;
	return b;
}
//-----------------------quantified opacity------------------
bdd RdPBDD::Accessible_epsilon_quantified(bdd From) {
	bdd M1;
	bdd M2 = From;
	int it = 0;
	do {
		// cout<<"M2: "<<M2<<endl;
		M1 = M2; // pour verifier si on travaille sur le meme noeud

		for (Set::const_iterator i = NonObservable.begin();
				!(i == NonObservable.end()); i++) {
			//cout<<"value on non observable i: "<< *i<<endl;

			// cout<<"i: "<<*i<<endl;
			bdd succ = relation[(*i)](M2); //find successors of M2 using nonobservable event i
			// cout<<"succ1: "<<succ<<endl;
			M2 = succ | M2; //why the bitwise or??//this is where the value of M2 changes towards it successor
			//test if secret? sec:=true;
			// cout<<"succ2: "<<succ<<endl;
		}	  //creates bdd of node and successors (with a bitwise or)
		TabMeta[nbmetastate] = M2;
		int intsize = bdd_anodecount(TabMeta, nbmetastate + 1);	//how many shared nodes are in bdd tabmeta
		if (MaxIntBdd < intsize) //maximum number of nodes??
			MaxIntBdd = intsize;
		it++;
		//	cout << bdd_nodecount(M2) << endl;
	} while (M1 != M2);
	//cout << endl;
	return M2;
}
