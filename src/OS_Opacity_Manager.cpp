#include "OS_Opacity_Manager.h"
#include <algorithm>
//constructors
OS_Opacity_Manager::OS_Opacity_Manager(){
	//default constructor
}
OS_Opacity_Manager::OS_Opacity_Manager( Model* model, vector<OS_Attacker> attackers, OS_Supervision_Manager supervisoryManager, string name){
    this->name = name;
	this->model = model;
	this->attackers = attackers;
	this->supervisoryManager=supervisoryManager;}
OS_Opacity_Manager::OS_Opacity_Manager(const OS_Opacity_Manager& manager){
	//copy constructor
	this->model = manager.model;
	this->attackers = manager.attackers;
	this->supervisoryManager=manager.supervisoryManager;


}
//destructor
OS_Opacity_Manager::~OS_Opacity_Manager(){
	//destructor
}

//getters
Model* OS_Opacity_Manager::getManagedModel()const {
	return this->model;
}
string OS_Opacity_Manager::getName(){
    return this->name;
}
OS_Supervision_Manager OS_Opacity_Manager::getSupervisoryManager(){
	return this->supervisoryManager;
}

//setters
void OS_Opacity_Manager::setName(string name){
    this->name=name;
}
void OS_Opacity_Manager::setManagedModel(Model* model) {
	this->model = model;
}
void OS_Opacity_Manager::setSupervisoryManager(OS_Supervision_Manager supervisoryManager){
	this->supervisoryManager=supervisoryManager;
}

/*Flow operators overload*/
ostream& operator<<(ostream& flux, const OS_Opacity_Manager& manager)
 {
        flux<<"\n***Opacity Manager***"<< "\nOpacity Manager's name : " <<manager.name <<"\nManaged attackers : ";
        for (int i =0; i!= manager.attackers.size(); i++)
        {
            flux<<manager.attackers[i]<<"\t";
        }
        flux<<"\nThe supervision manager : "<<manager.supervisoryManager;
        return flux;
}

//other methods


/*
*
* This is a method whose purpose is to deactivate -if possible- the controllable transitions that lead to the opacity violation
*
*  Attacker and supervisor have the same vision in this version of algorithm
*  No need to define observability however controllability matters
*
*  All controllable events are observable
*  we verify that all controllable events are observable
*  should be included in compute_canonical_deterministic_graph
*
*  Reinforce the opacity of the secret predicate, represented as a bdd S
*  according to an attacker who has the  same vision as supervisor
*  one attacker and one supervisor
*  we compute the symbolic observation graph and verify opacity
*  whenever it is leaked, we call this method to reinforce it
*
*
* @param  MDGraph& sog : the symbolic observation graph (SOG)
*         stack<Pair> st : stack used in computation of the SOG
*         bdd src : set of states inside the same aggregate a
*                   that have imaged in aggregate a_ by the transition t
*         bdd dest : set of states in the same aggregate a_ that are images to state in src
*         int t : transition represented as set<int>
*         Class_Of_State* a_ : Pre-image of the aggregate a_ by the transition t
*         bdd S : secret set of states represented in a bdd structure
*
* @return bool : success or failure of the reinforcement of opacity
*/

