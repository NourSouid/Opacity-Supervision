/* -*- C++ -*- */
#ifndef RdPBDD_H
#define RdPBDD_H
#include <vector>
#include <stack>
#include <utility>
#include <stdio.h>
#include <iostream>
#include "Net.h"
#include "bdd.h"
#include"MDGraph.h"
#include"Modular_Obs_Graph.h"
#include "Modular_Class_of_state.h"
#include "include/Model.h"
#include "OS_SOG_Generator.h"
#include "Opacity_Verifier.h"
using namespace std;
//#include"Modular_Obs_Graph.hpp"
/***********************************************/

typedef pair<Class_Of_State*, bdd> couple;
typedef pair<couple, Set> Pair;
typedef stack<Pair> pile;

std::string IntToString ( int number );

class Trans {
public:
    Trans();
	Trans(const bdd& var, bddPair* pair, const bdd& rel,const bdd& prerel,const bdd& Precond, const bdd& Postcond);
    Trans(const Trans&);//added by Nour
    Trans& operator= (Trans &);//added by Nour
	bdd operator()(const bdd& op) const;//Franchissement avant
	bdd operator[](const bdd& op) const;//Franchissement arrière
    friend bool operator<(Trans const&, Trans const& );//added by Nour

    bdd getRel();
    bdd getVar();
    void setRel(bdd);
    void setVar(bdd);
  friend class RdPBDD;

private:
	bdd var;
	bddPair* pair;
	bdd rel;
	bdd prerel;

    bdd Precond;
    bdd Postcond;


};

class RdPBDD : public Model, public OS_SOG_Generator, public Opacity_Verifier {
private :
  vector<class Transition> transitions;
  Set Observable;
  Set Secret[16384] ;
  Set NonObservable;
  map<string,int> transitionName;
  map<string,int> placeName;// j'ai ajouter placename map;
  Set InterfaceTrans;
  Set Formula_Trans;
  unsigned int Nb_places;
public:
  int OpacityChoice;
  int K_Step;
  bdd Secretb;

  bdd T;
  bdd currentvar;
  vector<Trans> relation;
  vector<Place> places ;
  bdd ReachableBDD1();
 // Set Set_Marked(bdd state);
  bdd ReachableBDD2();
  bdd ReachableBDD3();
  bdd VerifiParent(bdd s,int f);


//  Set GetMarked(std::vector<string> s);
  bool FindSbVector(vector<string> a , string c );
  bdd GetPrerel(bdd s , int t);
  string GetFather(bdd d);
/* G�n�ration de graphes d'observation */
void compute_canonical_deterministic_graph_Opt(MDGraph& g) ;
//void computeAndOpacify_SOG(MDGraph& g); // More convenient in OS_Opacity_Manager
bdd Accessible_epsilon(bdd From);
  //quantified opacity Saturate function
  bdd QSaturate(bdd From, bool init);
  bdd Accessible_epsilon_quantified(bdd From);//quantified
  bdd Accessible_epsilon2(bdd From);
  bdd StepForward(bdd From);
 // bdd get_Precedor(bdd From,int t);
  bdd StepBackward3(bdd From , int i);
  bdd VerificationPost(int t);
  int weakOpacity(bdd state,bdd secret , int numStatCros);
  int StrongOpacity(bdd state , bdd secret ,int numStatCros);
  bdd StepForward2(bdd From);
  bdd StepBackward(bdd From);
  //Set Set_Marked(bdd S);
  string ConvertToString (bdd S);
  bdd StepBackward2(bdd From);
  std::vector<string> Tokenizer(string s);
  bdd EmersonLey(bdd S, bool trace);
  Set GetSetMarked(std::vector<string> Vec_Ma) ;
  Set firable_obs(bdd State);
  bdd get_successor(bdd From,int t);
  void GeneralizedSynchProduct1(Modular_Obs_Graph& Gv, int NbSubnets,RdPBDD* Subnets[] ,int nbbddvar) ;
  bool Set_Div(bdd &S) const;
  bool Set_Bloc(bdd &S) const;
  bool SimpleOpacityVerifer(Set A ,Set B , bdd c );
  bdd FrontiereNodes(bdd From) const ;
  bdd CanonizeR(bdd s, unsigned int i) ;
  RdPBDD(const net&,int BOUND=32,bool init=false);
  ~RdPBDD(){};
};
/*____________Structure utiles aux produit synchronis� g�n�ralis� de n graphes d'observations ________*/

/*typedef pair<Modular_Class_Of_State*,bdd*> CoupleNodes; // M�ta-�tat (canonis�) + vecteur des bdds complets
typedef pair<Set*,Set*> CoupleSets;//ens d'ens des trans de la formule franchissables non encore trait�es + ens d'ens des trans de l'interface franchissables non encore trait�es
typedef pair<CoupleNodes,CoupleSets> PairProduct; // Couple + ens des ens des transitions franchissables non encore trait�es
typedef pair<PairProduct,bool> StackElt;
typedef vector<StackElt> Stack;*/
typedef pair<Modular_Class_Of_State*,bdd*> Couple;
typedef pair<Couple,Set*> StackElt;
typedef stack<StackElt> Stack;
typedef vector<StackElt> PileInt;
typedef pair<StackElt,PileInt> CanonicRep;
//typedef stack<CanonicRep> STACK;
typedef stack<PileInt> STACK;
int InStack(PileInt,Modular_Class_Of_State*);
bool MemeVecteur(vector<bdd>,vector<bdd>);
bool Inferieur(vector<bdd>,vector<bdd>);

#endif
