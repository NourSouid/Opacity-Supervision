#include "OS_Supervision_Manager.h"
#include <vector>

OS_Supervision_Manager::OS_Supervision_Manager()
{
    //default ctor
}

OS_Supervision_Manager::OS_Supervision_Manager( OS_Supervisor supervisor, string name){
    this->managedSupervisors.push_back(supervisor);
    this->name=name;
}
//parameterized constructor using one supervisor
OS_Supervision_Manager::OS_Supervision_Manager( OS_Supervision_Manager& supervisionManager){
    this->managedSupervisors=supervisionManager.managedSupervisors;
} //copy constructor

vector<OS_Supervisor> OS_Supervision_Manager::getManagedSupervisors(){
        return this->managedSupervisors;
}

/*
*
* desactivate the transition transToDesactivate for the supervisor
* after the trace trace
* save this in control vector of the supervisor
*
*/
void OS_Supervision_Manager:: desactivate(OS_Supervisor* supervisor, int transToDesactivate, string trace){
    //cout<<"first line of desactivate function"<<endl;
    //cout<<"after the trace "<<trace<<" desactivate t"<<transToDesactivate<<endl;
    /*
    if (trace=="")
        cout<<"void trace"<<endl;
        */
    supervisor->addControlAction(trace, transToDesactivate);
    //cout<<"control size of managed supervisor " <<supervisor->control.size()<<endl;
}
void OS_Supervision_Manager:: desactivate_Version2(OS_Supervisor supervisor, int transToDesactivate, Class_Of_State*  aggregate){
    cout<<"first line of desactivate function"<<endl;
    //desactivate the transition transToDesactivate for the supervisor
    // from aggregate
    //save this in control vector of the supervisor
//    supervisor.addControlAction(aggregate, transToDesactivate);
}
string OS_Supervision_Manager::getName(){
    return this->name;
}
void OS_Supervision_Manager::setName(string name){
    this->name=name;
}
ostream& operator<<(ostream& flux, const OS_Supervision_Manager& manager)
 {
        flux<<"***Supervision Manager***\n"<< "\nSupervision manager's name : " <<manager.name <<"\nManaged supervisors  : ";

        for (int i =0; i!= manager.managedSupervisors.size(); i++)
        {
            flux<<manager.managedSupervisors[i] <<"\t";
        }
        return flux;
}
OS_Supervision_Manager::~OS_Supervision_Manager()
{
    //dtor
}