bool OS_Opacity_Manager::opacifyByControl(MDGraph& sog, stack<Pair> st, bdd src, bdd dest, int t, Class_Of_State* a_, bdd S, string trace) {
	Class_Of_State* a;//pred de a_
    bdd Complete_meta_state;
    Set fire;
	bdd test = a_->class_state & S;
	//cout<<"trace at the first"<<trace<<endl;
//	cout<<"first line of opacify By control "<<endl;
//	cout<<"transition : "<<t<<endl;
//	cout<<"st.size() "<<st.size()<<endl;
//	cout<<"src : "<<src<<" dest : "<<dest<<endl;
	if (a_->class_state == test) {
	    //all elements of a_ are included in S
       // cout<<"totally included in secret set" <<endl;
        //cout<<"call for cut old branches"<<endl;
//        cout<<"a_->Successors.size() : "<<a_->Successors.size()<<endl;

		cutOldBranches(a_, sog);
		//test if the aggregate is already inserted in the sog
		//if called for the first aggregate violating opacity,
		//we do not need to delete it nor its transition
		//cout<<"sog.currentstate "<<sog.currentstate->class_state <<endl;
		//cout<<"a_"<<a_->class_state<<endl;
		if (sog.currentstate != a_ || sog.currentstate->class_state==bddfalse ) {
			//cout << "delete aggregate and its arc" << endl;
			sog.deleteAggregate(a_);
			sog.deleteArc();
		}

		//st.pop();
		//cout<<"nbre of aggregates : "<<sog.nbAggregates<<endl;
		//cout<<"nbre of arcs : "<<sog.nbArcs <<endl;
	}//ok
	else{
            cout<<"call for slim old branches"<<endl;
            slimOldBranches(a_, sog);

	}//end this
        //attacker and supervisor have the same vision in this version of algorithm
//        cout<<"st.size() "<<st.size()<<endl;
        OS_Supervision_Manager manager = this->supervisoryManager;
        vector<OS_Supervisor> supervisors = manager.getManagedSupervisors();//getManagedSupervisors; in our case we have an only one supervisor
        OS_Supervisor supervisor = supervisors[0];//supervisor
        set <int> controlled = supervisor.getControllable(); //set of controllable events
        /*
        cout<<"control size: "<<controlled.size()<<endl;
        set<int>::iterator ptr;
        for (ptr = controlled.begin(); ptr != controlled.end(); ptr++)
        {
            cout<<"t"<<*ptr<<",\t";
        }
        cout<<supervisor<<endl;
        */
        if (controlled.find(t) != controlled.end()) { //t is controllable by the supervisor
            //cout<<"t"<<t<<" is controllable "<<endl;
            //cout<<"trace here "<<trace<<endl;

            manager.desactivate(&supervisor, t, trace);

            // cout<<"end desactivate and return true "<<endl;
            //cout<<"size of control map in opacify : "<<supervisor.control.size() <<endl;
            supervisor.printControl();
             //cout<<"end printControl and return true "<<endl;
             //cout<<"a_ : "<<a_<<endl;
             //cout<<"sog "<<endl;
             //sog.printCompleteInformation();
            return true;//true
        }
        else if (st.size() > 1)
        {
            cout<<"!!!!!!  Backtracking !!!!!!"<<endl;
           // cout<<"********st.size() " <<st.size() <<endl;


            a = st.top().first.first;//(a2, A)
            st.pop();
            //Get the transition between two aggregates

           // cout<<" a "<<a->class_state<<" pred size of a : "<<a->Predecessors.size()<<" pred of a : "<<a->Predecessors.front().first->class_state<<endl;
            //cout<<"st.top().first.first "<<st.top().first.first->class_state<<" succ size of st.top : "<< st.top().first.first->Successors.size() <<endl;
            t = sog.getTrans( st.top().first.first, a);
           // cout<<"transition returned : "<<t<<endl;
            if (t==-1){
               // cout<<"error transition"<<endl;
                supervisor.printControl();
                return false;
            }

            //Delete the last trace
           //if trace changed to stack, change this
           //The trace format is: "-->ti" ==>length: 4 characters + transition's id
           //i : the transition id: we convert it to a string and we get the length of it
         //   trace.erase (  trace.size() -  (4+  to_string(t).size() ),8);


            //a->saturatePreImg(src) : inside the same aggregate
            //predecessor of bdd: returns bdd
            //we use the intersection between bdd
            //(&: intersection between bdd)
            //to get states in the same aggregate a
            dest = (this->model->StepBackward(src)) & (a->class_state);
            //cout<<"dest "<<dest<<endl;

            //a′←a\(dest∪src);
            bdd k = (dest|src);
            //cout<<"src "<<src<<endl;
            //cout<<"k "<<k<<endl;
            //cout<<"a->class_state "<<a->class_state<<endl;
            //cout<<"(a->class_state)- k "<<(a->class_state)- k<<endl;
            //cout<<"(a->class_state)- dest "<<(a->class_state)- dest<<endl;

            a_->class_state = (a->class_state)- k;//F : Empty --> why??
            //cout<<"a_ "<<a_->class_state<<endl;

            //src←PreImg (dest, t)∩st.Top()
            src = this->model->GetPrerel(dest, t);
            //cout<<"src before intersection "<<src<<endl;
            //src&= st.top().first.first->class_state ;
            //cout<<"src after intersection "<<src<<endl;//why not..??

            //sog.currentstate->class_state = bddtrue;

            //cout<<"->class_state"<<a_->class_state<<endl;
            //cout<<"The trace is "<<trace<<endl;
            if (opacifyByControl(sog, st, src, dest, t, a_, S, trace) ) {

                Complete_meta_state = this->model->Accessible_epsilon(a_->class_state);//the set "EnabledObs" in algorithm

                fire = this->model->firable_obs(Complete_meta_state);//enabled transitions from the meta state

                st.push(Pair(couple(a_, Complete_meta_state), fire));

                supervisor.printControl();


                return true;
            }
            else {
                cout<<"returns false 1"<<endl;
                supervisor.printControl();
                return false;
            }
        }
        else {
            cout<<"returns false 2"<<endl;
            supervisor.printControl();
            return false;
	}
}


