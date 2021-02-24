#include <utility>
#include <functional>
#include "OS_Supervisor.h"

OS_Supervisor::OS_Supervisor()
{
    this->name="-undefinedSupervisorName-";
}
OS_Supervisor::OS_Supervisor(string name, set <int> controllable, set <int> observable){
    this->name=name;
    this->controllable=controllable;
    this->observable=observable;

   // this->control= Empty vector
}
set <int> OS_Supervisor:: getControllable() const{
        return this->controllable;
}
string OS_Supervisor::getName() const{
    return this->name;
}
set <int> OS_Supervisor::getObservable() const{
    return this->observable;
}
std::map<string, set<int>>  OS_Supervisor::getControl() const{
    return this->control;
}
void OS_Supervisor::setName(string name){
    this->name=name;
}
void OS_Supervisor::setControllable ( set <int> controllable ){
    this->controllable=controllable;
}
void OS_Supervisor::setObservable ( set <int> observable ){
    this->observable=observable;
}
void OS_Supervisor::setControl (std::map<string, set<int>>  control){
    this->control=control;
}


void OS_Supervisor::addControlAction(string trace, int transToDesactivate){

    if (this->control.find(trace) != this->control.end())
    {
        this->control[trace].insert(transToDesactivate);
        cout<<"trace found and added "<<endl;
        return;
    }

    /*
    if (this->control.size()==1 && trace==tr)
    {
        //it is the first trace
        trace = "epsilon";
    }*/
//    cout<<"add control action "<<endl;
    set<int> first;
    first.insert(transToDesactivate);
    this->control.insert( pair<string,set<int>>(trace,first) );
  //  cout<<"inserted for the first time in the control map "<<endl;
  //  cout<<"control size: "<<control.size()<<endl;


}

void OS_Supervisor::printControl(){
    cout<<"\t======================================================"<<endl;
    cout<<"\t====output of the algorithm : desactivate function===="<<endl;
    cout<<"\t======================================================"<<endl;
    if (this->control.size()==0)
    {
        cout<<"No action to desactivate "<<endl;
        return;
    }

    std::map<std::string, set<int>>::iterator it = this->control.begin();
    while (it != this->control.end())
    {

        string trace = it->first;
        set<int> transToDesactivate = it->second;

        if (trace!="")
            cout<<"After the trace : " <<trace ;
        else
            cout<<"No previous trace : ";

        cout <<" Desactivate the transition :: " ;


       for (set<int>::iterator itInt = transToDesactivate.begin(); itInt != transToDesactivate.end(); ++itInt)
        cout << " t" << *itInt;


        cout<<endl;

        it++;
    }
    cout<<"======================================================"<<endl;
}

ostream& operator<<(ostream& flux, const OS_Supervisor& supervisor)
 {
        flux<<"***Supervisor*** \n"<< "Supervisor's name : " <<supervisor.name <<"\nSupervisor's observation : ";
        set<int>::iterator ptr;
        vector<pair< Class_Of_State*,  vector< int>>>::iterator ptr1;
        for (ptr = supervisor.observable.begin(); ptr != supervisor.observable.end(); ptr++)
        {
            flux<<"t"<<*ptr<<",\t";
        }
        flux<<"\nSupervisor's controllable events : ";
        for (ptr = supervisor.controllable.begin(); ptr != supervisor.controllable.end(); ptr++)
        {
            flux<<"t"<<*ptr<<",\t";
        }
        if(supervisor.control.empty())
            flux<<"\nControl : No control action yet"<<endl;
        else
        {
            flux<<"\nControl : Started control"<<endl;
        }

        return flux;
}

OS_Supervisor::~OS_Supervisor()
{
    //dtor
}
