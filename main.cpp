// version 0.1
#include <time.h>
#include <iostream>
#include <string>
#include<stdio.h>
#include <fstream>
using namespace std;
#include "bdd.h"
#include "fdd.h"
#include "Net.h"
#include "RdPBDD.h"
#include "math.h"
/*includes for supervision and control theory*/
#include "./include/OS_Attacker.h"
#include "OS_Supervisor.h"
#include "OS_Supervision_Manager.h"
#include "OS_Opacity_Manager.h"


double getTime() {
	return (double) clock() / (double) CLOCKS_PER_SEC;
}
int Menu() {
	int choix;
	cout << "\t\t______________________________________________" << endl;
	cout << "\t\t|                                            |" << endl;
	cout << "\t\t|        OBSERVATION GRAPHE TOOL             |" << endl;
	cout << "\t\t______________________________________________\n\n" << endl;
	cout
			<< "\tConstruction de l'espace d'etats accessible avec OBDDs            : 1\n"
			<< endl;
	cout
			<< "\tConstruction du graphe d'observation avec Canonize Dichotomique   : 2\n"
			<< endl;
	cout
			<< "\tProduit synchronise a la volee de n graphes d'observations        : 3\n"
			<< endl;
	cout
			<< "\tProduit synchronise canonise de n graphes d'observations          : 4\n"
			<< endl;
	cout << "\tQuitter l'outil : 0" << endl;
	cin >> choix;
	return choix;
}

void reordering(int nbvar) {

	// all the variables belong to a main block
	bdd_varblockall();
	// to each fdd variable corresponds a block
	for (int i = 0; i < nbvar; i++)
		fdd_intaddvarblock(i, i, BDD_REORDER_FIXED);

	bdd_autoreorder(BDD_REORDER_SIFT);

	bdd_reorder_verbose(2);
}

//*************************************************
int frequency_of_primes (int n) {
  int i,j;
  int freq=n-1;
  for (i=2; i<=n; ++i) for (j=sqrt(i);j>1;--j) if (i%j==0) {--freq; break;}
  return freq;
}

/***********************************************/
int main() {
   //write in the result file directly
	//not the purpose for now
	//freopen("C:/Users/nours/OneDrive/Desktop/Amina/examples/alpha-beta/QSOG-Results.txt", "w+", stdout);
	cout<<"Nour SOUID Workspace"<<endl;

	cout << "***********************************************************************"<< endl;
	cout << "***********************************************************************"<< endl;
	cout << "      Opacify Supervision using theory of control and supervision      "<< endl;
	cout << "***********************************************************************"<< endl;
	cout << "***********************************************************************"<< endl;

	char expl[100] = "";//example file
	char Obs[100] = "";	//obsevables
	char Int[100] = "";	//secrets
	char Int1[100] = "";//opacity choice

	//alpha.xml obs.txt Secret.txt Choix.txt 1
/*
strcpy(expl, "C:/Users/nours/OneDrive/Desktop/amina/examples/alpha-beta/alpha-beta.xml");
	strcpy(Obs, "C:/Users/nours/OneDrive/Desktop/amina/examples/alpha-beta/Obs.txt");
	strcpy(Int, "C:/Users/nours/OneDrive/Desktop/amina/examples/alpha-beta/Secrets.txt");
	strcpy(Int1, "C:/Users/nours/OneDrive/Desktop/amina/examples/alpha-beta/Choix.txt");
*/
	strcpy(expl, "C:/Users/nours/OneDrive/Desktop/amina/examples/nour/UseCase/alpha-beta.xml");
	strcpy(Obs, "C:/Users/nours/OneDrive/Desktop/amina/examples/nour/UseCase/Obs.txt");
	strcpy(Int, "C:/Users/nours/OneDrive/Desktop/amina/examples/nour/UseCase/Secrets.txt");
	strcpy(Int1, "C:/Users/nours/OneDrive/Desktop/amina/examples/nour/UseCase/Choix.txt");

	int b = 1; //maximal marking

	/***********************************************************/
    //create model
	net R( expl, Obs, Int,Int1);	//parse pnml file and create petri net (nodes)

	RdPBDD DR(R, b, false);
	/*cout<<"***************test********************"<<endl;
	cout<<"var : "<<DR.relation.front().getVar()<<endl;
	cout<<DR.relation.front().getRel()<<endl;
	cout<<DR.relation.back().getVar()<<endl;
	cout<<DR.relation.back().getRel()<<endl;
	//DR.relation.front().setRel(a);
	//DR.relation.back().setRel(bdd_true);
	cout<<DR.relation.front().getRel()<<endl;
	cout<<DR.relation.back().getRel()<<endl;*/

	MDGraph g;
	bdd_gbc_hook(NULL);//??
	//DR.compute_canonical_deterministic_graph_Opt(g); //create SOG
	//g.printCompleteInformation(); //print SOG data



    /*****************************************************************************/
    /*********************Opacification test**************************************/

    /***create attacker***/

    int obs[]= {0, 8, 9, 10, 11, 12, 14, 13, 15}; //same as the supervisor's observation
    set<int> transitions(obs,obs+9);//attacker's obervability
    OS_Attacker attacker ("attackerName", transitions);
    cout<<attacker<<endl;

    /***create supervisor***/

    //controllable events are a subset of observable events
    set<int> controllableSet(obs+7,obs+9), observableSet(obs,obs+9);
    OS_Supervisor supervisor("supervisorName", controllableSet, observableSet);
    cout<<supervisor<<endl;

    /***create supervision manager***/
    //using parametric constructor with only one supervisor
    //we could create a supervision manager and use the method supervise()
    //it creates directly a supervisor and add it to its managed supervisors


    OS_Supervision_Manager supervisionManager(supervisor, "supervisionManagerName");
   // cout<<"supervisionManager: "<<supervisionManager<<endl;

    /***create opacity manager***/

    vector<OS_Attacker> attackers;
    attackers.push_back(attacker);
    OS_Opacity_Manager opacityManager(&DR, attackers, supervisionManager);
    //cout<<"opacityManager: "<<opacityManager<<endl;

    /*****************************************************************************/
    /*****************************************************************************/

    //opacity manager is already created
    //can call its methods


//    cout<<"======Before opacification====== "<<endl;
    DR.compute_canonical_deterministic_graph_Opt(g);//deleting this is causing problem don't understand why
    //g.printCompleteInformation();

    MDGraph G1;
    opacityManager.computeAndOpacify_SOG(G1);
    cout<<"\n\n======After opacification====== "<<endl;
    //cout<<"nb of aggregates in main fct"<<G1.nbAggregates<<endl;
    G1.printCompleteInformation();



	return 0;
}