bool OS_Opacity_Manager::opacifyByControl_Version2(MDGraph& sog, stack<Pair> st, bdd src, bdd dest, int t, Class_Of_State* a_, bdd S) {
/*
****************** Optimal version **********************
*/
Class_Of_State* a;//pred de a_
    bdd Complete_meta_state;
    Set fire;
	bdd test = a_->class_state & S;
	//cout<<"first line of opacify By control "<<endl;
	//cout<<"transition : "<<t<<endl;
	//cout<<"st.size() "<<st.size()<<endl;
	//cout<<"src : "<<src<<" dest : "<<dest<<endl;
	if (a_->class_state == test) {
	    //all elements of a_ are included in S
        //cout<<"totally included in secret set" <<endl;
        //cout<<"call for cut old branches"<<endl;
        //cout<<"a_->Successors.size() : "<<a_->Successors.size()<<endl;

		cutOldBranches(a_, sog);
		//test if the aggregate is already inserted in the sog
		//if called for the first aggregate violating opacity,
		//we do not need to delete it nor its transition
		//cout<<"sog.currentstate "<<sog.currentstate->class_state <<endl;
		//cout<<"a_"<<a_->class_state<<endl;
		if (sog.currentstate != a_ || sog.currentstate->class_state==bddfalse ) {
			//cout << "delete aggregate and its arc" << endl;
			sog.deleteAggregate(a_);
			sog.deleteArc();
		}

		//st.pop();
		//cout<<"nbre of aggregates : "<<sog.nbAggregates<<endl;
		//cout<<"nbre of arcs : "<<sog.nbArcs <<endl;
	}//ok
	else{
            cout<<"call for slim old branches"<<endl;
            slimOldBranches(a_, sog);

	}//end this
        //attacker and supervisor have the same vision in this version of algorithm
        //cout<<"st.size() "<<st.size()<<endl;
        OS_Supervision_Manager manager = this->supervisoryManager;
        vector<OS_Supervisor> supervisors = manager.getManagedSupervisors();//getManagedSupervisors; in our case we have an only one supervisor
        OS_Supervisor supervisor = supervisors[0];//supervisor
        set <int> control = supervisor.getControllable(); //set of controllable events

        if (control.find(t) != control.end()) { //t is controllable by the supervisor
            //cout<<"t"<<t<<" is controllable "<<endl;
            manager.desactivate_Version2(supervisor, t, a);
            //triplet : ((traces observées de l'attaquant, état courant du système),int)
            //cout<<"end desactivate and return true "<<endl;
            return true;//true
        }
        else if (st.size() > 1)
        {
            //cout<<"!!!!!!  Backtracking !!!!!!"<<endl;
           // cout<<"********st.size() " <<st.size() <<endl;

            a = st.top().first.first;//(a2, A)
            st.pop();
            //Get the transition between two aggregates

           // cout<<" a "<<a->class_state<<" pred size of a : "<<a->Predecessors.size()<<" pred of a : "<<a->Predecessors.front().first->class_state<<endl;
           //// cout<<"st.top().first.first "<<st.top().first.first->class_state<<" succ size of st.top : "<< st.top().first.first->Successors.size() <<endl;
            t = sog.getTrans( st.top().first.first, a);//cout<<"transition returned : "<<t<<endl;
            if (t==-1){
                //cout<<"error transition"<<endl;
                return false;
            }
            //a->saturatePreImg(src) : inside the same aggregate
            //predecessor of bdd: returns bdd
            //we use the intersection between bdd
            //to get states in the same aggregate a
            dest = (this->model->StepBackward(src)) & (a->class_state);
            cout<<"dest "<<dest<<endl;

            //a′←a\(dest∪src);
            bdd k = (dest|src);
            cout<<"src "<<src<<endl;
            cout<<"k "<<k<<endl;
            cout<<"a->class_state "<<a->class_state<<endl;
            a_->class_state = (a->class_state)- k;
            cout<<"a_ "<<a_->class_state<<endl;

            //src←PreImg (dest, t)∩st.Top()
            src = this->model->GetPrerel(dest, t);
            //cout<<"src before in tersection "<<src<<endl;
            //src&= st.top().first.first->class_state ;
            //cout<<"src after intersection "<<src<<endl;//why not..??

            //sog.currentstate->class_state = bddtrue;
            if (opacifyByControl_Version2(sog, st, src, dest, t, a_, S)) {

                Complete_meta_state = this->model->Accessible_epsilon(a_->class_state);//the set "EnabledObs" in algorithm


                fire = this->model->firable_obs(Complete_meta_state);//enabled transitions from the meta state
                st.push(Pair(couple(a_, Complete_meta_state), fire));
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
	}
}


/*
*
*
* When the aggregate a’ is totally included in the secret state
* we have to cut all the branches already built from a’
* then it will be deleted
*
* It is a recursive function.
* we stop exploring a branch when we find a successor to a’ which is accessible elsewhere.
* That is to say, even if we delete (totally or partially) the aggregate a’, its successor will persist as it is
* @param  Class_Of_State* a : the aggregate to be cutted
*		  MDGraph& g : the symbolic observation graph (sog) g
*					   we use a reference to g
*
*
* @return void : modification is done on the aggregate and the graph
*/
void OS_Opacity_Manager::cutOldBranches(Class_Of_State* a, MDGraph& g) {
	//Later on, testing if the aggregate is an initial state aggregate or not
	//No information in Class_Of_State indicating if the aggregate is an initial state or not
	//can only use information in the MDGraph that's why we add it to parameters
	//we use an adress of MDGraph since we are deleting from this graph, so it may change

	//cout<<"first line of cut old branches "<<endl;
	Class_Of_State* a_;
	int t;
	vector< std::pair< Class_Of_State*, int > >  v = a->Successors;
	vector< std::pair< Class_Of_State*, int > >  v_;

//	cout<<"before the for loop"<<endl;
	//cout<<v.size()<<endl;
	if (v.size() == 0)
	{
		//cout << "No old branches to be cut" << endl;
		return;
	}
	for (vector< std::pair< Class_Of_State*, int >> ::iterator it = v.begin(); it != v.end(); ++it) {
		//cout<<"inside the for loop "<<endl;
		a_ = it->first;
		v_ = a_->Predecessors;
		t = it->second;
		std::pair< Class_Of_State*, int > elt = std::make_pair(a_, t);
		std::pair< Class_Of_State*, int > elt_ = std::make_pair(a, t);
		//Testing if a_ is accessible elsewhere or initial state
		if ((a_->Predecessors.size() > 1) || (g.initialstate == a_))
		{
		    //cout<<"if a is accessible elsewhere or initial state"<<endl;
			//delete the edge (a, t, a_)
			//cout<<"before deleting the arc"<<g.nbArcs<<endl;
			g.deleteArc();
            //cout<<"after deleting the arc"<<g.nbArcs<<endl;

			v.erase(std::remove(v.begin(), v.end(), elt), v.end());
			v_.erase(std::remove(v.begin(), v.end(), elt_), v.end());
		}
		else {
			cutOldBranches(a_, g);
			if (a_)
				{
				   // cout<<"nb aggregates before the delete"<<g.nbAggregates<<endl;
				    delete(a_);
				    g.nbAggregates--;
				    //cout<<"nb aggregates after the delete"<<g.nbAggregates<<endl;

				}
		}
	}
	//cout<<"end of function "<<endl;

}


/*
* In the case where a' is not completely included in the secret, that is to say it has been reduced (slimmed),
* it is then necessary to go through the branches already constructed from a' and “cut” these aggregates,
* ie d.Removing some states that would no longer be accessible
* @param : Class_Of_State* a : the aggregate to be reduced
*		   MDGraph& g : the sog
* @return : void : since the modification is made on the graph and the aggregate
*/
void OS_Opacity_Manager::slimOldBranches(Class_Of_State* a, MDGraph& g) {
	Class_Of_State* a_;//a'
	bdd bdda__;//a"
	//using bdd format in order to make comparison easier
	//convert to Class_Of_State only if needed
	int t;
	vector< std::pair< Class_Of_State*, int >>  v = a->Successors;
	vector< std::pair< Class_Of_State*, int >>  v_, v__;

	for (vector< std::pair< Class_Of_State*, int >>::iterator it = v.begin(); it != v.end(); ++it) {
		a_ = it->first;//successor of a
		t = it->second;//by transition t
		v_ = a_->Predecessors;

		std::pair< Class_Of_State*, int > elt = std::make_pair(a_, t);
		std::pair< Class_Of_State*, int > elt_ = std::make_pair(a, t);
		bdda__ = this->model->Accessible_epsilon(a->class_state);
		Class_Of_State* a__ = new Class_Of_State;;
		v__ = a__->Predecessors;
		std::pair< Class_Of_State*, int > newElt = std::make_pair(a__, t);

		//a"<-- Saturate(a,t)
		//then compare it with the successor of a by t
		//this step's purpose is to determine if there is a difference between
		//the initial image of a, which is a', and its image after removing some states
		//remember that this step(removing some states from aggregate a) was done in the algorithm OpacifyByControl
		if (bdda__ != a_->class_state) {//a'!=a"
			//the image of a has changed
			//which means that we have deleted some states from a
			//a have been slimmed
			//so should be its image
			//a' is no more the image of a
			//However, we need to check if the new image is violating opacity
			//in this case, we cut the total branch
			bdd test = bdda__ & (this->model->SecretBdd);
			if (bdda__ == test) {//a" in S
				cutOldBranches(a_, g);
				delete(a_);
			}
			else {
				if (v_.size() > 1) {//a’.predecessors().size() > 1
					//Delete the old edge(a, t, a_)
					v.erase(std::remove(v.begin(), v.end(), elt), v.end());
					v_.erase(std::remove(v.begin(), v.end(), elt_), v.end());
					//save the new one(a, t, a__)
					v.insert(v.begin(), newElt);
					v__.insert(v__.begin(), elt_);

					//nb arcs should be updated??
					//should insert this aggregate in g??

				}
				else {
					a__->blocage = this->model->Set_Bloc(bdda__);      // detect if blockage
					a__->boucle = this->model->Set_Div(bdda__); //detect if sequence is divergente (has an end)
					a__->class_state = this->model->CanonizeR(bdda__, 0);
					a_ = a__;
					slimOldBranches(a_, g);

				}

			}

		}


	}

}

/*
*
* this method is inspired by the method compute_canonical_deterministic_graph_opt of the interface SOG_Generator
*
* The idea of ​​this algorithm is to build the SOG of a given system.
* After the construction of each aggregate, we check whether the constructed aggregate is included in the secret set S.
* If this test is positive, the supervisor must intervene in order to reinforce the opacity.
* Hence the call to the OpacifyByControl function.
* If the call to this function returns "True" then we continue building the SOG.
* Otherwise, the SOG-Opacification function returns "False" indicating failure of the algorithm.
*
*
* @param : MDGraph& g : sog to be constructed and used to make the predicate opaque
* @return: void : modification is done on the graph itself
*
*/
void OS_Opacity_Manager::computeAndOpacify_SOG(MDGraph& g) {

	cout << "_________________________COMPUTE AND OPACIFY_________________________" << endl;

	bdd* TabMeta = new bdd[1000];
	int nbmetastate = 0;
	int MaxIntBdd = 0;

	std::string tr = "";

	pile st, st_;
	int NbIt = 0;
	int nbSucceEXstate = 0;
	int CounterStates = 0;
	int nbstate = 0;
	bool Cross_Secret = false, System_tested = true;
	int itext, itint;
	itext = itint = 0;
	Class_Of_State* reached_class;
	Set fire;
	Class_Of_State* c = new Class_Of_State;

	//Saturate(M0): initial state
	//get the corresponding aggregate
	bdd Complete_meta_state = this->model->Accessible_epsilon(this->model->M0);

	// Testing simple opacity for initial aggregate:
	bdd test = Complete_meta_state & this->model->SecretBdd;
//	cout<<"Test of the first aggregate : "<<test<<endl;

	//initial aggregate is violating opacity
	//do not continue execution
	//bool variable indicating failure
	if (Complete_meta_state == test) {
		cout << '\n' << "Opacity can not be enforced by control, first aggregate non opaque" << endl;
		System_tested = false;
		return;
	}

	//initial aggregate is not violating opacity
	//the SOG can be constructed
	//we begin by the initial aggregate
	//insert this aggregate and set it as the initial one

	// transitions enabled by this initial state
	fire = this->model->firable_obs(Complete_meta_state);

	//set the c attributes
	//remember c is of type Class_Of_State *
	// detect if it is blocking
	c->blocage = this->model->Set_Bloc(Complete_meta_state);
	//detect if sequence is divergent (has an end)
	c->boucle = this->model->Set_Div(Complete_meta_state);
	//checks if more than 2 internal loops?
	c->class_state = this->model->CanonizeR(Complete_meta_state, 0);

	//this first aggregate "c" is ready
	//add it to TabMeta (which is a tab)
	//increment the number of meta states "nbmetastate"
	//update the size: bdd_count
	TabMeta[nbmetastate] = c->class_state;
	nbmetastate++;
	double old_size = bdd_nodecount(c->class_state);//not used

	//push it to the stack
	//elements of type ((Class_Of_State*: the aggregate, bdd: corresponding bdd), set<int>: transitions enabled)
	st.push(Pair(couple(c, Complete_meta_state), fire));
//	st_.push(Pair(couple(c, Complete_meta_state), fire));
	//cout<<"just inserted the first elt in stack :"<< /*reached_class->class_state <<*/" st.size() "<<st.size()<<endl;

	g.setInitialState(c);
	g.insert(c);//V U {a}
	//cout<<"initial state inserted "<<c->class_state<<endl;

	//update the attribute (nbMarking) of the SOG
	g.nbMarking += bdd_pathcount(c->class_state);

	std::set<int>::iterator it;
	/*for (it = fire.begin(); it != fire.end(); ++it) {
                           cout<<"fire transition enabled "<< *it<<endl;
                        }//for testing
    */

	//construct the rest of the SOG
	//this is to be done until the stack st becomes empty
	do {
        //cout<<"***construct the rest of the SOG***"<<endl;
		int nbsecesor = 0;
		Pair e = st.top();
        //st.pop(); //only when no enabled event is possible

        if (e.second.size()==1){
               // cout<<"no enabled event is possible"<<endl;
                st.pop();
                }

		nbmetastate--; //another modifer
		while (!e.second.empty())
        //there is an observable event reached by c
        {
            //cout<<"stack not empty : there is an observable event reached by c"<<endl;
            //cout<<" e.second.size() : "<<e.second.size()<<endl;
			int t = *e.second.begin(); //copy observable event
			//cout<<"t : "<<t<<endl;
			//cout<<"(e.first.second): " <<(e.first.second)<<endl;

			e.second.erase(t); //remove observable event from list

			double nbnode;
			reached_class = new Class_Of_State;

			bdd GSuccesor = this->model->get_successor(e.first.second, t); //find succesors observable events
            //cout<<"succ e.first.second(t)"<<GSuccesor<<endl;

			bdd Complete_meta_state = this->model->Accessible_epsilon(GSuccesor); //aggregate succeossors
			//cout<< "Complete_meta_state"<<Complete_meta_state<<endl;

			//reached_class: same as c in the first aggregate
			//why using two variables?
			//could use c ???
			reached_class->class_state = this->model->CanonizeR(Complete_meta_state, 0);
			//cout<<"bdd of the reached class "<<reached_class->class_state<<endl;
			Class_Of_State* pos = g.find(reached_class);//reached_class is treated or not

            //reached_class have been treated before
			if (pos) {
                //cout<<"this aggregate has been treated before "<<endl;
				//observable transitions from meta_state (aggregate)
				//fire = this->model->firable_obs(Complete_meta_state); //find succ with obs events

                delete reached_class;
                e.first.first->Successors.insert(e.first.first->Successors.begin(),Edge(pos,t));
                pos->Predecessors.insert(pos->Predecessors.begin(),Edge(e.first.first,t));
                char a = (char)(((int)'0')+t);
                //cout<<"a : "<<a<<endl;
                //tr =tr +"-->"+"t"+a;
				//cout << "added an arc here  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
				//cout<<"trace "<<tr<<endl;
                g.addArc();

                }//end pos
			else {// (!pos)
                //cout<<"this state hasn't been treated before "<<endl;

				fire = this->model->firable_obs(Complete_meta_state);
				this->model->cross = GSuccesor & this->model->SecretBdd;

				bdd T = bdd_true();
				T = Complete_meta_state & this->model->SecretBdd;	//test simple opacity

				if (T == Complete_meta_state) {
					//cout<<"a included in secret bdd"<<endl;
					//opacity is leaked
					//should be imposed using the theory of control and supervision

					//states in a that are enabling the transition t
					//a.k.a : exit points of a by t
					//should add: a = e.first.second
//					cout<<"bdd of the reached class before src "<<reached_class->class_state<<endl;
					bdd src = this->model->GetPrerel(Complete_meta_state, t) ;
					//cout<<"src in compute :  "<<src<<endl;
					//src &= (e.first.first->class_state);why not ?
					//cout<<"src in compute :  "<<src<<endl;
                    //cout<<"t : "<<t <<endl;
                    //cout<<"Complete_meta_state "<<Complete_meta_state<<endl;
                   // cout<<"this->model->GetPrerel(e.first.second, t): "<<this->model->GetPrerel(e.first.second, t)<<endl;
                    //cout<<"(e.first.second): " <<(e.first.second)<<endl;


					bdd dest = this->model->get_successor(src, t) ;//totally included in secret set
					//cout<<"dest in compute : "<<dest<<endl;
					//dest&= (reached_class->class_state);//why not??
					//cout<<"dest after  in compute : "<<dest<<endl;
					//intersection between bdd is implemented through bdd &
					//cout<<"call for opacify by control "<<endl;
					g.currentstate=reached_class;

                    //cout<<"trace before call for opacify: "<<tr<<endl;

					//cout<<"size :"<<st.size();
					if (opacifyByControl(g, st, src, dest, t, reached_class, this->model->SecretBdd, tr)){
                        cout<<"Opacify returns True : opacification success"<<endl;
                        //cout<<"trace after call for opacify: "<<tr<<endl;
                        cout<<"Construct the rest of the SOG"<<endl;

					}

					else
                        {
                        cout<<"opacify returns false"<<endl;
						//-------------not needed-----------------//
						string a = IntToString(t);
						tr = tr + "-->" + "t" + a;
						cout << '\n' << "The system's opacity cannot be enforced by control " << endl;
						//-------------end not needed--------------//

						//cout << '\n' << "The system's opacity cannot be enforced by control " << endl;
						//cout<<"nbr of aggregates in call function : "<<g.nbAggregates<<endl;
						//cout<<"nbr of arcs in call function : "<<g.nbArcs<<endl ;
						System_tested = false;
						//cout<<"nbr of aggregates in call function : "<<g.nbAggregates<<endl;
						//cout<<"nbr of arcs in call function : "<<g.nbArcs<<endl ;
						return;
					}
				}//end in secret S
				else {//not a secret
                       // cout<<"not a secret set"<<endl;
					st.push(Pair(couple(reached_class, Complete_meta_state), fire));
					//st_.push(Pair(couple(reached_class, Complete_meta_state), fire));
					/*for (it = fire.begin(); it != fire.end(); ++it) {
                           cout<<"fire transition enabled "<< *it<<endl;
                        }//for testing
                        */

                    //cout<<"inserted an other element : "<< reached_class->class_state <<"st.size()"<<st.size()<<endl;
					TabMeta[nbmetastate] = reached_class->class_state; //aggregate v union a
					nbmetastate++;

					e.first.first->Successors.insert(e.first.first->Successors.begin(),Edge(reached_class, t));
					reached_class->Predecessors.insert(reached_class->Predecessors.begin(),Edge(e.first.first, t));

					char a = (char)(((int)'0')+t);
					tr =tr +"-->"+"t"+a;
					g.addArc();
					//cout << "added an arc here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
					g.insert(reached_class);
				}//end else
			}//end else (!pos)

		}	//end while empty there is an observable event reached by c

//cout<<"end while"<<endl;
	} while (!st.empty());//end do

}//end fct


